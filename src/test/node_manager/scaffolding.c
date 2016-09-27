#include "license_pbs.h" /* See here for the software license */
#include <vector>
#include <string>
#include <boost/ptr_container/ptr_vector.hpp>
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
#include "threadpool.h"
#include "id_map.hpp"
#include "machine.hpp"
#include "complete_req.hpp"
#include "json/json.h"


bool cray_enabled;
bool conn_success = true;
bool alloc_br_success = true;
char *path_node_usage = strdup("/tmp/idontexistatallnotevenalittle");
int str_to_attr_count;
int decode_resc_count;
int SvrNodeCt = 0; 
int svr_resc_size = 0;
char *path_nodestate;
char *path_nodepowerstate;
int allow_any_mom = FALSE;
unsigned int pbs_mom_port = 0;
attribute_def job_attr_def[10];
char server_name[PBS_MAXSERVERNAME + 1];
char *path_nodenote;
bool exit_called = false;
const char *dis_emsg[10];
tlist_head svr_newnodes; 
resource_def *svr_resc_def;
attribute_def node_attr_def[2];
char *path_nodenote_new;
struct server server;
int LOGLEVEL = 7; /* force logging code to be exercised as tests run */
struct pbsnode reporter;
struct pbsnode *alps_reporter = &reporter;
const char *alps_reporter_feature  = "alps_reporter";
const char *alps_starter_feature   = "alps_login";
threadpool_t    *task_pool;
bool             job_mode = false;
int              can_place = 0;
pbsnode          napali_node;


struct batch_request *alloc_br(int type)
  {
  struct batch_request *req = NULL;

  if (alloc_br_success == false)
    return(NULL);

  req = (struct batch_request *)calloc(1, sizeof(struct batch_request));

  if (req == NULL)
    {
    return(NULL);
    }

  req->rq_type = type;

  req->rq_conn = -1;  /* indicate not connected */
  req->rq_orgconn = -1;  /* indicate not connected */
  req->rq_time = 9877665;
  req->rq_reply.brp_choice = BATCH_REPLY_CHOICE_NULL;
  req->rq_noreply = FALSE;  /* indicate reply is needed */

  CLEAR_LINK(req->rq_link);

  return(req);
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
  }

int enqueue_threadpool_request(void *(*func)(void *), void *arg, threadpool_t *tp)
  {
  return(0);
  }

struct pbsnode *find_nodebyname(const char *nodename)
  {
  static struct pbsnode bob;
  static struct pbsnode other;
  static int    called = 0;

  if (called == 0)
    {
    other.change_name("lihue");
    called++;
    }

  if (!strcmp(nodename, "bob"))
    return(&bob);
  else if (!strcmp(nodename, "napali"))
    return(&napali_node);
  else if (!strcmp(nodename, "waimea"))
    return(&bob);
  else if (!strcmp(nodename, "2"))
    return(&bob);
  else if (!strcmp(nodename, "3"))
    return(&bob);
  else if (!strcmp(nodename, "lihue"))
    {
    return(&other);
    }
  else
    return(NULL);
  }


struct pbsnode *find_nodebyid(int id)
  {
  static struct pbsnode bob;

  if (id == 1)
    return(&bob);
  else
    return(NULL);
  }

struct pbsnode *find_node_in_allnodes(all_nodes *an, const char *nodename)
  {
  static struct pbsnode cray;

  if (!strcmp(nodename, "cray"))
    return(&cray);
  else
    return(NULL);
  }

struct work_task *set_task(enum work_type type, long event_id, void (*func)(work_task *), void *parm, int get_lock)
  {
  return(0);
  }

unsigned disrui(int stream, int *retval)
  {
  fprintf(stderr, "The call to disrui needs to be mocked!!\n");
  exit(1);
  }

void svr_disconnect(int handle)
  {
  return;
  }

struct pbsnode *next_host(all_nodes *an, all_nodes_iterator **iter, struct pbsnode *held)
  {
  fprintf(stderr, "The call to next_host needs to be mocked!!\n");
  exit(1);
  }

struct pbsnode *next_node(all_nodes *an, struct pbsnode *current, node_iterator *iter)
  {
  return(NULL);
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

int issue_Drequest(int conn, batch_request *br, bool close_handle)
  {
  return(0);
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

int svr_connect(unsigned long, unsigned int, int*, pbsnode*, void* (*)(void*))
  {
  if (conn_success == true)
    return(10);
  else
    return(-1);
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

job *svr_find_job(const char *jobid, int get_subjob)
  {
  static job pjob;

  time_t old = pjob.ji_last_reported_time;
  memset(&pjob, 0, sizeof(pjob));
  pjob.ji_mutex = (pthread_mutex_t *)calloc(1, sizeof(pthread_mutex_t));
  strcpy(pjob.ji_qs.ji_jobid, jobid);
  pjob.ji_last_reported_time = old;

  if (strstr(jobid, "lei.ac"))
    {
    return(NULL);
    }
  else if ((!strcmp(jobid, "1")) ||
           (!strcmp(jobid, "5")))
    {
    pjob.ji_wattr[JOB_ATR_exec_host].at_val.at_str = strdup("tom/0");
    }
  else if (!strcmp(jobid, "4"))
    {
    return(NULL);
    }
  else if (!strcmp(jobid, "2.napali"))
    {
    pjob.ji_qs.ji_state = JOB_STATE_RUNNING;
    }
  else if (strcmp(jobid, "2"))
    {
    pjob.ji_wattr[JOB_ATR_exec_host].at_val.at_str = strdup("bob/5");
    }
  else
    pjob.ji_wattr[JOB_ATR_exec_host].at_val.at_str = NULL;

  return(&pjob);
  }

job *svr_find_job_by_id(int id)
  {
  static job pjob;

  time_t old = pjob.ji_last_reported_time;
  memset(&pjob, 0, sizeof(pjob));
  pjob.ji_mutex = (pthread_mutex_t *)calloc(1, sizeof(pthread_mutex_t));
  sprintf(pjob.ji_qs.ji_jobid, "%d.napali", id);
  pjob.ji_last_reported_time = old;

  if ((id == 1) ||
      (id == 5))
    {
    pjob.ji_wattr[JOB_ATR_exec_host].at_val.at_str = strdup("tom/0");
    }
  else if (id == 4)
    {
    return(NULL);
    }
  else if (id == 2)
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

int tmp_unlock_node(struct pbsnode *the_node, const char *id, const char *msg, int logging)
  {
  return(0);
  }

int tmp_lock_node(struct pbsnode *the_node, const char *id, const char *msg, int logging)
  {
  return(0);
  }


void socket_read_flush(int socket) {}        

void close_conn(int sock,int has_mut) {}

void *send_hierarchy_threadtask(void *vp)
  { 
  fprintf(stderr, "The call to send_hierarchy_threadtask needs to be mocked!!\n");
  exit(1);                            
  }


int get_svr_attr_l(int index, long *l)
  {
  if (index == SRV_ATR_CrayEnabled)
    *l = 1;

  return(0);
  }

int get_svr_attr_b(int index, bool *b)
  {
  if (index == SRV_ATR_CrayEnabled)
    *b = true;

  return(0);
  }

int process_alps_status(

  char           *nd_name,
  boost::ptr_vector<std::string>& status_info)

  {
  return(0);
  }

struct pbsnode *get_next_login_node(

  std::vector<prop> *need)

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

int handle_complete_first_time(job *pjob)
  {
  return(0);
  }

int svr_setjobstate(job *pjob, int newstate, int newsubstate, int has_queue_mutex)
  {
  return(0);
  }

int unlock_ji_mutex(job *pjob, const char *id, const char *msg, int logging)
  {
  return(0);
  }

int add_execution_slot(struct pbsnode *pnode)

  {
  return(0);
  }

struct pbsnode *create_alps_subnode(

  struct pbsnode *parent,
  const char    *node_id)

  {
  return(NULL);
  }

void log_record(int eventtype, int objclass, const char *objname, const char *text) {}
void log_event(int eventtype, int objclass, const char *objname, const char *text) {}
void log_ext(int eventtype, const char *func_name, const char *msg, int level) {}
void log_err(int errnum, const char *routine, const char *text) {}


pbs_net_t get_hostaddr(

  int  *local_errno, /* O */
  const char *hostname)    /* I */
  {
  fprintf(stderr,"ERROR: %s is mocked.\n",__func__);
  return 0;
  }

int create_a_gpusubnode(struct pbsnode *np)
  {
  return(0);
  }

int node_gpustatus_list(pbs_attribute *attr, void *a, int b)
  {
  return(0);
  }

int str_to_attr(

  const char           *name,   /* I */
  char                 *val,    /* I */
  pbs_attribute        *attr,   /* O */
  struct attribute_def *padef,  /* I */
  int                   limit)  /* I */

  {
  str_to_attr_count++;

  return(ATTR_NOT_FOUND);
  }

int decode_resc(

  pbs_attribute *patr,  /* Modified on Return */
  const char    *name,  /* pbs_attribute name */
  const char    *rescn, /* I resource name - is used here */
  const char    *val,   /* resource value */
  int            perm)  /* access permissions */

  {
  decode_resc_count++;

  return(0);
  }


id_map::id_map() : counter(0) {}

int id_map::get_id(const char *name)
  {
  if (!strcmp(name, "bob"))
    return(1);
  else if (!strcmp(name, "2"))
    return(1);
  else if (!strcmp(name, "3"))
    return(1);
  else
    return(-1);
  }

const char *id_map::get_name(int id)
  {
  char buf[100];

  if (job_mode == true)
    {
    switch (id)
      {
      case 11:
      case 12:
      case 13:

        snprintf(buf, sizeof(buf), "%d.lei.ac", id);
        return(strdup(buf));
      
      case 4:

        snprintf(buf, sizeof(buf), "%d.napali", id);
        return(strdup(buf));

      default:

        return(NULL);

      }
    }
  else
    {
    snprintf(buf, sizeof(buf), "napali%d", id);
    return(strdup(buf));
    }
  }

id_map::~id_map() 
  {
  }

void translate_vector_to_range_string(

  std::string            &range_string,
  const std::vector<int> &indices)

  {
  } // END translate_vector_to_range_string()
  

int translate_range_string_to_vector(

  const char       *range_string,
  std::vector<int> &indices)

  {
  return(PBSE_NONE);
  } /* END translate_range_string_to_vector() */

void capture_until_close_character(

  char        **start,
  std::string  &storage,
  char          end)

  {
  if ((start == NULL) ||
      (*start == NULL))
    return;

  char *val = *start;
  char *ptr = strchr(val, end);

  // Make sure we found a close quote and this wasn't an empty string
  if ((ptr != NULL) &&
      (ptr != val))
    {
    storage = val;
    storage.erase(ptr - val);
    *start = ptr + 1; // add 1 to move past the character
    }
  } // capture_until_close_character()
  

id_map node_mapper;
id_map job_mapper;

job_usage_info::job_usage_info(int id) : internal_job_id(id)
  {
  }

#ifdef CAN_TIME
#include "timer.hpp"
microsecond_timer::microsecond_timer(const char *file, const char *func, int line) {}

microsecond_timer::~microsecond_timer() {}
#endif

ssize_t write_ac_socket(int fd, const void *buf, ssize_t count)
  {
  return(0);
  }


pbsnode::pbsnode() : nd_error(0), nd_properties(),
                     nd_mutex(), nd_id(-1), nd_addrs(), nd_prop(NULL), nd_status(),
                     nd_note(),
                     nd_stream(-1),
                     nd_flag(okay), nd_mom_port(PBS_MOM_SERVICE_PORT),
                     nd_mom_rm_port(PBS_MANAGER_SERVICE_PORT), nd_sock_addr(),
                     nd_nprops(0), nd_nstatus(0),
                     nd_slots(), nd_job_usages(), nd_needed(0), nd_np_to_be_used(0),
                     nd_state(INUSE_FREE), nd_ntype(0), nd_order(0),
                     nd_warnbad(0),
                     nd_lastupdate(0), nd_lastHierarchySent(0), nd_hierarchy_level(0),
                     nd_in_hierarchy(0), nd_ngpus(0), nd_gpus_real(0), nd_gpusn(),
                     nd_ngpus_free(0), nd_ngpus_needed(0), nd_ngpus_to_be_used(0),
                     nd_gpustatus(NULL), nd_ngpustatus(0), nd_nmics(0),
                     nd_micstatus(NULL), nd_nmics_alloced(0),
                     nd_nmics_free(0), nd_nmics_to_be_used(0), parent(NULL),
                     num_node_boards(0), node_boards(NULL), numa_str(),
                     gpu_str(), nd_mom_reported_down(0), nd_is_alps_reporter(0),
                     nd_is_alps_login(0), nd_ms_jobs(), alps_subnodes(NULL),
                     max_subnode_nppn(0), nd_power_state(0),
                     nd_power_state_change_time(0), nd_acl(NULL),
                     nd_requestid(), nd_tmp_unlock_count(0)

  {
  pthread_mutex_init(&this->nd_mutex,NULL);
  } // END empty constructor



pbsnode::pbsnode(

  const char *pname,
  u_long     *pul,
  bool        skip_address_lookup) : nd_error(0), nd_properties(), nd_mutex(), nd_prop(NULL),
                                     nd_status(),
                                     nd_note(),
                                     nd_stream(-1),
                                     nd_flag(okay),
                                     nd_mom_port(PBS_MOM_SERVICE_PORT),
                                     nd_mom_rm_port(PBS_MANAGER_SERVICE_PORT), nd_sock_addr(),
                                     nd_nprops(0), nd_nstatus(0),
                                     nd_slots(), nd_job_usages(), nd_needed(0), nd_np_to_be_used(0),
                                     nd_state(INUSE_DOWN), nd_ntype(0), nd_order(0),
                                     nd_warnbad(0),
                                     nd_lastupdate(0), nd_lastHierarchySent(0), nd_hierarchy_level(0),
                                     nd_in_hierarchy(0), nd_ngpus(0), nd_gpus_real(0), nd_gpusn(),
                                     nd_ngpus_free(0), nd_ngpus_needed(0), nd_ngpus_to_be_used(0),
                                     nd_gpustatus(NULL), nd_ngpustatus(0), nd_nmics(0),
                                     nd_micstatus(NULL), nd_nmics_alloced(0),
                                     nd_nmics_free(0), nd_nmics_to_be_used(0), parent(NULL),
                                     num_node_boards(0), node_boards(NULL), numa_str(),
                                     gpu_str(), nd_mom_reported_down(0), nd_is_alps_reporter(0),
                                     nd_is_alps_login(0), nd_ms_jobs(), alps_subnodes(NULL),
                                     max_subnode_nppn(0), nd_power_state(0),
                                     nd_power_state_change_time(0), nd_acl(NULL),
                                     nd_requestid(), nd_tmp_unlock_count(0)

  {
  this->nd_name            = pname;
  this->nd_properties.push_back(this->nd_name);
  this->nd_id              = 1;

  pthread_mutex_init(&this->nd_mutex,NULL);
  } // END constructor



pbsnode::~pbsnode()

  {
  } // END destructor



/*
 * lock_node()
 *
 * Locks this node
 */

int pbsnode::lock_node(
    
  const char     *id,
  const char     *msg,
  int             logging)

  {
  return(PBSE_NONE);
  } /* END lock_node() */



int pbsnode::tmp_lock_node(

  const char     *id,
  const char     *msg,
  int             logging)

  {
  return(PBSE_NONE);
  }



int pbsnode::unlock_node(
    
  const char     *id,
  const char     *msg,
  int             logging)

  {
  return(PBSE_NONE);
  } /* END unlock_node() */



int pbsnode::tmp_unlock_node(

  const char     *id,
  const char     *msg,
  int             logging)

  {
  return(PBSE_NONE);
  }



const char *pbsnode::get_name() const
  
  {
  return(this->nd_name.c_str());
  }



int pbsnode::get_error() const

  {
  return(this->nd_error);
  }



bool pbsnode::hasprop(

  std::vector<prop> *plist) const

  {
  if (plist == NULL)
    return(true);

  for (unsigned int i = 0; i < plist->size(); i++)
    {
    prop &need = plist->at(i);

    if (need.mark == 0) /* not marked, skip */
      continue;

    bool found = false;

    for (unsigned int i = 0; i < this->nd_properties.size(); i++)
      {
      if (this->nd_properties[i] == need.name)
        {
        found = true;
        break;
        }
      }

    if (found == false)
      return(found);
    }

  return(true);
  }  /* END hasprop() */



void pbsnode::update_properties()

  {
  this->nd_properties.clear();

  if (this->nd_prop)
    {
    int nprops = this->nd_prop->as_usedptr;

    for (int i = 0; i < nprops; i++)
      {
      this->nd_properties.push_back(this->nd_prop->as_string[i]);
      }
    }

  /* now add in name as last prop */
  this->nd_properties.push_back(this->nd_name);
  } // END update_prop_list()



void pbsnode::change_name(

  const char *name)

  {
  // Overwrite the old property
  for (unsigned int i = 0; i < this->nd_properties.size(); i++)
    {
    if (this->nd_name == this->nd_properties[i])
      this->nd_properties[i] = name;
    }

  this->nd_name = name;
  }



void pbsnode::add_property(

  const std::string &prop)

  {
  this->nd_properties.push_back(prop);
  }

bool pbsnode::update_internal_failure_counts(

  int rc)

  {
  bool held = false;
  char log_buf[2048];

  if (rc == PBSE_NONE)
    {
    this->nd_consecutive_successes++;

    if (this->nd_consecutive_successes > 1)
      {
      this->nd_proximal_failures = 0;

      if (this->nd_state & INUSE_NETWORK_FAIL)
        {
        snprintf(log_buf, sizeof(log_buf),
          "Node '%s' has had two or more consecutive network successes, marking online.",
          this->nd_name.c_str());
        log_record(1, 2, __func__, log_buf);
        this->remove_node_state_flag(INUSE_NETWORK_FAIL);
        }
      }
    }
  else
    {
    this->nd_proximal_failures++;
    this->nd_consecutive_successes = 0;

    if ((this->nd_proximal_failures > 2) &&
        ((this->nd_state & INUSE_NETWORK_FAIL) == 0))
      {
      snprintf(log_buf, sizeof(log_buf),
        "Node '%s' has had %d failures in close proximity, marking offline.",
        this->nd_name.c_str(), this->nd_proximal_failures);
      log_record(1, 2, __func__, log_buf);

      update_node_state(this, INUSE_NETWORK_FAIL);
      held = true;
      }
    }

  return(held);
  }

void pbsnode::remove_node_state_flag(

  int flag)

  {
  this->nd_state &= ~flag;
  }

void pbsnode::add_job_list_to_status(const std::string &job_list) {}

Machine::Machine() {}
Machine::~Machine() {}

void Machine::free_job_allocation(const char *jobid)
  {
  }

void Machine::displayAsJson(stringstream &out, bool include_jobs) const {}

int Machine::place_job(

  job        *pjob,
  string     &cpu_string,
  string     &mem_string,
  const char *hostname,
  bool        legacy_vmem)

  {
  return(0);
  }

int Machine::getTotalThreads() const
 {
 return(0);
 }


int Machine::getTotalChips() const
 {
 return(0);
 }

int Machine::how_many_tasks_can_be_placed(req &r) const
  {
  return(can_place);
  }

bool Machine::is_initialized() const
  {
  return(true);
  }

Socket::Socket() {}
Socket::~Socket() {}
Chip::Chip() {}
Chip::~Chip() {}
Core::Core() {}
Core::~Core() {}
PCI_Device::PCI_Device() {}
PCI_Device::~PCI_Device() {}

bool task_hosts_match(
        
  const char *task_host, 
  const char *this_hostname)

  {
  if (strcmp(task_host, this_hostname))
    {
    /* see if the short name might match */
    char task_hostname[PBS_MAXHOSTNAME];
    char local_hostname[PBS_MAXHOSTNAME];
    char *dot_ptr;

    strcpy(task_hostname, task_host);
    strcpy(local_hostname, this_hostname);

    dot_ptr = strchr(task_hostname, '.');
    if (dot_ptr != NULL)
      *dot_ptr = '\0';

    dot_ptr = strchr(local_hostname, '.');
    if (dot_ptr != NULL)
      *dot_ptr = '\0';

    if (strcmp(task_hostname, local_hostname))
      {
      /* this task does not belong to this host. Go to the next one */
      return(false);
      }
    }

  return(true);
  }

job::job() 
  {
  memset(this->ji_wattr, 0, sizeof(this->ji_wattr));
  }

job::~job() {}

void job::set_plugin_resource_usage_from_json(Json::Value &usage) {}

bool have_incompatible_dash_l_resource(job *pjob)
  {
  return(true);
  }


bool internal_job_id_exists(int internal_job_id)
  {
  if (internal_job_id < 10)
    return(true);
  else
    return(false);
  }

bool have_incompatible_dash_l_resource(pbs_attribute *pattr)
  {
  return(true);
  }

int pbsnode::get_version() const {return 0;}
