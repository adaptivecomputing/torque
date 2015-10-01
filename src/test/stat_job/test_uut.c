#include "license_pbs.h" /* See here for the software license */
#include <stdlib.h>
#include <stdio.h>

#include "pbs_error.h"
#include "pbs_job.h"
#include "test_stat_job.h"

bool include_in_status(int index);


START_TEST(test_include_in_status)
  {
  for (int i = 0; i < JOB_ATR_LAST; i++)
    {
    bool result = include_in_status(i);

    switch (i)
      {
      case JOB_ATR_jobname:
      case JOB_ATR_job_owner:
      case JOB_ATR_state:
      case JOB_ATR_resc_used:
      case JOB_ATR_in_queue:

        fail_unless(result == true);

        break;

      default:

        fail_unless(result == false);

        break;
      }
    }
  }
END_TEST

START_TEST(test_two)
  {


  }
END_TEST

Suite *stat_job_suite(void)
  {
  Suite *s = suite_create("stat_job_suite methods");
  TCase *tc_core = tcase_create("test_include_in_status");
  tcase_add_test(tc_core, test_include_in_status);
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
  sr = srunner_create(stat_job_suite());
  srunner_set_log(sr, "stat_job_suite.log");
  srunner_run_all(sr, CK_NORMAL);
  number_failed = srunner_ntests_failed(sr);
  srunner_free(sr);
  return number_failed;
  }
