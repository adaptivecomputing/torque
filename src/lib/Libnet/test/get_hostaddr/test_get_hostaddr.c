#include "license_pbs.h" /* See here for the software license */
#include "lib_net.h"
#include "test_get_hostaddr.h"
#include <stdlib.h>
#include <stdio.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netdb.h>


#include "pbs_error.h"


START_TEST(test_netaddr_long)
  {
  char buf[80];
  struct in_addr a_nbo;
  struct in_addr a_hbo;
  const char *ip_addr_str = "192.0.2.33";
  const char *ip_addr_str_rev = "33.2.0.192";

  // get network address in network byte order
  inet_aton(ip_addr_str, &a_nbo);

  // convert to host byte order
  a_hbo.s_addr = ntohl((uint32_t) a_nbo.s_addr);

  // get string from host byte order adddress
  netaddr_long(a_hbo.s_addr, buf);
  fail_unless(strcmp(ip_addr_str, buf) == 0);

  // get string from network byte order adddress
  netaddr_long(a_nbo.s_addr, buf);
  fail_unless(strcmp(ip_addr_str_rev, buf) == 0);

  }
END_TEST

Suite *get_hostaddr_suite(void)
  {
  Suite *s = suite_create("get_hostaddr_suite methods");

  TCase *tc_core = tcase_create("test_netaddr_long");
  tcase_add_test(tc_core, test_netaddr_long);
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
  sr = srunner_create(get_hostaddr_suite());
  srunner_set_log(sr, "get_hostaddr_suite.log");
  srunner_run_all(sr, CK_NORMAL);
  number_failed = srunner_ntests_failed(sr);
  srunner_free(sr);
  return number_failed;
  }
