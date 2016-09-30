#include "license_pbs.h" /* See here for the software license */
#include <stdlib.h>
#include <stdio.h>
#include <getopt.h>
#include <iostream>
#include <string>
#include <sstream>
#include "trq_auth_daemon.h"
#include "test_trq_auth_daemon.h"
#include "pbs_error.h"

extern bool daemonize_server;

void parse_command_line(int, char**);

bool are_we_forking()

  {
  char *forking = getenv("CK_FORK");

  if ((forking != NULL) &&  
      (!strcasecmp(forking, "no")))
    return(false);

  return(true);
  }

void set_optind()

  {
  if (are_we_forking() == false)
    optind = 0;
  }

START_TEST(test_parse_command_line1)
  {
  char *argv[] = {strdup("trqauthd"), strdup("-D")};
  set_optind();

  daemonize_server = true;
  parse_command_line(2, argv);
  fail_unless(daemonize_server == false);
  }
END_TEST


START_TEST(test_parse_command_line2)
  {
  char *argv[] = {strdup("trqauthd"), strdup("-F")};
  set_optind();

  daemonize_server = true;
  parse_command_line(2, argv);
  fail_unless(daemonize_server == false);
  }
END_TEST


START_TEST(test_parse_command_line3)
  {
  char *argv[] = {strdup("trqauthd")};
  set_optind();

  daemonize_server = true;
  parse_command_line(1, argv);
  fail_unless(daemonize_server == true);
  }
END_TEST


START_TEST(test_two)
  {
  }
END_TEST

Suite *trq_auth_daemon_suite(void)
  {
  Suite *s = suite_create("trq_auth_daemon_suite methods");
  TCase *tc_core = tcase_create("test_parse_command_line1");
  tcase_add_test(tc_core, test_parse_command_line1);
  suite_add_tcase(s, tc_core);

  tc_core = tcase_create("test_parse_command_line2");
  tcase_add_test(tc_core, test_parse_command_line2);
  suite_add_tcase(s, tc_core);

  tc_core = tcase_create("test_parse_command_line3");
  tcase_add_test(tc_core, test_parse_command_line3);
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
  SRunner *sr = NULL;  rundebug();
  sr = srunner_create(trq_auth_daemon_suite());
  srunner_set_log(sr, "trq_auth_daemon_suite.log");
  srunner_run_all(sr, CK_NORMAL);
  number_failed = srunner_ntests_failed(sr);
  srunner_free(sr);
  return number_failed;
  }
