#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

#include "user_info.h"
#include <check.h>

unsigned int get_num_queued(user_info_holder *, const char *);
unsigned int count_jobs_submitted(job *);


START_TEST(remove_server_suffix_test)
  {
  std::string u1("dbeer@napali");
  std::string u2("dbeer");
  std::string u3("dbeer@waimea");

  remove_server_suffix(u1);
  remove_server_suffix(u2);
  remove_server_suffix(u3);

  fail_unless(!strcmp(u1.c_str(), "dbeer"));
  fail_unless(!strcmp(u2.c_str(), "dbeer"));
  fail_unless(!strcmp(u3.c_str(), "dbeer"));
  }
END_TEST



START_TEST(get_num_queued_test)
  {
  unsigned int queued;
  users.lock();
  users.clear();
  users.unlock();
  user_info ui;

  ui.user_name = (char *)"tom";
  ui.num_jobs_queued = 1;

  users.lock();
  users.insert(&ui,ui.user_name);
  users.unlock();

  queued = get_num_queued(&users, "bob");
  fail_unless(queued == 0, "incorrect queued count for bob");

  queued = get_num_queued(&users, "tom");
  fail_unless(queued == 1, "incorrect queued count for tom");
  }
END_TEST




START_TEST(count_jobs_submitted_test)
  {
  unsigned int submitted;
  job          pjob;

  memset(&pjob, 0, sizeof(pjob));
  users.lock();
  users.clear();
  users.unlock();

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
  users.lock();
  users.clear();

  user_info *ui = (user_info *)calloc(1,sizeof(user_info));

  ui->user_name = strdup("tom");
  ui->num_jobs_queued = 1;

  users.insert(ui,ui->user_name);

  ui = (user_info *)calloc(1,sizeof(user_info));

  ui->user_name = strdup("bob");
  ui->num_jobs_queued = 0;

  users.insert(ui,ui->user_name);
  users.unlock();


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
  users.lock();
  users.clear();

  user_info *ui = (user_info *)calloc(1,sizeof(user_info));

  ui->user_name = strdup("tom");
  ui->num_jobs_queued = 1;

  users.insert(ui,ui->user_name);

  ui = (user_info *)calloc(1,sizeof(user_info));

  ui->user_name = strdup("bob");
  ui->num_jobs_queued = 0;

  users.insert(ui,ui->user_name);
  users.unlock();

  fail_unless(increment_queued_jobs(&users, (char *)"tom", &pjob) == 0, "can't increment queued jobs");
  pjob.ji_queue_counted = 0;
  fail_unless(increment_queued_jobs(&users, (char *)"bob", &pjob) == 0, "can't increment queued jobs");
  pjob.ji_queue_counted = 0;
  // after 1 increment the count should be 2 because initialize_user_info() starts out with tom at 
  // 1 instead of 0, as a normal program would start. Its done this way for the decrement code.
  fail_unless(get_num_queued(&users, "tom") == 2, "didn't actually increment tom 1");
  fail_unless(increment_queued_jobs(&users, strdup("tom@napali"), &pjob) == 0);
  fail_unless(get_num_queued(&users, "tom") == 3, "didn't actually increment tom 2");

  // Enqueue the job without resetting to make sure the count doesn't change
  fail_unless(increment_queued_jobs(&users, strdup("tom@napali"), &pjob) == 0);
  fail_unless(get_num_queued(&users, "tom") == 3, "Shouldn't have incremented with the bit set");

  // Make sure array subjobs are a no-op
  pjob.ji_queue_counted = 0;
  pjob.ji_is_array_template = FALSE;
  pjob.ji_wattr[JOB_ATR_job_array_id].at_flags = ATR_VFLAG_SET;
  fail_unless(increment_queued_jobs(&users, strdup("tom"), &pjob) == 0);
  fail_unless(get_num_queued(&users, "tom") == 3, "Shouldn't have incremented for an array sub-job");
  fail_unless(pjob.ji_queue_counted != 0);
  
  pjob.ji_queue_counted = 0;
  pjob.ji_wattr[JOB_ATR_job_array_id].at_flags = 0;
  fail_unless(increment_queued_jobs(&users, strdup("tom"), &pjob) == 0);
  fail_unless(get_num_queued(&users, "tom") == 4, "Should have incremented for non array sub-job");
  }
END_TEST



START_TEST(decrement_queued_jobs_test)
  {
  users.lock();
  users.clear();
  user_info *ui = (user_info *)calloc(1,sizeof(user_info));

  ui->user_name = strdup("tom"); 
  ui->num_jobs_queued = 1;
  job pjob;
  memset(&pjob, 0, sizeof(pjob));

  users.insert(ui,ui->user_name);
  users.unlock();

  pjob.ji_queue_counted = COUNTED_GLOBALLY;
  fail_unless(decrement_queued_jobs(&users, (char *)"bob", &pjob) == THING_NOT_FOUND, "decremented for non-existent user");
  pjob.ji_queue_counted = COUNTED_GLOBALLY;
  fail_unless(decrement_queued_jobs(&users, (char *)"tom@neverland.com", &pjob) == 0, "couldn't decrement for tom?");

  fail_unless(get_num_queued(&users, "tom") == 0, "didn't actually decrement tom");
  pjob.ji_queue_counted = COUNTED_GLOBALLY;
  
  fail_unless(decrement_queued_jobs(&users, (char *)"tom", &pjob) == 0,
    "couldn't decrement for tom?");
  fail_unless(get_num_queued(&users, "tom") == 0, "Disallow going negative in the count");

  // Make sure we are clearing and resetting the bits  
  fail_unless(increment_queued_jobs(&users, strdup("tom@napali"), &pjob) == 0);
  fail_unless(get_num_queued(&users, "tom") == 1);
  fail_unless(decrement_queued_jobs(&users, (char *)"tom", &pjob) == 0);
  fail_unless(get_num_queued(&users, "tom") == 0);

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
  tcase_add_test(tc_core, remove_server_suffix_test);
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

