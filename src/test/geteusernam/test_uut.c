#include "license_pbs.h" /* See here for the software license */
#include "test_geteusernam.h"
#include <stdlib.h>
#include <stdio.h>
#include <string>
#include "pbs_error.h"
#include "attribute.h"

int node_exception_check(pbs_attribute *pattr, void *pobject, int actmode);
void get_user_host_from_user( std::string &user_host, const std::string user);


START_TEST(test_get_user_host_from_user)
  {
  std::string user_host;
  std::string user;

  user = "user@hostname";
  get_user_host_from_user(user_host, user);
  fail_unless(strcmp(user_host.c_str(), "hostname") == 0);

  user = "user";
  get_user_host_from_user(user_host, user);
  fail_unless(user_host.size() == 0);
  }
END_TEST

START_TEST(test_node_exception_check)
  {
  pbs_attribute pattr;
  memset(&pattr, 0, sizeof(pattr));
  size_t need = sizeof(struct array_strings) + 3 * sizeof(char *);
  struct array_strings *arst = (struct array_strings *)calloc(1, need);
  char *buf = (char *)calloc(100, sizeof(char));
  strcpy(buf, "napali");
  strcpy(buf + 7, "waimea");
  arst->as_buf = buf;
  arst->as_usedptr = 2;
  arst->as_bufsize = 100;
  arst->as_next = arst->as_buf + 14;
  arst->as_string[0] = arst->as_buf;
  arst->as_string[1] = arst->as_buf + 7;
  pattr.at_val.at_arst = arst;

  // FREE shouldn't do any checking
  fail_unless(node_exception_check(&pattr, &pattr, ATR_ACTION_FREE) == PBSE_NONE);
  // waimea is set to not exist, so we should error
  fail_unless(node_exception_check(&pattr, &pattr, ATR_ACTION_NEW) == PBSE_UNKNODE);
 
  // make it so waimea isn't seen and we should succeed
  arst->as_usedptr = 1;
  fail_unless(node_exception_check(&pattr, &pattr, ATR_ACTION_NEW) == PBSE_NONE);
  }
END_TEST


START_TEST(test_two)
  {


  }
END_TEST

Suite *geteusernam_suite(void)
  {
  Suite *s = suite_create("geteusernam_suite methods");
  TCase *tc_core = tcase_create("test_get_user_host_from_user");
  tcase_add_test(tc_core, test_get_user_host_from_user);
  tcase_add_test(tc_core, test_node_exception_check);
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
  sr = srunner_create(geteusernam_suite());
  srunner_set_log(sr, "geteusernam_suite.log");
  srunner_run_all(sr, CK_NORMAL);
  number_failed = srunner_ntests_failed(sr);
  srunner_free(sr);
  return number_failed;
  }
