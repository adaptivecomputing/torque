#include "license_pbs.h" /* See here for the software license */
#include <stdlib.h>
#include <stdio.h> /* fprintf */
#include <pthread.h> /* pthread_mutex_t */

#include "pbs_nodes.h" /* all_nodes, pbsnode */
#include "queue.h" /* all_queues */
#include "attribute.h" /* attribute_def, svrattrl */
#include "server.h" /* server */
#include "pbs_job.h" /* all_jobs, job */
#include "batch_request.h" /* batch_request */
#include "array.h" /* job_array */
#include "list_link.h" /* tlist_head, list_link */
#include "work_task.h" /* work_task, work_type */
#include "u_tree.h" /* AvlTree */

all_nodes allnodes;
pthread_mutex_t *netrates_mutex = NULL;
all_queues svr_queues;
char server_name[PBS_MAXSERVERNAME + 1];
struct all_jobs alljobs;
attribute_def que_attr_def[10];
attribute_def node_attr_def[2];
struct server server;
struct all_jobs array_summary;
attribute_def svr_attr_def[10];
int svr_totnodes = 0;
int LOGLEVEL = 0;

struct batch_request *alloc_br(int type)
  {
  fprintf(stderr, "The call to alloc_br to be mocked!!\n");
  exit(1);
  }

pbs_queue *find_queuebyname(char *quename)
  {
  fprintf(stderr, "The call to find_queuebyname to be mocked!!\n");
  exit(1);
  }

int job_save(job *pjob, int updatetype, int mom_port)
  {
  fprintf(stderr, "The call to job_save to be mocked!!\n");
  exit(1);
  }

void svr_mailowner(job *pjob, int mailpoint, int force, char *text)
  {
  fprintf(stderr, "The call to svr_mailowner to be mocked!!\n");
  exit(1);
  }

svrattrl *attrlist_create(char *aname, char *rname, int vsize)
  {
  fprintf(stderr, "The call to attrlist_create to be mocked!!\n");
  exit(1);
  }

int modify_job_attr(job *pjob, svrattrl *plist, int perm, int *bad)
  {
  fprintf(stderr, "The call to modify_job_attr to be mocked!!\n");
  exit(1);
  }

int status_nodeattrib(svrattrl *pal, attribute_def *padef, struct pbsnode *pnode, int limit, int priv, tlist_head *phead, int *bad)
  {
  fprintf(stderr, "The call to status_nodeattrib to be mocked!!\n");
  exit(1);
  }

int reply_send(struct batch_request *request)
  {
  fprintf(stderr, "The call to reply_send to be mocked!!\n");
  exit(1);
  }

void *process_Dreply(void *new_sock)
  {
  fprintf(stderr, "The call to process_Dreply to be mocked!!\n");
  exit(1);
  }

void log_record(int eventtype, int objclass, char *objname, char *text)
  {
  fprintf(stderr, "The call to log_record to be mocked!!\n");
  exit(1);
  }

void free_br(struct batch_request *preq)
  {
  fprintf(stderr, "The call to free_br to be mocked!!\n");
  exit(1);
  }

job *find_array_template(char *arrayid)
  {
  fprintf(stderr, "The call to find_array_template to be mocked!!\n");
  exit(1);
  }

int lock_node(struct pbsnode *the_node, char *id, char *msg, int logging)
  {
  fprintf(stderr, "The call to lock_node to be mocked!!\n");
  exit(1);
  }

struct pbsnode *find_nodebyname(char *nodename)
  {
  fprintf(stderr, "The call to find_nodebyname to be mocked!!\n");
  exit(1);
  }

struct work_task *set_task(enum work_type type, long event_id, void (*func)(), void *parm, int get_lock)
  {
  fprintf(stderr, "The call to set_task to be mocked!!\n");
  exit(1);
  }

int unlock_node(struct pbsnode *the_node, char *id, char *msg, int logging)
  {
  fprintf(stderr, "The call to unlock_node to be mocked!!\n");
  exit(1);
  }

job_array *get_array(char *id)
  {
  fprintf(stderr, "The call to get_array to be mocked!!\n");
  exit(1);
  }

void svr_disconnect(int handle)
  {
  fprintf(stderr, "The call to svr_disconnect to be mocked!!\n");
  exit(1);
  }

struct pbsnode *next_host(all_nodes *an, int *iter, struct pbsnode *held)
  {
  fprintf(stderr, "The call to next_host to be mocked!!\n");
  exit(1);
  }

void req_reject(int code, int aux, struct batch_request *preq, char *HostName, char *Msg)
  {
  fprintf(stderr, "The call to req_reject to be mocked!!\n");
  exit(1);
  }

job *next_job(struct all_jobs *aj, int *iter)
  {
  fprintf(stderr, "The call to next_job to be mocked!!\n");
  exit(1);
  }

void rel_resc(job *pjob)
  {
  fprintf(stderr, "The call to rel_resc to be mocked!!\n");
  exit(1);
  }

void delete_task(struct work_task *ptask)
  {
  fprintf(stderr, "The call to delete_task to be mocked!!\n");
  exit(1);
  }

int status_job(job *pjob, struct batch_request *preq, svrattrl *pal, tlist_head *pstathd, int *bad)
  {
  fprintf(stderr, "The call to status_job to be mocked!!\n");
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

int issue_Drequest(int conn, struct batch_request *request, void (*func)(struct work_task *), struct work_task **ppwt)
  {
  fprintf(stderr, "The call to issue_Drequest to be mocked!!\n");
  exit(1);
  }

void reply_free(struct batch_reply *prep)
  {
  fprintf(stderr, "The call to reply_free to be mocked!!\n");
  exit(1);
  }

struct pbsnode *AVL_find(u_long key, uint16_t port, AvlTree tree)
  {
  fprintf(stderr, "The call to AVL_find to be mocked!!\n");
  exit(1);
  }

int *netcounter_get(void)
  {
  fprintf(stderr, "The call to netcounter_get to be mocked!!\n");
  exit(1);
  }

int hasprop(struct pbsnode *pnode, struct prop *props)
  {
  fprintf(stderr, "The call to hasprop to be mocked!!\n");
  exit(1);
  }

void release_req(struct work_task *pwt)
  {
  fprintf(stderr, "The call to release_req to be mocked!!\n");
  exit(1);
  }

void append_link(tlist_head *head, list_link *new, void *pobj)
  {
  fprintf(stderr, "The call to append_link to be mocked!!\n");
  exit(1);
  }

pbs_queue *next_queue(all_queues *aq, int *iter)
  {
  fprintf(stderr, "The call to next_queue to be mocked!!\n");
  exit(1);
  }

int svr_connect(pbs_net_t hostaddr, unsigned int port, struct pbsnode *pnode, void *(*func)(void *), enum conn_type cntype)
  {
  fprintf(stderr, "The call to svr_connect to be mocked!!\n");
  exit(1);
  }

int is_array(char *id)
  {
  fprintf(stderr, "The call to is_array to be mocked!!\n");
  exit(1);
  }

void log_event(int eventtype, int objclass, char *objname, char *text)
  {
  fprintf(stderr, "The call to log_event to be mocked!!\n");
  exit(1);
  }

void update_array_statuses(job_array *owned)
  {
  fprintf(stderr, "The call to update_array_statuses to be mocked!!\n");
  exit(1);
  }

struct pbsnode *tfind_addr(const u_long key, uint16_t port, job *pjob)
  {
  fprintf(stderr, "The call to tfind_addr to be mocked!!\n");
  exit(1);
  }

int svr_setjobstate(job *pjob, int newstate, int newsubstate)
  {
  fprintf(stderr, "The call to svr_setjobstate to be mocked!!\n");
  exit(1);
  }

job *find_job(char *jobid)
  {
  fprintf(stderr, "The call to find_job to be mocked!!\n");
  exit(1);
  }

int unlock_queue(struct pbs_queue *the_queue, char *method_name, char *msg, int logging)
  {
  fprintf(stderr, "The call to unlock_queue to be mocked!!\n");
  exit(1);
  }

void reply_badattr(int code, int aux, svrattrl *pal, struct batch_request *preq)
  {
  fprintf(stderr, "The call to reply_badattr to be mocked!!\n");
  exit(1);
  }

int insert_task(all_tasks *at, work_task *wt, int object)
  {
  fprintf(stderr, "The call to insert_task to be mocked!!\n");
  exit(1);
  }

int status_attrib(svrattrl *pal, attribute_def *padef, attribute *pattr, int limit, int priv, tlist_head *phead, int *bad, int IsOwner)
  {
  fprintf(stderr, "The call to status_attrib to be mocked!!\n");
  exit(1);
  }
