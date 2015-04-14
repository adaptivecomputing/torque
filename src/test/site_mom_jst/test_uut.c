#include "license_pbs.h" /* See here for the software license */
#include "lib_site.h"
#include "test_site_mom_jst.h"
#include <stdlib.h>
#include <stdio.h>
#include "pbs_job.h"

#include "pbs_error.h"

START_TEST(test_one)
  {
  /* As this is site specific, there is no implementation in this function */
  job *pjob = NULL;
  int rc = -1;
  rc = site_job_setup(pjob);
  fail_unless(rc == 0, "The return value has changed!!!");
  }
END_TEST

Suite *site_mom_jst_suite(void)
  {
  Suite *s = suite_create("site_mom_jst_suite methods");
  TCase *tc_core = tcase_create("test_one");
  tcase_add_test(tc_core, test_one);
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
/*  rundebug(); */
  sr = srunner_create(site_mom_jst_suite());
  srunner_set_log(sr, "site_mom_jst_suite.log");
  srunner_run_all(sr, CK_NORMAL);
  number_failed = srunner_ntests_failed(sr);
  srunner_free(sr);
  return number_failed;
  }
