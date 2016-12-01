#include "license_pbs.h" /* See here for the software license */
#include "req_signal.h"
#include "test_req_signal.h"
#include <stdlib.h>
#include <stdio.h>
#include "pbs_error.h"
#include "batch_request.h"
#include "attribute.h"

int issue_signal(job **, const char *, void (*func)(batch_request *), void *extra, char *extend);

extern char scaff_buffer[];
extern int  unlocked_job;
extern int  found_job;
extern int  relay_rc;
extern bool nullify_pointer;
extern bool find_job;

START_TEST(test_req_signaljob_relaying_msg)
  {
  struct batch_request request;
  job myjob;
  memset(&request, 0, sizeof(struct batch_request));
  memset(&myjob, 0, sizeof(job));
  myjob.ji_qs.ji_state = JOB_STATE_RUNNING;
  myjob.ji_qs.ji_un.ji_exect.ji_momaddr = 167838724;
  snprintf(request.rq_ind.rq_signal.rq_jid, PBS_MAXSVRJOBID, "%lu", (unsigned long)&myjob);
  memset(scaff_buffer, 0, 1024);
  req_signaljob(&request);
  const char *err = "relaying signal request to mom";
  fail_unless(strncmp(err, scaff_buffer, strlen(err)) == 0, "Error message was not constructed as expected");
  }
END_TEST

void free_null(pbs_attribute *attr);
void dummy_func(batch_request *preq) {}

void init_attr_def() 

  {
  extern attribute_def job_attr_def[];

  for (int i = 0; i < 10; i++)
    job_attr_def[i].at_free = free_null;
  }


START_TEST(test_issue_signal)
  {
  job pjob;
  job *ptr = &pjob;

  memset(&pjob, 0, sizeof(job));
  strcpy(pjob.ji_qs.ji_jobid, "1.nalthis");

  found_job = 0;
  unlocked_job = 0;
  relay_rc = 1;

  // Make sure that if we have a non-NULL job pointer the mutex is locked
  // found_job is set each time it's 'locked' in svr_find_job and unlocked_job is set each
  // time it's 'unlocked' in unlock_ji_mutex()
  fail_unless(issue_signal(&ptr, "SIGKILL", dummy_func, NULL, strdup("force")) == PBSE_NONE);
  fail_unless(ptr != NULL);
  fail_unless(found_job == unlocked_job); 

  relay_rc = 0;
  fail_unless(issue_signal(&ptr, "SIGKILL", dummy_func, NULL, strdup("force")) == PBSE_NONE);
  fail_unless(ptr != NULL);
  fail_unless(found_job == unlocked_job);

  nullify_pointer = true;
  relay_rc = 1;
  fail_unless(issue_signal(&ptr, "SIGKILL", dummy_func, NULL, strdup("force")) == PBSE_NONE);
  fail_unless(ptr != NULL);
  // On this code path we should have locked twice and unlocked only once, because we 'unlocked'
  // in relay_to_mom()
  fail_unless(found_job == unlocked_job + 1); 

  found_job = 0;
  unlocked_job = 0;
  nullify_pointer = true;
  relay_rc = 1;
  fail_unless(issue_signal(&ptr, "SIGKILL", dummy_func, NULL, NULL) != PBSE_NONE);
  fail_unless(ptr == NULL);
  // We come back NULL from relay_to_mom(), so we shouldn't change these variables
  fail_unless(unlocked_job == 0);
  fail_unless(found_job == 0);

  ptr = &pjob;
  nullify_pointer = false;
  relay_rc = 1;
  fail_unless(issue_signal(&ptr, "SIGKILL", dummy_func, NULL, NULL) != PBSE_NONE);
  fail_unless(ptr != NULL);
  fail_unless(unlocked_job == 0);
  fail_unless(found_job == 0);
  

  }
END_TEST


Suite *req_signal_suite(void)
  {
  Suite *s = suite_create("req_signal_suite methods");
  TCase *tc_core = tcase_create("test_req_signaljob_relaying_msg");
  tcase_add_test(tc_core, test_req_signaljob_relaying_msg);
  suite_add_tcase(s, tc_core);

  tc_core = tcase_create("test_issue_signal");
  tcase_add_test(tc_core, test_issue_signal);
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

  init_attr_def();

  rundebug();
  sr = srunner_create(req_signal_suite());
  srunner_set_log(sr, "req_signal_suite.log");
  srunner_run_all(sr, CK_NORMAL);
  number_failed = srunner_ntests_failed(sr);
  srunner_free(sr);
  return number_failed;
  }
