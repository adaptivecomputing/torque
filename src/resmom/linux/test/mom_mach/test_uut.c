#include "license_pbs.h" /* See here for the software license */
#include "mom_mach.h"
#include "test_uut.h"
#include <stdlib.h>
#include <stdio.h>
#include <set>
#include "node_frequency.hpp"

#include "pbs_error.h"

using namespace std;

bool injob(job *, pid_t, struct pidl **);

extern node_frequency nd_frequency;

void skip_space_delimited_values(int number_to_skip, char **str_to_advance);

START_TEST(test_skip_space_delimited_values)
  {
  char *str = strdup("a b c d e f g h i j k l");

  skip_space_delimited_values(1, &str);
  fail_unless(*str == 'b');

  skip_space_delimited_values(1, &str);
  fail_unless(*str == 'c');

  skip_space_delimited_values(1, &str);
  fail_unless(*str == 'd');

  skip_space_delimited_values(5, &str);
  fail_unless(*str == 'i');

  skip_space_delimited_values(5, &str);
  fail_unless(str == NULL);
  }
END_TEST

START_TEST(test_injob)
  {
  job myjob;
  pid_t job_pid;
  pid_t bad_job_pid;
  bool rc;


  myjob.ji_job_procs = new(std::set<pid_t>);

  job_pid = 1234;
  bad_job_pid = 5678;
  myjob.ji_job_procs->insert(job_pid);
  rc = injob(&myjob, job_pid, NULL);
  fail_unless(rc != true, "failed to find pid in job");

  rc = injob(&myjob, bad_job_pid, NULL);
  fail_unless(rc != false, "failed to detect pid not in job");

  }
END_TEST


START_TEST(test_two)
  {
  }
END_TEST

Suite *mom_mach_suite(void)
  {
  Suite *s = suite_create("mom_mach_suite methods");
  TCase *tc_core = tcase_create("test_skip_space_delimited_values");
  tcase_add_test(tc_core, test_skip_space_delimited_values);
  suite_add_tcase(s, tc_core);

  tc_core = tcase_create("test_injob");
  tcase_add_test(tc_core, test_injob);
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
  sr = srunner_create(mom_mach_suite());
  srunner_set_log(sr, "mom_mach_suite.log");
  srunner_run_all(sr, CK_NORMAL);
  number_failed = srunner_ntests_failed(sr);
  srunner_free(sr);
  return number_failed;
  }
