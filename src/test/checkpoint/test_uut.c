#include "license_pbs.h" /* See here for the software license */
#include <pbs_config.h>
#include <stdlib.h>
#include <stdio.h>

#include "pbs_job.h"
#include "checkpoint.h"
#include "test_checkpoint.h"

#include "pbs_error.h"

int establish_server_connection(job *pjob);

extern bool connect_fail;

START_TEST(establish_server_connection_test)
  {
  job *pjob = (job *)calloc(1, sizeof(job));

  snprintf(pjob->ji_qs.ji_jobid, sizeof(pjob->ji_qs.ji_jobid), "1.napali");

  fail_unless(establish_server_connection(pjob) == -1);
  connect_fail = true;
  pjob->ji_wattr[JOB_ATR_at_server].at_val.at_str = strdup("bob");
  fail_unless(establish_server_connection(pjob) == -1);
  connect_fail = false;
  fail_unless(establish_server_connection(pjob) >= 0);
  }
END_TEST

START_TEST(test_two)
  {


  }
END_TEST

Suite *checkpoint_suite(void)
  {
  Suite *s = suite_create("checkpoint_suite methods");
  TCase *tc_core = tcase_create("establish_server_connection_test");
  tcase_add_test(tc_core, establish_server_connection_test);
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
  sr = srunner_create(checkpoint_suite());
  srunner_set_log(sr, "checkpoint_suite.log");
  srunner_run_all(sr, CK_NORMAL);
  number_failed = srunner_ntests_failed(sr);
  srunner_free(sr);
  return number_failed;
  }
