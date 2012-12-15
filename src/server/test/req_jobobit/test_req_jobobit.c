#include "license_pbs.h" /* See here for the software license */
#include <stdlib.h>
#include <stdio.h>

#include "pbs_job.h"
#include "req_jobobit.h"
#include "test_req_jobobit.h"
#include "pbs_error.h"
#include "batch_request.h"
#include "sched_cmds.h"


char *setup_from(job *pjob, const char *suffix);
int   is_joined(job *pjob, enum job_atr ati);
batch_request *return_stdfile(batch_request *preq, job *pjob, enum job_atr ati);
void rel_resc(job *pjob);
int handle_exiting_or_abort_substate(job *pjob);

extern pthread_mutex_t *svr_do_schedule_mutex;
extern pthread_mutex_t *listener_command_mutex;
extern int svr_do_schedule;
extern int listener_command;



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
  svr_do_schedule_mutex = calloc(1, sizeof(pthread_mutex_t));
  listener_command_mutex = calloc(1, sizeof(pthread_mutex_t));
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
  sr = srunner_create(req_jobobit_suite());
  srunner_set_log(sr, "req_jobobit_suite.log");
  srunner_run_all(sr, CK_NORMAL);
  number_failed = srunner_ntests_failed(sr);
  srunner_free(sr);
  return number_failed;
  }
