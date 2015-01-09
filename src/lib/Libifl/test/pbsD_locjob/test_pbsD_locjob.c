#include "license_pbs.h" /* See here for the software license */
#include "lib_ifl.h"
#include "test_pbsD_locjob.h"
#include <stdlib.h>
#include <stdio.h>


#include "pbs_error.h"


START_TEST(test_pbs_locjob_err)
  {
  char jobid[1024];
  sprintf(jobid, "1.napali");
  int err;
  fail_unless(pbs_locjob_err(-1, jobid, NULL, NULL) == NULL);
  fail_unless(pbs_locjob_err(PBS_NET_MAX_CONNECTIONS, jobid, NULL, NULL) == NULL);
  memset(&jobid, 0, sizeof(jobid));
  fail_unless(pbs_locjob_err(0, jobid, NULL, &err) == NULL);
  fail_unless(pbs_locjob_err(0, NULL, NULL, &err) == NULL);
  }
END_TEST


START_TEST(test_two)
  {


  }
END_TEST


Suite *pbsD_locjob_suite(void)
  {
  Suite *s = suite_create("pbsD_locjob_suite methods");
  TCase *tc_core = tcase_create("test_pbs_locjob_err");
  tcase_add_test(tc_core, test_pbs_locjob_err);
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
  sr = srunner_create(pbsD_locjob_suite());
  srunner_set_log(sr, "pbsD_locjob_suite.log");
  srunner_run_all(sr, CK_NORMAL);
  number_failed = srunner_ntests_failed(sr);
  srunner_free(sr);
  return number_failed;
  }
