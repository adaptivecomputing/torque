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

/* Get all the jobs queued in the specified queue. */

#include <stdio.h>
#include <stdlib.h>

#include        "pbs_error.h"
#include        "pbs_ifl.h"
#include        "log.h"


extern int connector;

#include "toolkit.h"

Job *
schd_get_jobs(char *qname, char *state)
  {
  char   *id = "schd_get_jobs";
  int     idx;
  int     local_errno = 0;
  Job    *joblist = NULL, *jobtail = NULL, *new_joblist;
  Batch_Status *pbs_head, *pbs_ptr;
  AttrOpList *attr;

  static AttrOpList alist[] =
    {
      {NULL, NULL, NULL, NULL, EQ},
    {NULL, NULL, NULL, NULL, EQ}
    };

  if ((qname == NULL) && (state == NULL))
    {
    attr = NULL; /* Caller requested all jobs in all queues. */
    }
  else
    {
    /*
     * Initialize the search criteria since alist is a static struct
     * and it will retain the previous search when repeatedly called.
     */
    for (idx = 0; idx < (sizeof(alist) / sizeof(AttrOpList)); idx++)
      {
      alist[idx].next  = NULL;
      alist[idx].name  = NULL;
      alist[idx].value = NULL;
      }

    idx = 0;

    /* Was a specific queue requested? */

    if (qname != NULL)
      {
      alist[idx].name = ATTR_queue;
      alist[idx].value = qname;
      idx++;
      }

    /* Was a specific state requested? */
    if (state != NULL)
      {
      alist[idx].name = ATTR_state;
      alist[idx].value = state;

      if (idx > 0)
        alist[idx - 1].next = &alist[idx];

      idx++;
      }

    /* (More tests can be added here.) */

    attr = alist;
    }

  /* Ask PBS for the list of jobs requested */
  pbs_head = pbs_selstat_err(connector, attr, NULL, &local_errno);

  if ((pbs_head == NULL) && (local_errno))
    {
    (void)sprintf(log_buffer, "pbs_selstat failed, %d", local_errno);
    log_record(PBSEVENT_SYSTEM, PBS_EVENTCLASS_SERVER, id, log_buffer);
    return (NULL);
    }

  for (pbs_ptr = pbs_head; pbs_ptr != NULL; pbs_ptr = pbs_ptr->next)
    {
    /*
     * If there is no list yet, create one.  If there is already a list,
     * create a new element and place it after the current tail.  The new
     * element then becomes the tail.
     */
    new_joblist = (Job *)malloc(sizeof(Job));

    if (new_joblist == NULL)
      {
      log_record(PBSEVENT_SYSTEM, PBS_EVENTCLASS_SERVER, id,
                 "malloc(new Job)");
      /*
       * Free any allocated storage, set joblist to NULL, and break.
       * By doing this, the PBS batch_struct list will be freed,
       * and the NULL joblist returned to the caller.
       */

      if (joblist)
        schd_free_jobs(joblist);

      joblist = NULL;

      break;
      }

    new_joblist->next = NULL;

    if (!joblist)
      {
      joblist = new_joblist;
      jobtail = joblist;
      }
    else
      {
      jobtail->next = new_joblist;
      jobtail = jobtail->next;
      }

    /*
     * 'jobtail' now points to a newly-created Job at the end of the
     * list of jobs.  Call get_jobinfo() to fill it in with the contents
     * of this PBS batch_struct description.
     */

    schd_get_jobinfo(pbs_ptr, jobtail);
    }

  /*
   * We are left with a list of Job's that was created the from the list
   * of Batch_Structs we got from pbs_selstat().  The Job list should
   * contain everything we need to know about the jobs.  It is okay to
   * free the list returned by PBS, and return the list of Job's.
   */

  pbs_statfree(pbs_head);

  return (joblist);
  }
