#include "license_pbs.h" /* See here for the software license */
#include "queue_func.h"
#include "test_queue_func.h"
#include <stdlib.h>
#include <stdio.h>
#include "pbs_error.h"
START_TEST(test_one)
  {


  }
END_TEST

START_TEST(test_two)
  {


  }
END_TEST

Suite *queue_func_suite(void)
  {
  Suite *s = suite_create("queue_func_suite methods");
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
  sr = srunner_create(queue_func_suite());
  srunner_set_log(sr, "queue_func_suite.log");
  srunner_run_all(sr, CK_NORMAL);
  number_failed = srunner_ntests_failed(sr);
  srunner_free(sr);
  return number_failed;
  }
