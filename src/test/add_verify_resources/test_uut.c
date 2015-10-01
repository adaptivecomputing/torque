#include "license_pbs.h" /* See here for the software license */
#include <stdlib.h>
#include <stdio.h>

#include "pbs_cmds.h"
#include "pbs_error.h"
#include "test_add_verify_resources.h"

START_TEST(test_simple)
  {
  /* standard cases*/
  /* attr = "VAL,VAL1"*/
  fail_unless(add_verify_resources(NULL, (char*)"hostlist=\"1,2\"", 2)==0);
  /* attr = ^[*]"VAL,VAL1"*/
  fail_unless(add_verify_resources(NULL, (char*)"hostlist=^\"1,2\"", 2)==0);
  fail_unless(add_verify_resources(NULL, (char*)"hostlist=*\"1,2\"", 2)==0);
  fail_unless(add_verify_resources(NULL, (char*)"hostlist=*val[1-3]", 2)==0);
  fail_unless(add_verify_resources(NULL, (char*)"hostlist=*val[1-3]+val2[1,2,3]", 2)==0);
  fail_unless(add_verify_resources(NULL, (char*)"hostlist=*\"val[1-3],val2\"", 2)==0);
  /* attr = VAL[n-m]*/
  fail_unless(add_verify_resources(NULL, (char*)"hostlist=val[1,2]", 2)==0);
  /* attr = VAL[n,m,...,k]*/
  fail_unless(add_verify_resources(NULL, (char*)"hostlist=val[1,2,3,4,5,6,7,78,8]", 2)==0);
  /* attr = VAL[n-m]VAL1*/
  fail_unless(add_verify_resources(NULL, (char*)"hostlist=val[1-2]val1", 2)==0);
  /* attr = VAL[n,m...,k]VAL1*/
  fail_unless(add_verify_resources(NULL, (char*)"hostlist=val[1,2,3,3,4]val1", 2)==0);
  /* attr = VAL[n,m...,k],VAL1*/
  fail_unless(add_verify_resources(NULL, (char*)"hostlist=\"val[1,2,3,3,4],val1\"", 2)==0);
  /* attr = VAL[n-m],VAL1*/
  fail_unless(add_verify_resources(NULL, (char*)"hostlist=\"val[1-4],val1\"", 2)==0);
  }
END_TEST

START_TEST(test_advanced)
  {
  /* extentions*/
  /* attr = VAL[n-m],VAL1[n-m]VAL2*/
  fail_unless(add_verify_resources(NULL, (char*)"hostlist=\"val[1-4],val1[1-4]val2\"", 2)==0);

  /* attr = VAL[n-m],VAL1[n,m]VAL2*/
  fail_unless(add_verify_resources(NULL, (char*)"hostlist=\"val[1-4],val1\"", 2)==0);

  /* attr = VAL[n-m,k,r-m],VAL1[n,m]VAL2*/
  fail_unless(add_verify_resources(NULL, (char*)"hostlist=\"val[1-4,5,7-10],val1[1,2]val+val2[1,2-10]\"", 2)==0);
  }
END_TEST


Suite *add_verify_resources_suite(void)
  {
  Suite *s = suite_create("add_verify_resources_suite methods");
  TCase *tc_core = tcase_create("test_simple");
  tcase_add_test(tc_core, test_simple);
  suite_add_tcase(s, tc_core);

  tc_core = tcase_create("test_advanced");
  tcase_add_test(tc_core, test_advanced);
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
  sr = srunner_create(add_verify_resources_suite());
  srunner_set_log(sr, "add_verify_resources_suite.log");
  srunner_run_all(sr, CK_NORMAL);
  number_failed = srunner_ntests_failed(sr);
  srunner_free(sr);
  return number_failed;
  }
