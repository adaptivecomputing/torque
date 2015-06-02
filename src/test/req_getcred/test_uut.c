#include "license_pbs.h" /* See here for the software license */
#include "req_getcred.h"
#include "test_req_getcred.h"
#include <stdlib.h>
#include <stdio.h>
#include "pbs_error.h"

extern struct connection svr_conn[PBS_NET_MAX_CONNECTIONS];
extern int rejected;
extern int acked;


START_TEST(test_one)
  {
  batch_request req;


  memset(&req,0,sizeof(batch_request));
  memset(svr_conn,0,sizeof(svr_conn));
  req.rq_ind.rq_authen.rq_port = 42;
  strcpy(req.rq_user,"Tron");
  strcpy(req.rq_user,"Flynns");
  fail_unless(req_authenuser(&req) == PBSE_CLIENT_CONN_NOT_FOUND);
  req.rq_conn = 4;
  fail_unless(rejected);
  rejected = FALSE;
  acked = FALSE;
  svr_conn[4].cn_addr = 3577385;
  svr_conn[4].cn_port = 62;
  svr_conn[6].cn_addr = 3577385;
  svr_conn[6].cn_port = 42;
  svr_conn[5].cn_addr = 2083577385;
  svr_conn[5].cn_port = 42;
  svr_conn[7].cn_addr = 2265079;
  svr_conn[7].cn_port = 42;

  fail_unless(req_authenuser(&req) == PBSE_NONE);
  fail_unless(rejected == FALSE);
  fail_unless(acked);
  fail_unless(svr_conn[6].cn_authen == PBS_NET_CONN_AUTHENTICATED);
  fail_unless(svr_conn[5].cn_authen != PBS_NET_CONN_AUTHENTICATED);
  fail_unless(svr_conn[7].cn_authen != PBS_NET_CONN_AUTHENTICATED);

  }
END_TEST

START_TEST(test_two)
  {


  }
END_TEST

Suite *req_getcred_suite(void)
  {
  Suite *s = suite_create("req_getcred_suite methods");
  TCase *tc_core = tcase_create("test_one");
  tcase_add_test(tc_core, test_one);
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
  sr = srunner_create(req_getcred_suite());
  srunner_set_log(sr, "req_getcred_suite.log");
  srunner_run_all(sr, CK_NORMAL);
  number_failed = srunner_ntests_failed(sr);
  srunner_free(sr);
  return number_failed;
  }
