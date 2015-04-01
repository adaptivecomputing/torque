#include <stdio.h>
#include <stdlib.h>
#include <check.h>
#include <time.h>
#include "pbs_error.h"
#include "completed_jobs_map.h"

bool job_id_exists_rc = false;


START_TEST(test_one)
  {
  completed_jobs_map_class completed_jobs_map;
  time_t now = time(0);

  // first insert should work fine
  fail_unless(completed_jobs_map.add_job("1234.abc", 0) == true);

  // should be able to find it
  fail_unless(completed_jobs_map.is_job_id_in_map("1234.abc") == true);

  // second insert is a duplicate so should return false
  fail_unless(completed_jobs_map.add_job("1234.abc", 0) == false);

  // delete bogus job should return false
  fail_unless(completed_jobs_map.delete_job("bogus") == false);

  // delete valid job should return true
  fail_unless(completed_jobs_map.delete_job("1234.abc") == true);

  // should not be able to find it
  fail_unless(completed_jobs_map.is_job_id_in_map("1234.abc") == false);

  // insert should work fine
  fail_unless(completed_jobs_map.add_job("12345.abc", now) == true);

  // should be able to find it
  fail_unless(completed_jobs_map.is_job_id_in_map("12345.abc") == true);

  // expect 1 job to have be cleaned
  fail_unless(completed_jobs_map.cleanup_completed_jobs() == PBSE_NONE);

  // should not be able to find it
  fail_unless(completed_jobs_map.is_job_id_in_map("1234.abc") == false);

  // insert should work fine
  fail_unless(completed_jobs_map.add_job("12345.abc", (time_t)(now+10)) == true);
  // cleanup time in future so expect failure
  fail_unless(completed_jobs_map.cleanup_completed_jobs() == PBSE_NONE);

  // insert shold work fine
  fail_unless(completed_jobs_map.add_job("1.abc", (time_t)(now-10)) == true);
  // cleanup time in the past so expect success
  fail_unless(completed_jobs_map.cleanup_completed_jobs() == PBSE_NONE);

  // remove
  fail_unless(completed_jobs_map.delete_job("12345.abc") == true);

  // add in 5 jobs
  fail_unless(completed_jobs_map.add_job("12345.abc", now) == true);
  fail_unless(completed_jobs_map.add_job("22345.abc", now) == true);
  fail_unless(completed_jobs_map.add_job("32345.abc", now) == true);
  fail_unless(completed_jobs_map.add_job("42345.abc", now) == true);
  fail_unless(completed_jobs_map.add_job("52345.abc", now) == true);

  // make sure they all get cleaned
  fail_unless(completed_jobs_map.cleanup_completed_jobs() == PBSE_NONE);

  // make sure they all are gone
  fail_unless(completed_jobs_map.is_job_id_in_map("12345.abc") == false);
  fail_unless(completed_jobs_map.is_job_id_in_map("22345.abc") == false);
  fail_unless(completed_jobs_map.is_job_id_in_map("32345.abc") == false);
  fail_unless(completed_jobs_map.is_job_id_in_map("42345.abc") == false);
  fail_unless(completed_jobs_map.is_job_id_in_map("52345.abc") == false);

  // todo: test concurrency safety?
  }
END_TEST




START_TEST(test_two)
  {
  }
END_TEST






Suite *completed_jobs_map_suite(void)
  {
  Suite *s = suite_create("completed_jobs_map test suite methods");
  TCase *tc_core = tcase_create("test_one");
  tcase_add_test(tc_core, test_one);
  suite_add_tcase(s, tc_core);
  
  tc_core = tcase_create("test_two");
  tcase_add_test(tc_core, test_two);
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
  sr = srunner_create(completed_jobs_map_suite());
  srunner_set_log(sr, "completed_jobs_map_suite.log");
  srunner_run_all(sr, CK_NORMAL);
  number_failed = srunner_ntests_failed(sr);
  srunner_free(sr);
  return(number_failed);
  }
