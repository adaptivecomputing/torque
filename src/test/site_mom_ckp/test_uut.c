#include "license_pbs.h" /* See here for the software license */
#include "lib_site.h"
#include "test_site_mom_ckp.h"
#include <stdlib.h>
#include <stdio.h>


#include "pbs_error.h"

START_TEST(test_one)
  {
  job *pjob = NULL;
  int hold_type = -1;
  int rc = -1;
  rc = site_mom_postchk(pjob, hold_type);
  fail_unless(rc == 0, "return value has changed!!!");
  }
END_TEST

START_TEST(test_two)
  {
  job *pjob = NULL;
  int rc = -1;
  rc = site_mom_prerst(pjob);
  fail_unless(rc == 0, "return value has changed!!!");
  }
END_TEST

Suite *site_mom_ckp_suite(void)
  {
  Suite *s = suite_create("site_mom_ckp_suite methods");
  TCase *tc_core = tcase_create("test_one");
  tcase_add_test(tc_core, test_one);
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
  sr = srunner_create(site_mom_ckp_suite());
  srunner_set_log(sr, "site_mom_ckp_suite.log");
  srunner_run_all(sr, CK_NORMAL);
  number_failed = srunner_ntests_failed(sr);
  srunner_free(sr);
  return number_failed;
  }
