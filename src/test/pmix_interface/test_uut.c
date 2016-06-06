
#include "pmix_tracker.hpp"
#include "pmix_operation.hpp"

#include <check.h>

void notify_fence_complete(job *pjob);

extern int completed_op;

START_TEST(test_fences)
  {
  std::string    jid("1.napali");
  pmix_operation f;
  job            pjob;

  completed_op = 0;
  memset(&pjob, 0, sizeof(pjob));
  strcpy(pjob.ji_qs.ji_jobid, "2.napali");
  pending_fences[jid] = f;
  notify_fence_complete(&pjob);
  fail_unless(completed_op == 0);
  
  strcpy(pjob.ji_qs.ji_jobid, jid.c_str());
  notify_fence_complete(&pjob);
  fail_unless(completed_op == 1);
  }
END_TEST


Suite *pmix_interface_suite(void)
  {
  Suite *s = suite_create("pmix_interface_suite methods");
  TCase *tc_core = tcase_create("test_fences");
  tcase_add_test(tc_core, test_fences);
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
  sr = srunner_create(pmix_interface_suite());
  srunner_set_log(sr, "pmix_interface_suite.log");
  srunner_run_all(sr, CK_NORMAL);
  number_failed = srunner_ntests_failed(sr);
  srunner_free(sr);
  return number_failed;
  }

