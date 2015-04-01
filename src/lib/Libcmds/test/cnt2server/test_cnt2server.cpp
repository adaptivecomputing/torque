#include "license_pbs.h" /* See here for the software license */
#include "lib_cmds.h"
#include "test_cnt2server.h"
#include <stdlib.h>
#include <stdio.h>

#include "csv.h"
#include "pbs_error.h"
#include "pbs_ifl.h"


START_TEST(test_cnt2server_conf)
  {
  int rc;
  long retry = 10;

  rc = cnt2server_conf(retry);
  fail_unless(rc == PBSE_NONE, "cnt2server_retry failed", rc);

  }
END_TEST

START_TEST(test_two)
  {


  }
END_TEST

Suite *cnt2server_suite(void)
  {
  Suite *s = suite_create("cnt2server_suite methods");
  TCase *tc_core = tcase_create("test_cnt2server_conf");
  tcase_add_test(tc_core, test_cnt2server_conf);
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
  sr = srunner_create(cnt2server_suite());
  srunner_set_log(sr, "cnt2server_suite.log");
  srunner_run_all(sr, CK_NORMAL);
  number_failed = srunner_ntests_failed(sr);
  srunner_free(sr);
  return number_failed;
  }
