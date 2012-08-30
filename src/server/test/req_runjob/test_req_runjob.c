#include "license_pbs.h" /* See here for the software license */
#include "req_runjob.h"
#include "test_req_runjob.h"
#include <stdlib.h>
#include <stdio.h>
#include "pbs_error.h"
#include "pbs_job.h"



int requeue_job(job *pjob);



START_TEST(requeue_job_test)
  {
  job pjob;

  memset(&pjob, 0, sizeof(pjob));

  strcpy(pjob.ji_qs.ji_destin, "tom");

  fail_unless(requeue_job(&pjob) == -1, "didn't fail with non-existent node");

  strcpy(pjob.ji_qs.ji_destin, "bob");
  fail_unless(requeue_job(&pjob) == 0, "call failed");
  fail_unless(pjob.ji_qs.ji_destin[0] == '\0', "destination is still set");
  fail_unless(pjob.ji_qs.ji_state == JOB_STATE_QUEUED, "job not set to queued");
  fail_unless(pjob.ji_qs.ji_substate == JOB_SUBSTATE_QUEUED, "job not set to queued");
  }
END_TEST




START_TEST(test_two)
  {


  }
END_TEST




Suite *req_runjob_suite(void)
  {
  Suite *s = suite_create("req_runjob_suite methods");
  TCase *tc_core = tcase_create("requeue_job_test");
  tcase_add_test(tc_core, requeue_job_test);
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
  sr = srunner_create(req_runjob_suite());
  srunner_set_log(sr, "req_runjob_suite.log");
  srunner_run_all(sr, CK_NORMAL);
  number_failed = srunner_ntests_failed(sr);
  srunner_free(sr);
  return number_failed;
  }
