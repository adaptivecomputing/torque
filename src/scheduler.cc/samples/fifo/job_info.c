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
#include <errno.h>
#include "pbs_ifl.h"
#include "queue_info.h"
#include "job_info.h"
#include "constant.h"
#include "misc.h"
#include "config.h"
#include "globals.h"
#include "fairshare.h"
#include "node_info.h"



/*
 *
 * query_jobs - create an array of jobs in a specified queue
 *
 *   pbs_sd - connection to pbs_server
 *   qinfo  - queue to get jobs from
 *
 * returns pointer to the head of a list of jobs
 *
 */
job_info **query_jobs(int pbs_sd, queue_info *qinfo)
  {
  /* pbs_selstat() takes a linked list of attropl structs which tell it
   * what information about what jobs to return.  We want all jobs which are
   * in a specified queue
   */

  struct attropl opl =
    {
    NULL, ATTR_q, NULL, NULL, EQ
    };

  /* linked list of jobs returned from pbs_selstat() */

  struct batch_status *jobs;

  /* current job in jobs linked list */

  struct batch_status *cur_job;

  /* array of internal scheduler structures for jobs */
  job_info **jinfo_arr;

  /* current job in jinfo_arr array */
  job_info *jinfo;

  /* number of jobs in jinfo_arr */
  int num_jobs = 0;
  int i;

  opl.value = qinfo -> name;

  if ((jobs = pbs_selstat(pbs_sd, &opl, NULL)) == NULL)
    {
    if (pbs_errno > 0)
      fprintf(stderr, "pbs_selstat failed: %d\n", pbs_errno);

    return NULL;
    }

  cur_job = jobs;

  while (cur_job != NULL)
    {
    num_jobs++;
    cur_job = cur_job -> next;
    }

  /* allocate enough space for all the jobs and the NULL sentinal */
  if ((jinfo_arr = (job_info **) malloc(sizeof(jinfo) * (num_jobs + 1))) == NULL)
    {
    perror("Memory allocation error");
    pbs_statfree(jobs);
    return NULL;
    }

  cur_job = jobs;

  for (i = 0; cur_job != NULL; i++)
    {
    if ((jinfo = query_job_info(cur_job, qinfo)) == NULL)
      {
      pbs_statfree(jobs);
      free_jobs(jinfo_arr);
      return NULL;
      }

    /* get the fair share group info node */
    if (jinfo -> account != NULL)
      jinfo -> ginfo = find_alloc_ginfo(jinfo -> account);

    /* if the job is not in the queued state, don't even allow
     * it to be considered to be run.
     */
    if (!jinfo -> is_queued)
      jinfo -> can_not_run = 1;

    jinfo_arr[i] = jinfo;

    cur_job = cur_job -> next;
    }

  jinfo_arr[i] = NULL;

  pbs_statfree(jobs);

  return jinfo_arr;
  }

/*
 *
 * query_job_info - takes info from a batch_status about a job and
 *    converts it into a job_info struct
 *
 *   job - batch_status struct of job
 *   qinfo - queue where job resides
 *
 * returns job_info struct
 */

job_info *query_job_info(struct batch_status *job, queue_info *queue)
  {
  job_info *jinfo;  /* converted job */

  struct attrl *attrp;  /* list of attributes returned from server */
  int count;   /* int used in string -> int conversion */
  char *endp;   /* used for strtol() */
  resource_req *resreq;  /* resource_req list for resources requested  */

  if ((jinfo = new_job_info()) == NULL)
    return NULL;

  jinfo -> name = string_dup(job -> name);

  attrp = job -> attribs;

  jinfo -> queue = queue;

  while (attrp != NULL)
    {
    if (!strcmp(attrp -> name, ATTR_p))   /* priority */
      {
      count = strtol(attrp -> value, &endp, 10);

      if (*endp != '\n')
        jinfo -> priority = count;
      else
        jinfo -> priority = -1;
      }
    else if (!strcmp(attrp -> name, ATTR_qtime))  /* queue time */
      {
      count = strtol(attrp -> value, &endp, 10);

      if (*endp != '\n')
        jinfo -> qtime = count;
      else
        jinfo -> qtime = -1;
      }
    else if (!strcmp(attrp -> name, ATTR_state))   /* state of job */
      set_state(attrp -> value, jinfo);
    else if (!strcmp(attrp -> name, ATTR_comment))   /* job comment */
      jinfo -> comment = string_dup(attrp -> value);
    else if (!strcmp(attrp -> name, ATTR_euser))    /* account name */
      jinfo -> account = string_dup(attrp -> value);
    else if (!strcmp(attrp -> name, ATTR_egroup))    /* group name */
      jinfo -> group = string_dup(attrp -> value);
    else if (!strcmp(attrp -> name, ATTR_exechost))    /* where job is running*/
      jinfo -> job_node = find_node_info(attrp -> value,
                                         queue -> server -> nodes);
    else if (!strcmp(attrp -> name, ATTR_l))    /* resources requested*/
      {
      resreq = find_alloc_resource_req(attrp -> resource, jinfo -> resreq);

      if (resreq != NULL)
        {
        resreq -> res_str = string_dup(attrp -> value);
        resreq -> amount = res_to_num(attrp -> value);
        }

      if (jinfo -> resreq == NULL)
        jinfo -> resreq = resreq;
      }
    else if (!strcmp(attrp -> name, ATTR_used))    /* resources used */
      {
      resreq = find_alloc_resource_req(attrp -> resource, jinfo -> resused);

      if (resreq != NULL)
        resreq -> amount = res_to_num(attrp -> value);

      if (jinfo -> resused == NULL)
        jinfo -> resused = resreq;
      }

    attrp = attrp -> next;
    }

  return jinfo;
  }

/*
 *
 * new_job_info  - allocate and initialize new job_info structure
 *
 * returns new job_info structure
 *
 */
job_info *new_job_info()
  {
  job_info *jinfo;

  if ((jinfo = (job_info *) malloc(sizeof(job_info))) == NULL)
    return NULL;

  jinfo -> is_queued = 0;

  jinfo -> is_running = 0;

  jinfo -> is_held = 0;

  jinfo -> is_waiting = 0;

  jinfo -> is_transit = 0;

  jinfo -> is_exiting = 0;

  jinfo -> is_suspended = 0;

  jinfo -> is_completed = 0;

  jinfo -> is_starving = 0;

  jinfo -> can_not_run = 0;

  jinfo -> can_never_run = 0;

  jinfo -> name = NULL;

  jinfo -> comment = NULL;

  jinfo -> account = NULL;

  jinfo -> group = NULL;

  jinfo -> queue = NULL;

  jinfo -> priority = 0;

  jinfo -> sch_priority = 0;

  jinfo -> qtime = 0;

  jinfo -> resreq = NULL;

  jinfo -> resused = NULL;

  jinfo -> ginfo = NULL;

  jinfo -> job_node = NULL;

  return jinfo;
  }

/*
 *
 * new_resource_req - allocate and initalize new resoruce_req
 *
 * returns the new resource_req
 *
 */

resource_req *new_resource_req()
  {
  resource_req *resreq;

  if ((resreq = (resource_req *) malloc(sizeof(resource_req))) == NULL)
    return NULL;

  resreq -> name = NULL;

  resreq -> res_str = NULL;

  resreq -> amount = 0;

  resreq -> next = NULL;

  return resreq;
  }

/*
 *
 *  find_alloc_resource_req - find resource_req by name or allocate
 *      and initalize a new resource_req
 *      also adds new one to the list
 *
 *   name - resource_req to find
 *   reqlist - list to look through
 *
 * returns found or newly allocated resource_req
 *
 */
resource_req *find_alloc_resource_req(char *name, resource_req *reqlist)
  {
  resource_req *resreq;  /* used to find or create resource_req */
  resource_req *prev = NULL; /* previous resource_req in list */

  resreq = reqlist;

  while (resreq != NULL && strcmp(resreq -> name, name))
    {
    prev = resreq;
    resreq = resreq -> next;
    }

  if (resreq == NULL)
    {
    if ((resreq = new_resource_req()) == NULL)
      return NULL;

    resreq -> name = string_dup(name);

    if (prev != NULL)
      prev -> next = resreq;
    }

  return resreq;
  }

/*
 *
 * free_job_info - free all the memory used by a job_info structure
 *
 *   jinfo - the job_info to free
 *
 * returns nothing
 *
 */

void free_job_info(job_info *jinfo)
  {
  if (jinfo -> name != NULL)
    free(jinfo -> name);

  if (jinfo -> comment != NULL)
    free(jinfo -> comment);

  if (jinfo -> account != NULL)
    free(jinfo -> account);

  if (jinfo -> group != NULL)
    free(jinfo -> group);

  free_resource_req_list(jinfo -> resreq);

  free_resource_req_list(jinfo -> resused);

  free(jinfo);
  }

/*
 *
 * free_jobs - free an array of jobs
 *
 *   jarr - array of jobs to free
 *
 * returns nothing
 *
 */

void free_jobs(job_info **jarr)
  {
  int i;

  if (jarr == NULL)
    return;

  for (i = 0; jarr[i] != NULL; i++)
    free_job_info(jarr[i]);

  free(jarr);
  }

/*
 *
 * find_resource_req - find a resource_req from a resource_req list
 *
 *   reqlist - the resource_req list
 *   name - resoruce to look for
 *
 * returns found resource_req or NULL
 *
 */
resource_req *find_resource_req(resource_req *reqlist, const char *name)
  {
  resource_req *resreq;

  resreq = reqlist;

  while (resreq != NULL && strcmp(resreq -> name, name))
    resreq = resreq -> next;

  return resreq;
  }

/*
 *
 * free_resource_req_list - frees memory used by a resource_req list
 *
 *   list - resource_req list
 *
 * returns nothing
 */
void free_resource_req_list(resource_req *list)
  {
  resource_req *resreq, *tmp;

  resreq = list;

  while (resreq != NULL)
    {
    tmp = resreq;
    resreq = resreq -> next;

    if (tmp -> name != NULL)
      free(tmp -> name);

    if (tmp -> res_str != NULL)
      free(tmp -> res_str);

    free(tmp);
    }
  }

/*
 *
 * print_job_info - print out a job_info struct
 *
 *   jinfo - the job to print
 *   brief - only print job name
 *
 * returns nothing
 *
 */
void print_job_info(job_info *jinfo, char brief)
  {
  resource_req *resreq;   /* used to print the resources */

  if (jinfo == NULL)
    return;

  if (jinfo -> name != NULL)
    printf("%sJob Name: %s\n", brief ? "      " : "", jinfo -> name);

  if (!brief)
    {
    if (jinfo -> comment != NULL)
      printf("comment: %s\n", jinfo -> comment);

    if (jinfo -> queue != NULL)
      printf("queue: %s\n", jinfo -> queue -> name);

    if (jinfo -> job_node)
      printf("node: %s\n", jinfo -> job_node -> name);

    if (jinfo -> account)
      printf("account: %s\n", jinfo -> account);

    if (jinfo -> group)
      printf("group: %s\n", jinfo -> group);

    printf("priority: %d\n", jinfo -> priority);

    printf("sch_priority: %d\n", jinfo -> sch_priority);

    printf("qtime: %d: %s", (int)jinfo -> qtime, ctime(&(jinfo -> qtime)));


    printf("is_queued: %s\n", jinfo -> is_queued ? "TRUE" : "FALSE");

    printf("is_running: %s\n", jinfo -> is_running ? "TRUE" : "FALSE");

    printf("is_held: %s\n", jinfo -> is_held ? "TRUE" : "FALSE");

    printf("is_waiting: %s\n", jinfo -> is_waiting ? "TRUE" : "FALSE");

    printf("is_transit: %s\n", jinfo -> is_transit ? "TRUE" : "FALSE");

    printf("is_exiting: %s\n", jinfo -> is_exiting ? "TRUE" : "FALSE");

    printf("is_completed: %s\n", jinfo -> is_completed ? "TRUE" : "FALSE");

    printf("is_starving: %s\n", jinfo -> is_starving ? "TRUE" : "FALSE");

    printf("can_not_run: %s\n", jinfo -> can_not_run ? "TRUE" : "FALSE");

    printf("can_never_run: %s\n", jinfo -> can_never_run ? "TRUE" : "FALSE");

    resreq = jinfo -> resreq;

    while (resreq != NULL)
      {
      printf("resreq %s %ld\n", resreq -> name, resreq -> amount);
      resreq = resreq -> next;
      }
    }
  }


/*
 *
 * set_state - set the state flag in a job_info structure
 *   i.e. the is_* bit
 *
 *   state - the state
 *   jinfo - the job info structure
 *
 * returns nothing
 *
 */
void set_state(char *state, job_info *jinfo)
  {
  switch (state[0])
    {

    case 'Q':
      jinfo -> is_queued = 1;
      break;

    case 'R':
      jinfo -> is_running = 1;
      break;

    case 'T':
      jinfo -> is_transit = 1;
      break;

    case 'H':
      jinfo -> is_held = 1;
      break;

    case 'W':
      jinfo -> is_waiting = 1;
      break;

    case 'E':
      jinfo -> is_exiting = 1;
      break;

    case 'S':
      jinfo -> is_suspended = 1;
      break;

    case 'C':
      jinfo -> is_completed = 1;
      break;
    }
  }

/*
 *
 * update_job_on_run - update job information kept in a job_info
 *    when a job is run
 *
 *   pbs_sd - socket connection to pbs_server
 *   jinfo - the job to update
 *
 * returns nothing
 *
 */
void update_job_on_run(int pbs_sd, job_info *jinfo)
  {
  jinfo -> is_queued = 0;
  jinfo -> is_running = 1;
  }

/*
 *
 * update_job_comment - update a jobs comment attribute
 *
 *   pbs_sd - connection to the pbs_server
 *   jinfo  - job to update
 *   comment - the comment string
 *
 * returns
 *   0: comment needed updating
 *   non-zero: the comment did not need to be updated (same as before etc)
 *
 */

#include <string.h>

#define UDC_CLONEBUFSIZ 256

int update_job_comment(int pbs_sd, job_info *jinfo, char *comment)
  {
  /* the pbs_alterjob() call takes a linked list of attrl structures to alter
   * a job.  All we are interested in doing is changing the comment.
   */

  struct attrl attr =
    {
    NULL, ATTR_comment, NULL, NULL, 0
    };

  static char clone[UDC_CLONEBUFSIZ];
  
  if (jinfo == NULL)
    return 1;

  /* no need to update the job comment if it is the same */
  if (jinfo -> comment == NULL || strcmp(jinfo -> comment, comment))
    {
    if (jinfo -> comment != NULL)
      free(jinfo -> comment);

    jinfo -> comment = string_dup(comment);

    /* Assign new, copied version of "comment" to a pointer we can guarantee
       pbs_alterjob() can't mangle anything important. */

    strncpy(clone, comment, (size_t) (UDC_CLONEBUFSIZ - 1));

    clone[UDC_CLONEBUFSIZ - 1] = '\0';

    attr.value = clone;

    pbs_alterjob(pbs_sd, jinfo -> name, &attr, NULL);

    return 0;
    }

  return 1;
  }

/*
 *
 * update_jobs_cant_run - update an array of jobs which can not run
 *
 *   pbs_sd - connection to the PBS server
 *   jinfo_arr - the array to update
 *   start - the job which couldn't run
 *   comment - the comment to update
 *
 * returns nothing;
 *
 */
void update_jobs_cant_run(int pbs_sd, job_info **jinfo_arr, job_info *start,
                          char *comment, int start_where)
  {
  int i = 0;

  if (jinfo_arr != NULL)
    {
    /* We are not starting at the front of the array, so we need to find the
     * element to start with.
     */
    if (start != NULL)
      {
      for (; jinfo_arr[i] != NULL && jinfo_arr[i] != start; i++)
        ;
      }
    else
      i = 0;

    if (jinfo_arr[i] != NULL)
      {
      if (start_where == START_BEFORE_JOB)
        i--;
      else if (start_where == START_AFTER_JOB)
        i++;

      for (; jinfo_arr[i] != NULL; i++)
        {
        jinfo_arr[i] -> can_not_run = 1;
        update_job_comment(pbs_sd, jinfo_arr[i], comment);
        }
      }
    }
  }

/*
 *
 * job_filter - filters jobs on specified argument
 *
 *   jobs - array of jobs to filter through
 *   size - amount of jobs in array
 *   filter_func - pointer to a function that will filter
 *  - returns 1: job will be added to new array
 *  - returns 0: job will not be added to new array
 *   arg - an extra arg to pass to filter_func
 *
 * returns pointer to filtered list
 *
 * NOTE: this function allocates a new array
 *
 * filter_func prototype: int func( job_info *, void * )
 *
 */
job_info **job_filter(job_info** jobs, int size,
                      int (*filter_func)(job_info*, void*), void *arg)
  {
  job_info **new_jobs = NULL;   /* new array of jobs */
  int i, j = 0;

  if ((new_jobs = malloc((size + 1) * sizeof(job_info *))) == NULL)
    {
    perror("Memory allocation error");
    return NULL;
    }

  for (i = 0; i < size; i++)
    {
    if (filter_func(jobs[i], arg))
      {
      new_jobs[j] = jobs[i];
      j++;
      }
    }

  if ((new_jobs = realloc(new_jobs, (j + 1) * sizeof(job_info *))) == NULL)
    {
    perror("Memory Allocation Error");
    return NULL;
    }

  new_jobs[j] = NULL;

  return new_jobs;
  }

/*
 *
 * translate_job_fail_code - translate the failure code of
 *    is_ok_to_run_job into a comment and log message
 *
 *   fail_code        - return code from is_ok_to_run_job()
 *   OUT: comment_msg - translated comment
 *   OUT: log_msg     - translated log message
 *
 * returns
 *    1: comment and log messages were set
 *    0: comment and log messages were not set
 *
 */
int translate_job_fail_code(int fail_code, char *comment_msg, char *log_msg)
  {
  int rc = 1;

  if (fail_code < num_res)
    {
    strcpy(comment_msg, res_to_check[fail_code].comment_msg);
    strcpy(log_msg, res_to_check[fail_code].debug_msg);
    }
  else
    {
    switch (fail_code)
      {

      case QUEUE_JOB_LIMIT_REACHED:
        strcpy(comment_msg, COMMENT_QUEUE_JOB_LIMIT);
        strcpy(log_msg, INFO_QUEUE_JOB_LIMIT);
        break;

      case SERVER_JOB_LIMIT_REACHED:
        strcpy(comment_msg, COMMENT_SERVER_JOB_LIMIT);
        strcpy(log_msg, INFO_SERVER_JOB_LIMIT);
        break;

      case SERVER_USER_LIMIT_REACHED:
        strcpy(comment_msg, COMMENT_SERVER_USER_LIMIT);
        strcpy(log_msg, INFO_SERVER_USER_LIMIT);
        break;

      case QUEUE_USER_LIMIT_REACHED:
        strcpy(comment_msg, COMMENT_QUEUE_USER_LIMIT);
        strcpy(log_msg, INFO_QUEUE_USER_LIMIT);
        break;

      case QUEUE_GROUP_LIMIT_REACHED:
        strcpy(comment_msg, COMMENT_QUEUE_GROUP_LIMIT);
        strcpy(log_msg, INFO_QUEUE_GROUP_LIMIT);
        break;

      case SERVER_GROUP_LIMIT_REACHED:
        strcpy(comment_msg, COMMENT_SERVER_GROUP_LIMIT);
        strcpy(log_msg, INFO_SERVER_GROUP_LIMIT);
        break;

      case CROSS_DED_TIME_BOUNDRY:
        strcpy(comment_msg, COMMENT_CROSS_DED_TIME);
        strcpy(log_msg, INFO_CROSS_DED_TIME);
        break;

      case NO_AVAILABLE_NODE:
        strcpy(comment_msg, COMMENT_NO_AVAILABLE_NODE);
        strcpy(log_msg, INFO_NO_AVAILABLE_NODE);
        break;

      case NOT_QUEUED:
        /* do we really care if the job is not in a queued state?  there is
         * no way it'll be run in this state, why spam the log about it
         */
        log_msg[0] = '\0';
        comment_msg[0] = '\0';
        rc = 0;
        break;

      case NOT_ENOUGH_NODES_AVAIL:
        strcpy(comment_msg, COMMENT_NOT_ENOUGH_NODES_AVAIL);
        strcpy(log_msg, INFO_NOT_ENOUGH_NODES_AVAIL);
        break;

      case JOB_STARVING:
        strcpy(comment_msg, COMMENT_JOB_STARVING);
        sprintf(log_msg, INFO_JOB_STARVING, cstat.starving_job -> name);
        break;

      case SCHD_ERROR:
        strcpy(comment_msg, COMMENT_SCHD_ERROR);
        strcpy(log_msg, INFO_SCHD_ERROR);
        break;

      case SERVER_TOKEN_UTILIZATION:
        strcpy(comment_msg, COMMENT_TOKEN_UTILIZATION);
        sprintf(log_msg, INFO_TOKEN_UTILIZATION);
        break;

      default:
        rc = 0;
        comment_msg[0] = '\0';
        log_msg[0] = '\0';
      }
    }

  return rc;
  }

/*
 *
 * calc_assn_resource - calcualte the assigned resource in a job array
 *
 *   jinfo_arr - array of jobs
 *   resstr    - resource to calculate
 *
 * returns the calculated resource
 *  -1 on error
 *
 */
int calc_assn_resource(job_info **jinfo_arr, char *resstr)
  {
  resource_req *req;
  int res_amm = 0;
  int i;

  if (resstr == NULL || jinfo_arr == NULL)
    return -1;

  for (i = 0; jinfo_arr[i] != NULL; i++)
    {
    req = find_resource_req(jinfo_arr[i] -> resreq, resstr);

    if (req != NULL)
      res_amm += req -> amount;
    }

  return res_amm;
  }
