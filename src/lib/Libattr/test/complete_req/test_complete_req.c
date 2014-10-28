#include <stdio.h>
#include <stdlib.h>
#include <check.h>

#include "complete_req.hpp"
#include "pbs_error.h"


START_TEST(test_set_get_value)
  {
  complete_req c;

  fail_unless(c.set_value(1, "task_count", "5") == PBSE_NONE);
  fail_unless(c.set_value(0, "task_count", "4") == PBSE_NONE);
  fail_unless(c.set_value(0, "lprocs", "4") == PBSE_NONE);
  fail_unless(c.set_value(1, "gpus", "2") == PBSE_NONE);
  fail_unless(c.set_value(-1, "blah", "blah") == PBSE_BAD_PARAMETER);
  fail_unless(c.req_count() == 2);

  std::vector<std::string> names;
  std::vector<std::string> values;

  c.get_values(names, values);

  fail_unless(names[0] == "task_count.0");
  fail_unless(names[1] == "lprocs.0");
  fail_unless(names[2] == "thread_usage_policy.0");
  fail_unless(names[3] == "task_count.1", names[3].c_str());
  fail_unless(names[4] == "lprocs.1", names[4].c_str());
  fail_unless(names[5] == "gpus.1", names[4].c_str());
  fail_unless(names[6] == "thread_usage_policy.1");
  }
END_TEST


START_TEST(test_constructor)
  {
  complete_req c;

  req r1;
  req r2;

  r1.set_from_string("req[1]\ntask count: 6\nlprocs: all\n thread usage policy: use threads\nplacement type: place numa");
  r2.set_from_string("req[1]\ntask count: 8\nlprocs: 4\n thread usage policy: use cores\nplacement type: place socket");

  c.add_req(r1);
  c.add_req(r2);

  complete_req copy(c);

  fail_unless(copy.req_count() == 2);
  
  std::string c_out;
  std::string copy_out;

  c.toString(c_out);
  copy.toString(copy_out);

  fail_unless(c_out == copy_out);

  complete_req equals;
  equals = c;

  fail_unless(equals.req_count() == 2);

  std::string equals_out;
  equals.toString(equals_out);

  fail_unless(equals_out == c_out);

  }
END_TEST



START_TEST(test_to_string)
  {
  complete_req c;

  req r1;
  req r2;

  r1.set_from_string("req[1]\ntask count: 6\nlprocs: all\n thread usage policy: use threads\nplacement type: place numa");
  r2.set_from_string("req[1]\ntask count: 8\nlprocs: 4\n thread usage policy: use cores\nplacement type: place socket");

  c.add_req(r1);
  c.add_req(r2);

  fail_unless(c.req_count() == 2);

  std::string out_check;
  std::string out_master;

  r1.toString(out_master);
  out_master += '\n';
  r2.toString(out_master);
  c.toString(out_check);

  fail_unless(out_master == out_check, "master: \n%s \n!= produced: \n%s", out_master.c_str(),
      out_check.c_str());

  complete_req c2;

  c2.set_from_string(out_check);

  std::string c2_out;

  c2.toString(c2_out);

  fail_unless(c2_out == out_master, "master: \n%s \n!= produced: \n%s", out_master.c_str(), 
      c2_out.c_str());
  }
END_TEST



Suite *complete_req_suite(void)
  {
  Suite *s = suite_create("complete_req test suite methods");
  TCase *tc_core = tcase_create("test_constructor");
  tcase_add_test(tc_core, test_constructor);
  suite_add_tcase(s, tc_core);
  
  tc_core = tcase_create("test_to_string");
  tcase_add_test(tc_core, test_to_string);
  tcase_add_test(tc_core, test_set_get_value);
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
  sr = srunner_create(complete_req_suite());
  srunner_set_log(sr, "complete_req_suite.log");
  srunner_run_all(sr, CK_NORMAL);
  number_failed = srunner_ntests_failed(sr);
  srunner_free(sr);
  return(number_failed);
  }
