#include "license_pbs.h" /* See here for the software license */
#include <stdio.h>
#include <stdlib.h>
#include <check.h>
#include "pbs_job.h"
#include "pbs_error.h"

START_TEST(initialize_all_jobs_array_test)
  {
  initialize_all_jobs_array(NULL); /* TODO: add check */
  }
END_TEST

START_TEST(swap_jobs_test)
  {
  struct all_jobs alljobs;
  initialize_all_jobs_array(&alljobs);

  struct job *test_job = job_alloc();
  struct job *second_test_job = job_alloc();

  int result = swap_jobs(&alljobs,NULL,test_job);
  fail_unless(result != PBSE_NONE, "NULL first input job fail");

  result = insert_job_after(&alljobs,test_job,NULL);
  fail_unless(result != PBSE_NONE, "NULL second input job fail");

  insert_job(&alljobs, test_job);
  insert_job(&alljobs, second_test_job);
  result = swap_jobs(&alljobs, test_job,second_test_job);
  fail_unless(result == PBSE_NONE, "swap jobs fail");
  }
END_TEST

START_TEST(insert_job_test)
  {
  struct all_jobs alljobs;
  initialize_all_jobs_array(&alljobs);

  struct job *test_job = job_alloc();

  int result = insert_job(NULL, test_job);
  fail_unless(result != PBSE_NONE, "insert into null array fail");

  result = insert_job(&alljobs, NULL);
  fail_unless(result != PBSE_NONE, "NULL job insert fail");

  result = insert_job(&alljobs, test_job);
  fail_unless(result == PBSE_NONE, "job insert fail: %d",result);
  }
END_TEST

START_TEST(insert_job_after_test)
  {
  struct all_jobs alljobs;
  initialize_all_jobs_array(&alljobs);

  struct job *test_job = job_alloc();

  int result = insert_job_after(NULL,test_job,test_job);
  fail_unless(result != PBSE_NONE, "insert into null array fail");

  result = insert_job_after(&alljobs,NULL,test_job);
  fail_unless(result != PBSE_NONE, "NULL job after insert fail");

  result = insert_job_after(&alljobs,test_job,NULL);
  fail_unless(result != PBSE_NONE, "NULL job to insert fail");

  insert_job(&alljobs,test_job);
  result = insert_job_after(&alljobs,test_job,test_job);
  fail_unless(result == PBSE_NONE, "job insert fail");
  }
END_TEST

START_TEST(insert_job_after_index_test)
  {
  struct all_jobs alljobs;
  initialize_all_jobs_array(&alljobs);

  struct job *test_job = job_alloc();

  int result = insert_job_after_index(NULL,0,test_job);
  fail_unless(result != PBSE_NONE, "insert into null array fail");

  result = insert_job_after_index(&alljobs,0,NULL);
  fail_unless(result != PBSE_NONE, "NULL job insert fail");

  result = insert_job_after_index(&alljobs,0,test_job);
  fail_unless(result == PBSE_NONE, "job insert fail");
  }
END_TEST

START_TEST(insert_job_first_test)
  {
  struct all_jobs alljobs;
  initialize_all_jobs_array(&alljobs);

  struct job *test_job = job_alloc();

  int result = insert_job_first(NULL,test_job);
  fail_unless(result != PBSE_NONE, "insert into null array fail");

  result = insert_job_first(&alljobs,NULL);
  fail_unless(result != PBSE_NONE, "NULL job insert fail");

  result = insert_job_first(&alljobs,test_job);
  fail_unless(result == PBSE_NONE, "job insert fail");
  }
END_TEST

START_TEST(get_jobs_index_test)
  {
  struct all_jobs alljobs;
  initialize_all_jobs_array(&alljobs);

  struct job *test_job = job_alloc();

  int result = get_jobs_index(NULL,test_job);
  fail_unless(result == PBSE_BAD_PARAMETER, "null input array fail");

  result = get_jobs_index(&alljobs,NULL);
  fail_unless(result == PBSE_BAD_PARAMETER, "NULL input job fail");

  insert_job(&alljobs, test_job);
  result = get_jobs_index(&alljobs, test_job);
  fail_unless(result == PBSE_NONE, "get_jobs_index fail");
  }
END_TEST

START_TEST(has_job_test)
  {
  struct all_jobs alljobs;
  initialize_all_jobs_array(&alljobs);

  struct job *test_job = job_alloc();

  int result = has_job(NULL,test_job);
  fail_unless(result != PBSE_NONE, "null input array fail");

  result = has_job(&alljobs,NULL);
  fail_unless(result != PBSE_NONE, "NULL input job fail");

  insert_job(&alljobs, test_job);
  result = has_job(&alljobs, test_job);
  fail_unless(result == TRUE, "has_job fail");
  }
END_TEST

START_TEST(remove_job_test)
  {
  struct all_jobs alljobs;
  initialize_all_jobs_array(&alljobs);

  struct job *test_job = job_alloc();

  int result = remove_job(NULL,test_job);
  fail_unless(result != PBSE_NONE, "remove from null array fail");

  result = remove_job(&alljobs,NULL);
  fail_unless(result != PBSE_NONE, "NULL job remove fail");

  insert_job(&alljobs,test_job);
  result = remove_job(&alljobs,test_job);
  fail_unless(result == PBSE_NONE, "job remove fail");
  }
END_TEST

START_TEST(next_job_test)
  {
  struct all_jobs alljobs;
  initialize_all_jobs_array(&alljobs);

  struct job *result = next_job(NULL,NULL);
  fail_unless(result == NULL, "null input parameters fail");

  result = next_job(&alljobs,NULL);
  fail_unless(result == NULL, "NULL input iterator fail");
  }
END_TEST

START_TEST(next_job_from_back_test)
  {
  struct all_jobs alljobs;
  initialize_all_jobs_array(&alljobs);

  struct job *result = next_job_from_back(NULL,NULL);
  fail_unless(result == NULL, "null input parameters fail");

  result = next_job_from_back(&alljobs,NULL);
  fail_unless(result == NULL, "NULL input iterator fail");
  }
END_TEST

Suite *job_container_suite(void)
  {
  Suite *s = suite_create("job_container test suite methods");
  TCase *tc_core = tcase_create("initialize_all_jobs_array_test");
  tcase_add_test(tc_core, initialize_all_jobs_array_test);
  suite_add_tcase(s, tc_core);

  tc_core = tcase_create("swap_jobs_test");
  tcase_add_test(tc_core, swap_jobs_test);
  suite_add_tcase(s, tc_core);

  tc_core = tcase_create("insert_job_test");
  tcase_add_test(tc_core, insert_job_test);
  suite_add_tcase(s, tc_core);

  tc_core = tcase_create("insert_job_after_test");
  tcase_add_test(tc_core, insert_job_after_test);
  suite_add_tcase(s, tc_core);

  tc_core = tcase_create("insert_job_after_index_test");
  tcase_add_test(tc_core, insert_job_after_index_test);
  suite_add_tcase(s, tc_core);

  tc_core = tcase_create("insert_job_first_test");
  tcase_add_test(tc_core, insert_job_first_test);
  suite_add_tcase(s, tc_core);

  tc_core = tcase_create("get_jobs_index_test");
  tcase_add_test(tc_core, get_jobs_index_test);
  suite_add_tcase(s, tc_core);

  tc_core = tcase_create("has_job_test");
  tcase_add_test(tc_core, has_job_test);
  suite_add_tcase(s, tc_core);

  tc_core = tcase_create("remove_job_test");
  tcase_add_test(tc_core, remove_job_test);
  suite_add_tcase(s, tc_core);

  tc_core = tcase_create("next_job_test");
  tcase_add_test(tc_core, next_job_test);
  suite_add_tcase(s, tc_core);

  tc_core = tcase_create("next_job_from_back_test");
  tcase_add_test(tc_core, next_job_from_back_test);
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
  sr = srunner_create(job_container_suite());
  srunner_set_log(sr, "job_container_suite.log");
  srunner_run_all(sr, CK_NORMAL);
  number_failed = srunner_ntests_failed(sr);
  srunner_free(sr);
  return(number_failed);
  }
