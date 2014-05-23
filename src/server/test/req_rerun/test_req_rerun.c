#include "license_pbs.h" /* See here for the software license */
#include "req_rerun.h"
#include "test_req_rerun.h"
#include <stdlib.h>
#include <stdio.h>
#include "pbs_error.h"
#include "pbs_job.h"
#include "batch_request.h"

void requeue_job_without_contacting_mom(job &pjob);
int  handle_requeue_all(batch_request *preq);

START_TEST(test_requeue_job_without_contacting_mom)
  {
  job pjob;

  memset(&pjob, 0, sizeof(pjob));
  pjob.ji_wattr[JOB_ATR_exec_host].at_val.at_str = strdup("napali/0-5");
  pjob.ji_wattr[JOB_ATR_exec_host].at_flags = ATR_VFLAG_SET;

  // job isn't running so nothing should happen
  requeue_job_without_contacting_mom(pjob);
  fail_unless(pjob.ji_wattr[JOB_ATR_exec_host].at_val.at_str != NULL);
  fail_unless(pjob.ji_qs.ji_state == 0);

  pjob.ji_qs.ji_state = JOB_STATE_RUNNING;
  requeue_job_without_contacting_mom(pjob);
  fail_unless(pjob.ji_wattr[JOB_ATR_exec_host].at_val.at_str == NULL);
  fail_unless(pjob.ji_wattr[JOB_ATR_exec_host].at_flags == 0);
  fail_unless(pjob.ji_qs.ji_state == JOB_STATE_QUEUED);
  }
END_TEST

START_TEST(test_handle_requeue_all)
  {
  batch_request preq;

  memset(&preq, 0, sizeof(preq));

  // fail due to lack of permissions
  fail_unless(handle_requeue_all(&preq) == PBSE_PERM);

  preq.rq_perm |= ATR_DFLAG_MGWR;
  fail_unless(handle_requeue_all(&preq) == PBSE_NONE);
  }
END_TEST

Suite *req_rerun_suite(void)
  {
  Suite *s = suite_create("req_rerun_suite methods");
  TCase *tc_core = tcase_create("test_requeue_job_without_contacting_mom");
  tcase_add_test(tc_core, test_requeue_job_without_contacting_mom);
  suite_add_tcase(s, tc_core);

  tc_core = tcase_create("test_handle_requeue_all");
  tcase_add_test(tc_core, test_handle_requeue_all);
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
  sr = srunner_create(req_rerun_suite());
  srunner_set_log(sr, "req_rerun_suite.log");
  srunner_run_all(sr, CK_NORMAL);
  number_failed = srunner_ntests_failed(sr);
  srunner_free(sr);
  return number_failed;
  }
