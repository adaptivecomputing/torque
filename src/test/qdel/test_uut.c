#include "license_pbs.h" /* See here for the software license */
#include "test_qdel.h"
#include <stdlib.h>
#include <stdio.h>
#include <vector>

#include "pbs_error.h"

extern int cnt2server_conf_cnt;
extern std::vector<long> cnt2server_conf_arg;
extern char * get_trq_param_ret;

START_TEST(test_clientretry_disabled)
  {
  char arg0[] = "qdel";
  char arg1[] = "0";
  char * argv[] = {arg0, arg1};
  int argc = sizeof(argv) / sizeof(argv[0]);
  char *envp[] = {NULL};
  qdel_main(argc, argv, envp);
  fail_unless(cnt2server_conf_cnt == 0);
  }
END_TEST

START_TEST(test_clientretry_cmdline)
  {
  char arg0[] = "qdel";
  char arg1[] = "-b";
  char arg2[] = "100";
  char arg3[] = "0";
  char * argv[] = {arg0, arg1, arg2, arg3};
  int argc = sizeof(argv) / sizeof(argv[0]);
  char *envp[] = {NULL};
  qdel_main(argc, argv, envp);
  fail_unless(cnt2server_conf_cnt == 1);
  fail_unless(cnt2server_conf_arg[0] == 100);
  }
END_TEST

START_TEST(test_clientretry_conf_file)
  {
  char arg0[] = "qdel";
  char arg1[] = "0";
  char * argv[] = {arg0, arg1};
  int argc = sizeof(argv) / sizeof(argv[0]);
  char *envp[] = {NULL};

  char clientretry_val[] = "200";
  get_trq_param_ret = clientretry_val;
  qdel_main(argc, argv, envp);
  fail_unless(cnt2server_conf_cnt == 1);
  fail_unless(cnt2server_conf_arg[0] == 200);

  get_trq_param_ret = NULL;
  }
END_TEST

START_TEST(test_clientretry_env)
  {
  char arg0[] = "qdel";
  char arg1[] = "0";
  char * argv[] = {arg0, arg1};
  int argc = sizeof(argv) / sizeof(argv[0]);

  char env0[] = "PBS_CLIENTRETRY=300";
  char *envp[] = {env0, NULL};
  qdel_main(argc, argv, envp);
  fail_unless(cnt2server_conf_cnt == 1);
  fail_unless(cnt2server_conf_arg[0] == 300);
  }
END_TEST

START_TEST(test_two)
  {
  }
END_TEST

Suite *qdel_suite(void)
  {
  Suite *s = suite_create("qdel_suite methods");
  TCase *tc_core = tcase_create("test_clientretry");
  tcase_add_test(tc_core, test_clientretry_disabled);
  tcase_add_test(tc_core, test_clientretry_cmdline);
  tcase_add_test(tc_core, test_clientretry_conf_file);
  tcase_add_test(tc_core, test_clientretry_env);
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
  sr = srunner_create(qdel_suite());
  srunner_set_log(sr, "qdel_suite.log");
  srunner_run_all(sr, CK_NORMAL);
  number_failed = srunner_ntests_failed(sr);
  srunner_free(sr);
  return number_failed;
  }
