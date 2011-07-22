#include "test_catch_child.h"
#include "catch_child.h"
#include <stdlib.h>

/* possible values for recover:
 * JOB_RECOV_RUNNING
 * JOB_RECOV_DELETE
 * JOB_RECOV_TERM_REQUE
 * JOB_RECOV_REQUE
 *
 * external calls:
 *  stdlib calls
 *    opendir
 *    readdir
 *    sprintf
 *  torque calls
 *    job_recov
 *    log_record
 *    set_globid
 *    append_link
 *    job_nodes
 *    task_recov
 *    mom_checkpoint_recover
 *    addclient
 *    mom_deljob
 *    send_sisters_radix
 *    send_sisters
 *    job_save
 *    check_pwd
 *    closedir
 *    initialize_root_cpuset
 *  globals used
 *    log_buffer
 *    msg_daemonname
 */
extern int exit_called;
extern int LOGLEVEL;
extern char *path_jobs;
extern int exiting_tasks;
extern int tc;
extern int func_num;


void test_iaj_nodir_setup()
  {
  path_jobs = malloc(2);
  path_jobs[0] = '\0';
  path_jobs[1] = '\0';
  }

START_TEST(test_iaj_nodir) 
  {
  test_iaj_nodir_setup();
  init_abort_jobs(JOB_RECOV_RUNNING);
  fail_unless(exit_called == 1, "This call should have exited because of a null path");
  }
END_TEST

void test_iaj_pathdot()
  {
  path_jobs = malloc(2);
  path_jobs[0] = '.';
  path_jobs[1] = '\0';
  }

/* It should be noted that the #define does not work
START_TEST(test_iaj_cpuset) 
  {
#define PENABLE_LINUX26_CPUSETS
  test_iaj_pathdot();
  init_abort_jobs(JOB_RECOV_RUNNING);
#undef PENABLE_LINUX26_CPUSETS
  }
END_TEST
*/

START_TEST(test_iaj_jobnull)
  {
  test_iaj_pathdot();
  func_num = INIT_ABORT_JOBS;
  tc = 1;
  init_abort_jobs(JOB_RECOV_RUNNING);
  }
END_TEST

START_TEST(test_iaj_appendlink)
  {
  test_iaj_pathdot();
  func_num = INIT_ABORT_JOBS;
  tc = 2;
  init_abort_jobs(JOB_RECOV_RUNNING);
  }
END_TEST

START_TEST(test_iaj_running) 
  {
  test_iaj_pathdot();
  func_num = INIT_ABORT_JOBS;
  tc = 3;
  LOGLEVEL = 6;
  init_abort_jobs(JOB_RECOV_RUNNING);
  }
END_TEST

/* It should be noted that the #define does not work
START_TEST(test_iaj_migraterun) 
  {
#define PBS_CHKPT_MIGRATE
  test_iaj_pathdot();
  init_abort_jobs(JOB_RECOV_RUNNING);
#undef PBS_CHKPT_MIGRATE
  }
END_TEST
*/

START_TEST(test_iaj_prerun) 
  {
  test_iaj_pathdot();
  func_num = INIT_ABORT_JOBS;
  tc = 4;
  init_abort_jobs(JOB_RECOV_TERM_REQUE);
  }
END_TEST

START_TEST(test_iaj_suspend) 
  {
  test_iaj_pathdot();
  func_num = INIT_ABORT_JOBS;
  tc = 5;
  init_abort_jobs(JOB_RECOV_TERM_REQUE);
  }
END_TEST

START_TEST(test_iaj_exited) 
  {
  test_iaj_pathdot();
  func_num = INIT_ABORT_JOBS;
  tc = 6;
  init_abort_jobs(JOB_RECOV_TERM_REQUE);
  }
END_TEST

START_TEST(test_iaj_noterm) 
  {
  test_iaj_pathdot();
  func_num = INIT_ABORT_JOBS;
  tc = 7;
  init_abort_jobs(JOB_RECOV_TERM_REQUE);
  }
END_TEST

START_TEST(test_iaj_exiting) 
  {
  test_iaj_pathdot();
  func_num = INIT_ABORT_JOBS;
  tc = 8;
  LOGLEVEL = 6;
  init_abort_jobs(JOB_RECOV_TERM_REQUE);
  }
END_TEST

START_TEST(test_iaj_svflg) 
  {
  test_iaj_pathdot();
  func_num = INIT_ABORT_JOBS;
  tc = 9;
  LOGLEVEL = 6;
  init_abort_jobs(JOB_RECOV_TERM_REQUE);
  }
END_TEST

START_TEST(test_iaj_radix) 
  {
  test_iaj_pathdot();
  func_num = INIT_ABORT_JOBS;
  tc = 10;
  LOGLEVEL = 6;
  init_abort_jobs(JOB_RECOV_TERM_REQUE);
  }
END_TEST

START_TEST(test_iaj_nosis) 
  {
  test_iaj_pathdot();
  func_num = INIT_ABORT_JOBS;
  tc = 11;
  LOGLEVEL = 6;
  init_abort_jobs(JOB_RECOV_TERM_REQUE);
  }
END_TEST

START_TEST(test_iaj_exitingtasks) 
  {
  test_iaj_pathdot();
  func_num = INIT_ABORT_JOBS;
  tc = 12;
  LOGLEVEL = 6;
  init_abort_jobs(JOB_RECOV_TERM_REQUE);
  fail_unless(exiting_tasks == 1, "This did not reach the end of the code path");
  }
END_TEST

START_TEST(test_iaj_term_reque) 
  {
  test_iaj_pathdot();
  init_abort_jobs(JOB_RECOV_TERM_REQUE);
  }
END_TEST

/* It should be noted that the #define does not work
START_TEST(test_iaj_migratereque) 
  {
#define PBS_CHKPT_MIGRATE
  func_num = INIT_ABORT_JOBS;
  tc = 13;
  LOGLEVEL = 6;
  init_abort_jobs(JOB_RECOV_TERM_REQUE);
#undef PBS_CHKPT_MIGRATE
  }
END_TEST
*/
Suite *init_abort_jobs_suite(void)
  {
  Suite *s = suite_create("init_abort_jobs methods");
  TCase *tc_core = tcase_create("Core");
  tcase_add_exit_test(tc_core, test_iaj_nodir, 1);
//  tcase_add_test(tc_core, test_iaj_cpuset);
  tcase_add_test(tc_core, test_iaj_jobnull);
  tcase_add_test(tc_core, test_iaj_appendlink);
  tcase_add_test(tc_core, test_iaj_running);
//  tcase_add_test(tc_core, test_iaj_migraterun);
  tcase_add_test(tc_core, test_iaj_prerun);
  tcase_add_test(tc_core, test_iaj_suspend);
  tcase_add_test(tc_core, test_iaj_exiting);
  tcase_add_test(tc_core, test_iaj_noterm);
  tcase_add_test(tc_core, test_iaj_exiting);
  tcase_add_test(tc_core, test_iaj_svflg);
  tcase_add_test(tc_core, test_iaj_radix);
  tcase_add_test(tc_core, test_iaj_nosis);
  tcase_add_test(tc_core, test_iaj_exitingtasks);
  tcase_add_test(tc_core, test_iaj_term_reque);
//  tcase_add_test(tc_core, test_iaj_migratereque);
  suite_add_tcase(s, tc_core);
  return s;
  }

void test_iaj_pathdotdot()
  {
  path_jobs = malloc(3);
  path_jobs[0] = '.';
  path_jobs[1] = '.';
  path_jobs[2] = '\0';
  }

void rundebug()
  {
#define PENABLE_LINUX26_CPUSETS
  test_iaj_pathdotdot();
#define PBS_CHKPT_MIGRATE
  init_abort_jobs(JOB_RECOV_RUNNING);
#undef PENABLE_LINUX26_CPUSETS
  }

void mom_deljob(job *pjob)
  {
  }

int main(void)
  {
//  rundebug();
  int number_failed = 0;
  SRunner *sr = srunner_create(init_abort_jobs_suite());
  srunner_set_log(sr, "init_abort_jobs_suite.log");
  srunner_run_all(sr, CK_NORMAL);
  number_failed = srunner_ntests_failed(sr);
  srunner_free(sr);
  return number_failed;
  }
