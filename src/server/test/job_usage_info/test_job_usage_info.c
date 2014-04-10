#include <stdio.h>
#include <stdlib.h>
#include <check.h>

#include "job_usage_info.hpp"


START_TEST(test_constructor)
  {
  job_usage_info jui1(1);
  job_usage_info jui2(2);
  job_usage_info jui_copy(1);

  fail_unless(jui1.internal_job_id == 1);
  fail_unless(jui2.internal_job_id == 2);
  fail_unless((jui1 == jui2) == false);
  fail_unless(jui1 == jui_copy);
  }
END_TEST



Suite *job_usage_info_suite(void)
  {
  Suite *s = suite_create("job_usage_info test suite methods");
  TCase *tc_core = tcase_create("test_constructor");
  tcase_add_test(tc_core, test_constructor);
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
  sr = srunner_create(job_usage_info_suite());
  srunner_set_log(sr, "job_usage_info_suite.log");
  srunner_run_all(sr, CK_NORMAL);
  number_failed = srunner_ntests_failed(sr);
  srunner_free(sr);
  return(number_failed);
  }
