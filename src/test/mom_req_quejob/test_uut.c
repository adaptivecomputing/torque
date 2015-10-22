#include "license_pbs.h" /* See here for the software license */
#include <stdlib.h>
#include <stdio.h>

#include "batch_request.h"
#include "pbs_error.h"
#include "test_mom_req_quejob.h"

void mom_req_quejob(batch_request *preq);

// sensing variables
extern char prefix[];

START_TEST(test_mom_req_quejob)
  {
  batch_request preq;

  memset(&preq, 0, sizeof(preq));
  preq.rq_fromsvr = TRUE;
  strcpy(preq.rq_ind.rq_queuejob.rq_jid, "1.napali");
  CLEAR_HEAD(preq.rq_ind.rq_queuejob.rq_attr);

  mom_req_quejob(&preq);
  fail_unless(!strcmp("1.napali", prefix));
  }
END_TEST

START_TEST(test_two)
  {


  }
END_TEST

Suite *mom_req_quejob_suite(void)
  {
  Suite *s = suite_create("mom_req_quejob_suite methods");
  TCase *tc_core = tcase_create("test_mom_req_quejob");
  tcase_add_test(tc_core, test_mom_req_quejob);
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
  sr = srunner_create(mom_req_quejob_suite());
  srunner_set_log(sr, "mom_req_quejob_suite.log");
  srunner_run_all(sr, CK_NORMAL);
  number_failed = srunner_ntests_failed(sr);
  srunner_free(sr);
  return number_failed;
  }
