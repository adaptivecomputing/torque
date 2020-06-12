#include "license_pbs.h" /* See here for the software license */
#include "test_pbs_demux.h"
#include <stdlib.h>
#include <stdio.h>


#include "pbs_error.h"

void readit(int, struct routem*);

START_TEST(test_readit)
  {
  // this is a void function but expect
  //  immediate return with no segfault
  //  since readset not initialized
  readit(0, NULL);
  }
END_TEST

START_TEST(test_two)
  {


  }
END_TEST

Suite *pbs_demux_suite(void)
  {
  Suite *s = suite_create("pbs_demux_suite methods");
  TCase *tc_core = tcase_create("test_readit");
  tcase_add_test(tc_core, test_readit);
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
  sr = srunner_create(pbs_demux_suite());
  srunner_set_log(sr, "pbs_demux_suite.log");
  srunner_run_all(sr, CK_NORMAL);
  number_failed = srunner_ntests_failed(sr);
  srunner_free(sr);
  return number_failed;
  }
