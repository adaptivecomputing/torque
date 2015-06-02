#include "license_pbs.h" /* See here for the software license */
#include "lib_ifl.h"
#include "test_pbsD_stagein.h"
#include <stdlib.h>
#include <stdio.h>


#include "pbs_error.h"

START_TEST(test_pbs_stagein)
  {
  fail_unless(pbs_stagein(-1, strdup("1.napali"), NULL, NULL) == PBSE_IVALREQ);
  fail_unless(pbs_stagein(PBS_NET_MAX_CONNECTIONS, strdup("1.napali"), NULL, NULL) == PBSE_IVALREQ);
  fail_unless(pbs_stagein(0, strdup(""), NULL, NULL) == PBSE_IVALREQ);
  fail_unless(pbs_stagein(0, NULL, NULL, NULL) == PBSE_IVALREQ);
  }
END_TEST

START_TEST(test_two)
  {


  }
END_TEST

Suite *pbsD_stagein_suite(void)
  {
  Suite *s = suite_create("pbsD_stagein_suite methods");
  TCase *tc_core = tcase_create("test_pbs_stagein");
  tcase_add_test(tc_core, test_pbs_stagein);
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
  sr = srunner_create(pbsD_stagein_suite());
  srunner_set_log(sr, "pbsD_stagein_suite.log");
  srunner_run_all(sr, CK_NORMAL);
  number_failed = srunner_ntests_failed(sr);
  srunner_free(sr);
  return number_failed;
  }
