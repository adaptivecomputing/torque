#include "license_pbs.h" /* See here for the software license */
#include "lib_site.h"
#include "test_site_map_usr.h"
#include <stdlib.h>
#include <stdio.h>


#include "pbs_error.h"

START_TEST(test_one)
  {
  char *uname = strdup("someone");
  char *host = NULL;
  char *ret_uname = NULL;
  ret_uname = site_map_user(uname, host);
  fail_unless(strcmp(uname, ret_uname) == 0, "Usernames don't match");
  free(uname);
  }
END_TEST


Suite *site_map_usr_suite(void)
  {
  Suite *s = suite_create("site_map_usr_suite methods");
  TCase *tc_core = tcase_create("test_one");
  tcase_add_test(tc_core, test_one);
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
  sr = srunner_create(site_map_usr_suite());
  srunner_set_log(sr, "site_map_usr_suite.log");
  srunner_run_all(sr, CK_NORMAL);
  number_failed = srunner_ntests_failed(sr);
  srunner_free(sr);
  return number_failed;
  }
