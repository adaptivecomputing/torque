#include <string>
#include <iostream>
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
bool    gethostname_success;
bool    get_hostaddr_success;
bool    getpwuid_success;
bool    trqauthd_terminate_success;

extern   int request_type;
extern   int process_svr_conn_rc;

int get_active_pbs_server(char **active_server, int *port);
int build_request_svr(int auth_type, const char *user, int sock, std::string &message);
int build_active_server_response(std::string &message);
int set_active_pbs_server(const char *server_name, const int);

extern time_t pbs_tcp_timeout;
extern char   *my_active_server;





START_TEST(get_active_pbs_server_test)
  {
  char *server_name;
  int  rc;
  int  port;
  
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
  trqauthd_terminate_success = false;

  setenv("PBSAPITIMEOUT", "3", 1);
  get_active_pbs_server(&server_name, &port);
  fail_unless(pbs_tcp_timeout == 3);

  socket_success = false;
  rc = get_active_pbs_server(&server_name, &port);
  fail_unless(rc != PBSE_NONE, "get_active_pbs_server bad socket", rc);

  socket_success = true;
  connect_success = false;
  rc = get_active_pbs_server(&server_name, &port);
  fail_unless(rc != PBSE_NONE, "get_active_pbs_server bad connect", rc);

  write_success = false;
  connect_success = true;
  rc = get_active_pbs_server(&server_name, &port);
  fail_unless(rc != PBSE_NONE, "get_active_pbs_server bad connect", rc);

  write_success = true;
  socket_read_success = false;
  rc = get_active_pbs_server(&server_name, &port);
  fail_unless(rc != PBSE_NONE, "get_active_pbs_server bad connect", rc);

  }
END_TEST

START_TEST(test_trq_simple_connect)
  {
  const char *server_name = "localhost";
  int         handle = -1;
  int         rc;
  int         port = 15001;
  struct sockaddr sa;
  socklen_t   al;

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


  rc = trq_simple_connect(server_name, port, &handle);
  fail_unless(rc == PBSE_NONE, "trq_simple_connect failed success case", rc);
  fail_unless(handle >= 0, "trq_simple_connect returned invalid handle", handle);
  trq_simple_disconnect(handle);

  socket_success = false;
  rc = trq_simple_connect(server_name, port, &handle);
  fail_unless(rc == PBSE_SERVER_NOT_FOUND, "trq_simple_connect failed failed socket call", rc);

  socket_success = true;
  setsockopt_success = false;
  rc = trq_simple_connect(server_name, port, &handle);
  fail_unless(rc == PBSE_SERVER_NOT_FOUND, "trq_simple_connect failed failed setsockopt call", rc);

  setsockopt_success = true;
  connect_success = false;
  rc = trq_simple_connect(server_name, port, &handle);
  fail_unless(rc != PBSE_NONE, "trq_simple_connect failed failed connect call", rc);

  // make sure socket is IPv4
  al = sizeof(sa);
  rc = getsockname(handle, &sa, &al);
  fail_unless(rc != 0);
  fail_unless(sa.sa_family != AF_INET);
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
  char *ssh_key = NULL;
  char *sign_key = NULL;
  int   rc;
  int   port = 16001;

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
  rc = set_active_pbs_server(new_server_name, 15001);
  fail_unless(rc == PBSE_NONE, "set_active_pbs_server failed", rc);

  }
END_TEST

START_TEST(test_validate_active_pbs_server)
  {
  int rc;
  char *active_server;

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

  request_type = 1;
  rc = validate_active_pbs_server(&active_server);
  fail_unless(rc == PBSE_NONE, "validate_active_pbs_server failed", rc);

  socket_success = false;
  rc = validate_active_pbs_server(&active_server);
  fail_unless(rc != PBSE_NONE, "validate_active_pbs_server bad socket", rc);

  socket_success = true;;
  connect_success = false;
  rc = validate_active_pbs_server(&active_server);
  fail_unless(rc != PBSE_NONE, "validate_active_pbs_server bad connect", rc);

  write_success = false;
  connect_success = true;
  rc = validate_active_pbs_server(&active_server);
  fail_unless(rc != PBSE_NONE, "validate_active_pbs_server bad write", rc);

  write_success = true;
  socket_read_success = false;
  rc = validate_active_pbs_server(&active_server);
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
  getpwuid_success = true;
  get_hostaddr_success = true;
  gethostname_success = true;

  sock = (int *)calloc(1, sizeof(int));
  *sock = 20;
  request_type = -1;

  // a request_tyupe of -1 will cause a failure on the reading of the incoming request
  (*process_svr_conn)((void *)sock);
  fail_unless(process_svr_conn_rc != PBSE_NONE, "process_svr_conn socket_read_num does not fail as expected");

  // Test with an invalid request_type 
  process_svr_conn_rc = PBSE_NONE;
  sock = (int *)calloc(1, sizeof(int));
  *sock = 20;
  request_type = 10;
  (*process_svr_conn)((void *)sock);
  fail_unless(process_svr_conn_rc == PBSE_IVALREQ, "process_svr_conn did not fail as expected with invalid request type", process_svr_conn_rc); 

  // Test the success case for TRQ_GET_ACTIVE_SERVER
  process_svr_conn_rc = PBSE_NONE;
  sock = (int *)calloc(1, sizeof(int));
  *sock = 20;
  request_type = TRQ_GET_ACTIVE_SERVER;
  (*process_svr_conn)((void *)sock);
  fail_unless(process_svr_conn_rc == PBSE_NONE, "TRQ_GET_ACTIVE_SERVER failed");

  trqauthd_terminate_success = true;
  sock = (int *)calloc(1, sizeof(int));
  *sock = 20;
  request_type = TRQ_DOWN_TRQAUTHD;
  (*process_svr_conn)((void *)sock);
  fail_unless(process_svr_conn_rc == PBSE_NONE, "TRQ_GET_ACTIVE_SERVER failed");

  trqauthd_terminate_success = false;
  sock = (int *)calloc(1, sizeof(int));
  *sock = 20;
  request_type = TRQ_DOWN_TRQAUTHD;
  (*process_svr_conn)((void *)sock);
  fail_unless(process_svr_conn_rc == PBSE_PERM, "TRQ_GET_ACTIVE_SERVER failed");

  // Test the success case for TRQ_VALIDATE_ACTIVE_SERVER
  sock = (int *)calloc(1, sizeof(int));
  *sock = 20;
  socket_success = true;
  request_type = TRQ_VALIDATE_ACTIVE_SERVER;
  (*process_svr_conn)((void *)sock);
  fail_unless(((process_svr_conn_rc == PBSE_NONE) || (process_svr_conn_rc == PBSE_UNKREQ)), "TRQ_VALIDATE_ACTIVE_SERVER failed"); 

  // Test failure case for TRQ_VALIDATE_ACTIVE_SERVER with bad socket
  socket_success = false;
  sock = (int *)calloc(1, sizeof(int));
  *sock = 20;
  request_type = TRQ_VALIDATE_ACTIVE_SERVER;
  (*process_svr_conn)((void *)sock);
  fail_unless(process_svr_conn_rc == PBSE_NONE, "TRQ_VALIDATE_ACTIVE_SERVER failed");

  // Test the success case for TRQ_AUTH_CONNECTION
  sock = (int *)calloc(1, sizeof(int));
  *sock = 20;
  socket_success = true;
  request_type = TRQ_AUTH_CONNECTION;
  (*process_svr_conn)((void *)sock);
  fail_unless(process_svr_conn_rc == PBSE_NONE, "TRQ_AUTH_CONNECTION failed");
  
  // Test when validate_user fails
  socket_read_success = true;
  getsockopt_success = false;
  sock = (int *)calloc(1, sizeof(int));
  *sock = 20;
  request_type = TRQ_AUTH_CONNECTION;
  (*process_svr_conn)((void *)sock);
  fail_unless(process_svr_conn_rc != PBSE_NONE, "TRQ_AUTH_CONNECTION failed");

  // Test when socket_get_tcp_priv fails
  getsockopt_success = true;
  tcp_priv_success = false;
  sock = (int *)calloc(1, sizeof(int));
  *sock = 20;
  request_type = TRQ_AUTH_CONNECTION;
  (*process_svr_conn)((void *)sock);
  fail_unless(process_svr_conn_rc != PBSE_NONE, "TRQ_AUTH_CONNECTION failed");

  // Test when socket_connect fails
  tcp_priv_success = true;
  socket_connect_success = false;
  sock = (int *)calloc(1, sizeof(int));
  *sock = 20;
  request_type = TRQ_AUTH_CONNECTION;
  (*process_svr_conn)((void *)sock);
  fail_unless(process_svr_conn_rc != PBSE_NONE, "TRQ_AUTH_CONNECTION failed");

  // Test when socket_write fails
  socket_connect_success = true;
  write_success = false;
  sock = (int *)calloc(1, sizeof(int));
  *sock = 20;
  request_type = TRQ_AUTH_CONNECTION;
  (*process_svr_conn)((void *)sock);
  fail_unless(process_svr_conn_rc != PBSE_NONE, "TRQ_AUTH_CONNECTION failed");

  // Test when socket_write fails
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

START_TEST(build_request_svr_test)
  {
  std::string message;

  fail_unless(build_request_svr(AUTH_TYPE_IFF, NULL, 5, message) == PBSE_BAD_PARAMETER);
  fail_unless(build_request_svr(AUTH_TYPE_KEY, "dbeer", 5, message) == PBSE_NOT_IMPLEMENTED);
  fail_unless(build_request_svr(-17, "dbeer", 5, message) == PBSE_AUTH_INVALID);
  fail_unless(build_request_svr(AUTH_TYPE_IFF, "dbeer", 6, message) == PBSE_NONE);

  char buf[1024];
  snprintf(buf, sizeof(buf), "+%d+%d2+%d%d+%ddbeer%d+%d+0",
    PBS_BATCH_PROT_TYPE,
    PBS_BATCH_PROT_VER,
    PBS_BATCH_AuthenUser,
    1, // length of "dbeer" is 5, 1 char to represent 5
    (int)strlen("dbeer"),
    1, // number of character to represent 6 is 1
    6);
  fail_unless(!strcmp(message.c_str(), buf));
  
  fail_unless(build_request_svr(AUTH_TYPE_IFF, "dbeer", 7, message) == PBSE_NONE);
  snprintf(buf, sizeof(buf), "+%d+%d2+%d%d+%ddbeer%d+%d+0",
    PBS_BATCH_PROT_TYPE,
    PBS_BATCH_PROT_VER,
    PBS_BATCH_AuthenUser,
    1, // length of "dbeer" is 5, 1 char to represent 5
    (int)strlen("dbeer"),
    1, // number of character to represent 7 is 1
    7);
  fail_unless(!strcmp(message.c_str(), buf));
  }
END_TEST

START_TEST(build_active_server_response_test)
  {
  std::string message;
  set_active_pbs_server("", 15001);
  fail_unless(build_active_server_response(message) == PBSE_NONE);
  set_active_pbs_server("napali", 15001);
  fail_unless(build_active_server_response(message) == PBSE_NONE);
  fail_unless(!strcmp(message.c_str(), "0|6|napali|15001|"));
  }
END_TEST

START_TEST(test_set_trqauthd_addr)
  {
  int rc;

  gethostname_success = true;
  get_hostaddr_success = true;
  rc = set_trqauthd_addr();
  fail_unless(rc == PBSE_NONE, "set_trqauthd_addr failed for success case");

  // this test doesn't work because we're trying to use a built-in function. Some linker
  // magic would have to happen to use our built-in instead of the normal one
  //gethostname_success = false;
  //rc = set_trqauthd_addr();
  //fail_unless(rc != PBSE_NONE, "set_trqauthd_addr failed for success case");
  
  gethostname_success = true;
  get_hostaddr_success = false;
  rc = set_trqauthd_addr();
  fail_unless(rc != PBSE_NONE, "set_trqauthd_addr failed for success case");
  }
END_TEST

START_TEST(test_validate_user)
  {
  int rc;
  char msg [100];

  getsockopt_success = true;
  getpwuid_success = true;
  
  rc = validate_user(10, "eris", 1, msg);
  fail_unless(rc == PBSE_NONE);

  // send in a NULL name 
  rc = validate_user(10, NULL, 1, msg);
  fail_unless(rc != PBSE_NONE);

  // send a null msg pointer 
  rc = validate_user(10, "eris", 1, NULL);
  fail_unless(rc != PBSE_NONE);

  getsockopt_success = false;
  rc = validate_user(10, "eris", 1, msg);
  fail_unless(rc != PBSE_NONE);

  getsockopt_success = true;
  getpwuid_success = false;
  rc = validate_user(10, "eris", 1, msg);
  fail_unless(rc != PBSE_NONE);

  // test the case where user names won't match 
  getpwuid_success = true;
  rc = validate_user(10, "fred", 1, msg);
  fail_unless(rc != PBSE_NONE);

  // test the case where user pids won't match 
  rc = validate_user(10, "eris", 2, msg);
  fail_unless(rc != PBSE_NONE);

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

  tc_core = tcase_create("test_response_building");
  tcase_add_test(tc_core, build_request_svr_test);
  tcase_add_test(tc_core, build_active_server_response_test);
  suite_add_tcase(s, tc_core);

  tc_core = tcase_create("test_set_trqauthd_addr");
  tcase_add_test(tc_core, test_set_trqauthd_addr);
  suite_add_tcase(s, tc_core);

  tc_core = tcase_create("test_validate_user");
  tcase_add_test(tc_core, test_validate_user);
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
