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
char *msg_err_malloc = "malloc failed";
struct connection svr_conn[PBS_NET_MAX_CONNECTIONS];
tlist_head svr_newjobs;
char *msg_request = "Type %s request received from %s@%s, sock=%d";
AvlTree okclients = NULL;
int LOGLEVEL = 1;
char log_buffer[LOG_BUF_SIZE];


void req_rerunjob(struct batch_request *preq)
  {
  fprintf(stderr, "The call to req_rerunjob needs to be mocked!!\n");
  exit(1);
  }

void req_shutdown(struct batch_request *preq)
  {
  fprintf(stderr, "The call to req_shutdown needs to be mocked!!\n");
  exit(1);
  }

void job_purge(job *pjob)
  {
  fprintf(stderr, "The call to job_purge needs to be mocked!!\n");
  exit(1);
  }

void mom_server_update_receive_time_by_ip(u_long ipaddr, char *command_name)
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
  fprintf(stderr, "The call to get_connecthost needs to be mocked!!\n");
  exit(1);
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

void log_record(int eventtype, int objclass, char *objname, char *text)
  {
  fprintf(stderr, "The call to log_record needs to be mocked!!\n");
  exit(1);
  }

void req_jobscript(struct batch_request *preq)
  {
  fprintf(stderr, "The call to req_jobscript needs to be mocked!!\n");
  exit(1);
  }

void req_stat_job(struct batch_request *preq)
  {
  fprintf(stderr, "The call to req_stat_job needs to be mocked!!\n");
  exit(1);
  }

int dis_request_read(int sfds, struct batch_request *request)
  {
  fprintf(stderr, "The call to dis_request_read needs to be mocked!!\n");
  exit(1);
  }

void DIS_tcp_setup(int fd)
  {
  fprintf(stderr, "The call to DIS_tcp_setup needs to be mocked!!\n");
  exit(1);
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

void req_reject(int code, int aux, struct batch_request *preq, char *HostName, char *Msg)
  {
  fprintf(stderr, "The call to req_reject needs to be mocked!!\n");
  exit(1);
  }

void *get_next(list_link pl, char *file, int line)
  {
  fprintf(stderr, "The call to get_next needs to be mocked!!\n");
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
 
void req_quejob(struct batch_request *preq)
  {
  fprintf(stderr, "The call to req_quejob needs to be mocked!!\n");
  exit(1);
  }

char *pbse_to_txt(int err)
  {
  fprintf(stderr, "The call to pbse_to_txt needs to be mocked!!\n");
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

void req_returnfiles(struct batch_request *preq)
  {
  fprintf(stderr, "The call to req_returnfiles needs to be mocked!!\n");
  exit(1);
  }

void log_event(int eventtype, int objclass, char *objname, char *text)
  {
  fprintf(stderr, "The call to log_event needs to be mocked!!\n");
  exit(1);
  }

void req_deletejob(struct batch_request *preq)
  {
  fprintf(stderr, "The call to req_deletejob needs to be mocked!!\n");
  exit(1);
  }

void log_err(int errnum, char *routine, char *text)
  {
  fprintf(stderr, "The call to log_err needs to be mocked!!\n");
  exit(1);
  }

void req_jobcredential(struct batch_request *preq)
  {
  fprintf(stderr, "The call to req_jobcredential needs to be mocked!!\n");
  exit(1);
  }

void close_conn(int sd, int has_mutex)
  {
  fprintf(stderr, "The call to close_conn needs to be mocked!!\n");
  exit(1);
  }

void mom_req_holdjob(struct batch_request *preq)
  {
  fprintf(stderr, "The call to mom_req_holdjob needs to be mocked!!\n");
  exit(1);
  }

const char *reqtype_to_txt(int reqtype)
  {
  fprintf(stderr, "The call to reqtype_to_txt needs to be mocked!!\n");
  exit(1);
  }

void req_messagejob(struct batch_request *preq)
  {
  fprintf(stderr, "The call to req_messagejob needs to be mocked!!\n");
  exit(1);
  }

void net_add_close_func(int sd, void (*func)(int), int has_mutex)
  {
  fprintf(stderr, "The call to net_add_close_func needs to be mocked!!\n");
  exit(1);
  }



