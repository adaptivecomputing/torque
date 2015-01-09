#include "license_pbs.h" /* See here for the software license */
#include "lib_ifl.h"
#include "test_PBSD_status.h"
#include <stdlib.h>
#include <stdio.h>


#include "pbs_error.h"

START_TEST(test_one)
  {


  }
END_TEST

START_TEST(test_PBSD_status_get)
  {
  fail_unless(PBSD_status_get(NULL, -1) == NULL);
  fail_unless(PBSD_status_get(NULL, PBS_NET_MAX_CONNECTIONS) == NULL);

  }
END_TEST

Suite *PBSD_status_suite(void)
  {
  Suite *s = suite_create("PBSD_status_suite methods");
  TCase *tc_core = tcase_create("test_one");
  tcase_add_test(tc_core, test_one);
  suite_add_tcase(s, tc_core);

  tc_core = tcase_create("test_PBSD_status_get");
  tcase_add_test(tc_core, test_PBSD_status_get);
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
  sr = srunner_create(PBSD_status_suite());
  srunner_set_log(sr, "PBSD_status_suite.log");
  srunner_run_all(sr, CK_NORMAL);
  number_failed = srunner_ntests_failed(sr);
  srunner_free(sr);
  return number_failed;
  }
