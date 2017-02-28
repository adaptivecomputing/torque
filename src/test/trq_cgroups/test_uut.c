#include "license_pbs.h" /* See here for the software license */
#include "mom_mach.h"
#include "test_trq_cgroups.h"
#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include <list>

#include "pbs_error.h"
#include "pbs_job.h"
#include "trq_cgroups.h"

extern int LOGLEVEL;
extern bool global_trq_cg_signal_tasks_ok;

START_TEST(test_trq_cg_signal_tasks)
  {
  LOGLEVEL = 9;
  trq_cg_signal_tasks(".", SIGTERM);
  fail_unless(global_trq_cg_signal_tasks_ok, "test_trq_cg_signal_tasks failed");
  }
END_TEST

START_TEST(test_two)
  {
  }
END_TEST

Suite *trq_cgroups_suite(void)
  {
  Suite *s = suite_create("trq_cgroups_suite methods");
  TCase *tc_core = tcase_create("test_trq_cg_signal_tasks");
  tcase_add_test(tc_core, test_trq_cg_signal_tasks);
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
  sr = srunner_create(trq_cgroups_suite());
  srunner_set_log(sr, "trq_cgroups_suite.log");
  srunner_run_all(sr, CK_NORMAL);
  number_failed = srunner_ntests_failed(sr);
  srunner_free(sr);
  return number_failed;
  }
