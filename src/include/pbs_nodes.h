#ifndef PBS_NODES_H
#define PBS_NODES_H
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

/*
** Header file used for the node tracking routines.
*/

#include <pthread.h>
#include <netinet/in.h> /* sockaddr_in */
#include "resizable_array.h"
#include "hash_table.h"
#include "net_connect.h" /* pbs_net_t */
#include "pbs_ifl.h" /* resource_t */

/* NOTE:  requires server_limits.h */

#include "dynamic_string.h"

#ifdef NUMA_SUPPORT
/* NOTE: cpuset support needs hwloc */
#  include <hwloc.h>
#endif

#define NUMA_KEYWORD           "numa"
#define START_GPU_STATUS       "<gpu_status>"
#define END_GPU_STATUS         "</gpu_status>"

#ifdef NUMA_SUPPORT
#  define MAX_NODE_BOARDS      2048
#endif  /* NUMA_SUPPORT */
#ifdef PENABLE_LINUX26_CPUSETS
#  define TROOTCPUSET_PATH     "/dev/cpuset"
#  define TBOOTCPUSET_PATH     "/dev/cpuset/boot"
#  define TTORQUECPUSET_PATH   "/dev/cpuset/torque"
#  ifdef USELIBCPUSET
#    define TROOTCPUSET_BASE   "/"
#    define TBOOTCPUSET_BASE   "/boot"
#    define TTORQUECPUSET_BASE "/torque"
#  endif
#endif /* PENABLE_LINUX26_CPUSETS */

#define PBS_MAXNODENAME 80 /* upper bound on the node name size    */
#define BM_ERROR -20
#define MAX_LEVEL_DEPTH 100 /* maximum levels per path */


enum psit
  {
  okay,
  thinking,
  conflict,
  ignore
  };

enum gpmodeit
  {
  gpu_normal,
  gpu_exclusive_thread,
  gpu_prohibited,
  gpu_exclusive_process,
  gpu_unknown
  };

enum gpstatit
  {
  gpu_unallocated,
  gpu_shared,
  gpu_exclusive,
  gpu_unavailable
  };

struct prop
  {
  char *name;
  short mark;

  struct prop *next;
  };

struct jobinfo
  {
  char            jobid[PBS_MAXSVRJOBID+1];

  struct jobinfo *next;
  };

typedef struct alps_req_data
  {
  dynamic_string *node_list;
  int             ppn;
  } alps_req_data;

typedef struct single_spec_data
  {
  int          nodes;   /* nodes needed for this req */
  int          ppn;     /* ppn for this req */
  int          gpu;     /* gpus for this req */
  int          req_id;  /* the id of this alps req - used only for cray */
  struct prop *prop;    /* node properties needed */
  } single_spec_data;

typedef struct complete_spec_data
  {
  single_spec_data  *reqs;        /* array of data for each req */
  int                num_reqs;    /* number of reqs (number of '+' in spec + 1) */
  int                total_nodes; /* number of nodes for all reqs in a spec */
  /* pointer to start of req in spec, only valid until call of parse_req_data() */
  char             **req_start;   
  } complete_spec_data;

typedef struct node_job_add_info
  {
  char                      node_name[PBS_MAXNODENAME + 1];
  int                       ppn_needed;
  int                       gpu_needed;
  int                       is_external;
  struct node_job_add_info *next;
  } node_job_add_info;



struct pbssubn
  {

  struct pbsnode *host;

  struct pbssubn *next;

  struct jobinfo *jobs;     /* list of jobs allocating resources within subnode */
  /* does this include suspended jobs? */
  resource_t      allocto;
  enum psit      flag;  /* XXX */
  unsigned short  inuse;
  short           index;  /* subnode index */
  };

struct gpusubn
  {
  char            jobid[PBS_MAXSVRJOBID+1];   /* jobid on this gpu subnode */
  unsigned short  inuse;  /* 1 if this node is in use, 0 otherwise */
  enum gpstatit   state;  /* gpu state determined by server */
  enum gpmodeit   mode;   /* gpu mode from hardware */
  int             driver_ver;  /* Driver version reported from hardware */
  enum psit       flag;   /* same as for pbssubn */
  short           index;  /* gpu index */
  char           *gpuid;  /* gpu id */
  int             job_count;
  };


#ifdef NUMA_SUPPORT
typedef struct nodeboard_t
  {
  int                index;        /* the node's index */
  int                num_cpus;     /* count of cpus */
  int                num_nodes;    /* count of NUMA nodes */
  hwloc_bitmap_t     cpuset;       /* bitmap containing CPU IDs of this nodeboard */
  hwloc_bitmap_t     nodeset;      /* bitmap containing NUMA node IDs of this nodeboard */
  unsigned long      memsize;
  char             **path_meminfo; /* path to meminfo file for each NUMA node */
  unsigned long long pstat_busy;
  unsigned long long pstat_idle;
  float              cpuact;
  } nodeboard;
#endif /* NUMA_SUPPORT */





/* container for holding communication information */
typedef struct received_node
  {
  char            hostname[PBS_MAXNODENAME];
  dynamic_string *statuses;
  int             hellos_sent;
  } received_node;





/* struct used for iterating numa nodes */
typedef struct node_iterator 
  {
  int node_index;
  int numa_index;
  int alps_index;
  } node_iterator;



typedef struct all_nodes
  {
  resizable_array *ra;
  hash_table_t    *ht;

  pthread_mutex_t *allnodes_mutex;
  } all_nodes;




struct pbsnode
  {
  char                 *nd_name;             /* node's host name */

  struct pbssubn       *nd_psn;              /* ptr to list of subnodes */

  struct prop          *nd_first;            /* first and last property */

  struct prop          *nd_last;

  struct prop          *nd_f_st;             /* first and last status */

  struct prop          *nd_l_st;
  u_long               *nd_addrs;            /* IP addresses of host */

  struct array_strings *nd_prop;             /* array of properities */

  struct array_strings *nd_status;
  char                 *nd_note;             /* note set by administrator */
  int                   nd_stream;           /* stream to Mom on host */
  enum psit             nd_flag;
  unsigned short        nd_mom_port;         /* For multi-mom-mode unique port value PBS_MOM_SERVICE_PORT*/
  unsigned short        nd_mom_rm_port;      /* For multi-mom-mode unique port value PBS_MANAGER_SERVICE_PORT */
  struct sockaddr_in    nd_sock_addr;        /* address information */
  short                 nd_nprops;           /* number of properties */
  short                 nd_nstatus;          /* number of status items */
  short                 nd_nsn;              /* number of VPs  */
  short                 nd_nsnfree;          /* number of VPs free */
  short                 nd_nsnshared;        /* number of VPs shared */
  short                 nd_needed;           /* number of VPs needed */
  short                 nd_np_to_be_used;    /* number of VPs marked for a job but not yet assigned */
  unsigned short        nd_state;            /* node state (see INUSE_* #defines below) */
  unsigned short        nd_ntype;            /* node type */
  short                 nd_order;            /* order of user's request */
  time_t                nd_warnbad;
  time_t                nd_lastupdate;       /* time of last update. */
  unsigned short        nd_hierarchy_level;
  unsigned char         nd_in_hierarchy;     /* set to TRUE if in the hierarchy file */

  short                 nd_ngpus;            /* number of gpus */ 
  short                 nd_gpus_real;        /* gpus are real not virtual */ 
  struct gpusubn       *nd_gpusn;            /* gpu subnodes */
  short                 nd_ngpus_free;       /* number of free gpus */
  short                 nd_ngpus_needed;     /* number of gpus needed */
  short                 nd_ngpus_to_be_used; /* number of gpus marked for a job but not yet assigned */
  struct array_strings *nd_gpustatus;        /* string array of GPU status */
  short                 nd_ngpustatus;       /* number of gpu status items */

  struct pbsnode       *parent;              /* pointer to the node holding this node, or NULL */
  unsigned short        num_node_boards;     /* number of numa nodes */
  struct AvlNode       *node_boards;         /* private tree of numa nodes */
  char                 *numa_str;            /* comma-delimited string of processor values */
  char                 *gpu_str;             /* comma-delimited string of the number of gpus for each nodeboard */
  
  unsigned char         nd_is_alps_reporter;
  unsigned char         nd_is_alps_login;
  resizable_array      *nd_ms_jobs;          /* the jobs this node is mother superior for */
  all_nodes             alps_subnodes;

  pthread_mutex_t      *nd_mutex;            /* semaphore for accessing this node's data */
  };

extern struct pbsnode *alps_reporter;


#define INITIAL_NODE_SIZE  20



void            initialize_all_nodes_array(all_nodes *);
int             insert_node(all_nodes *,struct pbsnode *);
int             remove_node(all_nodes *,struct pbsnode *);
struct pbsnode *next_node(all_nodes *,struct pbsnode *,node_iterator *);
struct pbsnode *next_host(all_nodes *,int *,struct pbsnode *);
int             copy_properties(struct pbsnode *dest, struct pbsnode *src);


#define HELLO_RESEND_WAIT_TIME 10

typedef struct hello_info
  {
  char   *name;
  time_t  last_retry;
  int     num_retries;
  } hello_info;



typedef struct hello_container
  {
  resizable_array *ra;
  pthread_mutex_t *hello_mutex;
  } hello_container;


void        initialize_hello_container(hello_container *);
int         needs_hello(hello_container *, char *);
int         add_hello(hello_container *, char *);
int         add_hello_after(hello_container *, char *, int);
int         add_hello_info(hello_container *, hello_info *);
hello_info *pop_hello(hello_container *);
int         remove_hello(hello_container *, char *);
int         send_hierarchy(char *, unsigned short);
void       *send_hierarchy_threadtask(void *);



struct howl
  {
  char           *name;
  int             order;
  int             index;
  unsigned short  port;

  struct howl    *next;
  };


typedef struct tree_t
  {
  u_long          key;

  struct pbsnode *nodep;

  struct tree_t  *left;

  struct tree_t  *right;
  } tree;

typedef struct newtree_t
  {
  u_long             key;      /* value used to be stored and sorted */
  int              **index;    /* optional. pointer to array of newtree_t structures*/
  struct pbsnode    *nodep;

  struct newtree_t  *parent;
  struct newtree_t  *left;
  struct newtree_t  *right;
  } newtree;



/* NOTE:  should remove all node references and replace with 'tree' objects (NYI) */

/*
typedef struct node_t {
  u_long         key;
  struct node_t *left;
  struct node_t *right;
  } node;
*/

struct pbsnode *tfind(const u_long, tree **);
int tlist(tree *, char *, int);

/*
 * The following INUSE_ are used in both subnode.inuse and in node.nd_state
 */

#define INUSE_FREE             0x00 /* Node/VP is available   */
#define INUSE_OFFLINE          0x01 /* Node was removed by administrator */
#define INUSE_DOWN             0x02 /* Node is down/unresponsive   */
#define INUSE_DELETED          0x04 /* Node is "deleted"   */
#define INUSE_RESERVE          0x08 /* VP   being reserved by scheduler */
#define INUSE_JOB              0x10 /* VP   in use by job (exclusive use) */
#define INUSE_JOBSHARE         0x20 /* VP   is use by job(s) (time shared) */
#define INUSE_BUSY             0x40 /* Node is busy (high loadave)  */

#define INUSE_UNKNOWN          0x100 /* Node has not been heard from yet */
#define INUSE_NEEDS_HELLO_PING 0x200  /*node needs to be informed of a*/
/*new qmgr created node         */
#define INUSE_SUBNODE_MASK     0xff /* bits both in nd_state and inuse */
#define INUSE_COMMON_MASK  (INUSE_OFFLINE|INUSE_DOWN)
/* state bits that go from node to subn */

/*
 * NTYPE_ are used in node.nd_type
 */
#define NTYPE_CLUSTER  0x00 /* Node is normal allocatable node */
#define NTYPE_TIMESHARED 0x01 /* Node is Time Shared Node  */

#define TIMESHARED_SUFFIX "ts"
#define PBS_MAXNODENAME 80 /* upper bound on the node name size    */

#define PBSNODE_STATE  0x1   /* characteristic code */
#define PBSNODE_PROPERTIES 0x2   /* characteristic code */
#define PBSNODE_NTYPE  0x3   /* characteristic code */
#define PBSNODE_NTYPE_MASK 0xf   /* relevant ntype bits */

#define WRITENODE_STATE  0x1   /*associated w/ offline*/
#define WRITE_NEW_NODESFILE 0x2 /*changed: deleted,ntype,or properties*/
#define WRITENODE_NOTE   0x4   /*associated w/ note*/

/*
 * Although at the present time a struct pbssnode doesn't have an array of
 * attributes associated with it, it is convenient for the server's req_manager
 * function to use as much of the existing pbs_attribute processing functions and
 * philosophy as practical to carry out any node modification requests sent via
 * "qmgr".   For this reason, and for the fact that some day struct pbssnodes
 * may be redefined to possess attributes, it is convenient to pretend that
 * nodes have an associated attributes array just as a server or a queue or
 * a job would have.
 *
 * The following enum defines the index into the node_attr_def[]  array.
 */

enum nodeattr
  {
  ND_ATR_state,
  ND_ATR_np,
  ND_ATR_properties,
  ND_ATR_ntype,
  ND_ATR_jobs,
  ND_ATR_status,
  ND_ATR_note,
  ND_ATR_mom_port,
  ND_ATR_mom_rm_port,
  ND_ATR_num_node_boards,
  ND_ATR_numa_str,
  ND_ATR_gpus,
  ND_ATR_gpustatus,
  ND_ATR_gpus_str,
  ND_ATR_LAST
  }; /* WARNING: Must be the highest valued enum */



enum node_types
  {
  ND_TYPE_CRAY,
  ND_TYPE_EXTERNAL,
  ND_TYPE_LOGIN
  };


typedef struct node_check_info
  {
  struct prop *first;
  struct prop *first_status;
  short        state;
  short        ntype;
  int          nprops;
  int          nstatus;
  char        *note;
  } node_check_info;




extern all_nodes allnodes; 
extern struct pbsnode *alps_reporter;

extern int    svr_totnodes;  /* number of nodes (hosts) */
extern int    svr_tsnodes;  /* number of timeshared nodes */
extern int    svr_clnodes;  /* number of cluster nodes */

extern int    MultiMomMode; /* moms configured for multiple moms per machine */

extern int update_nodes_file(struct pbsnode *);

extern void bad_node_warning(pbs_net_t, struct pbsnode *);

struct pbsnode  *find_nodebyname(char *);
struct pbsnode  *find_node_in_allnodes(all_nodes *an, char *nodename);
int              create_partial_pbs_node(char *, unsigned long, int);
struct pbssubn  *create_subnode(struct pbsnode *pnode);

#ifdef BATCH_REQUEST_H 
void             initialize_pbssubn(struct pbsnode *, struct pbssubn *, struct prop *);
void             effective_node_delete(struct pbsnode *);
void             setup_notification(char *);

struct pbssubn  *find_subnodebyname(char *);

struct pbsnode  *find_nodebynameandaltname(char *, char *);
void             free_prop_list(struct prop*);
void             free_prop_attr(pbs_attribute*);
void             recompute_ntype_cnts();
int              create_pbs_node(char *, svrattrl *, int, int *);
int              mgr_set_node_attr(struct pbsnode *, attribute_def *, int, svrattrl *, int, int *, void *, int);
void            *send_hierarchy_file(void *);

node_iterator   *get_node_iterator();
void             reinitialize_node_iterator(node_iterator *);

struct prop     *init_prop(char *pname);
#endif /* BATCH_REQUEST_H */

int              initialize_pbsnode(struct pbsnode *, char *pname, u_long *pul, int ntype);
int              hasprop(struct pbsnode *pnode, struct prop *props);

#endif /* PBS_NODES_H */ 
