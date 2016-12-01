#include "license_pbs.h" /* See here for the software license */
#include "pbsd_init.h"
#include "pbs_job.h"
#include "test_pbsd_init.h"
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <log.h>

#include "pbs_error.h"
#include "queue.h"

int mk_subdirs(char **);
int pbsd_init_reque(job *, int);
void check_jobs_queue(job *pjob);
void remove_invalid_allocations(pbsnode *pnode);

extern char global_log_ext_msg[LOCAL_LOG_BUF_SIZE];
extern int enque_rc;
extern int evaluated;
extern int aborted;
extern int freed_job_allocation;
extern int job_state;
extern int unlocked_job;
extern int locked_job;
extern bool dont_find_job;
extern bool dont_find_node;
extern pbs_queue *allocd_queue;

#ifdef PENABLE_LINUX_CGROUPS
START_TEST(test_remove_invalid_allocations)
  {
  pbsnode pnode;

  // Set that we shouldn't find the jobs. There are 3 jobs, so we should free all 3.
  dont_find_job = true;
  dont_find_node = false;
  freed_job_allocation = 0;
  job_state = JOB_STATE_RUNNING;
  remove_invalid_allocations(&pnode);
  fail_unless(freed_job_allocation == 3);

  // Set that we find the jobs, but they aren't running. This should free all 3 again.
  dont_find_job = false;
  freed_job_allocation = 0;
  job_state = JOB_STATE_QUEUED;
  remove_invalid_allocations(&pnode);
  fail_unless(freed_job_allocation == 3);

  // Make it so that the jobs are in a running state, so none should get freed.
  freed_job_allocation = 0;
  job_state = JOB_STATE_RUNNING;
  remove_invalid_allocations(&pnode);
  fail_unless(freed_job_allocation == 0);
  }
END_TEST
#endif


START_TEST(test_check_jobs_queue)
  {
  job pjob;

  memset(&pjob, 0, sizeof(pjob));
  sprintf(pjob.ji_qs.ji_jobid, "1.napali");
  sprintf(pjob.ji_qs.ji_queue, "lost");

  unlocked_job = 0;
  locked_job = 0;

  check_jobs_queue(&pjob);
  fail_unless(allocd_queue != NULL);
  fail_unless(!strcmp(allocd_queue->qu_qs.qu_name, pjob.ji_qs.ji_queue));
  fail_unless(allocd_queue->qu_qs.qu_type == QTYPE_Execution);
  fail_unless(allocd_queue->qu_attr[QA_ATR_GhostQueue].at_val.at_long == 1);
  fail_unless(!strcmp(allocd_queue->qu_attr[QA_ATR_QType].at_val.at_str, "Execution"));
  
  // check_jobs_queue should unlock and re-lock the job once each
  fail_unless(unlocked_job == 1);
  fail_unless(locked_job == 1);
  }
END_TEST


START_TEST(test_mk_subdirs)
  {
  char *paths[3] = { NULL };

  // null path - expect failure
  fail_unless(mk_subdirs(NULL) != PBSE_NONE);
  fail_unless(global_log_ext_msg[0] == '\0');

  // no paths defined - expect success
  fail_unless(mk_subdirs(paths) == PBSE_NONE);
  fail_unless(global_log_ext_msg[0] == '\0');

  // path defined - expect success
  fail_unless((paths[0] = strdup("./subdir")) != NULL);
  fail_unless(mk_subdirs(paths) == PBSE_NONE);
  fail_unless(strncmp("created", global_log_ext_msg, strlen("created")) == 0);

  // path defined - expect success (no log msg since already created)
  global_log_ext_msg[0] = '\0';
  fail_unless((paths[0] = strdup("./subdir")) != NULL);
  fail_unless(mk_subdirs(paths) == PBSE_NONE);
  fail_unless(global_log_ext_msg[0] == '\0');

  // clean up
  fail_unless(system("rmdir ./subdir*") == 0);
  }
END_TEST


START_TEST(test_pbsd_init_reque)
  {
  job *pjob = (job *)calloc(1, sizeof(job));

  evaluated = 0;
  aborted = 0;
  enque_rc = PBSE_NONE;
  fail_unless(pbsd_init_reque(pjob, 0) == PBSE_NONE);
  fail_unless(evaluated == 0);

  enque_rc = PBSE_BADDEPEND;
  fail_unless(pbsd_init_reque(pjob, 1) == PBSE_NONE);
  fail_unless(evaluated == 1);
  
  enque_rc = 10;
  fail_unless(pbsd_init_reque(pjob, 1) == 10);
  fail_unless(evaluated == 1);
  }
END_TEST

Suite *pbsd_init_suite(void)
  {
  Suite *s = suite_create("pbsd_init_suite methods");
  TCase *tc_core = tcase_create("test_mk_subdirs");
  tcase_add_test(tc_core, test_mk_subdirs);
#ifdef PENABLE_LINUX_CGROUPS
  tcase_add_test(tc_core, test_remove_invalid_allocations);
#endif
  suite_add_tcase(s, tc_core);

  tc_core = tcase_create("test_pbsd_init_reque");
  tcase_add_test(tc_core, test_pbsd_init_reque);
  tcase_add_test(tc_core, test_check_jobs_queue);
  suite_add_tcase(s, tc_core);

  return s;
  }

void rundebug()
  {
  }

int main(void)
  {
  int number_failed = 0;
  SRunner *sr = NULL;
  rundebug();
  sr = srunner_create(pbsd_init_suite());
  srunner_set_log(sr, "pbsd_init_suite.log");
  srunner_run_all(sr, CK_NORMAL);
  number_failed = srunner_ntests_failed(sr);
  srunner_free(sr);
  return number_failed;
  }
