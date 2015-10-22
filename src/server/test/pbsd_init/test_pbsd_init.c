#include "license_pbs.h" /* See here for the software license */
#include "pbsd_init.h"
#include "pbs_job.h"
#include "test_pbsd_init.h"
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <log.h>

#include "pbs_error.h"

int mk_subdirs(char **);
int pbsd_init_reque(job *, int);

extern char global_log_ext_msg[LOCAL_LOG_BUF_SIZE];
extern int enque_rc;
extern int evaluated;
extern int aborted;

START_TEST(test_one)
  {


  }
END_TEST

START_TEST(test_pbsd_init_reque)
  {
  job *pjob = (job *)calloc(1, sizeof(job));

  evaluated = 0;
  aborted = 0;
  enque_rc = PBSE_NONE;
  fail_unless(pbsd_init_reque(pjob, 0) == PBSE_NONE);
  fail_unless(evaluated == 0);

  enque_rc = PBSE_BADDEPEND;
  fail_unless(pbsd_init_reque(pjob, 1) == PBSE_NONE);
  fail_unless(evaluated == 1);
  
  enque_rc = 10;
  fail_unless(pbsd_init_reque(pjob, 1) == 10);
  fail_unless(evaluated == 1);
  }
END_TEST

Suite *pbsd_init_suite(void)
  {
  Suite *s = suite_create("pbsd_init_suite methods");
  TCase *tc_core = tcase_create("test_one");
  tcase_add_test(tc_core, test_one);
  suite_add_tcase(s, tc_core);

  tc_core = tcase_create("test_pbsd_init_reque");
  tcase_add_test(tc_core, test_pbsd_init_reque);
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
  sr = srunner_create(pbsd_init_suite());
  srunner_set_log(sr, "pbsd_init_suite.log");
  srunner_run_all(sr, CK_NORMAL);
  number_failed = srunner_ntests_failed(sr);
  srunner_free(sr);
  return number_failed;
  }
