#include "license_pbs.h" /* See here for the software license */
#include "mom_process_request.h"
#include "test_mom_process_request.h"
#include <stdlib.h>
#include <stdio.h>


#include "pbs_error.h"

void *mom_process_request(void *sock_num);
extern char log_buffer[];

START_TEST(test_mom_process_request)
  {
  int sock = 1;

  mom_process_request(&sock);
  fail_unless(strstr(log_buffer, "connection not privileged") != NULL);
  }
END_TEST

START_TEST(test_mom_process_request_bad_host_err)
  {
  int sock = 999;

  mom_process_request(&sock);
  fail_unless(strncmp(log_buffer, "Access from host not allowed, or unknown host", strlen("Access from host not allowed, or unknown host")) == 0,
          "Error message was not constructed as expected: %s", log_buffer);
  }
END_TEST

Suite *mom_process_request_suite(void)
  {
  Suite *s = suite_create("mom_process_request_suite methods");
  TCase *tc_core = tcase_create("test_mom_process_request");
  tcase_add_test(tc_core, test_mom_process_request);
  suite_add_tcase(s, tc_core);

  tc_core = tcase_create("test_mom_process_request_bad_host_err");
  tcase_add_test(tc_core, test_mom_process_request_bad_host_err);
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
  sr = srunner_create(mom_process_request_suite());
  srunner_set_log(sr, "mom_process_request_suite.log");
  srunner_run_all(sr, CK_NORMAL);
  number_failed = srunner_ntests_failed(sr);
  srunner_free(sr);
  return number_failed;
  }
