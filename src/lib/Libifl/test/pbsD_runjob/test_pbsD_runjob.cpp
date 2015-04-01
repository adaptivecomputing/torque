#include "license_pbs.h" /* See here for the software license */
#include "lib_ifl.h"
#include "test_pbsD_runjob.h"
#include <stdlib.h>
#include <stdio.h>


#include "pbs_error.h"

START_TEST(test_pbs_runjob_err)
  {
  int err;
  fail_unless(pbs_runjob_err(-1, strdup("1.napali"), NULL, NULL, &err) == PBSE_IVALREQ * -1);
  fail_unless(pbs_runjob_err(PBS_NET_MAX_CONNECTIONS, strdup("1.napali"), NULL, NULL, &err) == PBSE_IVALREQ * -1);
  fail_unless(pbs_runjob_err(0, strdup(""), NULL, NULL, &err) == PBSE_IVALREQ * -1);
  fail_unless(pbs_runjob_err(0, NULL, NULL, NULL, &err) == PBSE_IVALREQ * -1);
  }
END_TEST


START_TEST(test_two)
  {


  }
END_TEST

Suite *pbsD_runjob_suite(void)
  {
  Suite *s = suite_create("pbsD_runjob_suite methods");
  TCase *tc_core = tcase_create("test_pbs_runjob_err");
  tcase_add_test(tc_core, test_pbs_runjob_err);
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
  sr = srunner_create(pbsD_runjob_suite());
  srunner_set_log(sr, "pbsD_runjob_suite.log");
  srunner_run_all(sr, CK_NORMAL);
  number_failed = srunner_ntests_failed(sr);
  srunner_free(sr);
  return number_failed;
  }
