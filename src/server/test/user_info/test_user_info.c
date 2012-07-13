#include <stdio.h>
#include <stdlib.h>
#include <check.h>
#include <errno.h>

#include "user_info.h"

unsigned int get_num_queued(char *);
unsigned int count_jobs_submitted(job *);

START_TEST(initialize_user_info_holder_test)
  {
  initialize_user_info_holder();

  fail_unless(users.ui_ra != NULL, "resizable array not initialized");
  fail_unless(users.ui_ht != NULL, "hash table not initialized");
  fail_unless(users.ui_mutex != NULL, "mutex not initialized");
  }
END_TEST




START_TEST(get_num_queued_test)
  {
  unsigned int queued;
  initialize_user_info_holder();

  queued = get_num_queued("bob");
  fail_unless(queued == 0, "incorrect queued count for bob");

  queued = get_num_queued("tom");
  fail_unless(queued == 1, "incorrect queued count for tom");
  }
END_TEST




START_TEST(count_jobs_submitted_test)
  {
  unsigned int submitted;
  job          pjob;

  memset(&pjob, 0, sizeof(pjob));
  initialize_user_info_holder();

  submitted = count_jobs_submitted(&pjob);
  fail_unless(submitted == 1, "incorrect count for non-array job");

  pjob.ji_wattr[JOB_ATR_job_array_request].at_val.at_str = "0-10";
  submitted = count_jobs_submitted(&pjob);
  fail_unless(submitted == 11, "incorrect count for 0-10");
  }
END_TEST



START_TEST(can_queue_new_job_test)
  {
  job pjob;

  memset(&pjob, 0, sizeof(pjob));
  initialize_user_info_holder();

  fail_unless(can_queue_new_job("bob", &pjob) == TRUE, "user without a job can't queue one?");
  fail_unless(can_queue_new_job("tom", &pjob) == FALSE, "tom allowed over limit");
  pjob.ji_wattr[JOB_ATR_job_array_request].at_val.at_str = "0-10";

  fail_unless(can_queue_new_job("bob", &pjob) == FALSE, "array job allowed over limit");
  fail_unless(can_queue_new_job("tom", &pjob) == FALSE, "array job allowed over limit");
  }
END_TEST



START_TEST(increment_queued_jobs_test)
  {
  job pjob;

  memset(&pjob, 0, sizeof(pjob));
  initialize_user_info_holder();

  fail_unless(increment_queued_jobs("tom", &pjob) == 0, "can't increment queued jobs");
  fail_unless(increment_queued_jobs("bob", &pjob) == 0, "can't increment queued jobs");
  fail_unless(increment_queued_jobs("bob", &pjob) == ENOMEM, "didn't get failure");
  }
END_TEST




START_TEST(decrement_queued_jobs_test)
  {
  initialize_user_info_holder();

  fail_unless(decrement_queued_jobs("bob") == THING_NOT_FOUND, "decremented for non-existent user");
  fail_unless(decrement_queued_jobs("tom") == 0, "couldn't decrement for tom?");
  fail_unless(get_num_queued("tom") == 0, "didn't actually decrement tom");

  }
END_TEST




Suite *user_info_suite(void)
  {
  Suite *s = suite_create("user_info test suite methods");
  TCase *tc_core = tcase_create("initialize_user_info_holder_test");
  tcase_add_test(tc_core, initialize_user_info_holder_test);
  suite_add_tcase(s, tc_core);
  
  tc_core = tcase_create("get_num_queued_test");
  tcase_add_test(tc_core, get_num_queued_test);
  suite_add_tcase(s, tc_core);
  
  tc_core = tcase_create("count_jobs_submitted_test");
  tcase_add_test(tc_core, count_jobs_submitted_test);
  suite_add_tcase(s, tc_core);
  
  tc_core = tcase_create("can_queue_new_job_test");
  tcase_add_test(tc_core, can_queue_new_job_test);
  suite_add_tcase(s, tc_core);

  tc_core = tcase_create("increment_queued_jobs_test");
  tcase_add_test(tc_core, increment_queued_jobs_test);
  suite_add_tcase(s, tc_core);

  tc_core = tcase_create("decrement_queued_jobs_test");
  tcase_add_test(tc_core, decrement_queued_jobs_test);
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
  sr = srunner_create(user_info_suite());
  srunner_set_log(sr, "user_info_suite.log");
  srunner_run_all(sr, CK_NORMAL);
  number_failed = srunner_ntests_failed(sr);
  srunner_free(sr);
  return(number_failed);
  }

