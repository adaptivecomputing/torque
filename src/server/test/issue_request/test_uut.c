#include "license_pbs.h" /* See here for the software license */

#include <string>
#include <stdlib.h>
#include <stdio.h>

#include "issue_request.h"
#include "test_uut.h"
#include "pbs_error.h"
#include "attribute.h"

bool return_addr;
bool local_connect;
bool net_rc_retry;
bool connect_error;

extern std::string rq_id_str;
void queue_a_retry_task(batch_request *preq, void (*replyfunc)(struct work_task *));

START_TEST(queue_a_retry_task_test)
  {
  batch_request *preq = (batch_request *)calloc(1, sizeof(batch_request));
  preq->rq_id = strdup("tom");

  queue_a_retry_task(preq, NULL);
  // rq_id_str is a sensing variable set in set_task()
  // this tells us we made a deep copy because we acquired a new rq_id for the 
  // request that we are reissuing.
  fail_unless(rq_id_str != "tom");
  }
END_TEST


START_TEST(test_one)
  {
  job testJob;
  job *pTestJob;
  struct batch_request request;

  memset(&testJob,0,sizeof(job));
  memset(&request,0,sizeof(request));
  pTestJob = &testJob;

  fail_unless(relay_to_mom(&pTestJob,&request,NULL) == PBSE_BADSTATE);

  decode_str(&testJob.ji_wattr[JOB_ATR_exec_host],NULL,NULL,"Sherrie",0);

  fail_unless(relay_to_mom(&pTestJob,&request,NULL) == PBSE_NONE);

  }
END_TEST

Suite *issue_request_suite(void)
  {
  Suite *s = suite_create("issue_request_suite methods");
  TCase *tc_core = tcase_create("test_one");
  tcase_add_test(tc_core, test_one);
  suite_add_tcase(s, tc_core);

  tc_core = tcase_create("test_reissue_to_svr");
  tcase_add_test(tc_core, queue_a_retry_task_test);
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
  sr = srunner_create(issue_request_suite());
  srunner_set_log(sr, "issue_request_suite.log");
  srunner_run_all(sr, CK_NORMAL);
  number_failed = srunner_ntests_failed(sr);
  srunner_free(sr);
  return number_failed;
  }
