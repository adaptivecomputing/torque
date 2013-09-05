#include <stdio.h>
#include <stdlib.h>
#include "utils.h"
#include <check.h>

extern int put_env_var(const char *name, const char *value);


START_TEST(test_one)
  {
  char *p;
  const char *name;
  const char *value;
  char buf[1024];
  int rc;

  /* the test variables */
  name = "TEST_NAME_ENV";
  value = "TEST_VALUE_ENV";

  /* call the function and look for normal return code */
  rc = put_env_var(name, value);
  fail_unless(rc == 0, "put_env_var returned with non-zero code");

  /* test NULL variable name */
  rc = put_env_var(NULL, value);
  fail_unless(rc != 0, "put_env_var did not fail when passed NULL variable name");

  /* test NULL variable name value */
  rc = put_env_var(name, NULL);
  fail_unless(rc != 0, "put_env_var did not fail when passed NULL variable value");

  /* test NULL variable name and NULL variable name value */
  rc = put_env_var(NULL, NULL);
  fail_unless(rc != 0, "put_env_var did not fail when passed NULL variable name and NULL variable value");

  /* confirm env var in environment */
  p = getenv(name);
  sprintf(buf, "%s=%s not found in environment", name, value);
  fail_unless(!strcmp(p, value), buf);
  }
END_TEST




Suite *u_putenv_suite(void)
  {
  Suite *s = suite_create("u_putenv test suite methods");
  TCase *tc_core = tcase_create("test_one");
  tcase_add_test(tc_core, test_one);
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
  sr = srunner_create(u_putenv_suite());
  srunner_set_log(sr, "u_putenv_suite.log");
  srunner_run_all(sr, CK_NORMAL);
  number_failed = srunner_ntests_failed(sr);
  srunner_free(sr);
  return(number_failed);
  }
