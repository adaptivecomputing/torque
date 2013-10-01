#include "license_pbs.h" /* See here for the software license */
#include "svr_jobfunc.h"
#include "pbs_job.h"
#include "server.h"
#include "queue.h"
#include "attribute.h"
#include "test_svr_jobfunc.h"
#include <stdlib.h>
#include <stdio.h>
#include "pbs_error.h"

int lock_ji_mutex(job *pjob, const char *id, const char *msg, int logging);
int chk_mppnodect(resource *mppnodect, pbs_queue *pque, long nppn, long mpp_width, char *EMsg);

extern int decrement_count;

START_TEST(chk_mppnodect_test)
  {
  }
END_TEST

START_TEST(svr_enquejob_test)
  {
  struct job test_job;
  int result = PBSE_NONE;

  /*initialize_globals*/
  server.sv_qs_mutex = (pthread_mutex_t *)calloc(1, sizeof(pthread_mutex_t));
  server.sv_attr_mutex = (pthread_mutex_t *)calloc(1, sizeof(pthread_mutex_t));
  server.sv_jobstates_mutex = (pthread_mutex_t *)calloc(1, sizeof(pthread_mutex_t));

  pthread_mutex_init(server.sv_qs_mutex,NULL);
  pthread_mutex_init(server.sv_attr_mutex,NULL);
  pthread_mutex_init(server.sv_jobstates_mutex,NULL);

  memset(&test_job, 0, sizeof(test_job));

  result = svr_enquejob(NULL, 0, 0, false);
  fail_unless(result != PBSE_NONE, "NULL input pointer fail");

  result = svr_enquejob(&test_job, 0, 0, false);
  /*Need more complicated mocking in order to have other result than PBSE_JOBNOTFOUND*/
  fail_unless(result == PBSE_JOBNOTFOUND, "svr_enquejob fail: %d", result);

  }
END_TEST

START_TEST(svr_dequejob_test)
  {
  int result = PBSE_NONE;
  job j;

  memset(&j, 0, sizeof(job));

  result = svr_dequejob(NULL, 0);
  fail_unless(result == PBSE_BAD_PARAMETER, "NULL input pointer fail");

  result = svr_dequejob(&j, 0);
  fail_unless(result == PBSE_JOBNOTFOUND, "svr_dequejob fail");

  j.ji_qs.ji_state = JOB_STATE_RUNNING;
  fail_unless(svr_dequejob(&j, 0) == PBSE_BADSTATE);
  }
END_TEST

START_TEST(svr_setjobstate_test)
  {
  struct job test_job;
  int result = PBSE_NONE;

  memset(&test_job, 0, sizeof(test_job));

  /*initialize_globals*/
  server.sv_qs_mutex = (pthread_mutex_t *)calloc(1, sizeof(pthread_mutex_t));
  server.sv_attr_mutex = (pthread_mutex_t *)calloc(1, sizeof(pthread_mutex_t));
  server.sv_jobstates_mutex = (pthread_mutex_t *)calloc(1, sizeof(pthread_mutex_t));

  pthread_mutex_init(server.sv_qs_mutex,NULL);
  pthread_mutex_init(server.sv_attr_mutex,NULL);
  pthread_mutex_init(server.sv_jobstates_mutex,NULL);

  result = svr_setjobstate(NULL, 0, 0, 0);
  fail_unless(result == PBSE_BAD_PARAMETER, "NULL input pointer fail");

  result = svr_setjobstate(&test_job, 0, 0, 0);
  fail_unless(result == PBSE_NONE, "svr_setjobstate fail");

  result = svr_setjobstate(&test_job, 1, 2, 3);
  fail_unless(result == PBSE_NONE, "svr_setjobstate fail");

  test_job.ji_qs.ji_state = JOB_STATE_RUNNING;
  test_job.ji_wattr[JOB_ATR_exec_host].at_val.at_str = strdup("napali/0");
  fail_unless(svr_setjobstate(&test_job, JOB_STATE_QUEUED, JOB_SUBSTATE_QUEUED, FALSE) == PBSE_NONE);
  fail_unless(test_job.ji_wattr[JOB_ATR_exec_host].at_val.at_str == NULL);

  decrement_count = 0;
  fail_unless(svr_setjobstate(&test_job, JOB_STATE_COMPLETE, JOB_SUBSTATE_COMPLETE, FALSE) == PBSE_NONE);
  fail_unless(decrement_count == 2);

  decrement_count = 0;
  fail_unless(svr_setjobstate(&test_job, JOB_STATE_COMPLETE, JOB_SUBSTATE_COMPLETE, FALSE) == PBSE_NONE);
  fail_unless(decrement_count == 0);
  }
END_TEST

START_TEST(svr_evaljobstate_test)
  {
  struct job test_job;
  int state = 0;
  int substate = 0;

  memset(&test_job, 0, sizeof(test_job));

  svr_evaljobstate(NULL, &state, &substate, 0);
  svr_evaljobstate(&test_job, NULL, &substate, 0);
  svr_evaljobstate(&test_job, &state, NULL, 0);

  test_job.ji_qs.ji_state = JOB_STATE_RUNNING;
  svr_evaljobstate(&test_job, &state, &substate, 0);
  fail_unless(test_job.ji_qs.ji_state == state, "svr_setjobstate state fail case 1");
  fail_unless(test_job.ji_qs.ji_substate == substate, "svr_setjobstate substate fail case 1");
  memset(&test_job, 0, sizeof(test_job));

  test_job.ji_wattr[JOB_ATR_hold].at_val.at_long = 1;
  svr_evaljobstate(&test_job, &state, &substate, 0);
  fail_unless(test_job.ji_qs.ji_state == state, "svr_setjobstate state fail case 2");
  fail_unless(test_job.ji_qs.ji_substate == substate, "svr_setjobstate substate fail case 2");
  memset(&test_job, 0, sizeof(test_job));

  test_job.ji_wattr[JOB_ATR_stagein].at_flags = 1;
  svr_evaljobstate(&test_job, &state, &substate, 0);
  fail_unless(test_job.ji_qs.ji_state == state, "svr_setjobstate state fail case 3");
  fail_unless(test_job.ji_qs.ji_substate == substate, "svr_setjobstate substate fail case 3");
  memset(&test_job, 0, sizeof(test_job));

  svr_evaljobstate(&test_job, &state, &substate, 0);
  fail_unless(test_job.ji_qs.ji_state == state, "svr_setjobstate state fail case 4");
  fail_unless(test_job.ji_qs.ji_substate == substate, "svr_setjobstate substate fail case 4");
  memset(&test_job, 0, sizeof(test_job));

  svr_evaljobstate(&test_job, &state, &substate, 1);
  fail_unless(JOB_STATE_QUEUED == state, "svr_setjobstate state fail case 5");
  fail_unless(JOB_SUBSTATE_QUEUED == substate, "svr_setjobstate substate fail case 5");

  }
END_TEST

START_TEST(get_variable_test)
  {
  struct job test_job;
  char* variable = (char *)"variable";
  char* result = NULL;

  memset(&test_job, 0, sizeof(test_job));

  result = get_variable(NULL, variable);
  fail_unless(result == NULL, "NULL input job pointer fail");

  result = get_variable(&test_job, NULL);
  fail_unless(result == NULL, "NULL input variable string pointer fail");

  result = get_variable(&test_job, variable);
  fail_unless(result == NULL, "svr_setjobstate fail");

  }
END_TEST

START_TEST(chk_resc_limits_test)
  {
  struct pbs_attribute test_attribute;
  struct pbs_queue test_queue;
  char message[] = "message";
  int result = -1;

  /*initialize_globals*/
  server.sv_qs_mutex = (pthread_mutex_t *)calloc(1, sizeof(pthread_mutex_t));
  server.sv_attr_mutex = (pthread_mutex_t *)calloc(1, sizeof(pthread_mutex_t));
  server.sv_jobstates_mutex = (pthread_mutex_t *)calloc(1, sizeof(pthread_mutex_t));

  pthread_mutex_init(server.sv_qs_mutex,NULL);
  pthread_mutex_init(server.sv_attr_mutex,NULL);
  pthread_mutex_init(server.sv_jobstates_mutex,NULL);

  memset(&test_attribute, 0, sizeof(test_attribute));
  memset(&test_queue, 0, sizeof(test_queue));

  result = chk_resc_limits(NULL, &test_queue, message);
  fail_unless(result == PBSE_BAD_PARAMETER, "NULL input pbs_attribute pointer fail");

  result = chk_resc_limits(&test_attribute, NULL, message);
  fail_unless(result == PBSE_BAD_PARAMETER, "NULL input pbs_queue pointer fail");

  result = chk_resc_limits(&test_attribute, &test_queue, NULL);
  fail_unless(result == PBSE_NONE, "chk_resc_limits with NULL message pointer fail");

  result = chk_resc_limits(&test_attribute, &test_queue, message);
  fail_unless(result == PBSE_NONE, "chk_resc_limits fail");

  }
END_TEST

START_TEST(svr_chkque_test)
  {
  struct job test_job;
  struct pbs_queue test_queue;
  struct array_strings disallowed_types_array_strings;
  char* hostname = (char *)"hostname";
  int result = -1;
  char* some_string = (char *)"some_string";

  /*initialize_globals*/
  server.sv_qs_mutex = (pthread_mutex_t *)calloc(1, sizeof(pthread_mutex_t));
  server.sv_attr_mutex = (pthread_mutex_t *)calloc(1, sizeof(pthread_mutex_t));
  server.sv_jobstates_mutex = (pthread_mutex_t *)calloc(1, sizeof(pthread_mutex_t));

  pthread_mutex_init(server.sv_qs_mutex,NULL);
  pthread_mutex_init(server.sv_attr_mutex,NULL);
  pthread_mutex_init(server.sv_jobstates_mutex,NULL);

  memset(&test_job, 0, sizeof(test_job));
  memset(&test_queue, 0, sizeof(test_queue));

  result = svr_chkque(NULL, &test_queue, hostname, 0, NULL);
  fail_unless(result == PBSE_BAD_PARAMETER, "NULL input job pointer fail");


  memset(&test_queue, 0, sizeof(test_queue));
  result = svr_chkque(&test_job, NULL, hostname, 0, NULL);
  result = svr_chkque(&test_job, NULL, hostname, 0, NULL);
  fail_unless(result == PBSE_BAD_PARAMETER, "NULL input pbs_queue pointer fail");

  result = svr_chkque(&test_job, &test_queue, NULL, 0, NULL);
  fail_unless(result != PBSE_NONE, "NULL input hostname pointer fail");

  test_queue.qu_qs.qu_type = QTYPE_Execution;
  result = svr_chkque(&test_job, &test_queue, hostname, 0, NULL);
  fail_unless(result == PBSE_QUNOENB, "svr_chkque fail");

  test_queue.qu_attr[QA_ATR_Enabled].at_val.at_long = 1;
  result = svr_chkque(&test_job, &test_queue, hostname, 0, NULL);
  fail_unless(result == PBSE_BAD_PARAMETER, "svr_chkque fail");

  test_queue.qu_attr[QA_ATR_AclGroupEnabled].at_val.at_long = 1;
  result = svr_chkque(&test_job, &test_queue, hostname, 0, NULL);
  fail_unless(result != PBSE_NONE, "svr_chkque fail");


  /* several test cases for check_queue_disallowed_types (not all)*/
  memset(&test_job, 0, sizeof(test_job));
  memset(&test_queue, 0, sizeof(test_queue));
  memset(&disallowed_types_array_strings,
         0,
         sizeof(disallowed_types_array_strings));
  test_queue.qu_qs.qu_type = QTYPE_Execution;
  test_queue.qu_attr[QA_ATR_DisallowedTypes].at_flags = ATR_VFLAG_SET;
  test_queue.qu_attr[QA_ATR_DisallowedTypes].at_val.at_arst = &disallowed_types_array_strings;
  disallowed_types_array_strings.as_string[0] = some_string;
  disallowed_types_array_strings.as_usedptr = 1;

  result = svr_chkque(&test_job, &test_queue, hostname, 0, NULL);
  fail_unless(result == PBSE_QUNOENB, "svr_chkque some_string fail");

  disallowed_types_array_strings.as_string[0] = (char *)Q_DT_batch;
  result = svr_chkque(&test_job, &test_queue, hostname, 0, NULL);
  fail_unless(result == PBSE_NOBATCH, "svr_chkque PBSE_NOBATCH fail");

  disallowed_types_array_strings.as_string[0] = (char *)Q_DT_rerunable;
  test_job.ji_wattr[JOB_ATR_rerunable].at_flags = ATR_VFLAG_SET;
  test_job.ji_wattr[JOB_ATR_rerunable].at_val.at_long = 1;
  result = svr_chkque(&test_job, &test_queue, hostname, 0, NULL);
  fail_unless(result == PBSE_NORERUNABLE, "svr_chkque PBSE_NORERUNABLE fail");

  disallowed_types_array_strings.as_string[0] = (char *)Q_DT_nonrerunable;
  test_job.ji_wattr[JOB_ATR_rerunable].at_val.at_long = 0;
  result = svr_chkque(&test_job, &test_queue, hostname, 0, NULL);
  fail_unless(result == PBSE_NONONRERUNABLE, "svr_chkque PBSE_NONONRERUNABLE fail");

  test_queue.qu_attr[QA_ATR_DisallowedTypes].at_val.at_arst = NULL;
  result = svr_chkque(&test_job, &test_queue, hostname, 0, NULL);
  fail_unless(result == PBSE_QUNOENB, "svr_chkque some_string fail");


  /* must reallocate as_string for this to work.
  disallowed_types_array_strings.as_usedptr = 2;
  disallowed_types_array_strings.as_string[0] = some_string;
  disallowed_types_array_strings.as_string[1] = (char *)Q_DT_fault_intolerant;
  result = svr_chkque(&test_job, &test_queue, hostname, 0, NULL);
  fail_unless(result == PBSE_NOFAULTINTOLERANT, "svr_chkque PBSE_NOFAULTINTOLERANT fail");
  */
  }
END_TEST

START_TEST(job_set_wait_test)
  {
  struct job test_job;
  struct pbs_attribute test_attribute;
  int result = -1;

  memset(&test_job, 0, sizeof(test_job));
  memset(&test_attribute, 0, sizeof(test_attribute));

  result = job_set_wait(NULL, &test_job, 0);
  fail_unless(result == PBSE_BAD_PARAMETER, "NULL input attribute pointer fail");

  result = job_set_wait(&test_attribute, NULL, 0);
  fail_unless(result == PBSE_BAD_PARAMETER, "NULL input job pointer fail");

  result = job_set_wait(&test_attribute, &test_job, 0);
  fail_unless(result == PBSE_NONE, "job_set_wait fail");
  }
END_TEST

START_TEST(prefix_std_file_test)
  {
  struct job test_job;
  struct dynamic_string test_string;
  char *result = NULL;

  memset(&test_job, 0, sizeof(test_job));
  memset(&test_string, 0, sizeof(test_string));

  result = prefix_std_file(NULL, &test_string, 0);
  fail_unless(result == NULL, "NULL input job pointer fail");

  result = prefix_std_file(&test_job, NULL, 0);
  fail_unless(result == NULL, "NULL input dynamic_string pointer fail");

  result = prefix_std_file(&test_job, &test_string, 0);
  fail_unless(result == NULL, "prefix_std_file fail");

  }
END_TEST

START_TEST(add_std_filename_test)
  {
  struct job test_job;
  char* path = (char *)"path";
  struct dynamic_string test_string;
  char* result = NULL;
  char* at_str = (char *)"string";

  memset(&test_job, 0, sizeof(test_job));
  memset(&test_string, 0, sizeof(test_string));
  test_job.ji_wattr[JOB_ATR_jobname].at_val.at_str = at_str;

  result = add_std_filename(NULL, path, 0, &test_string);
  fail_unless(result == NULL, "NULL input job pointer fail");

  result = add_std_filename(&test_job, NULL, 0, &test_string);
  fail_unless(result == NULL, "NULL input path pointer fail");

  result = add_std_filename(&test_job, path, 0, NULL);
  fail_unless(result == NULL, "NULL input dynamic_string pointer fail");

  result = add_std_filename(&test_job, path, 0, &test_string);
  fail_unless(result == NULL, "add_std_filename fail");

  }
END_TEST

START_TEST(get_jobowner_test)
  {
  char* from = (char *)"owner@host";
  char to[PBS_MAXUSER+1];

  memset(to, 0, sizeof(to));

  get_jobowner(NULL, to);
  get_jobowner(from, NULL);

  get_jobowner(from, to);
  fail_unless(strcmp(to,"owner") == 0, "get_jobowner fail");

  }
END_TEST

START_TEST(set_resc_deflt_test)
  {
  struct job test_job;
  struct pbs_attribute test_attribute;

  /*initialize_globals*/
  server.sv_qs_mutex = (pthread_mutex_t *)calloc(1, sizeof(pthread_mutex_t));
  server.sv_attr_mutex = (pthread_mutex_t *)calloc(1, sizeof(pthread_mutex_t));
  server.sv_jobstates_mutex = (pthread_mutex_t *)calloc(1, sizeof(pthread_mutex_t));

  pthread_mutex_init(server.sv_qs_mutex,NULL);
  pthread_mutex_init(server.sv_attr_mutex,NULL);
  pthread_mutex_init(server.sv_jobstates_mutex,NULL);

  memset(&test_job, 0, sizeof(test_job));
  memset(&test_attribute, 0, sizeof(test_attribute));

  set_resc_deflt(NULL, &test_attribute, 0);
  set_resc_deflt(&test_job, NULL, 0);
  set_resc_deflt(&test_job, &test_attribute, 0);
  set_resc_deflt(&test_job, &test_attribute, 1);
  }
END_TEST

START_TEST(set_chkpt_deflt_test)
  {
  struct job test_job;
  struct pbs_queue test_queue;

  memset(&test_job, 0, sizeof(test_job));
  memset(&test_queue, 0, sizeof(test_queue));

  set_chkpt_deflt(NULL, &test_queue);
  set_chkpt_deflt(&test_job, NULL);
  set_chkpt_deflt(&test_job, &test_queue);
  }
END_TEST

START_TEST(set_statechar_test)
  {
  struct job test_job;

  memset(&test_job, 0, sizeof(test_job));

  set_statechar(NULL);
  set_statechar(&test_job);
  }
END_TEST

START_TEST(lock_ji_mutex_test)
  {
  int        rc;
  job        pjob;
  const char *id;
  const char *msg;
  int        logging;

  pjob.ji_mutex = (pthread_mutex_t *)calloc(1, sizeof(pthread_mutex_t));
  pthread_mutex_init(pjob.ji_mutex, NULL);
  id = "lock unit test";
  msg = "locking";
  logging = 10;

  rc = lock_ji_mutex(&pjob, id, msg, logging);
  fail_unless(rc == 0, "did not lock mutex");

  }
END_TEST

Suite *svr_jobfunc_suite(void)
  {
  Suite *s = suite_create("svr_jobfunc_suite methods");
  TCase *tc_core = tcase_create("svr_enquejob_test");
  tcase_add_test(tc_core, svr_enquejob_test);
  suite_add_tcase(s, tc_core);

  tc_core = tcase_create("svr_dequejob_test");
  tcase_add_test(tc_core, svr_dequejob_test);
  suite_add_tcase(s, tc_core);

  tc_core = tcase_create("svr_setjobstate_test");
  tcase_add_test(tc_core, svr_setjobstate_test);
  suite_add_tcase(s, tc_core);

  tc_core = tcase_create("svr_evaljobstate_test");
  tcase_add_test(tc_core, svr_evaljobstate_test);
  suite_add_tcase(s, tc_core);

  tc_core = tcase_create("get_variable_test");
  tcase_add_test(tc_core, get_variable_test);
  suite_add_tcase(s, tc_core);

  tc_core = tcase_create("chk_resc_limits_test");
  tcase_add_test(tc_core, chk_resc_limits_test);
  suite_add_tcase(s, tc_core);

  tc_core = tcase_create("svr_chkque_test");
  tcase_add_test(tc_core, svr_chkque_test);
  suite_add_tcase(s, tc_core);

  tc_core = tcase_create("job_set_wait_test");
  tcase_add_test(tc_core, job_set_wait_test);
  suite_add_tcase(s, tc_core);

  tc_core = tcase_create("prefix_std_file_test");
  tcase_add_test(tc_core, prefix_std_file_test);
  suite_add_tcase(s, tc_core);

  tc_core = tcase_create("add_std_filename_test");
  tcase_add_test(tc_core, add_std_filename_test);
  suite_add_tcase(s, tc_core);

  tc_core = tcase_create("get_jobowner_test");
  tcase_add_test(tc_core, get_jobowner_test);
  suite_add_tcase(s, tc_core);

  tc_core = tcase_create("set_resc_deflt_test");
  tcase_add_test(tc_core, set_resc_deflt_test);
  suite_add_tcase(s, tc_core);

  tc_core = tcase_create("set_chkpt_deflt_test");
  tcase_add_test(tc_core, set_chkpt_deflt_test);
  suite_add_tcase(s, tc_core);

  tc_core = tcase_create("set_statechar_test");
  tcase_add_test(tc_core, set_statechar_test);
  suite_add_tcase(s, tc_core);

  tc_core = tcase_create("lock_ji_mutex_test");
  tcase_add_test(tc_core, lock_ji_mutex_test);
  tcase_add_test(tc_core, chk_mppnodect_test);
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
  sr = srunner_create(svr_jobfunc_suite());
  srunner_set_log(sr, "svr_jobfunc_suite.log");
  srunner_run_all(sr, CK_NORMAL);
  number_failed = srunner_ntests_failed(sr);
  srunner_free(sr);
  return number_failed;
  }
