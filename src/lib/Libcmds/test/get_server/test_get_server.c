#include "license_pbs.h" /* See here for the software license */
#include "lib_cmds.h"
#include "test_get_server.h"
#include <stdlib.h>
#include <stdio.h>


#include "pbs_error.h"

START_TEST(test_one_all)
  {
  char job_id[10];
  char job_id_out[64];
  int  job_id_out_size = 64;
  char server_out[64];
  int  server_out_size = 64;
  int  rc;
  char error_msg[256];

  strcpy(job_id, "all");

  rc = get_server(job_id, job_id_out, job_id_out_size, server_out, server_out_size);

  sprintf(error_msg, "rc = %d", rc);
  fail_unless(rc == PBSE_NONE, error_msg);

  }
END_TEST

START_TEST(test_two)
  {


  }
END_TEST

Suite *get_server_suite(void)
  {
  Suite *s = suite_create("get_server_suite methods");
  TCase *tc_core = tcase_create("test_one_all");
  tcase_add_test(tc_core, test_one_all);
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
  sr = srunner_create(get_server_suite());
  srunner_set_log(sr, "get_server_suite.log");
  srunner_run_all(sr, CK_NORMAL);
  number_failed = srunner_ntests_failed(sr);
  srunner_free(sr);
  return number_failed;
  }
