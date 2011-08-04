#include "test_catch_child.h"
#include "catch_child.h"
extern int tc;
extern int func_num;
extern int socket_ref;

extern int exiting_tasks;
extern job *obitReplyJob;

START_TEST(test_obit_reply_pjobnull)
  {
  socket_ref = 1;
  func_num = OBIT_REPLY;
  tc = 1;
  obit_reply(socket_ref);
  }
END_TEST

START_TEST(test_obit_reply_pbsenone)
 {
 socket_ref = 1;
 func_num = OBIT_REPLY;
 tc = 2;
 obit_reply(socket_ref);
 }
END_TEST

START_TEST(test_obit_reply_pbsealrdy)
 {
 socket_ref = 1;
 func_num = OBIT_REPLY;
 tc = 3;
 obit_reply(socket_ref);
 }
END_TEST

START_TEST(test_obit_reply_pbsecleaned)
 {
 socket_ref = 1;
 func_num = OBIT_REPLY;
 tc = 4;
 obit_reply(socket_ref);
 }
END_TEST

START_TEST(test_obit_reply_pbsecleanedvflag)
 {
 socket_ref = 1;
 func_num = OBIT_REPLY;
 tc = 9;
 obit_reply(socket_ref);
 }
END_TEST

START_TEST(test_obit_reply_pbseneg)
 {
 socket_ref = 1;
 func_num = OBIT_REPLY;
 tc = 5;
 obit_reply(socket_ref);
 fail_unless(exiting_tasks == 1, "exiting_tasks should be set");
 }
END_TEST

START_TEST(test_obit_reply_pbsebad)
 {
 socket_ref = 1;
 func_num = OBIT_REPLY;
 tc = 6;
 obit_reply(socket_ref);
 }
END_TEST

START_TEST(test_obit_reply_pbsesystem)
 {
 socket_ref = 1;
 func_num = OBIT_REPLY;
 tc = 7;
 obit_reply(socket_ref);
 }
END_TEST

START_TEST(test_obit_reply_pbsereject)
 {
 socket_ref = 1;
 func_num = OBIT_REPLY;
 tc = 8;
 obit_reply(socket_ref);
 }
END_TEST

START_TEST(test_obit_reply_failif)
 {
 socket_ref = 1;
 func_num = OBIT_REPLY;
 tc = 10;
 obit_reply(socket_ref);
 }
END_TEST

Suite *obit_reply_suite(void)
  {
  Suite *s = suite_create("obit_reply methods");
  TCase *tc_core = tcase_create("Core");
  tcase_add_test(tc_core, test_obit_reply_pjobnull);
  tcase_add_test(tc_core, test_obit_reply_pbsenone);
  tcase_add_test(tc_core, test_obit_reply_pbsealrdy);
  tcase_add_test(tc_core, test_obit_reply_pbsecleaned);
  tcase_add_test(tc_core, test_obit_reply_pbsecleanedvflag);
  tcase_add_test(tc_core, test_obit_reply_pbseneg);
  tcase_add_test(tc_core, test_obit_reply_pbsebad);
  tcase_add_test(tc_core, test_obit_reply_pbsesystem);
  tcase_add_test(tc_core, test_obit_reply_pbsereject);
  tcase_add_test(tc_core, test_obit_reply_failif);
  suite_add_tcase(s, tc_core);
  return s;
  }

void rundebug()
  {
  socket_ref = 1;
  func_num = OBIT_REPLY;
  tc = 5;
  obit_reply(socket_ref);
  }

void mom_deljob(job *pjob)
  {
  }

int main(void)
  {
  int number_failed = 0;
  SRunner *sr = NULL;
  SRunner *sr = NULL;
  rundebug();
  sr = srunner_create(obit_reply_suite());
  srunner_set_log(sr, "obit_reply_suite.log");
  srunner_run_all(sr, CK_NORMAL);
  number_failed = srunner_ntests_failed(sr);
  srunner_free(sr);
  return number_failed;
  }
