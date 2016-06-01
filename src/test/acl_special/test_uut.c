#include <stdio.h>
#include <stdlib.h>
#include <check.h>

#include "acl_special.hpp"


START_TEST(test_parse_qmgr_input)
  {
  acl_special a;
  std::string qmgr("dbeer@napali");
  std::string user;
  std::string host;
  std::string group;

  a.parse_qmgr_input(qmgr, user, host);
  fail_unless(user == "dbeer");
  fail_unless(host == "napali");

  qmgr = "company@napali";
  a.parse_qmgr_input(qmgr, user, host);
  fail_unless(user == "company");
  fail_unless(host == "napali");

  qmgr = "blahdieblah";
  a.parse_qmgr_input(qmgr, user, host);
  fail_unless(user.size() == 0);
  fail_unless(host.size() == 0);
  }
END_TEST




START_TEST(test_basics)
  {
  acl_special a;

  a.add_user_configuration("dbeer@napali");
  a.add_group_configuration("company@waimea");

  fail_unless(a.is_authorized("waimea", "jbeer") == false);
  fail_unless(a.is_authorized("napali", "dbeer") == true);
  }
END_TEST




Suite *acl_special_suite(void)
  {
  Suite *s = suite_create("acl_special test suite methods");
  TCase *tc_core = tcase_create("test_parse_qmgr_input");
  tcase_add_test(tc_core, test_parse_qmgr_input);
  suite_add_tcase(s, tc_core);
  
  tc_core = tcase_create("test_basics");
  tcase_add_test(tc_core, test_basics);
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
  sr = srunner_create(acl_special_suite());
  srunner_set_log(sr, "acl_special_suite.log");
  srunner_run_all(sr, CK_NORMAL);
  number_failed = srunner_ntests_failed(sr);
  srunner_free(sr);
  return(number_failed);
  }
