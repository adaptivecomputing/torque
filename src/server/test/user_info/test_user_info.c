#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

#include "user_info.h"
#include <check.h>

unsigned int get_num_queued(user_info_holder *, char *);
unsigned int count_jobs_submitted(job *);



START_TEST(get_num_queued_test)
  {
  unsigned int queued;
  users.clear();
  user_info ui;

  ui.user_name = (char *)"tom";
  ui.num_jobs_queued = 1;

  users.insert(&ui,ui.user_name);

  queued = get_num_queued(&users, (char *)"bob");
  fail_unless(queued == 0, "incorrect queued count for bob");

  queued = get_num_queued(&users, (char *)"tom");
  fail_unless(queued == 1, "incorrect queued count for tom");
  }
END_TEST




START_TEST(count_jobs_submitted_test)
  {
  unsigned int submitted;
  job          pjob;

  memset(&pjob, 0, sizeof(pjob));
  users.clear();

  submitted = count_jobs_submitted(&pjob);
  fail_unless(submitted == 1, "incorrect count for non-array job");

  pjob.ji_wattr[JOB_ATR_job_array_request].at_val.at_str = (char *)"0-10";
  submitted = count_jobs_submitted(&pjob);
  fail_unless(submitted == 11, "incorrect count for 0-10");
  }
END_TEST



START_TEST(can_queue_new_job_test)
  {
  job pjob;

  memset(&pjob, 0, sizeof(pjob));
  users.clear();

  user_info *ui = (user_info *)calloc(1,sizeof(user_info));

  ui->user_name = strdup("tom");
  ui->num_jobs_queued = 1;

  users.insert(ui,ui->user_name);

  ui = (user_info *)calloc(1,sizeof(user_info));

  ui->user_name = strdup("bob");
  ui->num_jobs_queued = 0;

  users.insert(ui,ui->user_name);


  fail_unless(can_queue_new_job((char *)"bob", &pjob) == TRUE, "user without a job can't queue one?");
  fail_unless(can_queue_new_job((char *)"tom", &pjob) == FALSE, (char *)"tom allowed over limit");
  pjob.ji_wattr[JOB_ATR_job_array_request].at_val.at_str = (char *)"0-10";

  fail_unless(can_queue_new_job((char *)"bob", &pjob) == FALSE, "array job allowed over limit");
  fail_unless(can_queue_new_job((char *)"tom", &pjob) == FALSE, "array job allowed over limit");
  }
END_TEST



START_TEST(increment_queued_jobs_test)
  {
  job pjob;

  memset(&pjob, 0, sizeof(pjob));
  users.clear();

  user_info *ui = (user_info *)calloc(1,sizeof(user_info));

  ui->user_name = strdup("tom");
  ui->num_jobs_queued = 1;

  users.insert(ui,ui->user_name);

  ui = (user_info *)calloc(1,sizeof(user_info));

  ui->user_name = strdup("bob");
  ui->num_jobs_queued = 0;

  users.insert(ui,ui->user_name);

  fail_unless(increment_queued_jobs(&users, (char *)"tom", &pjob) == 0, "can't increment queued jobs");
  fail_unless(increment_queued_jobs(&users, (char *)"bob", &pjob) == 0, "can't increment queued jobs");
  }
END_TEST




START_TEST(decrement_queued_jobs_test)
  {
  users.clear();
  user_info *ui = (user_info *)calloc(1,sizeof(user_info));

  ui->user_name = strdup("tom");
  ui->num_jobs_queued = 1;

  users.insert(ui,ui->user_name);

  fail_unless(decrement_queued_jobs(&users, (char *)"bob") == THING_NOT_FOUND, "decremented for non-existent user");
  fail_unless(decrement_queued_jobs(&users, (char *)"tom") == 0, "couldn't decrement for tom?");
  fail_unless(get_num_queued(&users, (char *)"tom") == 0, "didn't actually decrement tom");

  }
END_TEST




Suite *user_info_suite(void)
  {
  Suite *s = suite_create("user_info test suite methods");
  TCase *tc_core = tcase_create("get_num_queued_test");
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

