#include "test_catch_child.h"
#include "catch_child.h"
#include <stdlib.h>

extern int tc;
extern int func_num;
extern int ran_one;
/*extern char *getenv(char *); */
extern int multi_mom;
extern int LOGLEVEL;
extern void *calloc(size_t __nmemb, size_t __size);

START_TEST(test_scan_for_exiting_momdown)
  {
  func_num = SCAN_FOR_EXITING;
  tc = 1;
  LOGLEVEL = 6;
  multi_mom = 1;
  scan_for_exiting();
  }
END_TEST

START_TEST(test_scan_for_exiting_setenv)
  {
  func_num = SCAN_FOR_EXITING;
  tc = 2;
  LOGLEVEL = 6;
  multi_mom = 1;
  setenv("TORQUEFORCESEND", "2", 1);
  scan_for_exiting();
  }
END_TEST

START_TEST(test_scan_for_exiting_pjobnull)
  {
  func_num = SCAN_FOR_EXITING;
  tc = 4;
  LOGLEVEL = 6;
  scan_for_exiting();
  }
END_TEST

Suite *scan_for_exiting_suite(void)
  {
  Suite *s = suite_create("scan_for_exiting methods");
  TCase *tc_core = tcase_create("Core");
  tcase_add_test(tc_core, test_scan_for_exiting_momdown);
  tcase_add_test(tc_core, test_scan_for_exiting_setenv);
  tcase_add_test(tc_core, test_scan_for_exiting_pjobnull);
  suite_add_tcase(s, tc_core);
  return s;
  }

void rundebug()
  {
  func_num = SCAN_FOR_EXITING;
  tc = 1;
  LOGLEVEL = 6;
  multi_mom = 1;
  scan_for_exiting();
  }

int main(void)
  {
  int number_failed = 0;
  SRunner *sr = NULL;
  SRunner *sr = NULL;
  rundebug();
  sr = srunner_create(scan_for_exiting_suite());
  srunner_set_log(sr, "scan_for_exiting_suite.log");
  srunner_run_all(sr, CK_NORMAL);
  number_failed = srunner_ntests_failed(sr);
  srunner_free(sr);
  return number_failed;
  }
