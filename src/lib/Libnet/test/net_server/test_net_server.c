#include "license_pbs.h" /* See here for the software license */
#include "lib_net.h"
#include "test_net_server.h"
#include <stdlib.h>
#include <stdio.h>


#include "server_limits.h"
#include "pbs_error.h"

int add_connection(int sock, enum conn_type type, pbs_net_t addr, unsigned int port, unsigned int socktype, void *(*func)(void *), int add_wait_request);
void *accept_conn(void *new_conn);


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
