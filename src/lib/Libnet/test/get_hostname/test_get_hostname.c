#include "license_pbs.h" /* See here for the software license */
#include "lib_net.h"
#include "test_get_hostname.h"
#include <stdlib.h>
#include <stdio.h>


#include "pbs_error.h"

START_TEST(get_fullhostname_canonical_name_in)
  {
  char canonical_name[20];
  char server_name[20];
  int  bufsize = 20;
  char EMsg[128];
  char log_msg[128];
  int  rc;

  /* This test checks to make sure get_fullhostname only modifies 
     a short dns name. If a '.' is in the name it should not try
     and resolve to the canonical form */

  /* make a dns canonical name. The '.' is what is important */
  strcpy(canonical_name, "kmn.ac");
  rc = get_fullhostname(canonical_name, server_name, bufsize, EMsg);

  sprintf(log_msg, "rc = %d", rc);
  fail_unless(rc == PBSE_NONE, log_msg);
  fail_unless((strcmp(canonical_name, server_name) == 0), "names are not the same");


  }
END_TEST

START_TEST(test_two)
  {


  }
END_TEST

Suite *get_hostname_suite(void)
  {
  Suite *s = suite_create("get_hostname_suite methods");
  TCase *tc_core = tcase_create("get_fullhostname_canonical_name_in");
  tcase_add_test(tc_core, get_fullhostname_canonical_name_in);
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
  sr = srunner_create(get_hostname_suite());
  srunner_set_log(sr, "get_hostname_suite.log");
  srunner_run_all(sr, CK_NORMAL);
  number_failed = srunner_ntests_failed(sr);
  srunner_free(sr);
  return number_failed;
  }
