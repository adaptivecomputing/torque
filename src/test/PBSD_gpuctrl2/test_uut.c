#include "license_pbs.h" /* See here for the software license */
#include "lib_ifl.h"
#include "test_PBSD_gpuctrl2.h"
#include <stdlib.h>
#include <stdio.h>

#include "pbs_error.h"
#include "server_limits.h"


START_TEST(PBSD_gpu_put_test)
  {
  fail_unless(PBSD_gpu_put(-1, NULL, NULL, 0, 0, 0, NULL) == PBSE_IVALREQ);
  fail_unless(PBSD_gpu_put(PBS_NET_MAX_CONNECTIONS, NULL, NULL, 0, 0, 0, NULL) == PBSE_IVALREQ);
  }
END_TEST

START_TEST(test_two)
  {


  }
END_TEST

Suite *PBSD_gpuctrl2_suite(void)
  {
  Suite *s = suite_create("PBSD_gpuctrl2_suite methods");
  TCase *tc_core = tcase_create("PBSD_gpu_put_test");
  tcase_add_test(tc_core, PBSD_gpu_put_test);
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
  sr = srunner_create(PBSD_gpuctrl2_suite());
  srunner_set_log(sr, "PBSD_gpuctrl2_suite.log");
  srunner_run_all(sr, CK_NORMAL);
  number_failed = srunner_ntests_failed(sr);
  srunner_free(sr);
  return number_failed;
  }
