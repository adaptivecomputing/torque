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
#include "pbs_ifl.h"
#include "pbs_error.h"
#include "server_info.h"
#include "constant.h"
#include "queue_info.h"
#include "job_info.h"
#include "misc.h"
#include "config.h"
#include "node_info.h"


/*
 *
 * query_server - creates a structure of arrays consisting of a server
 *   and all the queues and jobs that reside in that server
 *
 *   pbs_sd - connection to pbs_server
 *
 * returns a pointer to the server_info struct
 *
 */
server_info *query_server(int pbs_sd)
  {

  struct batch_status *server; /* info about the server */
  server_info *sinfo;  /* scheduler internal form of server info */
  queue_info **qinfo;  /* array of queues on the server */
  resource *res;  /* ptr to cycle through sources on server */
  int       local_errno = 0;

  /* get server information from pbs server */

  if ((server = pbs_statserver_err(pbs_sd, NULL, NULL, &local_errno)) == NULL)
    {
    fprintf(stderr, "pbs_statserver failed: %d\n", local_errno);
    return NULL;
    }

  /* convert batch_status structure into server_info structure */
  if ((sinfo = query_server_info(server)) == NULL)
    {
    pbs_statfree(server);
    return NULL;
    }

  /* get the nodes, if any */
  sinfo -> nodes = query_nodes(pbs_sd, sinfo);

  /* get the queues */
  if ((sinfo -> queues = query_queues(pbs_sd, sinfo)) == NULL)
    {
    pbs_statfree(server);
    free_server(sinfo, 0);
    return NULL;
    }

  /* count the queues and total up the individual queue states
   * for server totals. (total up all the state_count structs)
   */
  qinfo = sinfo -> queues;

  while (*qinfo != NULL)
    {
    sinfo -> num_queues++;
    total_states(&(sinfo -> sc), &((*qinfo) -> sc));
    qinfo++;
    }

  if ((sinfo -> jobs = (job_info **) malloc(sizeof(job_info *) * (sinfo -> sc.total + 1))) == NULL)
    {
    free_server(sinfo, 1);
    perror("Memory allocation error");
    return NULL;
    }

  set_jobs(sinfo);

  sinfo -> running_jobs =
    job_filter(sinfo -> jobs, sinfo -> sc.total, check_run_job, NULL);

  res = sinfo -> res;

  while (res != NULL)
    {
    if (res -> assigned == UNSPECIFIED)
      res -> assigned = calc_assn_resource(sinfo -> running_jobs, res -> name);

    res = res -> next;
    }

  sinfo -> timesharing_nodes =

    node_filter(sinfo -> nodes, sinfo -> num_nodes, is_node_timeshared, NULL);

  pbs_statfree(server);

  return sinfo;
  }

/*
 *
 * query_server_info - takes info from a batch_status structure about
 *       a server into a server_info structure for easy
 *       access
 *
 *   server - batch_status struct of server info
 *   last   - the last server so the new one can be added to the chain
 *     possibly NULL
 *
 * returns newly allocated and filled server_info struct
 *
 */

server_info *query_server_info(struct batch_status *server)
  {

  struct attrl *attrp; /* linked list of attributes */
  server_info *sinfo; /* internal scheduler structure for server info */
  resource *resp; /* a resource to help create the resource list */
  sch_resource_t count; /* used to convert string -> integer */
  char *endp;  /* used with strtol() */

  if ((sinfo = new_server_info()) == NULL)
    return NULL;    /* error */

  sinfo -> name = string_dup(server -> name);

  attrp = server -> attribs;

  while (attrp != NULL)
    {
    if (!strcmp(attrp -> name, ATTR_dfltque)) /* default_queue */
      sinfo -> default_queue = string_dup(attrp -> value);
    else if (!strcmp(attrp -> name, ATTR_maxrun))  /* max_running */
      {
      count = strtol(attrp -> value, &endp, 10);

      if (*endp != '\0')
        count = -1;

      sinfo -> max_run = count;
      }
    else if (!strcmp(attrp -> name, ATTR_maxuserrun))  /* max_user_run */
      {
      count = strtol(attrp -> value, &endp, 10);

      if (*endp != '\0')
        count = -1;

      sinfo -> max_user_run = count;
      }
    else if (!strcmp(attrp -> name, ATTR_maxgrprun))  /* max_group_run */
      {
      count = strtol(attrp -> value, &endp, 10);

      if (*endp != '\0')
        count = -1;

      sinfo -> max_group_run = count;
      }
    else if (!strcmp(attrp -> name, ATTR_rescavail))    /* resources_available*/
      {
      count = res_to_num(attrp -> value);
      resp = find_alloc_resource(sinfo -> res, attrp -> resource);

      if (sinfo -> res == NULL)
        sinfo -> res = resp;

      if (resp != NULL)
        resp -> avail = count;
      }
    else if (!strcmp(attrp -> name, ATTR_rescmax))   /* resources_max */
      {
      count = res_to_num(attrp -> value);
      resp = find_alloc_resource(sinfo -> res, attrp -> resource);

      if (sinfo -> res == NULL)
        sinfo -> res = resp;

      if (resp != NULL)
        resp -> max = count;
      }
    else if (!strcmp(attrp -> name, ATTR_rescassn))   /* resources_assigned */
      {
      count = res_to_num(attrp -> value);
      resp = find_alloc_resource(sinfo -> res, attrp -> resource);

      if (sinfo -> res == NULL)
        sinfo -> res = resp;

      if (resp != NULL)
        resp -> assigned = count;
      }
    else if (!strcmp(attrp -> name, ATTR_tokens))  /* tokens */
      {
      sinfo->tokens = get_token_array(attrp -> value);
      }

    attrp = attrp -> next;
    }

  return sinfo;
  }

/*
 *
 * find_alloc_resource - try and find a resource, and if it is not there
 *         alocate space for it and add it to the resource
 *         list
 *
 *   resplist - the resoruce list
 *   name - the name of the resource
 *
 * returns either the found resource or newly allocated resource
 *
 */

resource *find_alloc_resource(resource *resplist, char *name)
  {
  resource *resp;  /* used to search through list of resources */
  resource *prev = NULL; /* the previous resources in the list */

  resp = resplist;

  while (resp != NULL && strcmp(resp -> name, name))
    {
    prev = resp;
    resp = resp -> next;
    }

  if (resp == NULL)
    {
    if ((resp = new_resource()) == NULL)
      return NULL;

    resp -> name = string_dup(name);

    if (prev != NULL)
      prev -> next = resp;
    }

  return resp;
  }

/*
 *
 * find_resource - finds a resource in a resource list
 *
 *   reslist - resource list
 *   name - name of resource to find
 *
 * returns resource if found or NULL if not
 *
 */

resource *find_resource(resource *reslist, const char *name)
  {
  resource *resp; /* used to search through list of resources */

  resp = reslist;

  while (resp != NULL && strcmp(resp -> name, name))
    resp = resp -> next;

  return resp;
  }

/*
 *
 * free_server_info - free the space used by a server_info structure
 *
 * returns norhing
 *
 */
void free_server_info(server_info *sinfo)
  {
  if (sinfo -> name != NULL)
    free(sinfo -> name);

  if (sinfo -> default_queue != NULL)
    free(sinfo -> default_queue);

  if (sinfo -> jobs != NULL)
    free(sinfo -> jobs);

  if (sinfo -> running_jobs != NULL)
    free(sinfo -> running_jobs);

  if (sinfo -> timesharing_nodes != NULL)
    free(sinfo -> timesharing_nodes);

  free_resource_list(sinfo -> res);

  free(sinfo);
  }

/*
 *
 * free_resource - free a resource struct
 *
 *   res - the resource to free
 *
 * returns nothing
 *
 */
void free_resource(resource *res)
  {
  res -> next = NULL;
  free(res -> name);
  free(res);
  }

/*
 *
 * free_resource_list - free a resource list
 *
 *   reslist - the reslist to free
 *
 * returns nothing
 *
 */
void free_resource_list(resource *res_list)
  {
  resource *tmp; /* temporary next resource holder */
  resource *res; /* current resource to free */

  res = res_list;

  while (res != NULL)
    {
    tmp = res -> next;
    free_resource(res);
    res = tmp;
    }
  }

/*
 *
 * new_server_info - allocate and initalize a new server_info struct
 *
 * returns pointer to new allocated struct
 *
 */
server_info *new_server_info()
  {
  server_info *sinfo;   /* the new server */

  if ((sinfo = (server_info *) malloc(sizeof(server_info))) == NULL)
    return NULL;

  sinfo -> name = NULL;

  sinfo -> res = NULL;

  sinfo -> default_queue = NULL;

  sinfo -> queues = NULL;

  sinfo -> jobs = NULL;

  sinfo -> running_jobs = NULL;

  sinfo -> nodes = NULL;

  sinfo -> timesharing_nodes = NULL;

  sinfo -> num_queues = 0;

  sinfo -> num_nodes = 0;

  sinfo -> max_run = INFINITY;

  sinfo -> max_user_run = INFINITY;

  sinfo -> max_group_run = INFINITY;

  sinfo -> tokens = NULL;

  init_state_count(&(sinfo -> sc));

  return sinfo;
  }

/*
 *
 * new_resource - allocate and initialize new resoruce struct
 *
 * returns new struct
 *
 */
resource *new_resource()
  {
  resource *resp;  /* the new resource */

  if ((resp = malloc(sizeof(resource))) == NULL)
    return NULL;

  resp -> next = NULL;

  resp -> max = INFINITY;

  resp -> assigned = UNSPECIFIED;

  resp -> avail = UNSPECIFIED;

  return resp;
  }

/*
 *
 * print_server_info - print server_info structure
 *
 *   sinfo - the struct to print
 *   brief - only print the name of the server
 *
 * returns nothing
 *
 */
void print_server_info(server_info *sinfo, char brief)
  {
  resource *resp; /* used in printing the resources */

  if (sinfo == NULL)
    return;

  if (sinfo -> name != NULL)
    printf("Server name: %s\n", sinfo -> name);

  if (!brief)
    {
    printf("default_queue: %s\n", sinfo -> default_queue);
    printf("max_run: %d\n", sinfo -> max_run);
    printf("max_user_run: %d\n", sinfo -> max_user_run);
    printf("max_group_run: %d\n", sinfo -> max_group_run);
    printf("num_nodes: %d\n", sinfo -> num_nodes);
    printf("num_queues: %d\n", sinfo -> num_queues);
    print_state_count(&sinfo -> sc);

    resp = sinfo -> res;

    while (resp != NULL)
      {
      printf("res %s max: %-10ld avail: %-10ld assigned: %-10ld\n",
             resp -> name, resp -> max, resp -> avail, resp -> assigned);

      resp = resp -> next;
      }
    }
  }

/*
 *
 * free_server - free a server and possibly its queues also
 *
 *   sinfo - the server_info list head
 *   free_queues_too - flag to free the queues attached to server also
 *
 * returns nothing
 *
 */
void free_server(server_info *sinfo, int free_objs_too)
  {
  if (sinfo == NULL)
    return;

  if (free_objs_too)
    {
    free_queues(sinfo -> queues, 1);
    free_nodes(sinfo -> nodes);
    }

  free_token_list(sinfo -> tokens);

  free_server_info(sinfo);
  }

/*
 *
 * update_server_on_run - update server_info strucutre when a job is run
 *
 *   sinfo - the server to update
 *   qinfo - the queue the job is in
 *   jinfo - the job that was run
 *
 * returns nothing
 *
 */
void update_server_on_run(server_info *sinfo, queue_info *qinfo, job_info *jinfo)
  {
  resource_req *resreq;  /* used to cycle through resources to update */
  resource *res;  /* used in finding a resource to update */

  sinfo -> sc.running++;
  sinfo -> sc.queued--;

  resreq = jinfo -> resreq;

  while (resreq != NULL)
    {
    res = find_resource(sinfo -> res, resreq -> name);

    if (res)
      res -> assigned += resreq -> amount;

    resreq = resreq -> next;
    }
  }

/*
 *
 * set_jobs - create a large server job array of all the jobs on the
 *     system by coping all the jobs from the queue job arrays
 *
 *   sinfo - the server
 *
 * returns nothing
 */
void set_jobs(server_info *sinfo)
  {
  queue_info **qinfo;  /* used to cycle through the array of queues */
  job_info **jinfo;  /* used in copying jobs to server array */
  int i = 0, j;

  qinfo = sinfo -> queues;

  while (*qinfo != NULL)
    {
    jinfo = (*qinfo) -> jobs;

    if (jinfo != NULL)
      {
      for (j = 0; jinfo[j] != NULL; j++, i++)
        sinfo -> jobs[i] = jinfo[j];
      }

    qinfo++;
    }

  sinfo -> jobs[i] = NULL;
  }

/*
 *
 *      check_run_job - function used by job_filter to filter out
 *                      non-running jobs.
 *
 *        job - the job to check
 *        arg - optional arg
 *
 *      returns 1 if the job is running
 *
 */
int check_run_job(job_info *job, void *arg)
  {
  return job -> is_running;
  }

token** get_token_array(char* tokenlist)
  {
  char **list;
  token **token_array;
  int i, count;

  list = break_comma_list(string_dup(tokenlist));

  count = 0;

  while (list[count] != NULL)
    {
    count++;
    }

  token_array = (token **)malloc(sizeof(token *) * (count + 1));

  for (i = 0;  i < count; i++)
    {
    token_array[i] = get_token(list[i]);
    }

  token_array[count] = NULL;

  free_string_array(list);

  return token_array;
  }

token* get_token(char* token_string)
  {

  char *colon;
  token *this_token = (token *) malloc(sizeof(token));
  char *work_string = string_dup(token_string);
  colon = strstr(work_string, ":");
  this_token->count = atof(colon + 1);
  *colon = 0;
  this_token->identifier = work_string;

  return this_token;
  }

/*
 * Free a token list
 *
 */

void free_token_list(token ** tokens)
  {
  int i;
  token *this_token;

  if (tokens != NULL)
    {
    for (i = 0; (this_token = tokens[i]) != NULL; i++)
      {
      free_token(this_token);
      }

    free(tokens);
    }
  }

/*
 * Free an individual token
 *
 */

void free_token(token* token_ptr)
  {
  if (token_ptr != NULL)
    {
    if (token_ptr->identifier != NULL)
      {
      free(token_ptr->identifier);
      }

    free(token_ptr);
    }
  }

