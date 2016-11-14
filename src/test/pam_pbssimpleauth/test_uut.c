#include "license_pbs.h" /* See here for the software license */
#include <pbs_config.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <iostream>
#include <check.h>
#include <security/pam_appl.h>
#include <security/pam_modules.h>


#include "pbs_error.h"
#include "pbs_job.h"
#include "log.h"

int job_read_xml(

  const char *filename,  /* I */   /* pathname to job save file */
  job  *pjob,     /* M */   /* pointer to a pointer of job structure to fill info */
  char *log_buf,   /* O */   /* buffer to hold error message */
  size_t buf_len);

START_TEST(test_job_read_xml)
  {
  job xjob;
  int rc;
  char log_buf[LOCAL_LOG_BUF_SIZE];

  rc = job_read_xml("job.xml", &xjob, log_buf, sizeof(log_buf));

  fail_unless(rc == PBSE_NONE);
  fail_unless(xjob.ji_qs.ji_un.ji_momt.ji_exuid == 500);
  fail_unless(xjob.ji_qs.ji_state == JOB_STATE_RUNNING);
  fail_unless(xjob.ji_qs.ji_substate == JOB_SUBSTATE_RUNNING);
  
  }
END_TEST

Suite *pam_pbssimpleauth_suite(void)
  {
  Suite *s = suite_create("pam_pbssimpleauth_suite methods");
  TCase *tc_core = tcase_create("test_job_read_xml");
  tcase_add_test(tc_core, test_job_read_xml);
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
  sr = srunner_create(pam_pbssimpleauth_suite());
  srunner_set_log(sr, "pam_pbssimpleauth_suite.log");
  srunner_run_all(sr, CK_NORMAL);
  number_failed = srunner_ntests_failed(sr);
  srunner_free(sr);
  return number_failed;
  }
