#include "license_pbs.h" /* See here for the software license */
#include "lib_ifl.h"
#include "test_pbsD_holdjob.h"
#include <stdlib.h>
#include <stdio.h>


#include "pbs_error.h"

START_TEST(test_pbs_holdjob_err)
  {
  char jobid[1024];
  sprintf(jobid, "1.napali");

  fail_unless(pbs_holdjob_err(-1, jobid, NULL, NULL, NULL) == PBSE_IVALREQ);
  fail_unless(pbs_holdjob_err(PBS_NET_MAX_CONNECTIONS, jobid, NULL, NULL, NULL) == PBSE_IVALREQ);
  memset(&jobid, 0, sizeof(jobid));
  fail_unless(pbs_holdjob_err(0, jobid, NULL, NULL, NULL) == PBSE_IVALREQ);
  fail_unless(pbs_holdjob_err(0, NULL, NULL, NULL, NULL) == PBSE_IVALREQ);
  }
END_TEST


START_TEST(test_two)
  {


  }
END_TEST

Suite *pbsD_holdjob_suite(void)
  {
  Suite *s = suite_create("pbsD_holdjob_suite methods");
  TCase *tc_core = tcase_create("test_pbs_holdjob_err");
  tcase_add_test(tc_core, test_pbs_holdjob_err);
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
  sr = srunner_create(pbsD_holdjob_suite());
  srunner_set_log(sr, "pbsD_holdjob_suite.log");
  srunner_run_all(sr, CK_NORMAL);
  number_failed = srunner_ntests_failed(sr);
  srunner_free(sr);
  return number_failed;
  }
