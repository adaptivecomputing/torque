#include "license_pbs.h" /* See here for the software license */
#include "req_holdjob.h"
#include "test_req_holdjob.h"
#include <stdlib.h>
#include <stdio.h>
#include "pbs_error.h"

int release_job(batch_request *preq, void *j, job_array *pa);

START_TEST(test_release_job)
  {
  job           *pjob = (job *)calloc(1, sizeof(job));
  batch_request  preq;

  pjob->ji_qs.ji_state = JOB_STATE_EXITING;
  fail_unless(release_job(&preq, pjob, NULL) == PBSE_NONE);
  fail_unless(pjob->ji_qs.ji_state == JOB_STATE_EXITING);

  pjob->ji_qs.ji_state = JOB_STATE_COMPLETE;
  fail_unless(release_job(&preq, pjob, NULL) == PBSE_NONE);
  fail_unless(pjob->ji_qs.ji_state == JOB_STATE_COMPLETE);
  }
END_TEST

START_TEST(test_two)
  {


  }
END_TEST

Suite *req_holdjob_suite(void)
  {
  Suite *s = suite_create("req_holdjob_suite methods");
  TCase *tc_core = tcase_create("test_release_job");
  tcase_add_test(tc_core, test_release_job);
  suite_add_tcase(s, tc_core);

  tc_core = tcase_create("test_two");
  tcase_add_test(tc_core, test_two);
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
  sr = srunner_create(req_holdjob_suite());
  srunner_set_log(sr, "req_holdjob_suite.log");
  srunner_run_all(sr, CK_NORMAL);
  number_failed = srunner_ntests_failed(sr);
  srunner_free(sr);
  return number_failed;
  }
