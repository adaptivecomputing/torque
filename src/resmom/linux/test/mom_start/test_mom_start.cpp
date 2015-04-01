#include "license_pbs.h" /* See here for the software license */
#include "mom_mach.h"
#include "test_mom_start.h"
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

Suite *mom_start_suite(void)
  {
  Suite *s = suite_create("mom_start_suite methods");
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
  sr = srunner_create(mom_start_suite());
  srunner_set_log(sr, "mom_start_suite.log");
  srunner_run_all(sr, CK_NORMAL);
  number_failed = srunner_ntests_failed(sr);
  srunner_free(sr);
  return number_failed;
  }
