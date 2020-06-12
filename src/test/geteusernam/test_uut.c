#include "license_pbs.h" /* See here for the software license */
#include <stdlib.h>
#include <stdio.h>
#include <string>
#include "pbs_error.h"
#include "pbs_job.h"
#include "attribute.h"
#include "test_geteusernam.h"

int node_exception_check(pbs_attribute *pattr, void *pobject, int actmode);
bool is_permitted_by_node_submit(const char *orighost, int logging);
bool is_user_allowed_to_submit_jobs(job *pjob, const char *luser, char *EMsg, int logging);

extern bool  exists;
extern char *var;


START_TEST(test_is_user_allowed_to_submit_jobs)
  {
  char *user = strdup("dbeer");
  job   pjob;
  char  emsg[1024];

  pjob.ji_wattr[JOB_ATR_job_owner].at_val.at_str = strdup("dbeer@roshar.cosmere");
  var = NULL;
  exists = false;

  // get_variable() retuns NULL, so we should fail
  fail_unless(is_user_allowed_to_submit_jobs(&pjob, user, emsg, 10) == false);

  // roshar.cosmere will pass but roshar won't; make sure the FQDN works
  var = strdup("roshar.cosmere");
  fail_unless(is_user_allowed_to_submit_jobs(&pjob, user, emsg, 10) == true);
  
  // threnody will pass but threnody.cosmere won't; make sure we check the short name
  var = strdup("threnody.cosmere");
  fail_unless(is_user_allowed_to_submit_jobs(&pjob, user, emsg, 10) == true);
  }
END_TEST


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
  exists = false;
  // nodes are set to not exist, so we should error
  fail_unless(node_exception_check(&pattr, &pattr, ATR_ACTION_NEW) == PBSE_UNKNODE);
 
  // the nodes exist 
  exists = true;
  fail_unless(node_exception_check(&pattr, &pattr, ATR_ACTION_NEW) == PBSE_NONE);
  }
END_TEST


Suite *geteusernam_suite(void)
  {
  Suite *s = suite_create("geteusernam_suite methods");
  TCase *tc_core = tcase_create("test_get_user_host_from_user");
  tcase_add_test(tc_core, test_node_exception_check);
  suite_add_tcase(s, tc_core);

  tc_core = tcase_create("test_is_permitted_by_node_submit");
  tcase_add_test(tc_core, test_is_permitted_by_node_submit);
  tcase_add_test(tc_core, test_is_user_allowed_to_submit_jobs);
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
