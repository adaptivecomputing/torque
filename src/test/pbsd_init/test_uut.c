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

extern char global_log_ext_msg[LOCAL_LOG_BUF_SIZE];
extern int enque_rc;
extern int evaluated;
extern int aborted;
extern pbs_queue *allocd_queue;

START_TEST(test_check_jobs_queue)
  {
  job pjob;

  memset(&pjob, 0, sizeof(pjob));
  sprintf(pjob.ji_qs.ji_jobid, "1.napali");
  sprintf(pjob.ji_qs.ji_queue, "lost");

  check_jobs_queue(&pjob);
  fail_unless(allocd_queue != NULL);
  fail_unless(!strcmp(allocd_queue->qu_qs.qu_name, pjob.ji_qs.ji_queue));
  fail_unless(allocd_queue->qu_qs.qu_type == QTYPE_Execution);
  fail_unless(allocd_queue->qu_attr[QA_ATR_GhostQueue].at_val.at_long == 1);
  fail_unless(!strcmp(allocd_queue->qu_attr[QA_ATR_QType].at_val.at_str, "Execution"));
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
