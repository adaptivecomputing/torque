#include "license_pbs.h" /* See here for the software license */
#include <stdlib.h>
#include <stdio.h> /* fprintf */
#include <time.h> /* time_t */

#include "server_limits.h" /* PBS_NET_MAX_CONNECTIONS, pbs_net_t */
#include "net_connect.h" /* connection */
#include "list_link.h" /* tlist_head, list_link */
#include "u_tree.h" /* AvlTree */
#include "log.h" /* LOG_BUF_SIZE */
#include "batch_request.h" /* batch_request */
#include "pbs_job.h" /* job */

time_t time_now;
const char *msg_err_malloc = "malloc failed";
struct connection svr_conn[PBS_NET_MAX_CONNECTIONS];
tlist_head svr_newjobs;
const char *msg_request = "Type %s request received from %s@%s, sock=%d";
AvlTree okclients = NULL;
int LOGLEVEL = 1;
char log_buffer[LOG_BUF_SIZE];


void req_rerunjob(batch_request *preq)
  {
  fprintf(stderr, "The call to req_rerunjob needs to be mocked!!\n");
  exit(1);
  }

void req_shutdown(struct batch_request *preq)
  {
  fprintf(stderr, "The call to req_shutdown needs to be mocked!!\n");
  exit(1);
  }

void mom_job_purge(job *pjob)
  {
  fprintf(stderr, "The call to mom_job_purge needs to be mocked!!\n");
  exit(1);
  }

void mom_server_update_receive_time_by_ip(u_long ipaddr, const char *command_name)
  {
  fprintf(stderr, "The call to mom_server_update_receive_time_by_ip needs to be mocked!!\n");
  exit(1);
  }

void req_modifyjob(struct batch_request *preq)
  {
  fprintf(stderr, "The call to req_modifyjob needs to be mocked!!\n");
  exit(1);
  }

int get_connecthost(int sock, char *namebuf, int size)
  {
  return(0);
  }

pbs_net_t get_connectaddr(int sock, int mutex)
  {
  fprintf(stderr, "The call to get_connectaddr needs to be mocked!!\n");
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

int req_stat_job(struct batch_request *preq)
  {
  fprintf(stderr, "The call to req_stat_job needs to be mocked!!\n");
  exit(1);
  }

int dis_request_read(struct tcp_chan *chan, struct batch_request *request)
  {
  return(0);
  }

struct tcp_chan *DIS_tcp_setup(int fd)
  {
  static struct tcp_chan chan;
  return(&chan);
  }

void req_rdytocommit(struct batch_request *preq)
  {
  fprintf(stderr, "The call to req_rdytocommit needs to be mocked!!\n");
  exit(1);
  }

int AVL_is_in_tree_no_port_compare(u_long key, uint16_t port, AvlTree tree)
  {
  fprintf(stderr, "The call to AVL_is_in_tree_no_port_compare needs to be mocked!!\n");
  exit(1);
  }

void req_reject(int code, int aux, struct batch_request *preq, const char *HostName, const char *Msg)
  {
  }

void *get_next(list_link pl, char *file, int line)
  {
  return(NULL);
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

void req_delfile(struct batch_request *preq)
  {
  fprintf(stderr, "The call to req_delfile needs to be mocked!!\n");
  exit(1);
  }
 
void req_signaljob(struct batch_request *preq)
  {
  fprintf(stderr, "The call to req_signaljob needs to be mocked!!\n");
  exit(1);
  }

void req_cpyfile(struct batch_request *preq)
  {
  fprintf(stderr, "The call to req_cpyfile needs to be mocked!!\n");
  exit(1);
  }
 
char *pbse_to_txt(int err)
  {
  fprintf(stderr, "The call to pbse_to_txt needs to be mocked!!\n");
  exit(1);
  }

void append_link(tlist_head *head, list_link *new_link, void *pobj)
  {
  }

void req_mvjobfile(struct batch_request *preq)
  {
  fprintf(stderr, "The call to req_mvjobfile needs to be mocked!!\n");
  exit(1);
  }

void req_returnfiles(struct batch_request *preq)
  {
  fprintf(stderr, "The call to req_returnfiles needs to be mocked!!\n");
  exit(1);
  }

void req_deletejob(struct batch_request *preq)
  {
  fprintf(stderr, "The call to req_deletejob needs to be mocked!!\n");
  exit(1);
  }

void req_jobcredential(struct batch_request *preq)
  {
  fprintf(stderr, "The call to req_jobcredential needs to be mocked!!\n");
  exit(1);
  }

void close_conn(int sd, int has_mutex)
  {
  }

void mom_req_holdjob(struct batch_request *preq)
  {
  fprintf(stderr, "The call to mom_req_holdjob needs to be mocked!!\n");
  exit(1);
  }

const char *reqtype_to_txt(int reqtype)
  {
  return("Queue_Job");
  }

void req_messagejob(struct batch_request *preq)
  {
  fprintf(stderr, "The call to req_messagejob needs to be mocked!!\n");
  exit(1);
  }

void net_add_close_func(int sd, void (*func)(int))
  {
  fprintf(stderr, "The call to net_add_close_func needs to be mocked!!\n");
  exit(1);
  }

void DIS_tcp_cleanup(struct tcp_chan *chan) {}

int mom_req_stat_job(

  struct batch_request *preq) 

  {
  return(0);
  }

void mom_req_quejob(batch_request *preq) {}

void req_delete_reservation(struct batch_request *request) {}
void req_change_power_state(struct batch_request*){}

void log_err(int errnum, const char *routine, const char *text) {}
void log_record(int eventtype, int objclass, const char *objname, const char *text) {}
void log_event(int eventtype, int objclass, const char *objname, const char *text) {}
void log_ext(int type, const char *func_name, const char *msg, int o) {}
