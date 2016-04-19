#include "license_pbs.h" /* See here for the software license */
#include <pbs_config.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "pbs_error.h"
#include "pbs_job.h"
#include "batch_request.h"
#include "test_requests.h"

void string_replchar(const char*, char, char);
void req_delete_reservation(batch_request *request);

extern std::list<job *>    alljobs_list;
extern int destroy_alps_rc;
extern int killed_a_job;
extern int reply_acked;
extern int req_rejected;

START_TEST(test_string_replchar)
  {
  char *mystring;

  mystring = (char *)calloc(1024, sizeof(char));
  fail_unless(NULL != mystring);

  // NULL should be handled without error
  string_replchar(NULL, 'a', 'b');

  // string empty
  string_replchar(mystring, 'a', 'b');
  fail_unless('\0' == *mystring);

  // simple replacement
  snprintf(mystring, 1024, "abcdefga");
  string_replchar(mystring, 'a', 'z');
  fail_unless(strcmp("zbcdefgz", mystring) == 0);

  // no replacement
  string_replchar(mystring, 'a', 'z');
  fail_unless(strcmp("zbcdefgz", mystring) == 0);
  }
END_TEST

START_TEST(test_req_delete_reservation)
  {
  batch_request  preq;
  job            pjob;
  const char    *res_id = "123";

  memset(&preq, 0, sizeof(preq));
  memset(&pjob, 0, sizeof(pjob));

  preq.rq_extend = strdup(res_id);

  killed_a_job = 0;
  reply_acked = 0;
  req_rejected = 0;
  
  // With destroy_alps_rc set to PBSE_NONE, we are successful and shouldn't try to kill
  // the job
  destroy_alps_rc = PBSE_NONE;
  req_delete_reservation(&preq);
  fail_unless(killed_a_job == 0);
  fail_unless(req_rejected == 0);
  fail_unless(reply_acked == 1);
  
  // Create a failure, make sure we attempt to kill the job, and reject the request
  destroy_alps_rc = -1;
  req_delete_reservation(&preq);
  fail_unless(killed_a_job == 1);
  fail_unless(req_rejected == 1);
  // Shouldn't change
  fail_unless(reply_acked == 1);

  }
END_TEST

Suite *requests_suite(void)
  {
  Suite *s = suite_create("requests_suite methods");
  TCase *tc_core = tcase_create("test_string_replchar");
  tcase_add_test(tc_core, test_string_replchar);
  suite_add_tcase(s, tc_core);

  tc_core = tcase_create("test_req_delete_reservation");
  tcase_add_test(tc_core, test_req_delete_reservation);
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
  sr = srunner_create(requests_suite());
  srunner_set_log(sr, "requests_suite.log");
  srunner_run_all(sr, CK_NORMAL);
  number_failed = srunner_ntests_failed(sr);
  srunner_free(sr);
  return number_failed;
  }
