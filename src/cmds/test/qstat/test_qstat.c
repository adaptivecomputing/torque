#include "license_pbs.h" /* See here for the software license */
#include "qstat.h"
#include "test_qstat.h"
#include <stdlib.h>
#include <stdio.h>


#include "pbs_error.h"

int time_to_string(char *time_string, int total_seconds);

START_TEST(time_to_string_test)
  {
  int  total_seconds = 3666;
  char buf[1024];

  time_to_string(buf, total_seconds);
  fail_unless(!strcmp("01:01:06", buf));

  total_seconds += 3600;
  total_seconds += 120;

  time_to_string(buf, total_seconds);
  fail_unless(!strcmp("02:03:06", buf));

  total_seconds += 120;
  total_seconds += 5;
  time_to_string(buf, total_seconds);
  fail_unless(!strcmp("02:05:11", buf));
  }
END_TEST

START_TEST(test_two)
  {


  }
END_TEST

Suite *qstat_suite(void)
  {
  Suite *s = suite_create("qstat_suite methods");
  TCase *tc_core = tcase_create("time_to_string_test");
  tcase_add_test(tc_core, time_to_string_test);
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
  sr = srunner_create(qstat_suite());
  srunner_set_log(sr, "qstat_suite.log");
  srunner_run_all(sr, CK_NORMAL);
  number_failed = srunner_ntests_failed(sr);
  srunner_free(sr);
  return number_failed;
  }
