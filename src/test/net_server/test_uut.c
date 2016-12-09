#include "license_pbs.h" /* See here for the software license */
#include "lib_net.h"
#include "test_net_server.h"
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string>
#include <errno.h>
#include <string.h>
#include <poll.h>


#include "server_limits.h"
#include "pbs_error.h"
#include "lib_net.h"

extern bool  unix_socket_success;
extern bool  socket_connect_success;
extern bool  socket_write_success;
extern bool  socket_read_success;
extern bool  socket_read_code;

extern nfds_t global_poll_nfds;
extern int global_poll_timeout_sec;

int add_connection(int sock, enum conn_type type, pbs_net_t addr, unsigned int port, unsigned int socktype, void *(*func)(void *), int add_wait_request);
void *accept_conn(void *new_conn);

int init_network(unsigned int port, void *(*readfunc)(void *));
void globalset_add_sock(int, u_long, u_long);
void globalset_del_sock(int);


START_TEST(netaddr_pbs_net_t_test_one)
  {
  pbs_net_t ipaddr;
  char      *ipaddr_str;

  ipaddr = 167773189;

  ipaddr_str = netaddr_pbs_net_t(ipaddr);
  fail_unless(ipaddr_str != NULL);
  fprintf(stdout, "\n%s\n", ipaddr_str);
  free(ipaddr_str);


  }
END_TEST

START_TEST(netaddr_pbs_net_t_test_two)
  {
  pbs_net_t ipaddr;
  char      *ipaddr_str;

  ipaddr = 0;

  ipaddr_str = netaddr_pbs_net_t(ipaddr);
  fail_unless(!strcmp(ipaddr_str, "unknown"));
  fprintf(stdout, "%s\n", ipaddr_str);
  free(ipaddr_str);


  }
END_TEST


START_TEST(test_add_connection)
  {
  fail_unless(add_connection(-1, ToServerDIS, 0, 0, PBS_SOCK_INET, accept_conn, 0) == PBSE_BAD_PARAMETER);
  fail_unless(add_connection(PBS_NET_MAX_CONNECTIONS, ToServerDIS, 0, 0, PBS_SOCK_INET, accept_conn, 0) == PBSE_BAD_PARAMETER);

  }
END_TEST

START_TEST(test_ping_trqauthd)
  {
  int rc;
  std::string unix_domain_file_name;

  unix_socket_success = true;
  socket_connect_success = true;
  socket_write_success = true;
  socket_read_success = true;
  socket_read_code = true;

  unix_domain_file_name = "./unit_test";
  unlink(unix_domain_file_name.c_str());
  rc = ping_trqauthd(unix_domain_file_name.c_str());
  fail_unless(rc == PBSE_NONE);

  unix_socket_success = false;
  rc = ping_trqauthd(unix_domain_file_name.c_str());
  fail_unless(rc == PBSE_SOCKET_FAULT);

  unix_socket_success = true;
  socket_connect_success = false;
  rc = ping_trqauthd(unix_domain_file_name.c_str());
  fail_unless(rc == PBSE_DOMAIN_SOCKET_FAULT, "ping_trqauthd failed for socket_connect_success = false", rc);

  socket_connect_success = true;
  socket_write_success = false;
  rc = ping_trqauthd(unix_domain_file_name.c_str());
  fail_unless(rc == PBSE_SOCKET_WRITE, "ping_trqauthd failed for socket_connect_success = false", rc);

  socket_write_success = true;
  socket_read_success = false;
  rc = ping_trqauthd(unix_domain_file_name.c_str());
  fail_unless(rc == PBSE_SOCKET_READ, "ping_trqauthd failed for socket_connect_success = false", rc);

  socket_read_success = true;
  socket_read_code = false;
  rc = ping_trqauthd(unix_domain_file_name.c_str());
  fail_unless(rc == PBSE_BADHOST, "ping_trqauthd failed for socket_connect_success = false", rc);

  unlink(unix_domain_file_name.c_str());
  }
END_TEST

START_TEST(test_check_trqauthd_unix_domain_port)
  {
  int fd;
  int rc;
  std::string unix_domain_file_name;

  unix_socket_success = true;
  socket_connect_success = true;
  socket_write_success = true;
  socket_read_success = true;
  socket_read_code = true;

  unix_domain_file_name = "./unit_test";
  fd = open(unix_domain_file_name.c_str(), O_RDWR | O_TRUNC | O_CREAT);
  fail_unless((fd >= 0), "Could not open unix domain file. %s", strerror(errno));
  
  rc = check_trqauthd_unix_domain_port(unix_domain_file_name.c_str());
  fail_unless((rc == PBSE_SOCKET_LISTEN), "check_trqauthd_unix_domain_port failed for existing unix domain file %s", unix_domain_file_name.c_str());
  rc = unlink(unix_domain_file_name.c_str());
  fail_unless((rc == 0), "could not unlink unix domain file", strerror(errno)); 

  fd = open(unix_domain_file_name.c_str(), O_RDWR | O_TRUNC | O_CREAT);
  fail_unless((fd >= 0), "Could not open unix domain file. %s", strerror(errno));

  socket_connect_success = false;
  rc = check_trqauthd_unix_domain_port(unix_domain_file_name.c_str());
  fail_unless((rc == PBSE_NONE), "check_trqauthd_unix_domain_port failed for existing unix domain file %s", unix_domain_file_name.c_str());

  socket_connect_success = true;
  rc = check_trqauthd_unix_domain_port(unix_domain_file_name.c_str());
  fail_unless((rc == PBSE_NONE), "check_trqauthd_unix_domain_port failed for success case", rc);

  }
END_TEST

START_TEST(test_init_network)
  {
  int rc;
  int timeout_sec = 10;
  nfds_t MaxNumDescriptors = get_max_num_descriptors();

  rc = init_network(0, NULL);
  fail_unless(rc == PBSE_NONE);

  rc = wait_request(timeout_sec, NULL);

  // examine the values passed to poll() to see if they were expected
  fail_unless(global_poll_nfds == MaxNumDescriptors);
  fail_unless(global_poll_timeout_sec == timeout_sec);
  }
END_TEST

Suite *net_server_suite(void)
  {
  Suite *s = suite_create("net_server_suite methods");
  TCase *tc_core = tcase_create("netaddr_pbs_net_t_test_one");
  tcase_add_test(tc_core, netaddr_pbs_net_t_test_one);
  suite_add_tcase(s, tc_core);

  tc_core = tcase_create("netaddr_pbs_net_t_test_two");
  tcase_add_test(tc_core, netaddr_pbs_net_t_test_two);
  suite_add_tcase(s, tc_core);

  tc_core = tcase_create("test_add_connection");
  tcase_add_test(tc_core, test_add_connection);
  suite_add_tcase(s, tc_core);

  tc_core = tcase_create("test_ping_trqauthd");
  tcase_add_test(tc_core, test_ping_trqauthd);
  suite_add_tcase(s, tc_core);

  tc_core = tcase_create("test_check_trqauthd_unix_domain_port");
  tcase_add_test(tc_core, test_check_trqauthd_unix_domain_port);
  suite_add_tcase(s, tc_core);

  tc_core = tcase_create("test_init_network");
  tcase_add_test(tc_core, test_init_network);
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
  sr = srunner_create(net_server_suite());
  srunner_set_log(sr, "net_server_suite.log");
  srunner_run_all(sr, CK_NORMAL);
  number_failed = srunner_ntests_failed(sr);
  srunner_free(sr);
  return number_failed;
  }
