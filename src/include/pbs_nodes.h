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

/* NOTE:  requires server_limits.h */

#define BM_ERROR    -20

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

  struct job *job;

  struct jobinfo *next;
  };

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
  struct job     *pjob;   /* job on this gpu subnode */
  unsigned short  inuse;  /* 1 if this node is in use, 0 otherwise */
  enum gpstatit	  state;  /* gpu state determined by server */
  enum gpmodeit   mode;   /* gpu mode from hardware */
  int             driver_ver;  /* Driver version reported from hardware */
  enum psit       flag;   /* same as for pbssubn */
  short           index;  /* gpu index */
  char           *gpuid;  /* gpu id */
  };

struct pbsnode
  {
  char   *nd_name; /* node's host name */

  struct pbssubn *nd_psn; /* ptr to list of subnodes */

  struct prop           *nd_first; /* first and last property */

  struct prop         *nd_last;

  struct prop           *nd_f_st;       /* first and last status */

  struct prop           *nd_l_st;
  u_long  *nd_addrs; /* IP addresses of host */

  struct array_strings *nd_prop; /* array of properities */

  struct array_strings  *nd_status;
  char           *nd_note;  /* note set by administrator */
  int     nd_stream; /* RPP stream to Mom on host */
  enum psit   nd_flag;
  short     nd_nprops; /* number of properties */
  short                  nd_nstatus;    /* number of status items */
  short     nd_nsn; /* number of VPs  */
  short     nd_nsnfree; /* number of VPs free */
  short    nd_nsnshared; /* number of VPs shared */
  short    nd_needed; /* number of VPs needed */
  unsigned short  nd_state;      /* node state (see INUSE_* #defines below) */
  unsigned short  nd_ntype; /* node type */
  short    nd_order; /* order of user's request */
  time_t                 nd_warnbad;
  time_t                 nd_lastupdate; /* time of last update. */

  short                 nd_ngpus;        /* number of gpus */ 
  short                 nd_gpus_real;    /* gpus are real not virtual */ 
  struct gpusubn       *nd_gpusn;        /* gpu subnodes */
  short                 nd_ngpus_free;   /* number of free gpus */
  short                 nd_ngpus_needed; /* number of gpus needed */
  struct array_strings *nd_gpustatus;    /* string array of GPU status */
  short                 nd_ngpustatus;    /* number of gpu status items */
  };

struct howl
  {
  char        *name;
  int          order;
  int          index;

  struct howl *next;
  };


typedef struct tree_t
  {
  u_long          key;

  struct pbsnode *nodep;

  struct tree_t  *left;

  struct tree_t  *right;
  } tree;

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

#define INUSE_FREE  0x00 /* Node/VP is available   */
#define INUSE_OFFLINE  0x01 /* Node was removed by administrator */
#define INUSE_DOWN  0x02 /* Node is down/unresponsive   */
#define INUSE_DELETED  0x04 /* Node is "deleted"   */
#define INUSE_RESERVE  0x08 /* VP   being reserved by scheduler */
#define INUSE_JOB  0x10 /* VP   in use by job (exclusive use) */
#define INUSE_JOBSHARE  0x20 /* VP   is use by job(s) (time shared) */
#define INUSE_BUSY  0x40 /* Node is busy (high loadave)  */

#define INUSE_UNKNOWN  0x100 /* Node has not been heard from yet */
#define INUSE_NEEDS_HELLO_PING 0x200  /*node needs to be informed of a*/
/*new qmgr created node         */
#define INUSE_SUBNODE_MASK 0xff /* bits both in nd_state and inuse */
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
 * function to use as much of the existing attribute processing functions and
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
  ND_ATR_gpus,
  ND_ATR_gpustatus,
  ND_ATR_LAST
  }; /* WARNING: Must be the highest valued enum */


extern struct pbsnode **pbsndmast;  /* array of ptr to nodes  */

extern struct pbsnode **pbsndlist;  /* array of ptr to nodes  */
extern int    svr_totnodes;  /* number of nodes (hosts) */
extern int    svr_tsnodes;  /* number of timeshared nodes */
extern int    svr_clnodes;  /* number of cluster nodes */

extern struct tree_t  *ipaddrs;

extern struct tree_t  *streams;

extern int update_nodes_file(void);

extern void bad_node_warning(pbs_net_t);
extern int addr_ok(pbs_net_t);

#ifdef BATCH_REQUEST_H
extern void initialize_pbssubn(struct pbsnode *, struct pbssubn *, struct prop *);
extern void effective_node_delete(struct pbsnode *);
extern void setup_notification(char *);

extern struct pbssubn  *find_subnodebyname(char *);

extern struct pbsnode  *find_nodebyname(char *);
extern void free_prop_list(struct prop*);
extern void free_prop_attr(attribute*);
extern void recompute_ntype_cnts();
extern  int create_pbs_node(char *, svrattrl *, int, int *);
extern  int create_partial_pbs_node(char *, unsigned long, int);
extern  int mgr_set_node_attr(struct pbsnode *, attribute_def *, int, svrattrl *, int, int *, void *, int);

struct prop  *init_prop(char *pname);
#endif /* BATCH_REQUEST_H */

#endif /* PBS_NODES_H */ 
