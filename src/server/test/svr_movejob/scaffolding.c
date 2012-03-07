#include "license_pbs.h" /* See here for the software license */
#include <stdlib.h>
#include <stdio.h> /* fprintf */

#include "libpbs.h" /* connect_handle */
#include "attribute.h" /* attribute */
#include "net_connect.h" /* pbs_net_t */
#include "pbs_job.h" /* job */
#include "batch_request.h" /* batch_request */
#include "queue.h" /* pbs_queue */
#include "pbs_nodes.h" /* pbsnode */
#include "list_link.h" /* tlist_head, list_link */

char *path_jobs;
struct connect_handle connection[10];
attribute_def job_attr_def[10];
char *pbs_o_host = "PBS_O_HOST";
char *msg_routexceed = "Route queue lifetime exceeded";
int queue_rank = 0;
char *path_spool;
char *msg_movejob = "Job moved to ";
pbs_net_t pbs_server_addr;
unsigned int pbs_server_port_dis;
int LOGLEVEL = 0;
char *msg_manager = "%s at request of %s@%s";



void finish_sendmom(job *pjob, struct batch_request *preq, long start_time, char *node_name, int status)
  {
  fprintf(stderr, "The call to finish_sendmom to be mocked!!\n");
  exit(1);
  }

int PBSD_commit_get_sid(int connect, long *sid, char *jobid)
  {
  fprintf(stderr, "The call to PBSD_commit_get_sid to be mocked!!\n");
  exit(1);
  }

pbs_queue *find_queuebyname(char *quename)
  {
  fprintf(stderr, "The call to find_queuebyname to be mocked!!\n");
  exit(1);
  }

char *parse_servername(char *name, unsigned int *service)
  {
  fprintf(stderr, "The call to parse_servername to be mocked!!\n");
  exit(1);
  }

int PBSD_jobfile(int c, int req_type, char *path, char *jobid, enum job_file which)
  {
  fprintf(stderr, "The call to PBSD_jobfile to be mocked!!\n");
  exit(1);
  }

int job_save(job *pjob, int updatetype, int mom_port)
  {
  fprintf(stderr, "The call to job_save to be mocked!!\n");
  exit(1);
  }

void job_purge(job *pjob)
  {
  fprintf(stderr, "The call to job_purge to be mocked!!\n");
  exit(1);
  }

int PBSD_jscript(int c, char *script_file, char *jobid)
  {
  fprintf(stderr, "The call to PBSD_jscript to be mocked!!\n");
  exit(1);
  }

pbs_net_t get_hostaddr(int *local_errno, char *hostname)
  {
  fprintf(stderr, "The call to get_hostaddr to be mocked!!\n");
  exit(1);
  }

void remove_stagein(job *pjob)
  {
  fprintf(stderr, "The call to remove_stagein to be mocked!!\n");
  exit(1);
  }

pbs_queue *get_jobs_queue(job **pjob)
  {
  fprintf(stderr, "The call to get_jobs_queue to be mocked!!\n");
  exit(1);
  }

void reply_ack(struct batch_request *preq)
  {
  fprintf(stderr, "The call to reply_ack to be mocked!!\n");
  exit(1);
  }

char *get_variable(job *pjob, char *variable)
  {
  fprintf(stderr, "The call to get_variable to be mocked!!\n");
  exit(1);
  }

int enqueue_threadpool_request(void *(*func)(void *),void *arg)
  {
  fprintf(stderr, "The call to enqueue_threadpool_request to be mocked!!\n");
  exit(1);
  }

void remove_checkpoint(job *pjob)
  {
  fprintf(stderr, "The call to remove_checkpoint to be mocked!!\n");
  exit(1);
  }

void svr_dequejob(job *pjob, int val)
  {
  fprintf(stderr, "The call to svr_dequejob to be mocked!!\n");
  exit(1);
  }

int unlock_node(struct pbsnode *the_node, char *id, char *msg, int logging)
  {
  fprintf(stderr, "The call to unlock_node to be mocked!!\n");
  exit(1);
  }

void svr_disconnect(int handle)
  {
  fprintf(stderr, "The call to svr_disconnect to be mocked!!\n");
  exit(1);
  }

void req_reject(int code, int aux, struct batch_request *preq, char *HostName, char *Msg)
  {
  fprintf(stderr, "The call to req_reject to be mocked!!\n");
  exit(1);
  }

int job_abt(job **pjobp, char *text)
  {
  fprintf(stderr, "The call to job_abt to be mocked!!\n");
  exit(1);
  }

void *get_next(list_link pl, char *file, int line)
  {
  fprintf(stderr, "The call to get_next to be mocked!!\n");
  exit(1);
  }

int job_route(job *jobp)
  {
  fprintf(stderr, "The call to job_route to be mocked!!\n");
  exit(1);
  }

int svr_enquejob(job *pjob, int has_sv_qs_mutex, int prev_index)
  {
  fprintf(stderr, "The call to svr_enquejob to be mocked!!\n");
  exit(1);
  }

void attrl_fixlink(tlist_head *phead)
  {
  fprintf(stderr, "The call to attrl_fixlink to be mocked!!\n");
  exit(1);
  }

int svr_connect(pbs_net_t hostaddr, unsigned int port, struct pbsnode *pnode, void *(*func)(void *), enum conn_type cntype)
  {
  fprintf(stderr, "The call to svr_connect to be mocked!!\n");
  exit(1);
  }

struct pbsnode *PGetNodeFromAddr(pbs_net_t addr)
  {
  fprintf(stderr, "The call to PGetNodeFromAddr to be mocked!!\n");
  exit(1);
  }

int PBSD_rdytocmt(int connect, char *jobid)
  {
  fprintf(stderr, "The call to PBSD_rdytocmt to be mocked!!\n");
  exit(1);
  }

void add_dest(job *jobp)
  {
  fprintf(stderr, "The call to add_dest to be mocked!!\n");
  exit(1);
  }

int svr_setjobstate(job *pjob, int newstate, int newsubstate, int  has_queue_mute)
  {
  fprintf(stderr, "The call to svr_setjobstate to be mocked!!\n");
  exit(1);
  }

job *find_job(char *jobid)
  {
  fprintf(stderr, "The call to find_job to be mocked!!\n");
  exit(1);
  }

int unlock_queue(struct pbs_queue *the_queue, const char *method_name, char *msg, int logging)
  {
  fprintf(stderr, "The call to unlock_queue to be mocked!!\n");
  exit(1);
  }

void svr_evaljobstate(job *pjob, int *newstate, int *newsub, int forceeval)
  {
  fprintf(stderr, "The call to svr_evaljobstate to be mocked!!\n");
  exit(1);
  }

char *PBSD_queuejob(int connect, int *local_errno, char *jobid, char *destin, struct attropl *attrib, char *extend)
  {
  fprintf(stderr, "The call to PBSD_queuejob to be mocked!!\n");
  exit(1);
  }

void delete_link(struct list_link *old) {}

char *pbse_to_txt(int err)
  {
  fprintf(stderr, "The call to pbse_to_txt to be mocked!!\n");
  exit(1);
  }

int svr_chkque(job *pjob, pbs_queue *pque, char *hostname, int mtype, char *EMsg)
  {
  fprintf(stderr, "The call to svr_chkque to be mocked!!\n");
  exit(1);
  }

int lock_queue(struct pbs_queue *the_queue, const char *method_name, char *msg, int logging)
  {
  fprintf(stderr, "The call to lock_queue to be mocked!!\n");
  exit(1);
  }

int get_parent_dest_queues(char *queue_parent_name, char *queue_dest_name, pbs_queue **parent, pbs_queue **dest, job **pjob_ptr)
  {
  fprintf(stderr, "The call to get_parent_dest_queues to be mocked!!\n");
  exit(1);
  }
