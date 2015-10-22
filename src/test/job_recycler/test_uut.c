#include "license_pbs.h" /* See here for the software license */
#include <stdlib.h>
#include <stdio.h>
#include "pbs_error.h"
#include "pbs_job.h"
#include "test_job_recycler.h"

job_recycler recycler;
bool exit_called = false;
extern pthread_t     open_threads[];
extern int           open_thread_count;

job *pop_job_from_recycler(all_jobs *aj);
void *remove_some_recycle_jobs(void *vp);


START_TEST(test_insert_into_recycler)
  {
  job *pj = job_alloc();
  initialize_recycler();

  fail_unless(insert_into_recycler(pj) == PBSE_NONE);

  fail_unless(recycler.rc_jobs.count() == 1);

  job *pj2 = job_alloc();
  fail_unless(insert_into_recycler(pj2) == PBSE_NONE);

  fail_unless(recycler.rc_jobs.count() == 2);

  // make sure we can't insert the job again with or without the flag set
  pj->ji_being_recycled = 0;
  fail_unless(insert_into_recycler(pj) != PBSE_NONE);
  fail_unless(recycler.rc_jobs.count() == 2);

  fail_unless(insert_into_recycler(pj) == PBSE_NONE);

  fail_unless(recycler.rc_jobs.count() == 2);
  }
END_TEST


START_TEST(test_remove_some_recycle_jobs)
  {
  job *pjobs[1000];
  initialize_recycler();

  for (int i = 0; i < 1000; i++)
    {
    pjobs[i] = job_alloc();
    fail_unless(insert_into_recycler(pjobs[i]) == PBSE_NONE);

    // make the first 700 get removed 
    if (i < 700)
      pjobs[i]->ji_momstat = 0;
    }

  pthread_t t1;
  pthread_t t2;
  pthread_t t3;

  pthread_create(&t1, NULL, remove_some_recycle_jobs, NULL);
  pthread_create(&t2, NULL, remove_some_recycle_jobs, NULL);
  pthread_create(&t3, NULL, remove_some_recycle_jobs, NULL);

  pthread_join(t1, NULL);
  pthread_join(t2, NULL);
  pthread_join(t3, NULL);

  // 300 should be left
  fail_unless(recycler.rc_jobs.count() == 300);
  }
END_TEST


START_TEST(test_pop_job_from_recycler)
  {
  job *pjobs[10];
  initialize_recycler();

  while (recycler.rc_jobs.count() > 0)
    pop_job_from_recycler(&recycler.rc_jobs);

  for (int i = 0; i < 10; i++)
    {
    pjobs[i] = job_alloc();
    fail_unless(insert_into_recycler(pjobs[i]) == PBSE_NONE);
    }

  for (unsigned int i = 0; i < 10; i++)
    {
    job *pjob = pop_job_from_recycler(&recycler.rc_jobs);
    fail_unless(pjob == pjobs[i]);
    fail_unless(recycler.rc_jobs.count() == 9 - i);
    }

  for (int i = 0; i < 3; i++)
    fail_unless(pop_job_from_recycler(&recycler.rc_jobs) == NULL);

  // test for records already freed or that it has not been recycled
  for (int i = 0; i < 5; i++)
    {
    pjobs[i] = job_alloc();
    fail_unless(insert_into_recycler(pjobs[i]) == PBSE_NONE);
    }

  job *pjob = pop_job_from_recycler(&recycler.rc_jobs);
  int count = 0;
  char buf[80];
  while(pjob)
    {
    count++;
    snprintf(buf,sizeof(buf),"%016lx",(long)pjob);
    fail_unless(strcmp(pjob->ji_qs.ji_jobid, buf)==0, pjob->ji_qs.ji_jobid);
    pjob = pop_job_from_recycler(&recycler.rc_jobs);
    }

  fail_unless(count == 5, "count of valid recycled jobs were wrong: %d", count);
  }
END_TEST


Suite *job_recycler_suite(void)
  {
  Suite *s = suite_create("job_recycler_suite methods");
  TCase *tc_core = tcase_create("test_insert_into_recycler");
  tcase_add_test(tc_core, test_insert_into_recycler);
  suite_add_tcase(s, tc_core);

  tc_core = tcase_create("test_pop_job_from_recycler");
  tcase_add_test(tc_core, test_pop_job_from_recycler);
  tcase_add_test(tc_core, test_remove_some_recycle_jobs);
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
  sr = srunner_create(job_recycler_suite());
  srunner_set_log(sr, "job_recycler_suite.log");
  srunner_run_all(sr, CK_NORMAL);
  number_failed = srunner_ntests_failed(sr);
  srunner_free(sr);
  return number_failed;
  }
