#include "license_pbs.h" /* See here for the software license */
#include "test_geteusernam.h"
#include <stdlib.h>
#include <stdio.h>
#include <string>
#include "pbs_error.h"

void get_user_host_from_user( std::string &user_host, const std::string user);


START_TEST(test_get_user_host_from_user)
  {
  std::string user_host;
  std::string user;

  user = "user@hostname";
  get_user_host_from_user(user_host, user);
  fail_unless(strcmp(user_host.c_str(), "hostname") == 0);

  user = "user";
  get_user_host_from_user(user_host, user);
  fail_unless(user_host.size() == 0);

  }
END_TEST

START_TEST(test_two)
  {


  }
END_TEST

Suite *geteusernam_suite(void)
  {
  Suite *s = suite_create("geteusernam_suite methods");
  TCase *tc_core = tcase_create("test_get_user_host_from_user");
  tcase_add_test(tc_core, test_get_user_host_from_user);
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
  sr = srunner_create(geteusernam_suite());
  srunner_set_log(sr, "geteusernam_suite.log");
  srunner_run_all(sr, CK_NORMAL);
  number_failed = srunner_ntests_failed(sr);
  srunner_free(sr);
  return number_failed;
  }
