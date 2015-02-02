#include "license_pbs.h" /* See here for the software license */
#include "job_func.h"
#include "test_job_func.h"
#include "server.h" /* server, NO_BUFFER_SPACE */
#include <errno.h> /* ENOMEM */

extern int tc;
extern int func_num;
extern attribute_def job_attr_def[];
extern struct server server;

bool set_task_called = false;

/*
START_TEST(test_job_log_open_fail)
  {
  int rc = 0;
  job *pjob = (job *)calloc(1, sizeof(job));
  func_num = RECORD_JOBINFO_SUITE;
  tc = 1;
  strcat(pjob->ji_qs.ji_jobid, "101");
  rc = record_jobinfo(pjob);
  fail_unless(rc == -1, "return value invalid (%d should be -1)", rc);
  }
END_TEST

START_TEST(test_log_job_record_fail)
  {
  int rc = 0;
  job *pjob = (job *)calloc(1, sizeof(job));
  func_num = RECORD_JOBINFO_SUITE;
  tc = 2;
  strcat(pjob->ji_qs.ji_jobid, "101");
  rc = record_jobinfo(pjob);
  }
END_TEST

START_TEST(test_for_loop_depend)
  {
  int rc = 0;
  job *pjob = (job *)calloc(1, sizeof(job));
  func_num = RECORD_JOBINFO_SUITE;
  tc = 3;
  strcat(pjob->ji_qs.ji_jobid, "101");
  // 9 = ATTR_depend
  pjob->ji_wattr[9].at_flags |= ATR_VFLAG_SET;
  rc = record_jobinfo(pjob);
  }
END_TEST

START_TEST(test_for_loop_nobufspace)
  {
  int rc = 0;
  job *pjob = (job *)calloc(1, sizeof(job));
  func_num = RECORD_JOBINFO_SUITE;
  tc = 4;
  strcat(pjob->ji_qs.ji_jobid, "101");
  pjob->ji_wattr[0].at_flags |= ATR_VFLAG_SET;
  rc = record_jobinfo(pjob);
  fail_if(rc == -3, "size of buffer not being incremented properly (%d != %d)", -3, rc);
  }
END_TEST

START_TEST(test_for_loop_nobufspace_realloc_fail)
  {
  int rc = 0;
  job *pjob = (job *)calloc(1, sizeof(job));
  func_num = RECORD_JOBINFO_SUITE;
  tc = 5;
  strcat(pjob->ji_qs.ji_jobid, "101");
  pjob->ji_wattr[0].at_flags |= ATR_VFLAG_SET;
  rc = record_jobinfo(pjob);
  fail_unless(rc == ENOMEM, "This should generate a out of memory error (%d != %d)", ENOMEM, rc);
  }
END_TEST

START_TEST(test_for_loop_resource)
  {
  int rc = 0;
  job *pjob = (job *)calloc(1, sizeof(job));
  func_num = RECORD_JOBINFO_SUITE;
  tc = 6;
  strcat(pjob->ji_qs.ji_jobid, "101");
  // 26 = JOB_ATR_resource/ATTR_l (ATR_TYPE_RESC)
  pjob->ji_wattr[26].at_flags |= ATR_VFLAG_SET;
  pjob->ji_wattr[26].at_type |= ATR_TYPE_RESC;
  rc = record_jobinfo(pjob);
  }
END_TEST

START_TEST(test_for_loop)
  {
  int rc = 0;
  job *pjob = (job *)calloc(1, sizeof(job));
  func_num = RECORD_JOBINFO_SUITE;
  tc = 7;
  strcat(pjob->ji_qs.ji_jobid, "101");
  pjob->ji_wattr[0].at_flags |= ATR_VFLAG_SET;
  rc = record_jobinfo(pjob);
  }
END_TEST

START_TEST(test_for_loop_logjobrecord_exit)
  {
  int rc = 0;
  job *pjob = (job *)calloc(1, sizeof(job));
  func_num = RECORD_JOBINFO_SUITE;
  tc = 8;
  server.sv_attr[SRV_ATR_RecordJobScript].at_val.at_long = 1;
  strcat(pjob->ji_qs.ji_jobid, "101");
  pjob->ji_wattr[0].at_flags |= ATR_VFLAG_SET;
  rc = record_jobinfo(pjob);
  }
END_TEST
*/

Suite *record_jobinfo_suite(void)
  {
  Suite *s = suite_create("record_jobinfo methods");
/*  TCase *tc_core = tcase_create("Core");
  tcase_add_test(tc_core, test_job_log_open_fail);
  tcase_add_test(tc_core, test_log_job_record_fail);
  tcase_add_test(tc_core, test_for_loop_depend);
  tcase_add_test(tc_core, test_for_loop_nobufspace);
  tcase_add_test(tc_core, test_for_loop_nobufspace_realloc_fail);
  tcase_add_test(tc_core, test_for_loop_resource);
  tcase_add_test(tc_core, test_for_loop);
  tcase_add_test(tc_core, test_for_loop_logjobrecord_exit);
  suite_add_tcase(s, tc_core);
  */
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
  sr = srunner_create(record_jobinfo_suite());
  srunner_set_log(sr, "record_jobinfo_suite.log");
  srunner_run_all(sr, CK_NORMAL);
  number_failed = srunner_ntests_failed(sr);
  srunner_free(sr);
  return number_failed;
  }
