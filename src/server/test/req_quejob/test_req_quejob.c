#include "license_pbs.h" /* See here for the software license */
#include "req_quejob.h"
#include "test_req_quejob.h"
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include "pbs_error.h"

extern all_jobs newjobs;

extern int req_jobcredential(struct batch_request *preq);

extern char *path_jobs;
char path_to_jobs[500];


START_TEST(test_one)
  {
  struct batch_request req;
  job j;
  char cmd[500];

  memset(&j,0,sizeof(j));
  memset(&req,0,sizeof(req));

  fail_unless(req_jobcredential(&req) == PBSE_IVALREQ);

  strcpy(j.ji_qs.ji_jobid,"SomeJob");
  strcpy(j.ji_qs.ji_fileprefix,"prefix");
  newjobs.lock();
  newjobs.insert(&j,j.ji_qs.ji_jobid);
  newjobs.unlock();

  fail_unless(req_jobcredential(&req) == PBSE_NONE);

  memset(&req,0,sizeof(req));

  strcpy(req.rq_ind.rq_jobfile.rq_jobid,"NotThisJob");
  fail_unless(req_jobscript(&req) == PBSE_IVALREQ);

  path_jobs = getcwd(path_to_jobs,sizeof(path_to_jobs));
  strcat(path_jobs,"/");
  sprintf(cmd,"rm -f %s*.SC",path_jobs);
  system(cmd);
  strcpy(req.rq_ind.rq_jobfile.rq_jobid,"1.napali");
  fail_unless(req_jobscript(&req) == PBSE_NONE);
  system(cmd);
  }
END_TEST

START_TEST(test_two)
  {


  }
END_TEST

Suite *req_quejob_suite(void)
  {
  Suite *s = suite_create("req_quejob_suite methods");
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
  sr = srunner_create(req_quejob_suite());
  srunner_set_log(sr, "req_quejob_suite.log");
  srunner_run_all(sr, CK_NORMAL);
  number_failed = srunner_ntests_failed(sr);
  srunner_free(sr);
  return number_failed;
  }
