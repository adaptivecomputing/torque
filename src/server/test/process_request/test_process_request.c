#include "license_pbs.h" /* See here for the software license */
#include "process_request.h"
#include "test_process_request.h"
#include <stdlib.h>
#include <stdio.h>
#include "pbs_error.h"

int process_request(struct tcp_chan *chan);
bool request_passes_acl_check(batch_request *request, unsigned long  conn_addr);
batch_request *alloc_br(int type);

extern bool check_acl;
extern bool find_node;
extern int free_attrlist_called;

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

START_TEST(test_request_passes_acl_check)
  {
  check_acl = false;
  batch_request preq;
  strcpy(preq.rq_host, "napali");

  fail_unless(request_passes_acl_check(&preq, 0) == true);
  
  find_node = true;
  check_acl = true;
  fail_unless(request_passes_acl_check(&preq, 0) == true);

  find_node = false;
  fail_unless(request_passes_acl_check(&preq, 0) == false);
  }
END_TEST

START_TEST(test_alloc_br)
  {
  batch_request *preq = alloc_br(PBS_BATCH_QueueJob);

  fail_unless(preq->rq_type == PBS_BATCH_QueueJob);
  fail_unless(preq->rq_conn == -1);
  fail_unless(preq->rq_orgconn == -1);
  fail_unless(preq->rq_reply.brp_choice == BATCH_REPLY_CHOICE_NULL);
  fail_unless(preq->rq_noreply == FALSE);
  fail_unless(preq->rq_time > 0);

  free_br(preq);
  fail_unless(free_attrlist_called > 0);
  }
END_TEST

Suite *process_request_suite(void)
  {
  Suite *s = suite_create("process_request_suite methods");
  TCase *tc_core = tcase_create("test_process_request");
  tcase_add_test(tc_core, test_process_request);
  suite_add_tcase(s, tc_core);

  tc_core = tcase_create("test_request_passes_acl_check");
  tcase_add_test(tc_core, test_request_passes_acl_check);
  tcase_add_test(tc_core, test_alloc_br);
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
