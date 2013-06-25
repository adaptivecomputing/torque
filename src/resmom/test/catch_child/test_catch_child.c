
#include <errno.h>

#include "test_catch_child.h"

void catch_child(int sig);
void *obit_reply(void *new_sock);

extern int  termin_child;
extern int  DIS_reply_read_count;
extern int  tc;
extern bool eintr_test;

START_TEST(obit_reply_test)
  {
  int sock = 1;
  DIS_reply_read_count = 0;
  
  eintr_test = true;
  obit_reply(&sock);
  fail_unless(DIS_reply_read_count == 11);
  eintr_test = false;
  }
END_TEST

START_TEST(test_catch_child_1)
  {
  termin_child = 0;
  catch_child(2);
  fail_unless(termin_child == 1, "termin_child is set to 1 by this method");
  }
END_TEST

Suite *catch_child_suite(void)
  {
  Suite *s = suite_create("catch_child methods");
  TCase *tc_core = tcase_create("Core");
  tcase_add_test(tc_core, test_catch_child_1);
  tcase_add_test(tc_core, obit_reply_test);
  suite_add_tcase(s, tc_core);
  return s;
  }

int main(void)
  {
  int number_failed = 0;
  SRunner *sr = NULL;
  sr = srunner_create(catch_child_suite());
  srunner_set_log(sr, "catch_child_suite.log");
  srunner_run_all(sr, CK_NORMAL);
  number_failed = srunner_ntests_failed(sr);
  srunner_free(sr);
  return number_failed;
  }
