#include <stdio.h>
#include <stdlib.h>
#include <check.h>

#include "delete_all_tracker.hpp"

START_TEST(test_basic_functionality)
  {
  delete_all_tracker dat;

  // nobody should be deleting to start
  fail_unless(dat.currently_deleting_all("dbeer", 0) == false);
  fail_unless(dat.currently_deleting_all("root", 0x20) == false);
  fail_unless(dat.start_deleting_all_if_possible("dbeer", 0) == true);
  fail_unless(dat.start_deleting_all_if_possible("root", 0x20) == true);
  fail_unless(dat.start_deleting_all_if_possible("dbeer", 0) == false);
  fail_unless(dat.start_deleting_all_if_possible("root", 0x20) == false);
  fail_unless(dat.currently_deleting_all("dbeer", 0) == true);
  fail_unless(dat.currently_deleting_all("root", 0x20) == true);
  dat.done_deleting_all("dbeer", 0);
  dat.done_deleting_all("root", 0x20);
  fail_unless(dat.currently_deleting_all("dbeer", 0) == false);
  fail_unless(dat.currently_deleting_all("root", 0x20) == false);
  }
END_TEST



Suite *delete_all_tracker_suite(void)
  {
  Suite *s = suite_create("delete_all_tracker test suite methods");
  TCase *tc_core = tcase_create("test_basic_functionality");
  tcase_add_test(tc_core, test_basic_functionality);
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
  sr = srunner_create(delete_all_tracker_suite());
  srunner_set_log(sr, "delete_all_tracker_suite.log");
  srunner_run_all(sr, CK_NORMAL);
  number_failed = srunner_ntests_failed(sr);
  srunner_free(sr);
  return(number_failed);
  }
