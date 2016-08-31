#include "license_pbs.h" /* See here for the software license */
#include <pbs_config.h>
#include "mom_job_func.h"
#include "test_mom_job_func.h"
#include <stdlib.h>
#include <stdio.h>
#include <semaphore.h>

#include <set>
#include <list>

#include "mom_job_cleanup.h"


#include "pbs_error.h"

std::set<pid_t> global_job_sid_set;

bool am_i_mother_superior(const job &pjob);
void remove_from_exiting_list(job *pjob);
job *mom_find_job_by_int_string(const char *jobint_string);
job *mom_find_job(const char *jobid);
void remove_from_job_list(job *pjob);

std::vector<exiting_job_info> exiting_job_list;
extern std::list<job *> alljobs_list;

sem_t *delete_job_files_sem;

START_TEST(test_mom_finding_jobs)
  {
  job *pjob1 = job_alloc();
  job *pjob2 = job_alloc();
  job *pjob3 = job_alloc();
  job *pjob4 = job_alloc();

  const char *jobid1 = "1.napali";
  const char *jobid2 = "2.napali";
  const char *jobid3 = "3.napali";
  const char *jobid4 = "40.napali";

  strcpy(pjob1->ji_qs.ji_jobid, jobid1);
  strcpy(pjob2->ji_qs.ji_jobid, jobid2);
  strcpy(pjob3->ji_qs.ji_jobid, jobid3);
  strcpy(pjob4->ji_qs.ji_jobid, jobid4);

  alljobs_list.push_back(pjob1);
  alljobs_list.push_back(pjob2);
  alljobs_list.push_back(pjob3);
  alljobs_list.push_back(pjob4);

  fail_unless(mom_find_job(jobid1) == pjob1);
  fail_unless(mom_find_job(jobid2) == pjob2);
  fail_unless(mom_find_job(jobid3) == pjob3);
  fail_unless(mom_find_job(jobid4) == pjob4);
  fail_unless(mom_find_job("4.napali") == NULL);
  fail_unless(mom_find_job_by_int_string("1") == pjob1);
  fail_unless(mom_find_job_by_int_string("2") == pjob2);
  fail_unless(mom_find_job_by_int_string("3") == pjob3);
  fail_unless(mom_find_job_by_int_string("0") == NULL);
  fail_unless(mom_find_job_by_int_string("4") == NULL);
  fail_unless(mom_find_job_by_int_string("40") == pjob4);

  remove_from_job_list(pjob2);
  fail_unless(mom_find_job(jobid1) == pjob1);
  fail_unless(mom_find_job(jobid2) == NULL);
  fail_unless(mom_find_job(jobid3) == pjob3);
  fail_unless(mom_find_job(jobid4) == pjob4);
  fail_unless(mom_find_job("4.napali") == NULL);
  fail_unless(mom_find_job_by_int_string("1") == pjob1);
  fail_unless(mom_find_job_by_int_string("2") == NULL);
  fail_unless(mom_find_job_by_int_string("3") == pjob3);
  fail_unless(mom_find_job_by_int_string("0") == NULL);
  fail_unless(mom_find_job_by_int_string("4") == NULL);
  fail_unless(mom_find_job_by_int_string("40") == pjob4);

  // Make sure we'll find jobs that don't have a server suffix
  const char *jobid5 = "5";
  const char *jobid6 = "6";
  job *pjob5 = job_alloc();
  job *pjob6 = job_alloc();

  strcpy(pjob5->ji_qs.ji_jobid, jobid5);
  strcpy(pjob6->ji_qs.ji_jobid, jobid6);
  alljobs_list.push_back(pjob5);
  alljobs_list.push_back(pjob6);
  fail_unless(mom_find_job("5.roshar") == pjob5);
  fail_unless(mom_find_job("6.scadrial") == pjob6);
  fail_unless(mom_find_job(jobid5) == pjob5);
  fail_unless(mom_find_job(jobid6) == pjob6);
  }
END_TEST


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

  tc_core = tcase_create("test_mom_finding_jobs");
  tcase_add_test(tc_core, test_mom_finding_jobs);
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
