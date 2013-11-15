#include "license_pbs.h" /* See here for the software license */
#include <pbs_config.h>
#include "mom_main.h"
#include "test_mom_main.h"
#include <stdlib.h>
#include <stdio.h>

extern bool parsing_hierarchy;
extern bool received_cluster_addrs;
extern char *path_mom_hierarchy;

void read_mom_hierarchy();

#include "pbs_error.h"

START_TEST(test_read_mom_hierarchy)
  {
  system("rm -f bob");
  path_mom_hierarchy = strdup("bob");
  read_mom_hierarchy();
  fail_unless(received_cluster_addrs == false);
  system("touch bob");
  // the following lines need more spoofing in order to work correctly
//  parsing_hierarchy = true;
//  read_mom_hierarchy();
//  fail_unless(received_cluster_addrs == true);
//  parsing_hierarchy = false;
  }
END_TEST

START_TEST(test_two)
  {


  }
END_TEST

Suite *mom_main_suite(void)
  {
  Suite *s = suite_create("mom_main_suite methods");
  TCase *tc_core = tcase_create("test_read_mom_hiearchy");
  tcase_add_test(tc_core, test_read_mom_hierarchy);
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
  sr = srunner_create(mom_main_suite());
  srunner_set_log(sr, "mom_main_suite.log");
  srunner_run_all(sr, CK_NORMAL);
  number_failed = srunner_ntests_failed(sr);
  srunner_free(sr);
  return number_failed;
  }
