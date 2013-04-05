#include "license_pbs.h" /* See here for the software license */
#include <stdlib.h>
#include <stdio.h> /* fprintf */
#include <netinet/in.h> /* sockaddr_in */

#include "attribute.h" /* attribute_def, pbs_attribute, svrattrl */
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
int LOGLEVEL = 7; /* force logging code to be exercised as tests run */
hello_container hellos;
struct pbsnode reporter;
struct pbsnode *alps_reporter = &reporter;
const char *alps_reporter_feature  = "alps_reporter";
const char *alps_starter_feature   = "alps_login";


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

struct pbsnode *find_nodebyname(const char *nodename)
  {
  static struct pbsnode bob;

  memset(&bob, 0, sizeof(bob));

  if (!strcmp(nodename, "bob"))
    return(&bob);
  else
    return(NULL);
  }

struct pbsnode *find_node_in_allnodes(struct all_nodes *an, char *nodename)
  {
  static struct pbsnode cray;

  memset(&cray, 0, sizeof(cray));

  if (!strcmp(nodename, "cray"))
    return(&cray);
  else
    return(NULL);
  }

struct work_task *set_task(enum work_type type, long event_id, void (*func)(work_task *), void *parm, int get_lock)
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

int node_status_list(pbs_attribute *new_attr, void *pnode, int actmode)
  {
  fprintf(stderr, "The call to node_status_list needs to be mocked!!\n");
  exit(1);
  }

int write_tcp_reply(struct tcp_chan *chan, int protocol, int version, int command, int exit_code)
  {
  fprintf(stderr, "The call to write_tcp_replwrite_tcp_reply needs to be mocked!!\n");
  exit(1);
  }

int issue_Drequest(int conn, batch_request *br)
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

resource_def *find_resc_def(resource_def *rscdf, const char *name, int limit)
  {
  fprintf(stderr, "The call to find_resc_def needs to be mocked!!\n");
  exit(1);
  }

int decode_arst(struct pbs_attribute *patr, const char *name, const char *rescn, const char *val, int perm)
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

void append_link(tlist_head *head, list_link *new_link, void *pobj)
  {
  fprintf(stderr, "The call to append_link needs to be mocked!!\n");
  exit(1);
  }

void free_arst(struct pbs_attribute *attr)
  {
  fprintf(stderr, "The call to free_arst needs to be mocked!!\n");
  exit(1);
  }

int svr_connect(unsigned long, unsigned int, int*, pbsnode*, void* (*)(void*), conn_type)
  {
  fprintf(stderr, "The call to svr_connect needs to be mocked!!\n");
  exit(1);
  }

int PNodeStateToString(int SBM, char *Buf, int BufSize)
  {
  fprintf(stderr, "The call to PNodeStateToString needs to be mocked!!\n");
  exit(1);
  }

int diswul(int stream, unsigned long value)
  {
  fprintf(stderr, "The call to diswul needs to be mocked!!\n");
  exit(1);
  }

resource *find_resc_entry(pbs_attribute *pattr, resource_def *rscdf)
  {
  fprintf(stderr, "The call to find_resc_entry needs to be mocked!!\n");
  exit(1);
  }

job *svr_find_job(char *jobid, int get_subjob)
  {
  static job pjob;

  memset(&pjob, 0, sizeof(pjob));
  pjob.ji_mutex = (pthread_mutex_t *)calloc(1, sizeof(pthread_mutex_t));
  strcpy(pjob.ji_qs.ji_jobid, jobid);

  if ((!strcmp(jobid, "1")) ||
      (!strcmp(jobid, "5")))
    {
    pjob.ji_wattr[JOB_ATR_exec_host].at_val.at_str = strdup("tom/0");
    }
  else if (!strcmp(jobid, "4"))
    {
    return(NULL);
    }
  else if (strcmp(jobid, "2"))
    {
    pjob.ji_wattr[JOB_ATR_exec_host].at_val.at_str = strdup("bob/5");
    }
  else
    pjob.ji_wattr[JOB_ATR_exec_host].at_val.at_str = NULL;

  return(&pjob);
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

int unlock_node(struct pbsnode *the_node, const char *id, const char *msg, int logging)
  {
  return(0);
  }        

int lock_node(pbsnode *the_node, const char *id,const char *msg, int logging)
  {
  return(0);
  }        

void socket_read_flush(int socket) {}        

void close_conn(int sock,int has_mut) {}

int remove_hello(hello_container *hc, char *nodename)
  { 
  fprintf(stderr, "The call to remove_hello needs to be mocked!!\n");
  exit(1);                            
  }


void *send_hierarchy_threadtask(void *vp)
  { 
  fprintf(stderr, "The call to send_hierarchy_threadtask needs to be mocked!!\n");
  exit(1);                            
  }

dynamic_string *get_dynamic_string(int size, const char *str)
  {
  fprintf(stderr, "The call to get_dynamic_string needs to be mocked!!\n");
  exit(1);                            
  }

int copy_to_end_of_dynamic_string(dynamic_string *ds, const char *str)
  {
  fprintf(stderr, "The call to copy_to_end_of_dynamic_string needs to be mocked!!\n");
  exit(1);                            
  }

void free_dynamic_string(dynamic_string *ds)
  {
  }



char *threadsafe_tokenizer(char **str, const char *delims)
  {
  fprintf(stderr, "The call to threadsafe_tokenizer needs to be mocked!!\n");
  exit(1);
  }

int get_svr_attr_l(int index, long *l)
  {
  return(0);
  }

int process_alps_status(

  char           *nd_name,
  dynamic_string *status_info)

  {
  return(0);
  }

struct pbsnode *get_next_login_node(

  struct prop *needed)

  {
  return(NULL);
  }

char *get_cached_nameinfo(
    
  struct sockaddr_in  *sai)

  {
  return(NULL);
  }

int insert_addr_name_info(
    
  char               *hostname,
  char               *full_hostname,
  struct sockaddr_in *sai)

  {
  return(0);
  }

void *next_thing(resizable_array *ra, int *iter)
  {
  return(NULL);
  }

resizable_array *initialize_resizable_array(int size)
  {
  return(NULL);
  }

int handle_complete_first_time(job *pjob)
  {
  return(0);
  }

void free_resizable_array(resizable_array *ra) {}

void *remove_thing_memcmp(resizable_array *ra, void *thing, unsigned int size)
  {
  return(NULL);
  }

int insert_thing(resizable_array *ra, void *thing)
  {
  return(0);
  }

int svr_setjobstate(job *pjob, int newstate, int newsubstate, int has_queue_mutex)
  {
  return(0);
  }

void *pop_thing(resizable_array *ra)
  {
  return(NULL);
  }

int unlock_ji_mutex(job *pjob, const char *id, const char *msg, int logging)
  {
  return(0);
  }

struct pbssubn *create_subnode(struct pbsnode *pnode)

  {
  return(NULL);
  }

struct pbsnode *create_alps_subnode(

  struct pbsnode *parent,
  char           *node_id)

  {
  return(NULL);
  }

int append_dynamic_string(dynamic_string *ds, const char *str)
  {
  return(0);
  }

int append_char_to_dynamic_string(dynamic_string *ds, char c)
  {
  return(0);
  }

void log_err(int errnum, const char *routine, const char *text) {}
void log_record(int eventtype, int objclass, const char *objname, const char *text) {}
void log_event(int eventtype, int objclass, const char *objname, const char *text) {}
void log_ext(int eventtype, const char *func_name, const char *msg, int level) {}

pbs_net_t get_hostaddr(

  int  *local_errno, /* O */
  char *hostname)    /* I */
  {
  fprintf(stderr,"ERROR: %s is mocked.\n",__func__);
  return 0;
  }

