#include <stdio.h>
#include <stdlib.h>
#include <check.h>

#include "exiting_jobs.h"
#include "log.h"
#include "hash_table.h"

int remove_entry_from_exiting_list(job_exiting_retry_info *jeri);
int retry_job_exit(job_exiting_retry_info *jeri);
int check_exiting_jobs();


START_TEST(record_job_as_exiting_test)
  {
  job pjob;

  memset(&pjob, 0, sizeof(pjob));
  strcpy(pjob.ji_qs.ji_jobid, "1.tom");

  fail_unless(record_job_as_exiting(&pjob) == 0, "Couldn't record job as exiting");
  }
END_TEST




START_TEST(remove_job_from_exiting_list_test)
  {
  job pjob;

  memset(&pjob, 0, sizeof(pjob));

  strcpy(pjob.ji_qs.ji_jobid, "2.napali");
  fail_unless(remove_job_from_exiting_list(&pjob) == 0, "Couldn't remove job from exiting list");
 
  strcpy(pjob.ji_qs.ji_jobid, "1.napali");
  fail_unless(remove_job_from_exiting_list(&pjob) == 0, "Couldn't remove job from exiting list");
  }
END_TEST




START_TEST(remove_entry_from_exiting_list_test)
  {
  job_exiting_retry_info *jeri = calloc(1, sizeof(job_exiting_retry_info));
  strcpy(jeri->jobid, "3.napali");
  fail_unless(remove_entry_from_exiting_list(jeri) == 0, "Couldn't remove entry");
  
  jeri = calloc(1, sizeof(job_exiting_retry_info));
  strcpy(jeri->jobid, "1.napali");
  fail_unless(remove_entry_from_exiting_list(jeri) == KEY_NOT_FOUND, "Removed entry that wasn't there");

  }
END_TEST




START_TEST(retry_job_exit_test)
  {
  job_exiting_retry_info *jeri = calloc(1, sizeof(job_exiting_retry_info));

  strcpy(jeri->jobid, "1.napali");

  fail_unless(retry_job_exit(jeri) == 0, "Didn't retry job");
  fail_unless(jeri->attempts == 1, "Didn't increment attempt count");

  jeri->attempts = 100;
  fail_unless(retry_job_exit(jeri) == 0, "Didn't remove after max attempts");
  }
END_TEST




START_TEST(check_exiting_jobs_test)
  {
  fail_unless(check_exiting_jobs() == 0, "Failure when inspecting");
  }
END_TEST




Suite *exiting_jobs_suite(void)
  {
  Suite *s = suite_create("exiting_jobs test suite methods");
  TCase *tc_core = tcase_create("record_job_as_exiting_test");
  tcase_add_test(tc_core, record_job_as_exiting_test);
  suite_add_tcase(s, tc_core);
  
  tc_core = tcase_create("remove_job_from_exiting_list_test");
  tcase_add_test(tc_core, remove_job_from_exiting_list_test);
  suite_add_tcase(s, tc_core);

  tc_core = tcase_create("remove_entry_from_exiting_list_test");
  tcase_add_test(tc_core, remove_entry_from_exiting_list_test);
  suite_add_tcase(s, tc_core);

  tc_core = tcase_create("retry_job_exit_test");
  tcase_add_test(tc_core, retry_job_exit_test);
  suite_add_tcase(s, tc_core);

  tc_core = tcase_create("check_exiting_jobs_test");
  tcase_add_test(tc_core, check_exiting_jobs_test);
  suite_add_tcase(s, tc_core);
  
  return(s);
  }

void rundebug()
  {
  }

int main(void)
  {
  int number_failed = 0;
  SRunner *sr = NULL;
  rundebug();
  sr = srunner_create(exiting_jobs_suite());
  srunner_set_log(sr, "exiting_jobs_suite.log");
  srunner_run_all(sr, CK_NORMAL);
  number_failed = srunner_ntests_failed(sr);
  srunner_free(sr);
  return(number_failed);
  }
