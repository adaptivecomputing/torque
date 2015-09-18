#include "license_pbs.h" /* See here for the software license */
#include "test_requests.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>


#include "pbs_error.h"

void string_replchar(const char*, char, char);

START_TEST(test_string_replchar)
  {
  char *mystring;

  mystring = (char *)calloc(1024, sizeof(char));
  fail_unless(NULL != mystring);

  // NULL should be handled without error
  string_replchar(NULL, 'a', 'b');

  // string empty
  string_replchar(mystring, 'a', 'b');
  fail_unless('\0' == *mystring);

  // simple replacement
  snprintf(mystring, 1024, "abcdefga");
  string_replchar(mystring, 'a', 'z');
  fail_unless(strcmp("zbcdefgz", mystring) == 0);

  // no replacement
  string_replchar(mystring, 'a', 'z');
  fail_unless(strcmp("zbcdefgz", mystring) == 0);
  }
END_TEST

START_TEST(test_two)
  {


  }
END_TEST

Suite *requests_suite(void)
  {
  Suite *s = suite_create("requests_suite methods");
  TCase *tc_core = tcase_create("test_string_replchar");
  tcase_add_test(tc_core, test_string_replchar);
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
  sr = srunner_create(requests_suite());
  srunner_set_log(sr, "requests_suite.log");
  srunner_run_all(sr, CK_NORMAL);
  number_failed = srunner_ntests_failed(sr);
  srunner_free(sr);
  return number_failed;
  }
