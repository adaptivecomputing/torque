#include "license_pbs.h" /* See here for the software license */
#include <stdlib.h>
#include <stdio.h> /* fprintf */

#include "pbs_config.h"
#include "libpbs.h" /* connect_handle */
#include "attribute.h" /* attribute */
#include "net_connect.h" /* pbs_net_t */
#include "pbs_job.h" /* job */
#include "batch_request.h" /* batch_request */
#include "queue.h" /* pbs_queue */
#include "pbs_nodes.h" /* pbsnode */
#include "list_link.h" /* tlist_head, list_link */
#include "threadpool.h"
#include "array.h"
#include "machine.hpp"
#include "mom_hierarchy_handler.h"
#include "id_map.hpp"


threadpool_t *request_pool;
char *path_jobs = strdup("/var/spool/torque/server_priv/jobs/");
struct connect_handle connection[10];
attribute_def job_attr_def[JOB_ATR_LAST];
const char *pbs_o_host = "PBS_O_HOST";
const char *msg_routexceed = "Route queue lifetime exceeded";
int queue_rank = 0;
char *path_spool;
const char *msg_movejob = "Job moved to ";
pbs_net_t pbs_server_addr;
unsigned int pbs_server_port_dis;
const char *msg_manager = "%s at request of %s@%s";
int LOGLEVEL = 7; /* force logging code to be exercised as tests run */
bool commit_error = false;
bool expired = false;
bool job_exist = false;
bool other_fail = false;
bool script_fail = false;
bool jobfile_fail = false;
bool rdycommit_fail = false;
int  retry;
bool connect_fail = false;
bool cray_enabled = false;

mom_hierarchy_handler hierarchy_handler;
std::string global_string;
id_map node_mapper;
struct pbsnode reporter;
struct pbsnode *alps_reporter = &reporter;
const char *alps_reporter_feature  = "alps_reporter";
const char *alps_starter_feature   = "alps_login";

void translate_vector_to_range_string(std::string &range_string, const std::vector<int> &indices)
  {
  return;
  }

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


bool task_hosts_match(const char *one, const char *two)
  {
  return(true);
  }

void add_to_property_list(std::string &propstr, const char *token)
  {
  if (propstr.length() != 0)
    propstr += ",";

  propstr += token;
  }





void finish_sendmom(char *job_id, batch_request *preq, long start_time, char *node_name, int status, int o)
  {
  }

int PBSD_commit_get_sid(int connect, long *sid, char *jobid)
  {
  if (commit_error == true)
    return(-1);
  else
    {
    *sid = 10;
    return(0);
    }
  }

pbs_queue *find_queuebyname(const char *quename)
  {
  fprintf(stderr, "The call to find_queuebyname to be mocked!!\n");
  exit(1);
  }

char *parse_servername(const char *name, unsigned int *service)
  {
  fprintf(stderr, "The call to parse_servername to be mocked!!\n");
  exit(1);
  }

int PBSD_jobfile(int c, int req_type, char *path, char *jobid, enum job_file which)
  {
  if (jobfile_fail == true)
    return(-1);
  else
    return(PBSE_NONE);
  }

int job_save(job *pjob, int updatetype, int mom_port)
  {
  return(0);
  }

int svr_job_purge(job *pjob, int leaveSpoolFiles)
  {
  fprintf(stderr, "The call to job_purge to be mocked!!\n");
  exit(1);
  }

int PBSD_jscript(int c, const char *script_file, const char *jobid)
  {
  if (script_fail == true)
    return(-1);

  return(0);
  }

pbs_net_t get_hostaddr(int *local_errno, const char *hostname)
  {
  fprintf(stderr, "The call to get_hostaddr to be mocked!!\n");
  exit(1);
  }

void remove_stagein(job **pjob_ptr)
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

char *get_variable(job *pjob, const char *variable)
  {
  fprintf(stderr, "The call to get_variable to be mocked!!\n");
  exit(1);
  }

int enqueue_threadpool_request(void *(*func)(void *),void *arg, threadpool_t *tp)
  {
  fprintf(stderr, "The call to enqueue_threadpool_request to be mocked!!\n");
  exit(1);
  }

void remove_checkpoint(job **pjob_ptr)
  {
  fprintf(stderr, "The call to remove_checkpoint to be mocked!!\n");
  exit(1);
  }

int svr_dequejob(job *pjob, int val)
  {
  fprintf(stderr, "The call to svr_dequejob to be mocked!!\n");
  exit(1);
  }

int unlock_node(struct pbsnode *the_node, const char *id, const char *msg, int logging)
  {
  fprintf(stderr, "The call to unlock_node to be mocked!!\n");
  exit(1);
  }

void svr_disconnect(int handle) {}

void req_reject(int code, int aux, struct batch_request *preq, const char *HostName, const char *Msg) {}

int job_abt(struct job **pjobp, const char *text, bool b=false)
  {
  fprintf(stderr, "The call to job_abt to be mocked!!\n");
  exit(1);
  }

void *get_next(list_link pl, char *file, int line)
  {
  return(NULL);
  }

int job_route(job *jobp)
  {
  fprintf(stderr, "The call to job_route to be mocked!!\n");
  exit(1);
  }

int svr_enquejob(job *pjob, int has_sv_qs_mutex, const char *prev_id, bool have_reservation, bool recov)
  {
  fprintf(stderr, "The call to svr_enquejob to be mocked!!\n");
  exit(1);
  }

void attrl_fixlink(tlist_head *phead)
  {
  fprintf(stderr, "The call to attrl_fixlink to be mocked!!\n");
  exit(1);
  }

int svr_connect(pbs_net_t hostaddr, unsigned int port, int *err, struct pbsnode *pnode, void *(*func)(void *))
  {
  static int retries = 0;
  if (retry != 0)
    {
    if (retry != retries++)
      return(PBS_NET_RC_RETRY);
    else
      return(7);
    }
  else if (connect_fail == true)
    return(PBS_NET_RC_FATAL);
  else
    return(7);
  }

struct pbsnode *PGetNodeFromAddr(pbs_net_t addr)
  {
  fprintf(stderr, "The call to PGetNodeFromAddr to be mocked!!\n");
  exit(1);
  }

int PBSD_rdytocmt(int connect, char *jobid)
  {
  if (rdycommit_fail == true)
    return(-1);

  return(0);
  }

void add_dest(job *jobp)
  {
  fprintf(stderr, "The call to add_dest to be mocked!!\n");
  exit(1);
  }

int svr_setjobstate(job *pjob, int newstate, int newsubstate, int  has_queue_mute)
  {
  pjob->ji_qs.ji_state = newstate;
  pjob->ji_qs.ji_substate = newsubstate;
  return(0);
  }

job *svr_find_job(const char *jobid, int get_subjob)
  {
  static job pjob;

  if (!strcmp(jobid, "1.napali"))
    {
    strcpy(pjob.ji_qs.ji_jobid, "1.napali");
    return(&pjob);
    }

  return(NULL);
  }

int unlock_queue(struct pbs_queue *the_queue, const char *method_name, const char *msg, int logging)
  {
  fprintf(stderr, "The call to unlock_queue to be mocked!!\n");
  exit(1);
  }

void svr_evaljobstate(job &pjob, int &newstate, int &newsub, int forceeval)
  {
  fprintf(stderr, "The call to svr_evaljobstate to be mocked!!\n");
  exit(1);
  }

char *PBSD_queuejob(int connect, int *local_errno, const char *jobid, const char *destin, struct attropl *attrib, char *extend)
  {
  if (expired == true)
    {
    *local_errno = PBSE_EXPIRED;
    return(NULL);
    }
  else if (job_exist == true)
    {
    *local_errno = PBSE_JOBEXIST;
    return(NULL);
    }
  else if (other_fail == true)
    {
    *local_errno = PBSE_TIMEOUT;
    return(NULL);
    }

  return(strdup(jobid));
  }

void delete_link(struct list_link *old) {}

char *pbse_to_txt(int err)
  {
  return(strdup("err"));
  }

int svr_chkque(job *pjob, pbs_queue *pque, char *hostname, int mtype, char *EMsg)
  {
  fprintf(stderr, "The call to svr_chkque to be mocked!!\n");
  exit(1);
  }

int lock_queue(struct pbs_queue *the_queue, const char *method_name, const char *msg, int logging)
  {
  fprintf(stderr, "The call to lock_queue to be mocked!!\n");
  exit(1);
  }

int get_parent_dest_queues(char *queue_parent_name, char *queue_dest_name, pbs_queue **parent, pbs_queue **dest, job **pjob_ptr)
  {
  fprintf(stderr, "The call to get_parent_dest_queues to be mocked!!\n");
  exit(1);
  }

void close_conn(

  int sd,        /* I */
  int has_mutex) /* I */

  {
  }

struct pbs_queue *lock_queue_with_job_held(

  struct pbs_queue  *pque,
  job              **pjob_ptr)

  {
  return(NULL);
  }

void svr_mailowner(job *pjob, int mailpoint, int force, const char *text) {}

int unlock_ji_mutex(job *pjob, const char *id, const char *msg, int logging)
  {
  return(0);
  }

bool get_jobs_array_recycled = false;
bool get_jobs_array_fail = false;

job_array *get_jobs_array(

  job **pjob_ptr)

  {
  static job_array pa;
  strcpy(pa.ai_qs.fileprefix, "2.napali");

  if (get_jobs_array_recycled == true)
    {
    *pjob_ptr = NULL;
    return(NULL);
    }
  else if (get_jobs_array_fail == true)
    {
    return(NULL);
    }
  else
    return(&pa);
  }

int unlock_ai_mutex(

  job_array  *pa,
  const char *id,
  const char *msg,
  int        logging)

  {
  return(0);
  }

void log_err(int errnum, const char *routine, const char *text) {}
void log_record(int eventtype, int objclass, const char *objname, const char *text) {}
void log_event(int eventtype, int objclass, const char *objname, const char *text) {}
void log_ext(int type, const char *func_name, const char *msg, int o) {}

int encode_exec_host(pbs_attribute *attr, tlist_head *phead, const char *atname, const char *rsname, int mode, int perm)
  {
  char *old_str;
  char *export_str;
  char *pipe;
  int   rc;

  if (attr->at_val.at_str == NULL)
    return(PBSE_NONE);

  if ((export_str = strdup(attr->at_val.at_str)) == NULL)
    return(PBSE_SYSTEM);

  while ((pipe = strchr(export_str, '|')) != NULL)
    *pipe = '+';

  old_str = attr->at_val.at_str;
  attr->at_val.at_str = export_str;

  rc = encode_str(attr, phead, atname, rsname, mode, perm);
  attr->at_val.at_str = old_str;
  free(export_str);

  return(rc);
  }

char *pbs_geterrmsg(int con)
  {
  return(strdup("err"));
  }

int get_svr_attr_l(int index, long *val)
  {
  return(0);
  }

int get_svr_attr_b(int index, bool *b)
  {
  return(0);
  }

int ctnodes(char *spec)
  {
  int   ct = 0;
  char *pc;

  while (1)
    {
    while (isspace((int)*spec))
      ++spec;

    if (isdigit((int)*spec))
      ct += atoi(spec);
    else
      ++ct;

    if ((pc = strchr(spec, '+')) == NULL)
      break;

    spec = pc + 1;
    }  /* END while (1) */

  return(ct);
  }  /* END ctnodes() */

char * csv_find_string(const char *csv_str, const char *search_str)
  {
  fprintf(stderr, "The call to csv_find_string to be mocked!!\n");
  exit(1);
  }

char *csv_nth(const char *csv_str, int n)
  {
  fprintf(stderr, "The call to csv_nth to be mocked!!\n");
  exit(1);
  }

int csv_length(const char *csv_str)
  {
  fprintf(stderr, "The call to csv_length to be mocked!!\n");
  exit(1);
  }

std::string get_path_jobdata(const char *a, const char *b)
  {
  std::string ret_string = path_jobs;
  return ret_string;
  }

job::job() {}
job::~job() {}

void update_failure_counts(const char *node_name, int rc) {}

int pbs_getaddrinfo(const char *pNode, struct addrinfo  *pHints, struct addrinfo **ppAddrInfoOut)
  {
  return(0);
  }

int record_node_property_list(std::string const &propstr, tlist_head *atrlist_ptr)
  {
  if ((global_string.length() > 0) && (propstr != global_string))
    {
    // no match, return not ok
    return(1);
    }

  // return ok
  return(0);
  }


struct pbsnode *next_host(

  all_nodes           *an,    /* I */
  all_nodes_iterator **iter,  /* M */
  struct pbsnode      *held)  /* I */

  {
  static struct pbsnode pnode;

  return(&pnode);
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

#include "../../src/lib/Libutils/machine.cpp"
#include "../../src/lib/Libutils/numa_socket.cpp"
#include "../../src/lib/Libutils/numa_chip.cpp"
#include "../../src/lib/Libutils/numa_core.cpp"
#include "../../src/lib/Libutils/numa_pci_device.cpp"
#include "../../src/lib/Libattr/req.cpp"
#include "../../src/lib/Libattr/complete_req.cpp"
#include "../../src/lib/Libutils/allocation.cpp"
#include "../../src/server/pbsnode.cpp"
#include "../../src/server/id_map.cpp"

struct pbsnode *find_nodebyname(

  const char *nodename)

  {
  static struct pbsnode pnode;

  return(&pnode);
  }

void update_node_state(pbsnode *pnode, int state) 
  {
  pnode->nd_state = state;
  }

job_array::job_array() {}
job_array::~job_array() {}

array_info::array_info() {}
array_info::~array_info() {}

