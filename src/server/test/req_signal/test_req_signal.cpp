#include "license_pbs.h" /* See here for the software license */
#include "req_signal.h"
#include "test_req_signal.h"
#include <stdlib.h>
#include <stdio.h>
#include "pbs_error.h"
extern char scaff_buffer[];

START_TEST(test_req_signaljob_relaying_msg)
  {
  struct batch_request request;
  job myjob;
  memset(&request, 0, sizeof(struct batch_request));
  memset(&myjob, 0, sizeof(job));
  myjob.ji_qs.ji_state = JOB_STATE_RUNNING;
  myjob.ji_qs.ji_un.ji_exect.ji_momaddr = 167838724;
  snprintf(request.rq_ind.rq_signal.rq_jid, PBS_MAXSVRJOBID, "%lu", (unsigned long)&myjob);
  memset(scaff_buffer, 0, 1024);
  req_signaljob(&request);
  fail_unless(strcmp("relaying signal request to mom 10.1.4.4",
    scaff_buffer) == 0, "Error message was not constructed as expected");
  }
END_TEST

START_TEST(test_two)
  {


  }
END_TEST

Suite *req_signal_suite(void)
  {
  Suite *s = suite_create("req_signal_suite methods");
  TCase *tc_core = tcase_create("test_req_signaljob_relaying_msg");
  tcase_add_test(tc_core, test_req_signaljob_relaying_msg);
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
  sr = srunner_create(req_signal_suite());
  srunner_set_log(sr, "req_signal_suite.log");
  srunner_run_all(sr, CK_NORMAL);
  number_failed = srunner_ntests_failed(sr);
  srunner_free(sr);
  return number_failed;
  }
