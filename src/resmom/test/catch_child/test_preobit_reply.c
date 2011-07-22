#include "test_catch_child.h"
#include "catch_child.h"

extern int tc;
extern int func_num;
extern LOGLEVEL;
extern socket_ref;

START_TEST(test_preobit_reply_first)
  {
  socket_ref = 1;
  func_num = PREOBIT_REPLY;
  tc = 1;
  LOGLEVEL = 2;
  preobit_reply(socket_ref);
  }
END_TEST

START_TEST(test_preobit_reply_irtn0)
  {
  socket_ref = 1;
  func_num = PREOBIT_REPLY;
  tc = 2;
  LOGLEVEL = 2;
  preobit_reply(socket_ref);
  }
END_TEST

START_TEST(test_preobit_reply_pjobnull)
  {
  socket_ref = 1;
  func_num = PREOBIT_REPLY;
  tc = 3;
  LOGLEVEL = 2;
  preobit_reply(socket_ref);
  }
END_TEST

START_TEST(test_preobit_reply_cleanedout)
  {
  socket_ref = 1;
  func_num = PREOBIT_REPLY;
  tc = 4;
  LOGLEVEL = 2;
  preobit_reply(socket_ref);
  }
END_TEST

START_TEST(test_preobit_reply_pbsenone)
  {
  socket_ref = 1;
  func_num = PREOBIT_REPLY;
  tc = 5;
  LOGLEVEL = 2;
  preobit_reply(socket_ref);
  }
END_TEST

START_TEST(test_preobit_reply_pbsedefault)
  {
  socket_ref = 1;
  func_num = PREOBIT_REPLY;
  tc = 6;
  LOGLEVEL = 2;
  preobit_reply(socket_ref);
  }
END_TEST

START_TEST(test_preobit_reply_brcstatus)
  {
  socket_ref = 1;
  func_num = PREOBIT_REPLY;
  tc = 7;
  LOGLEVEL = 2;
  preobit_reply(socket_ref);
  }
END_TEST

START_TEST(test_preobit_reply_pstatusnull)
  {
  socket_ref = 1;
  func_num = PREOBIT_REPLY;
  tc = 8;
  LOGLEVEL = 2;
  preobit_reply(socket_ref);
  }
END_TEST

START_TEST(test_preobit_reply_objname)
  {
  socket_ref = 1;
  func_num = PREOBIT_REPLY;
  tc = 9;
  LOGLEVEL = 2;
  preobit_reply(socket_ref);
  }
END_TEST

START_TEST(test_preobit_reply_sattrl)
  {
  socket_ref = 1;
  func_num = PREOBIT_REPLY;
  tc = 10;
  LOGLEVEL = 2;
  preobit_reply(socket_ref);
  }
END_TEST

START_TEST(test_preobit_reply_sattrlnohost)
  {
  socket_ref = 1;
  func_num = PREOBIT_REPLY;
  tc = 11;
  LOGLEVEL = 2;
  preobit_reply(socket_ref);
  }
END_TEST

START_TEST(test_preobit_reply_negcpid)
  {
  socket_ref = 1;
  func_num = PREOBIT_REPLY;
  tc = 12;
  LOGLEVEL = 2;
  preobit_reply(socket_ref);
  }
END_TEST

START_TEST(test_preobit_reply_atflags)
  {
  socket_ref = 1;
  func_num = PREOBIT_REPLY;
  tc = 13;
  LOGLEVEL = 2;
  preobit_reply(socket_ref);
  }
END_TEST

START_TEST(test_preobit_reply_atflagsneg)
  {
  socket_ref = 1;
  func_num = PREOBIT_REPLY;
  tc = 14;
  LOGLEVEL = 2;
  preobit_reply(socket_ref);
  }
END_TEST

Suite *preobit_reply_suite(void)
  {
  Suite *s = suite_create("preobit_reply methods");
  TCase *tc_core = tcase_create("Core");
  tcase_add_test(tc_core, test_preobit_reply_first);
  tcase_add_test(tc_core, test_preobit_reply_irtn0);
  tcase_add_test(tc_core, test_preobit_reply_pjobnull);
  tcase_add_test(tc_core, test_preobit_reply_cleanedout);
  tcase_add_test(tc_core, test_preobit_reply_pbsenone);
  tcase_add_test(tc_core, test_preobit_reply_pbsedefault);
  tcase_add_test(tc_core, test_preobit_reply_brcstatus);
  tcase_add_test(tc_core, test_preobit_reply_pstatusnull);
  tcase_add_test(tc_core, test_preobit_reply_objname);
  tcase_add_test(tc_core, test_preobit_reply_sattrl);
  tcase_add_test(tc_core, test_preobit_reply_sattrlnohost);
  tcase_add_test(tc_core, test_preobit_reply_negcpid);
  tcase_add_test(tc_core, test_preobit_reply_atflags);
  tcase_add_test(tc_core, test_preobit_reply_atflagsneg);
  suite_add_tcase(s, tc_core);
  return s;
  }

void rundebug()
  {
  socket_ref = 1;
  func_num = PREOBIT_REPLY;
  tc = 13;
  LOGLEVEL = 2;
  preobit_reply(socket_ref);
  }

int main(void)
  {
  int number_failed = 0;
  rundebug();
  SRunner *sr = srunner_create(preobit_reply_suite());
  srunner_set_log(sr, "preobit_reply_suite.log");
  srunner_run_all(sr, CK_NORMAL);
  number_failed = srunner_ntests_failed(sr);
  srunner_free(sr);
  return number_failed;
  }
