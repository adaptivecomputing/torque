#include "license_pbs.h" /* See here for the software license */
#include <stdlib.h>
#include <pthread.h>
#include <stdio.h>
#include "pbs_error.h"
#include "process_request.h"
#include "test_process_request.h"
#include "net_connect.h"

extern char scaff_buffer[];
extern struct connection svr_conn[];

int process_request(struct tcp_chan *chan);
bool request_passes_acl_check(batch_request *request, unsigned long  conn_addr);
batch_request *alloc_br(int type);
batch_request *read_request_from_socket(tcp_chan *chan);

extern bool check_acl;
extern bool find_node;
extern bool fail_get_connecthost;
extern struct connection svr_conn[];
extern char server_name[];
extern int free_attrlist_called;
extern int dis_req_read_rc;
extern bool fail_check;

void initialize_svr_conn(

  int i)

  {
  svr_conn[i].cn_mutex = (pthread_mutex_t *)calloc(1, sizeof(pthread_mutex_t));
  pthread_mutex_init(svr_conn[i].cn_mutex, NULL);
  svr_conn[i].cn_socktype = PBS_SOCK_UNIX;
  svr_conn[i].cn_addr = 5;
  }

void set_connection_type(

  int i,
  enum conn_type ct)

  {
  svr_conn[i].cn_active = ct;
  }

START_TEST(test_read_request_from_socket)
  {
  tcp_chan chan;

  memset(&chan, 0, sizeof(chan));

  chan.sock = -1;
  fail_unless(read_request_from_socket(&chan) == NULL, "invalid socket (-1) should return NULL");

  chan.sock = PBS_NET_MAX_CONNECTIONS;
  fail_unless(read_request_from_socket(&chan) == NULL, "invalid socket (PBS_NET_MAX_CONNECTIONS) should return NULL");

  chan.sock = 1;
  initialize_svr_conn(chan.sock);
  set_connection_type(chan.sock, Idle);
  fail_unless(read_request_from_socket(&chan) == NULL, "Idle connection type should fail");

  strcpy(server_name, "napali");
  set_connection_type(chan.sock, ToServerDIS);
  find_node = false;
  check_acl = true;
  fail_check = false;
  fail_unless(read_request_from_socket(&chan) == NULL, "Node not found should fail");
  fail_check = true;
  find_node = true;
  check_acl = false;
  fail_unless(read_request_from_socket(&chan) != NULL, "should pass");

  fail_get_connecthost = true;
  fail_unless(read_request_from_socket(&chan) == NULL, "fail if can't get connect host");

  dis_req_read_rc = PBSE_SYSTEM;
  batch_request *preq;
  fail_unless((preq = read_request_from_socket(&chan)) != NULL, "should get bad request");
  fail_unless(preq->rq_type == PBS_BATCH_Disconnect);

  dis_req_read_rc = 5;
  fail_unless((preq = read_request_from_socket(&chan)) != NULL, "should get bad request");
  fail_unless(preq->rq_failcode == dis_req_read_rc);
  }
END_TEST

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

START_TEST(test_process_request_bad_host_err)
  {
  struct tcp_chan chan;

  memset(&chan, 0, sizeof(chan));
  chan.sock = 999;
  svr_conn[999].cn_addr = 167838724;
  svr_conn[999].cn_active = FromClientDIS;
  memset(scaff_buffer, 0, 1024);
  process_request(&chan);
  const char *err = "Access from host not allowed";
  fail_unless(strncmp(err, scaff_buffer, strlen(err)) == 0, "Expected '%s', received '%s'", err, scaff_buffer);

  svr_conn[999].cn_addr = -1;
  memset(scaff_buffer, 0, 1024);
  process_request(&chan);
  fail_unless(strncmp(err, scaff_buffer, strlen(err)) == 0, "Expected '%s', received '%s'", err, scaff_buffer);
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

  tc_core = tcase_create("test_process_request_bad_host_err");
  tcase_add_test(tc_core, test_process_request_bad_host_err);
  tcase_add_test(tc_core ,test_read_request_from_socket);
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
