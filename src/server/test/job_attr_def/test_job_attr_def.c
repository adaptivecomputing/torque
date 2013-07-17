#include <stdio.h>
#include <stdlib.h>
#include <check.h>


START_TEST(test_one)
  {
  }
END_TEST




START_TEST(test_two)
  {
  }
END_TEST




Suite *job_attr_def_suite(void)
  {
  Suite *s = suite_create("job_attr_def test suite methods");
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
  sr = srunner_create(job_attr_def_suite());
  srunner_set_log(sr, "job_attr_def_suite.log");
  srunner_run_all(sr, CK_NORMAL);
  number_failed = srunner_ntests_failed(sr);
  srunner_free(sr);
  return(number_failed);
  }
