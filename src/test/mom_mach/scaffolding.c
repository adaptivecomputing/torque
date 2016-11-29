#include "license_pbs.h" /* See here for the software license */
#include <stdlib.h>
#include <stdio.h> /* fprintf */
#include <unistd.h>
#include <pwd.h> /* struct passwd */
#include <string>
#include <sys/types.h>
#include <ctype.h>
#include <grp.h>

#include "pbs_config.h"
#include <list>

#include "mom_main.h" /* MAX_LINE */
#include "resource.h" /* resource_def, resource */
#include "list_link.h" /* tlist_head, list_link, pidl */
#include "log.h" /* LOG_BUF_SIZE */
#include "attribute.h" /* pbs_attribute */
#include "resmon.h" /* rm_attribute */
#include "pbs_job.h" /* task */
#include "pbs_nodes.h"
#include "node_frequency.hpp"
#include "machine.hpp"
#include "log.h"

extern std::string cg_memory_path;
char         mom_alias[PBS_MAXHOSTNAME + 1];

Machine this_node;
std::string cg_cpuacct_path;
std::list<job *> alljobs_list;
char log_buffer[LOG_BUF_SIZE];
int svr_resc_size = 0;
char path_meminfo[MAX_LINE];
const char *no_parm = "required parameter not found";
long system_ncpus = 0;
time_t time_now = 0;
int ignmem = 0;
double wallfactor = 1.00;
double cputfactor = 1.00;
int exiting_tasks = 0;
const char *extra_parm = "extra parameter(s)";
int igncput = 0;
char *ret_string;
resource_def *svr_resc_def;
int ignvmem = 0;
const char *msg_momsetlim = "Job start failed. Can't set \"%s\" limit: %s.\n";
tlist_head svr_alljobs;
int LOGLEVEL = 7; /* force logging code to be exercised as tests run */
int ignwalltime = 0;
int rm_errno;
char         mom_host[PBS_MAXHOSTNAME + 1];
#ifdef NUMA_SUPPORT
int       num_node_boards;
nodeboard node_boards[MAX_NODE_BOARDS]; 
int       numa_index;
#endif
int   job_oom_score_adjust = 0;  /* no oom score adjust by default */
int   mom_oom_immunize = 0;  /* make pbs_mom processes immune? no by default */

void *get_next_return_value = NULL;

#ifdef PENABLE_LINUX26_CPUSETS
//hwloc_topology_t topology = NULL;       /* system topology */

int      memory_pressure_threshold = 0; /* 0: off, >0: check and kill */
short    memory_pressure_duration  = 0; /* 0: off, >0: check and kill */
int      MOMConfigUseSMT           = 1; /* 0: off, 1: on */
#endif

int trq_cg_get_task_stats(

  const char         *job_id,
  const unsigned int  req_index,
  const unsigned int  task_index,
  allocation         &al)
  {
  return(0);
  }

void log_event(int event, int event_class, const char *func_name, const char buf)
  {}

resource *add_resource_entry(pbs_attribute *pattr, resource_def *prdef)
  {
  fprintf(stderr, "The call to add_resource_entry needs to be mocked!!\n");
  exit(1);
  }

void checkret(char **spot, long len)
  {
  fprintf(stderr, "The call to checkret needs to be mocked!!\n");
  exit(1);
  }

const char *nullproc(struct rm_attribute *attrib)
  {
  fprintf(stderr, "The call to nullproc needs to be mocked!!\n");
  exit(1);
  }

struct rm_attribute *momgetattr(char *str)
  {
  fprintf(stderr, "The call to rm_attribute needs to be mocked!!\n");
  exit(1);
  }

void *get_next(list_link pl, char *file, int line)
  {
  void *p = get_next_return_value;

  /* return NULL for 2nd and subsequent calls */
  get_next_return_value = NULL;

  return(p);
  }

void free_pidlist(struct pidl *pl)
  {
  fprintf(stderr, "The call to free_pidlist needs to be mocked!!\n");
  exit(1);
  }

resource_def *find_resc_def(resource_def *rscdf, const char *name, int limit)
  {
  fprintf(stderr, "The call to find_resc_def needs to be mocked!!\n");
  exit(1);
  }

struct passwd * getpwnam_ext(char **user_buf, char * user_name)
  {
  fprintf(stderr, "The call to getpwnam_ext needs to be mocked!!\n");
  exit(1);
  }

char *pbse_to_txt(int err)
  {
  fprintf(stderr, "The call to pbse_to_txt needs to be mocked!!\n");
  exit(1);
  }

int task_save(task *ptask)
  {
  fprintf(stderr, "The call to task_save needs to be mocked!!\n");
  exit(1);
  }

resource *find_resc_entry(pbs_attribute *pattr, resource_def *rscdf)
  {
  fprintf(stderr, "The call to find_resc_entry needs to be mocked!!\n");
  exit(1);
  }

const char *loadave(struct rm_attribute *attrib)
  {
  fprintf(stderr, "The call to loadave needs to be mocked!!\n");
  exit(1);
  }

void log_err(int errnum, const char *routine, const char *text) {}
void log_record(int eventtype, int objclass, const char *objname, const char *text) {}
void log_event(int eventtype, int objclass, const char *objname, const char *text) {}
void log_ext(int code, const char *src, const char *msg, int level) {}

int get_cpuset_mempressure(const char *path)
  {
  return(0);
  }

pidl *get_cpuset_pidlist(const char *path, pidl *bob)
  {
  return(NULL);
  }

bool am_i_mother_superior(const job &pjob)
  {
  return(false);
  }

char *threadsafe_tokenizer(

  char       **str,    /* M */
  const char  *delims) /* I */

  {
  return(NULL);
  }

bool node_frequency::set_frequency(cpu_frequency_type, unsigned long, unsigned long)
  {
  return(false);
  }

node_frequency::node_frequency() {}
node_frequency::~node_frequency() {}

bool node_frequency::get_frequency(

  cpu_frequency_type &type,
  unsigned long      &currMhz,
  unsigned long      &maxMhz,
  unsigned long      &minMhz)

  {
  return(false);
  }

bool node_frequency::get_frequency_string(std::string& str,bool full)
 {
 return(false);
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



node_frequency nd_frequency;
void from_frequency(struct cpu_frequency_value *pfreq, char *cvnbuf) {}

void translate_vector_to_range_string(std::string &range_string, const std::vector<int> &indices)
  {
  return;
  }


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


void capture_until_close_character(

  char        **start,
  std::string  &storage,
  char          end) {}

bool task_hosts_match(const char *one, const char *two)
  {
  return(true);
  }

Machine::Machine() {}
Machine::~Machine() {}
Socket::Socket() {}
Socket::~Socket() {}
PCI_Device::PCI_Device() {}
PCI_Device::~PCI_Device() {}
Chip::Chip() {}
Chip::~Chip() {}
Core::Core() {}
Core::~Core() {}

int Machine::getHardwareStyle() const
  {
  return(this->hardwareStyle);
  }
      
int trq_cg_get_task_cput_stats(

  const char         *job_id,
  const unsigned int  req_index,
  const unsigned int  task_index,
  unsigned long      &cput_used)

  {
  return(0);
  }

int trq_cg_get_task_memory_stats(

  const char         *job_id,
  const unsigned int  req_index,
  const unsigned int  task_index,
  unsigned long long &mem_used)

  {
  return(0);
  }

void free_pwnam(

  struct passwd *pwdp,
  char          *user_buf)

  {
  if (user_buf)
    {
    free(user_buf);
    user_buf = NULL;
    }

  if (pwdp)
    {
    free(pwdp);
    pwdp = NULL;
    }

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
  char *grp_name) /* I */

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


task::~task() {}

#ifdef USE_RESOURCE_PLUGIN
void report_job_resources(
  
  const std::string &jid,
  const std::set<pid_t> &job_pids,
  std::map<std::string, std::string> &usage_info) {}
#endif

#include "../../src/lib/Libattr/req.cpp"
#include "../../src/lib/Libattr/complete_req.cpp"
#include "../../src/lib/Libutils/allocation.cpp"
