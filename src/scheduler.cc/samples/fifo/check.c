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
#include "pbs_ifl.h"
#include "log.h"
#include <string.h>
#include "check.h"
#include "config.h"
#include "server_info.h"
#include "queue_info.h"
#include "job_info.h"
#include "misc.h"
#include "constant.h"
#include "globals.h"
#include "dedtime.h"
#include "token_acct.h"

/* Internal functions */
int check_server_max_run(server_info *sinfo);
int check_queue_max_run(queue_info *qinfo);
int check_ded_time_queue(queue_info *qinfo);
int check_node_availability(job_info *jinfo, node_info **ninfo_arr);
int check_starvation(job_info *jinfo);
int check_ded_time_boundry(job_info *jinfo);


/*
 *
 * is_ok_to_run_in_queue - check to see if jobs can be run in a queue
 *
 *   qinfo - queue info
 *
 * returns SUCCESS on success or failure code
 *
 * NOTE: This function will be run once per queue every scheduling cycle
 *
 */

int is_ok_to_run_queue(queue_info *qinfo)
  {
  int rc = UNSPECIFIED;  /* Return Code */

  if (qinfo -> is_exec)
    {
    if (qinfo -> is_started)
      {
      if (!(rc = check_ded_time_queue(qinfo)))
        {
        if (!(rc = check_ignored(qinfo)))
          {
          rc = SUCCESS;
          }
        }
      }
    else
      rc = QUEUE_NOT_STARTED;
    }
  else
    rc = QUEUE_NOT_EXEC;

  return rc;
  }

/*
 *
 * is_ok_to_run_job - check to see if the job can currently fit into the
 *    system limits
 *
 *   pbs_sd - the connection descriptor to the pbs_server
 *   sinfo - server info
 *   qinfo - queue info
 *   jinfo - job info
 *
 * returns  0 on success or failure code
 *
 *
 */
int is_ok_to_run_job(int pbs_sd, server_info *sinfo, queue_info *qinfo,
                     job_info *jinfo)
  {
  int rc;                       /* Return Code */

  if ((rc = check_server_max_run(sinfo)))
    return rc;

  if ((rc = check_server_max_user_run(sinfo, jinfo -> account)))
    return rc;

  if ((rc = check_server_max_group_run(sinfo, jinfo -> group)))
    return rc;

  if ((rc = check_queue_max_run(qinfo)))
    return rc;

  if ((rc = check_queue_max_user_run(qinfo, jinfo -> account)))
    return rc;

  if ((rc = check_queue_max_group_run(qinfo, jinfo -> group)))
    return rc;

  if ((rc = check_ded_time_boundry(jinfo)))
    return rc;

  if ((rc = check_starvation(jinfo)))
    return rc;

  if ((rc = check_nodes(pbs_sd, jinfo, sinfo -> timesharing_nodes)))
    return rc;

  if ((rc = check_avail_resources(qinfo -> qres, jinfo)) != SUCCESS)
    return rc;

  if ((rc = check_avail_resources(sinfo -> res, jinfo)) != SUCCESS)
    return rc;

  if ((rc = check_token_utilization(sinfo, jinfo)) != SUCCESS)
    return rc;

  return SUCCESS;
  }


/*
 *
 * check_server_max_user_run - check if the user is within server
 *     user run limits
 *
 *   sinfo - the server
 *   account - the account name
 *
 * returns
 *   0: if the user has not reached their limit
 *   SERVER_USER_LIMIT_REACHED: if the user has reached their limit
 *
 */
int check_server_max_user_run(server_info *sinfo, char *account)
  {
  if (sinfo -> max_user_run == INFINITY ||
      count_by_user(sinfo -> running_jobs, account) < sinfo -> max_user_run)
    return 0;

  return SERVER_USER_LIMIT_REACHED;
  }

/*
 *
 * check_queue_max_user_run - check if the user is within queue
 *     user run limits
 *
 *   qinfo - the queue
 *   account - the account name
 *
 * returns
 *   0: if the user is under their limit
 *   QUEUE_USER_LIMIT_REACHED: if the user has reached their limit
 *
 */
int check_queue_max_user_run(queue_info *qinfo, char *account)
  {
  if (qinfo -> max_user_run == INFINITY ||
      count_by_user(qinfo -> running_jobs, account) < qinfo -> max_user_run)
    return 0;

  return QUEUE_USER_LIMIT_REACHED;
  }

/*
 *
 * check_queue_max_group_run - check to see if the group is within their
 *     queue running limits
 *
 *   qinfo - the queue
 *   group - the groupname
 *
 * returns
 *   0 if the group is within limits
 *   QUEUE_GROUP_LIMIT_REACHED: if group is not wihtin limits
 *
 */
int check_queue_max_group_run(queue_info *qinfo, char *group)
  {
  if (qinfo -> max_group_run == INFINITY ||
      count_by_group(qinfo -> running_jobs, group) < qinfo -> max_group_run)
    return 0;

  return QUEUE_GROUP_LIMIT_REACHED;
  }

/*
 *
 * check_server_max_group_run - check to see if group is within their
 *         server running limits
 *
 *   sinfo - the server
 *   group - the groupname
 *
 * returns
 *   0 if the group is not at their server max_group_run limit
 *   SERVER_GROUP_LIMIT_REACHED : if the limit is reached
 *
 */
int check_server_max_group_run(server_info *sinfo, char *group)
  {
  if (sinfo -> max_group_run == INFINITY ||
      count_by_group(sinfo -> running_jobs, group) < sinfo -> max_group_run)
    return 0;

  return SERVER_GROUP_LIMIT_REACHED;
  }

/*
 *
 *      check_avail_resources - check if there is available resources to run
 *                              a job on the server
 *
 *        reslist   - resources list
 *        jinfo - job
 *
 *      returns NULL on success or failure code
 *
 *      NOTE: if a resource is not available it is skipped
 *            All resources which are checked are in the global variable
 *              res_to_check
 *
 */
int check_avail_resources(resource *reslist, job_info *jinfo)
  {
  /* The resource needs to be found on the server and the requested resource
   * needs to be found from the job, these pointers are used to store the
   * results
   */
  resource_req *resreq;
  resource *res;
  int ret_code = UNSPECIFIED;
  char done = 0;                        /* Are we done? */
  int avail;                            /* amount of available resource */
  int i;

  for (i = 0; (i < num_res) && !done; i++)
    {
    res = find_resource(reslist, res_to_check[i].name);
    resreq = find_resource_req(jinfo -> resreq, res_to_check[i].name);

    /* if either of these are NULL then the system admin did not set a maximum
     * default or avail for the resource.  Skip it and move on
     */

    if ((res == NULL || resreq == NULL))
      continue;
    else
      {
      avail = dynamic_avail(res);

      if (avail != INFINITY && avail < resreq -> amount)
        {
        /* An insuffient amount of a resource has been found. */
        done = 1;
        ret_code = i;
        }
      }
    }

  if (!done)
    ret_code = SUCCESS;

  return ret_code;
  }



/*
 *
 * dynamic_avail - find out how much of a resource is available on a
 *   server.  If the resources_available attribute is
 *   set, use that, else use resources_max.
 *
 *   res - the resource to check
 *
 * returns the available amount of the resource
 *
 */

sch_resource_t dynamic_avail(resource *res)
  {
  if (res -> max == INFINITY && res -> avail == UNSPECIFIED)
    return INFINITY;

  if (res -> avail == UNSPECIFIED)
    return res -> max;
  else
    return res -> avail - res -> assigned;
  }

/*
 *
 * count_by_user - count the amount of jobs a user has in a job array
 *
 *   jobs - job array
 *   user - the username
 *
 * returns the count
 *
 */
int count_by_user(job_info **jobs, char *user)
  {
  int count = 0;  /* the accumulator to count the user's jobs */
  int i;

  if (jobs != NULL)
    {
    for (i = 0; jobs[i] != NULL; i++)
      if (!strcmp(user, jobs[i] -> account))
        count++;
    }

  return count;
  }

/*
 *
 *
 * count_by_group - count number of jobs a group has in job array
 *
 *   jobs - array of jobs
 *   group - group name
 *
 * returns the count
 *
 */
int count_by_group(job_info **jobs, char *group)
  {
  int i;
  int count = 0;  /* an accumulator to count the group's jobs */

  if (jobs != NULL)
    {
    for (i = 0; jobs[i] != NULL; i++)
      {
      if (!strcmp(jobs[i] -> group, group))
        count++;
      }
    }

  return count;
  }

/*
 *
 * check_ded_time_boundry  - check to see if a job would cross into
 *        dedicated time
 *
 *   jinfo - the job
 *
 * returns 0: will not cross a ded time boundry
 *  CROSS_DED_TIME_BOUNDRY: will cross a ded time boundry
 *
 */
int check_ded_time_boundry(job_info *jinfo)
  {
  resource_req *res;  /* used to get jobs walltime request */
  sch_resource_t finish_time; /* the finish time of the job */

  if (conf.ded_time[0].from)
    {
    res = find_resource_req(jinfo -> resreq, "walltime");

    if (res != NULL)
      {
      finish_time = cstat.current_time + res -> amount;

      if (!cstat.is_ded_time && finish_time > conf.ded_time[0].from)
        return CROSS_DED_TIME_BOUNDRY;
      else if (cstat.is_ded_time && finish_time > conf.ded_time[0].to)
        return CROSS_DED_TIME_BOUNDRY;
      else
        return 0;
      }
    else
      /* no walltime found for the job.  We have to assume it will cross a
       * dedtime boundry
       */
      return CROSS_DED_TIME_BOUNDRY;
    }

  return 0;
  }

/*
 *
 * check_nodes - check to see if there is suficient nodes available to
 *        run a job.
 *
 *   pbs_sd - communication descriptor to the pbs server
 *   jinfo  - job to check
 *   ninfo_arr - the node array
 *
 * returns 0 if the job can run
 *  NOT_ENOUGH_NODES_AVAIL if the job can not run
 *  SCHD_ERROR on error
 *
 */
int check_nodes(int pbs_sd, job_info *jinfo, node_info **ninfo_arr)
  {
  resource_req *nodes;  /* pointer for the nodes resource requested */
  int av;   /* are the nodes available */
  int al, res, down;  /* unused variables passed into pbs_rescquery */
  char *node_str;  /* pointer to node string */
  char *tmp;   /* needed to make buf a char ** the call */
  char errbuf[256];
  int rc;   /* return code */

  nodes = find_resource_req(jinfo -> resreq, "nodes");

  if (nodes != NULL)
    {
    /* pbs_rescquery needs a char ** for its second argument.  Since buf is
     * on the stack &buf == buf.  The tmp variable is used to provide the
     * char **
     */

    /* 7 = strlen("nodes=")  + '\0' */
    if ((node_str = malloc(strlen(nodes -> res_str) + 7)) == NULL)
      return SCHD_ERROR;

    sprintf(node_str, "nodes=%s", nodes -> res_str);

    tmp = node_str;

    if ((rc = pbs_rescquery(pbs_sd, &tmp, 1, &av, &al, &res, &down)) != 0)
      {
      free(node_str);
      sprintf(errbuf, "pbs_resquery error: %d", rc);
      sched_log(PBSEVENT_SYSTEM, PBS_EVENTCLASS_NODE, jinfo -> name, errbuf);
      return SCHD_ERROR;
      }
    else
      {
      free(node_str);
      /* the requested nodes will never be available */

      if (av < 0)
        jinfo -> can_never_run = 1;

      if (av > 0)
        return 0;
      else
        return NOT_ENOUGH_NODES_AVAIL;
      }
    }

  return check_node_availability(jinfo, ninfo_arr);
  }

/*
 *
 *      check_node_availability - determine that there is a timesharing node
 *                          available to run the job
 *
 *        jinfo - the job to run
 *        ninfo_arr - the array of nodes to check in
 *
 *      returns
 *   0: if we are load balancing and there is a node available or
 *      we are not load balancing
 *   NO_AVAILABLE_NODE: if there is no node available to run the job
 *
 */
int check_node_availability(job_info *jinfo, node_info **ninfo_arr)
  {
  int rc = NO_AVAILABLE_NODE; /* return code */
  resource_req *req;  /* used to get resource values */
  sch_resource_t ncpus;  /* number of CPUS job requested */
  sch_resource_t mem;  /* amount of memory job requested */
  char *arch;   /* arch the job requested */
  char *host;   /* host name the job requested */
  int i;

  if (cstat.load_balancing || cstat.load_balancing_rr)
    {
    if (jinfo != NULL && ninfo_arr != NULL)
      {
      if ((req = find_resource_req(jinfo -> resreq, "ncpus")) == NULL)
        ncpus = 1;
      else
        ncpus = req -> amount;

      if ((req = find_resource_req(jinfo -> resreq, "mem")) == NULL)
        mem = 0;
      else
        mem = req -> amount;

      if ((req = find_resource_req(jinfo -> resreq, "arch")) == NULL)
        arch = NULL;
      else
        arch = req -> res_str;

      if ((req = find_resource_req(jinfo -> resreq, "host")) == NULL)
        host = NULL;
      else
        host = req -> res_str;

      for (i = 0; ninfo_arr[i] != NULL && rc != 0; i++)
        {
        if (ninfo_arr[i] -> is_free &&
            (host == NULL || !strcmp(host, ninfo_arr[i] -> name)) &&
            (arch == NULL || !strcmp(arch, ninfo_arr[i] -> arch)) &&
            mem <= ninfo_arr[i] -> physmem)
          {
          if (ninfo_arr[i] -> loadave + ncpus <= ninfo_arr[i] -> max_load)
            rc = 0;
          }
        }
      }
    else
      rc = 0;  /* no nodes */
    }
  else
    rc = 0;  /* we are not load balancing */

  return rc;
  }

/*
 *
 * check_ded_time_queue - check if it is the approprate time to run jobs
 *          in a dedtime queue
 *
 *   qinfo - the queue
 *
 * returns:
 *   0: if it is dedtime and qinfo is a dedtime queue or
 *      if it is not dedtime and qinfo is not a dedtime queue
 *
 *   DED_TIME: if jobs can not run in queue because of dedtime restrictions
 *
 */
int check_ded_time_queue(queue_info *qinfo)
  {
  int rc = 0;  /* return code */

  if (cstat.is_ded_time)
    {
    if (qinfo -> dedtime_queue)
      rc = 0;
    else
      rc = DED_TIME;
    }
  else
    {
    if (qinfo -> dedtime_queue)
      rc = DED_TIME;
    else
      rc = 0;
    }

  return rc;
  }

/*
 *
 * check_queue_max_run - check to see if queues max run has been reached
 *
 *   qinfo - the queue to check
 *
 * returns
 *   0    : limit has not been reached
 *   QUEUE_JOB_LIMIT_REACHED : limit has been reached
 *
 */
int check_queue_max_run(queue_info *qinfo)
  {
  if (qinfo->max_run == INFINITY)
    return 0;
  else if (qinfo -> max_run > qinfo -> sc.running)
    return 0;
  else
    return QUEUE_JOB_LIMIT_REACHED;
  }

/*
 *
 * check_server_max_run - check to see if the server max_running limit has
 *          been reached
 *
 *   sinfo - the server to check
 *
 * returns
 *   0: if the limit has not been reached
 *   SERVER_JOB_LIMIT_REACHED: if the limit has been reached
 *
 */
int check_server_max_run(server_info *sinfo)
  {
  if (sinfo -> max_run == INFINITY)
    return 0;
  else if (sinfo -> max_run > sinfo -> sc.running)
    return 0;
  else
    return SERVER_JOB_LIMIT_REACHED;
  }

/*
 *
 * check_starvation - if there is a starving job, don't allow jobs to run
 *      until the starving job runs.
 *
 *   jinfo - the current job to check
 *
 * returns
 *   0: if the job is the starving job or no jobs are starving
 *   JOB_STARVING: if it is not the starving job
 *
 */
int check_starvation(job_info *jinfo)
  {
  if (cstat.starving_job == NULL || cstat.starving_job == jinfo)
    return 0;
  else
    return JOB_STARVING;
  }

static token **token_used = NULL;
static token **token_pool = NULL;

/*
 *
 *   sum_tokens - Add to the list of tokens. If the type already exists add the count
 *                otherwise insert a new entry
 *
 *      this_token - token to add to the list
 *      token_list - list of tokens
 *
 */

void sum_tokens(token *this_token, token ***token_list)
  {
  int i;

  if (*token_list == NULL)
    {
    *token_list = (token**)malloc(sizeof(token *) * 1000);
    memset(*token_list, 0, sizeof(token *) * 1000);
    }

  for (i = 0; (*token_list)[i] != NULL; i++)
    {
    if (strcmp(this_token->identifier, (*token_list)[i]->identifier) == 0)
      {
      (*token_list)[i]->count += this_token->count;
      break;
      }
    }

  if ((*token_list)[i] == NULL)
    {
    (*token_list)[i] = (token*)malloc(sizeof(token));
    (*token_list)[i]->identifier = strdup(this_token->identifier);
    (*token_list)[i]->count = this_token->count;
    }
  }

/*
 *
 *   get_token_consumption - Create accouting record from list of tokens
 *
 *      buffer - string buffer for the accounting record output
 *      token_list - list of tokens to create record from
 *
 */


void get_token_consumption(char * buffer, token ***token_list)
  {
  int i = 0;
  char print_buffer[1024];

  memset(buffer, 0 , TOKEN_ACCT_MAX_RCD);

  if (*token_list != NULL)
    {

    for (i = 0; (*token_list)[i] != NULL; i++)
      {
      if (i > 0)
        buffer = strcat(buffer, ",");

      sprintf(print_buffer, "%s:%.2f", (*token_list)[i]->identifier, (*token_list)[i]->count);

      buffer = strcat(buffer, print_buffer);

      free((*token_list)[i]->identifier);

      free((*token_list)[i]);
      }

    free(*token_list);

    *token_list = NULL;
    }
  }

/*
 *
 *    check_token_utiliztion
 *
 *      sinfo - the current server state
 *
 *    returns
 *      0: If token utiliztion has not reach max
 *      SERVER_TOKEN_UTILIZATION if max token usage has been reached
 *
 */

int check_token_utilization(server_info *sinfo, job_info *jinfo)
  {
  resource_req *tokens_req;
  resource_req *running_job_tokens_req;

  token *running_job_tokens;
  token *job_tokens;
  token *server_tokens;

  job_info **job_ptr = 0;
  int i = 0;
  float count_used = 0.0;
  float max_count = 0.0;
  float count_requested = 0.0;

  char token_log_message[TOKEN_ACCT_MAX_RCD];

  int ret = SUCCESS;

  tokens_req = find_resource_req(jinfo -> resreq, ATTR_tokens);

  if (tokens_req != NULL)
    {

    /* Pull out the jobs token request */
    job_tokens = get_token(tokens_req->res_str);
    count_requested = job_tokens->count;

    /* Now dig through the running jobs to find what is being used */
    /* Sum up the total usage */

    job_ptr = sinfo->running_jobs;

    for (i = 0; job_ptr[i] != NULL; i++)
      {

      running_job_tokens_req = find_resource_req(job_ptr[i]->resreq, ATTR_tokens);

      /* If the job didn't ask for any tokens don't process it */

      if (running_job_tokens_req != NULL)
        {

        running_job_tokens = get_token(running_job_tokens_req->res_str);

        /* Sum up all tokens in use for accounting purposes */
        sum_tokens(running_job_tokens, &token_used);

        /* Check if the running job is using a token of the same type that is being requested */

        if (strcmp(running_job_tokens->identifier, job_tokens->identifier) == 0)
          {
          /* Got it */

          count_used += running_job_tokens->count;
          }

        free_token(running_job_tokens);
        }
      }

    /* Check what the max limit is for this server */
    /* Loop through all the tokens until we find the right kind */
    /* Also, sum up all the tokens in the server pool for accounting purposes */

    for (i = 0; (server_tokens = sinfo->tokens[i]) != NULL; i++)
      {

      sum_tokens(server_tokens, &token_pool);

      if (!strcmp(server_tokens->identifier, job_tokens->identifier))
        {
        max_count = server_tokens->count;
        }
      }


    /* Finally check if the requested plus the used exceeds the allowed */

    if (count_used + count_requested > max_count)
      {
      ret = SERVER_TOKEN_UTILIZATION;
      /* Have to call these functions just for cleanup */
      get_token_consumption(token_log_message, &token_used);
      get_token_consumption(token_log_message, &token_pool);
      }
    else
      {
      /* Write accounting record for the event of allocating tokens to this job */
      token_account_record(TOKEN_ACCT_RUN, jinfo->name, tokens_req->res_str);
      /* Write accounting record of what is currently being used */
      sum_tokens(job_tokens, &token_used);
      get_token_consumption(token_log_message, &token_used);
      token_account_record(TOKEN_ACCT_ABT, NULL, token_log_message);
      /* Write accounting record of the current pool (total of unused and used) */
      get_token_consumption(token_log_message, &token_pool);
      token_account_record(TOKEN_ACCT_POOL, NULL, token_log_message);
      }

    free_token(job_tokens);
    }

  return ret;
  }

/** Check if the queue is ignored
*
* @param qinfo queue to be checked
* @returns 0 if OK, 1 if ignored
*/
int check_ignored(queue_info *qinfo)
{
  int i;
  
  /* check if the queue is ignored */
  for (i = 0; i < MAX_IGNORED_QUEUES; i++)
    if (!strcmp(qinfo->name,conf.ignored_queues[i]))
        return QUEUE_IGNORED;

  return SUCCESS;
}

