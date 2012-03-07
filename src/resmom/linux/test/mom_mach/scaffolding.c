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

int svr_resc_size = 0;
char path_meminfo[MAX_LINE];
char no_parm[] = "required parameter not found";
long system_ncpus = 0;
time_t time_now = 0;
int ignmem = 0;
double wallfactor = 1.00;
double cputfactor = 1.00;
int exiting_tasks = 0;
char extra_parm[] = "extra parameter(s)";
int igncput = 0;
char *ret_string;
resource_def *svr_resc_def;
int ignvmem = 0;
char *msg_momsetlim = "Job start failed. Can't set \"%s\" limit: %s.\n";
tlist_head svr_alljobs;
int LOGLEVEL = 0;
int ignwalltime = 0;
int rm_errno;




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

char *nullproc(struct rm_attribute *attrib)
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
  fprintf(stderr, "The call to get_next needs to be mocked!!\n");
  exit(1);
  }

void free_pidlist(struct pidl *pl)
  {
  fprintf(stderr, "The call to free_pidlist needs to be mocked!!\n");
  exit(1);
  }

resource_def *find_resc_def(resource_def *rscdf, char *name, int limit)
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

char *loadave(struct rm_attribute *attrib)
  {
  fprintf(stderr, "The call to loadave needs to be mocked!!\n");
  exit(1);
  }
