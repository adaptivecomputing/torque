#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include "pbs_config.h"
#include "mom_main.h"
#include "mom_config.h"
#include "pbs_error.h"
#include "test_mom_main.h"

extern bool parsing_hierarchy;
extern bool received_cluster_addrs;
extern char *path_mom_hierarchy;
extern int  MOMJobDirStickySet;
extern time_t time_now;
extern time_t wait_time;
extern time_t LastServerUpdateTime;
extern time_t last_poll_time;
extern bool ForceServerUpdate;

void read_mom_hierarchy();
int  parse_integer_range(const char *range_str, int &start, int &end);
time_t calculate_select_timeout();

extern int  exiting_tasks;

bool call_scan_for_exiting();
extern tlist_head svr_alljobs;

START_TEST(test_read_mom_hierarchy)
  {
  system("rm -f bob");
  path_mom_hierarchy = strdup("bob");
  read_mom_hierarchy();
  fail_unless(received_cluster_addrs == false);
  system("touch bob");
  // the following lines need more spoofing in order to work correctly
//  parsing_hierarchy = true;
//  read_mom_hierarchy();
//  fail_unless(received_cluster_addrs == true);
//  parsing_hierarchy = false;
  }
END_TEST

START_TEST(test_call_scan_for_exiting)
  {
  exiting_tasks = true;

  fail_unless(call_scan_for_exiting() == true);

  exiting_tasks = false;

  job job1;
  job job2;
  job job3;

  memset(&job1,0,sizeof(job1));
  memset(&job2,0,sizeof(job2));
  memset(&job3,0,sizeof(job3));
  svr_alljobs.ll_prior = &job3.ji_alljobs;
  svr_alljobs.ll_next = &job1.ji_alljobs;
  svr_alljobs.ll_struct = NULL;

  job1.ji_alljobs.ll_prior = &svr_alljobs;
  job1.ji_alljobs.ll_next = &job2.ji_alljobs;
  job1.ji_alljobs.ll_struct = &job1;

  job2.ji_alljobs.ll_prior = &job1.ji_alljobs;
  job2.ji_alljobs.ll_next = &job3.ji_alljobs;
  job2.ji_alljobs.ll_struct = &job2;

  job3.ji_alljobs.ll_prior = &job2.ji_alljobs;
  job3.ji_alljobs.ll_next = &svr_alljobs;
  job3.ji_alljobs.ll_struct = &job3;

  fail_unless(call_scan_for_exiting() == false);

  job2.ji_qs.ji_substate = JOB_SUBSTATE_EXITING;

  fail_unless(call_scan_for_exiting() == true);
  }
END_TEST

START_TEST(test_parse_integer_range)
  {
  int start;
  int end;

  fail_unless(parse_integer_range("0", start, end) == PBSE_NONE);
  fail_unless(start == end);
  fail_unless(start == 0);

  fail_unless(parse_integer_range("0-2", start, end) == PBSE_NONE);
  fail_unless(end == 2);
  fail_unless(start == 0);

  fail_unless(parse_integer_range("2-4", start, end) == PBSE_NONE);
  fail_unless(end == 4);
  fail_unless(start == 2);

  fail_unless(parse_integer_range("4-2", start, end) != PBSE_NONE);
  }
END_TEST


START_TEST(test_mom_job_dir_sticky_config)
  {
  char *tempfilename = tempnam("/tmp", "test");
  fail_unless((tempfilename != NULL), "Failed to create a temporary filename");

  FILE *fp = fopen(tempfilename, "w");
  fail_unless(fp != NULL, "Failed to create a file to test mom_job_dir_sticky_config");

  fprintf(fp, "#some configuration\n");
  fprintf(fp, "$configversion xyz\n");
  fprintf(fp, "$loglevel 11\n");
  fflush(fp);

  int s = fclose(fp);
  fail_unless(s == 0, "Failed to close test file 1st time");

  get_mom_job_dir_sticky_config(tempfilename);
  fail_unless(MOMJobDirStickySet == 0, "Failed to detect MOMJobDirStickySet was absent");

  fp = fopen(tempfilename, "a");
  fail_unless(fp != NULL, "Failed to open file to append to continue testing on mom_job_dir_sticky_config");
  fprintf(fp, "$jobdirectory_sticky\ttrue\n");

  s = fclose(fp);
  fail_unless(s == 0, "Failed to close test file 2nd time");

  get_mom_job_dir_sticky_config(tempfilename);
  fail_unless(MOMJobDirStickySet == 1, "Failed to detect MOMJobDirStickySet");

  fp = fopen(tempfilename, "w");
  fail_unless(fp != NULL, "Failed to open file to write to continue testing on mom_job_dir_sticky_config");
  fprintf(fp, "#some more testing\n");
  fprintf(fp, "$jobdirectory_sticky false\n");

  s = fclose(fp);
  fail_unless(s == 0, "Failed to close test file 3rd time");

  get_mom_job_dir_sticky_config(tempfilename);
  fail_unless(MOMJobDirStickySet == 0, "Failed to detect MOMJobDirStickySet was false");

  unlink(tempfilename);
  get_mom_job_dir_sticky_config(tempfilename);
  fail_unless(MOMJobDirStickySet == 0, "Failed to detect MOMJobDirStickySet was unset");
  }
END_TEST


/**
 * time_t calculate_select_timeout(void)
 * Input:
 *  (G) time_t time_now - periodically updated current time.
 *  (G) time_t wait_time - constant systme-dependent value.
 *  (G) time_t LastServerUpdateTime - last status update timestamp.
 *  (G) int ServerStatUpdateInterval - status update interval.
 *  (G) time_t last_poll_time - last poll timestamp.
 *  (G) int CheckPollTime - poll interval.
 */
START_TEST(calculate_select_timeout_test)
  {
  ForceServerUpdate = false;

  /* wait time is minimum */
  time_now = 110;
  wait_time = 9;
  LastServerUpdateTime = 100;
  ServerStatUpdateInterval = 20; /* 10 seconds till the next status update */
  last_poll_time = 90;
  CheckPollTime = 30; /* 10 seconds till the next poll */
  fail_unless(calculate_select_timeout() == 9);

  /* status update is minimum */
  time_now = 110;
  wait_time = 10;
  LastServerUpdateTime = 100;
  ServerStatUpdateInterval = 19; /* 9 seconds till the next status update */
  last_poll_time = 90;
  CheckPollTime = 30; /* 10 seconds till the next poll */
  fail_unless(calculate_select_timeout() == 9);

  /* poll is minimum */
  time_now = 110;
  wait_time = 10;
  LastServerUpdateTime = 100;
  ServerStatUpdateInterval = 20; /* 10 seconds till the next status update */
  last_poll_time = 90;
  CheckPollTime = 29; /* 9 seconds till the next poll */
  fail_unless(calculate_select_timeout() == 9);

  /* LastServerUpdateTime is zero */
  time_now = 110;
  wait_time = 10;
  LastServerUpdateTime = 0;
  ServerStatUpdateInterval = 20;
  last_poll_time = 90;
  CheckPollTime = 30; /* 10 seconds till the next poll */
  fail_unless(calculate_select_timeout() == 1);

  /* status update is minimum and was needed to be sent some time ago */
  time_now = 110;
  wait_time = 10;
  LastServerUpdateTime = 89;
  ServerStatUpdateInterval = 20; /* -1 seconds till the next status update */
  last_poll_time = 90;
  CheckPollTime = 30; /* 10 seconds till the next poll */
  fail_unless(calculate_select_timeout() == 1);

  /* poll is minimum and was needed to be sent some time ago */
  time_now = 110;
  wait_time = 10;
  LastServerUpdateTime = 100;
  ServerStatUpdateInterval = 20; /* 10 seconds till the next status update */
  last_poll_time = 79;
  CheckPollTime = 30; /* -1 seconds till the next poll */
  fail_unless(calculate_select_timeout() == 1);
  }
END_TEST


Suite *mom_main_suite(void)
  {
  Suite *s = suite_create("mom_main_suite methods");
  TCase *tc_core = tcase_create("test_read_mom_hierarchy");
  tcase_add_test(tc_core, test_read_mom_hierarchy);
  suite_add_tcase(s, tc_core);

  tc_core = tcase_create("test_mom_job_dir_sticky_config");
  tcase_add_test(tc_core, test_mom_job_dir_sticky_config);
  tcase_add_test(tc_core, test_parse_integer_range);
  suite_add_tcase(s, tc_core);

  tc_core = tcase_create("calculate_select_timeout_test");
  tcase_add_test(tc_core, calculate_select_timeout_test);
  suite_add_tcase(s, tc_core);

  tc_core = tcase_create("test_call_scan_for_exiting");
  tcase_add_test(tc_core, test_call_scan_for_exiting);
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
  sr = srunner_create(mom_main_suite());
  srunner_set_log(sr, "mom_main_suite.log");
  srunner_run_all(sr, CK_NORMAL);
  number_failed = srunner_ntests_failed(sr);
  srunner_free(sr);
  return number_failed;
  }
