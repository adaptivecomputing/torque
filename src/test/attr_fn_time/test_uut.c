#include "license_pbs.h" /* See here for the software license */
#include "test_attr_fn_time.h"
#include <stdlib.h>
#include <stdio.h>

#include "attribute.h"
#include "pbs_error.h"

int get_time_string(char *time_string, int string_size, long timeval);
long time_str_to_seconds(const char *str);

START_TEST(test_get_time_string)
  {
  char buf[1024];
  get_time_string(buf, sizeof(buf), 200);
  fail_unless(!strcmp(buf, "00:03:20"));

  get_time_string(buf, sizeof(buf), 3800);
  fail_unless(!strcmp(buf, "01:03:20"));

  get_time_string(buf, sizeof(buf), 7523);
  fail_unless(!strcmp(buf, "02:05:23"), "time: %s", buf);

  get_time_string(buf, sizeof(buf), 380);
  fail_unless(!strcmp(buf, "00:06:20"));
  }
END_TEST

START_TEST(test_time_str_to_seconds)
  {
  const char *s1 = "10:09:08:07.654"; // DD:HH:MM:SS.mmm (.milliseconds)
  const char *s2 = "18:00:00"; // HH:MM::SS
  const char *s3 = "bob"; // bad
  const char *s4 = "18:00:00:00:00:00"; // bad 
  const char *s5 = "18:00:00:walltime=18:00:00"; // bad 
  const char *s6 = "00:04:00"; // HH:MM:SS

  fail_unless(time_str_to_seconds(s1) == 896888); // round up for .654
  fail_unless(time_str_to_seconds(s2) == 64800);
  fail_unless(time_str_to_seconds(s3) == -1);
  fail_unless(time_str_to_seconds(s4) == -1);
  fail_unless(time_str_to_seconds(s5) == -1);
  fail_unless(time_str_to_seconds(s6) == 240);
  }
END_TEST

Suite *attr_fn_time_suite(void)
  {
  Suite *s = suite_create("attr_fn_time_suite methods");
  TCase *tc_core = tcase_create("test_get_time_string");
  tcase_add_test(tc_core, test_get_time_string);
  suite_add_tcase(s, tc_core);

  tc_core = tcase_create("test_time_str_to_seconds");
  tcase_add_test(tc_core, test_time_str_to_seconds);
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
  sr = srunner_create(attr_fn_time_suite());
  srunner_set_log(sr, "attr_fn_time_suite.log");
  srunner_run_all(sr, CK_NORMAL);
  number_failed = srunner_ntests_failed(sr);
  srunner_free(sr);
  return number_failed;
  }
