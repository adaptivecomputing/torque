#include "license_pbs.h" /* See here for the software license */
#include "lib_ifl.h"
#include "test_PBSD_manager_caps.h"
#include <stdlib.h>
#include <stdio.h>


#include "pbs_error.h"

START_TEST(test_PBSD_manager)
  {
  fail_unless(PBSD_manager(-1, 0, 0, 0, NULL, NULL, NULL, NULL) == PBSE_IVALREQ);
  fail_unless(PBSD_manager(PBS_NET_MAX_CONNECTIONS, 0, 0, 0, NULL, NULL, NULL, NULL) == PBSE_IVALREQ);

  }
END_TEST

START_TEST(test_two)
  {


  }
END_TEST

Suite *PBSD_manager_caps_suite(void)
  {
  Suite *s = suite_create("PBSD_manager_caps_suite methods");
  TCase *tc_core = tcase_create("test_PBSD_manager");
  tcase_add_test(tc_core, test_PBSD_manager);
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
  sr = srunner_create(PBSD_manager_caps_suite());
  srunner_set_log(sr, "PBSD_manager_caps_suite.log");
  srunner_run_all(sr, CK_NORMAL);
  number_failed = srunner_ntests_failed(sr);
  srunner_free(sr);
  return number_failed;
  }
