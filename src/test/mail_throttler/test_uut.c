#include <stdio.h>
#include <stdlib.h>

#include "mail_throttler.hpp"
#include "pbs_error.h"
#include "resource.h"

#include <check.h>


START_TEST(test_job_constructor)
  {
  job pjob;
  const char *jname = "highstorm_predictor";
  const char *owner = "dbeer@roshar";
  const char *queue = "batch";
  const char *jid = "1.nalthis";
  const char *hlist = "scadrial:ppn=16";
  const char *join = "oe";
  const char *outpath = "/home/dbeer/highstorms/";

  std::vector<resource> resources;

  pjob.ji_wattr[JOB_ATR_jobname].at_val.at_str = strdup(jname);
  pjob.ji_wattr[JOB_ATR_job_owner].at_val.at_str = strdup(owner);
  pjob.ji_wattr[JOB_ATR_exec_host].at_val.at_str = strdup(hlist);
  pjob.ji_wattr[JOB_ATR_join].at_flags = ATR_VFLAG_SET;
  pjob.ji_wattr[JOB_ATR_join].at_val.at_str = strdup(join);
  pjob.ji_wattr[JOB_ATR_outpath].at_val.at_str = strdup(outpath);
  pjob.ji_wattr[JOB_ATR_init_work_dir].at_flags = ATR_VFLAG_SET;
  pjob.ji_wattr[JOB_ATR_init_work_dir].at_val.at_str = strdup(outpath);
  pjob.ji_wattr[JOB_ATR_resource].at_val.at_ptr = &resources;
  pjob.ji_wattr[JOB_ATR_resc_used].at_val.at_ptr = &resources;

  strcpy(pjob.ji_qs.ji_queue, queue);
  strcpy(pjob.ji_qs.ji_jobid, jid);

  mail_info mi(&pjob);

  fail_unless(mi.errFile == outpath);
  fail_unless(mi.outFile == outpath);
  fail_unless(mi.jobid == jid);
  fail_unless(mi.exec_host == hlist);
  fail_unless(mi.jobname == jname);
  fail_unless(mi.queue_name == queue);
  fail_unless(mi.owner == owner);
  fail_unless(mi.working_directory == outpath, "Got '%s', expected '%s'", mi.working_directory.c_str(), outpath);
  }
END_TEST


START_TEST(test_adding)
  {
  mail_throttler mt;
  mail_info      mi;

  mi.mailto = "a";
  fail_unless(mt.add_email_entry(mi) == true);
  fail_unless(mt.add_email_entry(mi) == false);
  fail_unless(mt.add_email_entry(mi) == false);
  fail_unless(mt.add_email_entry(mi) == false);

  std::vector<mail_info> pending;
  fail_unless(mt.get_email_list(mi.mailto, pending) == PBSE_NONE);
  fail_unless(pending.size() == 4);
  fail_unless(mt.get_email_list(mi.mailto, pending) != PBSE_NONE);
  fail_unless(mt.get_email_list("b", pending) != PBSE_NONE);
  fail_unless(mt.get_email_list("c", pending) != PBSE_NONE);
  
  fail_unless(mt.add_email_entry(mi) == true);
  fail_unless(mt.add_email_entry(mi) == false);
  fail_unless(mt.add_email_entry(mi) == false);

  mi.mailto = "b";
  fail_unless(mt.add_email_entry(mi) == true);
  fail_unless(mt.add_email_entry(mi) == false);

  mi.mailto = "c";
  fail_unless(mt.add_email_entry(mi) == true);
  fail_unless(mt.add_email_entry(mi) == false);

  fail_unless(mt.get_email_list("d", pending) != PBSE_NONE);
  fail_unless(mt.get_email_list("3", pending) != PBSE_NONE);
  fail_unless(mt.get_email_list("a", pending) == PBSE_NONE);
  fail_unless(pending.size() == 3);
  fail_unless(mt.get_email_list("b", pending) == PBSE_NONE);
  fail_unless(pending.size() == 2);
  fail_unless(mt.get_email_list("c", pending) == PBSE_NONE);
  fail_unless(pending.size() == 2);
  fail_unless(mt.get_email_list("a", pending) != PBSE_NONE);
  fail_unless(mt.get_email_list("b", pending) != PBSE_NONE);
  fail_unless(mt.get_email_list("c", pending) != PBSE_NONE);
  }
END_TEST


Suite *mail_throttler_suite(void)
  {
  Suite *s = suite_create("mail_throttler test suite methods");
  TCase *tc_core = tcase_create("test_adding");
  tcase_add_test(tc_core, test_adding);
  suite_add_tcase(s, tc_core);
  
  tc_core = tcase_create("test_job_constructor");
  tcase_add_test(tc_core, test_job_constructor);
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
  sr = srunner_create(mail_throttler_suite());
  srunner_set_log(sr, "mail_throttler_suite.log");
  srunner_run_all(sr, CK_NORMAL);
  number_failed = srunner_ntests_failed(sr);
  srunner_free(sr);
  return(number_failed);
  }

