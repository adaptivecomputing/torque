#include "license_pbs.h" /* See here for the software license */
#include "pbs_error.h"
#include <stdlib.h>
#include <stdio.h>
#include <string>
#include "pbs_error.h"
#include "pbs_job.h"
#include "test_accounting.h"


extern char *acct_file;
void add_procs_and_nodes_used(job &pjob, std::string &acct_data);
const char *exec1 = "napali/0+napali/1+napali/2+napali/3+napali/4+napali/5";
const char *exec2 = "2/0+2/1+2/2+2/3+3/0+3/1+3/2+3/3+4/0+4/1+4/2+4/3";
const char *exec3 = "2/0-31+3/0-31+4/0-31+5/0-31+6/0-31+7/0-31+8/0-31+9/0-31+10/0-31";
const char *exec4 = "napali";

START_TEST(test_add_procs_and_nodes_used)
  {
  job pjob;
  std::string used;

  pjob.ji_wattr[JOB_ATR_exec_host].at_val.at_str = strdup(exec1);
  add_procs_and_nodes_used(pjob, used);
  fail_unless(used == "total_execution_slots=6 unique_node_count=1 ", used.c_str());
  used.clear();
  
  pjob.ji_wattr[JOB_ATR_exec_host].at_val.at_str = strdup(exec2);
  add_procs_and_nodes_used(pjob, used);
  fail_unless(used == "total_execution_slots=12 unique_node_count=3 ", used.c_str());
  used.clear();
  
  pjob.ji_wattr[JOB_ATR_exec_host].at_val.at_str = strdup(exec3);
  add_procs_and_nodes_used(pjob, used);
  fail_unless(used == "total_execution_slots=288 unique_node_count=9 ", used.c_str());
  used.clear();

  pjob.ji_wattr[JOB_ATR_exec_host].at_val.at_str = strdup(exec4);
  add_procs_and_nodes_used(pjob, used);
  fail_unless(used == "total_execution_slots=1 unique_node_count=1 ", used.c_str());
  }
END_TEST

START_TEST(test_two)
  {


  }
END_TEST

Suite *accounting_suite(void)
  {
  Suite *s = suite_create("accounting_suite methods");
  TCase *tc_core = tcase_create("test_add_procs_and_nodes_used");
  tcase_add_test(tc_core, test_add_procs_and_nodes_used);
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
  sr = srunner_create(accounting_suite());
  srunner_set_log(sr, "accounting_suite.log");
  srunner_run_all(sr, CK_NORMAL);
  number_failed = srunner_ntests_failed(sr);
  srunner_free(sr);
  return number_failed;
  }
