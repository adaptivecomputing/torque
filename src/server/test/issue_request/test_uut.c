#include "license_pbs.h" /* See here for the software license */
#include "issue_request.h"
#include "test_uut.h"
#include <stdlib.h>
#include <stdio.h>
#include "pbs_error.h"
#include "attribute.h"

START_TEST(test_one)
  {
  job testJob;
  job *pTestJob;
  struct batch_request request;

  memset(&testJob,0,sizeof(job));
  memset(&request,0,sizeof(request));
  pTestJob = &testJob;

  fail_unless(relay_to_mom(&pTestJob,&request,NULL) == PBSE_BADSTATE);

  decode_str(&testJob.ji_wattr[JOB_ATR_exec_host],NULL,NULL,"Sherrie",0);

  fail_unless(relay_to_mom(&pTestJob,&request,NULL) == PBSE_NONE);

  }
END_TEST

START_TEST(test_two)
  {


  }
END_TEST

Suite *issue_request_suite(void)
  {
  Suite *s = suite_create("issue_request_suite methods");
  TCase *tc_core = tcase_create("test_one");
  tcase_add_test(tc_core, test_one);
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
  sr = srunner_create(issue_request_suite());
  srunner_set_log(sr, "issue_request_suite.log");
  srunner_run_all(sr, CK_NORMAL);
  number_failed = srunner_ntests_failed(sr);
  srunner_free(sr);
  return number_failed;
  }
