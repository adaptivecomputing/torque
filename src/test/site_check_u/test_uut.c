#include "license_pbs.h" /* See here for the software license */
#include "lib_site.h"
#include "test_site_check_u.h"
#include <stdlib.h>
#include <stdio.h>


#include "pbs_error.h"

bool is_permitted_by_node_submit(const char *orighost, int logging);
extern bool exists;

START_TEST(test_is_permitted_by_node_submit)
  {
  int logging = 5;
  exists = true;
  fail_unless(is_permitted_by_node_submit("napali", logging) == false);
  fail_unless(is_permitted_by_node_submit("waimea", logging) == false);
  fail_unless(is_permitted_by_node_submit("lihue", logging) == true);
  fail_unless(is_permitted_by_node_submit("wailua", logging) == true);

  exists = false;
  fail_unless(is_permitted_by_node_submit("haole", logging) == false);
  }
END_TEST

START_TEST(test_two)
  {


  }
END_TEST

Suite *site_check_u_suite(void)
  {
  Suite *s = suite_create("site_check_u_suite methods");
  TCase *tc_core = tcase_create("test_is_permitted_by_node_submit");
  tcase_add_test(tc_core, test_is_permitted_by_node_submit);
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
  sr = srunner_create(site_check_u_suite());
  srunner_set_log(sr, "site_check_u_suite.log");
  srunner_run_all(sr, CK_NORMAL);
  number_failed = srunner_ntests_failed(sr);
  srunner_free(sr);
  return number_failed;
  }
