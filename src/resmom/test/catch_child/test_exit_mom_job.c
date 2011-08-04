#include "test_catch_child.h"
#include "catch_child.h"
extern int tc;
extern int func_num;
extern int ran_one;
extern char *getenv(char *);
extern int multi_mom;
extern int LOGLEVEL;
extern void *calloc(size_t __nmemb, size_t __size);

START_TEST(test_exit_mom_job_nojihost)
  {
  job *pjob = (job *)calloc(1, sizeof(job));
  int mom_radix = 0;
  func_num = EXIT_MOM_JOB;
  tc = 1;
  LOGLEVEL = 6;
  multi_mom = 1;
  exit_mom_job(pjob, mom_radix);
  }
END_TEST

START_TEST(test_exit_mom_job_tmnullevent)
  {
  job *pjob = (job *)calloc(1, sizeof(job));
  int mom_radix = 0;
  func_num = EXIT_MOM_JOB;
  tc = 2;
  LOGLEVEL = 6;
  pjob->ji_hosts = (hnodent *)calloc(2, sizeof(hnodent));
  pjob->ji_hosts[0].hn_stream = 1;
  pjob->ji_obit = TM_NULL_EVENT;
  exit_mom_job(pjob, mom_radix);
  }
END_TEST

START_TEST(test_exit_mom_job_atrvflag)
  {
  job *pjob = (job *)calloc(1, sizeof(job));
  int mom_radix = 0;
  func_num = EXIT_MOM_JOB;
  tc = 4;
  LOGLEVEL = 6;
  pjob->ji_hosts = (hnodent *)calloc(2, sizeof(hnodent));
  pjob->ji_hosts[0].hn_stream = 1;
  pjob->ji_obit = TM_TASKS;
  pjob->ji_wattr[(int)JOB_ATR_interactive].at_flags |= ATR_VFLAG_SET;
  pjob->ji_wattr[(int)JOB_ATR_interactive].at_val.at_long = 1;
  exit_mom_job(pjob, mom_radix);
  }
END_TEST

START_TEST(test_exit_mom_job_momradix3)
  {
  job *pjob = (job *)calloc(1, sizeof(job));
  int mom_radix = 3;
  func_num = EXIT_MOM_JOB;
  tc = 6;
  LOGLEVEL = 6;
  pjob->ji_hosts = (hnodent *)calloc(2, sizeof(hnodent));
  pjob->ji_hosts[0].hn_stream = 1;
  pjob->ji_obit = TM_TASKS;
  pjob->ji_wattr[(int)JOB_ATR_interactive].at_flags = 0;
  exit_mom_job(pjob, mom_radix);
  }
END_TEST

Suite *exit_mom_job_suite(void)
  {
  Suite *s = suite_create("exit_mom_job methods");
  TCase *tc_core = tcase_create("Core");
  tcase_add_test(tc_core, test_exit_mom_job_nojihost);
  tcase_add_test(tc_core, test_exit_mom_job_tmnullevent);
  tcase_add_test(tc_core, test_exit_mom_job_atrvflag);
  tcase_add_test(tc_core, test_exit_mom_job_momradix3);
  suite_add_tcase(s, tc_core);
  return s;
  }

void rundebug()
  {
  job *pjob = (job *)calloc(1, sizeof(job));
  int mom_radix = 0;
  func_num = EXIT_MOM_JOB;
  tc = 1;
  LOGLEVEL = 6;
  multi_mom = 1;
  exit_mom_job(pjob, mom_radix);
  }

int main(void)
  {
  int number_failed = 0;
  SRunner *sr = NULL;
  SRunner *sr = NULL;
  rundebug();
  sr = srunner_create(exit_mom_job_suite());
  srunner_set_log(sr, "exit_mom_job_suite.log");
  srunner_run_all(sr, CK_NORMAL);
  number_failed = srunner_ntests_failed(sr);
  srunner_free(sr);
  return number_failed;
  }
