#ifndef _SVRFUNC_H
#define _SVRFUNC_H
#include "license_pbs.h" /* See here for the software license */
#include "queue.h" /* pbs_queue */
#include "batch_request.h"

/*
 * misc server function prototypes
 */

int get_svr_attr_l(int index, long *l);

extern int   chk_hold_priv(long hold, int priv);
extern void  get_jobowner(char *from, char *to); 
extern char *parse_servername(char *, unsigned int *);
extern void  process_Areply(int);
extern void  *mom_process_request(void *);
extern void  process_dis_request(int);
extern int   save_struct(char *, unsigned int, int, char *, size_t *, size_t);
extern void  schedule_job(void);
extern char *site_map_user(char *, char *);
extern int   socket_to_handle(int, int *);
extern void  svr_disconnect(int);
extern int   svr_get_privilege(char *, char *);
extern int   srv_shutdown(int);
extern void  write_node_state(void);
extern int  write_node_note(void);
extern int   setup_nodes(void);
extern int   node_avail(char *spec, int  *navail,
                              int *nalloc, int *nreserved, int *ndown);
extern void  node_unreserve(resource_t handle);
extern int   node_reserve(char *speclist, resource_t tag);
extern int   init_resc_defs(void);

int is_svr_attr_set(int);
int set_svr_attr(int, void *);

#ifdef PBS_JOB_H
extern int   set_nodes(job *, char *, int, char **, char **, char *, char *);
extern int   is_ts_node(char *);
extern char *find_ts_node(void);
extern void  free_nodes(job *);
#endif /* PBS_JOB_H */

#ifdef ATTRIBUTE_H
extern int   check_que_enable(pbs_attribute *, void *, int);
extern int   set_queue_type(pbs_attribute *, void *, int);
#ifdef QUEUE_H
extern int   chk_resc_limits(pbs_attribute *, pbs_queue *, char *);
#endif /* QUEUE_H */
#endif /* ATTRIBUTE_H */

#ifdef PBS_NET_H
struct pbsnode;
extern int   svr_connect(pbs_net_t, unsigned int, int *, struct pbsnode *, void *(*)(void *), enum conn_type);
#endif /* PBS_NET_H */

#ifdef WORK_TASK_H
extern void  release_req(struct work_task *);
#endif /* WORK_TASK_H */

extern int   issue_Drequest(int, struct batch_request *);


/* The following is used in req_stat.c and req_select.c */


struct select_list
  {

  struct select_list *sl_next; /* ptr to next in list   */
  enum batch_op       sl_op; /* comparison operator   */
  attribute_def      *sl_def; /* ptr to attr definition,for at_comp */
  int                 sl_atindx; /* index into attribute_def, for type */
  pbs_attribute       sl_attr; /* the pbs_attribute (value) */
  };

struct stat_cntl    /* used in req_stat_job */
  {
  int        sc_XXXX;
  int        sc_type;
  int        sc_XXXY;
  int        sc_conn;
  pbs_queue      *sc_pque;

  struct batch_request *sc_origrq;

  struct select_list   *sc_select;
  void (*sc_post)(struct stat_cntl *);
  char        sc_jobid[PBS_MAXSVRJOBID+1];
  };

/*
 * the following defines are due to the fact that O_SYNC was not
 * POSIX standard (TDISABLEFILESYNC managed w/configure --enable-filesync)
 */

#if TDISABLEFILESYNC
#define O_Sync 0
#else
#ifdef O_SYNC
#define O_Sync O_SYNC
#elif _FSYNC
#define O_Sync _FSYNC
#else
#define O_Sync 0
#endif /* O_SYNC */
#endif /* TDISABLEFILESYNC */

#endif /* _SVRFUNC_H */
