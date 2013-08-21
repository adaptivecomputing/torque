#include "license_pbs.h" /* See here for the software license */
#include "lib_ifl.h"
#include "test_trq_auth.h"
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include "pbs_error.h"
#include "errno.h"
#include <sys/types.h>
#include <pwd.h>

#define getsockopt getsockopt

bool    connect_success;
bool    getaddrinfo_success;
bool    socket_success;
bool    setsockopt_success;
bool    close_success;
bool    write_success;
bool    socket_read_success;
bool    socket_read_num_success;
bool    getsockopt_success;
bool    tcp_priv_success;
bool    socket_connect_success;
bool    DIS_success;

extern   int request_type;
extern   int process_svr_conn_rc;

int get_active_pbs_server(char **active_server);

extern time_t pbs_tcp_timeout;
extern char   *my_active_server;

START_TEST(get_active_pbs_server_test)
  {
  char *server_name;
  int  rc;
  
  connect_success = true;
  getaddrinfo_success = true;
  socket_success = true;
  setsockopt_success = true;
  close_success = true;
  write_success = true;
  socket_read_success = true;
  socket_read_num_success = true;
  getsockopt_success = true;
  tcp_priv_success = true;
  socket_connect_success = true;
  DIS_success = true;

  setenv("PBSAPITIMEOUT", "3", 1);
  get_active_pbs_server(&server_name);
  fail_unless(pbs_tcp_timeout == 3);

  socket_success = false;
  rc = get_active_pbs_server(&server_name);
  fail_unless(rc != PBSE_NONE, "get_active_pbs_server bad socket", rc);

  socket_success = true;
  connect_success = false;
  rc = get_active_pbs_server(&server_name);
  fail_unless(rc != PBSE_NONE, "get_active_pbs_server bad connect", rc);

  write_success = false;
  connect_success = true;
  rc = get_active_pbs_server(&server_name);
  fail_unless(rc != PBSE_NONE, "get_active_pbs_server bad connect", rc);

  write_success = true;
  socket_read_success = false;
  rc = get_active_pbs_server(&server_name);
  fail_unless(rc != PBSE_NONE, "get_active_pbs_server bad connect", rc);

  }
END_TEST

START_TEST(test_trq_simple_connect)
  {
  const char *server_name = "localhost";
  int         batch_port = 15001;
  int         handle = -1;
  int         rc;

  connect_success = true;
  getaddrinfo_success = true;
  socket_success = true;
  setsockopt_success = true;
  close_success = true;
  write_success = true;
  socket_read_success = true;
  socket_read_num_success = true;
  getsockopt_success = true;
  tcp_priv_success = true;
  socket_connect_success = true;
  DIS_success = true;


  rc = trq_simple_connect(server_name, batch_port, &handle);
  fail_unless(rc == PBSE_NONE, "trq_simple_connect failed success case", rc);
  fail_unless(handle >= 0, "trq_simple_connect returned invalid handle", handle);
  trq_simple_disconnect(handle);

  socket_success = false;
  rc = trq_simple_connect(server_name, batch_port, &handle);
  fail_unless(rc == PBSE_SYSTEM, "trq_simple_connect failed failed socket call", rc);

  socket_success = true;
  setsockopt_success = false;
  rc = trq_simple_connect(server_name, batch_port, &handle);
  fail_unless(rc == PBSE_SYSTEM, "trq_simple_connect failed failed setsockopt call", rc);

  setsockopt_success = true;
  connect_success = false;
  rc = trq_simple_connect(server_name, batch_port, &handle);
  fail_unless(rc != PBSE_NONE, "trq_simple_connect failed failed connect call", rc);

  }
END_TEST

START_TEST(test_trq_simple_disconnect)
  {
  int         handle = 1;
  int         rc;

  connect_success = true;
  getaddrinfo_success = true;
  socket_success = true;
  setsockopt_success = true;
  close_success = true;
  write_success = true;
  socket_read_success = true;
  socket_read_num_success = true;
  getsockopt_success = true;
  tcp_priv_success = true;
  socket_connect_success = true;
  DIS_success = true;

  rc = trq_simple_disconnect(handle);
  fail_unless(rc == PBSE_NONE, "trq_simple_disconnect failed success case", rc);


  }
END_TEST

START_TEST(test_validate_server)
  {
  char  active_server_name[PBS_MAXHOSTNAME+1];
  int   port = 15001;
  char *ssh_key = NULL;
  char *sign_key = NULL;
  int   rc;

  connect_success = true;
  getaddrinfo_success = true;
  socket_success = true;
  setsockopt_success = true;
  close_success = true;
  write_success = true;
  socket_read_success = true;
  socket_read_num_success = true;
  getsockopt_success = true;
  tcp_priv_success = true;
  socket_connect_success = true;
  DIS_success = true;

  strcpy(active_server_name, "localhost");
  rc = validate_server(active_server_name, port, ssh_key, &sign_key);
  fail_unless(rc == PBSE_NONE, "validate_server success case failed", rc);

  }
END_TEST

START_TEST(test_set_active_pbs_server)
  {
  char new_server_name[PBS_MAXHOSTNAME + 1];
  int rc;

  strcpy(new_server_name, "localhost");
  rc = set_active_pbs_server(new_server_name);
  fail_unless(rc == PBSE_NONE, "set_active_pbs_server failed", rc);

  }
END_TEST

START_TEST(test_validate_active_pbs_server)
  {
  int rc;
  char *active_server;
  int  port = 15001;

  connect_success = true;
  getaddrinfo_success = true;
  socket_success = true;
  setsockopt_success = true;
  close_success = true;
  write_success = true;
  socket_read_success = true;
  socket_read_num_success = true;
  getsockopt_success = true;
  tcp_priv_success = true;
  socket_connect_success = true;
  DIS_success = true;

  rc = validate_active_pbs_server(&active_server, port);
  fail_unless(rc == PBSE_NONE, "validate_active_pbs_server failed", rc);

  socket_success = false;
  rc = validate_active_pbs_server(&active_server, port);
  fail_unless(rc != PBSE_NONE, "validate_active_pbs_server bad socket", rc);

  socket_success = true;;
  connect_success = false;
  rc = validate_active_pbs_server(&active_server, port);
  fail_unless(rc != PBSE_NONE, "validate_active_pbs_server bad connect", rc);

  write_success = false;
  connect_success = true;
  rc = validate_active_pbs_server(&active_server, port);
  fail_unless(rc != PBSE_NONE, "validate_active_pbs_server bad write", rc);

  write_success = true;
  socket_read_success = false;
  rc = validate_active_pbs_server(&active_server, port);
  fail_unless(rc != PBSE_NONE, "validate_active_pbs_server bad write", rc);

  }
END_TEST


START_TEST(test_process_svr_conn)
  {
  int *sock;

  connect_success = true;
  getaddrinfo_success = true;
  socket_success = true;
  setsockopt_success = true;
  close_success = true;
  write_success = true;
  socket_read_success = true;
  socket_read_num_success = true;
  getsockopt_success = true;
  tcp_priv_success = true;
  socket_connect_success = true;
  DIS_success = true;

  sock = (int *)calloc(1, sizeof(int));
  *sock = 20;
  request_type = -1;

  /* a request_tyupe of -1 will cause a failure on the reading of the incoming request */
  (*process_svr_conn)((void *)sock);
  fail_unless(process_svr_conn_rc != PBSE_NONE, "process_svr_conn socket_read_num does not fail as expected");

  /* Test with an invalid request_type */
  process_svr_conn_rc = PBSE_NONE;
  sock = (int *)calloc(1, sizeof(int));
  *sock = 20;
  request_type = 10;
  (*process_svr_conn)((void *)sock);
  fail_unless(process_svr_conn_rc == PBSE_IVALREQ, "process_svr_conn did not fail as expected with invalid request type", process_svr_conn_rc); 


  /* Test the success case for TRQ_GET_ACTIVE_SERVER */
  process_svr_conn_rc = PBSE_NONE;
  sock = (int *)calloc(1, sizeof(int));
  *sock = 20;
  request_type = TRQ_GET_ACTIVE_SERVER;
  (*process_svr_conn)((void *)sock);
  fail_unless(process_svr_conn_rc == PBSE_NONE, "TRQ_GET_ACTIVE_SERVER failed");

  sock = (int *)calloc(1, sizeof(int));
  *sock = 20;
  request_type = TRQ_DOWN_TRQAUTHD;
  (*process_svr_conn)((void *)sock);
  fail_unless(process_svr_conn_rc == PBSE_NONE, "TRQ_GET_ACTIVE_SERVER failed");


  /* Test the success case for TRQ_VALIDATE_ACTIVE_SERVER */
  sock = (int *)calloc(1, sizeof(int));
  *sock = 20;
  socket_success = true;
  request_type = TRQ_VALIDATE_ACTIVE_SERVER;
  (*process_svr_conn)((void *)sock);
  fail_unless(((process_svr_conn_rc == PBSE_NONE) || (process_svr_conn_rc == PBSE_UNKREQ)), "TRQ_VALIDATE_ACTIVE_SERVER failed"); 

  /* Test failure case for TRQ_VALIDATE_ACTIVE_SERVER with bad socket */
  socket_success = false;
  sock = (int *)calloc(1, sizeof(int));
  *sock = 20;
  request_type = TRQ_VALIDATE_ACTIVE_SERVER;
  (*process_svr_conn)((void *)sock);
  fail_unless(process_svr_conn_rc == PBSE_NONE, "TRQ_VALIDATE_ACIVE_SERVER failed");

  /* Test the success case for TRQ_AUTH_CONNECTION */
  sock = (int *)calloc(1, sizeof(int));
  *sock = 20;
  socket_success = true;
  request_type = TRQ_AUTH_CONNECTION;
  (*process_svr_conn)((void *)sock);
  fail_unless(process_svr_conn_rc == PBSE_NONE, "TRQ_AUTH_CONNECTION failed");
  
  /* Test when socket read fails */
  socket_read_success = false;
  sock = (int *)calloc(1, sizeof(int));
  *sock = 20;
  request_type = TRQ_AUTH_CONNECTION;
  (*process_svr_conn)((void *)sock);
  fail_unless(process_svr_conn_rc != PBSE_NONE, "TRQ_AUTH_CONNECTION failed");

  /* Test when validate_user fails */
  socket_read_success = true;
  getsockopt_success = false;
  sock = (int *)calloc(1, sizeof(int));
  *sock = 20;
  request_type = TRQ_AUTH_CONNECTION;
  (*process_svr_conn)((void *)sock);
  fail_unless(process_svr_conn_rc != PBSE_NONE, "TRQ_AUTH_CONNECTION failed");

  /* Test when socket_get_tcp_priv fails */
  getsockopt_success = true;
  tcp_priv_success = false;
  sock = (int *)calloc(1, sizeof(int));
  *sock = 20;
  request_type = TRQ_AUTH_CONNECTION;
  (*process_svr_conn)((void *)sock);
  fail_unless(process_svr_conn_rc != PBSE_NONE, "TRQ_AUTH_CONNECTION failed");

  /* Test when socket_connect fails */
  tcp_priv_success = true;
  socket_connect_success = false;
  sock = (int *)calloc(1, sizeof(int));
  *sock = 20;
  request_type = TRQ_AUTH_CONNECTION;
  (*process_svr_conn)((void *)sock);
  fail_unless(process_svr_conn_rc != PBSE_NONE, "TRQ_AUTH_CONNECTION failed");

  /* Test when socket_write fails */
  socket_connect_success = true;
  write_success = false;
  sock = (int *)calloc(1, sizeof(int));
  *sock = 20;
  request_type = TRQ_AUTH_CONNECTION;
  (*process_svr_conn)((void *)sock);
  fail_unless(process_svr_conn_rc != PBSE_NONE, "TRQ_AUTH_CONNECTION failed");

  /* Test when socket_write fails */
  write_success = true;
  DIS_success = false;
  sock = (int *)calloc(1, sizeof(int));
  *sock = 20;
  request_type = TRQ_AUTH_CONNECTION;
  (*process_svr_conn)((void *)sock);
  fail_unless(process_svr_conn_rc != PBSE_NONE, "TRQ_AUTH_CONNECTION failed");
  }
END_TEST 

START_TEST(test_send_svr_disconnect)
  {
  int sock = 10;
  char *user = NULL;


  user = (char *)malloc(10);
  fail_unless(user != NULL);
  strcpy(user, "fred");
  send_svr_disconnect(sock, user);
  free(user);

  }
END_TEST


START_TEST(test_two)
  {

  }
END_TEST


Suite *trq_auth_suite(void)
  {
  Suite *s = suite_create("trq_auth_suite methods");
  TCase *tc_core = tcase_create("get_active_pbs_server_test");
  tcase_add_test(tc_core, get_active_pbs_server_test);
  suite_add_tcase(s, tc_core);

  tc_core = tcase_create("test_trq_simple_connect");
  tcase_add_test(tc_core, test_trq_simple_connect);
  suite_add_tcase(s, tc_core);

  tc_core = tcase_create("test_trq_simple_disconnect");
  tcase_add_test(tc_core, test_trq_simple_disconnect);
  suite_add_tcase(s, tc_core);

  tc_core = tcase_create("test_validate_server");
  tcase_add_test(tc_core, test_validate_server);
  suite_add_tcase(s, tc_core);

  tc_core = tcase_create("test_set_active_pbs_server");
  tcase_add_test(tc_core, test_set_active_pbs_server);
  suite_add_tcase(s, tc_core);

  tc_core = tcase_create("test_validate_active_pbs_server");
  tcase_add_test(tc_core, test_validate_active_pbs_server);
  suite_add_tcase(s, tc_core);

  tc_core = tcase_create("test_process_svr_conn");
  tcase_add_test(tc_core, test_process_svr_conn);
  suite_add_tcase(s, tc_core);

  tc_core = tcase_create("test_send_svr_disconnect");
  tcase_add_test(tc_core, test_send_svr_disconnect);
  suite_add_tcase(s, tc_core);

  tc_core = tcase_create("test_two");
  tcase_add_test(tc_core, test_two);
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
  sr = srunner_create(trq_auth_suite());
  srunner_set_log(sr, "trq_auth_suite.log");
  srunner_run_all(sr, CK_NORMAL);
  number_failed = srunner_ntests_failed(sr);
  srunner_free(sr);
  return number_failed;
  }
