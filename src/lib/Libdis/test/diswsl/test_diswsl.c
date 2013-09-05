#include "license_pbs.h" /* See here for the software license */
#include "dis_internal.h"
#include "test_diswsl.h"
#include <stdlib.h>
#include <stdio.h>
#include <string>


#include "pbs_error.h"

extern std::string output;

START_TEST(test_one)
  {
  struct tcp_chan chan;

  output = "";
  diswsl(&chan,5);
  fail_unless(strcmp(output.c_str(),"+5") == 0,"Incorrectly encoded.");
  output = "";
  diswsl(&chan,327);
  fail_unless(strcmp(output.c_str(),"3+327") == 0,"Incorrectly encoded.");
  output = "";
  diswsl(&chan,-200202020);
  fail_unless(strcmp(output.c_str(),"9-200202020") == 0,"Incorrectly encoded.");
  output = "";
  diswsl(&chan,-0);
  fail_unless(strcmp(output.c_str(),"+0") == 0,"Incorrectly encoded.");
  }
END_TEST

START_TEST(test_two)
  {


  }
END_TEST

Suite *diswsl_suite(void)
  {
  Suite *s = suite_create("diswsl_suite methods");
  TCase *tc_core = tcase_create("test_one");
  tcase_add_test(tc_core, test_one);
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
  sr = srunner_create(diswsl_suite());
  srunner_set_log(sr, "diswsl_suite.log");
  srunner_run_all(sr, CK_NORMAL);
  number_failed = srunner_ntests_failed(sr);
  srunner_free(sr);
  return number_failed;
  }
