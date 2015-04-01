#include "license_pbs.h" /* See here for the software license */
#include "lib_site.h"
#include "test_site_alt_rte.h"
#include <stdlib.h>
#include <stdio.h>
#include "pbs_job.h" /* job */
#include "queue.h" /* pbs_queue */

extern job *scaf_pjob;
extern pbs_queue *scaf_qp;
extern int scaf_retry_time;

#include "pbs_error.h"

START_TEST(test_one)
  {
  /* As this is site specific, the implementation is partial.
   * The return value is a call to default_router so this just
   * makes sure that the parameters aren't corrupted */
  int rc = -1;
  job *pjob = (job *)calloc(1, sizeof(job));
  pbs_queue *qp = (pbs_queue *)calloc(1, sizeof(pbs_queue));
  int retry_time = 3;
  rc = site_alt_router(pjob, qp, retry_time);
  if (rc)
    {;}
  fail_unless(pjob == scaf_pjob, "job ptr changed in the function!!!!");
  fail_unless(qp == scaf_qp, "queue ptr changed in the function!!!!");
  fail_unless(retry_time == scaf_retry_time, "retry value changed in the function!!!!");
  free(pjob);
  free(qp);
  }
END_TEST

Suite *site_alt_rte_suite(void)
  {
  Suite *s = suite_create("site_alt_rte_suite methods");
  TCase *tc_core = tcase_create("test_one");
  tcase_add_test(tc_core, test_one);
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
/*  rundebug(); */
  sr = srunner_create(site_alt_rte_suite());
  srunner_set_log(sr, "site_alt_rte_suite.log");
  srunner_run_all(sr, CK_NORMAL);
  number_failed = srunner_ntests_failed(sr);
  srunner_free(sr);
  return number_failed;
  }
