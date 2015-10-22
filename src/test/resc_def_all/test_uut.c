#include "license_pbs.h" /* See here for the software license */
#include "test_uut.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "pbs_error.h"

long count_proc(char *);

START_TEST(test_count_proc)
  {

  fail_unless(count_proc(NULL) == 0);
  fail_unless(count_proc(strdup("")) == 0);
  fail_unless(count_proc(strdup("1")) == 1);
  fail_unless(count_proc(strdup("1+2:ppn=5")) == 11);
  fail_unless(count_proc(strdup("1:ppn=1+5:ppn=100+5+1000:ppn=10000")) == 10000506);

  }
END_TEST

START_TEST(test_two)
  {


  }
END_TEST

Suite *resc_def_all_suite(void)
  {
  Suite *s = suite_create("resc_def_all_suite methods");
  TCase *tc_core = tcase_create("test_count_proc");
  tcase_add_test(tc_core, test_count_proc);
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
  sr = srunner_create(resc_def_all_suite());
  srunner_set_log(sr, "resc_def_all_suite.log");
  srunner_run_all(sr, CK_NORMAL);
  number_failed = srunner_ntests_failed(sr);
  srunner_free(sr);
  return number_failed;
  }
