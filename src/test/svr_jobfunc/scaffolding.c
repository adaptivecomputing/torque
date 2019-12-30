#include "license_pbs.h" /* See here for the software license */
#include <pbs_config.h>
#include <stdlib.h>
#include <stdio.h> /* fprintf */
#include <pthread.h> /* pthread */
#include <sys/types.h>
#include <grp.h>

#include "queue.h" /* all_queues, pbs_queue */
#include "attribute.h" /* pbs_attribute */
#include "pbs_job.h" /* all_jobs, job */
#include "resource.h" /* resouce */
#include "server.h" /* server */
#include "work_task.h" /* work_task */
#include "sched_cmds.h" /* SCH_SCHEDULE_NULL */
#include "list_link.h" /* list_link */
#include "pbs_nodes.h"
#include "complete_req.hpp"
#include "attr_req_info.hpp"
#ifdef PENABLE_LINUX_CGROUPS
#include "machine.hpp"
#endif
#include "log.h"
#include "utils.h"

all_nodes               allnodes;
bool possible = false;
bool cray_enabled = false;


bool exit_called = false;
extern int svr_resc_size;
all_queues svr_queues;
const char *msg_daemonname = "unset";
attribute_def job_attr_def[JOB_ATR_LAST];
const char *msg_badwait = "Invalid time in work task for waiting, job = %s";
all_jobs alljobs;
const char *pbs_o_host = "PBS_O_HOST";
extern resource_def *svr_resc_def;
int svr_clnodes = 0;
int comp_resc_gt; 
struct server server;
all_jobs array_summary;
int svr_do_schedule = SCH_SCHEDULE_NULL;
int listener_command = SCH_SCHEDULE_NULL;
int LOGLEVEL = 10;
pthread_mutex_t *svr_do_schedule_mutex;
pthread_mutex_t *listener_command_mutex;
struct pbsnode *alps_reporter;
user_info_holder users;
int decrement_count;
svr_job napali_job;
std::string set_resource;
const char *incompatible_l[] = { "nodes", "size", "mppwidth", "mem", "hostlist",
                                 "ncpus", "procs", "pvmem", "pmem", "vmem", "reqattr",
                                 "software", "geometry", "opsys", "tpn", "trl", NULL };
bool get_jobs_queue_force_null = false;
std::string global_log_buf;


void remove_server_suffix(

  std::string &user_name)

  {
  size_t pos = user_name.find("@");

  if (pos != std::string::npos)
    user_name.erase(pos);
  }

resource *add_resource_entry(pbs_attribute *pattr, resource_def *prdef)
  {
  fprintf(stderr, "The call to add_resource_entry to be mocked!!\n");
  exit(1);
  }

pbs_queue *find_queuebyname(const char *quename)
  {
  pbs_queue *pq = (pbs_queue *)calloc(1, sizeof(pbs_queue));

  pq->qu_qs.qu_type = QTYPE_Unset;

  pq->qu_mutex = (pthread_mutex_t*)calloc(1, sizeof(pthread_mutex_t));
  pq->qu_jobs = new all_jobs();
  pq->qu_jobs_array_sum = new all_jobs();

  snprintf(pq->qu_qs.qu_name, sizeof(pq->qu_qs.qu_name), "%s", quename);

  /* set up the user info struct */
  pq->qu_uih = (user_info_holder *)calloc(1, sizeof(user_info_holder));

  return(pq);
  }

void account_record(int acctype, svr_job *pjob, const char *text)
  {
  fprintf(stderr, "The call to account_record to be mocked!!\n");
  exit(1);
  }

char *arst_string(const char *str, pbs_attribute *pattr)
  {
  return(NULL);
  }

int svr_job_save(svr_job *pjob)
  {
  return(0);
  }

long attr_ifelse_long(pbs_attribute *attr1, pbs_attribute *attr2, long deflong)
  {
  return(0);
  }

pbs_queue *get_jobs_queue(svr_job **pjob)
  {
  pbs_queue *pq = (pbs_queue *)calloc(1, sizeof(pbs_queue));

  if (get_jobs_queue_force_null)
    return(NULL);

  pq->qu_qs.qu_type = QTYPE_Unset;

  pq->qu_mutex = (pthread_mutex_t*)calloc(1, sizeof(pthread_mutex_t));
  pq->qu_jobs = new all_jobs();
  pq->qu_jobs_array_sum = new all_jobs();

  snprintf(pq->qu_qs.qu_name, sizeof(pq->qu_qs.qu_name), "%s", "qu_name");

  /* set up the user info struct */
  pq->qu_uih = (user_info_holder *)calloc(1, sizeof(user_info_holder));

  memset(pq->qu_njstate, 5, sizeof(pq->qu_njstate));

  return(pq);
  }

int procs_available(int proc_ct)
  {
  fprintf(stderr, "The call to procs_available to be mocked!!\n");
  exit(1);
  }

int decode_str(pbs_attribute *patr, const char *name, const char *rescn, const char *val, int perm)
  {
  fprintf(stderr, "The call to decode_str to be mocked!!\n");
  exit(1);
  }

int decode_resc(pbs_attribute *patr, const char *name, const char *rescn, const char *val, int perm)
  {
  fprintf(stderr, "The call to decode_str to be mocked!!\n");
  exit(1);
  }

int depend_on_que(pbs_attribute *pattr, void *pjob, int mode)
  {
  fprintf(stderr, "The call to depend_on_que to be mocked!!\n");
  exit(1);
  }

int insert_job_after(all_jobs *aj, svr_job *already_in, svr_job *pjob)
  {
  fprintf(stderr, "The call to insert_job_after to be mocked!!\n");
  exit(1);
  }

int has_job(all_jobs *aj, svr_job *pjob)
  {
  return(0);
  }

struct work_task *set_task(enum work_type type, long event_id, void (*func)(work_task *), void *parm, int get_lock)
  {
  fprintf(stderr, "The call to set_task to be mocked!!\n");
  exit(1);
  }

int insert_job(all_jobs *aj, svr_job *pjob)
  {
  return(0);
  }

int procs_requested(char *spec)
  {
  fprintf(stderr, "The call to procs_requested to be mocked!!\n");
  exit(1);
  }

int num_array_jobs(const char *req_str)
  {
  return(0);
  }

svr_job *next_job(all_jobs *aj, all_jobs_iterator *iter)
  {
  fprintf(stderr, "The call to next_job to be mocked!!\n");
  exit(1);
  }

int comp_resc2(struct pbs_attribute *attr, struct pbs_attribute *with, int IsQueueCentric, char *EMsg, enum compare_types type)
  {
  return(0);
  }

resource_def *find_resc_def(resource_def *rscdf, const char *name, int limit)
  {
  if (!strcmp(name, "mppnppn") ||
      !strcmp(name, "mppwidth"))
    {
    return((resource_def *)name);
    }

  return(NULL);
  }

char * csv_find_string(const char *csv_str, const char *search_str)
  {
  fprintf(stderr, "The call to csv_find_string to be mocked!!\n");
  exit(1);
  }

int node_avail_complex(char *spec, int *navail, int *nalloc, int *nresvd, int *ndown)
  {
  fprintf(stderr, "The call to node_avail_complex to be mocked!!\n");
  exit(1);
  }

work_task *next_task(all_tasks *at, int *iter)
  {
  fprintf(stderr, "The call to next_task to be mocked!!\n");
  exit(1);
  }

pbs_queue *next_queue(all_queues *aq, all_queues_iterator *iter)
  {
  return(NULL);
  }

int remove_job(all_jobs *aj, svr_job *pjob, bool b)
  {
  return(PBSE_JOBNOTFOUND);
  }

int set_jobexid(svr_job *pjob, pbs_attribute *attrry, std::string& EMsg)
  {
  return(0);
  }

int site_acl_check(job *pjob, pbs_queue *pque)
  {
  return(0);
  }

resource *find_resc_entry(pbs_attribute *pattr, resource_def *rscdf)
  {
  for (int i = 0; incompatible_l[i] != NULL; i++)
    if (set_resource == incompatible_l[i])
      return((resource *)1);

  return(NULL);
  }

svr_job *svr_find_job(const char *jobid, int get_subjob)
  {
  const char *job_id = "job_id";

  if (jobid != NULL)
    {
    if (strcmp(job_id,jobid) == 0)
      {
      static struct svr_job job_id_job;
      memset(&job_id_job, 0, sizeof(job_id_job));
      return(&job_id_job);
      }
    else if (!strcmp(jobid, "1.napali"))
      return(&napali_job);
    }

  return(NULL);
  }

int insert_job_first(all_jobs *aj, svr_job *pjob)
  {
  fprintf(stderr, "The call to insert_job_first to be mocked!!\n");
  exit(1);
  }

int unlock_queue(struct pbs_queue *the_queue, const char *id, const char *msg, int logging)
  {
  return(0);
  }

int acl_check(pbs_attribute *pattr, const char *name, int type)
  {
  return(0);
  }

int insert_task(all_tasks *at, work_task *wt)
  {
  fprintf(stderr, "The call to insert_task to be mocked!!\n");
  exit(1);
  }

void free_str(struct pbs_attribute *attr)
  {
  fprintf(stderr, "The call to free_str to be mocked!!\n");
  exit(1);
  }

void *get_next(list_link pl, char *file, int line)
  {
  return(NULL);
  }

int unlock_startup() 
  {
  return(0);
  }

int lock_startup()
  {
  return(0);
  }

int insert_job_after_index(all_jobs *aj, int index, svr_job *pjob)
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

unsigned int get_num_queued(user_info_holder *uih, const char *user_name)
  {
  return(0);
  }

int increment_queued_jobs(user_info_holder *uih, const char *user_name, svr_job *pjob)
  {
  return(0);
  }

int decrement_queued_jobs(user_info_holder *uih, const char *user_name, svr_job *pjob)
  {
  decrement_count++;
  return(0);
  }

int get_jobs_index(all_jobs *aj, struct svr_job *pjob)
  {
  return(0);
  }

int get_svr_attr_l(int attr_index, long *l)
  {
  return(0);
  }

int get_svr_attr_b(int index, bool *b)
  {
  return(0);
  }

int lock_node(struct pbsnode *the_node, const char *id, const char *msg, int logging)
  {
  return(0);
  }

int unlock_node(struct pbsnode *the_node, const char *id, const char *msg, int logging)
  {
  return(0);
  }

void log_err(int errnum, const char *routine, const char *text)
  {
  global_log_buf = text;
  }

void log_record(int eventtype, int objclass, const char *objname, const char *text) {}
void log_event(int eventtype, int objclass, const char *objname, const char *text) {}

int remove_procct(svr_job *pjob)
  {
  return(0);
  }

int initialize_procct(svr_job *pjob)
  {
  return(0);
  }

void free_nodes(svr_job *pjob, const char *spec) {}

int comp_size(struct pbs_attribute *attr, struct pbs_attribute *with)
  {
  fprintf(stderr, "The call to comp_size to be mocked!!\n");
  exit(1);
  }

int decode_time(pbs_attribute *patr, const char *name, const char *rescn, const char *val, int perm)
  {
  fprintf(stderr, "The call to decode_time to be mocked!!\n");
  exit(1);
  }

int decode_size(pbs_attribute *patr, const char *name, const char *rescn, const char *val, int perm)
  {
  fprintf(stderr, "The call to decode_size to be mocked!!\n");
  exit(1);
  }

int set_size(struct pbs_attribute *attr, struct pbs_attribute *new_attr, enum batch_op op)
  {
  fprintf(stderr, "The call to set_size to be mocked!!\n");
  exit(1);
  }

int set_ll(struct pbs_attribute *attr, struct pbs_attribute *new_attr, enum batch_op op)
  {
  fprintf(stderr, "The call to set_ll to be mocked!!\n");
  exit(1);
  }

int encode_time(pbs_attribute *attr, tlist_head *phead, const char *atname, const char *rsname, int mode, int perm)
  {
  fprintf(stderr, "The call to encode_time to be mocked!!\n");
  exit(1);
  }

int encode_ll(pbs_attribute *attr, tlist_head *phead, const char *atname, const char *rsname, int mode, int perm)
  {
  fprintf(stderr, "The call to encode_ll to be mocked!!\n");
  exit(1);
  }

int decode_ll(pbs_attribute *patr, const char *name, const char *rescn, const char *val, int perm)
  {
  fprintf(stderr, "The call to decode_ll to be mocked!!\n");
  exit(1);
  }

int comp_ll(struct pbs_attribute *attr, struct pbs_attribute *with)
  {
  fprintf(stderr, "The call to comp_ll to be mocked!!\n");
  exit(1);
  }

int decode_tokens(pbs_attribute *patr, const char *name, const char *rescn, const char *val, int perm)
  {
  fprintf(stderr, "The call to decode_tokens to be mocked!!\n");
  exit(1);
  }

int get_svr_attr_arst(int index, struct array_strings **arst)
  {
  return(-1);
  }

int encode_size(pbs_attribute *attr, tlist_head *phead, const char *atname, const char *rsname, int mode, int perm)
  {
  fprintf(stderr, "The call to encode_size to be mocked!!\n");
  exit(1);
  }

void log_ext(int i, char const* s, char const* s2, int i2)
  {
  }

int csv_length(const char *csv_str)
  {
  fprintf(stderr, "The call to decode_tokens to be mocked!!\n");
  exit(1);
  }

char *csv_nth(const char *csv_str, int n)
  {
  fprintf(stderr, "The call to decode_tokens to be mocked!!\n");
  exit(1);
  }

int is_whitespace(

  char c)

  {
  if ((c == ' ')  ||
      (c == '\n') ||
      (c == '\t') ||
      (c == '\r') ||
      (c == '\f'))
    return(TRUE);
  else
    return(FALSE);
  } /* END is_whitespace */


void move_past_whitespace(

  char **str)

  {
  if ((str == NULL) ||
      (*str == NULL))
    return;

  char *current = *str;

  while (is_whitespace(*current) == TRUE)
    current++;

  *str = current;
  } // END move_past_whitespace()


int translate_range_string_to_vector(

  const char       *range_string,
  std::vector<int> &indices)

  {
  char *str = strdup(range_string);
  char *ptr = str;
  int   prev;
  int   curr;

  while (*ptr != '\0')
    {
    prev = strtol(ptr, &ptr, 10);
    
    if (*ptr == '-')
      {
      ptr++;
      curr = strtol(ptr, &ptr, 10);

      while (prev <= curr)
        {
        indices.push_back(prev);

        prev++;
        }

      if ((*ptr == ',') ||
          (is_whitespace(*ptr)))
        ptr++;
      }
    else
      {
      indices.push_back(prev);

      if ((*ptr == ',') ||
          (is_whitespace(*ptr)))
        ptr++;
      }
    }

  free(str);
  return(PBSE_NONE);
  } /* END translate_range_string_to_vector() */

void create_size_string(

  char *buf, 
  struct size_value values)

  {
  }

int to_size(

  const char        *val,   /* I */
  struct size_value *psize) /* O */

  {
  return(0);
  }


void reinitialize_node_iterator(

  node_iterator *iter)

  {
  if (iter != NULL)
    {
    iter->node_index = NULL;
    iter->numa_index = -1;
    iter->alps_index = NULL;
    }
  } /* END reinitialize_node_iterator() */

#ifdef PENABLE_LINUX_CGROUPS
PCI_Device::PCI_Device() {}
PCI_Device::~PCI_Device() {}
Socket::Socket() {}
Socket::~Socket() {}
Chip::Chip() {}
Chip::~Chip() {}
Core::Core() {}
Core::~Core() {}

Machine::Machine() {}

bool Machine::check_if_possible(int &sockets, int &numa_nodes, int &cores, int &threads) const
  {
  return(possible);
  }

pbsnode::pbsnode() : nd_layout()
#else
pbsnode::pbsnode()
#endif

  {
  }

struct pbsnode *next_node(
    
  all_nodes     *an,
  pbsnode       *pnode,
  node_iterator *ni)

  {
  static int next_node_count = 0;
  static pbsnode *pn = new pbsnode();

  if (next_node_count++ % 2 == 0)
    return(pn);
  else
    return(NULL);
  }


void free_grname(

  struct group *grp,
  char         *user_buf)

  {
  if (user_buf)
    {
    free(user_buf);
    user_buf = NULL;
    }

  if (grp)
    {
    free(grp);
    grp = NULL;
    }

  }


struct group *getgrnam_ext(

  char **user_buf,
  const char *grp_name) /* I */

  {
  struct group *grp;
  char  *buf;
  long   bufsize;
  struct group *result;
  int rc;

  *user_buf = NULL;
  if (grp_name == NULL)
    return(NULL);

  bufsize = sysconf(_SC_GETGR_R_SIZE_MAX);
  if (bufsize == -1)
    bufsize = 8196;

  buf = (char *)malloc(bufsize);
  if (buf == NULL)
    {
    log_event(PBSEVENT_JOB, PBS_EVENTCLASS_JOB, __func__, "failed to allocate memory");
    return(NULL);
    }

  int alloc_size = sizeof(struct group);
  grp = (struct group *)calloc(1, alloc_size);
  if (grp == NULL)
    {
    log_event(PBSEVENT_JOB, PBS_EVENTCLASS_JOB, __func__, "could not allocate passwd structure");
    free(buf);
    return(NULL);
    }

  rc = getgrnam_r(grp_name, grp, buf, bufsize, &result);
  if ((rc) ||
      (result == NULL))
    {
    /* See if a number was passed in instead of a name */
    if (isdigit(grp_name[0]))
      {
      rc = getgrgid_r(atoi(grp_name), grp, buf, bufsize, &result);
      if ((rc == 0) &&
          (result != NULL))
        {
        *user_buf = buf;
        return(grp);
        }
      }

    sprintf(buf, "getgrnam_r failed: %d", rc);
    log_event(PBSEVENT_JOB, PBS_EVENTCLASS_JOB, __func__, buf);

    free(buf);
    free(grp);

    return (NULL);
    }

  *user_buf = buf;
  return(grp);
  } /* END getgrnam_ext() */



int pbsnode::lock_node(const char *id, const char *msg, int level)
  {

  return(0);
  }


int pbsnode::unlock_node(const char *id, const char *msg, int level)
  {

  return(0);
  }

job::job()
  {
  memset(this->ji_wattr, 0, sizeof(this->ji_wattr));
  memset(&this->ji_qs, 0, sizeof(this->ji_qs));
  }

job::~job() {}
svr_job::svr_job() : ji_is_array_template(false)
  {
  this->ji_mutex = (pthread_mutex_t *)calloc(1, sizeof(pthread_mutex_t));
  pthread_mutex_init(this->ji_mutex, NULL);
  this->ji_parent_job = NULL;
  }
svr_job::~svr_job() {}

#include "../../lib/Libattr/req.cpp"
#include "../../lib/Libattr/complete_req.cpp"
#include "../../lib/Libattr/attr_req_info.cpp"

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
  this->ji_wattr[index].at_flags = 0;
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
  if (this->ji_wattr[index].at_flags == 0)
    return(NULL);
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

void job::set_route_queue_time(time_t t)
  {
  this->ji_qs.ji_un.ji_routet.ji_quetime = t;
  }

  long time_str_to_seconds(const char *str)
  {
  return(0);
  }
