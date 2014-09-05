#include "license_pbs.h" /* See here for the software license */
#include "process_request.h"
#include "test_process_request.h"
#include <stdlib.h>
#include <stdio.h>
#include "pbs_error.h"

extern char scaff_buffer[];
extern struct connection svr_conn[];

int process_request(struct tcp_chan *chan);

START_TEST(test_process_request)
  {
  struct tcp_chan chan;

  memset(&chan, 0, sizeof(chan));
  chan.sock = -1;
  fail_unless(process_request(&chan) == PBSE_SOCKET_CLOSE);
  chan.sock = 66034;
  fail_unless(process_request(&chan) == PBSE_SOCKET_CLOSE);

  }
END_TEST

START_TEST(test_process_request_bad_host_err)
  {
  struct tcp_chan chan;

  memset(&chan, 0, sizeof(chan));
  chan.sock = 999;
  svr_conn[999].cn_addr = 167838724;
  svr_conn[999].cn_active = FromClientDIS;
  memset(scaff_buffer, 0, 1024);
  process_request(&chan);
  fail_unless(strncmp("Access from host not allowed, or unknown host:",
    scaff_buffer, strlen("Access from host not allowed, or unknown host:")) == 0, scaff_buffer);

  svr_conn[999].cn_addr = -1;
  memset(scaff_buffer, 0, 1024);
  process_request(&chan);
  fail_unless(strncmp("Access from host not allowed, or unknown host:",
    scaff_buffer, strlen("Access from host not allowed, or unknown host:")) == 0, scaff_buffer);
  }
END_TEST

Suite *process_request_suite(void)
  {
  Suite *s = suite_create("process_request_suite methods");
  TCase *tc_core = tcase_create("test_process_request");
  tcase_add_test(tc_core, test_process_request);
  suite_add_tcase(s, tc_core);

  tc_core = tcase_create("test_process_request_bad_host_err");
  tcase_add_test(tc_core, test_process_request_bad_host_err);
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
  sr = srunner_create(process_request_suite());
  srunner_set_log(sr, "process_request_suite.log");
  srunner_run_all(sr, CK_NORMAL);
  number_failed = srunner_ntests_failed(sr);
  srunner_free(sr);
  return number_failed;
  }
