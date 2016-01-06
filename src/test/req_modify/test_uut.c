#include "license_pbs.h" /* See here for the software license */
#include "req_modify.h"
#include "test_req_modify.h"
#include <stdlib.h>
#include <stdio.h>
#include "pbs_error.h"

batch_request *preq;

extern char *get_correct_jobname_return;

START_TEST(test_req_modifyjob)
  {
  // initialize
  get_correct_jobname_return = NULL;

  // create space for the req
  preq = (batch_request *)calloc(1, sizeof(batch_request));
  fail_unless(preq != NULL);

  // load a full job name into the req name
  snprintf(preq->rq_ind.rq_modify.rq_objname, sizeof(preq->rq_ind.rq_modify.rq_objname),
     "%s", "123.servername");

  // call the function to unit test
  req_modifyjob(preq);

  // the request objname should be unchanged
  fail_unless(0 == strncmp(preq->rq_ind.rq_modify.rq_objname, "123.servername",
    sizeof(preq->rq_ind.rq_modify.rq_objname)));

  // set up get_correct_jobname() to return a different value
  get_correct_jobname_return = strdup("123");

  // call the function to unit test
  req_modifyjob(preq);

  // make sure the request objname was changed to the forced value
  fail_unless(!strcmp(preq->rq_ind.rq_modify.rq_objname, "123"));
  }
END_TEST

START_TEST(test_two)
  {


  }
END_TEST

Suite *req_modify_suite(void)
  {
  Suite *s = suite_create("req_modify_suite methods");
  TCase *tc_core = tcase_create("test_req_modifyjob");
  tcase_add_test(tc_core, test_req_modifyjob);
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
  sr = srunner_create(req_modify_suite());
  srunner_set_log(sr, "req_modify_suite.log");
  srunner_run_all(sr, CK_NORMAL);
  number_failed = srunner_ntests_failed(sr);
  srunner_free(sr);
  return number_failed;
  }
