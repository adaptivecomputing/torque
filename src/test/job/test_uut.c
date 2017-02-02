#include "license_pbs.h" /* See here for the software license */
#include "lib_utils.h"
#include <stdlib.h>
#include <stdio.h>

#include "pbs_error.h"
#include "pbs_job.h"
#include <check.h>


START_TEST(test_basic)
  {
  job pjob;

  pjob.set_jobid("1.roshar");
  pjob.set_fileprefix("1.roshar");
  fail_unless(!strcmp("1.roshar", pjob.get_jobid()));
  fail_unless(!strcmp("1.roshar", pjob.get_fileprefix()));
  fail_unless(pjob.get_priority() == 0);
  fail_unless(pjob.get_ordering() == 0);

  pjob.set_state(JOB_STATE_QUEUED);
  pjob.set_substate(JOB_SUBSTATE_QUEUED);
  fail_unless(pjob.get_state(), JOB_STATE_QUEUED);
  fail_unless(pjob.get_substate(), JOB_SUBSTATE_QUEUED);
  
  pjob.set_destination("batch");
  fail_unless(!strcmp(pjob.get_destination(), "batch"));
  pjob.set_queue("batch");
  fail_unless(!strcmp(pjob.get_queue(), "batch"));
  pjob.set_start_time(100);
  fail_unless(pjob.get_start_time() == 100);
  pjob.set_svrflags(1);
  fail_unless(pjob.get_svrflags() == 1);

  pjob.set_ji_momaddr(2);
  fail_unless(pjob.get_ji_momaddr() == 2);

  pjob.set_ji_momport(30004);
  pjob.set_ji_mom_rmport(30005);
  fail_unless(pjob.get_ji_momport() == 30004);
  fail_unless(pjob.get_ji_mom_rmport() == 30005);
  
  pjob.set_un_type(1);
  fail_unless(pjob.get_un_type() == 1);
  pjob.set_route_retry_time(10);
  pjob.set_route_queue_time(20);
  fail_unless(pjob.get_route_retry_time() == 10);
  fail_unless(pjob.get_route_queue_time() == 20);
  
  pjob.set_svraddr(5);
  pjob.set_exgid(300);
  pjob.set_exuid(400);
  pjob.set_mom_exitstat(2);
  fail_unless(pjob.get_exgid() == 300);
  fail_unless(pjob.get_exuid() == 400);
  fail_unless(pjob.get_mom_exitstat() == 2);
  fail_unless(pjob.get_svraddr() == 5);

  pjob.set_scriptsz(1000);
  fail_unless(pjob.get_scriptsz() == 1000);

  pjob.set_exec_exitstat(0);
  fail_unless(pjob.get_exec_exitstat() == 0);

  pjob.set_modified(true);
  fail_unless(pjob.has_been_modified());
  pjob.set_modified(false);
  fail_unless(pjob.has_been_modified() == false);

  pjob.set_qs_version(9);
  fail_unless(pjob.get_qs_version() == 9);

  pjob.set_fromsock(7);
  pjob.set_fromaddr(2000);
  fail_unless(pjob.get_fromsock() == 7);
  fail_unless(pjob.get_fromaddr() == 2000);

  fail_unless(pjob.get_attr_flags(JOB_ATR_exec_host) == 0);
  fail_unless(pjob.set_str_attr(JOB_ATR_exec_host, strdup("sel/0-11+nalthis/0-11")) == PBSE_NONE);
  fail_unless((pjob.get_attr_flags(JOB_ATR_exec_host) & ATR_VFLAG_SET) != 0);
  fail_unless(pjob.is_attr_set(JOB_ATR_exec_host) == true);
  pbs_attribute *pattr = pjob.get_attr(JOB_ATR_exec_host);
  fail_unless((pattr->at_flags & ATR_VFLAG_SET) != 0);

  // Can't set a long attribute to a string
  fail_unless(pjob.set_str_attr(JOB_ATR_ctime, strdup("bob")) != PBSE_NONE);
  fail_unless(pjob.get_creq_attr(JOB_ATR_req_information) == NULL);

  fail_unless(pjob.set_long_attr(JOB_ATR_ctime, 20) == PBSE_NONE);
  fail_unless(pjob.get_long_attr(JOB_ATR_ctime) == 20);

  fail_unless(pjob.set_ll_attr(JOB_ATR_pagg_id, 5423) == PBSE_NONE);
  fail_unless(pjob.get_ll_attr(JOB_ATR_pagg_id) == 5423);

  fail_unless(pjob.set_bool_attr(JOB_ATR_rerunable, true) == PBSE_NONE);
  fail_unless(pjob.get_bool_attr(JOB_ATR_rerunable) == true);

  fail_unless(pjob.set_char_attr(JOB_ATR_state, 'Q') == PBSE_NONE);
  fail_unless(pjob.get_char_attr(JOB_ATR_state) == 'Q');
  }
END_TEST


Suite *job_suite(void)
  {
  Suite *s = suite_create("job_suite methods");
  TCase *tc_core = tcase_create("test_basic");
  tcase_add_test(tc_core, test_basic);
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
  sr = srunner_create(job_suite());
  srunner_set_log(sr, "job_suite.log");
  srunner_run_all(sr, CK_NORMAL);
  number_failed = srunner_ntests_failed(sr);
  srunner_free(sr);
  return number_failed;
  }
