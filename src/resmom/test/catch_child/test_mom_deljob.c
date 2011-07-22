#include "test_catch_child.h"
#include "catch_child.h"
#include "pbs_job.h"

START_TEST(test_mom_deljob_1)
  {
  job *pjob;
  mom_deljob(pjob);
  }
END_TEST

Suite *mom_deljob_suite(void)
  {
  Suite *s = suite_create("mom_deljob methods");
  TCase *tc_core = tcase_create("Core");
  tcase_add_test(tc_core, test_mom_deljob_1);
  suite_add_tcase(s, tc_core);
  return s;
  }

int main(void)
  {
  int number_failed = 0;
  SRunner *sr = srunner_create(mom_deljob_suite());
  srunner_set_log(sr, "mom_deljob_suite.log");
  srunner_run_all(sr, CK_NORMAL);
  number_failed = srunner_ntests_failed(sr);
  srunner_free(sr);
  return number_failed;
  }
