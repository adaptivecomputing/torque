#include "license_pbs.h" /* See here for the software license */
#include "test_attr_fn_tokens.h"
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include "attribute.h" /* decode_str */


#include "pbs_error.h"

START_TEST(test_decode_token)
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
  strcpy(val, "someval:33.3");

  rc = decode_tokens(&pattr, name, rescn, val, perm);
  sprintf(EMsg, "decode tokens failed: %d", rc);
  fail_unless(rc == PBSE_NONE, EMsg);

  }
END_TEST

START_TEST(test_decode_token_no_colon)
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
  strcpy(val, "someval");

  rc = decode_tokens(&pattr, name, rescn, val, perm);
  sprintf(EMsg, "decode tokens failed: %d", rc);
  fail_unless(rc == PBSE_BADATVAL, EMsg);
  }
END_TEST

Suite *attr_fn_tokens_suite(void)
  {
  Suite *s = suite_create("attr_fn_tokens_suite methods");
  TCase *tc_core = tcase_create("test_tokens");
  tcase_add_test(tc_core, test_decode_token);
  tcase_set_timeout(tc_core, 600);
  suite_add_tcase(s, tc_core);

  tc_core = tcase_create("test_decode_token_no_colon");
  tcase_add_test(tc_core, test_decode_token_no_colon);
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
  sr = srunner_create(attr_fn_tokens_suite());
  srunner_set_log(sr, "attr_fn_tokens_suite.log");
  srunner_run_all(sr, CK_NORMAL);
  number_failed = srunner_ntests_failed(sr);
  srunner_free(sr);
  return number_failed;
  }
