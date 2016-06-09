#include <stdio.h>
#include <stdlib.h>
#include "utils.h"
#include <check.h>
#include <vector>
#include <string>



START_TEST(test_capture_until_close_character)
  {
  char *str = strdup("bob:(tim is bob)");
  char *start = strchr(str, '(');
  char *ptr = NULL;
  std::string storage;

  start++;

  capture_until_close_character(&ptr, storage, '"');
  fail_unless(storage.size() == 0);
  capture_until_close_character(&start, storage, '"');
  fail_unless(storage.size() == 0);
  capture_until_close_character(&start, storage, ')');
  fail_unless(storage == "tim is bob");
  }
END_TEST


START_TEST(test_translate_vector_to_range_string)
  {
  std::string range_string;
  std::vector<int> indices;

  for (int i = 0; i < 10; i++)
    indices.push_back(i);

  translate_vector_to_range_string(range_string, indices);
  fail_unless(range_string == "0-9", "string is %s", range_string.c_str());

  indices.clear();
  for (int i = 0; i < 10; i++)
    {
    if (i % 2 == 0)
      indices.push_back(i);
    }

  translate_vector_to_range_string(range_string, indices);
  fail_unless(range_string == "0,2,4,6,8", range_string.c_str());
  
  indices.clear();
  for (int i = 0; i < 3; i++)
    indices.push_back(i);
  for (int i = 4; i < 7; i++)
    indices.push_back(i);
  translate_vector_to_range_string(range_string, indices);
  fail_unless(range_string == "0-2,4-6");
  
  indices.clear();
  for (int i = 0; i < 3; i++)
    indices.push_back(i);
  for (int i = 4; i < 7; i++)
    indices.push_back(i);
  indices.push_back(8);
  indices.push_back(10);
  for (int i = 13; i < 15; i++)
    indices.push_back(i);

  translate_vector_to_range_string(range_string, indices);
  fail_unless(range_string == "0-2,4-6,8,10,13-14");
  }
END_TEST


START_TEST(test_translate_range_string_to_vector)
  {
  std::vector<int> indices;

  translate_range_string_to_vector("1\n", indices);
  fail_unless(indices.size() == 1);
  fail_unless(indices[0] == 1);

  indices.clear();
  translate_range_string_to_vector("0-1\n", indices);
  fail_unless(indices.size() == 2);
  fail_unless(indices[0] == 0);
  fail_unless(indices[1] == 1);

  indices.clear();
  translate_range_string_to_vector("1-4", indices);
  fail_unless(indices.size() == 4);
  fail_unless(indices[0] == 1);
  fail_unless(indices[1] == 2);
  fail_unless(indices[2] == 3);
  fail_unless(indices[3] == 4);
  
  indices.clear();
  translate_range_string_to_vector("0-2,6-8", indices);
  fail_unless(indices.size() == 6);
  fail_unless(indices[0] == 0);
  fail_unless(indices[1] == 1);
  fail_unless(indices[2] == 2);
  fail_unless(indices[3] == 6);
  fail_unless(indices[4] == 7);
  fail_unless(indices[5] == 8);

  indices.clear();
  translate_range_string_to_vector("qt32", indices);
  // Invalid, should give us a vector of 0
  fail_unless(indices.size() == 0);

  indices.clear();
  translate_range_string_to_vector("\n       6142-6143\n     ", indices);
  fail_unless(indices.size() == 2);
  fail_unless(indices[0] == 6142);
  fail_unless(indices[1] == 6143);
  }
END_TEST

START_TEST(test_task_hosts_match)
  {
  std::string  hostlist;
  std::string  this_host;
  bool         match;

  hostlist = "numa3";
  this_host = "numa3";

  match = task_hosts_match(hostlist.c_str(), this_host.c_str());
  fail_unless(match==true);

  hostlist = "numa3.ac";

  /* short names can match FQDN */
  match = task_hosts_match(hostlist.c_str(), this_host.c_str());
  fail_unless(match==true);

  hostlist = "nowforsomethingcompletelydiffernt";
  match = task_hosts_match(hostlist.c_str(), this_host.c_str());
  fail_unless(match==false);

  }
END_TEST


Suite *u_misc_suite(void)
  {
  Suite *s = suite_create("u_misc test suite methods");
  TCase *tc_core = tcase_create("test_translate_range_string_to_vector");
  tcase_add_test(tc_core, test_translate_range_string_to_vector);
  tcase_add_test(tc_core, test_translate_vector_to_range_string);
  suite_add_tcase(s, tc_core);
  
  tc_core = tcase_create("test_capture");
  tcase_add_test(tc_core, test_capture_until_close_character);
  suite_add_tcase(s, tc_core);
  
  tc_core = tcase_create("test_task_hosts_match");
  tcase_add_test(tc_core, test_task_hosts_match);
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
  sr = srunner_create(u_misc_suite());
  srunner_set_log(sr, "u_misc_suite.log");
  srunner_run_all(sr, CK_NORMAL);
  number_failed = srunner_ntests_failed(sr);
  srunner_free(sr);
  return(number_failed);
  }
