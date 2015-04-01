#include "license_pbs.h" /* See here for the software license */
#include "lib_ifl.h"
#include "test_pbsD_alterjo.h"
#include <stdlib.h>
#include <stdio.h>


#include "pbs_error.h"

START_TEST(test_pbsD_alterjob_asyncflag)
  {
  char jobid[1024];
  snprintf(jobid, sizeof(jobid), "1.napali");

  fail_unless(pbs_alterjob_asyncflag(-1, jobid, NULL, NULL, 1, NULL) == PBSE_IVALREQ);
  fail_unless(pbs_alterjob_asyncflag(PBS_NET_MAX_CONNECTIONS, jobid, NULL, NULL, 1, NULL) == PBSE_IVALREQ);

  fail_unless(pbs_alterjob_asyncflag(0, NULL, NULL, NULL, 1, NULL) == PBSE_IVALREQ);
  memset(&jobid, 0, sizeof(jobid));
  fail_unless(pbs_alterjob_asyncflag(0, jobid, NULL, NULL, 1, NULL) == PBSE_IVALREQ);
  
  }
END_TEST

START_TEST(test_two)
  {


  }
END_TEST

Suite *pbsD_alterjo_suite(void)
  {
  Suite *s = suite_create("pbsD_alterjo_suite methods");
  TCase *tc_core = tcase_create("test_pbsD_alterjob_aynscflag");
  tcase_add_test(tc_core, test_pbsD_alterjob_asyncflag);
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
  sr = srunner_create(pbsD_alterjo_suite());
  srunner_set_log(sr, "pbsD_alterjo_suite.log");
  srunner_run_all(sr, CK_NORMAL);
  number_failed = srunner_ntests_failed(sr);
  srunner_free(sr);
  return number_failed;
  }
