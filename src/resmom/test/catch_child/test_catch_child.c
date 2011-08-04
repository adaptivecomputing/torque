#include "test_catch_child.h"
#include "catch_child.h"
extern int termin_child;

START_TEST (test_catch_child_1)
  {
  termin_child = 0;
  catch_child(2);
  fail_unless(termin_child == 1, "termin_child is set to 1 by this method");
  }
END_TEST

Suite *catch_child_suite(void)
  {
  Suite *s = suite_create("catch_child methods");
  TCase *tc_core = tcase_create("Core");
  tcase_add_test(tc_core, test_catch_child_1);
  suite_add_tcase(s, tc_core);
  return s;
  }

int main(void)
  {
  int number_failed = 0;
  sr = srunner_create(catch_child_suite());
  srunner_set_log(sr, "catch_child_suite.log");
  srunner_run_all(sr, CK_NORMAL);
  number_failed = srunner_ntests_failed(sr);
  srunner_free(sr);
  return number_failed;
  }
