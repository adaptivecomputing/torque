#include "license_pbs.h" /* See here for the software license */
#include "req_manager.h"
#include "test_req_manager.h"
#include <stdlib.h>
#include <stdio.h>
#include "pbs_error.h"
#include "pbs_nodes.h"
#include "attribute.h"

int check_default_gpu_mode_str(pbs_attribute *pattr, void *pobj, int actmode);
extern int default_gpu_mode;

START_TEST(test_check_default_gpu_mode_str)
  {
  pbs_attribute pattr;
  pattr.at_flags |= ATR_VFLAG_SET;

  // Valid
  pattr.at_val.at_str = strdup("exclusive_process");
  fail_unless(check_default_gpu_mode_str(&pattr, NULL, ATR_ACTION_ALTER) == PBSE_NONE);
  fail_unless(default_gpu_mode == gpu_exclusive_process);

  pattr.at_val.at_str = strdup("exclusive_thread");
  fail_unless(check_default_gpu_mode_str(&pattr, NULL, ATR_ACTION_ALTER) == PBSE_NONE);
  fail_unless(default_gpu_mode == gpu_exclusive_thread);

  pattr.at_val.at_str = strdup("default");
  fail_unless(check_default_gpu_mode_str(&pattr, NULL, ATR_ACTION_ALTER) == PBSE_NONE);
  fail_unless(default_gpu_mode == gpu_normal);

  pattr.at_val.at_str = strdup("shared");
  fail_unless(check_default_gpu_mode_str(&pattr, NULL, ATR_ACTION_ALTER) == PBSE_NONE);
  fail_unless(default_gpu_mode == gpu_normal);

  // Spelling error
  pattr.at_val.at_str = strdup("sharedf");
  fail_unless(check_default_gpu_mode_str(&pattr, NULL, ATR_ACTION_ALTER) == PBSE_ATTRTYPE);

  // Other operations shouldn't cause an error
  fail_unless(check_default_gpu_mode_str(&pattr, NULL, ATR_ACTION_NOOP) == PBSE_NONE);
  fail_unless(check_default_gpu_mode_str(&pattr, NULL, ATR_ACTION_NEW) == PBSE_NONE);
  fail_unless(check_default_gpu_mode_str(&pattr, NULL, ATR_ACTION_RECOV) == PBSE_NONE);
  fail_unless(check_default_gpu_mode_str(&pattr, NULL, ATR_ACTION_FREE) == PBSE_NONE);
  }
END_TEST

START_TEST(test_two)
  {


  }
END_TEST

Suite *req_manager_suite(void)
  {
  Suite *s = suite_create("req_manager_suite methods");
  TCase *tc_core = tcase_create("test_check_default_gpu_mode_str");
  tcase_add_test(tc_core, test_check_default_gpu_mode_str);
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
  sr = srunner_create(req_manager_suite());
  srunner_set_log(sr, "req_manager_suite.log");
  srunner_run_all(sr, CK_NORMAL);
  number_failed = srunner_ntests_failed(sr);
  srunner_free(sr);
  return number_failed;
  }
