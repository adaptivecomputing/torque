#include "license_pbs.h" /* See here for the software license */
#include <stdlib.h>
#include <stdio.h> /* fprintf */
#include <netinet/in.h> /* sockaddr_in */

#include "attribute.h" /* attribute_def, attribute, svrattrl */
#include "pbs_ifl.h" /* PBS_MAXSERVERNAME */
#include "list_link.h" /* tlist_head, list_link */
#include "resource.h" /* resource_def, resource */
#include "server.h" /* server */
#include "batch_request.h" /* batch_request */
#include "pbs_job.h" /* job */
#include "u_tree.h" /* AvlTree */
#include "net_connect.h" /* pbs_net_t */
#include "pbs_nodes.h" /* pbsnode, all_nodes, node_iterator */
#include "work_task.h" /* work_task, work_type */

int SvrNodeCt = 0; 
int svr_resc_size = 0;
char *path_nodestate;
int allow_any_mom = FALSE;
unsigned int pbs_mom_port = 0;
attribute_def job_attr_def[10];
char server_name[PBS_MAXSERVERNAME + 1];
char *path_nodenote;
const char *dis_emsg[10];
tlist_head svr_newnodes; 
resource_def *svr_resc_def;
attribute_def node_attr_def[2];
char *path_nodenote_new;
struct server server;
int LOGLEVEL = 0;




struct batch_request *alloc_br(int type)
  {
  fprintf(stderr, "The call to alloc_br needs to be mocked!!\n");
  exit(1);
  }

void DIS_tcp_reset(int fd, int i)
  {
  fprintf(stderr, "The call to DIS_tcp_reset needs to be mocked!!\n");
  exit(1);
  }

int ctnodes(char *spec)
  {
  fprintf(stderr, "The call to ctnodes needs to be mocked!!\n");
  exit(1);
  }

char * netaddr(struct sockaddr_in *ap)
  {
  fprintf(stderr, "The call to netaddr needs to be mocked!!\n");
  exit(1);
  }

int modify_job_attr(job *pjob, svrattrl *plist, int perm, int *bad)
  {
  fprintf(stderr, "The call to modify_job_attr needs to be mocked!!\n");
  exit(1);
  }

int create_partial_pbs_node(char *nodename, unsigned long addr, int perms)
  {
  fprintf(stderr, "The call to create_partial_pbs_node needs to be mocked!!\n");
  exit(1);
  }

AvlTree AVL_delete_node(u_long key, uint16_t port, AvlTree tree)
  {
  fprintf(stderr, "The call to AVL_delete_node needs to be mocked!!\n");
  exit(1);
  }

void *process_Dreply(void *new_sock)
  {
  fprintf(stderr, "The call to process_Dreply needs to be mocked!!\n");
  exit(1);
  }

void log_record(int eventtype, int objclass, char *objname, char *text)
  {
  fprintf(stderr, "The call to log_record needs to be mocked!!\n");
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

int enqueue_threadpool_request(void *(*func)(void *), void *arg)
  {
  fprintf(stderr, "The call to enqueue_threadpool_request needs to be mocked!!\n");
  exit(1);
  }

struct pbsnode *find_nodebyname(char *nodename)
  {
  fprintf(stderr, "The call to find_nodebyname needs to be mocked!!\n");
  exit(1);
  }

struct work_task *set_task(enum work_type type, long event_id, void (*func)(), void *parm, int get_lock)
  {
  fprintf(stderr, "The call to set_task needs to be mocked!!\n");
  exit(1);
  }

unsigned disrui(int stream, int *retval)
  {
  fprintf(stderr, "The call to disrui needs to be mocked!!\n");
  exit(1);
  }

void svr_disconnect(int handle)
  {
  fprintf(stderr, "The call to svr_disconnect needs to be mocked!!\n");
  exit(1);
  }

struct pbsnode *next_host(all_nodes *an, int *iter, struct pbsnode *held)
  {
  fprintf(stderr, "The call to next_host needs to be mocked!!\n");
  exit(1);
  }

void log_ext(int errnum, char *routine, char *text, int severity)
  {
  fprintf(stderr, "The call to log_ext needs to be mocked!!\n");
  exit(1);
  }

struct pbsnode *next_node(all_nodes *an, struct pbsnode *current, node_iterator *iter)
  {
  fprintf(stderr, "The call to next_node needs to be mocked!!\n");
  exit(1);
  }

int DIS_tcp_wflush(int fd)
  {
  fprintf(stderr, "The call to DIS_tcp_wflush needs to be mocked!!\n");
  exit(1);
  }

struct prop *init_prop(char *pname)
  {
  fprintf(stderr, "The call to init_prop needs to be mocked!!\n");
  exit(1);
  }

int node_status_list(attribute *new, void *pnode, int actmode)
  {
  fprintf(stderr, "The call to node_status_list needs to be mocked!!\n");
  exit(1);
  }

int write_tcp_reply(int sock, int protocol, int version, int command, int exit_code)
  {
  fprintf(stderr, "The call to write_tcp_replwrite_tcp_reply needs to be mocked!!\n");
  exit(1);
  }

int issue_Drequest(int conn, struct batch_request *request, void (*func)(struct work_task *), struct work_task **ppwt)
  {
  fprintf(stderr, "The call to issue_Drequest needs to be mocked!!\n");
  exit(1);
  }

struct pbsnode *AVL_find(u_long key, uint16_t port, AvlTree tree)
  {
  fprintf(stderr, "The call to AVL_find needs to be mocked!!\n");
  exit(1);
  }

node_iterator *get_node_iterator()
  {
  fprintf(stderr, "The call to get_node_iterator needs to be mocked!!\n");
  exit(1);
  }

resource_def *find_resc_def(resource_def *rscdf, char *name, int limit)
  {
  fprintf(stderr, "The call to find_resc_def needs to be mocked!!\n");
  exit(1);
  }

int decode_arst(struct attribute *patr, char *name, char *rescn, char *val, int perm)
  {
  fprintf(stderr, "The call to decode_arst needs to be mocked!!\n");
  exit(1);
  }

char *disrst(int stream, int *retval)
  {
  fprintf(stderr, "The call to disrst needs to be mocked!!\n");
  exit(1);
  }

void release_req(struct work_task *pwt)
  {
  fprintf(stderr, "The call to release_req needs to be mocked!!\n");
  exit(1);
  }

void append_link(tlist_head *head, list_link *new, void *pobj)
  {
  fprintf(stderr, "The call to append_link needs to be mocked!!\n");
  exit(1);
  }

void free_arst(struct attribute *attr)
  {
  fprintf(stderr, "The call to free_arst needs to be mocked!!\n");
  exit(1);
  }

int svr_connect(pbs_net_t hostaddr, unsigned int port, struct pbsnode *pnode, void *(*func)(void *), enum conn_type cntype)
  {
  fprintf(stderr, "The call to svr_connect needs to be mocked!!\n");
  exit(1);
  }

int PNodeStateToString(int SBM, char *Buf, int BufSize)
  {
  fprintf(stderr, "The call to PNodeStateToString needs to be mocked!!\n");
  exit(1);
  }

void log_event(int eventtype, int objclass, char *objname, char *text)
  {
  fprintf(stderr, "The call to log_event needs to be mocked!!\n");
  exit(1);
  }

void log_err(int errnum, char *routine, char *text)
  {
  fprintf(stderr, "The call to log_err needs to be mocked!!\n");
  exit(1);
  }

int diswul(int stream, unsigned long value)
  {
  fprintf(stderr, "The call to diswul needs to be mocked!!\n");
  exit(1);
  }

resource *find_resc_entry(attribute *pattr, resource_def *rscdf)
  {
  fprintf(stderr, "The call to find_resc_entry needs to be mocked!!\n");
  exit(1);
  }

job *find_job(char *jobid)
  {
  fprintf(stderr, "The call to find_job needs to be mocked!!\n");
  exit(1);
  }

int update_nodes_file(struct pbsnode *held)
  {
  fprintf(stderr, "The call to update_nodes_file needs to be mocked!!\n");
  exit(1);
  }

int diswsi(int stream, int value)
  {
  fprintf(stderr, "The call to diswsi needs to be mocked!!\n");
  exit(1);
  }

int disrsi(int stream, int *retval)
  {
  fprintf(stderr, "The call to disrsi needs to be mocked!!\n");
  exit(1);
  }

void reinitialize_node_iterator(node_iterator *iter)
  {
  fprintf(stderr, "The call to reinitialize_node_iterator needs to be mocked!!\n");
  exit(1);
  }

int unlock_node(struct pbsnode *the_node, char *id, char *msg, int logging)
  { 
  fprintf(stderr, "The call to unlock_node needs to be mocked!!\n");
  exit(1);      
  }        

int lock_node(struct pbsnode *the_node, char *id, char *msg, int logging)
  { 
  fprintf(stderr, "The call to lock_node needs to be mocked!!\n");
  exit(1);                            
  }        

void socket_read_flush(int socket)
  { 
  fprintf(stderr, "The call to socket_read_flush needs to be mocked!!\n");
  exit(1);                            
  }        

void close_conn(int sock,int has_mut)
  { 
  fprintf(stderr, "The call to close_conn needs to be mocked!!\n");
  exit(1);                            
  }        
