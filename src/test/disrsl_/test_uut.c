#include "license_pbs.h" /* See here for the software license */
#include "test_disrsl_.h"
#include <stdlib.h>
#include <stdio.h>

#include "dis_internal.h"
#include "dis.h"
#include "pbs_error.h"
#include "tcp.h"

extern unsigned ulmaxdigs;

START_TEST(test_one)
  {
  struct tcp_chan chan;
  int             negate = 0;
  unsigned long   val;

  memset(&chan, 0, sizeof(chan));
  fail_unless(disrsl_(&chan, &negate, &val, ulmaxdigs + 1) == DIS_OVERFLOW);
  }
END_TEST

START_TEST(test_two)
  {


  }
END_TEST

Suite *disrsl__suite(void)
  {
  Suite *s = suite_create("disrsl__suite methods");
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
  sr = srunner_create(disrsl__suite());
  srunner_set_log(sr, "disrsl__suite.log");
  srunner_run_all(sr, CK_NORMAL);
  number_failed = srunner_ntests_failed(sr);
  srunner_free(sr);
  return number_failed;
  }
