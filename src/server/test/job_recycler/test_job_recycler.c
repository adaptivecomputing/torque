#include "license_pbs.h" /* See here for the software license */
#include "job_recycler.h"
#include "test_job_recycler.h"
#include <stdlib.h>
#include <stdio.h>
#include "pbs_error.h"

job_recycler recycler;

START_TEST(test_insert_into_recycler)
  {
  job *pj = job_alloc();
  initialize_recycler();

  fail_unless(insert_into_recycler(pj) == PBSE_NONE);

  fail_unless(recycler.rc_jobs.ra->num == 1);

  job *pj2 = job_alloc();
  fail_unless(insert_into_recycler(pj2) == PBSE_NONE);

  fail_unless(recycler.rc_jobs.ra->num == 2);

  fail_unless(insert_into_recycler(pj) == PBSE_NONE);

  fail_unless(recycler.rc_jobs.ra->num == 2);
  }
END_TEST

START_TEST(test_two)
  {


  }
END_TEST

Suite *job_recycler_suite(void)
  {
  Suite *s = suite_create("job_recycler_suite methods");
  TCase *tc_core = tcase_create("test_insert_into_recycler");
  tcase_add_test(tc_core, test_insert_into_recycler);
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
  sr = srunner_create(job_recycler_suite());
  srunner_set_log(sr, "job_recycler_suite.log");
  srunner_run_all(sr, CK_NORMAL);
  number_failed = srunner_ntests_failed(sr);
  srunner_free(sr);
  return number_failed;
  }
