#include <stdio.h>
#include <stdlib.h>
#include "batch_request.h"
#include <check.h>


START_TEST(test_one)
  {

  batch_request *pBr1 = (batch_request *)calloc(1,sizeof(batch_request));
  batch_request *pBr2 = (batch_request *)calloc(1,sizeof(batch_request));
  batch_request *pBr3 = (batch_request *)calloc(1,sizeof(batch_request));
  batch_request *pBr4 = (batch_request *)calloc(1,sizeof(batch_request));

  int rc = get_batch_request_id(pBr1);
  fail_unless(rc == PBSE_NONE);
  rc = get_batch_request_id(pBr2);
  fail_unless(rc == PBSE_NONE);
  rc = get_batch_request_id(pBr3);
  fail_unless(rc == PBSE_NONE);
  rc = get_batch_request_id(pBr4);
  fail_unless(rc == PBSE_NONE);

  batch_request *tmp = get_batch_request((char *)"0");
  fail_unless(tmp == pBr1);
  tmp = get_remove_batch_request((char *)"0");
  fail_unless(tmp == pBr1);
  tmp = get_batch_request((char *)"0");
  fail_unless(tmp == NULL);

  tmp = get_batch_request((char *)"1");
  fail_unless(tmp == pBr2);
  rc = remove_batch_request((char *)"1");
  fail_unless(rc == PBSE_NONE);
  tmp = get_batch_request((char *)"1");
  fail_unless(tmp == NULL);



  }
END_TEST





START_TEST(test_two)
  {
  }
END_TEST




Suite *batch_request_suite(void)
  {
  Suite *s = suite_create("batch_request suite methods");

  TCase *tc_core = tcase_create("test_one");
  tcase_add_test(tc_core, test_one);
  suite_add_tcase(s, tc_core);

  tc_core = tcase_create("test_two");
  tcase_add_test(tc_core, test_two);
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
  sr = srunner_create(batch_request_suite());
  srunner_set_log(sr, "batch_request_suite.log");
  srunner_run_all(sr, CK_NORMAL);
  number_failed = srunner_ntests_failed(sr);
  srunner_free(sr);
  return(number_failed);
  }
