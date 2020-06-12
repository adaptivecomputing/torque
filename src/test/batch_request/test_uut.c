#include <stdio.h>
#include <stdlib.h>
#include "batch_request.h"
#include <check.h>


START_TEST(test_one)
  {

  batch_request *pBr1 = new (batch_request);
  batch_request *pBr2 = new (batch_request);
  batch_request *pBr3 = new (batch_request);
  batch_request *pBr4 = new (batch_request);

  int rc = get_batch_request_id(pBr1);
  fail_unless(rc == PBSE_NONE);
  rc = get_batch_request_id(pBr2);
  fail_unless(rc == PBSE_NONE);
  rc = get_batch_request_id(pBr3);
  fail_unless(rc == PBSE_NONE);
  rc = get_batch_request_id(pBr4);
  fail_unless(rc == PBSE_NONE);

  batch_request *tmp = get_batch_request((char *)"0");
  fail_unless(tmp == pBr1);
  tmp = get_remove_batch_request((char *)"0");
  fail_unless(tmp == pBr1);
  tmp = get_batch_request((char *)"0");
  fail_unless(tmp == NULL);

  tmp = get_batch_request((char *)"1");
  fail_unless(tmp == pBr2);
  rc = remove_batch_request((char *)"1");
  fail_unless(rc == PBSE_NONE);
  tmp = get_batch_request((char *)"1");
  fail_unless(tmp == NULL);



  }
END_TEST


START_TEST(test_constructor)
  {
  batch_request *preq = new batch_request(PBS_BATCH_QueueJob);

  fail_unless(preq->rq_type == PBS_BATCH_QueueJob);
  fail_unless(preq->rq_conn == -1);
  fail_unless(preq->rq_orgconn == -1);
  fail_unless(preq->rq_reply.brp_choice == BATCH_REPLY_CHOICE_NULL);
  fail_unless(preq->rq_noreply == FALSE);
  fail_unless(preq->rq_time > 0);

  delete preq;
  }
END_TEST


START_TEST(test_br_copy_constructor)
  {
  batch_request preq;
  batch_request *dup;

  preq.rq_perm = 1;
  strcpy(preq.rq_user, "dbeer");
  strcpy(preq.rq_host, "napali");
  preq.rq_extend = strdup("tom");
  preq.rq_type = PBS_BATCH_RunJob;
  preq.rq_ind.rq_run.rq_destin = strdup("napali");

  dup = new batch_request(preq);
  fail_unless(dup != NULL);
  fail_unless(!strcmp(dup->rq_extend, "tom"));
  fail_unless(!strcmp(dup->rq_user, "dbeer"));
  fail_unless(!strcmp(dup->rq_host, "napali"));
  fail_unless(!strcmp(dup->rq_extend, "tom"));
  fail_unless(!strcmp(dup->rq_ind.rq_run.rq_destin, "napali"));

  preq.rq_type = PBS_BATCH_Rerun;
  const char *rerun_jobid = "4.roshar";
  strcpy(preq.rq_ind.rq_rerun, rerun_jobid);
  batch_request *rerun_dep = new batch_request(preq);
  fail_unless(!strcmp(rerun_dep->rq_ind.rq_rerun, rerun_jobid));
  }
END_TEST 




Suite *batch_request_suite(void)
  {
  Suite *s = suite_create("batch_request suite methods");

  TCase *tc_core = tcase_create("test_one");
  tcase_add_test(tc_core, test_one);
  tcase_add_test(tc_core, test_br_copy_constructor);
  suite_add_tcase(s, tc_core);

  tc_core = tcase_create("test_constructor");
  tcase_add_test(tc_core, test_constructor);
  suite_add_tcase(s, tc_core);

  return(s);
  }




void rundebug()
  {
  }




int main(void)
  {
  int number_failed = 0;
  SRunner *sr = NULL;
  rundebug();
  sr = srunner_create(batch_request_suite());
  srunner_set_log(sr, "batch_request_suite.log");
  srunner_run_all(sr, CK_NORMAL);
  number_failed = srunner_ntests_failed(sr);
  srunner_free(sr);
  return(number_failed);
  }
