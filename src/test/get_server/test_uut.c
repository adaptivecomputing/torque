#include "license_pbs.h" /* See here for the software license */
#include "test_get_server.h"
#include <stdlib.h>
#include <stdio.h>


#include "pbs_error.h"

int get_server(const char *job_id_in, char *job_id_out, int job_id_out_size, char *server_out, int server_out_size);


START_TEST(test_get_server)
  {
  char jid[1024];
  char server[1024];

  memset(server, 0, sizeof(server));

  fail_unless(get_server("1.napali", jid, sizeof(jid), server, sizeof(server)) == PBSE_NONE);
  // Specifying a job id with server shouldn't set the server or HA gets hosed
  fail_unless(strlen(server) == 0);
  // If they specify a full job id take exactly what they specified
  fail_unless(!strcmp(jid, "1.napali"));
  
  fail_unless(get_server("1@napali", jid, sizeof(jid), server, sizeof(server)) == PBSE_NONE);
  // @ should get the server set
  fail_unless(!strcmp(server, "napali"));
  // default server is set to return "napali"
  fail_unless(!strcmp(jid, "1.napali"));
  
  fail_unless(get_server("1@waimea", jid, sizeof(jid), server, sizeof(server)) == PBSE_NONE);
  // @ should get the server set
  fail_unless(!strcmp(server, "waimea"));
  // default server is set to return "napali"
  fail_unless(!strcmp(jid, "1.napali"));
  
  fail_unless(get_server("1.napali.ac", jid, sizeof(jid), server, sizeof(server)) == PBSE_NONE);
  // @ should get the server set
  fail_unless(strlen(server) == 0);
  // If they specify a full job id take exactly what they specified
  fail_unless(!strcmp(jid, "1.napali.ac"));
  }
END_TEST

START_TEST(test_two)
  {


  }
END_TEST

Suite *get_server_suite(void)
  {
  Suite *s = suite_create("get_server_suite methods");
  TCase *tc_core = tcase_create("test_get_server");
  tcase_add_test(tc_core, test_get_server);
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
