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
#include "queue.h"

all_nodes allnodes;
pthread_mutex_t *netrates_mutex = NULL;
all_queues svr_queues;
char server_name[PBS_MAXSERVERNAME + 1];
all_jobs alljobs;
attribute_def que_attr_def[10];
attribute_def node_attr_def[2];
struct server server;
all_jobs array_summary;
attribute_def svr_attr_def[10];
int svr_totnodes = 0;
int LOGLEVEL = 7; /* force logging code to be exercised as tests run */
bool exit_called = false;
int abort_called;
bool svr_find_job_returns_null = false;

struct batch_request *alloc_br(int type)
  {
  fprintf(stderr, "The call to alloc_br to be mocked!!\n");
  exit(1);
  }

pbs_queue *find_queuebyname(const char *quename)
  {
  fprintf(stderr, "The call to find_queuebyname to be mocked!!\n");
  exit(1);
  }

int svr_job_save(svr_job *pjob)
  {
  return(0);
  }

void svr_mailowner(svr_job *pjob, int mailpoint, int force, const char *text)
  {
  fprintf(stderr, "The call to svr_mailowner to be mocked!!\n");
  exit(1);
  }

svrattrl *attrlist_create(const char *aname, const char *rname, int vsize)
  {
  fprintf(stderr, "The call to attrlist_create to be mocked!!\n");
  exit(1);
  }

int modify_job_attr(svr_job *pjob, svrattrl *plist, int perm, int *bad)
  {
  fprintf(stderr, "The call to modify_job_attr to be mocked!!\n");
  exit(1);
  }

int status_nodeattrib(svrattrl *pal, attribute_def *padef, struct pbsnode *pnode, int limit, int priv, tlist_head *phead, int *bad)
  {
  fprintf(stderr, "The call to status_nodeattrib to be mocked!!\n");
  exit(1);
  }

int reply_send_svr(struct batch_request *request)
  {
  fprintf(stderr, "The call to reply_send_svr to be mocked!!\n");
  exit(1);
  }

void free_br(struct batch_request *preq)
  {
  fprintf(stderr, "The call to free_br to be mocked!!\n");
  exit(1);
  }

svr_job *find_array_template(char *arrayid)
  {
  fprintf(stderr, "The call to find_array_template to be mocked!!\n");
  exit(1);
  }

int lock_node(struct pbsnode *the_node, const char *id, const char *msg, int logging)
  {
  fprintf(stderr, "The call to lock_node to be mocked!!\n");
  exit(1);
  }

struct pbsnode *find_nodebyname(const char *nodename)
  {
  fprintf(stderr, "The call to find_nodebyname to be mocked!!\n");
  exit(1);
  }

struct work_task *set_task(enum work_type type, long event_id, void (*func)(struct work_task *), void *parm, int get_lock)
  {
  fprintf(stderr, "The call to set_task to be mocked!!\n");
  exit(1);
  }

int unlock_node(struct pbsnode *the_node, const char *id, const char *msg, int logging)
  {
  fprintf(stderr, "The call to unlock_node to be mocked!!\n");
  exit(1);
  }

job_array *get_array(const char *id)
  {
  return(NULL);
  }

void svr_disconnect(int handle)
  {
  }

struct pbsnode *next_host(all_nodes *an, all_nodes_iterator **iter, struct pbsnode *held)
  {
  fprintf(stderr, "The call to next_host to be mocked!!\n");
  exit(1);
  }

void req_reject(int code, int aux, struct batch_request *preq, const char *HostName, const char *Msg)
  {
  }

svr_job *next_job(all_jobs *aj, all_jobs_iterator *iter)
  {
  return(NULL);
  }

void rel_resc(svr_job *pjob)
  {
  }

void delete_task(struct work_task *ptask)
  {
  }

int status_job(

  svr_job           *pjob, /* ptr to job to status */
  batch_request *preq,
  svrattrl      *pal, /* specific attributes to status */
  tlist_head    *pstathd, /* RETURN: head of list to append status to */
  bool           condensed,
  int           *bad) /* RETURN: index of first bad pbs_attribute */

  {
  fprintf(stderr, "The call to status_job to be mocked!!\n");
  exit(1);
  }

int job_abt(struct svr_job **pjobp, const char *text, bool b=false)
  {
  abort_called++;
  return(0);
  }

void *get_next(list_link pl, char *file, int line)
  {
  fprintf(stderr, "The call to get_next to be mocked!!\n");
  exit(1);
  }

int issue_Drequest(int conn, struct batch_request *request, bool close_handle)
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

void netcounter_get(int netrates[])
  {
  fprintf(stderr, "The call to netcounter_get to be mocked!!\n");
  exit(1);
  }

void release_req(struct work_task *pwt)
  {
  fprintf(stderr, "The call to release_req to be mocked!!\n");
  exit(1);
  }

void append_link(tlist_head *head, list_link *new_link, void *pobj)
  {
  fprintf(stderr, "The call to append_link to be mocked!!\n");
  exit(1);
  }

pbs_queue *next_queue(all_queues *aq, all_queues_iterator *iter)
  {
  fprintf(stderr, "The call to next_queue to be mocked!!\n");
  exit(1);
  }

int svr_connect(pbs_net_t hostaddr, unsigned int port, int *err, struct pbsnode *pnode, void *(*func)(void *))
  {
  fprintf(stderr, "The call to svr_connect to be mocked!!\n");
  exit(1);
  }

int is_id_array(char *id)
  {
  fprintf(stderr, "The call to is_id_array to be mocked!!\n");
  exit(1);
  }

void update_array_statuses()
  {
  fprintf(stderr, "The call to update_array_statuses to be mocked!!\n");
  exit(1);
  }

struct pbsnode *tfind_addr(u_long key, uint16_t port, char *pjob_id)
  {
  fprintf(stderr, "The call to tfind_addr to be mocked!!\n");
  exit(1);
  }

int svr_setjobstate(svr_job *pjob, int newstate, int newsubstate, int  has_queue_mute)
  {
  fprintf(stderr, "The call to svr_setjobstate to be mocked!!\n");
  exit(1);
  }

svr_job *svr_find_job(const char *jobid, int get_subjob)
  {
  svr_job *pjob = (svr_job *)calloc(1, sizeof(svr_job));
  if (svr_find_job_returns_null)
    return(NULL);

  pjob->set_jobid(jobid);
  return(pjob);
  }

int unlock_queue(struct pbs_queue *the_queue, const char *method_name, const char *msg, int logging)
  {
  fprintf(stderr, "The call to unlock_queue to be mocked!!\n");
  exit(1);
  }

void reply_badattr(int code, int aux, svrattrl *pal, struct batch_request *preq)
  {
  fprintf(stderr, "The call to reply_badattr to be mocked!!\n");
  exit(1);
  }

int insert_task(all_tasks *at, work_task *wt)
  {
  fprintf(stderr, "The call to insert_task to be mocked!!\n");
  exit(1);
  }

int status_attrib(

  svrattrl      *pal,      /* I */
  attribute_def *padef,
  pbs_attribute *pattr,
  int            limit,
  int            priv,
  tlist_head    *phead,
  bool           condensed,
  int           *bad,
  int            IsOwner)  /* 0 == FALSE, 1 == TRUE */

  {
  fprintf(stderr, "The call to status_attrib to be mocked!!\n");
  exit(1);
  }

int get_svr_attr_l(int index, long *l)
  {
  return(0);
  }

int get_svr_attr_b(int index, bool *b)
  {
  return(0);
  }

pbs_queue *get_jobs_queue(svr_job **pjob)
  {
  return((*pjob)->ji_qhdr);
  }

int get_alps_statuses(

  struct pbsnode       *parent,
  struct batch_request *preq,
  int                  *bad,
  tlist_head           *pstathd)

  {
  return(0);
  }

int unlock_sv_qs_mutex(pthread_mutex_t *sv_qs_mutex, const char *msg_string)
  {
  return(0);
  }

int lock_sv_qs_mutex(pthread_mutex_t *sv_qs_mutex, const char *msg_string)
  {
  return(0);
  }


int unlock_ji_mutex(svr_job *pjob, const char *id, const char *msg, int logging)
  {
  return(0);
  }

int unlock_ai_mutex(job_array *pa, const char *id, const char *msg, int logging)
  {
  return(0);
  }

int lock_ai_mutex(job_array *pa, const char *id, const char *msg, int logging)
  {
  return(0);
  }

void log_err(int errnum, const char *routine, const char *text) {}
void log_record(int eventtype, int objclass, const char *objname, const char *text) {}
void log_event(int eventtype, int objclass, const char *objname, const char *text) {}

int svr_unresolvednodes = 0;

const char *pbsnode::get_name() const
  {
  return(this->nd_name.c_str());
  }

int pbsnode::lock_node(const char *msg, const char *id, int level)
  {
  return(0);
  }

int pbsnode::unlock_node(const char *msg, const char *id, int level)
  {
  return(0);
  }

bool pbsnode::hasprop(std::vector<prop> *needed) const
  {
  return(true);
  }

  void set_reply_type(struct batch_reply *preply, int type)
  {
  preply->brp_choice = type;
  }

batch_request::~batch_request()

  {
  }

batch_request::batch_request(const batch_request &other)

  {
  }

batch_request::batch_request()
  {
  }

batch_request::batch_request(int type) : rq_type(type)
  {
  }

struct timeval *job::get_tv_attr(int index)
  {
  return(&this->ji_wattr[index].at_val.at_timeval);
  }

int job::set_tv_attr(int index, struct timeval *tv)
  {
  memcpy(&(this->ji_wattr[index].at_val.at_timeval), tv, sizeof(struct timeval));
  this->ji_wattr[index].at_flags |= ATR_VFLAG_SET;
  return(PBSE_NONE);
  }

int job::set_resc_attr(int index, std::vector<resource> *resources)
  {
  this->ji_wattr[index].at_val.at_ptr = resources;
  this->ji_wattr[index].at_flags |= ATR_VFLAG_SET;
  return(PBSE_NONE);
  }

void job::set_exec_exitstat(int ev)
  {
  this->ji_qs.ji_un.ji_exect.ji_exitstat = ev;
  }

unsigned short job::get_ji_mom_rmport() const
  {
  return(this->ji_qs.ji_un.ji_exect.ji_mom_rmport);
  }

int job::set_creq_attr(int index, complete_req *cr)
  {
  this->ji_wattr[index].at_val.at_ptr = cr;
  this->ji_wattr[index].at_flags |= ATR_VFLAG_SET;
  return(PBSE_NONE);
  }

void job::set_qs_version(int version)
  {
  this->ji_qs.qs_version = version;
  }

void job::set_queue(const char *queue)
  {
  snprintf(this->ji_qs.ji_queue, sizeof(this->ji_qs.ji_queue), "%s", queue);
  }

int job::get_un_type() const
  {
  return(this->ji_qs.ji_un_type);
  }

void job::set_ji_momaddr(unsigned long momaddr)
  {
  this->ji_qs.ji_un.ji_exect.ji_momaddr = momaddr;
  }

void job::set_ji_mom_rmport(unsigned short mom_rmport)
  {
  this->ji_qs.ji_un.ji_exect.ji_mom_rmport = mom_rmport;
  }

void job::set_ji_momport(unsigned short momport)
  {
  this->ji_qs.ji_un.ji_exect.ji_momport = momport;
  }

const char *job::get_queue() const
  {
  return(this->ji_qs.ji_queue);
  }

void job::set_scriptsz(size_t scriptsz)
  {
  this->ji_qs.ji_un.ji_newt.ji_scriptsz = scriptsz;
  }

size_t job::get_scriptsz() const
  {
  return(this->ji_qs.ji_un.ji_newt.ji_scriptsz);
  }

pbs_net_t job::get_fromaddr() const
  {
  return(this->ji_qs.ji_un.ji_newt.ji_fromaddr);
  }

int job::get_fromsock() const
  {
  return(this->ji_qs.ji_un.ji_newt.ji_fromsock);
  }

void job::set_fromaddr(pbs_net_t fromaddr)
  {
  this->ji_qs.ji_un.ji_newt.ji_fromaddr = fromaddr;
  }

void job::set_fromsock(int sock)
  {
  this->ji_qs.ji_un.ji_newt.ji_fromsock = sock;
  }

int job::get_qs_version() const
  {
  return(this->ji_qs.qs_version);
  }

void job::set_un_type(int type)
  {
  this->ji_qs.ji_un_type = type;
  }

int job::get_exec_exitstat() const
  {
  return(this->ji_qs.ji_un.ji_exect.ji_exitstat);
  }

int job::get_svrflags() const
  {
  return(this->ji_qs.ji_svrflags);
  }

void job::set_modified(bool m)
  {
  this->ji_modified = m;
  }

void job::set_attr(int index)
  {
  this->ji_wattr[index].at_flags |= ATR_VFLAG_SET;
  }

void job::set_fileprefix(const char *prefix)
  {
  strcpy(this->ji_qs.ji_fileprefix, prefix);
  }

int job::set_char_attr(int index, char c)
  {
  this->ji_wattr[index].at_val.at_char = c;
  this->ji_wattr[index].at_flags |= ATR_VFLAG_SET;
  return(PBSE_NONE);
  }

void job::set_svrflags(int flags)
  {
  this->ji_qs.ji_svrflags = flags;
  }

const char *job::get_destination() const
  {
  return(this->ji_qs.ji_destin);
  }

void job::free_attr(int index)
  {
  }

void job::set_substate(int substate)
  {
  this->ji_qs.ji_substate = substate;
  }

void job::set_state(int state)
  {
  this->ji_qs.ji_state = state;
  }

void job::set_destination(const char *destination)
  {
  snprintf(this->ji_qs.ji_destin, sizeof(this->ji_qs.ji_destin), "%s", destination);
  }

pbs_net_t job::get_ji_momaddr() const
  {
  return(this->ji_qs.ji_un.ji_exect.ji_momaddr);
  }

bool job::has_been_modified() const
  {
  return(this->ji_modified);
  }

tlist_head *job::get_list_attr(int index)
  {
  return(&this->ji_wattr[index].at_val.at_list);
  }

complete_req *job::get_creq_attr(int index) const
  {
  complete_req *cr = NULL;
  if (this->ji_wattr[index].at_flags & ATR_VFLAG_SET)
    cr = (complete_req *)this->ji_wattr[index].at_val.at_ptr;

  return(cr);
  }

void job::set_exgid(unsigned int gid)
  {
  this->ji_qs.ji_un.ji_momt.ji_exgid = gid;
  }

void job::set_exuid(unsigned int uid)
  {
  this->ji_qs.ji_un.ji_momt.ji_exuid = uid;
  }

unsigned short job::get_ji_momport() const
  {
  return(this->ji_qs.ji_un.ji_exect.ji_momport);
  }

void job::set_jobid(const char *jobid)
  {
  strcpy(this->ji_qs.ji_jobid, jobid);
  }

int job::get_attr_flags(int index) const
  {
  return(this->ji_wattr[index].at_flags);
  }

struct jobfix &job::get_jobfix()
  {
  return(this->ji_qs);
  }

int job::set_bool_attr(int index, bool b)
  {
  this->ji_wattr[index].at_val.at_bool = b;
  this->ji_wattr[index].at_flags |= ATR_VFLAG_SET;
  return(PBSE_NONE);
  }

bool job::get_bool_attr(int index) const
  {
  return(this->ji_wattr[index].at_val.at_bool);
  }

std::vector<resource> *job::get_resc_attr(int index)
  {
  return((std::vector<resource> *)this->ji_wattr[index].at_val.at_ptr);
  }

const char *job::get_str_attr(int index) const
  {
  return(this->ji_wattr[index].at_val.at_str);
  }

const char *job::get_jobid() const
  {
  return(this->ji_qs.ji_jobid);
  }

int job::get_substate() const
  {
  return(this->ji_qs.ji_substate);
  }

int job::get_state() const
  {
  return(this->ji_qs.ji_state);
  }

void job::unset_attr(int index)
  {
  this->ji_wattr[index].at_flags = 0;
  }

bool job::is_attr_set(int index) const
  {
  return((this->ji_wattr[index].at_flags & ATR_VFLAG_SET) != 0);
  }

const char *job::get_fileprefix() const
  {
  return(this->ji_qs.ji_fileprefix);
  }

int job::set_long_attr(int index, long l)
  {
  this->ji_wattr[index].at_val.at_long = l;
  this->ji_wattr[index].at_flags |= ATR_VFLAG_SET;
  return(PBSE_NONE);
  }

int job::set_str_attr(int index, char *str)
  {
  this->ji_wattr[index].at_val.at_str = str;
  this->ji_wattr[index].at_flags |= ATR_VFLAG_SET;
  return(PBSE_NONE);
  }

long job::get_long_attr(int index) const
  {
  return(this->ji_wattr[index].at_val.at_long);
  }

time_t job::get_start_time() const
  {
  return(this->ji_qs.ji_stime);
  }

void job::set_start_time(time_t t)
  {
  this->ji_qs.ji_stime = t;
  }

pbs_attribute *job::get_attr(int index)
  {
  return(this->ji_wattr + index);
  }
