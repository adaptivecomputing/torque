#include "license_pbs.h" /* See here for the software license */
#include "req_runjob.h"
#include "test_uut.h"
#include <stdlib.h>
#include <stdio.h>
#include "pbs_error.h"
#include "pbs_job.h"
extern char scaff_buffer[];


int requeue_job(job *pjob);
extern int send_job_to_mom(job **, batch_request *, job *);
char      *get_mail_text(batch_request *preq, const char *job_id);


START_TEST(requeue_job_test)
  {
  job pjob;

  memset(&pjob, 0, sizeof(pjob));

  strcpy(pjob.ji_qs.ji_destin, "tom");

  fail_unless(requeue_job(&pjob) == -1, "didn't fail with non-existent node");

  strcpy(pjob.ji_qs.ji_destin, "bob");
  fail_unless(requeue_job(&pjob) == 0, "call failed");
  fail_unless(pjob.ji_qs.ji_destin[0] == '\0', "destination is still set");
  fail_unless(pjob.ji_qs.ji_state == JOB_STATE_QUEUED, "job not set to queued");
  fail_unless(pjob.ji_qs.ji_substate == JOB_SUBSTATE_QUEUED, "job not set to queued");
  }
END_TEST


START_TEST(test_get_mail_text)
  {
  batch_request  preq;
  const char    *txt = "tom is bob";
  char          *ret_txt;

  memset(&preq, 0, sizeof(preq));
  preq.rq_reply.brp_un.brp_txt.brp_txtlen = 999000001;
  preq.rq_reply.brp_un.brp_txt.brp_str = strdup(txt);
  fail_unless(get_mail_text(&preq, "1.napali") == NULL);
  
  preq.rq_reply.brp_un.brp_txt.brp_txtlen = 10;
  fail_unless((ret_txt = get_mail_text(&preq, "1.napali")) != NULL);
  fail_unless(!strcmp(ret_txt, txt));
  }
END_TEST


START_TEST(test_two)
  {
  struct batch_request request;
  job myjob;
  memset(&request, 0, sizeof(struct batch_request));
  memset(&myjob, 0, sizeof(job));
  myjob.ji_qs.ji_state = JOB_STATE_RUNNING;
  myjob.ji_qs.ji_un.ji_exect.ji_momaddr = 167838724;
  //setting up so that my mock svr_find_job would return this job..
  snprintf(myjob.ji_qs.ji_jobid, PBS_MAXSVRJOBID, "%lu", (unsigned long)&myjob);
  memset(scaff_buffer, 0, 1024);

  job *pjob = &myjob;
  send_job_to_mom(&pjob, &request, NULL);
  fail_unless(strcmp("unable to run job, send to MOM '10.1.4.4' failed",
    scaff_buffer) == 0, "Error message was not constructed as expected");
  }
END_TEST




Suite *req_runjob_suite(void)
  {
  Suite *s = suite_create("req_runjob_suite methods");
  TCase *tc_core = tcase_create("requeue_job_test");
  tcase_add_test(tc_core, requeue_job_test);
  suite_add_tcase(s, tc_core);

  tc_core = tcase_create("test_two");
  tcase_add_test(tc_core, test_two);
  tcase_add_test(tc_core, test_get_mail_text);
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
  sr = srunner_create(req_runjob_suite());
  srunner_set_log(sr, "req_runjob_suite.log");
  srunner_run_all(sr, CK_NORMAL);
  number_failed = srunner_ntests_failed(sr);
  srunner_free(sr);
  return number_failed;
  }
