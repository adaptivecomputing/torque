#include "license_pbs.h" /* See here for the software license */
#include "lib_net.h"
#include "test_net_client.h"
#include <stdlib.h>
#include <stdio.h>
#include <poll.h>


#include "pbs_error.h"

bool wait_for_write_ready(int, int);

extern int   global_poll_rc;
extern short global_poll_revents;

START_TEST(test_wait_for_write_ready)
  {

  global_poll_rc = 0;
  fail_unless(wait_for_write_ready(0, 0) == false);

  global_poll_rc = -1;
  fail_unless(wait_for_write_ready(0, 0) == false);

  global_poll_rc = 1;
  global_poll_revents = 0;
  fail_unless(wait_for_write_ready(0, 0) == false);

  global_poll_rc = 1;
  global_poll_revents = POLLOUT;
  fail_unless(wait_for_write_ready(0, 0) == true);
  }
END_TEST

START_TEST(test_two)
  {


  }
END_TEST

Suite *net_client_suite(void)
  {
  Suite *s = suite_create("net_client_suite methods");
  TCase *tc_core = tcase_create("test_wait_for_write_ready");
  tcase_add_test(tc_core, test_wait_for_write_ready);
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
  sr = srunner_create(net_client_suite());
  srunner_set_log(sr, "net_client_suite.log");
  srunner_run_all(sr, CK_NORMAL);
  number_failed = srunner_ntests_failed(sr);
  srunner_free(sr);
  return number_failed;
  }
