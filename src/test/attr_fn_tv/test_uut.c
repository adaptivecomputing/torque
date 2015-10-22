#include "license_pbs.h" /* See here for the software license */
#include "test_attr_fn_tv.h"
#include <stdlib.h>
#include <stdio.h>

#include "attribute.h"
#include "pbs_error.h"

START_TEST(decode_tv)
  {
  pbs_attribute pattr;
  char    name[32];
  char    rescn[32];
  char    val[32];
  char    EMsg[48];
  int     perm = 0;
  int     rc;
  
  memset(&pattr, 0, sizeof(pbs_attribute));
  strcpy(name, "freddy");
  strcpy(rescn, "someresc");
  strcpy(val, "33.3");

  rc = decode_tv(&pattr, name, rescn, val, perm);
  sprintf(EMsg, "decode_tv unexpectedly failed: %d", rc);
  fail_unless(rc == PBSE_NONE, EMsg);

  }
END_TEST

START_TEST(decode_tv_no_dot)
  {
  pbs_attribute pattr;
  char    name[32];
  char    rescn[32];
  char    val[32];
  char    EMsg[48];
  int     perm = 0;
  int     rc;
  
  memset(&pattr, 0, sizeof(pbs_attribute));
  strcpy(name, "freddy");
  strcpy(rescn, "someresc");
  strcpy(val, "33");

  rc = decode_tv(&pattr, name, rescn, val, perm);
  sprintf(EMsg, "decode_tv unexpectedly failed: %d", rc);
  fail_unless(rc == PBSE_NONE, EMsg);

  }
END_TEST

START_TEST(decode_tv_two_dots)
  {
  pbs_attribute pattr;
  char    name[32];
  char    rescn[32];
  char    val[32];
  char    EMsg[48];
  int     perm = 0;
  int     rc;
  
  memset(&pattr, 0, sizeof(pbs_attribute));
  strcpy(name, "freddy");
  strcpy(rescn, "someresc");
  strcpy(val, "33.44.55");

  rc = decode_tv(&pattr, name, rescn, val, perm);
  sprintf(EMsg, "decode_tv unexpectedly failed: %d", rc);
  fail_unless(rc == PBSE_BADATVAL, EMsg);

  }
END_TEST

START_TEST(decode_tv_not_digit)
  {
  pbs_attribute pattr;
  char    name[32];
  char    rescn[32];
  char    val[32];
  char    EMsg[48];
  int     perm = 0;
  int     rc;
  
  memset(&pattr, 0, sizeof(pbs_attribute));
  strcpy(name, "freddy");
  strcpy(rescn, "someresc");
  strcpy(val, "XX.44");

  rc = decode_tv(&pattr, name, rescn, val, perm);
  sprintf(EMsg, "decode_tv unexpectedly failed: %d", rc);
  fail_unless(rc == PBSE_BADATVAL, EMsg);

  }
END_TEST

START_TEST(decode_tv_null_val)
  {
  pbs_attribute pattr;
  char    name[32];
  char    rescn[32];
  char    *val = NULL;
  char    EMsg[48];
  int     perm = 0;
  int     rc;
  
  memset(&pattr, 0, sizeof(pbs_attribute));
  strcpy(name, "freddy");
  strcpy(rescn, "someresc");

  rc = decode_tv(&pattr, name, rescn, val, perm);
  sprintf(EMsg, "decode_tv unexpectedly failed: %d", rc);
  fail_unless(rc == PBSE_NONE, EMsg);

  }
END_TEST

START_TEST(decode_tv_val_empty)
  {
  pbs_attribute pattr;
  char    name[32];
  char    rescn[32];
  char    val[32];
  char    EMsg[48];
  int     perm = 0;
  int     rc;
  
  memset(&pattr, 0, sizeof(pbs_attribute));
  memset(val, 0, 32);
  strcpy(name, "freddy");
  strcpy(rescn, "someresc");

  rc = decode_tv(&pattr, name, rescn, val, perm);
  sprintf(EMsg, "decode_tv unexpectedly failed: %d", rc);
  fail_unless(rc == PBSE_NONE, EMsg);

  }
END_TEST

START_TEST(test_two)
  {


  }
END_TEST


Suite *attr_fn_tv_suite(void)
  {
  Suite *s = suite_create("attr_fn_tv_suite methods");
  TCase *tc_core = tcase_create("decode_tv");
  tcase_add_test(tc_core, decode_tv);
  suite_add_tcase(s, tc_core);

  tc_core = tcase_create("decode_tv_no_dot");
  tcase_add_test(tc_core, decode_tv_no_dot);
  suite_add_tcase(s, tc_core);

  tc_core = tcase_create("decode_tv_two_dots");
  tcase_add_test(tc_core, decode_tv_two_dots);
  suite_add_tcase(s, tc_core);

  tc_core = tcase_create("decode_tv_not_digit");
  tcase_add_test(tc_core, decode_tv_not_digit);
  suite_add_tcase(s, tc_core);

  tc_core = tcase_create("decode_tv_null_val");
  tcase_add_test(tc_core, decode_tv_null_val);
  suite_add_tcase(s, tc_core);

  tc_core = tcase_create("decode_tv_val_empty");
  tcase_add_test(tc_core, decode_tv_val_empty);
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
  sr = srunner_create(attr_fn_tv_suite());
  srunner_set_log(sr, "attr_fn_tv_suite.log");
  srunner_run_all(sr, CK_NORMAL);
  number_failed = srunner_ntests_failed(sr);
  srunner_free(sr);
  return number_failed;
  }
