/*
*         OpenPBS (Portable Batch System) v2.3 Software License
* 
* Copyright (c) 1999-2000 Veridian Information Solutions, Inc.
* All rights reserved.
* 
* ---------------------------------------------------------------------------
* For a license to use or redistribute the OpenPBS software under conditions
* other than those described below, or to purchase support for this software,
* please contact Veridian Systems, PBS Products Department ("Licensor") at:
* 
*    www.OpenPBS.org  +1 650 967-4675                  sales@OpenPBS.org
*                        877 902-4PBS (US toll-free)
* ---------------------------------------------------------------------------
* 
* This license covers use of the OpenPBS v2.3 software (the "Software") at
* your site or location, and, for certain users, redistribution of the
* Software to other sites and locations.  Use and redistribution of
* OpenPBS v2.3 in source and binary forms, with or without modification,
* are permitted provided that all of the following conditions are met.
* After December 31, 2001, only conditions 3-6 must be met:
* 
* 1. Commercial and/or non-commercial use of the Software is permitted
*    provided a current software registration is on file at www.OpenPBS.org.
*    If use of this software contributes to a publication, product, or
*    service, proper attribution must be given; see www.OpenPBS.org/credit.html
* 
* 2. Redistribution in any form is only permitted for non-commercial,
*    non-profit purposes.  There can be no charge for the Software or any
*    software incorporating the Software.  Further, there can be no
*    expectation of revenue generated as a consequence of redistributing
*    the Software.
* 
* 3. Any Redistribution of source code must retain the above copyright notice
*    and the acknowledgment contained in paragraph 6, this list of conditions
*    and the disclaimer contained in paragraph 7.
* 
* 4. Any Redistribution in binary form must reproduce the above copyright
*    notice and the acknowledgment contained in paragraph 6, this list of
*    conditions and the disclaimer contained in paragraph 7 in the
*    documentation and/or other materials provided with the distribution.
* 
* 5. Redistributions in any form must be accompanied by information on how to
*    obtain complete source code for the OpenPBS software and any
*    modifications and/or additions to the OpenPBS software.  The source code
*    must either be included in the distribution or be available for no more
*    than the cost of distribution plus a nominal fee, and all modifications
*    and additions to the Software must be freely redistributable by any party
*    (including Licensor) without restriction.
* 
* 6. All advertising materials mentioning features or use of the Software must
*    display the following acknowledgment:
* 
*     "This product includes software developed by NASA Ames Research Center,
*     Lawrence Livermore National Laboratory, and Veridian Information 
*     Solutions, Inc.
*     Visit www.OpenPBS.org for OpenPBS software support,
*     products, and information."
* 
* 7. DISCLAIMER OF WARRANTY
* 
* THIS SOFTWARE IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND. ANY EXPRESS
* OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
* OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, AND NON-INFRINGEMENT
* ARE EXPRESSLY DISCLAIMED.
* 
* IN NO EVENT SHALL VERIDIAN CORPORATION, ITS AFFILIATED COMPANIES, OR THE
* U.S. GOVERNMENT OR ANY OF ITS AGENCIES BE LIABLE FOR ANY DIRECT OR INDIRECT,
* INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
* LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,
* OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
* LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
* NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
* EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
* 
* This license will be governed by the laws of the Commonwealth of Virginia,
* without reference to its choice of law rules.
*/
/* $Id$ */

/*
 * Perform any scheduler initialization.
 */

#include <ctype.h>
#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <sys/utsname.h>

#include "toolkit.h"
#include "gblxvars.h"

#include "net_connect.h"
#include "sched_cmds.h"
#include "log.h"

/*
 * Flag register to prevent SIGHUP from causing a reconfiguration
 * while the current configuration is in use.  SCHD_SIGFLAGS_BUSY
 * is set when the scheduler is actively using the structs defined
 * by the configuration.  The SIGHUP handler sets the flag
 * SCHD_SIGFLAGS_RECONFIG which signals the scheduler to reconfigure
 * itself before beginning the next scheduling iteration.
 */
volatile sig_atomic_t schd_sigflags = 0;

/*
 * Declarations of global variables related to configuration
 */

/* Queue lists. */
QueueList  *schd_SubmitQueue		= NULL;
QueueList  *schd_BatchQueues		= NULL;

/* Fair Access Control List */
FairAccessList *schd_FairACL		= NULL;

int     schd_FirstRun		= 1;

/* Scheduler configuration parameters */
int     schd_STARVATION_TIMEOUT		= 0;
time_t  schd_ENFORCE_PRIME_TIME		= 0;
time_t  schd_PRIME_TIME_START		= 0;
time_t  schd_PRIME_TIME_END		= 0;
time_t	schd_PT_WALLT_LIMIT		= 0;
time_t  schd_MAX_WAIT_TIME		= 60*60*24*5;  /* five days */
int     schd_TARGET_LOAD_PCT		= 90;
int     schd_TARGET_LOAD_MINUS		= 15;
int     schd_TARGET_LOAD_PLUS		= 10;
int     schd_MAX_USER_RUN_JOBS		= 10;
int     schd_SCHED_RESTART_ACTION	= 0;
int     schd_SUSPEND_THRESHOLD		= 10;
int     schd_FORCE_REQUEUE		= 1;
int     schd_UPDATE_COMMENTS		= 1;

char    schd_ThisHost[PBS_MAXHOSTNAME];
char   *schd_CmdStr[16];
char   *schd_SCHED_HOST			= NULL;
char   *schd_JOB_DUMPFILE		= NULL;
char   *schd_PREDICTED_SCHEDULE		= NULL;
char   *schd_CfgFilename		= NULL;
char   *schd_AllocFilename		= NULL;
char   *schd_CurrentFilename		= NULL;

static void catch_HUP (int signo);
static void reset_config (void);
static void create_sched_cmdstr (void);

/*
 * Initialize the scheduler from the configuration and holidays files.  
 */
/*ARGSUSED*/
int 
schedinit(int argc, char *argv[])
{
    char   *id = "schedinit";
    struct utsname name;
    struct sigaction act, oact;
    char   *ptr, canon[PBS_MAXHOSTNAME + 1];

    DBPRT(("\n%s\n", schd_VersionString));
    log_record(PBSEVENT_SYSTEM, PBS_EVENTCLASS_SERVER, id, 
	schd_VersionString);

    /* 
     * If this is the initial startup configuration, then schd_TimeNow will
     * be 0.  Initialize it to the current time, so it can be tested against
     * various times when initializing.
     */

    if (schd_TimeNow == 0) {
	schd_TimeNow = time(NULL);
	DBPRT(("%s: initialize/startup at %s", id, ctime(&schd_TimeNow)));
    }

    /*
     * Determine location of configuration file.  Check for the presence of
     * the PBSSCHED_CONFIG environment variable.  If not defined, fall back 
     * to the compiled default CONFIGFILE.
     * 
     * Since neither the environment variables nor the compiled-in default
     * can be changed (with the exception of someone wreaking havoc with
     * a debugger or something), this only needs to be done once.
     */
    if (schd_CfgFilename == NULL) {
	ptr = getenv("PBSSCHED_CONFIG");
	if (ptr == NULL)
	    ptr = CONFIGFILE;

	schd_CfgFilename = schd_strdup(ptr);
	if (schd_CfgFilename == NULL) {
	    (void)sprintf(log_buffer, "schd_strdup() failed for configfile");
	    log_record(PBSEVENT_SYSTEM, PBS_EVENTCLASS_SERVER, id, log_buffer);
	    DBPRT(("%s: %s\n", id, log_buffer));
	    return (-1);
	}

    }

    /* 
     * From this point on, goto cleanup_and_error: to clean up any allocated 
     * storage for filenames.
     */

    DBPRT(("SCHEDINIT: Reading configuration from '%s'\n", schd_CfgFilename));

    /* Reset the configuration to a "known" state. */
    reset_config();

    /* Determine on what host this scheduler running. */
    uname(&name);
    if (get_fullhostname(name.nodename, canon, PBS_MAXHOSTNAME) == 0) {
	strncpy(schd_ThisHost, canon, PBS_MAXHOSTNAME);
    } else {
	(void)sprintf(log_buffer, 
	    "Failed to canonicalize uname %s (using it anyway)",
	    name.nodename);
	log_record(PBSEVENT_SYSTEM, PBS_EVENTCLASS_SERVER, id, log_buffer);
	DBPRT(("%s: %s\n", id, log_buffer));
	strncpy(schd_ThisHost, name.nodename, PBS_MAXHOSTNAME);
    }
    schd_lowercase(schd_ThisHost);
    DBPRT(("%s: This host is '%s'\n", id, schd_ThisHost));

    /* 
     * Register the state of the config file.  The call to reset_config()
     * above will have cleared all file watches.
     */
    if (schd_register_file(schd_CfgFilename)) {
	(void)sprintf(log_buffer, "cannot watch %s", schd_CfgFilename);
	log_record(PBSEVENT_SYSTEM, PBS_EVENTCLASS_SERVER, id, log_buffer);
	DBPRT(("%s: %s\n", id, log_buffer));

	goto cleanup_and_error;
    }

    /* Read the configuration file. */
    if (schd_get_config(schd_CfgFilename))
	return (-1);


    /* if we are NOT making a distinction between Prime and Non-Prime Time,
     * then simply empty the holidays array; otherwise, try to read and load
     * the holidays file.
     */
    if (!schd_ENFORCE_PRIME_TIME)
	init_holidays();	
    else {
        /*
         * Register the state of the holidays file.  This allows schd_req() to
         * reload it if it is changed.
         */
        if (schd_register_file(HOLIDAYS_FILE)) {
	    (void)sprintf(log_buffer, "cannot watch %s", HOLIDAYS_FILE);
	    log_record(PBSEVENT_SYSTEM, PBS_EVENTCLASS_SERVER, id, log_buffer);
	    DBPRT(("%s: %s\n", id, log_buffer));

	    goto cleanup_and_error;
        }

        /* Get a list of prime/non-prime times from the holidays file */
        if (schd_read_holidays() < 0)
	    return (-1);
    }

    /* 
     * Set up a signal handler for SIGHUP.  catch_HUP() will re-read the
     * configuration file.
     */
    act.sa_flags   = 0;
    act.sa_handler = catch_HUP;
    sigemptyset(&act.sa_mask);
    if (sigaction(SIGHUP, &act, &oact)) {
	(void)sprintf(log_buffer, "Failed to setup SIGHUP handler.");
	log_record(PBSEVENT_SYSTEM, PBS_EVENTCLASS_SERVER, id, log_buffer);
	DBPRT(("%s: %s\n", id, log_buffer));
    }

    DBPRT(("SCHEDINIT: configuration complete.\n"));
    return (0);

cleanup_and_error:

    /* 
     * Some error occurred.  Remove watches and free the storage allocated
     * for the filenames.
     */
    if (schd_CfgFilename) {
	schd_forget_file(schd_CfgFilename);
	free(schd_CfgFilename);
    }

    schd_CfgFilename     = NULL;

    return (-1);
}

/*ARGSUSED*/
static void
catch_HUP(int signo)
{
    /* 
     * Arrange for the configuration file to be re-read before the next
     * scheduling iteration.  It is not safe (due to global state in the
     * C library) to use any stream-based I/O (c.f. POSIX1003.1, Section
     * 3.3.1.3, note 3f).
     *
     */

    schd_sigflags |= SCHD_SIGFLAGS_RECONFIG;

    return;
}

/*
 * Reset the scheduler's configuration to a "known" state.
 */

static void
reset_config (void)
{
    /* char   *id = "reset_config"; */

    /* 
     * Clear out any contents of the lists previously defined.
     */

    if (schd_SubmitQueue)		schd_destroy_qlist(schd_SubmitQueue);
    if (schd_BatchQueues)		schd_destroy_qlist(schd_BatchQueues);
    if (schd_FairACL)			schd_destroy_fairshare(schd_FairACL);

    /* 
     * Clear queue list head pointers.  The contents of the lists have
     * been destroyed, but the head still points to invalidated memory.
     */

    schd_SubmitQueue			= NULL;
    schd_BatchQueues			= NULL;
    schd_FairACL			= NULL;
    schd_FirstRun			= 1;	/* First run since reconfig */

    /* 
     * Reset scheduler configuration parameters
     */
    schd_ENFORCE_PRIME_TIME		= 0;
    schd_PRIME_TIME_START		= 0;
    schd_PRIME_TIME_END			= 0;
    schd_PT_WALLT_LIMIT			= 0;
    schd_PT_WALLT_LIMIT			= 0;
    schd_MAX_WAIT_TIME			= 60*60*24*5; /* five days */
    schd_TARGET_LOAD_PCT		= 90;
    schd_TARGET_LOAD_MINUS		= 15;
    schd_TARGET_LOAD_PLUS		= 10;
    schd_MAX_USER_RUN_JOBS		= 10;
    schd_SCHED_RESTART_ACTION		= 0;
    schd_SUSPEND_THRESHOLD		= 10;
    schd_FORCE_REQUEUE			= 1;
    schd_UPDATE_COMMENTS		= 1;

    /* 
     * Free allocated storage for configuration commands. 
     */
    if (schd_SCHED_HOST)		free(schd_SCHED_HOST);
    if (schd_JOB_DUMPFILE)		free(schd_JOB_DUMPFILE);
    if (schd_PREDICTED_SCHEDULE)	free(schd_PREDICTED_SCHEDULE);

    schd_SCHED_HOST			= NULL;
    schd_JOB_DUMPFILE			= NULL;
    schd_PREDICTED_SCHEDULE		= NULL;

    /*
     * Clear file change times.  NULL means "clear all files".
     */

    schd_forget_file(NULL);
    schd_ThisHost[0] 			= '\0';

    /* Get the text-strings for the scheduler commands. */
    create_sched_cmdstr();

}

static void
create_sched_cmdstr(void)
{
    static char *c_null		= "Null command";
    static char *c_new		= "New job arrived";
    static char *c_term		= "Job terminated";
    static char *c_time		= "Periodic run";
    static char *c_recyc	= "Scheduler re-cycled";
    static char *c_cmd		= "Schedule request";
    static char *c_config	= "Configure scheduler";
    static char *c_quit		= "Exit gracefully";
    static char *c_rules	= "New ruleset requested";
    static char *c_first	= "Server startup, 1st run";

    memset(schd_CmdStr, 0, sizeof (schd_CmdStr));

    schd_CmdStr[SCH_SCHEDULE_NULL]	= c_null;
    schd_CmdStr[SCH_SCHEDULE_NEW]	= c_new;
    schd_CmdStr[SCH_SCHEDULE_TERM]	= c_term;
    schd_CmdStr[SCH_SCHEDULE_TIME]	= c_time;
    schd_CmdStr[SCH_SCHEDULE_RECYC]	= c_recyc;
    schd_CmdStr[SCH_SCHEDULE_CMD]	= c_cmd;
    schd_CmdStr[SCH_CONFIGURE]		= c_config;
    schd_CmdStr[SCH_QUIT]		= c_quit;
    schd_CmdStr[SCH_RULESET]		= c_rules;
    schd_CmdStr[SCH_SCHEDULE_FIRST]	= c_first;
}

int 
schd_destroy_fairshare(FairAccessList *list)
{
    int num_items = 0;
    FairAccessList *FALptr, *N_FALptr;
    AccessEntry    *AEptr, *N_AEptr;

    if (schd_FairACL) {

        for (FALptr = schd_FairACL; FALptr != NULL; FALptr = N_FALptr) {
	    N_FALptr = FALptr->next;
            for (AEptr = FALptr->entry; AEptr != NULL; AEptr = N_AEptr) {
		N_AEptr = AEptr->next;
                if (AEptr->name)
		    free(AEptr->name);
	        free(AEptr);
    	    }
	    free(FALptr);
        }
    }
    schd_FairACL = NULL;

    return (num_items);
}
