#include "license_pbs.h" /* See here for the software license */
#include <stdio.h>
#include <stdlib.h>
#include <check.h>
#include "pbs_job.h"
#include "pbs_error.h"

extern resizable_array *initialize_resizable_array(int size);
extern void traverse_all_jobs(void (*traverseCallback)(const char *pJobID,void *callbackParameter),void *callbackParameter);
char *get_correct_jobname(const char *jobid);


START_TEST(get_correct_jobname_test)
  {
  // with nothing set, get_correct_jobname should just return the jobid passed in.
  char *jobid = get_correct_jobname("1.napali.ac");
  fail_unless(!strcmp(jobid, "1.napali.ac"));
  }
END_TEST

START_TEST(initialize_all_jobs_array_test)
  {
  initialize_all_jobs_array(NULL); /* TODO: add check */
  }
END_TEST

START_TEST(swap_jobs_test)
  {
  struct all_jobs alljobs;
  struct job *test_job;
  struct job *second_test_job;

  int result;
  initialize_all_jobs_array(&alljobs);

  test_job = job_alloc();
  second_test_job = job_alloc();

  result = swap_jobs(&alljobs,NULL,test_job);
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
  struct job *test_job;

  int result;
  initialize_all_jobs_array(&alljobs);

  test_job = job_alloc();

  result = insert_job(NULL, test_job);
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
  struct job *test_job;

  int result;
  initialize_all_jobs_array(&alljobs);

  test_job = job_alloc();

  result = insert_job_after(NULL,test_job,test_job);
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
  struct job *test_job;

  int result;
  initialize_all_jobs_array(&alljobs);

  test_job = job_alloc();

  result = insert_job_after_index(NULL,0,test_job);
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
  struct job *test_job = job_alloc();

  int result;
  initialize_all_jobs_array(&alljobs);

  result = insert_job_first(NULL,test_job);
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
  struct job *test_job = job_alloc();
  int result;
  initialize_all_jobs_array(&alljobs);

  result = get_jobs_index(NULL,test_job);
  fail_unless(result == -1 * PBSE_BAD_PARAMETER, "null input array fail");

  result = get_jobs_index(&alljobs,NULL);
  fail_unless(result == -1 * PBSE_BAD_PARAMETER, "NULL input job fail");

  insert_job(&alljobs, test_job);
  result = get_jobs_index(&alljobs, test_job);
  fail_unless(result == PBSE_NONE, "get_jobs_index fail");
  }
END_TEST

START_TEST(has_job_test)
  {
  struct all_jobs alljobs;
  struct job *test_job = job_alloc();
  int result;
  initialize_all_jobs_array(&alljobs);


  result = has_job(NULL,test_job);
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
  int result;
  struct job *test_job = job_alloc();
  initialize_all_jobs_array(&alljobs);

  result = remove_job(NULL,test_job);
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
  struct job *result;
  initialize_all_jobs_array(&alljobs);
  result = next_job(NULL,NULL);

  fail_unless(result == NULL, "null input parameters fail");

  result = next_job(&alljobs,NULL);
  fail_unless(result == NULL, "NULL input iterator fail");
  }
END_TEST

START_TEST(next_job_from_back_test)
  {
  struct all_jobs alljobs;
  struct job *result;
  initialize_all_jobs_array(&alljobs);

  result = next_job_from_back(NULL,NULL);
  fail_unless(result == NULL, "null input parameters fail");

  result = next_job_from_back(&alljobs,NULL);
  fail_unless(result == NULL, "NULL input iterator fail");
  }
END_TEST

void null_found_job(const char *pJobID,void *jobsParm)
  {
  job **jobs = (job **)jobsParm;

  for(int i = 0;i < 4;i++)
    {
    if((jobs[i] != NULL)&&(strcmp(jobs[i]->ji_qs.ji_jobid,pJobID) == 0))
      {
      jobs[i] = NULL;
      return;
      }
    }
  fprintf(stderr,"Job not found.\n");
  exit(1);
  }

START_TEST(traverse_all_jobs_test)
  {
  extern struct all_jobs alljobs;

  struct job *test_job[4];

  initialize_all_jobs_array(&alljobs);
  resizable_array *ar = initialize_resizable_array(10);

  for(int i = 0;i < 4;i++)
    {
    test_job[i] = job_alloc();
    sprintf(test_job[i]->ji_qs.ji_jobid,"Job_%d",i);
    insert_thing(ar,(void *)test_job[i]);
    //ar->slots[i].item = (void *)test_job[i];
    //ar->max++;
    }
  alljobs.ra = ar;

  traverse_all_jobs(null_found_job,test_job);

  for(int i = 0;i < 4;i++)
    {
    fail_unless((test_job[i] == NULL),"Job not found.");
    }
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
  tcase_add_test(tc_core, get_correct_jobname_test);
  suite_add_tcase(s, tc_core);
  
  tc_core = tcase_create("traverse_all_jobs_test");
  tcase_add_test(tc_core, traverse_all_jobs_test);
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
