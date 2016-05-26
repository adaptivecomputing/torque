#include "license_pbs.h" /* See here for the software license */
#include "svr_func.h"
#include "test_svr_func.h"
#include <stdlib.h>
#include <stdio.h>
#include "pbs_error.h"

int keep_completed_val_check(pbs_attribute *pattr,void *pobj,int actmode);

START_TEST(test_keep_comleted_val_check)
  {
	 pbs_attribute keep;
	 keep.at_val.at_long = -1;
	 fail_unless(keep_completed_val_check(&keep, NULL, 0) == 1, "keep->at_val.at_long is < 0");
	 keep.at_val.at_long = 1;
	 fail_unless(keep_completed_val_check(&keep, NULL, 0) == 0, "keep->at_val.at_long is > -1");
  }
END_TEST

START_TEST(test_two)
  {


  }
END_TEST

Suite *svr_func_suite(void)
  {
  Suite *s = suite_create("svr_func_suite methods");
  TCase *tc_core = tcase_create("test_keep_comleted_val_check");
  tcase_add_test(tc_core, test_keep_comleted_val_check);
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
  sr = srunner_create(svr_func_suite());
  srunner_set_log(sr, "svr_func_suite.log");
  srunner_run_all(sr, CK_NORMAL);
  number_failed = srunner_ntests_failed(sr);
  srunner_free(sr);
  return number_failed;
  }
