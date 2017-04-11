#include "license_pbs.h" /* See here for the software license */
#include "lib_site.h"
#include "test_site_check_u.h"
#include <stdlib.h>
#include <stdio.h>


#include "pbs_error.h"

int  site_check_user_map(job *pjob, char *luser, char *EMsg, int logging);
bool is_permitted_by_node_submit(const char *orighost, int logging);

extern bool  exists;
extern char *var;

START_TEST(test_is_permitted_by_node_submit)
  {
  int logging = 5;
  exists = true;
  fail_unless(is_permitted_by_node_submit("napali", logging) == false);
  fail_unless(is_permitted_by_node_submit("waimea", logging) == false);
  fail_unless(is_permitted_by_node_submit("lihue", logging) == true);
  fail_unless(is_permitted_by_node_submit("wailua", logging) == true);

  exists = false;
  fail_unless(is_permitted_by_node_submit("haole", logging) == false);
  }
END_TEST

START_TEST(test_site_check_user_map)
  {
  job   pjob;
  char *user = strdup("dbeer");
  char  emsg[1024];
  char *msg_ptr = emsg;

  pjob.ji_wattr[JOB_ATR_job_owner].at_val.at_str = strdup("dbeer@roshar.cosmere");
  var = NULL;
  exists = false;

  // get_variable() returns NULL, so we should get an error
  fail_unless(site_check_user_map(&pjob, user, msg_ptr, 10) == -1);

  // roshar.cosmere will pass but roshar won't, make sure we check the FQDN
  var = strdup("roshar.cosmere");
  fail_unless(site_check_user_map(&pjob, user, msg_ptr, 10) == 0);
 
  // threnody will pass but threnody.cosmere won't, make sure we check the short name
  var = strdup("threnody.cosmere");
  fail_unless(site_check_user_map(&pjob, user, msg_ptr, 10) == 0);

  }
END_TEST

Suite *site_check_u_suite(void)
  {
  Suite *s = suite_create("site_check_u_suite methods");
  TCase *tc_core = tcase_create("test_is_permitted_by_node_submit");
  tcase_add_test(tc_core, test_is_permitted_by_node_submit);
  suite_add_tcase(s, tc_core);

  tc_core = tcase_create("test_site_check_user_map");
  tcase_add_test(tc_core, test_site_check_user_map);
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
  sr = srunner_create(site_check_u_suite());
  srunner_set_log(sr, "site_check_u_suite.log");
  srunner_run_all(sr, CK_NORMAL);
  number_failed = srunner_ntests_failed(sr);
  srunner_free(sr);
  return number_failed;
  }
