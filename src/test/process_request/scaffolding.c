#include "license_pbs.h" /* See here for the software license */
#include <stdlib.h>
#include <stdio.h> /* fprintf */
#include <arpa/inet.h>

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
#include "threadpool.h"
#include "acl_special.hpp"

bool exit_called = false;
const char *msg_err_noqueue = "Unable to requeue job, queue is not defined";
struct credential conn_credent[PBS_NET_MAX_CONNECTIONS];
char server_name[PBS_MAXSERVERNAME + 1];
const char *msg_err_malloc = "malloc failed";
struct connection svr_conn[PBS_NET_MAX_CONNECTIONS];
const char *msg_request = "Type %s request received from %s@%s, sock=%d";
struct server server;
char *server_host;
int LOGLEVEL = 10; /* force logging code to be exercised as tests run */
threadpool_t *request_pool;
bool check_acl;
bool find_node;
bool fail_get_connecthost = false;
int free_attrlist_called;
char scaff_buffer[1024];
int dis_req_read_rc = PBSE_NONE;

bool fail_check;

acl_special limited_acls;

acl_special::acl_special() : ug_acls()

  {
  }

bool threadpool_is_too_busy(threadpool *tp, int permissions)
  {
  return(false);
  }

int pthread_mutex_lock(pthread_mutex_t *mutex) throw()
  {
  return(0);
  }

int pthread_mutex_unlock(pthread_mutex_t *mutex) throw()
  {
  return(0);
  }

int req_releasejob(batch_request *preq)
  {
  fprintf(stderr, "The call to req_releasejob needs to be mocked!!\n");
  exit(1);
  }

int req_rerunjob(batch_request *preq)
  {
  fprintf(stderr, "The call to req_rerunjob needs to be mocked!!\n");
  exit(1);
  }

int req_stat_svr(batch_request *preq)
  {
  fprintf(stderr, "The call to req_stat_svr needs to be mocked!!\n");
  exit(1);
  }

void req_shutdown(struct batch_request *preq)
  {
  fprintf(stderr, "The call to req_shutdown needs to be mocked!!\n");
  exit(1);
  }

int req_rescq(batch_request *preq)
  {
  fprintf(stderr, "The call to req_rescq needs to be mocked!!\n");
  exit(1);
  }

int svr_job_purge(svr_job *pjob, int leaveSpoolFiles)
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
  if (sock == 999)
    return(-1);

  if (fail_get_connecthost == true)
    return(-1);

  snprintf(namebuf, size, "napali");
  return(0);
  }

int req_holdjob(batch_request *preq)
  {
  fprintf(stderr, "The call to req_holdjob needs to be mocked!!\n");
  exit(1);
  }

int req_registerarray(batch_request *preq)
  {
  fprintf(stderr, "The call to req_registerarray needs to be mocked!!\n");
  exit(1);
  }

void delete_link(struct list_link *old)
  {
  fprintf(stderr, "The call to delete_link needs to be mocked!!\n");
  exit(1);
  }

void req_jobscript(batch_request *preq, bool perform_commit)
  {
  return;
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

int req_jobobit(batch_request *preq)
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
  return(dis_req_read_rc);
  }

int req_stat_que(batch_request *preq)
  {
  fprintf(stderr, "The call to req_stat_que needs to be mocked!!\n");
  exit(1);
  }

void req_track(struct batch_request *preq)
  {
  fprintf(stderr, "The call to req_track needs to be mocked!!\n");
  exit(1);
  }

int req_holdarray(batch_request *preq)
  {
  fprintf(stderr, "The call to req_holdarray needs to be mocked!!\n");
  exit(1);
  }

int req_movejob(batch_request *preq)
  {
  fprintf(stderr, "The call to req_movejob needs to be mocked!!\n");
  exit(1);
  }

int unlock_node(struct pbsnode *the_node, const char *id, const char *msg, int logging)
  {
  return(0);
  }

void req_rdytocommit(struct batch_request *preq)
  {
  fprintf(stderr, "The call to req_rdytocommit needs to be mocked!!\n");
  exit(1);
  }

void req_reject(int code, int aux, struct batch_request *preq, const char *HostName, const char *Msg)
  {
  }

svr_job *next_job(all_jobs *aj, all_jobs_iterator *iter)
  {
  fprintf(stderr, "The call to next_job needs to be mocked!!\n");
  exit(1);
  }

int req_releasearray(batch_request *preq)
  {
  fprintf(stderr, "The call to req_releasearray needs to be mocked!!\n");
  exit(1);
  }

int req_connect(struct batch_request *preq)
  {
  return(0);
  }

int job_abt(struct svr_job **pjobp, const char *text, bool b=false)
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

int svr_enquejob(svr_job *pjob, int has_sv_qs_mutex, const char *prev_id, bool reservation, bool recov)
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

void reply_free(struct batch_reply *prep) {}

void free_attrlist(tlist_head *pattrlisthead) 
  {
  free_attrlist_called++;
  }

void req_signaljob(struct batch_request *preq)  {}

int req_register(batch_request *preq)
  {
  fprintf(stderr, "The call to req_register needs to be mocked!!\n");
  exit(1);
  }

void req_rescreserve(struct batch_request *preq)
  {
  fprintf(stderr, "The call to req_rescreserve needs to be mocked!!\n");
  exit(1);
  }

int req_quejob(batch_request *preq, int version)
  {
  return(PBSE_NONE);
  }

void req_deletearray(struct batch_request *preq)
  {
  fprintf(stderr, "The call to req_deletearray needs to be mocked!!\n");
  exit(1);
  }

int req_locatejob(batch_request *preq)
  {
  fprintf(stderr, "The call to req_locatejob needs to be mocked!!\n");
  exit(1);
  }

char * csv_find_string(const char *csv_str, const char *search_str)
  {
  fprintf(stderr, "The call to csv_find_string needs to be mocked!!\n");
  exit(1);
  }

char *pbse_to_txt(int err)
  {
  if (err == PBSE_BADHOST)
    return (char *)"Access from host not allowed, or unknown host";
  
  return(strdup("bob"));
  }

int req_stagein(batch_request *preq)
  {
  fprintf(stderr, "The call to req_stagein needs to be mocked!!\n");
  exit(1);
  }

int req_modifyarray(batch_request *preq)
  {
  fprintf(stderr, "The call to req_modifyarray needs to be mocked!!\n");
  exit(1);
  }

void append_link(tlist_head *head, list_link *new_link, void *pobj)
  {
  fprintf(stderr, "The call to append_link needs to be mocked!!\n");
  exit(1);
  }

void req_mvjobfile(struct batch_request *preq)
  {
  fprintf(stderr, "The call to req_mvjobfile needs to be mocked!!\n");
  exit(1);
  }

int remove_job(all_jobs *aj, svr_job *pjob, bool b)
  {
  fprintf(stderr, "The call to remove_job needs to be mocked!!\n");
  exit(1);
  }

int authenticate_user(struct batch_request *preq, struct credential *pcred, char **autherr)
  {
  fprintf(stderr, "The call to authenticate_user needs to be mocked!!\n");
  exit(1);
  }

struct pbsnode *find_nodebyname(const char *node_name)
  {
  static pbsnode nd;

  if (find_node == true)
    return(&nd);

  return(NULL);
  }

int req_manager(batch_request *preq)
  {
  fprintf(stderr, "The call to req_manager needs to be mocked!!\n");
  exit(1);
  }

int req_modify_node(struct batch_request *preq)
  {
  fprintf(stderr, "The call to req_modify_node needs to be mocked!!\n");
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

int req_runjob(batch_request *preq)
  {
  fprintf(stderr, "The call to req_runjob needs to be mocked!!\n");
  exit(1);
  }

int req_jobcredential(batch_request *preq)
  {
  fprintf(stderr, "The call to req_jobcredential needs to be mocked!!\n");
  exit(1);
  }

void close_conn(int sd, int has_mutex)
  {
  fprintf(stderr, "The call to close_conn needs to be mocked!!\n");
  exit(1);
  }

svr_job *svr_find_job(const char *jobid, int get_subjob)
  {
  fprintf(stderr, "The call to find_job needs to be mocked!!\n");
  exit(1);
  }

const char *reqtype_to_txt(int reqtype)
  {
  return("Queue Job");
  }

int req_orderjob(batch_request *preq)
  {
  fprintf(stderr, "The call to req_orderjob needs to be mocked!!\n");
  exit(1);
  }

void req_messagejob(struct batch_request *preq)
  {
  fprintf(stderr, "The call to req_messagejob needs to be mocked!!\n");
  exit(1);
  }

int req_gpuctrl_svr(batch_request *preq)
  {
  fprintf(stderr, "The call to req_gpuctrl needs to be mocked!!\n");
  exit(1);
  }

int req_stat_node(batch_request *preq)
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

int req_authenuser(batch_request *preq)
  {
  fprintf(stderr, "The call to req_authenuser needs to be mocked!!\n");
  exit(1);
  }

int req_selectjobs(batch_request *preq)
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
  if (check_acl == true)
    *l = 1;

  return(0);
  }

int get_svr_attr_b(int index, bool *b)
  {
  if (check_acl == true)
    *b = true;

  return(0);
  }

int acl_check_my_array_string(struct array_strings *pas, const char *name, int type)
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

int unlock_ji_mutex(svr_job *pjob, const char *id, const char *msg, int logging)
  {
  return(0);
  }

int pbs_getaddrinfo(const char *hostname, struct addrinfo *in, struct addrinfo **out)
  {
  return(0);
  }

bool log_available(int eventtype)
  {
  return true;
  }

void log_err(int errnum, const char *routine, const char *text) {}
void log_record(int eventtype, int objclass, const char *objname, const char *text) {}
void log_event(int eventtype, int objclass, const char *objname, const char *text) 
  {
  snprintf(scaff_buffer, sizeof(scaff_buffer), "%s", text);
  }

pbsnode::pbsnode() {}
pbsnode::~pbsnode() {}

int pbsnode::unlock_node(const char *id, const char *msg, int level)
  {
  return(0);
  }

bool acl_special::is_authorized(const std::string &host, const std::string &user) const
  {
  if(fail_check == false)
    return(false);
  return(true);
  }

int req_job_cleanup_done(batch_request *preq)

  {
  return(PBSE_NONE);
  }

int req_quejob2(batch_request *preq)
  {
  return(0);
  }

int req_commit2(batch_request *preq)
  {
  return(0);
  }

batch_request::batch_request() {}
batch_request::~batch_request() {}

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

tlist_head job::get_list_attr(int index)
  {
  return(this->ji_wattr[index].at_val.at_list);
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

pbs_attribute *job::get_attr(int index)
  {
  return(this->ji_wattr + index);
  }



