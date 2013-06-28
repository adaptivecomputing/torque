#include "license_pbs.h" /* See here for the software license */
#include "lib_utils.h"
#include "net_cache.h"
#include "test_u_mom_hierarchy.h"
#include <stdlib.h>
#include <stdio.h>

#include <errno.h>



#include "pbs_error.h"

int dummySocket = 43;
int dummySocketAfterRetries = 0;
int connectAddrRetries = 10;
int tcpSocket = 42;


START_TEST(test_one)
  {
  struct sockaddr addr;

  memset(&addr,0,sizeof(struct sockaddr));
  fail_unless(tcp_connect_sockaddr(&addr,sizeof(struct sockaddr)) == TRANSIENT_SOCKET_FAIL);

  dummySocket = EINTR;
  connectAddrRetries = 3;

  fail_unless(tcp_connect_sockaddr(&addr,sizeof(struct sockaddr)) == 42);
  fail_unless(connectAddrRetries == -1);

  }
END_TEST

START_TEST(test_two)
  {


  }
END_TEST

Suite *u_mom_hierarchy_suite(void)
  {
  Suite *s = suite_create("u_mom_hierarchy_suite methods");
  TCase *tc_core = tcase_create("test_one");
  tcase_add_test(tc_core, test_one);
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
  sr = srunner_create(u_mom_hierarchy_suite());
  srunner_set_log(sr, "u_mom_hierarchy_suite.log");
  srunner_run_all(sr, CK_NORMAL);
  number_failed = srunner_ntests_failed(sr);
  srunner_free(sr);
  return number_failed;
  }
