#include "license_pbs.h" /* See here for the software license */
#include <stdlib.h>
#include <stdio.h> /* fprintf */

#include "libpbs.h" /* connect_handle */
#include "net_connect.h" /* connection */
#include "work_task.h" /* all_tasks */
#include "batch_request.h" /* batch_request */
#include "pbs_ifl.h" /* attropl */
#include "net_connect.h" /* pbs_net_t */
#include "list_link.h" /* tlist_head, list_link */

int pbs_errno = 0;
char *msg_daemonname = "unset";
struct connect_handle connection[10];
struct connection svr_conn[PBS_NET_MAX_CONNECTIONS];
char *msg_norelytomom = "Server could not connect to MOM";
unsigned int pbs_server_port_dis;
int LOGLEVEL = 0;
all_tasks task_list_event;
char *msg_issuebad = "attempt to issue invalid request of type %d";

char *parse_servername(char *name, unsigned int *service)
  {
  fprintf(stderr, "The call to parse_servername needs to be mocked!!\n");
  exit(1);
  }

int encode_DIS_Register(struct tcp_chan *chan, struct batch_request *preq)
  {
  fprintf(stderr, "The call to encode_DIS_Register needs to be mocked!!\n");
  exit(1);
  }

int PBSD_gpu_put(int c, char *node, char *gpuid, int gpumode, int reset_perm, int reset_vol, char *extend)
  {
  fprintf(stderr, "The call to PBSD_gpu_put needs to be mocked!!\n");
  exit(1);
  }

int PBSD_mgr_put(int c, int function, int command, int objtype, char *objname, struct attropl *aoplp, char *extend)
  {
  fprintf(stderr, "The call to PBSD_mgr_put needs to be mocked!!\n");
  exit(1);
  }

int encode_DIS_JobId(struct tcp_chan *chan, char *jobid)
  {
  fprintf(stderr, "The call to encode_DIS_JobId needs to be mocked!!\n");
  exit(1);
  }

pbs_net_t get_hostaddr(int *local_errno, char *hostname)
  {
  fprintf(stderr, "The call to get_hostaddr needs to be mocked!!\n");
  exit(1);
  }

int DIS_reply_read(struct tcp_chan *chan, struct batch_reply *preply)
  {
  fprintf(stderr, "The call to DIS_reply_read needs to be mocked!!\n");
  exit(1);
  }

char *netaddr_pbs_net_t(pbs_net_t ipadd)
  {
  fprintf(stderr, "The call to netaddr_pbs_net_t needs to be mocked!!\n");
  exit(1);
  }

void free_br(struct batch_request *preq)
  {
  fprintf(stderr, "The call to free_br needs to be mocked!!\n");
  exit(1);
  }

int encode_DIS_ReturnFiles(struct tcp_chan *chan, struct batch_request *preq)
  {
  fprintf(stderr, "The call to encode_DIS_ReturnFiles needs to be mocked!!\n");
  exit(1);
  }

struct work_task *set_task(enum work_type type, long event_id, void (*func)(), void *parm, int get_lock)
  {
  fprintf(stderr, "The call to set_task needs to be mocked!!\n");
  exit(1);
  }

void DIS_tcp_setup(int fd)
  {
  fprintf(stderr, "The call to DIS_tcp_setup needs to be mocked!!\n");
  exit(1);
  }

int encode_DIS_TrackJob(struct tcp_chan *chan, struct batch_request *br)
  {
  fprintf(stderr, "The call to encode_DIS_TrackJob needs to be mocked!!\n");
  exit(1);
  }

void svr_disconnect(int handle)
  {
  fprintf(stderr, "The call to svr_disconnect needs to be mocked!!\n");
  exit(1);
  }

int DIS_tcp_wflush(int fd)
  {
  fprintf(stderr, "The call to DIS_tcp_wflush needs to be mocked!!\n");
  exit(1);
  }

void delete_task(struct work_task *ptask)
  {
  fprintf(stderr, "The call to delete_task needs to be mocked!!\n");
  exit(1);
  }

void *get_next(list_link pl, char *file, int line)
  {
  fprintf(stderr, "The call to get_next needs to be mocked!!\n");
  exit(1);
  }

int encode_DIS_CopyFiles(struct tcp_chan *chan, struct batch_request *preq)
  {
  fprintf(stderr, "The call to encode_DIS_CopyFiles needs to be mocked!!\n");
  exit(1);
  }

int encode_DIS_ReqHdr(struct tcp_chan *chan, int reqt, char *user)
  {
  fprintf(stderr, "The call to encode_DIS_ReqHdr needs to be mocked!!\n");
  exit(1);
  }

void attrl_fixlink(tlist_head *phead)
  {
  fprintf(stderr, "The call to attrl_fixlink needs to be mocked!!\n");
  exit(1);
  }

int PBSD_status_put(int c, int function, char *id, struct attrl *attrib, char *extend)
  {
  fprintf(stderr, "The call to PBSD_status_put needs to be mocked!!\n");
  exit(1);
  }

work_task *next_task(all_tasks *at, int *iter)
  {
  fprintf(stderr, "The call to next_task needs to be mocked!!\n");
  exit(1);
  }

int svr_connect(pbs_net_t hostaddr, unsigned int port, struct pbsnode *pnode, void *(*func)(void *), enum conn_type cntype)
  {
  fprintf(stderr, "The call to svr_connect needs to be mocked!!\n");
  exit(1);
  }

void dispatch_task(struct work_task *ptask)
  {
  fprintf(stderr, "The call to dispatch_task needs to be mocked!!\n");
  exit(1);
  }

int dispatch_request(int sfds, struct batch_request *request)
  {
  fprintf(stderr, "The call to dispatch_request needs to be mocked!!\n");
  exit(1);
  }

int PBSD_msg_put(int c, char *jobid, int fileopt, char *msg, char *extend)
  {
  fprintf(stderr, "The call to PBSD_msg_put needs to be mocked!!\n");
  exit(1);
  }

void close_conn(int sd, int has_mutex)
  {
  fprintf(stderr, "The call to close_conn needs to be mocked!!\n");
  exit(1);
  }

struct pbsnode *tfind_addr(const u_long key, uint16_t port, job *pjob)
  {
  fprintf(stderr, "The call to tfind_addr needs to be mocked!!\n");
  exit(1);
  }

int encode_DIS_ReqExtend(struct tcp_chan *chan, char *extend)
  {
  fprintf(stderr, "The call to encode_DIS_ReqExtend needs to be mocked!!\n");
  exit(1);
  }

int PBSD_sig_put(int c, char *jobid, char *signal, char *extend)
  {
  fprintf(stderr, "The call to PBSD_sig_put needs to be mocked!!\n");
  exit(1);
  }

int unlock_node(struct pbsnode *the_node, char *id, char *msg, int logging)
  { 
  fprintf(stderr, "The call to unlock_node needs to be mocked!!\n");
  exit(1);                    
  }

void DIS_tcp_cleanup(struct tcp_chan *chan)
  {
  }

batch_request *get_remove_batch_request(

  char *br_id)

  {
  return(NULL);
  }

int get_batch_request_id(

  batch_request *preq)

  {
  return(0);
  }

job *svr_find_job(char *jobid, int get_subjob)
  {
  return(NULL);
  }

int unlock_ji_mutex(job *pjob, const char *id, char *msg, int logging)
  {
  return(0);
  }
