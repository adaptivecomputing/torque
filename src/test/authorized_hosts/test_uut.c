#include "license_pbs.h" /* See here for the software license */
#include "lib_utils.h"
#include <stdlib.h>
#include <stdio.h>


#include "pbs_error.h"
#include "pbs_nodes.h"
#include "authorized_hosts.hpp"



START_TEST(test_basics)
  {
  authorized_hosts ah;

  ah.add_authorized_address(1, 1, "roshar");
  ah.add_authorized_address(1, 5, "roshar2");
  ah.add_authorized_address(2, 2, "scadrial");
  ah.add_authorized_address(2, 6, "scadrial2");
  ah.add_authorized_address(3, 3, "nalthis");
  ah.add_authorized_address(3, 7, "nalthis2");
  ah.add_authorized_address(4, 4, "elantris");
  ah.add_authorized_address(4, 8, "elantris2");

  std::string list;
  ah.list_authorized_hosts(list);
  fail_unless(list.find(":1") != std::string::npos, list.c_str());
  fail_unless(list.find(":2") != std::string::npos, list.c_str());
  fail_unless(list.find(":3") != std::string::npos, list.c_str());
  fail_unless(list.find(":4") != std::string::npos, list.c_str());
  fail_unless(list.find(":5") != std::string::npos, list.c_str());
  fail_unless(list.find(":6") != std::string::npos, list.c_str());
  fail_unless(list.find(":7") != std::string::npos, list.c_str());
  fail_unless(list.find(":8") != std::string::npos, list.c_str());

  for (int i = 1; i < 5; i++)
    {
    // Make sure we find what we just added
    fail_unless(ah.is_authorized(i));
    fail_unless(ah.is_authorized(i, i));
    fail_unless(ah.get_authorized_node(i) != NULL);
    fail_unless(ah.get_authorized_node(i, i) != NULL);
    fail_unless(ah.get_authorized_node(i, i + 4) != NULL);

    // Make sure the port matching functions only work if the port matches
    fail_unless(ah.get_authorized_node(i, i + 1) == NULL, "Found authorized node for %d:%d", i, i+1);
    fail_unless(ah.is_authorized(i, i + 1) == false);
    }

  fail_unless(!strcmp(ah.get_authorized_node(1, 1)->get_name(), "roshar"));
  fail_unless(!strcmp(ah.get_authorized_node(1, 5)->get_name(), "roshar2"));
  fail_unless(!strcmp(ah.get_authorized_node(2, 2)->get_name(), "scadrial"));
  fail_unless(!strcmp(ah.get_authorized_node(2, 6)->get_name(), "scadrial2"));
  fail_unless(!strcmp(ah.get_authorized_node(3, 3)->get_name(), "nalthis"));
  fail_unless(!strcmp(ah.get_authorized_node(3, 7)->get_name(), "nalthis2"));
  fail_unless(!strcmp(ah.get_authorized_node(4, 4)->get_name(), "elantris"));
  fail_unless(!strcmp(ah.get_authorized_node(4, 8)->get_name(), "elantris2"));

  fail_unless(ah.remove_address(1, 1) == true);
  fail_unless(ah.is_authorized(1) == true); // the other port is still there
  fail_unless(ah.is_authorized(1, 5) == true); // the other port is still there
  fail_unless(ah.is_authorized(1, 1) == false);
  fail_unless(ah.remove_address(1, 5) == true);
  fail_unless(ah.remove_address(1, 1) == false);
  fail_unless(ah.remove_address(1, 5) == false);
  fail_unless(ah.is_authorized(1) == false);
  fail_unless(ah.is_authorized(1, 1) == false);
  fail_unless(ah.is_authorized(1, 5) == false);

  // Random stuff shouldn't work
  for (int i = 5; i < 9; i++)
    {
    fail_unless(ah.is_authorized(i) == false);
    fail_unless(ah.is_authorized(i, i) == false);
    fail_unless(ah.get_authorized_node(i) == NULL);
    fail_unless(ah.get_authorized_node(i, i) == NULL);
    }

  // Clear so everything fails
  ah.clear();
  
  for (int i = 1; i < 5; i++)
    {
    fail_unless(ah.is_authorized(i) == false);
    fail_unless(ah.is_authorized(i, i) == false);
    fail_unless(ah.get_authorized_node(i) == NULL);
    fail_unless(ah.get_authorized_node(i, i) == NULL);
    }
  }
END_TEST



Suite *authorized_hosts_suite(void)
  {
  Suite *s = suite_create("authorized_hosts_suite methods");
  TCase *tc_core = tcase_create("test_basics");
  tcase_add_test(tc_core, test_basics);
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
  sr = srunner_create(authorized_hosts_suite());
  srunner_set_log(sr, "authorized_hosts_suite.log");
  srunner_run_all(sr, CK_NORMAL);
  number_failed = srunner_ntests_failed(sr);
  srunner_free(sr);
  return number_failed;
  }
