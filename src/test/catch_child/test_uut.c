#include <pbs_config.h>
#include "utils.h"
#include <errno.h>
#include "pbs_job.h"
#include "test_catch_child.h"
#include "mom_job_cleanup.h"


void catch_child(int sig);
bool eligible_for_exiting_check(job *pjob);
void check_jobs_main_process(job *pjob, task *ptask);
bool non_mother_superior_cleanup(job *pjob);
bool mother_superior_cleanup(job *pjob, int limit, int *found);
void *obit_reply(void *new_sock);
hnodent *get_node(job *pjob, tm_node_id nodeid);

extern int termin_child;
extern int server_down;
extern int exiting_tasks;
extern int  DIS_reply_read_count;
extern int  tc;
extern int  called_open_socket;
extern int  called_fork_me;
extern bool eintr_test;
extern std::vector<exiting_job_info> exiting_job_list;


/*
 * Current test cases consist of:
 * Not found (not jobs)
 * Not found (multiple jobs)
 * Found (multiple jobs)
 */

void init_job(job *pjob)
  {
  memset(pjob, 0, sizeof(job));
  pjob->ji_numvnod = 2;
  pjob->ji_vnods = (vnodent *)calloc(pjob->ji_numvnod, sizeof(vnodent));
  pjob->ji_vnods[0].vn_node = 1;
  pjob->ji_vnods[0].vn_host = (hnodent *)calloc(sizeof(hnodent), 1);
  pjob->ji_vnods[0].vn_host->hn_node = 1;
  pjob->ji_vnods[1].vn_node = 2;
  pjob->ji_vnods[1].vn_host = (hnodent *)calloc(sizeof(hnodent), 1);
  pjob->ji_vnods[1].vn_host->hn_node = 2;
  }

void cleanup_job(job *pjob)
  {
  free(pjob->ji_vnods[0].vn_host);
  free(pjob->ji_vnods[1].vn_host);
  free(pjob->ji_vnods);
  pjob->ji_vnods = NULL;
  }

START_TEST(test_get_node_1)
  {
  job pjob;
  tm_node_id nodeid = 1;
  hnodent *res;
  init_job(&pjob);
  pjob.ji_numvnod = 0;
  res = get_node(&pjob, nodeid);
  fail_unless(res == NULL, "No data passed in, nothing should be found");
  cleanup_job(&pjob);
  }
END_TEST

START_TEST(test_get_node_2)
  {
  hnodent *res;
  job pjob;
  tm_node_id nodeid = 3;
  init_job(&pjob);
  res = get_node(&pjob, nodeid);
  fail_unless(res == NULL, "Node should not have been found");
  cleanup_job(&pjob);
  }
END_TEST

START_TEST(test_get_node_3)
  {
  job pjob;
  tm_node_id nodeid = 2;
  hnodent *res;
  init_job(&pjob);
  res = get_node(&pjob, nodeid);
  fail_if(res == NULL, "A valid node should have been returned");
  fail_unless(res->hn_node == 2, "Incorrect node was returned");
  cleanup_job(&pjob);
  }
END_TEST


START_TEST(obit_reply_test)
  {
  int sock = 1;
  DIS_reply_read_count = 0;
  
  eintr_test = true;
  obit_reply(&sock);
  fail_unless(DIS_reply_read_count == 11);
  eintr_test = false;
  }
END_TEST

START_TEST(test_catch_child_1)
  {
  termin_child = 0;
  catch_child(2);
  fail_unless(termin_child == 1, "termin_child is set to 1 by this method");
  }
END_TEST

START_TEST(test_eligible_for_exiting_check)
  {
  job *pjob = (job *)calloc(1, sizeof(job));
  server_down = TRUE;

  fail_unless(eligible_for_exiting_check(pjob) == false);
  server_down = FALSE;
  fail_unless(eligible_for_exiting_check(pjob) == false);
  pjob->ji_wattr[JOB_ATR_Cookie].at_flags |= ATR_VFLAG_SET;
  fail_unless(eligible_for_exiting_check(pjob) == true);

  pjob->ji_flags |= MOM_CHECKPOINT_POST;
  fail_unless(eligible_for_exiting_check(pjob) == false);
  
  pjob->ji_flags |= MOM_CHECKPOINT_ACTIVE;
  fail_unless(eligible_for_exiting_check(pjob) == false);

  pjob->ji_flags = 0;
  fail_unless(eligible_for_exiting_check(pjob) == true);
  }
END_TEST

START_TEST(test_jobs_main_process)
  {
  job *pjob = (job *)calloc(1, sizeof(job));
  task ptask;

  pjob->ji_wattr[JOB_ATR_job_radix].at_val.at_long = 2;
  check_jobs_main_process(pjob, &ptask);
  fail_unless(pjob->ji_qs.ji_substate != JOB_SUBSTATE_EXITING);
  
  pjob->ji_sampletim -= 500;
  check_jobs_main_process(pjob, &ptask);
  fail_unless(pjob->ji_qs.ji_substate != JOB_SUBSTATE_EXITING);
  }
END_TEST

START_TEST(test_non_mother_superior_cleanup)
  {
  job *pjob = (job *)calloc(1, sizeof(job));

  pjob->ji_qs.ji_svrflags |= JOB_SVFLG_HERE;

  fail_unless(non_mother_superior_cleanup(pjob) == true);

  pjob->ji_qs.ji_svrflags = 0;
  pjob->ji_qs.ji_svrflags |= JOB_SVFLG_INTERMEDIATE_MOM;
  fail_unless(non_mother_superior_cleanup(pjob) == true);

  pjob->ji_qs.ji_substate = JOB_SUBSTATE_NOTERM_REQUE;
  fail_unless(non_mother_superior_cleanup(pjob) == true);
  fail_unless(exiting_tasks == 1);
  
  pjob->ji_qs.ji_substate = JOB_SUBSTATE_EXITING;
  fail_unless(non_mother_superior_cleanup(pjob) == true);
  
  pjob->ji_qs.ji_svrflags = 0;
  fail_unless(non_mother_superior_cleanup(pjob) == true);
  }
END_TEST

START_TEST(test_mother_superior_cleanup)
  {
  job *pjob = (job *)calloc(1, sizeof(job));
  sprintf(pjob->ji_qs.ji_jobid, "1.napali");
  int  found_one = 1;

  fail_unless(mother_superior_cleanup(pjob, 10, &found_one) == false);

  pjob->ji_qs.ji_substate = JOB_SUBSTATE_EXIT_WAIT;
  fail_unless(mother_superior_cleanup(pjob, 1, &found_one) == true);

  exiting_job_list.clear();
  pjob->ji_numnodes = 2;
  pjob->ji_hosts[1].hn_sister = SISTER_KILLDONE;
  pjob->ji_flags = MOM_EPILOGUE_RUN;
  fail_unless(mother_superior_cleanup(pjob, 1, &found_one) == true);
  fail_unless(exiting_job_list.size() == 1);
  fail_unless(exiting_job_list[0].jobid == "1.napali");
  
  called_fork_me = 0;
  pjob->ji_flags = 0;
  fail_unless(mother_superior_cleanup(pjob, 1, &found_one) == true);
  fail_unless(called_open_socket == 1);
  fail_unless(called_fork_me == 1);
  }
END_TEST

Suite *catch_child_suite(void)
  {
  Suite *s = suite_create("catch_child methods");
  TCase *tc_core = tcase_create("Core");
  tcase_add_test(tc_core, test_catch_child_1);
  tcase_add_test(tc_core, obit_reply_test);
  tcase_add_test(tc_core, test_eligible_for_exiting_check);
  tcase_add_test(tc_core, test_jobs_main_process);
  tcase_add_test(tc_core, test_non_mother_superior_cleanup);
  tcase_add_test(tc_core, test_mother_superior_cleanup);
  
  s = suite_create("get_node_tests");
  tcase_add_test(tc_core, test_get_node_1);
  tcase_add_test(tc_core, test_get_node_2);
  tcase_add_test(tc_core, test_get_node_3);
  suite_add_tcase(s, tc_core);
  return s;
  }

int main(void)
  {
  int number_failed = 0;
  SRunner *sr = NULL;
  sr = srunner_create(catch_child_suite());
  srunner_set_log(sr, "catch_child_suite.log");
  srunner_run_all(sr, CK_NORMAL);
  number_failed = srunner_ntests_failed(sr);
  srunner_free(sr);
  return number_failed;
  }
