#include "license_pbs.h" /* See here for the software license */
#include "net_connect.h" /* pbs_net_t */
#include "pbsd_main.h"
#include "test_pbsd_main.h"
#include <stdlib.h>
#include <stdio.h>
#include <getopt.h>
#include <sys/types.h>
#include <unistd.h>
#include "pbs_error.h"
#include "mcom.h"
#include "completed_jobs_map.h"

void parse_command_line(int argc, char *argv[]);
extern bool auto_send_hierarchy;
extern int server_init_type;
extern completed_jobs_map_class completed_jobs_map;
extern bool TDoBackground;
extern bool LineBufferOutput;

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

START_TEST(test_parse_command_line_case1)
  {
  char *argv[] = {strdup("pbs_server"), strdup("-n")};
  set_optind();

  fail_unless(auto_send_hierarchy == true);
  parse_command_line(2, argv);
  fail_unless(auto_send_hierarchy == false, "-n failed");
  }
END_TEST

START_TEST(test_parse_command_line_case2)
  {
  char *argv[] = {strdup("pbs_server"), strdup("-tcreate")};
  set_optind();
  server_init_type = 1;

  parse_command_line(2, argv);
  fail_unless(server_init_type == 4, "-tcreate failed");
  }
END_TEST

START_TEST(test_parse_command_line_case3)
  {
  char *argv[] = {strdup("pbs_server")};
  set_optind();
  server_init_type = 1;

  parse_command_line(1, argv);
  fail_unless(server_init_type == 1, "empty failed");
  }
END_TEST


START_TEST(test_parse_command_line_case4)
  {
  char *argv[] = {strdup("pbs_server"), strdup("-tlala")};
  set_optind();

  parse_command_line(2, argv);
  }
END_TEST


START_TEST(test_parse_command_line_case5)
  {
  char *argv[] = {strdup("pbs_server"), strdup("-D")};
  set_optind();

  TDoBackground = true;
  LineBufferOutput = false;
  parse_command_line(2, argv);
  fail_unless(TDoBackground == false);
  fail_unless(LineBufferOutput);
  }
END_TEST


START_TEST(test_parse_command_line_case6)
  {
  char *argv[] = {strdup("pbs_server"), strdup("-F")};
  set_optind();

  TDoBackground = true;
  LineBufferOutput = false;
  parse_command_line(2, argv);
  fail_unless(TDoBackground == false);
  fail_unless(LineBufferOutput == false);
  }
END_TEST


START_TEST(test_parse_command_line_case7)
  {
  char *argv[] = {strdup("pbs_server")};
  set_optind();

  TDoBackground = true;
  LineBufferOutput = false;
  parse_command_line(1, argv);
  fail_unless(TDoBackground == true);
  fail_unless(LineBufferOutput == false);
  }
END_TEST


Suite *pbsd_main_suite(void)
  {
  Suite *s = suite_create("pbsd_main_suite methods");
  TCase *tc_core = tcase_create("test_parse_command_line_case1");
  tcase_add_test(tc_core, test_parse_command_line_case1);
  suite_add_tcase(s, tc_core);

  tc_core = tcase_create("test_parse_command_line_case2");
  tcase_add_test(tc_core, test_parse_command_line_case2);
  suite_add_tcase(s, tc_core);

  tc_core = tcase_create("test_parse_command_line_case3");
  tcase_add_test(tc_core, test_parse_command_line_case3);
  suite_add_tcase(s, tc_core);

  if (are_we_forking() == true)
    { 
    tc_core = tcase_create("test_parse_command_line_case4");
    tcase_add_exit_test(tc_core, test_parse_command_line_case4, 1);
    suite_add_tcase(s, tc_core);
    }

  tc_core = tcase_create("test_parse_command_line_case5");
  tcase_add_test(tc_core, test_parse_command_line_case5);
  suite_add_tcase(s, tc_core);

  tc_core = tcase_create("test_parse_command_line_case6");
  tcase_add_test(tc_core, test_parse_command_line_case6);
  suite_add_tcase(s, tc_core);

  tc_core = tcase_create("test_parse_command_line_case7");
  tcase_add_test(tc_core, test_parse_command_line_case7);
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
  sr = srunner_create(pbsd_main_suite());
  srunner_set_log(sr, "pbsd_main_suite.log");
  srunner_run_all(sr, CK_NORMAL);
  number_failed = srunner_ntests_failed(sr);
  srunner_free(sr);
  return number_failed;
  }
