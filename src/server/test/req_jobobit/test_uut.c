#include "license_pbs.h" /* See here for the software license */
#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>

#include "pbs_job.h"
#include "req_jobobit.h"
#include "test_uut.h"
#include "pbs_error.h"
#include "batch_request.h"
#include "sched_cmds.h"
#include "server.h"
#include "work_task.h"


char *setup_from(job *pjob, const char *suffix);
int   is_joined(job *pjob, enum job_atr ati);
batch_request *return_stdfile(batch_request *preq, job *pjob, enum job_atr ati);
void rel_resc(job *pjob);
int handle_exiting_or_abort_substate(job *pjob);
int setrerun(job *pjob);
batch_request *setup_cpyfiles(batch_request *preq, job *pjob, char *from, char *to, int direction, int tflag);
int handle_returnstd(job *pjob, batch_request *preq, int type);
int mom_comm(job *pjob, void *(*func)(struct work_task *vp));
int handle_complete_first_time(job *pjob);
int handle_complete_second_time(struct work_task *ptask);
int handle_complete_subjob(job *pjob);
int handle_exited(job *pjob);
int add_comment_to_parent(job *parent_job, int cray_subjob_exited_nonzero, int exit_status);
int end_of_job_accounting(job *pjob, char *acctbuf, int accttail);
int handle_terminating_array_subjob(job *pjob);


extern pthread_mutex_t *svr_do_schedule_mutex;
extern pthread_mutex_t *listener_command_mutex;
extern int svr_do_schedule;
extern int listener_command;
int alloc_br_null;
extern struct server server;
extern int bad_connect;
extern int bad_job;
extern int cray_enabled;
extern int double_bad;
extern int reported;
extern int bad_drequest;
extern int usage;

void init_server()
  {
  server.sv_attr_mutex = (pthread_mutex_t *)calloc(1, sizeof(pthread_mutex_t));
  pthread_mutex_init(server.sv_attr_mutex, NULL);
  }


START_TEST(setup_from_test)
  {
  job   pjob;
  char *str;

  memset(&pjob, 0, sizeof(pjob));
  strcpy(pjob.ji_qs.ji_fileprefix, "bob");
  str = setup_from(&pjob, "by");
  fail_unless(!strcmp(str, "bobby"));
  }
END_TEST




START_TEST(is_joined_test)
  {
  job   pjob;

  memset(&pjob, 0, sizeof(pjob));
  pjob.ji_wattr[JOB_ATR_join].at_flags |= ATR_VFLAG_SET;

  pjob.ji_wattr[JOB_ATR_join].at_val.at_str = strdup("oe");
  fail_unless(is_joined(&pjob, JOB_ATR_exec_host) == 0);
  fail_unless(is_joined(&pjob, JOB_ATR_outpath) == 0);
  fail_unless(is_joined(&pjob, JOB_ATR_errpath) == 1);

  pjob.ji_wattr[JOB_ATR_join].at_val.at_str = strdup("e");
  fail_unless(is_joined(&pjob, JOB_ATR_outpath) == 0);

  pjob.ji_wattr[JOB_ATR_join].at_val.at_str = strdup("o");
  fail_unless(is_joined(&pjob, JOB_ATR_errpath) == 0);
  }
END_TEST




START_TEST(return_stdfile_test)
  {
  job            pjob;
  batch_request  preq;
  batch_request *p1 = &preq;
  batch_request *p2;

  memset(&pjob, 0, sizeof(pjob));
  memset(&preq, 0, sizeof(preq));
  pjob.ji_wattr[JOB_ATR_checkpoint_name].at_flags = ATR_VFLAG_SET;

  fail_unless(return_stdfile(&preq, &pjob, JOB_ATR_outpath) != NULL);
  fail_unless(return_stdfile(&preq, &pjob, JOB_ATR_errpath) != NULL);

  pjob.ji_wattr[JOB_ATR_join].at_flags |= ATR_VFLAG_SET;
  pjob.ji_wattr[JOB_ATR_join].at_val.at_str = strdup("oe");

  fail_unless((p2 = return_stdfile(&preq, &pjob, JOB_ATR_errpath)) == p1);
  fail_unless((p2 = return_stdfile(NULL, &pjob, JOB_ATR_outpath)) != NULL);
  fail_unless(p2->rq_type == PBS_BATCH_ReturnFiles);

  pjob.ji_wattr[JOB_ATR_checkpoint_name].at_flags = 0;
  fail_unless(return_stdfile(&preq, &pjob, JOB_ATR_outpath) == NULL);

  pjob.ji_wattr[JOB_ATR_interactive].at_flags |= ATR_VFLAG_SET;
  pjob.ji_wattr[JOB_ATR_interactive].at_val.at_long = 1;
  fail_unless(return_stdfile(&preq, &pjob, JOB_ATR_outpath) == NULL);
  }
END_TEST




START_TEST(rel_resc_test)
  {
  job pjob;
  svr_do_schedule_mutex = (pthread_mutex_t *)calloc(1, sizeof(pthread_mutex_t));
  listener_command_mutex = (pthread_mutex_t *)calloc(1, sizeof(pthread_mutex_t));
  pthread_mutex_init(svr_do_schedule_mutex, NULL);
  pthread_mutex_init(listener_command_mutex, NULL);

  rel_resc(&pjob);
  fail_unless(svr_do_schedule == SCH_SCHEDULE_TERM);
  fail_unless(listener_command == SCH_SCHEDULE_TERM);
  }
END_TEST




START_TEST(handle_exiting_or_abort_substate_test)
  {
  job            pjob;

  memset(&pjob, 0, sizeof(pjob));
  
  fail_unless(handle_exiting_or_abort_substate(&pjob) == PBSE_NONE);
  fail_unless(pjob.ji_qs.ji_state == JOB_STATE_EXITING);
  fail_unless(pjob.ji_qs.ji_substate == JOB_SUBSTATE_RETURNSTD);

  pjob.ji_wattr[JOB_ATR_depend].at_flags |= ATR_VFLAG_SET;
  fail_unless(handle_exiting_or_abort_substate(&pjob) == PBSE_NONE);
  }
END_TEST




START_TEST(setrerun_test)
  {
  job pjob;

  memset(&pjob, 0, sizeof(pjob));

  fail_unless(setrerun(&pjob) != PBSE_NONE);
  pjob.ji_wattr[JOB_ATR_rerunable].at_val.at_long = 1;
  fail_unless(setrerun(&pjob) == PBSE_NONE);
  fail_unless(pjob.ji_qs.ji_substate == JOB_SUBSTATE_RERUN);
  }
END_TEST




START_TEST(setup_cpyfiles_test)
  {
  job           *pjob = (job *)calloc(1, sizeof(job));
  batch_request *preq;
  alloc_br_null = 1;
  fail_unless(setup_cpyfiles(NULL, pjob, strdup("from"), strdup("to"), 1, 1) == NULL);
  alloc_br_null = 0;

  strcpy(pjob->ji_qs.ji_jobid, "1.napali");
  preq = setup_cpyfiles(NULL, pjob, strdup("from"), strdup("to"), 1, 1);
  fail_unless(preq != NULL);

  pjob->ji_wattr[JOB_ATR_egroup].at_val.at_str = strdup("dbeer");
  preq = setup_cpyfiles(NULL, pjob, strdup("from"), strdup("to"), 1, 1);

  preq = setup_cpyfiles(preq, pjob, strdup("from"), strdup("to"), 1, 1);
  fail_unless(preq != NULL);
  }
END_TEST




START_TEST(handle_returnstd_test)
  {
  batch_request *preq;
  job           *pjob;

  pjob = (job *)calloc(1, sizeof(job));
  preq = (batch_request *)calloc(1, sizeof(batch_request));

  strcpy(pjob->ji_qs.ji_jobid, "1.napali");
  strcpy(pjob->ji_qs.ji_fileprefix, "1.napali");
  
  fail_unless(handle_returnstd(pjob, preq, WORK_Immed) == PBSE_JOB_FILE_CORRUPT);
  
  pjob->ji_wattr[JOB_ATR_exec_host].at_val.at_str = strdup("napali/0");
  fail_unless(handle_returnstd(pjob, preq, WORK_Deferred_Reply) == PBSE_NONE);
  
  fail_unless(handle_returnstd(pjob, preq, WORK_Immed) == PBSE_NONE);
  }
END_TEST




START_TEST(mom_comm_test)
  {
  job pjob;

  memset(&pjob, 0, sizeof(pjob));
  strcpy(pjob.ji_qs.ji_jobid, "1.napali");
  pjob.ji_wattr[JOB_ATR_exec_host].at_val.at_str = strdup("napali/0+napali/1");

  fail_unless(mom_comm(&pjob, NULL) >= 0);
  
  /* set some variables for error cases */
  bad_connect = 1;
  cray_enabled = 1;
  fail_unless(mom_comm(&pjob, NULL) == -1);
  bad_job = 1;
  fail_unless(mom_comm(&pjob, NULL) == -1 * PBSE_JOB_RECYCLED);

  bad_connect = 0;
  cray_enabled = 0;
  bad_job = 0;
  }
END_TEST




START_TEST(handle_complete_first_time_test)
  {
  job pjob;

  memset(&pjob, 0, sizeof(pjob));
  strcpy(pjob.ji_qs.ji_jobid, "1.napali");

  fail_unless(handle_complete_first_time(&pjob) == 0);
  cray_enabled = 1;
  double_bad = 1;

  fail_unless(handle_complete_first_time(&pjob) == PBSE_JOBNOTFOUND);

  double_bad = 0;
  fail_unless(handle_complete_first_time(&pjob) == 0);

  pjob.ji_qs.ji_substate = JOB_SUBSTATE_COMPLETE;
  pjob.ji_wattr[JOB_ATR_comp_time].at_flags = ATR_VFLAG_SET;
  fail_unless(handle_complete_first_time(&pjob) == 0);
  }
END_TEST




START_TEST(handle_complete_second_time_test)
  {
  struct work_task *ptask;
  ptask = (work_task *)calloc(1, sizeof(*ptask));
  ptask->wt_mutex = (pthread_mutex_t *)calloc(1, sizeof(pthread_mutex_t));
  handle_complete_second_time(ptask);
  
  ptask = (work_task *)calloc(1, sizeof(*ptask));
  ptask->wt_mutex = (pthread_mutex_t *)calloc(1, sizeof(pthread_mutex_t));
  ptask->wt_parm1 = strdup("1.napali");
  handle_complete_second_time(ptask);

  bad_job = 1;
  ptask = (work_task *)calloc(1, sizeof(*ptask));
  ptask->wt_mutex = (pthread_mutex_t *)calloc(1, sizeof(pthread_mutex_t));
  ptask->wt_parm1 = strdup("1.napali");
  handle_complete_second_time(ptask);

  bad_job = 0;
  reported = 1;
  ptask = (work_task *)calloc(1, sizeof(*ptask));
  ptask->wt_mutex = (pthread_mutex_t *)calloc(1, sizeof(pthread_mutex_t));
  ptask->wt_parm1 = strdup("1.napali");
  handle_complete_second_time(ptask);
  }
END_TEST




START_TEST(handle_complete_subjob_test)
  {
  job *parent   = (job *)calloc(1, sizeof(job));
  job *cray     = (job *)calloc(1, sizeof(job));
  job *external = (job *)calloc(1, sizeof(job));

  strcpy(parent->ji_qs.ji_jobid, "1.napali");
  svr_do_schedule_mutex = (pthread_mutex_t *)calloc(1, sizeof(pthread_mutex_t));
  listener_command_mutex = (pthread_mutex_t *)calloc(1, sizeof(pthread_mutex_t));
  pthread_mutex_init(svr_do_schedule_mutex, NULL);
  pthread_mutex_init(listener_command_mutex, NULL);

  cray->ji_parent_job = parent;
  external->ji_parent_job = parent;
  parent->ji_cray_clone = cray;
  parent->ji_external_clone = external;

  fail_unless(handle_complete_subjob(cray) == PBSE_NONE);
  fail_unless(parent->ji_qs.ji_state != JOB_STATE_COMPLETE);

  printf("final\n");

  double_bad = 1;
  cray->ji_qs.ji_state = JOB_STATE_COMPLETE;
  fail_unless(handle_complete_subjob(external) == PBSE_NONE);
  fail_unless(parent->ji_qs.ji_state == JOB_STATE_COMPLETE);
  fail_unless(handle_complete_subjob(external) == PBSE_NONE);
  }
END_TEST




START_TEST(handle_exited_test)
  {
  job *pjob = (job *)calloc(1, sizeof(job));

  strcpy(pjob->ji_qs.ji_jobid, "1.napali");
  pjob->ji_wattr[JOB_ATR_exec_host].at_val.at_str = strdup("napali/0+napali/1");
  svr_do_schedule_mutex = (pthread_mutex_t *)calloc(1, sizeof(pthread_mutex_t));
  listener_command_mutex = (pthread_mutex_t *)calloc(1, sizeof(pthread_mutex_t));
  pthread_mutex_init(svr_do_schedule_mutex, NULL);
  pthread_mutex_init(listener_command_mutex, NULL);

  double_bad = 0;
  fail_unless(handle_exited(pjob) == PBSE_NONE);

  double_bad = 1;
  fail_unless(handle_exited(pjob) == PBSE_JOBNOTFOUND);
  double_bad = 0;

  bad_job = TRUE;
  fail_unless(handle_exited(pjob) == PBSE_CONNECT);
  bad_drequest = TRUE;
  fail_unless(handle_exited(pjob) == PBSE_CONNECT);
  alloc_br_null = TRUE;
  }
END_TEST




START_TEST(add_comment_to_parent_test)
  {
  job *pjob = (job *)calloc(1, sizeof(job));

  strcpy(pjob->ji_qs.ji_jobid, "1.napali");
  pjob->ji_wattr[JOB_ATR_Comment].at_val.at_str = strdup("napali/0+napali/1");
  fail_unless(add_comment_to_parent(pjob, TRUE, 12) == PBSE_NONE);
  fail_unless(add_comment_to_parent(pjob, FALSE, 12) == PBSE_NONE);
  }
END_TEST




START_TEST(end_of_job_accounting_test)
  {
  char *str = strdup("bob\ntom");
  int   accttail = 5;
  job  *pjob = (job *)calloc(1, sizeof(job));
  strcpy(pjob->ji_qs.ji_jobid, "1.napali");

  fail_unless(end_of_job_accounting(pjob, str, accttail) == PBSE_NONE);
  fail_unless(strchr(str, '\n') == NULL);
  usage = 1;
  fail_unless(end_of_job_accounting(pjob, str, accttail) == PBSE_NONE);
  usage = 0;
  }
END_TEST




START_TEST(handle_terminating_array_subjob_test)
  {
  job  *pjob = (job *)calloc(1, sizeof(job));
  strcpy(pjob->ji_qs.ji_jobid, "1[1].napali");
  strcpy(pjob->ji_arraystructid, "1[].napali");

  double_bad = 1;
  fail_unless(handle_terminating_array_subjob(pjob) == PBSE_UNKJOBID);

  double_bad = 0;
  bad_job = 0;
  fail_unless(handle_terminating_array_subjob(pjob) == PBSE_NONE);

  bad_job = 1;
  fail_unless(handle_terminating_array_subjob(pjob) == PBSE_UNKJOBID);
  bad_job = 0;

  }
END_TEST




Suite *req_jobobit_suite(void)
  {
  Suite *s = suite_create("req_jobobit_suite methods");
  TCase *tc_core = tcase_create("setup_from_test");
  tcase_add_test(tc_core, setup_from_test);
  suite_add_tcase(s, tc_core);

  tc_core = tcase_create("is_joined_test");
  tcase_add_test(tc_core, is_joined_test);
  suite_add_tcase(s, tc_core);

  tc_core = tcase_create("return_stdfile_test");
  tcase_add_test(tc_core, return_stdfile_test);
  suite_add_tcase(s, tc_core);

  tc_core = tcase_create("rel_resc_test");
  tcase_add_test(tc_core, rel_resc_test);
  suite_add_tcase(s, tc_core);

  tc_core = tcase_create("handle_exiting_or_abort_substate_test");
  tcase_add_test(tc_core, handle_exiting_or_abort_substate_test);
  suite_add_tcase(s, tc_core);

  tc_core = tcase_create("setrerun_test");
  tcase_add_test(tc_core, setrerun_test);
  suite_add_tcase(s, tc_core);

  tc_core = tcase_create("setup_cpyfiles_test");
  tcase_add_test(tc_core, setup_cpyfiles_test);
  suite_add_tcase(s, tc_core);

  tc_core = tcase_create("handle_returnstd_test");
  tcase_add_test(tc_core, handle_returnstd_test);
  suite_add_tcase(s, tc_core);

  tc_core = tcase_create("mom_comm_test");
  tcase_add_test(tc_core, mom_comm_test);
  suite_add_tcase(s, tc_core);

  tc_core = tcase_create("handle_complete_first_time_test");
  tcase_add_test(tc_core, handle_complete_first_time_test);
  suite_add_tcase(s, tc_core);
  
  tc_core = tcase_create("handle_complete_second_time_test");
  tcase_add_test(tc_core, handle_complete_second_time_test);
  suite_add_tcase(s, tc_core);

  tc_core = tcase_create("handle_complete_subjob_test");
  tcase_add_test(tc_core, handle_complete_subjob_test);
  suite_add_tcase(s, tc_core);

  tc_core = tcase_create("handle_exited_test");
  tcase_add_test(tc_core, handle_exited_test);
  suite_add_tcase(s, tc_core);

  tc_core = tcase_create("add_comment_to_parent_test");
  tcase_add_test(tc_core, add_comment_to_parent_test);
  suite_add_tcase(s, tc_core);

  tc_core = tcase_create("end_of_job_accounting_test");
  tcase_add_test(tc_core, end_of_job_accounting_test);
  suite_add_tcase(s, tc_core);

  tc_core = tcase_create("handle_terminating_array_subjob_test");
  tcase_add_test(tc_core, handle_terminating_array_subjob_test);
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
  init_server();
  rundebug();
  sr = srunner_create(req_jobobit_suite());
  srunner_set_log(sr, "req_jobobit_suite.log");
  srunner_run_all(sr, CK_NORMAL);
  number_failed = srunner_ntests_failed(sr);
  srunner_free(sr);
  return number_failed;
  }
