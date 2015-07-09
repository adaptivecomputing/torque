#include "license_pbs.h" /* See here for the software license */
#include "test_disrl_.h"
#include <stdlib.h>
#include <stdio.h>
#include <stddef.h>
#include <pbs_config.h>

#include "dis.h"
#include "dis_internal.h"
#include "tcp.h"
#include "pbs_error.h"


START_TEST(test_one)
  {
  struct tcp_chan   chan;
  long double       ldval;
  dis_long_double_t *ptr = &ldval;
  unsigned          ndigs;
  unsigned          nskips;

  memset(&chan, 0, sizeof(chan));
  fail_unless(disrl_(&chan, ptr, &ndigs, &nskips, 0, dis_umaxd + 1) != PBSE_NONE);
/*int disrl_(

  struct tcp_chan   *chan,
  dis_long_double_t *ldval,
  unsigned          *ndigs,
  unsigned          *nskips,
  unsigned           sigd,
  unsigned           count) */
  }
END_TEST

START_TEST(test_two)
  {


  }
END_TEST

Suite *disrl__suite(void)
  {
  Suite *s = suite_create("disrl__suite methods");
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
  sr = srunner_create(disrl__suite());
  srunner_set_log(sr, "disrl__suite.log");
  srunner_run_all(sr, CK_NORMAL);
  number_failed = srunner_ntests_failed(sr);
  srunner_free(sr);
  return number_failed;
  }
