#include "license_pbs.h" /* See here for the software license */
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <string>
#include <sstream>
#include "trq_auth_daemon.h"
#include "test_trq_auth_daemon.h"
#include "pbs_error.h"


START_TEST(test_load_trqauthd_config)
  {
  }
END_TEST

START_TEST(test_two)
  {
  }
END_TEST

Suite *trq_auth_daemon_suite(void)
  {
  Suite *s = suite_create("trq_auth_daemon_suite methods");
  TCase *tc_core = tcase_create("test_load_trqauthd_config");
  tcase_add_test(tc_core, test_load_trqauthd_config);
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
  SRunner *sr = NULL;  rundebug();
  sr = srunner_create(trq_auth_daemon_suite());
  srunner_set_log(sr, "trq_auth_daemon_suite.log");
  srunner_run_all(sr, CK_NORMAL);
  number_failed = srunner_ntests_failed(sr);
  srunner_free(sr);
  return number_failed;
  }
