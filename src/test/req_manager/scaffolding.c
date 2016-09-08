#include "license_pbs.h" /* See here for the software license */
#include <stdlib.h>
#include <stdio.h> /* fprintf */
#include <string>
#include <vector>
#include <boost/ptr_container/ptr_vector.hpp>

#include "pbs_nodes.h" /* all_nodes, pbsnode, node_check_info, prop */
#include "queue.h" /* all_queues, pbs_queue */
#include "pbs_ifl.h" /* PBS_MAXSERVERNAME */
#include "resource.h" /* resource_def */
#include "attribute.h" /* attribute_def, pbs_attribute, svrattrl */
#include "server.h" /* server */
#include "batch_request.h" /* batch_request */
#include "list_link.h" /* list_link */
#include "work_task.h" /* work_type */
#include "mom_hierarchy_handler.h"
#include "acl_special.hpp"


all_nodes allnodes;
int svr_resc_size = 0;
int disable_timeout_check = 1;
all_queues svr_queues;
const char *msg_daemonname = "unset";
const char *msg_man_del = "deleted";
const char *msg_attrtype = "Warning: type of queue %s incompatible with attribute %s";
const char *msg_man_cre = "created";
const char *msg_man_set = "attributes set: ";
const char *msg_manager = "%s at request of %s@%s";
const char *msg_man_uns = "attributes unset: ";
char server_name[PBS_MAXSERVERNAME + 1];
resource_def *svr_resc_def;
attribute_def que_attr_def[10];
attribute_def node_attr_def[2];
struct server server;
const char *array_disallowed_types[] = {"hi", "hello"};
attribute_def svr_attr_def[10];
int LOGLEVEL = 7; /* force logging code to be exercised as tests run */
int svr_chngNodesfile = 0;
int svr_totnodes = 0;
bool exit_called = false;
time_t pbs_incoming_tcp_timeout;

mom_hierarchy_t *mh;
boost::ptr_vector<std::string> hierarchy_holder;
pthread_mutex_t                 hierarchy_holder_Mutex = PTHREAD_MUTEX_INITIALIZER;

int write_node_note(void)
  {
  fprintf(stderr, "The call to write_node_note to be mocked!!\n");
  exit(1);
  }

pbs_queue *find_queuebyname(const char *quename)
  {
  fprintf(stderr, "The call to find_queuebyname to be mocked!!\n");
  exit(1);
  }

void clear_attr(pbs_attribute *pattr, attribute_def *pdef)
  {
  fprintf(stderr, "The call to clear_attr to be mocked!!\n");
  exit(1);
  }

int attr_atomic_node_set(struct svrattrl *plist, pbs_attribute *old, pbs_attribute *new_attr, attribute_def *pdef, int limit, int unkn, int privil, int *badattr, bool update_nodes_file)
  {
  return(0);
  }

void reply_ack(struct batch_request *preq)
  {
  fprintf(stderr, "The call to reply_ack to be mocked!!\n");
  exit(1);
  }

void delete_link(struct list_link *old)
  {
  fprintf(stderr, "The call to delete_link to be mocked!!\n");
  exit(1);
  }

void setup_notification(char *pname)
  {
  fprintf(stderr, "The call to setup_notification to be mocked!!\n");
  exit(1);
  }

int chk_characteristic(struct pbsnode *pnode, node_check_info *nci, int *pneed_todo)
  {
  fprintf(stderr, "The call to chk_characteristic to be mocked!!\n");
  exit(1);
  }

struct pbsnode *find_nodebyname(const char *nodename)
  {
  fprintf(stderr, "The call to find_nodebyname to be mocked!!\n");
  exit(1);
  }

pbs_queue *que_alloc(const char *name, int sv_qs_mutex_held)
  {
  fprintf(stderr, "The call to que_alloc to be mocked!!\n");
  exit(1);
  }

struct work_task *set_task(enum work_type type, long event_id, void (*func)(struct work_task *), void *parm, int get_lock)
  {
  fprintf(stderr, "The call to set_task to be mocked!!\n");
  exit(1);
  }

void effective_node_delete(struct pbsnode **pnode)
  {
  fprintf(stderr, "The call to effective_node_delete to be mocked!!\n");
  exit(1);
  }

int unlock_node(struct pbsnode *the_node, const char *id, const char *msg, int logging)
  {
  fprintf(stderr, "The call to unlock_node to be mocked!!\n");
  exit(1);
  }

int find_attr(struct attribute_def *attr_def, const char *name, int limit)
  {
  fprintf(stderr, "The call to find_attr to be mocked!!\n");
  exit(1);
  }

void free_prop_list(struct prop *prop)
  {
  fprintf(stderr, "The call to free_prop_list to be mocked!!\n");
  exit(1);
  }

void write_node_state(void)
  {
  fprintf(stderr, "The call to write_node_state to be mocked!!\n");
  exit(1);
  }

struct pbsnode *next_host(
  all_nodes           *an,    /* I */
  all_nodes_iterator **iter,  /* M */
  struct pbsnode      *held)  /* I */
  {
  fprintf(stderr, "The call to next_host to be mocked!!\n");
  exit(1);
  }

void req_reject(int code, int aux, struct batch_request *preq, const char *HostName, const char *Msg)
  {
  fprintf(stderr, "The call to req_reject to be mocked!!\n");
  exit(1);
  }

struct pbsnode *next_node(all_nodes *an, struct pbsnode *current, node_iterator *iter)
  {
  fprintf(stderr, "The call to next_node to be mocked!!\n");
  exit(1);
  }

struct prop *init_prop(char *pname)
  {
  fprintf(stderr, "The call to init_prop to be mocked!!\n");
  exit(1);
  }

int attr_atomic_set(struct svrattrl *plist, pbs_attribute *old, pbs_attribute *new_attr, attribute_def *pdef, int limit, int unkn, int privil, int *badattr)
  {
  fprintf(stderr, "The call to attr_atomic_set to be mocked!!\n");
  exit(1);
  }

void save_characteristic(struct pbsnode *pnode, node_check_info *nci)
  {
  fprintf(stderr, "The call to save_characteristic to be mocked!!\n");
  exit(1);
  }

void *get_next(list_link pl, char *file, int line)
  {
  fprintf(stderr, "The call to get_next to be mocked!!\n");
  exit(1);
  }

void que_free(pbs_queue *pq, int sv_qs_mutex_held)
  {
  fprintf(stderr, "The call to que_free to be mocked!!\n");
  exit(1);
  }

resource_def *find_resc_def(resource_def *rscdf, const char *name, int limit)
  {
  fprintf(stderr, "The call to find_resc_def to be mocked!!\n");
  exit(1);
  }

void recompute_ntype_cnts(void)
  {
  fprintf(stderr, "The call to recompute_ntype_cnts to be mocked!!\n");
  exit(1);
  }

void attr_atomic_kill(pbs_attribute *temp, attribute_def *pdef, int limit)
  {
  fprintf(stderr, "The call to attr_atomic_kill to be mocked!!\n");
  exit(1);
  }

char *pbse_to_txt(int err)
  {
  fprintf(stderr, "The call to pbse_to_txt to be mocked!!\n");
  exit(1);
  }

pbs_queue *next_queue(all_queues *aq, all_queues_iterator *iter)
  {
  fprintf(stderr, "The call to next_queue to be mocked!!\n");
  exit(1);
  }

int init_resc_defs(void)
  {
  fprintf(stderr, "The call to init_resc_defs to be mocked!!\n");
  exit(1);
  }

void free_arst(struct pbs_attribute *attr)
  {
  fprintf(stderr, "The call to free_arst to be mocked!!\n");
  exit(1);
  }

int get_fullhostname(char *shortname, char *namebuf, int bufsize, char *EMsg)
  {
  fprintf(stderr, "The call to get_fullhostname to be mocked!!\n");
  exit(1);
  }

int que_purge(pbs_queue *pque)
  {
  fprintf(stderr, "The call to que_purge to be mocked!!\n");
  exit(1);
  }

int PNodeStateToString(int SBM, char *Buf, int BufSize)
  {
  fprintf(stderr, "The call to PNodeStateToString to be mocked!!\n");
  exit(1);
  }

resource *find_resc_entry(pbs_attribute *pattr, resource_def *rscdf)
  {
  fprintf(stderr, "The call to find_resc_entry to be mocked!!\n");
  exit(1);
  }

int svr_save(struct server *ps, int mode)
  {
  fprintf(stderr, "The call to svr_save to be mocked!!\n");
  exit(1);
  }

int update_nodes_file(struct pbsnode *held)
  {
  fprintf(stderr, "The call to update_nodes_file to be mocked!!\n");
  exit(1);
  }

void list_move(tlist_head *from, tlist_head *to)
  {
  fprintf(stderr, "The call to list_move to be mocked!!\n");
  exit(1);
  }

int unlock_queue(struct pbs_queue *the_queue, const char *method_name, const char *msg, int logging)
  {
  fprintf(stderr, "The call to unlock_queue to be mocked!!\n");
  exit(1);
  }

int que_save(pbs_queue *pque)
  {
  fprintf(stderr, "The call to que_save to be mocked!!\n");
  exit(1);
  }

void reinitialize_node_iterator(node_iterator *iter)
  {
  fprintf(stderr, "The call to reinitialize_node_iterator to be mocked!!\n");
  exit(1);
  }

int create_pbs_node(char *objname, svrattrl *plist, int perms, int *bad)
  {
  fprintf(stderr, "The call to create_pbs_node to be mocked!!\n");
  exit(1);
  }

void reply_badattr(int code, int aux, svrattrl *pal, struct batch_request *preq)
  {
  fprintf(stderr, "The call to reply_badattr to be mocked!!\n");
  exit(1);
  }

void reply_text(struct batch_request *preq, int code, const char *text)
  {
  }

int safe_strncat(char *str, const char *to_append, size_t space_remaining)
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

void log_err(int errnum, const char *routine, const char *text) {}
void log_record(int eventtype, int objclass, const char *objname, const char *text) {}
void log_event(int eventtype, int objclass, const char *objname, const char *text) {}

void write_node_power_state(void) {}

int req_runjob(batch_request *preq)
  {
  fprintf(stderr, "The call to req_runjob needs to be mocked!!\n");
  exit(1);
  }

struct batch_request *alloc_br(int type)
  {
  fprintf(stderr, "The call to alloc_br needs to be mocked!!\n");
  exit(1);
  }

void req_deletejob(struct batch_request *preq)
  {
  fprintf(stderr, "The call to req_deletejob needs to be mocked!!\n");
  exit(1);
  }
void add_all_nodes_to_hello_container()
  {
  fprintf(stderr, "The call to %s needs to be mocked!!\n",__func__);
  exit(1);
  }

job *svr_find_job_by_id(int id)
  {
  fprintf(stderr, "The call to %s needs to be mocked!!\n",__func__);
  exit(1);
  }

int unlock_ji_mutex(job *pjob, const char *id, const char *msg, int logging)
  {
  return(0);
  }

int lock_node(struct pbsnode *the_node, const char *id, const char *msg, int logging)
  {
  return(0);
  }

void free_br(struct batch_request *preq)
  {
  }

void prepare_mom_hierarchy(

  std::vector<std::string> &send_format)
  {
  fprintf(stderr, "The call to %s needs to be mocked!!\n",__func__);
  exit(1);
  }

int req_rerunjob(batch_request *preq)
  {
  fprintf(stderr, "The call to %s needs to be mocked!!\n",__func__);
  exit(1);
  }

int tmp_lock_node(struct pbsnode *the_node, const char *id, const char *msg, int logging)
  {
  return(0);
  }

void ensure_deleted(struct work_task *ptask)
  {
  }

int tmp_unlock_node(struct pbsnode *the_node, const char *id, const char *msg, int logging)
  {
  return(0);
  }

int create_pbs_dynamic_node(
  char     *objname,
  svrattrl *plist,
  int       perms,
  int      *bad)

  {
  return(0);
  }

mom_hierarchy_handler hierarchy_handler; //The global declaration.

void mom_hierarchy_handler::reloadHierarchy()
  {
  }

int get_svr_attr_l(

  int   attr_index,
  long *l)

  {
  return(0);
  }

int get_svr_attr_b(int index, bool *b)
  {
  return(0);
  }

void pbsnode::update_properties() {}

const char *pbsnode::get_name() const 
  {
  return(this->nd_name.c_str());
  }

bool pbsnode::hasprop(std::vector<prop> *needed) const
  {
  return(true);
  }

int pbsnode::tmp_lock_node(const char *caller, const char *msg, int level)
  {
  return(0);
  }

int pbsnode::tmp_unlock_node(const char *caller, const char *msg, int level)
  {
  return(0);
  }

int pbsnode::unlock_node(const char *caller, const char *msg, int level)
  {
  return(0);
  }

struct prop *init_prop(

  const char *pname) /* I */

  {
  return(NULL);
  }

pbsnode &pbsnode::operator =(

  const pbsnode &other)

  {
  return(*this);
  }

pbsnode::pbsnode()
  {
  }

pbsnode::~pbsnode()
  {
  }

void acl_special::add_user_configuration(const std::string &qmgr_input)
  {
  }

void acl_special::add_group_configuration(const std::string &qmgr_input)
  {
  }

void acl_special::clear_users()
  {
  }

void acl_special::clear_groups()
  {
  }

void acl_special::remove_user_configuration(const std::string &qmgr_input)
  {
  }

void acl_special::remove_group_configuration(const std::string &qmgr_input)
  {
  }

acl_special::acl_special() {}

acl_special limited_acls;

