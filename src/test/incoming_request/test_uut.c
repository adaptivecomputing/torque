#include <stdio.h>
#include <stdlib.h>
#include <check.h>

#include "tcp.h"

int get_protocol_type(struct tcp_chan *chan, int &rc);

extern int    peek_count;
extern time_t pbs_incoming_tcp_timeout;
extern bool   busy_pool;

START_TEST(test_get_protocol_type)
  {
  struct tcp_chan chan;
  int             rc;

  memset(&chan, 0, sizeof(chan));

  fail_unless(get_protocol_type(&chan, rc) == 2);
  fail_unless(peek_count == 1);

  // make sure we're retrying if we have time
  pbs_incoming_tcp_timeout = 300;
  fail_unless(get_protocol_type(&chan, rc) == 2);
  fail_unless(peek_count == 3);

  // make sure we don't retry if our timeout is below 5
  pbs_incoming_tcp_timeout = 3;
  fail_unless(get_protocol_type(&chan, rc) == 0);
  fail_unless(peek_count == 4);

  // make sure we don't retry if our threadpool is busy
  peek_count = 5; // will fail with odd numbers
  pbs_incoming_tcp_timeout = 300;
  busy_pool = true;
  fail_unless(get_protocol_type(&chan, rc) == 0);
  fail_unless(peek_count == 6);
  }
END_TEST




START_TEST(test_two)
  {
  }
END_TEST




Suite *incoming_request_suite(void)
  {
  Suite *s = suite_create("incoming_request test suite methods");
  TCase *tc_core = tcase_create("test_get_protocol_type");
  tcase_add_test(tc_core, test_get_protocol_type);
  suite_add_tcase(s, tc_core);
  
  tc_core = tcase_create("test_two");
  tcase_add_test(tc_core, test_two);
  suite_add_tcase(s, tc_core);
  
  return(s);
  }

void rundebug()
  {
  }

int main(void)
  {
  int number_failed = 0;
  SRunner *sr = NULL;
  rundebug();
  sr = srunner_create(incoming_request_suite());
  srunner_set_log(sr, "incoming_request_suite.log");
  srunner_run_all(sr, CK_NORMAL);
  number_failed = srunner_ntests_failed(sr);
  srunner_free(sr);
  return(number_failed);
  }
