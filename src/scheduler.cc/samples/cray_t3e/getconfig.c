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
 * This module contains the necessary functions for
 * processing the config file and setting the 
 * appropriate variables.  These functions are:
 *
 * get_config:   main loop for processing the config file
 * open_config:  open the configuration file for read
 * close_config: close the configuration file
 * set_cf_gopt:  validate the configuration file and
 *               set appropriate variables
 */

#include <sys/types.h>
#include <ctype.h>
#include <errno.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include "pbs_ifl.h"
#include "net_connect.h"
#include "log.h"

/* Scheduler header files */
#include "toolkit.h"
#include "gblxvars.h"

#define	MAX_LINE_SIZE	512	/* maximum line size */

extern int errno;

static int post_config (void);
static int validate_config (void);
static int set_cfg_opt (char *, char *);
static int arg_to_qlist (char *, char *, QueueList **);
static int get_variance (char *, int *, int *);
static void print_config (void);
static int qlist_disjoint (QueueList *qlist1, QueueList *qlist2);
static int qlist_unique (QueueList *qlist);
static int qlist_one_per_host (QueueList *qlist);

/* Main loop for processing the configuration file */
int 
schd_get_config(char *filename)
{
    char   *id = "schd_get_config";

    char    line[MAX_LINE_SIZE];
    char    cfg_option[MAX_LINE_SIZE];
    char    cfg_arg[MAX_LINE_SIZE];
    FILE   *cfgfd = NULL;

    char   *comment;
    int     linenum = 0, error = 0;
    size_t  linelen;

    cfgfd = fopen(filename, "r");

    if (cfgfd == NULL) {
	(void)sprintf(log_buffer, 
	    "Opening '%s': %s", filename, strerror(errno));

	log_record(PBSEVENT_SYSTEM, PBS_EVENTCLASS_SERVER, id, log_buffer);
	DBPRT(("%s: %s\n", id, log_buffer));

	return (-1);
    }

    /* Walk through the configuration file line by line. */
    while (fgets(line, sizeof (line), cfgfd)) {
	linenum++;

	linelen = strlen(line);

	if (linelen == (sizeof (line) - 1)) {
	    (void)sprintf(log_buffer, 
	       "Error in config file %s, line %d: Line too long",
		filename, linenum);
	    log_record(PBSEVENT_SYSTEM, PBS_EVENTCLASS_SERVER, id, log_buffer);
	    DBPRT(( "%s\n", log_buffer));

	    error = -1;
	    break;
	} else {
	    linelen--;			/* Move back to newline. */
	    line[linelen] = '\0';	/* And remove it. */
	}

	/* If there is a comment on this line, remove it from view. */
	if ((comment = strchr(line, '#')) != NULL) 
	    *comment = '\0';

	if (strlen(line) < 2)
	    continue;		/* skip blank lines */

	/* Split the option and the argument. */
	if (sscanf(line, "%s %s", cfg_option, cfg_arg) != 2) {
	    /* Unable to read a cfg_option and cfg_arg */
	    (void)sprintf(log_buffer, "Error in config file %s, line %d",
		filename, linenum);
	    log_record(PBSEVENT_SYSTEM, PBS_EVENTCLASS_SERVER, id, log_buffer);
	    DBPRT(( "%s\n", log_buffer));

	    error = -1;
	    break;
        }

	/* Set the configurable options */
	if (set_cfg_opt(cfg_option, cfg_arg)) {
	    /* Unable to parse the option. */
	    (void)sprintf(log_buffer, 
		"Error parsing option '%s' in config file %s, line %d",
		cfg_option, filename, linenum);
	    log_record(PBSEVENT_SYSTEM, PBS_EVENTCLASS_SERVER, id, log_buffer);
	    DBPRT(( "%s\n", log_buffer));

	    error = -1;
	    break;
	}
    }

    /* Close the config file */

    fclose(cfgfd);

    if (error)
	return (error);

    /* Do any post-configuration necessary. */
    if (!post_config())
	return (-1);

    if (!validate_config())
	return (-1);

    print_config();
    return (0);
}


/* 
 * Now that an option and its argument have been read, validate them and
 * set the appropriate global configuration variables.
 */
static int 
set_cfg_opt(char *cfg_option, char *cfg_arg)
{
    char   *id = "set_cfg_opt";

    /* XXX Should smash case on these before doing string compares? */

    if (!strcmp(cfg_option, "TARGET_LOAD_PCT")) {
	schd_TARGET_LOAD_PCT = atoi(cfg_arg);
	return (0);
    }
    
    if (!strcmp(cfg_option, "TARGET_LOAD_VARIANCE")) {
	return (get_variance(cfg_arg, 
	    &schd_TARGET_LOAD_MINUS, &schd_TARGET_LOAD_PLUS));
    }
    
    if (!strcmp(cfg_option, "HIGH_SYSTIME")) {
	schd_HIGH_SYSTIME = atoi(cfg_arg);
	return (0);
    }
    
    if (!strcmp(cfg_option, "MAX_JOBS")) {
	schd_MAX_JOBS = atoi(cfg_arg);
	return (0);
    }
    
    if (!strcmp(cfg_option, "MIN_JOBS")) {
	schd_MIN_JOBS = atoi(cfg_arg);
	return (0);
    }
    
    if (!strcmp(cfg_option, "MAX_DEDICATED_JOBS")) {
	schd_MAX_DEDICATED_JOBS = atoi(cfg_arg);
	return (0);
    }
    
    if (!strcmp(cfg_option, "MAX_USER_RUN_JOBS")) {
	schd_MAX_USER_RUN_JOBS = atoi(cfg_arg);
	return (0);
    }
    
    if (!strcmp(cfg_option, "ENFORCE_ALLOCATION")) {
	return schd_val2booltime(cfg_arg, &schd_ENFORCE_ALLOCATION);
    }
    
    if (!strcmp(cfg_option, "TEST_ONLY")) {
	return schd_val2bool(cfg_arg, &schd_TEST_ONLY);
    }
    
    if (!strcmp(cfg_option, "ENFORCE_DEDICATED_TIME")) {
	return schd_val2booltime(cfg_arg, &schd_ENFORCE_DEDTIME);
    }
    
    if (!strcmp(cfg_option, "DEDICATED_TIME_COMMAND")) {
	if (schd_DEDTIME_COMMAND)
	    free(schd_DEDTIME_COMMAND);
	schd_DEDTIME_COMMAND = schd_strdup(cfg_arg);
	if (schd_DEDTIME_COMMAND == NULL) {
	    log_record(PBSEVENT_SYSTEM, PBS_EVENTCLASS_SERVER, id,
		"schd_strdup(schd_DEDTIME_COMMAND)");
	    return (-1);
	}
	return (0);
    }

    if (!strcmp(cfg_option, "DEDICATED_TIME_CACHE_SECS")) {
	schd_DEDTIME_CACHE_SECS = atoi(cfg_arg);
	return (0);
    }
    
    if (!strcmp(cfg_option, "DECAY_FACTOR")) {
	schd_DECAY_FACTOR = atof(cfg_arg);
	return (0);
    }
    
    if (!strcmp(cfg_option, "OA_DECAY_FACTOR")) {
	schd_OA_DECAY_FACTOR = atof(cfg_arg);
	return (0);
    }
    
    if (!strcmp(cfg_option, "SCHED_ACCT_DIR")) {
	if (schd_SCHED_ACCT_DIR)
	    free(schd_SCHED_ACCT_DIR);
	schd_SCHED_ACCT_DIR = schd_strdup(cfg_arg);
	if (schd_SCHED_ACCT_DIR == NULL) {
	    log_record(PBSEVENT_SYSTEM, PBS_EVENTCLASS_SERVER, id,
		"schd_strdup(schd_SCHED_ACCT_DIR)");
	    return (-1);
	}
	return (0);
    }
    
    if (!strcmp(cfg_option, "SYSTEM_NAME")) {
	if (schd_SYSTEM_NAME)
	    free(schd_SYSTEM_NAME);
	schd_SYSTEM_NAME = schd_strdup(cfg_arg);
	schd_lowercase(schd_SYSTEM_NAME);
	return (0);
    }
    
    if (!strcmp(cfg_option, "SERVER_HOST")) {
	if (schd_SERVER_HOST)
	    free(schd_SERVER_HOST);
	schd_SERVER_HOST = schd_strdup(cfg_arg);
	if (schd_SERVER_HOST == NULL) {
	    log_record(PBSEVENT_SYSTEM, PBS_EVENTCLASS_SERVER, id,
		"schd_strdup(schd_SERVER_HOST)");
	    return (-1);
	}
	schd_lowercase(schd_SERVER_HOST);
	return (0);
    }
    
    if (!strcmp(cfg_option, "SCHED_HOST")) {
	if (schd_SCHED_HOST)
	    free(schd_SCHED_HOST);
	schd_SCHED_HOST = schd_strdup(cfg_arg);
	if (schd_SCHED_HOST == NULL) {
	    log_record(PBSEVENT_SYSTEM, PBS_EVENTCLASS_SERVER, id,
		"schd_strdup(schd_SCHED_HOST)");
	    return (-1);
	}
	schd_lowercase(schd_SCHED_HOST);
	return (0);
    }
    
    if (!strcmp(cfg_option, "SORTED_JOB_DUMPFILE")) {
	if (schd_JOB_DUMPFILE)
	    free(schd_JOB_DUMPFILE);
	schd_JOB_DUMPFILE = schd_strdup(cfg_arg);
	if (schd_JOB_DUMPFILE == NULL) {
	    log_record(PBSEVENT_SYSTEM, PBS_EVENTCLASS_SERVER, id,
		"schd_strdup(schd_JOB_DUMPFILE)");
	    return (-1);
	}
	return (0);
    }
    
    if (!strcmp(cfg_option, "SCHED_RESTART_ACTION")) {
	if (strcmp(cfg_arg, "NONE") == 0) {
	    schd_SCHED_RESTART_ACTION = SCHD_RESTART_NONE;
	    return (0);
	}

	if (strcmp(cfg_arg, "RESUBMIT") == 0) {
	    schd_SCHED_RESTART_ACTION = SCHD_RESTART_RESUBMIT;
	    return (0);
	}

	if (strcmp(cfg_arg, "RERUN") == 0) {
	    schd_SCHED_RESTART_ACTION = SCHD_RESTART_RERUN;
	    return (0);
	}

	return (-1);		/* Bad argument */
    }
    
    if (!strcmp(cfg_option, "SORT_BY_PAST_USAGE")) {
	return schd_val2bool(cfg_arg, &schd_SORT_BY_PAST_USAGE);
    }
    
    if (!strcmp(cfg_option, "ENFORCE_PRIME_TIME")) {
	return schd_val2booltime(cfg_arg, &schd_ENFORCE_PRIME_TIME);
    }
    
    if (!strcmp(cfg_option, "PRIME_TIME_START")) {
	schd_PRIME_TIME_START = schd_val2sec(cfg_arg);
	return (0);
    }
    
    if (!strcmp(cfg_option, "PRIME_TIME_END")) {
	schd_PRIME_TIME_END = schd_val2sec(cfg_arg);
	return (0);
    }

    if (!strcmp(cfg_option, "PRIME_TIME_SMALL_NODE_LIMIT")) {
	schd_PT_SMALL_NODE_LIMIT = atoi(cfg_arg);
	return (0);
    }

    if (!strcmp(cfg_option, "PRIME_TIME_SMALL_WALLT_LIMIT")) {
	schd_PT_SMALL_WALLT_LIMIT = schd_val2sec(cfg_arg);
	return (0);
    }

    if (!strcmp(cfg_option, "PRIME_TIME_WALLT_LIMIT")) {
	schd_PT_WALLT_LIMIT = schd_val2sec(cfg_arg);
	return (0);
    }

    if (!strcmp(cfg_option, "NONPRIME_DRAIN_SYS")) {
	return schd_val2bool(cfg_arg, &schd_NONPRIME_DRAIN_SYS);
    }

    if (!strcmp(cfg_option, "NP_DRAIN_BACKTIME")) {
	schd_NP_DRAIN_BACKTIME = schd_val2sec(cfg_arg);
	return (0);
    }

    if (!strcmp(cfg_option, "NP_DRAIN_IDLETIME")) {
	schd_NP_DRAIN_IDLETIME = schd_val2sec(cfg_arg);
	return (0);
    }


    if (!strcmp(cfg_option, "SMALL_JOB_MAX")) {
	schd_SMALL_JOB_MAX = atoi(cfg_arg);
	return (0);
    }

    if (!strcmp(cfg_option, "WALLT_LIMIT_LARGE_JOB")) {
	schd_WALLT_LARGE_LIMIT = schd_val2sec(cfg_arg);
	return (0);
    }

    if (!strcmp(cfg_option, "WALLT_LIMIT_SMALL_JOB")) {
	schd_WALLT_SMALL_LIMIT = schd_val2sec(cfg_arg);
	return (0);
    }


    if (!strcmp(cfg_option, "MAX_QUEUED_TIME")) {
	schd_MAX_QUEUED_TIME = schd_val2sec(cfg_arg);
	return (0);
    }
    
    if (!strcmp(cfg_option, "SMALL_QUEUED_TIME")) {
	schd_SMALL_QUEUED_TIME = schd_val2sec(cfg_arg);
	return (0);
    }
    
    if (!strcmp(cfg_option, "INTERACTIVE_LONG_WAIT")) {
	schd_INTERACTIVE_LONG_WAIT = schd_val2sec(cfg_arg);
	return (0);
    }
    
    if (!strcmp(cfg_option, "SUBMIT_QUEUE")) {
	arg_to_qlist(cfg_arg, ",", &schd_SubmitQueue);
	return (0);
    }
    
    if (!strcmp(cfg_option, "BATCH_QUEUES")) {
	arg_to_qlist(cfg_arg, ",", &schd_BatchQueues);
	return (0);
    }
    
    if (!strcmp(cfg_option, "DEDICATED_QUEUES")) {
	arg_to_qlist(cfg_arg, ",", &schd_DedQueues);
	return (0);
    }

    if (!strcmp(cfg_option, "SPECIAL_QUEUE")) {
	if (schd_SpecialQueue)
	    free(schd_SpecialQueue);
	schd_SpecialQueue = schd_strdup(cfg_arg);
	if (schd_SpecialQueue == NULL) {
	    log_record(PBSEVENT_SYSTEM, PBS_EVENTCLASS_SERVER, id,
		"schd_strdup(schd_SpecialQueue)");
	    return (-1);
	}
	return (0);
    }

    if (!strcmp(cfg_option, "CHALLENGE_QUEUE_NAME")) {
	if (schd_ChallengeQueue)
	    free(schd_ChallengeQueue);
	schd_ChallengeQueue = schd_strdup(cfg_arg);
	if (schd_ChallengeQueue == NULL) {
	    log_record(PBSEVENT_SYSTEM, PBS_EVENTCLASS_SERVER, id,
		"schd_strdup(schd_ChallengeQueue)");
	    return (-1);
	}
	return (0);
    }
    if (!strcmp(cfg_option, "BACKGROUND_QUEUE_NAME")) {
	if (schd_BackgroundQueue)
	    free(schd_BackgroundQueue);
	schd_BackgroundQueue = schd_strdup(cfg_arg);
	if (schd_BackgroundQueue == NULL) {
	    log_record(PBSEVENT_SYSTEM, PBS_EVENTCLASS_SERVER, id,
		"schd_strdup(schd_BackgroundQueue)");
	    return (-1);
	}
	return (0);
    }

    if (!strcmp(cfg_option, "FAKE_MACHINE_MULT")) {
	schd_FAKE_MACH_MULT = atoi(cfg_arg);
	return (0);
    }
    
    if (!strcmp(cfg_option, "AVOID_FRAGMENTATION")) {
	return schd_val2bool(cfg_arg, &schd_AVOID_FRAGS);
    }

    if (!strcmp(cfg_option, "USAGE_WEIGHT")) {
	schd_USAGE_WEIGHT = atoi(cfg_arg);
	return (0);
    }
    
    if (!strcmp(cfg_option, "NODES_WEIGHT")) {
	schd_NODES_WEIGHT = atoi(cfg_arg);
	return (0);
    }
    
    if (!strcmp(cfg_option, "TIMEQ_WEIGHT")) {
	schd_TIMEQ_WEIGHT = atoi(cfg_arg);
	return (0);
    }
    
    
    /* Unknown option -- return an error. */
    return (-1);
}

static int
post_config(void)
{
    /* char   *id = "post_config"; */

    /* Set up per-queue primetime enforcement. */
    if (schd_BatchQueues)
        schd_reset_observed_pt(schd_BatchQueues);

    /* Post processing complete. */
    return (1);
}

static int 
validate_config(void)
{
    char   *id = "validate_config";
    char   *xit = "  Exiting.";
    /*
     * Some simple tests to make sure the configuration is
     * at least minimally rational.
     */

    if (schd_TEST_ONLY) {
	(void)sprintf(log_buffer, "NOTE: Scheduler is in testing-only mode.");
	log_record(PBSEVENT_SYSTEM, PBS_EVENTCLASS_SERVER, id, log_buffer);
	DBPRT(("%s: %s\n", id, log_buffer));
    }

    DBPRT(("%s: validate submit queues\n", id));
    if (schd_SubmitQueue == NULL) {
	(void)sprintf(log_buffer, "No SUBMIT_QUEUE specified.%s", xit);
	log_record(PBSEVENT_SYSTEM, PBS_EVENTCLASS_SERVER, id, log_buffer);
	DBPRT(("%s\n", log_buffer));
	return (0);
    }
    if (schd_SubmitQueue->next != NULL) {
	(void)sprintf(log_buffer,
	    "Only one SUBMIT_QUEUE can be specified.%s", xit);
	log_record(PBSEVENT_SYSTEM, PBS_EVENTCLASS_SERVER, id, log_buffer);
	DBPRT(("%s\n", log_buffer));
	return (0);
    }

    DBPRT(("%s: validate batch queues\n", id));
    if (schd_BatchQueues == NULL) {
	(void)sprintf(log_buffer, "No BATCH_QUEUES specified.%s", xit);
	log_record(PBSEVENT_SYSTEM, PBS_EVENTCLASS_SERVER, id, log_buffer);
	DBPRT(("%s\n", log_buffer));
	return (0);
    }
    if (!qlist_unique(schd_BatchQueues)) {
	(void)sprintf(log_buffer, 
	    "Queues listed in BATCH_QUEUES cannot be repeated.%s", xit);
	log_record(PBSEVENT_SYSTEM, PBS_EVENTCLASS_SERVER, id, log_buffer);
	DBPRT(("%s\n", log_buffer));
	return (0);
    }

    if (schd_ENFORCE_PRIME_TIME) {
	DBPRT(("%s: validate primetime\n", id));
	if (schd_PRIME_TIME_START <= 0 || schd_PRIME_TIME_END <= 0) {
	    (void)sprintf(log_buffer, 
		"ENFORCE_PRIME_TIME set but PRIME_TIME_START/END is not "
		"specified.%s", xit);
	    log_record(PBSEVENT_SYSTEM, PBS_EVENTCLASS_SERVER, id, log_buffer);
	    DBPRT(("%s\n", log_buffer));
	    return (0);
	}

        if (schd_PT_WALLT_LIMIT <= 0) {
            (void)sprintf(log_buffer, 
                "ENFORCE_PRIME_TIME set but no PRIME_TIME_WALLT_LIMIT given.");
            log_record(PBSEVENT_SYSTEM, PBS_EVENTCLASS_SERVER, id, log_buffer);
            DBPRT(("%s\n", log_buffer));
            return (0);
        }

        if ((schd_PT_SMALL_NODE_LIMIT > 0) && 
            (schd_PT_SMALL_WALLT_LIMIT <= 0)) 
        {
            (void)sprintf(log_buffer, 
                "PRIMETIME_SMALL_NODE_LIMIT set but no "
		"PRIME_TIME_SMALL_WALLT_LIMIT given.");
            log_record(PBSEVENT_SYSTEM, PBS_EVENTCLASS_SERVER, id, log_buffer);
            DBPRT(("%s\n", log_buffer));
            return (0);
        }

        if ((schd_PT_SMALL_NODE_LIMIT <= 0) && 
            (schd_PT_SMALL_WALLT_LIMIT > 0))
        {
            (void)sprintf(log_buffer, 
                "PRIME_TIME_SMALL_WALLT_LIMIT set but no "
		"PRIMETIME_SMALL_NODE_LIMIT given.");
            log_record(PBSEVENT_SYSTEM, PBS_EVENTCLASS_SERVER, id, log_buffer);
            DBPRT(("%s\n", log_buffer));
            return (0);
        }
    }

    if (schd_SMALL_JOB_MAX) {
	if (schd_WALLT_LARGE_LIMIT <= 0) {
	    (void)sprintf(log_buffer, 
		"SMALL_JOB_MAX set but no WALLT_LIMIT_LARGE_JOB given.");
	    log_record(PBSEVENT_SYSTEM, PBS_EVENTCLASS_SERVER, id, log_buffer);
	    DBPRT(("%s\n", log_buffer));
	    return (0);
	}

	if (schd_WALLT_SMALL_LIMIT <= 0) {
	    (void)sprintf(log_buffer, 
		"SMALL_JOB_MAX set but no WALLT_LIMIT_SMALL_JOB given.");
	    log_record(PBSEVENT_SYSTEM, PBS_EVENTCLASS_SERVER, id, log_buffer);
	    DBPRT(("%s\n", log_buffer));
	    return (0);
	}
    }

    if (schd_ENFORCE_DEDTIME) {
	DBPRT(("%s: validate dedicated queue\n", id));
	if (schd_DedQueues == NULL) {
	    (void)sprintf(log_buffer, 
		"ENFORCE_DEDICATED_TIME set but no dedicated queues "
		"specified.%s", xit);
	    log_record(PBSEVENT_SYSTEM, PBS_EVENTCLASS_SERVER, id, log_buffer);
	    DBPRT(("%s\n", log_buffer));
	    return (0);
	}
	if (!qlist_one_per_host(schd_DedQueues)) {
	    (void)sprintf(log_buffer, 
	    "Only one DEDICATED_QUEUES entry per exechost allowed.%s", xit);
	    log_record(PBSEVENT_SYSTEM, PBS_EVENTCLASS_SERVER, id, log_buffer);
	    DBPRT(("%s\n", log_buffer));
	    return (0);
	}
	if (!qlist_disjoint(schd_DedQueues, schd_BatchQueues)) {
	    (void)sprintf(log_buffer, 
	    "Queues in DEDICATED_QUEUES may not be listed in BATCH_QUEUES.%s",
		    xit);
	    log_record(PBSEVENT_SYSTEM, PBS_EVENTCLASS_SERVER, id, log_buffer);
	    DBPRT(("%s\n", log_buffer));
	    return (0);
	}
	DBPRT(("%s: validate dedicated time command\n", id));
	if (schd_DEDTIME_COMMAND == NULL) {
	    (void)sprintf(log_buffer, 
		"ENFORCE_DEDICATED_TIME set but no DEDICATED_TIME_COMMAND "
		"specified.%s", xit);
	    log_record(PBSEVENT_SYSTEM, PBS_EVENTCLASS_SERVER, id, log_buffer);
	    DBPRT(("%s\n", log_buffer));
	    return (0);
	}
	if (access(schd_DEDTIME_COMMAND, X_OK)) {
	    (void)sprintf(log_buffer, 
		"Execution denied for DEDICATED_TIME_COMMAND '%s'.%s", 
		schd_DEDTIME_COMMAND, xit);
	    log_record(PBSEVENT_SYSTEM, PBS_EVENTCLASS_SERVER, id, log_buffer);
	    DBPRT(("%s\n", log_buffer));
	    return (0);

	}
    }

    DBPRT(("%s: verify that queue sets are disjoint\n", id));
    if (!qlist_disjoint(schd_SubmitQueue, schd_BatchQueues)) {
	(void)sprintf(log_buffer, 
	    "SUBMIT_QUEUE %s cannot be listed in BATCH_QUEUES.%s",
		schd_SubmitQueue->queue->qname, xit);
	log_record(PBSEVENT_SYSTEM, PBS_EVENTCLASS_SERVER, id, log_buffer);
	DBPRT(("%s\n", log_buffer));
	return (0);
    }

    if ((schd_FAKE_MACH_MULT < 1)) {
	    (void)sprintf(log_buffer, 
		"FAKE_MACHINE_MULT must be integer number > 0.%s", xit);
	    log_record(PBSEVENT_SYSTEM, PBS_EVENTCLASS_SERVER, id, log_buffer);
	    DBPRT(("%s\n", log_buffer));
	    return (0);
    }

    if ((schd_TIMEQ_WEIGHT < 0)||(schd_TIMEQ_WEIGHT > 10)) {
	    (void)sprintf(log_buffer, 
		"TIMEQ_WEIGHT must be integer number [0,1-10].%s", xit);
	    log_record(PBSEVENT_SYSTEM, PBS_EVENTCLASS_SERVER, id, log_buffer);
	    DBPRT(("%s\n", log_buffer));
	    return (0);
    }
    if ((schd_USAGE_WEIGHT < 0)||(schd_USAGE_WEIGHT > 10)) {
	    (void)sprintf(log_buffer, 
		"USAGE_WEIGHT must be integer number [0,1-10].%s", xit);
	    log_record(PBSEVENT_SYSTEM, PBS_EVENTCLASS_SERVER, id, log_buffer);
	    DBPRT(("%s\n", log_buffer));
	    return (0);
    }
    if ((schd_NODES_WEIGHT < 0)||(schd_NODES_WEIGHT > 10)) {
	    (void)sprintf(log_buffer, 
		"NODES_WEIGHT must be integer number [0,1-10].%s", xit);
	    log_record(PBSEVENT_SYSTEM, PBS_EVENTCLASS_SERVER, id, log_buffer);
	    DBPRT(("%s\n", log_buffer));
	    return (0);
    }

    DBPRT(("%s: Scheduler configuration appears valid.\n", id));
    return (1);
}

/* print_config(): Dump the current config to the log */
static void
print_config(void)
{
    char   *id = "print_config";
    QueueList *qptr;

    if (schd_TEST_ONLY) {
	(void)sprintf(log_buffer, "%-24s = %s", "TEST_ONLY", 
	    schd_bool2val(schd_TEST_ONLY));
	log_record(PBSEVENT_SYSTEM, PBS_EVENTCLASS_SERVER, id, log_buffer);
    }

    if (schd_SubmitQueue) {
	(void)sprintf(log_buffer, "%-24s = %s@%s", "SUBMIT_QUEUE",
	    schd_SubmitQueue->queue->qname, schd_SubmitQueue->queue->exechost);
	log_record(PBSEVENT_SYSTEM, PBS_EVENTCLASS_SERVER, id, log_buffer);
    }

    if (schd_BatchQueues) {
	for (qptr = schd_BatchQueues; qptr != NULL; qptr = qptr->next) {
	    (void)sprintf(log_buffer, "%-24s = %s@%s", 
		(qptr == schd_BatchQueues) ? "BATCH_QUEUES" : "",
		qptr->queue->qname, qptr->queue->exechost);
	    log_record(PBSEVENT_SYSTEM, PBS_EVENTCLASS_SERVER, id, log_buffer);
	}
    }

    if (schd_DedQueues) {
	for (qptr = schd_DedQueues; qptr != NULL; qptr = qptr->next) {
	    (void)sprintf(log_buffer, "%-24s = %s@%s", 
		(qptr == schd_DedQueues) ? "DEDICATED_QUEUES" : "",
		qptr->queue->qname, qptr->queue->exechost);
	    log_record(PBSEVENT_SYSTEM, PBS_EVENTCLASS_SERVER, id, log_buffer);
	}
    }

    (void)sprintf(log_buffer, "%-24s = %s", "ENFORCE_PRIME_TIME",
	schd_booltime2val(schd_ENFORCE_PRIME_TIME));
    log_record(PBSEVENT_SYSTEM, PBS_EVENTCLASS_SERVER, id, log_buffer);

    (void)sprintf(log_buffer, "%-24s = %s", "PRIME_TIME_WALLT_LIMIT", 
        schd_sec2val(schd_PT_WALLT_LIMIT));
    log_record(PBSEVENT_SYSTEM, PBS_EVENTCLASS_SERVER, id, log_buffer);

    if (schd_PT_SMALL_NODE_LIMIT) {
        (void)sprintf(log_buffer, "%-24s = %d", "PRIME_TIME_SMALL_NODE_LIMIT", 
            schd_PT_SMALL_NODE_LIMIT);
        log_record(PBSEVENT_SYSTEM, PBS_EVENTCLASS_SERVER, id, log_buffer);

        (void)sprintf(log_buffer, "%-24s = %s", "PRIME_TIME_SMALL_WALLT_LIMIT",
            schd_sec2val(schd_PT_SMALL_WALLT_LIMIT));
        log_record(PBSEVENT_SYSTEM, PBS_EVENTCLASS_SERVER, id, log_buffer);
    }

    (void)sprintf(log_buffer, "%-24s = %s", "NONPRIME_DRAIN_SYS",
	schd_bool2val(schd_NONPRIME_DRAIN_SYS));
    log_record(PBSEVENT_SYSTEM, PBS_EVENTCLASS_SERVER, id, log_buffer);

    if (schd_NP_DRAIN_BACKTIME > 0) {
	(void)sprintf(log_buffer, "%-24s = %s", "NP_DRAIN_BACKTIME",
	    schd_sec2val(schd_NP_DRAIN_BACKTIME));
	log_record(PBSEVENT_SYSTEM, PBS_EVENTCLASS_SERVER, id, log_buffer);
    }

    if (schd_NP_DRAIN_IDLETIME > 0) {
	(void)sprintf(log_buffer, "%-24s = %s", "NP_DRAIN_IDLETIME",
	schd_sec2val(schd_NP_DRAIN_IDLETIME));
	log_record(PBSEVENT_SYSTEM, PBS_EVENTCLASS_SERVER, id, log_buffer);
    }

    (void)sprintf(log_buffer, "%-24s = %s", "WALLT_LIMIT_LARGE_JOB",
        schd_sec2val(schd_WALLT_LARGE_LIMIT));
    log_record(PBSEVENT_SYSTEM, PBS_EVENTCLASS_SERVER, id, log_buffer);

    if (schd_SMALL_JOB_MAX) {
        (void)sprintf(log_buffer, "%-24s = %d", "SMALL_JOB_MAX",
            schd_SMALL_JOB_MAX);
        log_record(PBSEVENT_SYSTEM, PBS_EVENTCLASS_SERVER, id, log_buffer);

        (void)sprintf(log_buffer, "%-24s = %s", "WALLT_LIMIT_SMALL_JOB",
            schd_sec2val(schd_WALLT_SMALL_LIMIT));
        log_record(PBSEVENT_SYSTEM, PBS_EVENTCLASS_SERVER, id, log_buffer);
    }

    (void)sprintf(log_buffer, "%-24s = %s", "PRIME_TIME_START",
	schd_sec2val(schd_PRIME_TIME_START));
    log_record(PBSEVENT_SYSTEM, PBS_EVENTCLASS_SERVER, id, log_buffer);

    (void)sprintf(log_buffer, "%-24s = %s", "PRIME_TIME_END",
	schd_sec2val(schd_PRIME_TIME_END));
    log_record(PBSEVENT_SYSTEM, PBS_EVENTCLASS_SERVER, id, log_buffer);

    (void)sprintf(log_buffer, "%-24s = %d%%", "TARGET_LOAD_PCT",
	schd_TARGET_LOAD_PCT);
    log_record(PBSEVENT_SYSTEM, PBS_EVENTCLASS_SERVER, id, log_buffer);

    (void)sprintf(log_buffer, "%-24s = -%d%%,+%d%%", "TARGET_LOAD_VARIANCE",
	schd_TARGET_LOAD_MINUS, schd_TARGET_LOAD_PLUS);
    log_record(PBSEVENT_SYSTEM, PBS_EVENTCLASS_SERVER, id, log_buffer);

    (void)sprintf(log_buffer, "%-24s = %d", "HIGH_SYSTIME", schd_HIGH_SYSTIME);
    log_record(PBSEVENT_SYSTEM, PBS_EVENTCLASS_SERVER, id, log_buffer);

    (void)sprintf(log_buffer, "%-24s = %d", "MAX_JOBS", schd_MAX_JOBS);
    log_record(PBSEVENT_SYSTEM, PBS_EVENTCLASS_SERVER, id, log_buffer);

    (void)sprintf(log_buffer, "%-24s = %d", "MIN_JOBS", schd_MIN_JOBS);
    log_record(PBSEVENT_SYSTEM, PBS_EVENTCLASS_SERVER, id, log_buffer);

    (void)sprintf(log_buffer, "%-24s = %s", "MAX_QUEUED_TIME",
	schd_sec2val(schd_MAX_QUEUED_TIME));
    log_record(PBSEVENT_SYSTEM, PBS_EVENTCLASS_SERVER, id, log_buffer);

    (void)sprintf(log_buffer, "%-24s = %s", "SMALL_QUEUED_TIME",
	schd_sec2val(schd_SMALL_QUEUED_TIME));
    log_record(PBSEVENT_SYSTEM, PBS_EVENTCLASS_SERVER, id, log_buffer);

    (void)sprintf(log_buffer, "%-24s = %d", "INTERACTIVE_LONG_WAIT",
	schd_INTERACTIVE_LONG_WAIT);
    log_record(PBSEVENT_SYSTEM, PBS_EVENTCLASS_SERVER, id, log_buffer);

    (void)sprintf(log_buffer, "%-24s = %d", "MAX_DEDICATED_JOBS",
	schd_MAX_DEDICATED_JOBS);
    log_record(PBSEVENT_SYSTEM, PBS_EVENTCLASS_SERVER, id, log_buffer);

    (void)sprintf(log_buffer, "%-24s = %s", "SORT_BY_PAST_USAGE",
	schd_bool2val(schd_SORT_BY_PAST_USAGE));
    log_record(PBSEVENT_SYSTEM, PBS_EVENTCLASS_SERVER, id, log_buffer);

    (void)sprintf(log_buffer, "%-24s = %s", "ENFORCE_ALLOCATION",
	schd_booltime2val(schd_ENFORCE_ALLOCATION));
    log_record(PBSEVENT_SYSTEM, PBS_EVENTCLASS_SERVER, id, log_buffer);

    (void)sprintf(log_buffer, "%-24s = %s", "ENFORCE_DEDICATED_TIME",
	schd_booltime2val(schd_ENFORCE_DEDTIME));
    log_record(PBSEVENT_SYSTEM, PBS_EVENTCLASS_SERVER, id, log_buffer);

    (void)sprintf(log_buffer, "%-24s = %s", "SCHED_ACCT_DIR",
	schd_SCHED_ACCT_DIR ? schd_SCHED_ACCT_DIR : "[null]");
    log_record(PBSEVENT_SYSTEM, PBS_EVENTCLASS_SERVER, id, log_buffer);

    (void)sprintf(log_buffer, "%-24s = %s", "DEDICATED_TIME_COMMAND",
	schd_DEDTIME_COMMAND ? schd_DEDTIME_COMMAND : "[null]");
    log_record(PBSEVENT_SYSTEM, PBS_EVENTCLASS_SERVER, id, log_buffer);

    (void)sprintf(log_buffer, "%-24s = %s", "SYSTEM_NAME",
	schd_SYSTEM_NAME ? schd_SYSTEM_NAME : "[null]");
    log_record(PBSEVENT_SYSTEM, PBS_EVENTCLASS_SERVER, id, log_buffer);

    (void)sprintf(log_buffer, "%-24s = %s", "SERVER_HOST",
	schd_SERVER_HOST ? schd_SERVER_HOST : "[null]");
    log_record(PBSEVENT_SYSTEM, PBS_EVENTCLASS_SERVER, id, log_buffer);

    (void)sprintf(log_buffer, "%-24s = %s", "SCHED_HOST",
	schd_SCHED_HOST ? schd_SCHED_HOST : "[null]");
    log_record(PBSEVENT_SYSTEM, PBS_EVENTCLASS_SERVER, id, log_buffer);

    (void)sprintf(log_buffer, "%-24s = %s", "SCHED_RESTART_ACTION",
	(schd_SCHED_RESTART_ACTION == SCHD_RESTART_NONE ? "NONE" : 
	(schd_SCHED_RESTART_ACTION == SCHD_RESTART_RESUBMIT ? "RESUBMIT" :
	(schd_SCHED_RESTART_ACTION == SCHD_RESTART_RERUN ? "RERUN" : "?"))));
    log_record(PBSEVENT_SYSTEM, PBS_EVENTCLASS_SERVER, id, log_buffer);

    if (schd_AVOID_FRAGS) {
	(void)sprintf(log_buffer, "%-24s = %s", "AVOID_FRAGMENTATION",
	    schd_bool2val(schd_AVOID_FRAGS));
	log_record(PBSEVENT_SYSTEM, PBS_EVENTCLASS_SERVER, id, log_buffer);
    }

    if (schd_JOB_DUMPFILE) {
	(void)sprintf(log_buffer, "%-24s = %s", "SORTED_JOB_DUMPFILE",
	    schd_JOB_DUMPFILE);
	log_record(PBSEVENT_SYSTEM, PBS_EVENTCLASS_SERVER, id, log_buffer);
    }

    if (schd_FAKE_MACH_MULT != 1) {
	(void)sprintf(log_buffer, "%-24s = %d", "FAKE_MACHINE_MULT",
	    schd_FAKE_MACH_MULT);
	log_record(PBSEVENT_SYSTEM, PBS_EVENTCLASS_SERVER, id, log_buffer);
    }
}

static int 
arg_to_qlist(char *arg, char *sep, QueueList **qlist_ptr)
{
    char	*id = "arg_to_qlist";
    QueueList  *qptr = NULL, *new;
    int     num = 0;
    char   *name, *exechost, canon[PBS_MAXHOSTNAME + 1];

    /*
     * Multiple lines may be used to add queues to the queue list.  Find
     * the tail of the passed-in list (if there is one), and assign the
     * qptr to the tail element.  Later, the new element will be hung off
     * qptr's next field (or qptr will be set to it.)
     */

    if (*qlist_ptr) {
	for (qptr = *qlist_ptr; qptr->next != NULL; qptr = qptr->next)
	    /* Walk the list, looking for last element. */;
    } else {
	qptr = NULL;
    }

    for (name = strtok(arg, sep); name != NULL; name = strtok(NULL, sep)) {

	/* 
	 * If the list is NULL, create the first element and point qptr
	 * at it.  If not, take the qptr from the last iteration (which
	 * will be the head the second time through) and place a new
	 * element on its next pointer.  Then replace qptr with the
	 * address of the newly allocated struct.
	 */
	
	new = (QueueList *)malloc(sizeof (QueueList));

	if (new == NULL) {
	    log_record(PBSEVENT_SYSTEM, PBS_EVENTCLASS_SERVER, id,
		"malloc(newQueue)");
	    goto error_in_list;
	}
	memset(new, 0, sizeof (QueueList));

	if (qptr == NULL) {
	    *qlist_ptr = new;
	    qptr = *qlist_ptr;
	} else {
	    qptr->next = new;
	    qptr = new;
	}

	new->queue = (Queue *)malloc(sizeof (Queue));
	if (new->queue == NULL) {
	    log_record(PBSEVENT_SYSTEM, PBS_EVENTCLASS_SERVER, id,
		"malloc(newQueue->queue)");
	    goto error_in_list;
	}
	memset(new->queue, 0, sizeof (Queue));

	/*
	 * Queue names may be either 'queue3' or 'queue3@exechost'.
	 * If there is a '@', convert it to a '\0' and copy the two
	 * halves of the string into the qname and exechost fields.
	 * Otherwise, this queue is local to this host - paste in the
	 * "local" hostname.
	 */
	if ((exechost = strchr(name, '@')) != NULL) {
	    /* Parse queue@host into queue and hostname. */
	    *exechost = '\0';		/* '@' ==> '\0' to terminate qname   */
	    exechost ++;		/* Next character after the new '\0' */

	    if (get_fullhostname(exechost, canon, PBS_MAXHOSTNAME) == 0) {
		exechost = canon;	/* Point at canonical name. */

	    } else {
		sprintf(log_buffer, "Warning: Cannot canonicalize queue %s@%s",
		    name, exechost);
		log_record(PBSEVENT_SYSTEM, PBS_EVENTCLASS_SERVER, id, 
		    log_buffer);
		DBPRT(("%s: %s\n", id, log_buffer));
	    }
	} else {
	    exechost = schd_ThisHost;	/* Queue lives on localhost. */
	}

	new->queue->qname = schd_strdup(name);
	if (new->queue->qname == NULL) {
	    log_record(PBSEVENT_SYSTEM, PBS_EVENTCLASS_SERVER, id,
		"schd_strdup(qname)");
	    goto error_in_list;
	}

	new->queue->exechost = schd_strdup(exechost);
	if (new->queue->exechost == NULL) {
	    log_record(PBSEVENT_SYSTEM, PBS_EVENTCLASS_SERVER, id,
		"schd_strdup(exechost)");
	    goto error_in_list;
	}

	num++;
    }

    return (num);

error_in_list:
    /* Something went wrong - delete the new list and return a fatal error. */

    if (*qlist_ptr) {
	schd_destroy_qlist(*qlist_ptr);
	*qlist_ptr = NULL;
    }

    return (-1);
}

static int
qlist_disjoint(QueueList *qlist1, QueueList *qlist2)
{
    QueueList *q1, *q2;

    /*
     * If both pointers are NULL, or they point to the same list, then
     * they are obviously not disjoint sets.
     */
    if (qlist1 == qlist2)
	return (0);

    /*
     * If any element of one list cannot be found in any element of the
     * other list, than the sets are disjoint.  Walk the first list, and
     * search for the queue on the second list.  If not found, then they
     * are disjoint.
     */

    for (q1 = qlist1; q1 != NULL; q1 = q1->next) {
	for (q2 = qlist2; q2 != NULL; q2 = q2->next)
	    /* Check the name, then break ties with exechost. */
	    if ((strcmp(q1->queue->qname, q2->queue->qname) == 0) &&
	        (strcmp(q1->queue->exechost, q2->queue->exechost) == 0))
	    {
		break;
	    }

	if (q2 == NULL)
	    return (1);
    }

    /*
     * Walk the second list, searching for the named queue on the first
     * list.  If not found, then they are disjoint.
     */

    for (q1 = qlist1; q1 != NULL; q1 = q1->next) {
	for (q2 = qlist2; q2 != NULL; q2 = q2->next)

	    /* Make sure both the name and execution host match. */
	    if ((strcmp(q1->queue->qname, q2->queue->qname) == 0) &&
	        (strcmp(q1->queue->exechost, q2->queue->exechost) == 0))
	    {
		break;
	    }

	if (q2 == NULL)
	    return (1);
    }

    /*
     * Both lists contain the same set of (possibly repeated) elements.
     * They are not disjoint.
     */

    return (0);
}

static int
qlist_unique(QueueList *qlist)
{
    QueueList *q1, *q2;

    /* No member of the empty set is repeated. */
    if (qlist == NULL)
	return (1);

    /* A list with only 1 element cannot have repeated elements. */
    if (qlist->next == NULL)
	return (1);

    /*
     * For each element in the list, see if any other element in the list
     * matches its queue name.  If so, the members of the set are not unique.
     */
    for (q1 = qlist; q1 != NULL; q1 = q1->next)
	for (q2 = qlist; q2 != NULL; q2 = q2->next) {
	    /* Is this the *same pointer* (not the same name)? */
	    if (q1 == q2)
		continue;
	    
	    /* 
	     * If the queues match, then the list members are not unique.
	     * Make sure both the name and execution host match.
	     */
	    if ((strcmp(q1->queue->qname, q2->queue->qname) == 0) &&
	        (strcmp(q1->queue->exechost, q2->queue->exechost) == 0))
	    {
		return (0);
	    }

	}

    /* No repeated elements found in the list -- it is unique. */
    return (1);
}

/* Make sure no host is listed twice in the list. */
static int
qlist_one_per_host(QueueList *qlist)
{
    QueueList *q1, *q2;

    for (q1 = qlist; q1 != NULL; q1 = q1->next) {
	for (q2 = qlist; q2 != NULL; q2 = q2->next) {
	    if (q1 == q2)
		continue;

	    if (strcmp(q1->queue->exechost, q2->queue->exechost) == 0)
		return (0);
	}
    }
    return (1);		/* No host mentioned more than once. */
}

static int
get_variance(char *string, int *lowp, int *highp)
{
/*    char   *id = "get_variance"; */
    char   *ptr, *buf, *end, sign;
    long    n;
    int     i, low = -1, high = -1;

    if ((string == NULL) || ((buf = schd_strdup(string)) == NULL))
	return (-1);

    ptr = strtok(buf, ",");

    while (ptr != NULL) {
	/* Ensure that the string matches '{+-}[0-9][0-9]*%'. */

	sign = *ptr;
	if ((sign != '+') && (sign != '-'))
	    goto parse_error;

	ptr++;

	if ((*ptr < '0') || (*ptr > '9'))
	    goto parse_error;

	n = strtol(ptr, &end, 10);
	if (n > INT_MAX)
	    goto parse_error;
	i = (int)n;
	
	if (*end != '%')
	    goto parse_error;

	if (sign == '-') {
	    if (low >= 0)		/* Already set. */
		goto parse_error;
	    else
		low = i;
	} else {
	    if (high >= 0)		/* Already set. */
		goto parse_error;
	    else
		high = i;
	}

	ptr = strtok(NULL, ",");
    }

    free(buf);

    *lowp  = (low >= 0) ? low : 0;
    *highp = (high >= 0) ? high : 0;

    return (0);

parse_error:
    free(buf);
    return (-1);
}
