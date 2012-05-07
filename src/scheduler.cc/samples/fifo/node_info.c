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
#include <ctype.h>
#include <sys/types.h>
#include "pbs_ifl.h"
#include "log.h"
#include "rm.h"
#include "node_info.h"
#include "misc.h"
#include "globals.h"



/* Internal functions */
int set_node_type(node_info *ninfo, char *ntype);


/*
 *      query_nodes - query all the nodes associated with a server
 *
 *   pbs_sd - communication descriptor wit the pbs server
 *   sinfo -  server information
 *
 * returns array of nodes associated with server
 *
 */
node_info **query_nodes(int pbs_sd, server_info *sinfo)
  {

  struct batch_status *nodes;  /* nodes returned from the server */

  struct batch_status *cur_node; /* used to cycle through nodes */
  node_info **ninfo_arr;  /* array of nodes for scheduler's use */
  node_info *ninfo;   /* used to set up a node */
  char errbuf[256];
  char *err;    /* used with pbs_geterrmsg() */
  int num_nodes = 0;   /* the number of nodes */
  int i;
  int local_errno;

  if ((nodes = pbs_statnode_err(pbs_sd, NULL, NULL, NULL, &local_errno)) == NULL)
    {
    err = pbs_geterrmsg(pbs_sd);
    sprintf(errbuf, "Error getting nodes: %s", err);
    sched_log(PBSEVENT_SCHED, PBS_EVENTCLASS_NODE, "", errbuf);
    return NULL;
    }

  cur_node = nodes;

  while (cur_node != NULL)
    {
    num_nodes++;
    cur_node = cur_node -> next;
    }

  if ((ninfo_arr = (node_info **) malloc((num_nodes + 1) * sizeof(node_info *))) == NULL)
    {
    perror("Error Allocating Memory");
    pbs_statfree(nodes);
    return NULL;
    }

  cur_node = nodes;

  for (i = 0; cur_node != NULL; i++)
    {
    if ((ninfo = query_node_info(cur_node, sinfo)) == NULL)
      {
      pbs_statfree(nodes);
      free_nodes(ninfo_arr);
      return NULL;
      }

    /* query mom on the node for resources */
    talk_with_mom(ninfo);

    ninfo_arr[i] = ninfo;

    cur_node = cur_node -> next;
    }

  ninfo_arr[i] = NULL;

  sinfo -> num_nodes = num_nodes;
  pbs_statfree(nodes);
  return ninfo_arr;
  }

/*
 *
 *      query_node_info - collect information from a batch_status and
 *                        put it in a node_info struct for easier access
 *
 *   node - a node returned from a pbs_statnode() call
 *
 * returns a node_info filled with information from node
 *
 */
node_info *query_node_info(struct batch_status *node, server_info *sinfo)
  {
  node_info *ninfo;  /* the new node_info */

  struct attrl *attrp;  /* used to cycle though attribute list */

  if ((ninfo = new_node_info()) == NULL)
    return NULL;

  attrp = node -> attribs;

  ninfo -> name = string_dup(node -> name);

  ninfo -> server = sinfo;

  while (attrp != NULL)
    {
    /* Node State... i.e. offline down free etc */
    if (!strcmp(attrp -> name, ATTR_NODE_state))
      set_node_state(ninfo, attrp -> value);

    /* properties from the servers nodes file */
    else if (!strcmp(attrp -> name, ATTR_NODE_properties))
      ninfo -> properties = break_comma_list(attrp -> value);

    /* the jobs running on the node */
    else if (!strcmp(attrp -> name, ATTR_NODE_jobs))
      ninfo -> jobs = break_comma_list(attrp -> value);

    /* the node type... i.e. timesharing or cluster */
    else if (!strcmp(attrp -> name, ATTR_NODE_ntype))
      set_node_type(ninfo, attrp -> value);

    attrp = attrp -> next;
    }

  return ninfo;
  }

/*
 *
 *      new_node_info - allocates a new node_info
 *
 * returns the new node_info
 *
 */
node_info *new_node_info()
  {
  node_info *new;

  if ((new = (node_info *) malloc(sizeof(node_info))) == NULL)
    {
    perror("Memory Allocation Error");
    return NULL;
    }

  new -> is_down = 0;

  new -> is_free = 0;
  new -> is_offline = 0;
  new -> is_unknown = 0;
  new -> is_reserved = 0;
  new -> is_exclusive = 0;
  new -> is_sharing = 0;
  new -> is_timeshare = 0;
  new -> is_cluster = 0;

  new -> name = NULL;
  new -> properties = NULL;
  new -> jobs = NULL;

  new -> max_load = 0.0;
  new -> ideal_load = 0.0;
  new -> arch = NULL;
  new -> ncpus = 0;
  new -> physmem = 0;
  new -> loadave = 0.0;

  return new;
  }

/*
 *
 * free_nodes - free all the nodes in a node_info array
 *
 *   ninfo_arr - the node info array
 *
 * returns nothing
 *
 */
void free_nodes(node_info **ninfo_arr)
  {
  int i;

  if (ninfo_arr != NULL)
    {
    for (i = 0; ninfo_arr[i] != NULL; i++)
      free_node_info(ninfo_arr[i]);

    free(ninfo_arr);
    }
  }

/*
 *
 *      free_node_info - frees memory used by a node_info
 *
 *   ninfo - the node to free
 *
 * returns nothing
 *
 */
void free_node_info(node_info *ninfo)
  {
  if (ninfo != NULL)
    {
    if (ninfo -> name != NULL)
      free(ninfo -> name);

    if (ninfo -> properties != NULL)
      free_string_array(ninfo -> properties);

    if (ninfo -> jobs != NULL)
      free_string_array(ninfo -> jobs);

    if (ninfo -> arch != NULL)
      free(ninfo -> arch);

    free(ninfo);
    }
  }

/*
 *
 * set_node_type - set the node type bits
 *
 *   ninfo - the node to set the type
 *   ntype - the type string from the server
 *
 * returns non-zero on error
 *
 */
int set_node_type(node_info *ninfo, char *ntype)
  {
  char errbuf[256];

  if (ntype != NULL && ninfo != NULL)
    {
    if (!strcmp(ntype, ND_timeshared))
      ninfo -> is_timeshare = 1;
    else if (!strcmp(ntype, ND_cluster))
      ninfo -> is_cluster = 1;
    else
      {
      sprintf(errbuf, "Unknown node type: %s", ntype);
      sched_log(PBSEVENT_SCHED, PBS_EVENTCLASS_NODE, ninfo -> name, errbuf);
      return 1;
      }

    return 0;
    }

  return 1;
  }

/*
 *
 *      set_node_state - set the node state info bits
 *
 *   ninfo - the node to set the state
 *   state - the state string from the server
 *
 * returns non-zero on error
 *
 */
int set_node_state(node_info *ninfo, char *state)
  {
  char errbuf[256];
  char *tok;    /* used with strtok() */

  if (ninfo != NULL && state != NULL)
    {
    tok = strtok(state, ",");

    while (tok != NULL)
      {
      while (isspace((int) *tok))
        tok++;

      if (!strcmp(tok, ND_down))
        ninfo -> is_down = 1;
      else if (!strcmp(tok, ND_free))
        ninfo -> is_free = 1;
      else if (!strcmp(tok, ND_offline))
        ninfo -> is_offline = 1;
      else if (!strcmp(tok, ND_state_unknown))
        ninfo -> is_unknown = 1;
      else if (!strcmp(tok, ND_job_exclusive))
        ninfo -> is_exclusive = 1;
      else if (!strcmp(tok, ND_job_sharing))
        ninfo -> is_sharing = 1;
      else if (!strcmp(tok, ND_reserve))
        ninfo -> is_reserved = 1;
      else if (!strcmp(tok, ND_busy))
        ninfo -> is_busy = 1;
      else
        {
        sprintf(errbuf, "Unknown Node State: %s", tok);
        sched_log(PBSEVENT_SCHED, PBS_EVENTCLASS_NODE, ninfo -> name, errbuf);
        }

      tok = strtok(NULL, ",");
      }

    return 0;
    }

  return 1;
  }




/*
 *
 *      talk_with_mom - talk to mom and get resources
 *
 *   ninfo - the node to to talk to its mom
 *
 * returns non-zero on error
 *
 */

int talk_with_mom(

  node_info *ninfo)

  {
  int mom_sd;   /* connection descriptor to mom */
  char *mom_ans;  /* the answer from mom - getreq() */
  char *endp;   /* used with strtol() */
  double testd;   /* used to convert string -> double */
  int testi;   /* used to convert string -> int */
  char errbuf[256];
  int i;
  int local_errno = 0;

  if ((ninfo != NULL) && !ninfo->is_down && !ninfo->is_offline)
    {
    if ((mom_sd = openrm(ninfo -> name, pbs_rm_port)) < 0)
      {
      sched_log(PBSEVENT_SYSTEM, PBS_EVENTCLASS_REQUEST, ninfo -> name, "Can not open connection to mom");
      return 1;
      }

    for (i = 0; i < num_resget; i++)
      addreq_err(mom_sd, &local_errno, (char *) res_to_get[i]);

    for (i = 0; i < num_resget && (mom_ans = getreq_err(&local_errno, (mom_sd))) != NULL; i++)
      {
      if (!strcmp(res_to_get[i], "max_load"))
        {
        testd = strtod(mom_ans, &endp);

        if (*endp == '\0')
          ninfo -> max_load = testd;
        else
          ninfo -> max_load = ninfo -> ncpus;

        free(mom_ans);
        }
      else if (!strcmp(res_to_get[i], "ideal_load"))
        {
        testd = strtod(mom_ans, &endp);

        if (*endp == '\0')
          ninfo -> ideal_load = testd;
        else
          ninfo -> ideal_load = ninfo -> ncpus;

        free(mom_ans);
        }
      else if (!strcmp(res_to_get[i], "arch"))
        ninfo -> arch = mom_ans;
      else if (!strcmp(res_to_get[i], "ncpus"))
        {
        testi = strtol(mom_ans, &endp, 10);

        if (*endp == '\0')
          ninfo -> ncpus = testi;
        else
          ninfo -> ncpus = 1;

        free(mom_ans);
        }
      else if (!strcmp(res_to_get[i], "physmem"))
        {
        ninfo -> physmem = res_to_num(mom_ans);
        free(mom_ans);
        }
      else if (!strcmp(res_to_get[i], "loadave"))
        {
        testd = strtod(mom_ans, &endp);

        if (*endp == '\0')
          ninfo -> loadave = testd;
        else
          ninfo -> loadave = -1.0;

        free(mom_ans);
        }
      else
        {
        sprintf(errbuf, "Unknown resource value[%d]: %s", i, mom_ans);
        sched_log(PBSEVENT_SCHED, PBS_EVENTCLASS_NODE, ninfo -> name, errbuf);
        }
      }

    closerm_err(&local_errno, mom_sd);
    }

  return 0;
  }

/*
 *
 * node_filter - filter a node array and return a new filterd array
 *
 *   nodes - the array to filter
 *   size  - size of nodes
 *    filter_func - pointer to a function that will filter the nodes
 *  - returns 1: job will be added to filtered array
 *  - returns 0: job will NOT be added to filtered array
 *   arg - an optional arg passed to filter_func
 *
 * returns pointer to filtered array
 *
 * filter_func prototype: int func( node_info *, void * )
 *
 */
node_info **node_filter(node_info **nodes, int size,
                        int (*filter_func)(node_info*, void*), void *arg)
  {
  node_info **new_nodes = NULL;   /* the new node array */
  node_info **tmp = NULL;
  int i, j;

  if ((new_nodes = (node_info **) malloc((size + 1) * sizeof(node_info *))) == NULL)
    {
    perror("Memory Allocation Error");
    return NULL;
    }

  for (i = 0, j = 0; i < size; i++)
    {
    if (filter_func(nodes[i], arg))
      {
      new_nodes[j] = nodes[i];
      j++;
      }
    }

  new_nodes[j] = NULL;

  if (j == 0)
    {
    free(new_nodes);
    new_nodes = NULL;
    }
  else if ((tmp = realloc(new_nodes, (j + 1) * sizeof(node_info *))) == NULL)
    {
    perror("Memory Allocation Error");
    free(new_nodes);
    new_nodes = NULL;
    }
  else
    {
    new_nodes = tmp;
    }

  return new_nodes;
  }

/*
 *
 *      is_node_timeshared - check if a node is timeshared
 *
 *        node - node to check
 *        arg  - unused argument
 *
 *      returns
 *        1: is a timeshared node
 *        0: is not a timeshared node
 *
 *      NOTE: this function used for node_filter
 *
 */
int is_node_timeshared(node_info *node, void *arg)
  {
  if (node != NULL)
    return node -> is_timeshare;

  return 0;
  }

/*
 *
 * find_best_node - find the best node to run a job
 *
 *   jinfo - the job to run
 *   ninfo_arr - array of nodes to find the best of
 *
 * returns the node to run the job on
 *
 */
node_info *find_best_node(job_info *jinfo, node_info **ninfo_arr)
  {
  node_info *possible_node = NULL; /* node which under max node not ideal*/
  node_info *good_node = NULL;  /* node which is under ideal load */
  resource_req *req;   /* used to find requested resources */
  sch_resource_t ncpus;   /* used for number of CPUS on nodes */
  sch_resource_t mem;   /* used for memory on the nodes */
  char *arch;    /* used for the architecture of nodes */
  char *host;    /* used for the node name */
  int i = 0, c;   /* index & loop vars */
  int ln_i;    /* index of the last node in ninfo_arr*/
  float good_node_la = 1.0e10;  /* the best load ave found yet */
  float proj_la;   /* the projected load average */
  char logbuf[256];   /* buffer for log messages */

  /* name of the last node a job ran on - used in load balance round robin */
  static char last_node_name[PBS_MAXSVRJOBID];

  if (ninfo_arr == NULL || jinfo == NULL)
    return NULL;

  /* if the job is requesting nodes, then don't try and load balance it */
  if (find_resource_req(jinfo -> resreq, "nodes") != NULL)
    return NULL;

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

  if (last_node_name[0] == '\0' && ninfo_arr[0] != NULL)
    strcpy(last_node_name, ninfo_arr[0] -> name);

  if (cstat.load_balancing_rr)
    {
    sched_log(PBSEVENT_DEBUG2, PBS_EVENTCLASS_NODE, last_node_name, "Last node a job was run on");
    /* find the node we last ran a job on */

    for (ln_i = 0; ninfo_arr[ln_i] != NULL &&
         strcmp(ninfo_arr[ln_i] -> name, last_node_name); ln_i++)
      ;

    /* if the last node has been deleted since the last scheduling cycle */
    if (ninfo_arr[ln_i] == NULL)
      {
      strcpy(last_node_name, ninfo_arr[0] -> name);
      /* ln_i = 0; */
      }

    /* if the last node is at the end of the list, cycle back to the front */
    else if (ninfo_arr[ln_i+1] == NULL)
      i = 0;

    /* start one after the last node */
    else
      i = ln_i + 1;
    }
  else
    {
    i = 0;
    ln_i = jinfo -> queue -> server -> num_nodes;
    }


  for (c = 0; c < jinfo -> queue -> server -> num_nodes; c++)
    {
    /* if the job didn't specify memory, it will default to 0, and if
     * the mom didn't return physmem, it defaults to 0.
     */
    if (ninfo_arr[i] -> is_free  &&
        (arch == NULL || !strcmp(arch, ninfo_arr[i] -> arch)) &&
        (host == NULL || !strcmp(host, ninfo_arr[i] -> name)) &&
        mem <= ninfo_arr[i] -> physmem)
      {
      proj_la = ninfo_arr[i] -> loadave + ncpus;

      if (cstat.load_balancing)
        {
        if (proj_la <= ninfo_arr[i] -> ideal_load)
          {
          if (ninfo_arr[i] -> loadave < good_node_la)
            {
            sprintf(logbuf, "Possible low-loaded node load: %6.2f proj load: %6.2f ideal_load: %6.2f last good ideal: %6.2f", ninfo_arr[i] -> loadave, proj_la, ninfo_arr[i] -> ideal_load, good_node_la);

            good_node = ninfo_arr[i];
            good_node_la = ninfo_arr[i] -> loadave;
            }
          else
            sprintf(logbuf, "Node Rejected, Load higher then last possible node: load: %6.2f last good ideal: %6.2f", ninfo_arr[i] -> loadave, good_node_la);
          }
        else if (possible_node == NULL && proj_la <= ninfo_arr[i] -> max_load)
          {
          if (ninfo_arr[i] -> loadave < good_node_la)
            {
            sprintf(logbuf, "Possible medium-loaded node load: %6.2f max_load: %6.2f proj load: %6.2f last good ideal: %6.2f", ninfo_arr[i] -> loadave, ninfo_arr[i] -> max_load, proj_la, good_node_la);
            possible_node = ninfo_arr[i];
            good_node_la = ninfo_arr[i] -> loadave;
            }
          else
            sprintf(logbuf, "Node Rejected, Load higher then last possible node: load: %6.2f last good ideal: %6.2f", ninfo_arr[i] -> loadave, good_node_la);
          }
        else
          sprintf(logbuf, "Node Rejected, Load too high: load: %6.2f proj load: %6.2f max_load: %6.2f", ninfo_arr[i] -> loadave, proj_la, ninfo_arr[i] -> max_load);
        }
      else
        {
        if (ninfo_arr[i] -> loadave + proj_la  < ninfo_arr[i] -> max_load)
          {
          good_node = ninfo_arr[i];
          break;
          }
        else
          sprintf(logbuf, "Node Rejected, Load too high: load: %6.2f proj load: %6.2f max_load: %6.2f", ninfo_arr[i] -> loadave, proj_la, ninfo_arr[i] -> max_load);
        }
      }
    else
      sprintf(logbuf, "Node Rejected, node does not fit job requirements.");


    sched_log(PBSEVENT_DEBUG2, PBS_EVENTCLASS_NODE, ninfo_arr[i] -> name, logbuf);

    logbuf[0] = '\0';

    i++;

    if (ninfo_arr[i] == NULL)
      i = 0;
    }

  if (good_node == NULL)
    {
    if (cstat.load_balancing_rr)
      strcpy(last_node_name, possible_node -> name);

    sched_log(PBSEVENT_DEBUG2, PBS_EVENTCLASS_NODE, possible_node -> name, "Node Chosen to run job on");

    return possible_node;
    }
  else
    {
    if (cstat.load_balancing_rr)
      strcpy(last_node_name, good_node -> name);

    sched_log(PBSEVENT_DEBUG2, PBS_EVENTCLASS_NODE, good_node -> name, "Node Chosen to run job on");

    return good_node;
    }

  return NULL;  /* should never get here */
  }

/*
 *
 * find_node_info - find a node in a node array
 *
 *   nodename - the node to find
 *   ninfo_arr - the array of nodes to look in
 *
 * returns the node or NULL of not found
 *
 */
node_info *find_node_info(char *nodename, node_info **ninfo_arr)
  {
  int i;

  if (nodename != NULL && ninfo_arr != NULL)
    {
    for (i = 0; ninfo_arr[i] != NULL &&
         strcmp(ninfo_arr[i] -> name, nodename); i++)
      ;

    return ninfo_arr[i];
    }

  return NULL;
  }

/*
 *
 * print_node - print all the information in a node.  Mainly used for
 *        debugging purposes
 *
 *   ninfo - the node to print
 *   brief - boolean: only print the name ?
 *
 * returns nothing
 *
 */
void print_node(node_info *ninfo, int brief)
  {
  int i;

  if (ninfo != NULL)
    {
    if (ninfo -> name != NULL)
      printf("Node: %s\n", ninfo -> name);

    if (!brief)
      {
      printf("is_down: %s\n", ninfo -> is_down ? "TRUE" : "FALSE");
      printf("is_free: %s\n", ninfo -> is_free ? "TRUE" : "FALSE");
      printf("is_offline: %s\n", ninfo -> is_offline ? "TRUE" : "FALSE");
      printf("is_unknown: %s\n", ninfo -> is_unknown ? "TRUE" : "FALSE");
      printf("is_reserved: %s\n", ninfo -> is_reserved ? "TRUE" : "FALSE");
      printf("is_exclusive: %s\n", ninfo -> is_exclusive ? "TRUE" : "FALSE");
      printf("is_sharing: %s\n", ninfo -> is_sharing ? "TRUE" : "FALSE");
      printf("is_timeshare: %s\n", ninfo -> is_timeshare ? "TRUE" : "FALSE");
      printf("is_cluster: %s\n", ninfo -> is_cluster ? "TRUE" : "FALSE");

      if (ninfo -> properties != NULL)
        {
        printf("Properties: ");

        for (i = 0; ninfo -> properties[i] != NULL; i++)
          {
          if (i)
            printf(", ");

          printf("%s", ninfo -> properties[i]);
          }

        printf("\n");
        }

      if (ninfo -> jobs != NULL)
        {
        printf("Running Jobs: ");

        for (i = 0; ninfo -> jobs[i] != NULL; i++)
          {
          if (i)
            printf(", ");

          printf("%s", ninfo -> jobs[i]);
          }
        }
      }
    }
  }




