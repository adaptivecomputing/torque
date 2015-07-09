#include "license_pbs.h" /* See here for the software license */
#include "lib_ifl.h"
#include "test_pbsD_resc.h"
#include <stdlib.h>
#include <stdio.h>


#include "pbs_error.h"

int PBS_resc(int c, int reqtype, char **rescl, int ct, resource_t rh);

START_TEST(test_PBS_resc)
  {
  fail_unless(PBS_resc(-1, 0, NULL, 1, 2) == PBSE_IVALREQ);
  fail_unless(PBS_resc(PBS_NET_MAX_CONNECTIONS, 0, NULL, 1, 2) == PBSE_IVALREQ);
  }
END_TEST


START_TEST(test_pbs_rescquery)
  {
  fail_unless(pbs_rescquery(-1, NULL, 1, NULL, NULL, NULL, NULL) == PBSE_IVALREQ);
  fail_unless(pbs_rescquery(PBS_NET_MAX_CONNECTIONS, NULL, 1, NULL, NULL, NULL, NULL) == PBSE_IVALREQ);
  }
END_TEST


START_TEST(test_pbs_rescreserve)
  {
  fail_unless(pbs_rescreserve(-1, NULL, 1, NULL) == PBSE_IVALREQ);
  fail_unless(pbs_rescreserve(PBS_NET_MAX_CONNECTIONS, NULL, 1, NULL) == PBSE_IVALREQ);
  }
END_TEST


START_TEST(test_pbs_rescrelease)
  {
  fail_unless(pbs_rescrelease(-1, 0) == PBSE_IVALREQ);
  fail_unless(pbs_rescrelease(PBS_NET_MAX_CONNECTIONS, 0) == PBSE_IVALREQ);
  }
END_TEST


Suite *pbsD_resc_suite(void)
  {
  Suite *s = suite_create("pbsD_resc_suite methods");
  TCase *tc_core = tcase_create("test_PBS_resc");
  tcase_add_test(tc_core, test_PBS_resc);
  tcase_add_test(tc_core, test_pbs_rescrelease);
  suite_add_tcase(s, tc_core);

  tc_core = tcase_create("test_pbs_rescquery");
  tcase_add_test(tc_core, test_pbs_rescquery);
  tcase_add_test(tc_core, test_pbs_rescreserve);
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
  sr = srunner_create(pbsD_resc_suite());
  srunner_set_log(sr, "pbsD_resc_suite.log");
  srunner_run_all(sr, CK_NORMAL);
  number_failed = srunner_ntests_failed(sr);
  srunner_free(sr);
  return number_failed;
  }
