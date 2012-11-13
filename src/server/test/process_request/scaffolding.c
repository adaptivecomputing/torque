#include "license_pbs.h" /* See here for the software license */
#include <stdlib.h>
#include <stdio.h> /* fprintf */

#include "server_limits.h" /* PBS_NET_MAX_CONNECTIONS */
#include "credential.h" /* credential */
#include "pbs_ifl.h" /* PBS_MAXSERVERNAME */
#include "net_connect.h" /* connection, pbs_net_t */
#include "server.h" /* server */
#include "batch_request.h" /* batch_request */
#include "pbs_job.h" /* job, all_jobs */
#include "list_link.h" /* list_link, tlist_head */
#include "pbs_nodes.h" /* pbsnode */
#include "attribute.h" /* pbs_attribute */

char *msg_err_noqueue = "Unable to requeue job, queue is not defined";
struct credential conn_credent[PBS_NET_MAX_CONNECTIONS];
char server_name[PBS_MAXSERVERNAME + 1];
char *msg_err_malloc = "malloc failed";
struct connection svr_conn[PBS_NET_MAX_CONNECTIONS];
char *msg_request = "Type %s request received from %s@%s, sock=%d";
struct server server;
char *server_host;
int LOGLEVEL = 0;



void *req_releasejob(void *vp)
  {
  fprintf(stderr, "The call to req_releasejob needs to be mocked!!\n");
  exit(1);
  }

void *req_rerunjob(void *vp)
  {
  fprintf(stderr, "The call to req_rerunjob needs to be mocked!!\n");
  exit(1);
  }

void *req_stat_svr(void *vp)
  {
  fprintf(stderr, "The call to req_stat_svr needs to be mocked!!\n");
  exit(1);
  }

void req_shutdown(struct batch_request *preq)
  {
  fprintf(stderr, "The call to req_shutdown needs to be mocked!!\n");
  exit(1);
  }

void *req_rescq(void *vp)
  {
  fprintf(stderr, "The call to req_rescq needs to be mocked!!\n");
  exit(1);
  }

int svr_job_purge(job *pjob)
  {
  fprintf(stderr, "The call to job_purge needs to be mocked!!\n");
  exit(1);
  }

void req_modifyjob(struct batch_request *preq)
  {
  fprintf(stderr, "The call to req_modifyjob needs to be mocked!!\n");
  exit(1);
  }

void req_rescfree(struct batch_request *preq)
  {
  fprintf(stderr, "The call to req_rescfree needs to be mocked!!\n");
  exit(1);
  }

int get_connecthost(int sock, char *namebuf, int size)
  {
  fprintf(stderr, "The call to get_connecthost needs to be mocked!!\n");
  exit(1);
  }

void *req_holdjob(void *vp)
  {
  fprintf(stderr, "The call to req_holdjob needs to be mocked!!\n");
  exit(1);
  }

void *req_registerarray(void *vp)
  {
  fprintf(stderr, "The call to req_registerarray needs to be mocked!!\n");
  exit(1);
  }

void delete_link(struct list_link *old)
  {
  fprintf(stderr, "The call to delete_link needs to be mocked!!\n");
  exit(1);
  }

void req_jobscript(struct batch_request *preq)
  {
  fprintf(stderr, "The call to req_jobscript needs to be mocked!!\n");
  exit(1);
  }

int svr_get_privilege(char *user, char *host)
  {
  fprintf(stderr, "The call to svr_get_privilege needs to be mocked!!\n");
  exit(1);
  }

int enqueue_threadpool_request(void *(*func)(void *),void *arg)
  {
  fprintf(stderr, "The call to enqueue_threadpool_request needs to be mocked!!\n");
  exit(1);
  }

void *req_jobobit(void *vp)
  {
  fprintf(stderr, "The call to req_jobobit needs to be mocked!!\n");
  exit(1);
  }

int *req_stat_job(struct batch_request *preq)
  {
  fprintf(stderr, "The call to req_stat_job needs to be mocked!!\n");
  exit(1);
  }

int dis_request_read(struct tcp_chan *chan, struct batch_request *request)
  {
  fprintf(stderr, "The call to dis_request_read needs to be mocked!!\n");
  exit(1);
  }

void *req_stat_que(void *vp)
  {
  fprintf(stderr, "The call to req_stat_que needs to be mocked!!\n");
  exit(1);
  }

void req_track(struct batch_request *preq)
  {
  fprintf(stderr, "The call to req_track needs to be mocked!!\n");
  exit(1);
  }

void *req_holdarray(void *vp)
  {
  fprintf(stderr, "The call to req_holdarray needs to be mocked!!\n");
  exit(1);
  }

void *req_movejob(void *vp)
  {
  fprintf(stderr, "The call to req_movejob needs to be mocked!!\n");
  exit(1);
  }

int unlock_node(struct pbsnode *the_node, const char *id, char *msg, int logging)
  {
  fprintf(stderr, "The call to unlock_node needs to be mocked!!\n");
  exit(1);
  }

void req_rdytocommit(struct batch_request *preq)
  {
  fprintf(stderr, "The call to req_rdytocommit needs to be mocked!!\n");
  exit(1);
  }

void req_reject(int code, int aux, struct batch_request *preq, char *HostName, char *Msg)
  {
  fprintf(stderr, "The call to req_reject needs to be mocked!!\n");
  exit(1);
  }

job *next_job(struct all_jobs *aj, int *iter)
  {
  fprintf(stderr, "The call to next_job needs to be mocked!!\n");
  exit(1);
  }

void *req_releasearray(void *vp)
  {
  fprintf(stderr, "The call to req_releasearray needs to be mocked!!\n");
  exit(1);
  }

void req_connect(struct batch_request *preq)
  {
  fprintf(stderr, "The call to req_connect needs to be mocked!!\n");
  exit(1);
  }

int job_abt(job **pjobp, char *text)
  {
  fprintf(stderr, "The call to job_abt needs to be mocked!!\n");
  exit(1);
  }

void *get_next(list_link pl, char *file, int line)
  {
  fprintf(stderr, "The call to get_next needs to be mocked!!\n");
  exit(1);
  }

int req_altauthenuser(struct batch_request *preq)
  {
  fprintf(stderr, "The call to req_altauthenuser needs to be mocked!!\n");
  exit(1);
  }

int svr_enquejob(job *pjob, int has_sv_qs_mutex, int prev_index)
  {
  fprintf(stderr, "The call to svr_enquejob needs to be mocked!!\n");
  exit(1);
  }

void req_checkpointjob(struct batch_request *preq)
  {
  fprintf(stderr, "The call to req_checkpointjob needs to be mocked!!\n");
  exit(1);
  }

void req_commit(struct batch_request *preq)
  {
  fprintf(stderr, "The call to req_commit needs to be mocked!!\n");
  exit(1);
  }

void reply_free(struct batch_reply *prep)
  {
  fprintf(stderr, "The call to reply_free needs to be mocked!!\n");
  exit(1);
  }

void free_attrlist(tlist_head *pattrlisthead)
  {
  fprintf(stderr, "The call to free_attrlist needs to be mocked!!\n");
  exit(1);
  }

void req_signaljob(struct batch_request *preq)
  {
  fprintf(stderr, "The call to req_signaljob needs to be mocked!!\n");
  exit(1);
  }

void *req_register(void *vp)
  {
  fprintf(stderr, "The call to req_register needs to be mocked!!\n");
  exit(1);
  }

void req_rescreserve(struct batch_request *preq)
  {
  fprintf(stderr, "The call to req_rescreserve needs to be mocked!!\n");
  exit(1);
  }

void req_quejob(struct batch_request *preq, char **job_id)
  {
  fprintf(stderr, "The call to req_quejob needs to be mocked!!\n");
  exit(1);
  }

void req_deletearray(struct batch_request *preq)
  {
  fprintf(stderr, "The call to req_deletearray needs to be mocked!!\n");
  exit(1);
  }

void *req_locatejob(void *vp)
  {
  fprintf(stderr, "The call to req_locatejob needs to be mocked!!\n");
  exit(1);
  }

char * csv_find_string(char *csv_str, char *search_str)
  {
  fprintf(stderr, "The call to csv_find_string needs to be mocked!!\n");
  exit(1);
  }

char *pbse_to_txt(int err)
  {
  fprintf(stderr, "The call to pbse_to_txt needs to be mocked!!\n");
  exit(1);
  }

void *req_stagein(void *vp)
  {
  fprintf(stderr, "The call to req_stagein needs to be mocked!!\n");
  exit(1);
  }

void *req_modifyarray(void *vp)
  {
  fprintf(stderr, "The call to req_modifyarray needs to be mocked!!\n");
  exit(1);
  }

void append_link(tlist_head *head, list_link *new, void *pobj)
  {
  fprintf(stderr, "The call to append_link needs to be mocked!!\n");
  exit(1);
  }

void req_mvjobfile(struct batch_request *preq)
  {
  fprintf(stderr, "The call to req_mvjobfile needs to be mocked!!\n");
  exit(1);
  }

int remove_job(struct all_jobs *aj, job *pjob)
  {
  fprintf(stderr, "The call to remove_job needs to be mocked!!\n");
  exit(1);
  }

int authenticate_user(struct batch_request *preq, struct credential *pcred, char **autherr)
  {
  fprintf(stderr, "The call to authenticate_user needs to be mocked!!\n");
  exit(1);
  }

struct pbsnode *PGetNodeFromAddr(pbs_net_t addr)
  {
  fprintf(stderr, "The call to PGetNodeFromAddr needs to be mocked!!\n");
  exit(1);
  }

void *req_manager(void *vp)
  {
  fprintf(stderr, "The call to req_manager needs to be mocked!!\n");
  exit(1);
  }

int is_array(char *id)
  {
  fprintf(stderr, "The call to is_array needs to be mocked!!\n");
  exit(1);
  }

void req_deletejob(struct batch_request *preq)
  {
  fprintf(stderr, "The call to req_deletejob needs to be mocked!!\n");
  exit(1);
  }

void *req_runjob(void *vp)
  {
  fprintf(stderr, "The call to req_runjob needs to be mocked!!\n");
  exit(1);
  }

void *req_jobcredential(void *vp)
  {
  fprintf(stderr, "The call to req_jobcredential needs to be mocked!!\n");
  exit(1);
  }

void close_conn(int sd, int has_mutex)
  {
  fprintf(stderr, "The call to close_conn needs to be mocked!!\n");
  exit(1);
  }

job *svr_find_job(char *jobid, int get_subjob)
  {
  fprintf(stderr, "The call to find_job needs to be mocked!!\n");
  exit(1);
  }

const char *reqtype_to_txt(int reqtype)
  {
  fprintf(stderr, "The call to reqtype_to_txt needs to be mocked!!\n");
  exit(1);
  }

void *req_orderjob(void *vp)
  {
  fprintf(stderr, "The call to req_orderjob needs to be mocked!!\n");
  exit(1);
  }

void req_messagejob(struct batch_request *preq)
  {
  fprintf(stderr, "The call to req_messagejob needs to be mocked!!\n");
  exit(1);
  }

void *req_gpuctrl_svr(void *vp)
  {
  fprintf(stderr, "The call to req_gpuctrl needs to be mocked!!\n");
  exit(1);
  }

void *req_stat_node(void *vp)
  {
  fprintf(stderr, "The call to req_stat_node needs to be mocked!!\n");
  exit(1);
  }

void net_add_close_func(int sd, void (*func)(int))
  {
  fprintf(stderr, "The call to net_add_close_func needs to be mocked!!\n");
  exit(1);
  }

int acl_check(pbs_attribute *pattr, char *name, int type)
  {
  fprintf(stderr, "The call to acl_check needs to be mocked!!\n");
  exit(1);
  }

pbs_net_t get_connectaddr(int sock, int mutex)
  {
  fprintf(stderr, "The call to get_connectaddr needs to be mocked!!\n");
  exit(1);
  }

void *req_authenuser(void *vp)
  {
  fprintf(stderr, "The call to req_authenuser needs to be mocked!!\n");
  exit(1);
  }

void *req_selectjobs(void *vp)
  {
  fprintf(stderr, "The call to req_selectjobs needs to be mocked!!\n");
  exit(1);
  }

int get_svr_attr_arst(int index, struct array_strings **arst)
  {
  return(0);
  }

int get_svr_attr_l(int index, long *l)
  {
  return(0);
  }

void memmgr_destroy(memmgr **mgr) {}

void *memmgr_calloc(memmgr **mgr, int qty, int size)
  {
  return(NULL);
  }

int memmgr_init(memmgr **mgr, int mgr_size)
  {
  return(0);
  }

int acl_check_my_array_string(struct array_strings *pas, char *name, int type)
  {
  return(0);
  }

void globalset_del_sock(int sock) {}

char * netaddr_long(long ap, char *out)
  {
  return(NULL);
  }

int remove_batch_request(

  char *br_id)

  {
  return(0);
  }

int unlock_ji_mutex(job *pjob, const char *id, char *msg, int logging)
  {
  return(0);
  }
