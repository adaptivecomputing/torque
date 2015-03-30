#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <check.h>
#include "license_pbs.h" /* See here for the software license */
#include "get_path_jobdata.h"
#include "pbs_error.h"

extern int global_is_svr_attr_set_rc;

START_TEST(test_get_path_jobdata)
  {
  global_is_svr_attr_set_rc = FALSE;

  fail_unless(get_path_jobdata(NULL, NULL) == "");
  fail_unless(get_path_jobdata("12345", "/foo/") == "/foo/");

  global_is_svr_attr_set_rc = TRUE;
  fail_unless(get_path_jobdata("12345", "/foo/") == "/foo/5/");
  fail_unless(get_path_jobdata("", "/foo/") == "/foo/");
  fail_unless(get_path_jobdata("a", "/foo/") == "/foo/");
  fail_unless(get_path_jobdata("9", "/foo/") == "/foo/9/");
  fail_unless(get_path_jobdata("127363.server", "/foo/") == "/foo/3/");
  fail_unless(get_path_jobdata("127361[].server", "/foo/") == "/foo/1/");
  }
END_TEST


START_TEST(test_two)
  {
  }
END_TEST




Suite *get_path_jobdata_suite(void)
  {
  Suite *s = suite_create("get_path_jobdata test suite methods");
  TCase *tc_core = tcase_create("test_get_path_jobdata");
  tcase_add_test(tc_core, test_get_path_jobdata);
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
  sr = srunner_create(get_path_jobdata_suite());
  srunner_set_log(sr, "get_path_jobdata_suite.log");
  srunner_run_all(sr, CK_NORMAL);
  number_failed = srunner_ntests_failed(sr);
  srunner_free(sr);
  return(number_failed);
  }
