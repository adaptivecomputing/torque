#include <stdio.h>
#include <stdlib.h>
#include <check.h>

#include "mail_throttler.hpp"
#include "pbs_error.h"



START_TEST(test_adding)
  {
  mail_throttler mt;
  mail_info      mi;

  mi.mailto = "a";
  fail_unless(mt.add_email_entry(mi) == true);
  fail_unless(mt.add_email_entry(mi) == false);
  fail_unless(mt.add_email_entry(mi) == false);
  fail_unless(mt.add_email_entry(mi) == false);

  std::vector<mail_info> pending;
  fail_unless(mt.get_email_list(mi.mailto, pending) == PBSE_NONE);
  fail_unless(pending.size() == 4);
  fail_unless(mt.get_email_list(mi.mailto, pending) != PBSE_NONE);
  fail_unless(mt.get_email_list("b", pending) != PBSE_NONE);
  fail_unless(mt.get_email_list("c", pending) != PBSE_NONE);
  
  fail_unless(mt.add_email_entry(mi) == true);
  fail_unless(mt.add_email_entry(mi) == false);
  fail_unless(mt.add_email_entry(mi) == false);

  mi.mailto = "b";
  fail_unless(mt.add_email_entry(mi) == true);
  fail_unless(mt.add_email_entry(mi) == false);

  mi.mailto = "c";
  fail_unless(mt.add_email_entry(mi) == true);
  fail_unless(mt.add_email_entry(mi) == false);

  fail_unless(mt.get_email_list("d", pending) != PBSE_NONE);
  fail_unless(mt.get_email_list("3", pending) != PBSE_NONE);
  fail_unless(mt.get_email_list("a", pending) == PBSE_NONE);
  fail_unless(pending.size() == 3);
  fail_unless(mt.get_email_list("b", pending) == PBSE_NONE);
  fail_unless(pending.size() == 2);
  fail_unless(mt.get_email_list("c", pending) == PBSE_NONE);
  fail_unless(pending.size() == 2);
  fail_unless(mt.get_email_list("a", pending) != PBSE_NONE);
  fail_unless(mt.get_email_list("b", pending) != PBSE_NONE);
  fail_unless(mt.get_email_list("c", pending) != PBSE_NONE);
  }
END_TEST


START_TEST(test_two)
  {
  }
END_TEST


Suite *mail_throttler_suite(void)
  {
  Suite *s = suite_create("mail_throttler test suite methods");
  TCase *tc_core = tcase_create("test_adding");
  tcase_add_test(tc_core, test_adding);
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
  sr = srunner_create(mail_throttler_suite());
  srunner_set_log(sr, "mail_throttler_suite.log");
  srunner_run_all(sr, CK_NORMAL);
  number_failed = srunner_ntests_failed(sr);
  srunner_free(sr);
  return(number_failed);
  }

