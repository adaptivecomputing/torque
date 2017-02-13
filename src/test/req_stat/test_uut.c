#include "license_pbs.h" /* See here for the software license */
#include "req_stat.h"
#include "test_req_stat.h"
#include <stdlib.h>
#include <stdio.h>
#include "pbs_error.h"
#include "array.h"

bool in_execution_queue(job *pjob, job_array *pa);
//int stat_to_mom(const char*, struct stat_cntl*);

job *get_next_status_job(struct stat_cntl *cntl, int &job_array_index, job_array *pa, all_jobs_iterator *iter);
extern int abort_called;
extern bool svr_find_job_returns_null;

enum TJobStatTypeEnum
  {
  tjstNONE = 0,
  tjstJob,
  tjstQueue,
  tjstServer,
  tjstTruncatedQueue,
  tjstTruncatedServer,
  tjstSummarizeArraysServer,
  tjstSummarizeArraysQueue,
  tjstArray,
  tjstLAST
  };

START_TEST(test_in_execution_queue)
  {
  job       *pjob = (job *)calloc(1, sizeof(job));
  job_array *pa = (job_array *)calloc(1, sizeof(job_array));
  pbs_queue *pque = (pbs_queue *)calloc(1, sizeof(pbs_queue));
  pque->qu_mutex = (pthread_mutex_t *)calloc(1, sizeof(pthread_mutex_t));

  fail_unless(in_execution_queue(NULL, NULL) == false);
  fail_unless(in_execution_queue(pjob, NULL) == false);
  fail_unless(in_execution_queue(pjob, pa) == false);

  pque->qu_qs.qu_type = QTYPE_Execution;
  pjob->ji_qhdr = pque;
  fail_unless(in_execution_queue(pjob, pa) == true);
  }
END_TEST


START_TEST(test_stat_update)
  {
  batch_request preq;
  stat_cntl     cntl;

  memset(&preq, 0, sizeof(preq));
  memset(&cntl, 0, sizeof(cntl));
  abort_called = 0;

  preq.rq_reply.brp_choice = BATCH_REPLY_CHOICE_Queue;
  preq.rq_reply.brp_un.brp_txt.brp_str = strdup("MSG=1.napali");

  // Make sure that the job isn't aborted.
  stat_update(&preq, &cntl);
  fail_unless(abort_called == 0);
  }
END_TEST


START_TEST(test_get_next_status_job)
  {
  struct stat_cntl cntl;
  int              array_index = -1;
  pbs_queue        pque;

  job_array *pa = (job_array *)calloc(1, sizeof(job_array));
  pa->ai_qs.array_size = 2;
  pa->job_ids = (char **)calloc(2, sizeof(char *));
  pa->job_ids[0] = strdup("1[0].napali");
  pa->job_ids[1] = strdup("1[1].napali");

  // next job is currently set to return NULL every time, so all of these are NULL
  cntl.sc_type = tjstQueue;
  cntl.sc_pque = &pque;
  fail_unless(get_next_status_job(&cntl, array_index, pa, NULL) == NULL);

  cntl.sc_type = tjstSummarizeArraysQueue;
  fail_unless(get_next_status_job(&cntl, array_index, pa, NULL) == NULL);

  cntl.sc_type = tjstSummarizeArraysServer;
  fail_unless(get_next_status_job(&cntl, array_index, pa, NULL) == NULL);

  cntl.sc_type = tjstServer;
  fail_unless(get_next_status_job(&cntl, array_index, pa, NULL) == NULL);

  // these should grab the jobs from the array
  cntl.sc_type = tjstArray;
  job *pjob = get_next_status_job(&cntl, array_index, pa, NULL);
  fail_unless(pjob != NULL);
  fail_unless(array_index == 0);
  fail_unless(!strcmp(pjob->ji_qs.ji_jobid, "1[0].napali"));
  
  pjob = get_next_status_job(&cntl, array_index, pa, NULL);
  fail_unless(pjob != NULL);
  fail_unless(array_index == 1);
  fail_unless(!strcmp(pjob->ji_qs.ji_jobid, "1[1].napali"));
  
  // we are now past the number of jobs in the array, this should return NULL
  pjob = get_next_status_job(&cntl, array_index, pa, NULL);
  fail_unless(pjob == NULL);
  fail_unless(array_index == 2);
  }
END_TEST


START_TEST(test_stat_to_mom)
  {
  struct stat_cntl *scp;

  scp = (struct stat_cntl *)calloc(1, sizeof(struct stat_cntl));

  svr_find_job_returns_null = true;
  fail_unless(stat_to_mom(strdup("12345"), scp) == PBSE_JOBNOTFOUND);

  svr_find_job_returns_null = false;
  fail_unless(stat_to_mom(strdup("12345"), scp) == PBSE_BADSTATE);
  }
END_TEST


Suite *req_stat_suite(void)
  {
  Suite *s = suite_create("req_stat_suite methods");
  TCase *tc_core = tcase_create("test_in_execution_queue");
  tcase_add_test(tc_core, test_in_execution_queue);
  tcase_add_test(tc_core, test_stat_update);
  suite_add_tcase(s, tc_core);

  tc_core = tcase_create("test_get_next_status_job");
  tcase_add_test(tc_core, test_get_next_status_job);
  suite_add_tcase(s, tc_core);

  tc_core = tcase_create("test_stat_to_mom");
  tcase_add_test(tc_core, test_stat_to_mom);
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
  sr = srunner_create(req_stat_suite());
  srunner_set_log(sr, "req_stat_suite.log");
  srunner_run_all(sr, CK_NORMAL);
  number_failed = srunner_ntests_failed(sr);
  srunner_free(sr);
  return number_failed;
  }
