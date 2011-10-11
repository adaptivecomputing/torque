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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "pbs_error.h"
#include "pbs_ifl.h"
#include "log.h"
#include "queue_info.h"
#include "constant.h"
#include "misc.h"
#include "check.h"
#include "config.h"
#include "globals.h"


/*
 *
 * query_queues - creates an array of queue_info structs which contain
 *   an array of jobs
 *
 *   pbs_sd - connection to the pbs_server
 *   sinfo  - server to query queues from
 *
 * returns pointer to the head of the queue structure
 *
 */
queue_info **query_queues(int pbs_sd, server_info *sinfo)
  {
  /* the linked list of queues returned from the server */

  struct batch_status *queues;

  /* the current queue in the linked list of queues */

  struct batch_status *cur_queue;

  /* array of pointers to internal scheduling structure for queues */
  queue_info **qinfo_arr;

  /* the current queue we are working on */
  queue_info *qinfo;

  /* return code */
  int ret;

  /* buffer to store comment message */
  char comment[MAX_COMMENT_SIZE];

  /* buffer to store log message */
  char log_msg[MAX_LOG_SIZE];

  /* used to cycle through jobs to calculate assigned resources */
  resource *res;

  int i;
  int num_queues = 0;
  int local_errno = 0;

  /* get queue info from PBS server */

  if ((queues = pbs_statque_err(pbs_sd, NULL, NULL, NULL, &local_errno)) == NULL)
    {
    fprintf(stderr, "Statque failed: %d\n", local_errno);
    return NULL;
    }

  cur_queue = queues;

  while (cur_queue != NULL)
    {
    num_queues++;
    cur_queue = cur_queue -> next;
    }

  if ((qinfo_arr = (queue_info **) malloc(sizeof(queue_info *) * (num_queues + 1))) == NULL)
    {
    perror("Memory Allocation error");
    pbs_statfree(queues);
    return NULL;
    }

  cur_queue = queues;

  for (i = 0; cur_queue != NULL; i++)
    {
    /* convert queue information from batch_status to queue_info */
    if ((qinfo = query_queue_info(cur_queue, sinfo)) == NULL)
      {
      pbs_statfree(queues);
      free_queues(qinfo_arr, 1);
      return NULL;
      }

    /* get all the jobs which reside in the queue */
    qinfo -> jobs = query_jobs(pbs_sd, qinfo);

    /* check if the queue is a dedicated time queue */
    if (conf.ded_prefix[0] != '\0')
      if (!strncmp(qinfo -> name, conf.ded_prefix, strlen(conf.ded_prefix)))
        qinfo -> dedtime_queue = 1;

    /* check if it is OK for jobs to run in the queue */
    ret = is_ok_to_run_queue(qinfo);

    if (ret == SUCCESS)
      qinfo -> is_ok_to_run = 1;
    else
      {
      qinfo -> is_ok_to_run = 0;
      translate_queue_fail_code(ret, comment, log_msg);
      sched_log(PBSEVENT_DEBUG2, PBS_EVENTCLASS_QUEUE, qinfo -> name, log_msg);
      update_jobs_cant_run(pbs_sd, qinfo -> jobs, NULL, comment, START_WITH_JOB);
      }

    count_states(qinfo -> jobs, &(qinfo -> sc));

    qinfo -> running_jobs = job_filter(qinfo -> jobs, qinfo -> sc.total, check_run_job, NULL);

    res = qinfo -> qres;

    while (res != NULL)
      {
      if (res -> assigned == UNSPECIFIED)
        res -> assigned = calc_assn_resource(qinfo -> running_jobs, res-> name);

      res = res -> next;
      }

    qinfo_arr[i] = qinfo;

    cur_queue = cur_queue -> next;
    }

  qinfo_arr[i] = NULL;

  pbs_statfree(queues);

  return qinfo_arr;
  }

/*
 *
 * query_queue_info - collects information from a batch_status and
 *      puts it in a queue_info struct for easier access
 *
 *   queue - batch_status struct to get queue information from
 *   sinfo - server where queue resides
 *
 * returns newly allocated and filled queue_info or NULL on error
 *
 */

queue_info *query_queue_info(struct batch_status *queue, server_info *sinfo)
  {

  struct attrl *attrp;  /* linked list of attributes from server */

  struct queue_info *qinfo; /* queue_info being created */
  resource *resp;  /* resource in resource qres list */
  char *endp;   /* used with strtol() */
  int count;   /* used to convert string -> integer */

  if ((qinfo = new_queue_info()) == NULL)
    return NULL;

  attrp = queue -> attribs;

  qinfo -> name = string_dup(queue -> name);

  qinfo -> server = sinfo;

  while (attrp != NULL)
    {
    if (!strcmp(attrp -> name, ATTR_start))   /* started */
      {
      if (!strcmp(attrp -> value, "True"))
        qinfo -> is_started = 1;
      }
    else if (!strcmp(attrp -> name, ATTR_maxrun))  /* max_running */
      {
      count = strtol(attrp -> value, &endp, 10);

      if (*endp != '\0')
        count = -1;

      qinfo -> max_run = count;
      }
    else if (!strcmp(attrp -> name, ATTR_maxuserrun))  /* max_user_run */
      {
      count = strtol(attrp -> value, &endp, 10);

      if (*endp != '\0')
        count = -1;

      qinfo -> max_user_run = count;
      }
    else if (!strcmp(attrp -> name, ATTR_maxgrprun))  /* max_group_run */
      {
      count = strtol(attrp -> value, &endp, 10);

      if (*endp != '\0')
        count = -1;

      qinfo -> max_group_run = count;
      }
    else if (!strcmp(attrp -> name, ATTR_p))    /* priority */
      {
      count = strtol(attrp -> value, &endp, 10);

      if (*endp != '\0')
        count = -1;

      qinfo -> priority = count;
      }
    else if (!strcmp(attrp -> name, ATTR_qtype))  /* queue_type */
      {
      if (!strcmp(attrp -> value, "Execution"))
        {
        qinfo -> is_exec = 1;
        qinfo -> is_route = 0;
        }
      else if (!strcmp(attrp -> value, "Route"))
        {
        qinfo -> is_route = 1;
        qinfo -> is_exec = 0;
        }
      }
    else if (!strcmp(attrp -> name, ATTR_rescavail))    /* resources_available*/
      {
      count = res_to_num(attrp -> value);
      resp = find_alloc_resource(qinfo -> qres, attrp -> resource);

      if (qinfo -> qres == NULL)
        qinfo -> qres = resp;

      if (resp != NULL)
        resp -> avail = count;
      }
    else if (!strcmp(attrp -> name, ATTR_rescmax))      /* resources_max */
      {
      count = res_to_num(attrp -> value);
      resp = find_alloc_resource(qinfo -> qres, attrp -> resource);

      if (qinfo -> qres == NULL)
        qinfo -> qres = resp;

      if (resp != NULL)
        resp -> max = count;
      }
    else if (!strcmp(attrp -> name, ATTR_rescassn))     /* resources_assigned */
      {
      count = res_to_num(attrp -> value);
      resp = find_alloc_resource(qinfo -> qres, attrp -> resource);

      if (qinfo -> qres == NULL)
        qinfo -> qres = resp;

      if (resp != NULL)
        resp -> assigned = count;
      }

    attrp = attrp -> next;
    }

  return qinfo;
  }

/*
 *
 * new_queue_info - allocate and initalize a new queue_info struct
 *
 * returns the newly allocated struct or NULL on error
 *
 */

queue_info *new_queue_info()
  {
  queue_info *qinfo;

  if ((qinfo = malloc(sizeof(queue_info))) == NULL)
    return NULL;

  qinfo -> is_started  = 0;

  qinfo -> is_exec  = 0;

  qinfo -> is_route  = 0;

  qinfo -> dedtime_queue = 0;

  qinfo -> is_ok_to_run  = 0;

  qinfo -> priority  = 0;

  init_state_count(&(qinfo -> sc));

  qinfo -> max_run  = INFINITY;

  qinfo -> max_user_run  = INFINITY;

  qinfo -> max_group_run = INFINITY;

  qinfo -> name   = NULL;

  qinfo -> qres   = NULL;

  qinfo -> jobs   = NULL;

  qinfo -> running_jobs  = NULL;

  qinfo -> server  = NULL;

  return qinfo;
  }

/*
 *
 * print_queue_info - print all information in a queue_info struct
 *
 *   qinfo - queue to print
 *   brief - only print queue name
 *   deep  - print jobs in queue also
 *
 * returns nothing
 *
 */
void print_queue_info(queue_info *qinfo, char brief, char deep)
  {
  job_info *jinfo;

  if (qinfo == NULL)
    return;

  if (qinfo -> name != NULL)
    printf("\n%sQueue name: %s\n", brief ? "    " : "", qinfo -> name);

  if (!brief)
    {
    printf("is_started: %s\n", qinfo -> is_started ? "TRUE" : "FALSE");
    printf("is_exec: %s\n", qinfo -> is_exec ? "TRUE" : "FALSE");
    printf("is_route: %s\n", qinfo -> is_route ? "TRUE" : "FALSE");
    printf("dedtime_queue: %s\n", qinfo -> dedtime_queue ? "TRUE" : "FALSE");
    printf("is_ok_to_run: %s\n", qinfo -> is_ok_to_run ? "TRUE" : "FALSE");
    printf("max_run: %d\n", qinfo -> max_run);
    printf("max_user_run: %d\n", qinfo -> max_user_run);
    printf("max_group_run: %d\n", qinfo -> max_group_run);
    printf("priority: %d\n", qinfo -> priority);
    print_state_count(&(qinfo -> sc));
    }

  if (deep)
    {
    jinfo = qinfo -> jobs[0];

    while (jinfo != NULL)
      {
      print_job_info(jinfo, brief);
      jinfo++;
      }
    }
  }

/*
 *
 * free_queues - free an array of queues
 *
 *   qarr - qinfo array to delete
 *   free_jobs_too - free the jobs in the queue also
 *
 * returns nothing
 *
 */

void free_queues(queue_info **qarr, char free_jobs_too)
  {
  int i;

  if (qarr == NULL)
    return;

  for (i = 0; qarr[i] != NULL; i++)
    {
    if (free_jobs_too)
      free_jobs(qarr[i] -> jobs);

    free_queue_info(qarr[i]);
    }

  free(qarr);

  }

/*
 *
 * update_queue_on_run - update the information kept in a qinfo structure
 *      when a job is run
 *
 *   qinfo - the queue to update
 *   jinfo - the job that was run
 *
 * returns nothing;
 *
 */
void update_queue_on_run(queue_info *qinfo, job_info *jinfo)
  {
  resource_req *resreq;
  resource *res;

  qinfo -> sc.running++;
  qinfo -> sc.queued--;

  resreq = jinfo -> resreq;

  while (resreq != NULL)
    {
    res = find_resource(qinfo -> qres, resreq -> name);

    if (res)
      res -> assigned += resreq -> amount;

    resreq = resreq -> next;
    }

  }

/*
 *
 * free_queue_info - free space used by a queue info struct
 *
 *   qinfo - queue to free
 *
 * returns nothing
 *
 */
void free_queue_info(queue_info *qinfo)
  {
  if (qinfo -> name != NULL)
    free(qinfo -> name);

  if (qinfo -> qres != NULL)
    free_resource_list(qinfo -> qres);

  if (qinfo -> running_jobs != NULL)
    free(qinfo -> running_jobs);

  free(qinfo);
  }

/*
 *
 *   translate_queue_fail_code - translate the failure code of
 *   is_ok_to_run_in_queue into log and comment_msg messages
 *
 *   pbs_sd           - communication descriptor to the pbs server
 *   fail_code        - failure code to translate
 *   OUT: comment_msg_msg - translated comment message
 *   OUT: log_msg     - translated log message
 *
 * returns nothing
 *
 */
void translate_queue_fail_code(int fail_code,
                               char *comment_msg, char *log_msg)
  {
  switch (fail_code)
    {

    case QUEUE_NOT_STARTED:
      strcpy(comment_msg, COMMENT_QUEUE_NOT_STARTED);
      strcpy(log_msg, INFO_QUEUE_NOT_STARTED);
      break;

    case QUEUE_NOT_EXEC:
      strcpy(comment_msg, COMMENT_QUEUE_NOT_EXEC);
      strcpy(log_msg, INFO_QUEUE_NOT_EXEC);
      break;

    case DED_TIME:
      strcpy(comment_msg, COMMENT_DED_TIME);
      strcpy(log_msg, INFO_DED_TIME);
      break;
      
    case QUEUE_IGNORED:
      strcpy(comment_msg, COMMENT_QUEUE_IGNORED);
      strcpy(log_msg, INFO_QUEUE_IGNORED);
      break;

    default:
      comment_msg[0] = '\0';
    }
  }
