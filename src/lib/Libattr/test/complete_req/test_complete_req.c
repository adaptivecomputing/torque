#include <stdio.h>
#include <stdlib.h>
#include <check.h>

#include "complete_req.hpp"


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
