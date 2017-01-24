#include "license_pbs.h" /* See here for the software license */
#include <stdio.h>
#include <stdlib.h>
#include "pbs_job.h"
#include "pbs_error.h"
#include <check.h>

const char *get_correct_jobname(const char *jobid, std::string &correct);

void log_err(int,const char *,const char *)
{}


START_TEST(iterator_test)
  {
  all_jobs alljobs;
  struct job *jobs[5];

  for (int i = 0; i < 5; i++)
    {
    jobs[i] = job_alloc();
    sprintf(jobs[i]->ji_qs.ji_jobid, "%d.threnody", i);
    insert_job(&alljobs, jobs[i]);
    }

  // get a reverse iterator
  all_jobs_iterator *iter = alljobs.get_iterator(true);
  for (int i = 4; i >= 0; i--)
    {
    char buf[1024];
    job *ptr = iter->get_next_item();
    sprintf(buf, "%d.threnody", i);
    fail_unless(ptr != NULL);
    fail_unless(!strcmp(ptr->ji_qs.ji_jobid, buf), 
      "Expected %s, but got %s", buf, ptr->ji_qs.ji_jobid);
    }

  // get a forwards iterator
  iter = alljobs.get_iterator();
  for (int i = 0; i < 5; i++)
    {
    char buf[1024];
    job *ptr = iter->get_next_item();
    sprintf(buf, "%d.threnody", i);
    fail_unless(ptr != NULL);
    fail_unless(!strcmp(ptr->ji_qs.ji_jobid, buf), 
      "Expected %s, but got %s", buf, ptr->ji_qs.ji_jobid);
    }
  }
END_TEST


START_TEST(get_correct_jobname_test)
  {
  // with nothing set, get_correct_jobname should just return the jobid passed in.
  std::string correct;
  get_correct_jobname("1.napali.ac", correct);
  fail_unless(correct == "1.napali.ac");
  }
END_TEST

START_TEST(swap_jobs_test)
  {
  all_jobs alljobs;
  struct job *test_job;
  struct job *second_test_job;

  int result;

  test_job = job_alloc();
  second_test_job = job_alloc();
  strcpy(test_job->ji_qs.ji_jobid,"test");
  strcpy(second_test_job->ji_qs.ji_jobid,"second_test");

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
  all_jobs alljobs;
  struct job *test_job;

  int result;

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
  all_jobs alljobs;
  struct job *test_job;

  int result;

  test_job = job_alloc();
  strcpy(test_job->ji_qs.ji_jobid,"mylittlejob");

  result = insert_job_after(NULL,test_job,test_job);
  fail_unless(result != PBSE_NONE, "insert into null array fail");

  result = insert_job_after(&alljobs,(char *)NULL,test_job);
  fail_unless(result != PBSE_NONE, "NULL job after insert fail");

  result = insert_job_after(&alljobs,test_job,NULL);
  fail_unless(result != PBSE_NONE, "NULL job to insert fail");

  insert_job(&alljobs,test_job);
  result = insert_job_after(&alljobs,test_job,test_job);
  fail_unless(result == PBSE_NONE, "job insert fail");
  }
END_TEST

START_TEST(insert_job_first_test)
  {
  all_jobs alljobs;
  struct job *test_job = job_alloc();

  int result;

  result = insert_job_first(NULL,test_job);
  fail_unless(result != PBSE_NONE, "insert into null array fail");

  result = insert_job_first(&alljobs,NULL);
  fail_unless(result != PBSE_NONE, "NULL job insert fail");

  result = insert_job_first(&alljobs,test_job);
  fail_unless(result == PBSE_NONE, "job insert fail");
  }
END_TEST

START_TEST(has_job_test)
  {
  all_jobs alljobs;
  struct job *test_job = job_alloc();
  int result;


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
  all_jobs alljobs;
  int result;
  struct job *test_job = job_alloc();

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
  all_jobs alljobs;
  struct job *result;
  result = next_job(NULL,NULL);

  fail_unless(result == NULL, "null input parameters fail");

  result = next_job(&alljobs,NULL);
  fail_unless(result == NULL, "NULL input iterator fail");

  struct job *test_job1 = job_alloc();
  strcpy(test_job1->ji_qs.ji_jobid, "test_job1");
  int rc = insert_job(&alljobs,test_job1);
  fail_unless(rc == PBSE_NONE, "job insert fail1");

  struct job *test_job2 = job_alloc();
  strcpy(test_job2->ji_qs.ji_jobid, "test_job2");
  rc = insert_job(&alljobs,test_job2);
  fail_unless(rc == PBSE_NONE, "job insert fail2");

  struct job *test_job3 = job_alloc();
  strcpy(test_job3->ji_qs.ji_jobid, "test_job3");
  rc = insert_job(&alljobs,test_job3);
  fail_unless(rc == PBSE_NONE, "job insert fai3");

  struct job *test_job4 = job_alloc();
  strcpy(test_job4->ji_qs.ji_jobid, "test_job4");
  rc = insert_job(&alljobs,test_job4);
  fail_unless(rc == PBSE_NONE, "job insert fail4");

  struct job *test_job5 = job_alloc();
  strcpy(test_job5->ji_qs.ji_jobid, "test_job5");
  rc = insert_job(&alljobs,test_job5);
  fail_unless(rc == PBSE_NONE, "job insert fail5");

  /* first transverse to see if we get all 5 jobs */
  all_jobs_iterator *iter;
  alljobs.lock();
  iter = alljobs.get_iterator();
  alljobs.unlock();

  job *pjob = next_job(&alljobs,iter);
  int jobcount = 0;

  while(pjob != NULL)
    {
    jobcount++;
    pjob = next_job(&alljobs,iter);
    }

  fail_unless(jobcount == 5, "Expected job counts to be 5, but it was %d",
    jobcount);

  all_jobs_iterator *iter2;
  alljobs.lock();
  iter2 = alljobs.get_iterator();
  alljobs.unlock();

  /* simulate another thread had added more jobs to the alljobs */
  struct job *test_job6 = job_alloc();
  strcpy(test_job6->ji_qs.ji_jobid, "test_job6");
  rc = insert_job(&alljobs,test_job6);
  fail_unless(rc == PBSE_NONE, "job insert fail6");

  pjob = next_job(&alljobs,iter2);
  jobcount = 0;

  while(pjob != NULL)
    {
    jobcount++;
    fail_unless(pjob->ji_qs.ji_jobid[0] != (char)254, 
      "get_next returned a deleted job");
    pjob = next_job(&alljobs,iter2);
    }

  fail_unless(jobcount == 6, "Expected job counts to be 6, but it was %d",
    jobcount);
  }
END_TEST

START_TEST(find_job_by_array_with_removed_record_test)
  {
  int result;
  all_jobs alljobs;

  struct job *test_job1 = job_alloc();
  strcpy(test_job1->ji_qs.ji_jobid, "test_job1");
  result = insert_job(&alljobs,test_job1);
  fail_unless(result == PBSE_NONE, "job insert fail1");

  struct job *test_job2 = job_alloc();
  strcpy(test_job2->ji_qs.ji_jobid, "test_job2");
  result = insert_job(&alljobs,test_job2);
  fail_unless(result == PBSE_NONE, "job insert fail2");
  
  struct job *test_job3 = job_alloc();
  strcpy(test_job3->ji_qs.ji_jobid, "test_job3");
  result = insert_job(&alljobs,test_job3);
  fail_unless(result == PBSE_NONE, "job insert fai3");

  struct job *test_job4 = job_alloc();
  strcpy(test_job4->ji_qs.ji_jobid, "test_job4");
  result = insert_job(&alljobs,test_job4);
  fail_unless(result == PBSE_NONE, "job insert fail4");

  struct job *test_job5 = job_alloc();
  strcpy(test_job5->ji_qs.ji_jobid, "test_job5");
  result = insert_job(&alljobs,test_job5);
  fail_unless(result == PBSE_NONE, "job insert fail5");
  }
END_TEST

Suite *job_container_suite(void)
  {
  Suite *s = suite_create("job_container test suite methods");
  TCase *tc_core = tcase_create("swap_jobs_test");
  tcase_add_test(tc_core, swap_jobs_test);
  suite_add_tcase(s, tc_core);

  tc_core = tcase_create("insert_job_test");
  tcase_add_test(tc_core, insert_job_test);
  suite_add_tcase(s, tc_core);

  tc_core = tcase_create("get_correct_jobname_test");
  tcase_add_test(tc_core, get_correct_jobname_test);
  suite_add_tcase(s, tc_core);

  tc_core = tcase_create("insert_job_after_test");
  tcase_add_test(tc_core, insert_job_after_test);
  suite_add_tcase(s, tc_core);

  tc_core = tcase_create("insert_job_first_test");
  tcase_add_test(tc_core, insert_job_first_test);
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

  tc_core = tcase_create("find_job_by_array_with_removed_record");
  tcase_add_test(tc_core, find_job_by_array_with_removed_record_test);
  tcase_add_test(tc_core, iterator_test);
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
