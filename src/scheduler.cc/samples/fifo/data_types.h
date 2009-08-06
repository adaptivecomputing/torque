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

#ifndef DATA_TYPES_H
#define DATA_TYPES_H

#include <time.h>
#include "pbs_ifl.h"
#include "constant.h"
#include "config.h"

struct server_info;

struct state_count;

struct queue_info;

struct node_info;

struct job_info;

struct resource;

struct resource_req;

struct holiday;

struct prev_job_info;

struct group_info;

struct usage_info;

struct token;

typedef struct state_count state_count;

typedef struct server_info server_info;

typedef struct queue_info queue_info;

typedef struct job_info job_info;

typedef struct node_info node_info;

typedef struct resource resource;

typedef struct resource_req resource_req;

typedef struct usage_info usage_info;

typedef struct group_info group_info;

typedef struct prev_job_info prev_job_info;

typedef struct token token;

typedef RESOURCE_TYPE sch_resource_t;
/* since resource values and usage values are linked */
typedef sch_resource_t usage_t;

struct token
  {
  char* identifier;             /* Token identifier */
  float count;                  /* The number of tokens available of type identifier */
  };


struct state_count
  {
  int running;                  /* number of jobs in the running state*/
  int queued;                   /* number of jobs in the queued state */
  int held;                     /* number of jobs in the held state */
  int transit;                  /* number of jobs in the transit state */
  int waiting;                  /* number of jobs in the waiting state */
  int exiting;                  /* number of jobs in the exiting state */
  int suspended;  /* number of jobs in the suspended state */
  int completed;                /* number of jobs in the completed state */
  int total;   /* total number of jobs in all states */
  };

struct server_info
  {
  char *name;   /* name of server */

  struct resource *res;  /* list of resources */
  char *default_queue;  /* the default queue atribute of the server */
  int max_run;   /* max jobs that can be run at one time */
  int max_user_run;  /* max jobs a user can run at one time */
  int max_group_run;  /* max jobs a group can run at one time */
  int num_queues;  /* number of queues that reside on the server */
  int num_nodes;  /* number of nodes associated with the server */
  state_count sc;  /* number of jobs in each state */
  queue_info **queues;  /* array of queues */
  job_info **jobs;  /* array of jobs on the server */
  job_info **running_jobs; /* array of jobs in the running state */
  node_info **nodes;  /* array of nodes associated with the server */
  node_info **timesharing_nodes;/* array of timesharing nodes */
  token **tokens;               /* array of tokens */
  };

struct queue_info
  {

unsigned is_started:
  1; /* is queue started */

unsigned is_exec:
  1;  /* is the queue an execution queue */

unsigned is_route:
  1;  /* is the queue a routing queue */

unsigned is_ok_to_run:
  1; /* is it ok to run jobs in this queue */

unsigned dedtime_queue:
  1; /* jobs can run in dedicated time */

  struct server_info *server;   /* server where queue resides */
  char *name;                   /* queue name */
  state_count sc;               /* number of jobs in different states */
  int max_run;                  /* max jobs that can run in queue */
  int max_user_run;             /* max jobs that a user can run in queue */
  int max_group_run;            /* max jobs that a group can run in a queue */
  int priority;   /* priority of queue */

  struct resource *qres; /* list of resources on the queue */
  job_info **jobs;  /* array of jobs that reside in queue */
  job_info **running_jobs; /* array of jobs in the running state */
  };

struct job_info
  {

unsigned is_queued:
  1;   /* state booleans */

unsigned is_running:
  1;

unsigned is_held:
  1;

unsigned is_waiting:
  1;

unsigned is_transit:
  1;

unsigned is_exiting:
  1;

unsigned is_suspended:
  1;

unsigned is_completed:
  1;

unsigned is_starving:
  1; /* job has waited passed starvation time */

unsigned can_not_run:
  1; /* set in a cycle of a job cant run this cycle*/

unsigned can_never_run:
  1; /* set if a job can never be run */

  char *name;   /* name of job */
  char *comment;  /* comment field of job */
  char *account;  /* username of the owner of the job */
  char *group;   /* groupname of the owner of the job */

  struct queue_info *queue; /* queue where job resides */
  int priority;   /* PBS priority of job */
  int sch_priority;  /* internal scheduler priority */
  time_t qtime;   /* the time the job was last queued */
  resource_req *resreq;  /* list of resources requested */
  resource_req *resused; /* a list of resources used */
  group_info *ginfo;  /* the fair share node for the owner */
  node_info *job_node;  /* node the job is running on */
  };

struct node_info
  {

unsigned is_down:
  1;  /* node is down */

unsigned is_free:
  1;  /* node is free to run a job */

unsigned is_offline:
  1; /* node is off-line */

unsigned is_unknown:
  1; /* node is in an unknown state */

unsigned is_reserved:
  1; /* node has been reserved */

unsigned is_exclusive:
  1; /* node is running in job-exclusive mode */

unsigned is_sharing:
  1; /* node is running in job-sharing mode */

unsigned is_busy:
  1;  /* load on node is too high to schedule */

  /* node types */

unsigned is_timeshare:
  1; /* this node is for time-sharing only */

unsigned is_cluster:
  1; /* This node is a member of a cluster */

  char *name;   /* name of the node */
  char **properties;  /* the node properties */
  char **jobs;   /* the jobs currently running on the node */

  server_info *server;  /* server that the node is associated with */

  float max_load;  /* the load not to go over */
  float ideal_load;  /* the ideal load of the machine */
  char *arch;   /* machine architecture */
  int ncpus;   /* number of cpus */
  int physmem;   /* amount of physical memory in kilobytes */
  float loadave;  /* current load average */
  };

struct resource
  {
  char *name;   /* name of the resource */
  sch_resource_t avail;  /* availble amount of the resource */
  sch_resource_t max;  /* max amount of the resource */
  sch_resource_t assigned; /* amount of the resource assigned */

  struct resource *next; /* next resource in list */
  };

struct resource_req
  {
  char *name;   /* name of the resource */
  sch_resource_t amount; /* numeric value of resource */
  char *res_str;  /* string value of resource */

  struct resource_req *next; /* next resource_req in list */
  };

struct prev_job_info
  {
  char *name;   /* name of job */
  char *account;  /* account name of user */
  group_info *ginfo;  /* ginfo of user in fair share tree */
  resource_req *resused; /* resources used by the job */
  };

struct mom_res
  {
  char name[MAX_RES_NAME_SIZE];  /* name of resources for addreq() */
  char ans[MAX_RES_RET_SIZE];  /* what is returned from getreq() */

unsigned eol:
  1;   /* set for sentinal value */
  };

/* global data types */

struct group_info
  {
  char *name;    /* name of user/group */
  int resgroup;    /* resgroup the group is in */
  int cresgroup;   /* resgroup of the children of group */
  int shares;    /* number of shares this group has */
  float percentage;   /* overall percentage the group has */
  usage_t usage;   /* calculated usage info */
  usage_t temp_usage;   /* usage plus any temporary usage */

  group_info *parent;   /* parent node */
  group_info *sibling;   /* sibling node */
  group_info *child;   /* child node */
  };

/* This structure is used to write out the usage to disk */

struct group_node_usage
  {
  char name[9];
  usage_t usage;
  };


struct usage_info
  {
  char *name;   /* name of the user */

  struct resource_req *reslist; /* list of resources */
  int computed_value;  /* value computed from usage info */
  };

struct t
  {

unsigned hour :
  5;

unsigned min :
  6;

unsigned none :
  1;

unsigned all  :
  1;
  };

struct sort_info
  {
  enum sort_type sort;
  char *config_name;
  int (*cmp_func)(const void *, const void*);
  };

struct timegap
  {
  time_t from;
  time_t to;
  };

struct config
  {
  /* these bits control the scheduling policy
   * prime_* is the prime time setting
   * non_prime_* is the non-prime setting
   */

unsigned prime_rr :
  1; /* round robin through queues*/

unsigned non_prime_rr :
  1;

unsigned prime_bq :
  1; /* by queue */

unsigned non_prime_bq :
  1;

unsigned prime_sf :
  1; /* strict fifo */

unsigned non_prime_sf :
  1;

unsigned prime_fs :
  1; /* fair share */

unsigned non_prime_fs :
  1;

unsigned prime_lb :
  1; /* load balancing */

unsigned non_prime_lb :
  1;

unsigned prime_hsv :
  1; /* help starving jobs */

unsigned non_prime_hsv:
  1;

unsigned prime_sq :
  1; /* sort queues by priority */

unsigned non_prime_sq :
  1;

unsigned prime_lbrr :
  1; /* round robin through load balanced nodes */

unsigned non_prime_lbrr:
  1;


  struct sort_info *sort_by;  /* current sort */

  struct sort_info *prime_sort;  /* prime time sort */

  struct sort_info *non_prime_sort; /* non-prime time sort */
  group_info *group_root;  /* root of group_info tree */
  time_t half_life;   /* half-life time in seconds */
  time_t sync_time;   /* time between syncing usage to disk */

  struct t prime[HIGH_DAY][HIGH_PRIME]; /* prime time start and prime time end*/
  int holidays[MAX_HOLIDAY_SIZE]; /* holidays in julian date */
  int num_holidays;   /* number of acuall holidays */

  struct timegap ded_time[MAX_DEDTIME_SIZE]; /* dedicated times */
  int holiday_year;   /* the year the holidays are for */
  int unknown_shares;   /* unknown group shares */
  int log_filter;   /* what events to filter out */
  char ded_prefix[PBS_MAXQUEUENAME +1]; /* prefix to dedicated queues */
  time_t max_starve;   /* starving threshold */
  char* ignored_queues[MAX_IGNORED_QUEUES]; /* list of ignored queues */
  };

/* for description of these bits, check the PBS admin guide or scheduler IDS */

struct status
  {

unsigned round_robin:
  1;

unsigned by_queue:
  1;

unsigned strict_fifo:
  1;

unsigned fair_share:
  1;

unsigned load_balancing:
  1;

unsigned load_balancing_rr:
  1;

unsigned help_starving_jobs:
  1;

unsigned sort_queues:
  1;

unsigned is_prime:
  1;

unsigned is_ded_time:
  1;

  struct sort_info *sort_by;

  time_t current_time;

  job_info *starving_job; /* the most starving job */

  };

/* static data types */

struct rescheck
  {
  char *name;
  char *comment_msg;
  char *debug_msg;
  };

#endif
