#include "license_pbs.h" /* See here for the software license */
#include "mom_comm.h"
#include "test_mom_comm.h"
#include <stdlib.h>
#include <stdio.h>

#include "dis.h"
#include "pbs_error.h"
#include "pbs_nodes.h"
#include "pbs_job.h"

extern int disrsi_return_index;
extern int disrst_return_index;
extern int disrsi_array[];
extern char *disrst_array[];

received_node *get_received_node_entry(char *str);

START_TEST(test_read_status_strings_null_chan_doesnt_crash)
  {
  fail_unless(DIS_INVALID == read_status_strings(NULL, 1));
  }
END_TEST


START_TEST(test_read_status_strings_loop)
  {
  struct tcp_chan chan;
  disrst_return_index = 0;
  disrsi_return_index = 0;
  disrsi_array[0] = DIS_SUCCESS;
  disrsi_array[1] = DIS_SUCCESS;
  disrst_array[0] = strdup("jobid");
  disrst_array[1] = strdup("cookie");
  read_status_strings(&chan, 1);
  }
END_TEST

START_TEST(test_get_received_node_entry)
  {
  fail_unless(get_received_node_entry((char *)"pickle") != NULL);
  }
END_TEST

START_TEST(task_save_test)
  {
  int result = 0;
  struct task test_task;
  struct job test_job;
  const char *file_prefix = "prefix";

  memset(&test_task, 0, sizeof(test_task));
  memset(&test_job, 0, sizeof(test_job));

  result = task_save(NULL);
  fail_unless(result == PBSE_BAD_PARAMETER, "NULL input fail");

  result = task_save(&test_task);
  fail_unless(result == PBSE_BAD_PARAMETER, "NULL pointer to owning job fail");

  test_task.ti_job = &test_job;
  strncpy(test_job.ji_qs.ji_fileprefix,
          file_prefix,
          sizeof(test_job.ji_qs.ji_fileprefix) - 1);
  /*
  result = task_save(&test_task);
  fail_unless(result == -1, "task_save fail");
  */
  }
END_TEST

START_TEST(im_request_test)
  {
  struct tcp_chan test_chan;
  memset(&test_chan, 0, sizeof(test_chan));

  im_request(&test_chan, 0);

  memset(&test_chan, 0, sizeof(test_chan));
  disrsi_return_index = 0;
  disrst_return_index = 0;
  disrst_array[0] = strdup("other");
  disrst_array[1] = strdup("other");
  disrsi_array[0] = IM_KILL_JOB;
  disrsi_array[0] = 0;
  disrsi_array[1] = 0;
  im_request(&test_chan, IM_PROTOCOL_VER);

  memset(&test_chan, 0, sizeof(test_chan));
  disrsi_return_index = 0;
  disrst_return_index = 0;
  disrst_array[0] = strdup("jobid");
  disrst_array[1] = strdup("cookie");
  disrsi_array[0] = IM_ALL_OKAY;
  disrsi_array[0] = 0;
  disrsi_array[1] = 0;
  im_request(&test_chan, IM_PROTOCOL_VER);

  memset(&test_chan, 0, sizeof(test_chan));
  disrsi_return_index = 0;
  disrst_return_index = 0;
  disrst_array[0] = strdup("jobid");
  disrst_array[1] = strdup("cookie");
  disrsi_array[0] = IM_JOIN_JOB;
  im_request(&test_chan, IM_PROTOCOL_VER);

  memset(&test_chan, 0, sizeof(test_chan));
  disrsi_return_index = 0;
  disrst_return_index = 0;
  disrst_array[0] = strdup("jobid");
  disrst_array[1] = strdup("cookie");
  disrsi_array[0] = IM_KILL_JOB;
  im_request(&test_chan, IM_PROTOCOL_VER);

  memset(&test_chan, 0, sizeof(test_chan));
  disrsi_return_index = 0;
  disrst_return_index = 0;
  disrst_array[0] = strdup("jobid");
  disrst_array[1] = strdup("cookie");
  disrsi_array[0] = IM_SPAWN_TASK;
  im_request(&test_chan, IM_PROTOCOL_VER);

  memset(&test_chan, 0, sizeof(test_chan));
  disrsi_return_index = 0;
  disrst_return_index = 0;
  disrst_array[0] = strdup("jobid");
  disrst_array[1] = strdup("cookie");
  disrsi_array[0] = IM_GET_TASKS;
  im_request(&test_chan, IM_PROTOCOL_VER);

  memset(&test_chan, 0, sizeof(test_chan));
  disrsi_return_index = 0;
  disrst_return_index = 0;
  disrst_array[0] = strdup("jobid");
  disrst_array[1] = strdup("cookie");
  disrsi_array[0] = IM_SIGNAL_TASK;
  im_request(&test_chan, IM_PROTOCOL_VER);

  memset(&test_chan, 0, sizeof(test_chan));
  disrsi_return_index = 0;
  disrst_return_index = 0;
  disrst_array[0] = strdup("jobid");
  disrst_array[1] = strdup("cookie");
  disrsi_array[0] = IM_OBIT_TASK;
  im_request(&test_chan, IM_PROTOCOL_VER);

  memset(&test_chan, 0, sizeof(test_chan));
  disrsi_return_index = 0;
  disrst_return_index = 0;
  disrst_array[0] = strdup("jobid");
  disrst_array[1] = strdup("cookie");
  disrsi_array[0] = IM_POLL_JOB;
  im_request(&test_chan, IM_PROTOCOL_VER);

  memset(&test_chan, 0, sizeof(test_chan));
  disrsi_return_index = 0;
  disrst_return_index = 0;
  disrst_array[0] = strdup("jobid");
  disrst_array[1] = strdup("cookie");
  disrsi_array[0] = IM_GET_INFO;
  im_request(&test_chan, IM_PROTOCOL_VER);

  memset(&test_chan, 0, sizeof(test_chan));
  disrsi_return_index = 0;
  disrst_return_index = 0;
  disrst_array[0] = strdup("jobid");
  disrst_array[1] = strdup("cookie");
  disrsi_array[0] = IM_GET_RESC;
  im_request(&test_chan, IM_PROTOCOL_VER);

  memset(&test_chan, 0, sizeof(test_chan));
  disrsi_return_index = 0;
  disrst_return_index = 0;
  disrst_array[0] = strdup("jobid");
  disrst_array[1] = strdup("cookie");
  disrsi_array[0] = IM_ABORT_JOB;
  im_request(&test_chan, IM_PROTOCOL_VER);

  memset(&test_chan, 0, sizeof(test_chan));
  disrsi_return_index = 0;
  disrst_return_index = 0;
  disrst_array[0] = strdup("jobid");
  disrst_array[1] = strdup("cookie");
  disrsi_array[0] = IM_GET_TID;
  im_request(&test_chan, IM_PROTOCOL_VER);

  memset(&test_chan, 0, sizeof(test_chan));
  disrsi_return_index = 0;
  disrst_return_index = 0;
  disrst_array[0] = strdup("jobid");
  disrst_array[1] = strdup("cookie");
  disrsi_array[0] = IM_RADIX_ALL_OK;
  im_request(&test_chan, IM_PROTOCOL_VER);

  memset(&test_chan, 0, sizeof(test_chan));
  disrsi_return_index = 0;
  disrst_return_index = 0;
  disrst_array[0] = strdup("jobid");
  disrst_array[1] = strdup("cookie");
  disrsi_array[0] = IM_JOIN_JOB_RADIX;
  im_request(&test_chan, IM_PROTOCOL_VER);

  memset(&test_chan, 0, sizeof(test_chan));
  disrsi_return_index = 0;
  disrst_return_index = 0;
  disrst_array[0] = strdup("jobid");
  disrst_array[1] = strdup("cookie");
  disrsi_array[0] = IM_KILL_JOB_RADIX;
  im_request(&test_chan, IM_PROTOCOL_VER);

  memset(&test_chan, 0, sizeof(test_chan));
  disrsi_return_index = 0;
  disrst_return_index = 0;
  disrst_array[0] = strdup("jobid");
  disrst_array[1] = strdup("cookie");
  disrsi_array[0] = IM_MAX;
  im_request(&test_chan, IM_PROTOCOL_VER);

  memset(&test_chan, 0, sizeof(test_chan));
  disrsi_return_index = 0;
  disrst_return_index = 0;
  disrst_array[0] = strdup("jobid");
  disrst_array[1] = strdup("cookie");
  disrsi_array[0] = IM_ERROR;
  im_request(&test_chan, IM_PROTOCOL_VER);
  }
END_TEST

START_TEST(im_join_job_as_sister_test)
  {
  int result = -1;
  const char *test_job_id = "not_jobid";
  const char *test_cookie = "cookie";
  struct tcp_chan test_chan;
  struct sockaddr_in test_sock_addr;

  memset(&test_chan, 0, sizeof(test_chan));
  memset(&test_sock_addr, 0, sizeof(test_sock_addr));

  result = im_join_job_as_sister(&test_chan,
                                 (char *)test_job_id,
                                 &test_sock_addr,
                                 (char *)test_cookie,
                                 0,
                                 0,
                                 0,
                                 0);
  fail_unless(result==0);
  }
END_TEST

START_TEST(tm_spawn_request_test)
  {
  struct tcp_chan test_chan;
  struct job test_job;
  struct hnodent test_hnodent;
  const char *test_cookie = "cookie";
  int reply = 0;
  int ret = 0;
  int result = 0;

  memset(&test_chan, 0, sizeof(test_chan));
  memset(&test_job, 0, sizeof(test_job));
  memset(&test_hnodent, 0, sizeof(test_hnodent));

  result = tm_spawn_request(&test_chan,
                            &test_job,
                            0,
                            0,
                            (char *)test_cookie,
                            &reply,
                            &ret,
                            0,
                            &test_hnodent,
                            0);

  fail_unless(result == TM_DONE, "tm_spawn_request fail: %d", result);
  }
END_TEST

Suite *mom_comm_suite(void)
  {
  Suite *s = suite_create("mom_comm_suite methods");
  TCase *tc_core = tcase_create("mom_comm");

  tc_core = tcase_create("test_read_status_strings_null_chan_doesnt_crash");
  tcase_add_test(tc_core, test_read_status_strings_null_chan_doesnt_crash);
  suite_add_tcase(s, tc_core);

  tc_core = tcase_create("test_read_status_strings_loop");
  tcase_add_test(tc_core, test_read_status_strings_loop);
  suite_add_tcase(s, tc_core);

  tc_core = tcase_create("test_get_received_node_entry");
  tcase_add_test(tc_core, test_get_received_node_entry);
  suite_add_tcase(s, tc_core);

  tc_core = tcase_create("task_save_test");
  tcase_add_test(tc_core, task_save_test);
  suite_add_tcase(s, tc_core);

  tc_core = tcase_create("im_request_test");
  tcase_add_test(tc_core, im_request_test);
  suite_add_tcase(s, tc_core);

  tc_core = tcase_create("im_join_job_as_sister_test");
  tcase_add_test(tc_core, im_join_job_as_sister_test);
  suite_add_tcase(s, tc_core);

  tc_core = tcase_create("tm_spawn_request_test");
  tcase_add_test(tc_core, tm_spawn_request_test);
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
  sr = srunner_create(mom_comm_suite());
  srunner_set_log(sr, "mom_comm_suite.log");
  srunner_run_all(sr, CK_NORMAL);
  number_failed = srunner_ntests_failed(sr);
  srunner_free(sr);
  return number_failed;
  }
