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
#include <map>
#include <set>
#include <sys/types.h>
#include <vector>
#include <string>

#include "execution_slot_tracker.hpp"
#include "net_connect.h" /* pbs_net_t */
#include "pbs_ifl.h" /* resource_t */

/* NOTE:  requires server_limits.h */

#include "container.hpp"
#include "job_usage_info.hpp"
#include "attribute.h"
#ifdef PENABLE_LINUX_CGROUPS
#include "machine.hpp"
#endif
#include "runjob_help.hpp"
#include "attribute.h"

#ifdef NUMA_SUPPORT
/* NOTE: cpuset support needs hwloc */
#  include <hwloc.h>
#endif

#define NUMA_KEYWORD           "numa"
#define START_GPU_STATUS       "<gpu_status>"
#define END_GPU_STATUS         "</gpu_status>"
#define START_MIC_STATUS       "<mic_status>"
#define END_MIC_STATUS         "</mic_status>"

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

#define NODE_POWER_CHANGE_TIMEOUT 30

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
  gpu_unknown,
  gpu_mode_not_set
  };

enum gpstatit
  {
  gpu_unallocated,
  gpu_shared,
  gpu_exclusive,
  gpu_unavailable
  };

class gpusubn
  {
  public:
  int             job_internal_id; /* internal id of job on gpu */
  bool            inuse;  /* 1 if this node is in use, 0 otherwise */
  enum gpstatit   state;  /* gpu state determined by server */
  enum gpmodeit   mode;   /* gpu mode from hardware */
  int             driver_ver;  /* Driver version reported from hardware */
  enum psit       flag;
  short           index;  /* gpu index */
  std::string     gpuid;  /* gpu id */
  int             job_count;

  gpusubn() : job_internal_id(-1), inuse(false), state(gpu_unallocated), mode(gpu_normal),
              driver_ver(-1), flag(okay), index(-1), gpuid(), job_count(0)
    {
    }

  gpusubn(int gindex) : job_internal_id(-1), inuse(false), state(gpu_unallocated), mode(gpu_normal),
              driver_ver(-1), flag(okay), index(gindex), gpuid(), job_count(0)
    {
    }
  };



#ifdef NUMA_SUPPORT
typedef struct nodeboard_t
  {
  int                index;           /* the node's index */
  int                num_cpus;        /* count of cpus */
  int                num_nodes;       /* count of NUMA nodes */
  hwloc_bitmap_t     cpuset;          /* bitmap containing CPU IDs of this nodeboard */
  hwloc_bitmap_t     nodeset;         /* bitmap containing NUMA node IDs of this nodeboard */
  unsigned long      memsize;
  char             **path_meminfo;    /* path to meminfo file for each NUMA node */
  unsigned long long pstat_busy;
  unsigned long long pstat_idle;
  int                mic_start_index; /* index of first mic for this board */
  int                mic_end_index;   /* index of last mic for this board */
  int                gpu_start_index; /* index of first gpu for this board */
  int                gpu_end_index;   /* index of last gpu for this board */
  float              cpuact;
  } nodeboard;
#endif /* NUMA_SUPPORT */




#define SEND_HELLO 11

/* container for holding communication information */
class received_node
  {
  public:
  std::string              hostname;
  std::vector<std::string> statuses;
  int                      hellos_sent;
  };


class pbsnode
  {
private:
  std::string                          nd_name;             /* node's host name */
  int                                  nd_error;            // set if there's an error
  std::vector<std::string>             nd_properties;       // The node's properties
  int                                  nd_version;          // The node's software version
  std::map<std::string, unsigned int>  nd_plugin_generic_resources; // Plugin-supplied gres
  std::map<std::string, double>        nd_plugin_generic_metrics; // Plugin-supplied gmetrics
  std::map<std::string, std::string>   nd_plugin_varattrs; // Plugin-supplied varattrs
  std::string                          nd_plugin_features;

public:
  // Network failures without two consecutive successive between them.
  int                           nd_proximal_failures;
  // Consecutive succesful network transactions
  int                           nd_consecutive_successes;
  pthread_mutex_t               nd_mutex;            // mutex for accessing this node 
  int                           nd_id;               /* node's id */

  std::vector<prop>             nd_f_st;
 

  std::vector<u_long>           nd_addrs;            /* IP addresses of host */

  struct array_strings         *nd_prop;             /* array of properities */

  std::string                   nd_status;
  std::string                   nd_note;             /* note set by administrator */
  int                           nd_stream;           /* stream to Mom on host */
  enum psit                     nd_flag;
  unsigned short                nd_mom_port;         /* For multi-mom-mode unique port value PBS_MOM_SERVICE_PORT*/
  unsigned short                nd_mom_rm_port;   /* For multi-mom-mode unique port value PBS_MANAGER_SERVICE_PORT */
  struct sockaddr_in            nd_sock_addr;        /* address information */
  short                         nd_nprops;           /* number of properties */
  short                         nd_nstatus;          /* number of status items */
  execution_slot_tracker        nd_slots;            /* bitmap of execution slots */
  std::vector<job_usage_info >  nd_job_usages;       /* information about each job using this node */
  short                         nd_needed;           /* number of VPs needed */
  short                         nd_np_to_be_used;    /* number of VPs marked for a job but not yet assigned */
  unsigned short                nd_state;            /* node state (see INUSE_* #defines below) */
  unsigned short                nd_ntype;            /* node type */
  short                         nd_order;            /* order of user's request */
  time_t                        nd_warnbad;
  time_t                        nd_lastupdate;       /* time of last update. */
  time_t                        nd_lastHierarchySent; /* last time the hierarchy was sent to this node. */
  unsigned short                nd_hierarchy_level;
  unsigned char                 nd_in_hierarchy;     /* set to TRUE if in the hierarchy file */

  short                         nd_ngpus;            /* number of gpus */ 
  short                         nd_gpus_real;        /* gpus are real not virtual */ 
  std::vector<gpusubn>          nd_gpusn;            /* gpu subnodes */
  short                         nd_ngpus_free;       /* number of free gpus */
  short                         nd_ngpus_needed;     /* number of gpus needed */
  short                         nd_ngpus_to_be_used; /* number of gpus marked for a job but not yet assigned */
  struct array_strings         *nd_gpustatus;        /* string array of GPU status */
  short                         nd_ngpustatus;       /* number of gpu status items */

  short                         nd_nmics;            /* number of mics */
  struct array_strings         *nd_micstatus;        /* string array of MIC status */
  std::vector<int>              nd_micjobids;        /* ids for the jobs on the mic(s) */
  short                         nd_nmics_alloced;    /* number of mic slots alloc'ed */
  short                         nd_nmics_free;       /* number of free mics */
  short                         nd_nmics_to_be_used; /* number of mics marked for a job but not yet assigned */
 
  pbsnode                      *parent;              /* pointer to the node holding this node, or NULL */
  unsigned short                num_node_boards;     /* number of numa nodes */
  struct AvlNode               *node_boards;         /* private tree of numa nodes */
  std::string                   numa_str;            /* comma-delimited string of processor values */
  std::string                   gpu_str;             /* comma-delimited string of the number of gpus for each nodeboard */

  unsigned char                 nd_mom_reported_down;/* notes that the mom reported its own shutdown */
  
  unsigned char                 nd_is_alps_reporter;
  unsigned char                 nd_is_alps_login;
  std::vector<std::string>       *nd_ms_jobs;          /* the jobs this node is mother superior for */
  container::item_container<struct pbsnode *> *alps_subnodes;       /* collection of alps subnodes */
  int                           max_subnode_nppn;    /* maximum ppn of an alps subnode */
  unsigned short              nd_power_state;
  unsigned char               nd_mac_addr[6];
  time_t                        nd_power_state_change_time; //
  char                          nd_ttl[32];
  struct array_strings         *nd_acl;
  std::string                   nd_requestid;
  unsigned char               nd_tmp_unlock_count;    /*Nodes will get temporarily unlocked so that
                                                       further processing can happen, but the function
                                                       doing the unlock intends to lock it again
                                                       so we need a flag here to prevent a node from being
                                                       deleted while it is temporarily locked. */

  /* numa hardware configuration information */
#ifdef PENABLE_LINUX_CGROUPS
  Machine                    nd_layout;
#endif


  pbsnode();
  pbsnode(const pbsnode &other);
  pbsnode(const char *pname, u_long *pul, bool skip_address_lookup);
  ~pbsnode();
  pbsnode &operator =(const pbsnode &other);
  bool operator ==(const pbsnode &other);

  // CONST methods
  int         get_error() const;
  const char *get_name() const;
  bool        hasprop(std::vector<prop> *props) const;
  void        write_compute_node_properties(FILE *nin) const;
  void        write_to_nodes_file(FILE *nin) const;
  int         copy_properties(pbsnode *dest) const;
  int         get_version() const;
  void        add_plugin_resources(tlist_head *phead) const;

  // NON-CONST methods
  void change_name(const char *new_name);
  void set_version(const char *version_str);
  void update_properties();
  bool update_internal_failure_counts(int rc);
  void add_property(const std::string &prop);
  int tmp_lock_node(const char *method_name, const char *msg, int logging);
  int tmp_unlock_node(const char *method_name, const char *msg, int logging);
  int lock_node(const char *method_name, const char *msg, int logging);
  int unlock_node(const char *method_name, const char *msg, int logging);
  int encode_properties(tlist_head *);
  void copy_gpu_subnodes(const pbsnode &src);
  void remove_node_state_flag(int flag);
  void capture_plugin_resources(const char *str);
  void add_job_list_to_status(const std::string &job_list);
  };


typedef container::item_container<struct pbsnode *>                all_nodes;
typedef container::item_container<struct pbsnode *>::item_iterator all_nodes_iterator;

/* struct used for iterating numa nodes */
typedef struct node_iterator
  {
  all_nodes_iterator *node_index;
  int                numa_index;
  all_nodes_iterator *alps_index;
  } node_iterator;

#define INITIAL_NODE_SIZE  20



void            initialize_all_nodes_array(all_nodes *);
int             insert_node(all_nodes *,struct pbsnode *);
int             remove_node(all_nodes *,struct pbsnode *);
struct pbsnode *next_node(all_nodes *,struct pbsnode *,node_iterator *);
struct pbsnode *next_host(all_nodes *,all_nodes_iterator **,struct pbsnode *);
int             copy_properties(struct pbsnode *dest, struct pbsnode *src);
bool            node_exists(const char *node_name);
void            update_failure_counts(const char *node_name, int rc);


#if 0
#define HELLO_RESEND_WAIT_TIME 10

class hello_info
  {
  public:
    int     id;
    time_t  last_retry;
    int     num_retries;
  hello_info(int ident) : id(ident), last_retry(0), num_retries(0) {}
  };



typedef container::item_container<hello_info *>                 hello_container;
typedef container::item_container<hello_info *>::item_iterator  hello_container_iterator;

int         needs_hello(hello_container *, char *);
int         add_hello(hello_container *, int);
int         add_hello_after(hello_container *, int, int);
int         add_hello_info(hello_container *, hello_info *);
hello_info *pop_hello(hello_container *);
int         remove_hello(hello_container *, int);
#endif

int         send_hierarchy(char *, unsigned short);
void       *send_hierarchy_threadtask(void *);


//extern hello_container  hellos;



class howl
  {
  public:
  std::string     hostname;
  int             order;
  int             index;
  unsigned short  port;
  howl(const std::string &name) : hostname(name) {}
  howl(const std::string &name, int o, int i, unsigned int p) : hostname(name), order(o), index(i),
                                                                port(p) {}
  howl() : hostname(), order(-1), index(-1), port(0) {}
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
/* 0x20 was used for sharing processors, but this is no longer supported */
#define INUSE_BUSY             0x40 /* Node is busy (high loadave)  */
#define INUSE_NOHIERARCHY      0x80 /* The node has not been sent the hiearchy yet. */

#define INUSE_NOT_READY       (INUSE_DOWN|INUSE_NOHIERARCHY)

#define INUSE_UNKNOWN          0x100 /* Node has not been heard from yet */
#define INUSE_NETWORK_FAIL     0x200 /* Node has had too many network failures */
#define INUSE_SUBNODE_MASK     0xfff /* bits both in nd_state and inuse */
#define INUSE_COMMON_MASK  (INUSE_OFFLINE|INUSE_DOWN)

/* Node power state defines */

#define POWER_STATE_RUNNING    0 //Up and running
#define POWER_STATE_STANDBY    1 //Linux string "standby"
#define POWER_STATE_SUSPEND    2 //Linux string "mem"
#define POWER_STATE_SLEEP      3 //Linux not supported
#define POWER_STATE_HIBERNATE  4 //Linux string "disk"
#define POWER_STATE_SHUTDOWN   5 //Must be done via ACPI

/* state bits that go from node to subn */

/*
 * NTYPE_ are used in node.nd_type
 */
#define NTYPE_CLUSTER  0x00 /* Node is normal allocatable node */
/* 0x01 was used for Time Shared Node but this doens't exist anymore */

#define TIMESHARED_SUFFIX "ts"
#define PBS_MAXNODENAME 80 /* upper bound on the node name size    */

#define PBSNODE_STATE  0x1   /* characteristic code */
#define PBSNODE_PROPERTIES 0x2   /* characteristic code */
#define PBSNODE_NTYPE  0x3   /* characteristic code */
#define PBSNODE_NTYPE_MASK 0xf   /* relevant ntype bits */

#define WRITENODE_STATE  0x1   /*associated w/ offline*/
#define WRITE_NEW_NODESFILE 0x2 /*changed: deleted,ntype,or properties*/
#define WRITENODE_NOTE   0x4   /*associated w/ note*/
#define WRITENODE_POWER_STATE 0x8

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
  ND_ATR_power_state,
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
  ND_ATR_gpus_str,
  ND_ATR_gpustatus,
  ND_ATR_mics,
  ND_ATR_micstatus,
  ND_ATR_ttl,
  ND_ATR_acl,
  ND_ATR_requestid,
#ifdef PENABLE_LINUX_CGROUPS
  ND_ATR_total_sockets,
  ND_ATR_total_numa_nodes,
  ND_ATR_total_cores,
  ND_ATR_total_threads,
  ND_ATR_dedicated_sockets,
  ND_ATR_dedicated_numa_nodes,
  ND_ATR_dedicated_cores,
  ND_ATR_dedicated_threads,
#endif
  ND_ATR_LAST
  }; /* WARNING: Must be the highest valued enum */



enum node_types
  {
  ND_TYPE_CRAY,
  ND_TYPE_EXTERNAL,
  ND_TYPE_LOGIN
  };


class node_check_info
  {
public:
  std::vector<std::string>  properties;
  prop                     *first_status;
  short        state;
  short        ntype;
  int          nprops;
  int          nstatus;
  std::string  note;
  short        power_state;
  unsigned char ttl[32];
  int          acl_size;
  std::string  rqid;
  };


class sync_job_info
  {
  public:
  std::string job_info;
  std::string node_name;
  time_t  timestamp;
  bool    sync_jobs;

  sync_job_info() : job_info(), node_name(), sync_jobs(false)
    {
    this->timestamp = time(NULL);
    }
  } ;



extern all_nodes allnodes; 
extern struct pbsnode *alps_reporter;

extern int    svr_totnodes;  /* number of nodes (hosts) */
extern int   svr_unresolvednodes;
extern int    svr_clnodes;  /* number of cluster nodes */

extern int    MultiMomMode; /* moms configured for multiple moms per machine */

extern int update_nodes_file(struct pbsnode *);

struct pbsnode  *tfind_addr(const u_long key, uint16_t port, char *job_momname);
struct pbsnode  *find_nodebyname(const char *);
struct pbsnode  *find_nodebyid(int);
struct pbsnode  *find_node_in_allnodes(all_nodes *an, const char *nodename);
int              create_partial_pbs_node(char *, unsigned long, int);
int              add_execution_slot(struct pbsnode *pnode);
extern void      delete_a_subnode(struct pbsnode *pnode);
void             effective_node_delete(pbsnode **);
void             free_prop_list(struct prop*);

void             reinitialize_node_iterator(node_iterator *);
int              mgr_set_node_attr(struct pbsnode *, attribute_def *, int, svrattrl *, int, int *, void *, int, bool);

#ifdef BATCH_REQUEST_H 
void             setup_notification(char *);

struct pbsnode  *find_nodebynameandaltname(char *, char *);
void             free_prop_attr(pbs_attribute*);
void             recompute_ntype_cnts();
int              create_pbs_node(char *, svrattrl *, int, int *);
int              create_pbs_dynamic_node(char *, svrattrl *, int, int *);
void            *send_hierarchy_file(void *);

node_iterator   *get_node_iterator();

#endif /* BATCH_REQUEST_H */

struct prop     *init_prop(const char *pname);
void             update_node_state(struct pbsnode *np, int newstate);
int              is_job_on_node(struct pbsnode *np, int internal_job_id);
void            *sync_node_jobs(void *vp);

#endif /* PBS_NODES_H */ 
