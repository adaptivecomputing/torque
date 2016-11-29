#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string>
#include <sstream>
#include "pbs_config.h"
#include "mom_main.h"
#include "mom_config.h"
#include "mom_func.h"
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
extern int MOMCudaVisibleDevices;
extern bool daemonize_mom;

time_t pbs_tcp_timeout;
unsigned long setcudavisibledevices(const char *);
void set_report_mom_cuda_visible_devices(std::stringstream &output, char *curr);
void read_mom_hierarchy();
int  parse_integer_range(const char *range_str, int &start, int &end);
time_t calculate_select_timeout();
int process_layout_request(tcp_chan *chan);
bool should_resend_obit(job *pjob, int diff);
void check_job_in_mom_wait(job *pjob);
void evaluate_job_in_prerun(job *pjob);

extern attribute_def job_attr_def[];
extern int  exiting_tasks;
extern int  job_exit_wait_time;

void check_job_substates(bool &call_exiting);
extern tlist_head svr_alljobs;
extern int wsi_ret;
extern int wcs_ret;
extern int flush_ret;
extern int job_bailed;
extern bool am_i_ms;

bool are_we_forking()

  {
  char *forking = getenv("CK_FORK");

  if ((forking != NULL) &&  
      (!strcasecmp(forking, "no")))
    return(false);

  return(true);
  }

void set_optind()

  {
  if (are_we_forking() == false)
    optind = 0;
  }

int encode_fake(

  pbs_attribute  *attr,    /* ptr to pbs_attribute */
  tlist_head     *phead,   /* head of attrlist */
  const char     *atname,  /* name of pbs_attribute */
  const char     *rsname,  /* resource name or null */
  int             mode,    /* encode mode, unused here */
  int             perm)    /* only used for resources */

  {
  return(0);
  }


START_TEST(test_evaluate_job_in_prerun)
  {
  job    pjob;

  time_now = time(NULL);

  max_join_job_wait_time = 30;
  resend_join_job_wait_time = 15;

  // Set this so we don't actually try to re-connect to the sisters
  pjob.ji_numnodes = 1;
  pjob.ji_qs.ji_state = JOB_STATE_RUNNING;
  pjob.ji_qs.ji_substate = JOB_SUBSTATE_PRERUN;
  pjob.ji_joins_sent = time_now - max_join_job_wait_time - 1;
  pjob.ji_joins_resent = FALSE;
  am_i_ms = false;
  job_bailed = 0;

  // If I'm not mother superior, we shouldn't do anything
  evaluate_job_in_prerun(&pjob);
  fail_unless(job_bailed == 0);

  // Make me mother superior, now we should do something
  am_i_ms = true;
  pjob.ji_joins_resent = FALSE;
  evaluate_job_in_prerun(&pjob);
  fail_unless(job_bailed == 1);
  
  // If my state isn't running, we shouldn't do anything
  pjob.ji_qs.ji_state = JOB_STATE_QUEUED;
  pjob.ji_joins_resent = FALSE;
  evaluate_job_in_prerun(&pjob);
  fail_unless(job_bailed == 1); // shouldn't change

  // I have to set this up so we don't segfault
  for (int i = 0; i < JOB_ATR_LAST; i++)
    job_attr_def[i].at_encode = encode_fake;
  
  // Now make me re-send the joins
  pjob.ji_qs.ji_state = JOB_STATE_RUNNING;
  pjob.ji_joins_sent = time_now - resend_join_job_wait_time - 1;
  pjob.ji_joins_resent = FALSE;
  evaluate_job_in_prerun(&pjob);
  fail_unless(pjob.ji_joins_resent == TRUE);
  fail_unless(job_bailed == 1); // shouldn't change

  }
END_TEST


START_TEST(test_check_job_in_mom_wait)
  {
  job    pjob;

  memset(&pjob, 0, sizeof(pjob));
  pjob.ji_qs.ji_substate = JOB_SUBSTATE_MOM_WAIT;

  time_now = time(NULL);

  // Make sure we don't transition if ji_kill_started == 0
  check_job_in_mom_wait(&pjob);
  fail_unless(pjob.ji_qs.ji_substate == JOB_SUBSTATE_MOM_WAIT);
  
  // Make sure we have to wait the specified timeout
  pjob.ji_kill_started = time_now - job_exit_wait_time;
  check_job_in_mom_wait(&pjob);
  fail_unless(pjob.ji_qs.ji_substate == JOB_SUBSTATE_MOM_WAIT);

  // Make sure we transition once we pass it
  pjob.ji_kill_started = time_now - job_exit_wait_time - 1;
  check_job_in_mom_wait(&pjob);
  fail_unless(pjob.ji_qs.ji_substate == JOB_SUBSTATE_EXITING);
  }
END_TEST


START_TEST(test_should_resend_obit)
  {
  job    pjob;
  int    diff = 10;
  time_now = time(NULL);

  memset(&pjob, 0, sizeof(pjob));
  pjob.ji_obit_sent = time_now;

  // Running jobs shouldn't re-send their obits
  pjob.ji_qs.ji_substate = JOB_SUBSTATE_RUNNING;
  fail_unless(should_resend_obit(&pjob, diff) == false);
  pjob.ji_obit_busy_time = time_now - (2 * diff);
  fail_unless(should_resend_obit(&pjob, diff) == false);

  // Being past the busy wait time should trigger re-sending for any of the states
  pjob.ji_qs.ji_substate = JOB_SUBSTATE_OBIT;
  fail_unless(should_resend_obit(&pjob, diff) == true);
  pjob.ji_qs.ji_substate = JOB_SUBSTATE_EXITED;
  fail_unless(should_resend_obit(&pjob, diff) == true);
  pjob.ji_qs.ji_substate = JOB_SUBSTATE_EXITING;
  fail_unless(should_resend_obit(&pjob, diff) == true);
  
  pjob.ji_obit_busy_time = 0;
  pjob.ji_qs.ji_substate = JOB_SUBSTATE_OBIT;
  pjob.ji_obit_sent = time_now - 1;
  // This shouldn't put us past waiting
  fail_unless(should_resend_obit(&pjob, diff) == false);
  pjob.ji_obit_sent -= 10; // the wait time
  fail_unless(should_resend_obit(&pjob, diff) == true);
  pjob.ji_obit_sent = time_now;

  // Exiting jobs should retry if they received a minus one and are past waiting
  // This shouldn't put us past waiting
  pjob.ji_qs.ji_substate = JOB_SUBSTATE_EXITING;
  pjob.ji_obit_minus_one_time = time_now - 1;
  fail_unless(should_resend_obit(&pjob, diff) == false);
  pjob.ji_obit_minus_one_time -= 15; // the wait time
  fail_unless(should_resend_obit(&pjob, diff) == true);
  pjob.ji_obit_minus_one_time = 0;

  // Exited jobs should receive a response within 30 seconds even when the server is slammed
  pjob.ji_qs.ji_substate = JOB_SUBSTATE_EXITED;
  pjob.ji_exited_time = time_now - 1;
  fail_unless(should_resend_obit(&pjob, diff) == false);
  pjob.ji_exited_time -= 30; // the wait time
  fail_unless(should_resend_obit(&pjob, diff) == true);
  }
END_TEST


START_TEST(test_process_layout_request)
  {
  wsi_ret = -1;
  fail_unless(process_layout_request(NULL) == -1, "diswsi failure should trigger failure");

  wsi_ret = 0;
  wcs_ret = -1;
  fail_unless(process_layout_request(NULL) == -1, "diswcs failure should trigger failure");
  
  wcs_ret = 0;
  flush_ret = -1;
  fail_unless(process_layout_request(NULL) == -1, "flush failure should trigger failure");
  
  flush_ret = 0;
  fail_unless(process_layout_request(NULL) == 0, "Should've succeeded");
  }
END_TEST


START_TEST(test_read_mom_hierarchy)
  {
  system("rm -f bob");
  path_mom_hierarchy = strdup("bob");
  read_mom_hierarchy();
  fail_unless(received_cluster_addrs == false);
  //system("touch bob");
  // the following lines need more spoofing in order to work correctly
//  parsing_hierarchy = true;
//  read_mom_hierarchy();
//  fail_unless(received_cluster_addrs == true);
//  parsing_hierarchy = false;
  }
END_TEST


START_TEST(test_check_job_substates)
  {
  bool check_exiting = false;
  exiting_tasks = true;
  
  check_job_substates(check_exiting);
  fail_unless(check_exiting == true);

  exiting_tasks = false;

  job *job1 = (job *)calloc(1, sizeof(job));
  job *job2 = (job *)calloc(1, sizeof(job));
  job *job3 = (job *)calloc(1, sizeof(job));

  alljobs_list.push_back(job1);
  alljobs_list.push_back(job2);
  alljobs_list.push_back(job3);

  check_job_substates(check_exiting);
  fail_unless(check_exiting == false);

  job2->ji_qs.ji_substate = JOB_SUBSTATE_EXITING;

  check_job_substates(check_exiting);
  fail_unless(check_exiting == true);

  alljobs_list.clear();
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
  /*
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
  fail_unless(MOMJobDirStickySet == 0, "Failed to detect MOMJobDirStickySet was unset"); */
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


START_TEST(test_parse_command_line1)
  {
  char *argv[] = {strdup("pbs_mom"), strdup("-D")};
  set_optind();

  daemonize_mom = true;
  parse_command_line(2, argv);
  fail_unless(daemonize_mom == false);
  }
END_TEST


START_TEST(test_parse_command_line2)
  {
  char *argv[] = {strdup("pbs_mom"), strdup("-F")};
  set_optind();

  daemonize_mom = true;
  parse_command_line(2, argv);
  fail_unless(daemonize_mom == false);
  }
END_TEST


START_TEST(test_parse_command_line3)
  {
  char *argv[] = {strdup("pbs_mom")};
  set_optind();

  daemonize_mom = true;
  parse_command_line(1, argv);
  fail_unless(daemonize_mom == true);
  }
END_TEST


Suite *mom_main_suite(void)
  {
  Suite *s = suite_create("mom_main_suite methods");
  TCase *tc_core = tcase_create("test_read_mom_hierarchy");
  tcase_add_test(tc_core, test_read_mom_hierarchy);
  tcase_add_test(tc_core, test_process_layout_request);
  suite_add_tcase(s, tc_core);

  tc_core = tcase_create("test_mom_job_dir_sticky_config");
  tcase_add_test(tc_core, test_mom_job_dir_sticky_config);
  tcase_add_test(tc_core, test_parse_integer_range);
  tcase_add_test(tc_core, test_check_job_in_mom_wait);
  suite_add_tcase(s, tc_core);

  tc_core = tcase_create("calculate_select_timeout_test");
  tcase_add_test(tc_core, calculate_select_timeout_test);
  tcase_add_test(tc_core, test_evaluate_job_in_prerun);
  suite_add_tcase(s, tc_core);

  tc_core = tcase_create("test_check_job_substates");
  tcase_add_test(tc_core, test_check_job_substates);
  tcase_add_test(tc_core, test_should_resend_obit);
  suite_add_tcase(s, tc_core);

  tc_core = tcase_create("test_parse_command_line1");
  tcase_add_test(tc_core, test_parse_command_line1);
  suite_add_tcase(s, tc_core);

  tc_core = tcase_create("test_parse_command_line2");
  tcase_add_test(tc_core, test_parse_command_line2);
  suite_add_tcase(s, tc_core);

  tc_core = tcase_create("test_parse_command_line3");
  tcase_add_test(tc_core, test_parse_command_line3);
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
