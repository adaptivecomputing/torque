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
 * This file contains routines for support Fair Access to system, including
 * implementing a per-queue Shares or percentages of system resources, and
 * Maximum Running Jobs Limit
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

/* Scheduler header files */
#include "toolkit.h"
#include "gblxvars.h"
#include "msgs.h"

/* PBS header files */
#include "pbs_error.h"
#include "log.h"

/* extract and store the Fair Access Directives from a line just read from
 * the scheduler's configuration file.
 */
int 
arg_to_fairaccess(char *arg, char *sep, FairAccessList **fairacl_ptr)
{
    int     max_A=0, max_B=0, fieldcnt, found;
    char   *id = "arg_to_fairaccess";
    char   *field, aclname[30], group[30], entrytuple[60], scratch[15];
    AccessEntry *new, *FAptr=NULL, *fptr=NULL;

    /* first we process the configuration line passed in to use, saving
     * the important bits for later; at this point we don't know if we
     * have a new queue/arch entry, or simply an addition to an existing 
     * queue's FairAccess list.
     */
    fieldcnt = 0;
    for (field = strtok(arg, sep); field != NULL; field = strtok(NULL,sep)) {
	fieldcnt++;

	if (fieldcnt == 1 &&	/* first field on FAIR_ACCESS line */
  	   (strcmp(field, "ARCH"))) {
	    sprintf(log_buffer,
		"Unrecognized FAIR_ACCESS directive: %s", field);
	    log_record(PBSEVENT_SYSTEM, PBS_EVENTCLASS_SERVER, id, log_buffer);
	    return(-1);
	}
	else if (fieldcnt == 2)		/* architecture string */
	    strcpy(aclname, field);

	else if (fieldcnt == 3)		/* group ID */
	    strcpy(group, field);

	else if (fieldcnt == 4)		/* max CPUs per group */
	    max_A = atoi(field);

	else if (fieldcnt == 5)		/* max Memory (MB) per group */
	    max_B = atoi(field);
    }
    if (fieldcnt != 5) {
	sprintf(log_buffer,
	    "Incomplete FAIR_ACCESS directive: %s", arg);
	log_record(PBSEVENT_SYSTEM, PBS_EVENTCLASS_SERVER, id, log_buffer);
	return(-1);
    }
    if (max_A < 1) {
	sprintf(log_buffer,
	    "FAIR_ACCESS max CPUS (%d) should be greater than 1", max_A);
	log_record(PBSEVENT_SYSTEM, PBS_EVENTCLASS_SERVER, id, log_buffer);
	return(-1);
    }
    if (max_B < 1) {
	sprintf(log_buffer,
	    "FAIR_ACCESS max Memory (%d mb) should be greater than 1", max_B);
	log_record(PBSEVENT_SYSTEM, PBS_EVENTCLASS_SERVER, id, log_buffer);
	return(-1);
    }

    /*
     * Multiple lines may be used to add entries to the FairACL list. Find
     * the tail of the passed-in list (if there is one), and assign the
     * FAptr to the tail element.  Later, the new element will be hung off
     * FAptr's next field (or FAptr will be set to it.)
     */

    if (*fairacl_ptr == NULL) {
	*fairacl_ptr = (FairAccessList *)malloc(sizeof (FairAccessList));
	(*fairacl_ptr)->entry = NULL;
	(*fairacl_ptr)->next = NULL;
    }
    FAptr = (*fairacl_ptr)->entry;

    sprintf(entrytuple, "%s:%s", aclname, group);
    found=0;
    if (FAptr) {
	/* search for an existing entry for this queue/arch  */
	for (fptr = FAptr; fptr != NULL; fptr = fptr->next) {
	    if (!strcmp(fptr->name, entrytuple)) {
		found = 1;
		break; /* need to add a new ACL on the entry */
	    }
	}
	if (!found) {
	    for (fptr = FAptr; fptr->next != NULL; fptr = fptr->next)
	    /* Walk the list, looking for last element. */;
	}
    }

   /* if we found the entry, then we need add the new info to it;
    * otherwise, we first need to create a new entry struct, and
    * then add to it...
    */

    if (!found) {
        new = (AccessEntry *)malloc(sizeof (AccessEntry));
        if (new == NULL) {
            log_record(PBSEVENT_SYSTEM, PBS_EVENTCLASS_SERVER, id,
    	        "malloc(newAccessEntry)");
	    return(-1);
        }
        memset(new, 0, sizeof (AccessEntry));
        
        if (FAptr == NULL) {
            (*fairacl_ptr)->entry = new;
            fptr = (*fairacl_ptr)->entry;
        } else {
            fptr->next = new;
	    fptr = new;
        }
	new->name = schd_strdup(entrytuple);
    }
    
    /* Finially we get to fill it in with the info we saved earlier */
    new->max_cpu      = max_A;
    sprintf(scratch, "%dmb", max_B);
    new->max_mem      = schd_val2byte(scratch);

    return (0);
}

void schd_print_fairaccess(void)
{
    char  	   *id = "print_fairacl";
    FairAccessList *FALptr;
    AccessEntry    *AEptr;

    if (schd_FairACL) {

      sprintf(log_buffer,"FAIR_ACCESS: ARCH Directives:");
      log_record(PBSEVENT_SYSTEM, PBS_EVENTCLASS_SERVER, id, log_buffer);
      sprintf(log_buffer,"  %-17s %-8s %8s",
	"Arch:GroupID", "Max:CPUs", "Memory");
      log_record(PBSEVENT_SYSTEM, PBS_EVENTCLASS_SERVER, id, log_buffer);

      for (FALptr = schd_FairACL; FALptr != NULL; FALptr = FALptr->next) {
        for (AEptr = FALptr->entry; AEptr != NULL; AEptr = AEptr->next) {
          if (AEptr->name) {
	      sprintf(log_buffer,"  %-17s %6.0d %s",
	          AEptr->name, AEptr->max_cpu, schd_byte2val(AEptr->max_mem));
       	      log_record(PBSEVENT_SYSTEM, PBS_EVENTCLASS_SERVER, id,
	          log_buffer);
          }
        }
      }
    }
}

/* 
 * Check if running this job in a specific queue would exceed the
 * per-group architecture limits.
 */
int job_exceeds_accesslimits(Job *job, char *queue_arch, size_t arch_mem,
	int arch_cpu)
{
    FairAccessList *FALptr;
    AccessEntry    *AEptr;
    int		    default_cpu = 0, default_mem = 0;
    char	    search_name[MAX_TXT];

    if (job->arch)
        sprintf(search_name, "%s:%s", job->arch, job->group);
    else {
        if (queue_arch) 
            sprintf(search_name, "%s:%s", queue_arch, job->group);
	else
	    return(1);
    }

    for (FALptr = schd_FairACL; FALptr != NULL; FALptr = FALptr->next) {
        for (AEptr = FALptr->entry; AEptr != NULL;  AEptr = AEptr->next) {
            if (AEptr->name) {
                if (!strcmp(AEptr->name, search_name)) {
		    if (arch_cpu + job->ncpus > AEptr->max_cpu)
			return(2);
		    if (arch_mem + job->memory > AEptr->max_mem)
	                 return(3);
		    else
	                 return(0);
	        }
            }
        }
    }

    /* if we got here, then we didn't find an exact match. Search the
     * list again, this time looking for the default value.
     */
    if (job->arch)
        sprintf(search_name, "%s:default", job->arch);
    else {
        if (queue_arch) 
            sprintf(search_name, "%s:default", queue_arch);
    }

    for (FALptr = schd_FairACL; FALptr != NULL; FALptr = FALptr->next) {
        for (AEptr = FALptr->entry; AEptr != NULL;  AEptr = AEptr->next) {
            if (AEptr->name) {
                if (!strcmp(AEptr->name, search_name)) {
		    if (arch_cpu + job->ncpus > AEptr->max_cpu)
			return(2);
		    if (arch_mem + job->memory > AEptr->max_mem)
	                 return(3);
		    else
	                 return(0);
	        }
            }
        }
    }

    /* hummphf. the admin forgot to define a default. assume the worst.  */
    return (1);
}
