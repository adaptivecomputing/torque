#include "license_pbs.h" /* See here for the software license */
#include <pbs_config.h>
#include "mom_job_func.h"
#include "test_mom_job_func.h"
#include <stdlib.h>
#include <stdio.h>

#include <set>

#include "mom_job_cleanup.h"


#include "pbs_error.h"

std::set<pid_t> global_job_sid_set;

bool am_i_mother_superior(const job &pjob);
void remove_from_exiting_list(job *pjob);

std::vector<exiting_job_info> exiting_job_list;

START_TEST(test_am_i_mother_superior)
  {
  job pjob;

  memset(&pjob, 0, sizeof(pjob));
  fail_unless(am_i_mother_superior(pjob) == false);
  pjob.ji_qs.ji_svrflags |= JOB_SVFLG_HERE;
  fail_unless(am_i_mother_superior(pjob) == true);

  }
END_TEST

START_TEST(test_remove_from_exiting_list)
  {
  job pjob;

  exiting_job_list.push_back(exiting_job_info("0.napali"));
  exiting_job_list.push_back(exiting_job_info("1.napali"));

  // make sure there's no infinite loop if a job isn't in the list
  strcpy(pjob.ji_qs.ji_jobid, "2.napali");
  remove_from_exiting_list(&pjob);
  
  strcpy(pjob.ji_qs.ji_jobid, "1.napali");
  remove_from_exiting_list(&pjob);
  fail_unless(exiting_job_list.size() == 1);
  
  strcpy(pjob.ji_qs.ji_jobid, "0.napali");
  remove_from_exiting_list(&pjob);
  fail_unless(exiting_job_list.size() == 0);
  }
END_TEST

Suite *mom_job_func_suite(void)
  {
  Suite *s = suite_create("mom_job_func_suite methods");
  TCase *tc_core = tcase_create("test_am_i_mother_superior");
  tcase_add_test(tc_core, test_am_i_mother_superior);
  suite_add_tcase(s, tc_core);

  tc_core = tcase_create("test_remove_from_exiting_list");
  tcase_add_test(tc_core, test_remove_from_exiting_list);
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
  sr = srunner_create(mom_job_func_suite());
  srunner_set_log(sr, "mom_job_func_suite.log");
  srunner_run_all(sr, CK_NORMAL);
  number_failed = srunner_ntests_failed(sr);
  srunner_free(sr);
  return number_failed;
  }
