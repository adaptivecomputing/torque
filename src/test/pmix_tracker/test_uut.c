
#include "pmix_tracker.hpp"
#include "pbs_job.h"

#include <check.h>

START_TEST(test_constructor)
  {
  job *pjob = (job *)calloc(1, sizeof(job));
  strcpy(pjob->ji_qs.ji_jobid, "1.napali");

  pmix_tracker pt(pjob, 1);
  fail_unless(pt.id_matches("1") == true);
  }
END_TEST


Suite *pmix_tracker_suite(void)
  {
  Suite *s = suite_create("pmix_tracker_suite methods");
  TCase *tc_core = tcase_create("test_constructor");
  tcase_add_test(tc_core, test_constructor);
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
  sr = srunner_create(pmix_tracker_suite());
  srunner_set_log(sr, "pmix_tracker_suite.log");
  srunner_run_all(sr, CK_NORMAL);
  number_failed = srunner_ntests_failed(sr);
  srunner_free(sr);
  return number_failed;
  }

