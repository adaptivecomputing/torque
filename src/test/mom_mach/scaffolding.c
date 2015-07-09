#include "license_pbs.h" /* See here for the software license */
#include <stdlib.h>
#include <stdio.h> /* fprintf */
#include <pwd.h> /* struct passwd */

#include "../../../mom_main.h" /* MAX_LINE */
#include "resource.h" /* resource_def, resource */
#include "list_link.h" /* tlist_head, list_link, pidl */
#include "log.h" /* LOG_BUF_SIZE */
#include "attribute.h" /* pbs_attribute */
#include "resmon.h" /* rm_attribute */
#include "pbs_job.h" /* task */
#include "pbs_nodes.h"
#include "pbs_config.h"
#include "node_frequency.hpp"


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

struct passwd * getpwnam_ext(char * user_name)
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

node_frequency nd_frequency;
void from_frequency(struct cpu_frequency_value *pfreq, char *cvnbuf) {}
