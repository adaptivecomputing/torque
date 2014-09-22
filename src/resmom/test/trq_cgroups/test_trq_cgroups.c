#include <stdio.h>
#include <stdlib.h>
#include <check.h>
#include "trq_cgroups.h"

void trq_cg_init_subsys_online();

START_TEST(test_trq_cg_init_subsys_online)
  {
  trq_cg_init_subsys_online();
  }
END_TEST

START_TEST(test_trq_cg_initialize_hierarchy)
  {
  int rc;

  rc = trq_cg_initialize_hierarchy();
  fail_unless(rc==0, "Failed to initialize hierarchy");
  }
END_TEST




START_TEST(test_two)
  {
  }
END_TEST




Suite *trq_cgroups_suite(void)
  {
  Suite *s = suite_create("trq_cgroups test suite methods");
  TCase *tc_core = tcase_create("test_trq_cg_init_subsys_online");
  tcase_add_test(tc_core, test_trq_cg_init_subsys_online);
  suite_add_tcase(s, tc_core);
  
  tc_core = tcase_create("test_trq_cg_initialize_hierarchy");
  tcase_add_test(tc_core, test_trq_cg_initialize_hierarchy);
  suite_add_tcase(s, tc_core);
  
  tc_core = tcase_create("test_two");
  tcase_add_test(tc_core, test_two);
  suite_add_tcase(s, tc_core);
  
  return(s);
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
  return(number_failed);
  }
