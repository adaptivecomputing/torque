#include "license_pbs.h" /* See here for the software license */
#include <stdlib.h>
#include <stdio.h> /* fprintf */
#include <sys/ioctl.h> /* winsize */
#include <termios.h> /* termios */

#include "pbs_ifl.h" /* PBS_MAXHOSTNAME */
#include "log.h" /* LOG_BUF_SIZE */
#include "pbs_job.h" /* job, task */
#include "list_link.h" /* list_link */


char log_buffer[LOG_BUF_SIZE];
int termin_child = 0;
int multi_mom = 0;
int exiting_tasks = 0;
char mom_host[PBS_MAXHOSTNAME + 1];
unsigned int pbs_rm_port = 0;
tlist_head svr_alljobs;
char noglobid[] = "none";
int LOGLEVEL = 7; /* force logging code to be exercised as tests run */
char *AllocParCmd = NULL;


int job_save(job *pjob, int updatetype, int mom_port)
  {
  fprintf(stderr, "The call to job_save needs to be mocked!!\n");
  exit(1);
  }

char *get_job_envvar(job *pjob, char *variable)
  {
  fprintf(stderr, "The call to get_job_envvar needs to be mocked!!\n");
  exit(1);
  }

int mom_get_sample(void)
  {
  fprintf(stderr, "The call to mom_get_sample needs to be mocked!!\n");
  exit(1);
  }

int kill_task(struct task *task, int sig, int pg)
  {
  fprintf(stderr, "The call to kill_task needs to be mocked!!\n");
  exit(1);
  }

void *get_prior(list_link pl, char *file, int line)
  {
  fprintf(stderr, "The call to get_prior needs to be mocked!!\n");
  exit(1);
  }

void *get_next(list_link pl, char *file, int line)
  {
  fprintf(stderr, "The call to get_next needs to be mocked!!\n");
  exit(1);
  }

int mom_set_use(job *pjob)
  {
  fprintf(stderr, "The call to mom_set_use needs to be mocked!!\n");
  exit(1);
  }

int task_save(task *ptask)
  {
  fprintf(stderr, "The call to task_save needs to be mocked!!\n");
  exit(1);
  }

void check_partition_confirm_script(char *path, char *msg) {}

void log_err(int errnum, const char *routine, const char *text) {}
void log_record(int eventtype, int objclass, const char *objname, const char *text) {}
void log_event(int eventtype, int objclass, const char *objname, const char *text) {}

#ifndef USEOLDTTY
extern "C"
{
int openpty (int *__amaster, int *__aslave, char *__name, const struct termios *__termp, const struct winsize *__winp)
  {
  fprintf(stderr, "The call to openpty needs to be mocked!!\n");
  exit(1);
  }
}
#endif

int kill_task(job *pjob, struct task *task, int sig, int pg)
  {
  return(0);
  }

void check_and_act_on_obit(job *pjob, int rank) {}
