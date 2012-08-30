#include "license_pbs.h" /* See here for the software license */
#include "job_func.h"
#include "test_job_func.h"
#include <stdlib.h>
#include <stdio.h>
#include "pbs_error.h"

extern attribute_def job_attr_def[];
int hostname_in_externals(char *, char *);
int fix_cray_exec_hosts(job *);
int fix_external_exec_hosts(job *);
int change_external_job_name(job *);
int split_job(job *);

char buf[4096];

START_TEST(hostname_in_externals_test)
  {
  char *externals = strdup("one+two+three");
  
  fail_unless(hostname_in_externals("one", externals) == TRUE, "couldn't find entry");
  fail_unless(hostname_in_externals("two", externals) == TRUE, "couldn't find entry");
  fail_unless(hostname_in_externals("three", externals) == TRUE, "couldn't find entry");

  /* corner cases */
  fail_unless(hostname_in_externals("thre", externals) == FALSE, "false positive");
  fail_unless(hostname_in_externals("tw", externals) == FALSE, "false positive");
  fail_unless(hostname_in_externals("on", externals) == FALSE, "false positive");
  fail_unless(hostname_in_externals("wo", externals) == FALSE, "false positive");
  fail_unless(hostname_in_externals("ne", externals) == FALSE, "false positive");
  fail_unless(hostname_in_externals("hree", externals) == FALSE, "false positive");
  fail_unless(hostname_in_externals("tthree", externals) == FALSE, "false positive");
  fail_unless(hostname_in_externals("ttwo", externals) == FALSE, "false positive");
  fail_unless(hostname_in_externals("oone", externals) == FALSE, "false positive");
  }
END_TEST




START_TEST(fix_cray_exec_hosts_test)
  {
  char *externals = "one+two+three";
  char *exec1 = "one/0+one/1+napali/0+napali/1+napali/2+napali/3+a1/0+a1/1+a1/2+a1/3";
  char *exec2 = "napali/0+napali/1+napali/2+napali/3+a1/0+a1/1+a1/2+a1/3+two/0+two/1";
  char *exec3 = "napali/0+napali/1+napali/2+napali/3+three/0+three/1+a1/0+a1/1+a1/2+a1/3";
  job   pjob;

  memset(&pjob, 0, sizeof(pjob));

  pjob.ji_wattr[JOB_ATR_exec_host].at_val.at_str = strdup(exec1);
  pjob.ji_wattr[JOB_ATR_external_nodes].at_val.at_str = strdup(externals);


  fix_cray_exec_hosts(&pjob);
  snprintf(buf, sizeof(buf), "exec hosts should be %s but is %s",
    "napali/0+napali/1+napali/2+napali/3+a1/0+a1/1+a1/2+a1/3",
    pjob.ji_wattr[JOB_ATR_exec_host].at_val.at_str);

  fail_unless(!strcmp(pjob.ji_wattr[JOB_ATR_exec_host].at_val.at_str, "napali/0+napali/1+napali/2+napali/3+a1/0+a1/1+a1/2+a1/3"), buf);

  pjob.ji_wattr[JOB_ATR_exec_host].at_val.at_str = strdup(exec2);
  fix_cray_exec_hosts(&pjob);
  snprintf(buf, sizeof(buf), "exec hosts should be %s but is %s",
    "napali/0+napali/1+napali/2+napali/3+a1/0+a1/1+a1/2+a1/3",
    pjob.ji_wattr[JOB_ATR_exec_host].at_val.at_str);

  fail_unless(!strcmp(pjob.ji_wattr[JOB_ATR_exec_host].at_val.at_str, "napali/0+napali/1+napali/2+napali/3+a1/0+a1/1+a1/2+a1/3"), buf);

  pjob.ji_wattr[JOB_ATR_exec_host].at_val.at_str = strdup(exec3);
  fix_cray_exec_hosts(&pjob);
  snprintf(buf, sizeof(buf), "exec hosts should be %s but is %s",
    "napali/0+napali/1+napali/2+napali/3+a1/0+a1/1+a1/2+a1/3",
    pjob.ji_wattr[JOB_ATR_exec_host].at_val.at_str);

  fail_unless(!strcmp(pjob.ji_wattr[JOB_ATR_exec_host].at_val.at_str, "napali/0+napali/1+napali/2+napali/3+a1/0+a1/1+a1/2+a1/3"), buf);
  }
END_TEST



START_TEST(fix_external_exec_hosts_test)
  {
  char *externals = "one+two+three";
  char *exec1 = "one/0+one/1+napali/0+napali/1+napali/2+napali/3+a1/0+a1/1+a1/2+a1/3";
  char *exec2 = "napali/0+napali/1+napali/2+napali/3+a1/0+a1/1+a1/2+a1/3+two/0+two/1";
  char *exec3 = "napali/0+napali/1+napali/2+napali/3+three/0+three/1+a1/0+a1/1+a1/2+a1/3";
  job   pjob;

  memset(&pjob, 0, sizeof(pjob));

  fail_unless(fix_external_exec_hosts(&pjob) == -2, "error codes not correctly checked");

  pjob.ji_wattr[JOB_ATR_exec_host].at_val.at_str = strdup(exec1);
  pjob.ji_wattr[JOB_ATR_external_nodes].at_val.at_str = strdup(externals);

  fix_external_exec_hosts(&pjob);
  fail_unless(!strcmp(pjob.ji_wattr[JOB_ATR_exec_host].at_val.at_str, "one/0+one/1"), "not set correctly");

  
  pjob.ji_wattr[JOB_ATR_exec_host].at_val.at_str = strdup(exec2);
  fix_external_exec_hosts(&pjob);
  fail_unless(!strcmp(pjob.ji_wattr[JOB_ATR_exec_host].at_val.at_str, "two/0+two/1"), "not set correctly");
  
  pjob.ji_wattr[JOB_ATR_exec_host].at_val.at_str = strdup(exec3);
  fix_external_exec_hosts(&pjob);
  fail_unless(!strcmp(pjob.ji_wattr[JOB_ATR_exec_host].at_val.at_str, "three/0+three/1"), "not set correctly");
  }
END_TEST



START_TEST(change_external_job_name_test)
  {
  job pjob;

  strcpy(pjob.ji_qs.ji_jobid, "1.napali");
  change_external_job_name(&pjob);
  fail_unless(!strcmp(pjob.ji_qs.ji_jobid, "1-0.napali"), "name not changed correctly");

  strcpy(pjob.ji_qs.ji_jobid, "12.napali");
  change_external_job_name(&pjob);
  fail_unless(!strcmp(pjob.ji_qs.ji_jobid, "12-0.napali"), "name not changed correctly");

  strcpy(pjob.ji_qs.ji_jobid, "1005.napali");
  change_external_job_name(&pjob);
  fail_unless(!strcmp(pjob.ji_qs.ji_jobid, "1005-0.napali"), "name not changed correctly");
  }
END_TEST



START_TEST(split_job_test)
  {
  job   pjob;
  job  *external;
  job  *cray;
  char *externals = "one+two+three";
  char *exec1 = "one/0+one/1+napali/0+napali/1+napali/2+napali/3+a1/0+a1/1+a1/2+a1/3";

  memset(&pjob, 0, sizeof(pjob));

  strcpy(pjob.ji_qs.ji_jobid, "12.napali");
  pjob.ji_wattr[JOB_ATR_exec_host].at_val.at_str = strdup(exec1);
  pjob.ji_wattr[JOB_ATR_exec_host].at_flags |= ATR_VFLAG_SET;
  pjob.ji_wattr[JOB_ATR_external_nodes].at_val.at_str = strdup(externals);
  pjob.ji_wattr[JOB_ATR_external_nodes].at_flags |= ATR_VFLAG_SET;

  split_job(&pjob);

  external = pjob.ji_external_clone;
  cray     = pjob.ji_cray_clone;

  fail_unless(!strcmp(external->ji_wattr[JOB_ATR_exec_host].at_val.at_str, "one/0+one/1"), "external's exec host bad");
  fail_unless(!strcmp(cray->ji_wattr[JOB_ATR_exec_host].at_val.at_str, "napali/0+napali/1+napali/2+napali/3+a1/0+a1/1+a1/2+a1/3"), "cray's exec host bad");
  fail_unless(!strcmp(external->ji_qs.ji_jobid, "12-0.napali"), "external's name not correct");
  }
END_TEST




Suite *job_func_suite(void)
  {
  Suite *s = suite_create("job_func_suite methods");
  TCase *tc_core = tcase_create("hostname_in_externals_test");
  tcase_add_test(tc_core, hostname_in_externals_test);
  suite_add_tcase(s, tc_core);

  tc_core = tcase_create("fix_cray_exec_hosts_test");
  tcase_add_test(tc_core, fix_cray_exec_hosts_test);
  suite_add_tcase(s, tc_core);

  tc_core = tcase_create("fix_external_exec_hosts_test");
  tcase_add_test(tc_core, fix_external_exec_hosts_test);
  suite_add_tcase(s, tc_core);

  tc_core = tcase_create("change_external_job_name_test");
  tcase_add_test(tc_core, change_external_job_name_test);
  suite_add_tcase(s, tc_core);

  tc_core = tcase_create("split_job_test");
  tcase_add_test(tc_core, split_job_test);
  suite_add_tcase(s, tc_core);

  return(s);
  }

void rundebug()
  {
  }

int main(void)
  {
  int number_failed = 0;
  SRunner *sr = NULL;
  rundebug();
  sr = srunner_create(job_func_suite());
  srunner_set_log(sr, "job_func_suite.log");
  srunner_run_all(sr, CK_NORMAL);
  number_failed = srunner_ntests_failed(sr);
  srunner_free(sr);
  return number_failed;
  }
