#include "license_pbs.h" /* See here for the software license */
#include "common_cmds.h"
#include <string>
#include <stdlib.h>
#include <stdio.h>
#include <pbs_ifl.h>
#include "u_hash_map_structs.h"
#include "test_common_cmds.h"
#include <pbs_constants.h>


extern int env_add_call;
extern int env_del_call;
extern int env_find_call;
extern int tc_num;

void add_env_value_to_string(std::string &val, const char *env_val_to_add);

START_TEST(test_add_env_value_to_string)
  {
  std::string bob = "bob=";
  
  add_env_value_to_string(bob, "tom,ray");
  fail_unless(!strcmp(bob.c_str(), "bob=tom\\,ray"), bob.c_str());
  
  bob = "bob=";
  add_env_value_to_string(bob, "tom\"ray");
  fail_unless(!strcmp(bob.c_str(), "bob=tom\\\"ray"));
  
  bob = "bob=";
  add_env_value_to_string(bob, "tom'ray");
  fail_unless(!strcmp(bob.c_str(), "bob=tom\\'ray"), bob.c_str());
  
  bob = "bob=";
  add_env_value_to_string(bob, "tom\\ray");
  fail_unless(!strcmp(bob.c_str(), "bob=tom\\\\ray"), bob.c_str());
  
  bob = "bob=";
  add_env_value_to_string(bob, "tom\nray");
  fail_unless(!strcmp(bob.c_str(), "bob=tom\\\nray"), bob.c_str());
  }
END_TEST

START_TEST(test_parse_env_line_nospace)
  {
  int rc = PBSE_NONE;
  char env_var[] = "ONE_VAR=THE_VAL";
  char *name = NULL, *val = NULL;
  rc = parse_env_line(env_var, &name, &val);
  fail_unless(rc == PBSE_NONE,"Invalid return from parse_env_line");
  fail_unless(name != NULL, "name should have a value");
  fail_unless(val != NULL, "val should have a value");
  fail_unless(strcmp("ONE_VAR", name) == 0, "[ONE_VAR] != [%s]", name);
  fail_unless(strcmp("THE_VAL", val) == 0, "[THE_VAL] != [%s]", val);
  }
END_TEST

START_TEST(test_parse_env_line_space)
  {
  int rc = PBSE_NONE;
  char env_var[] = "ONE_VAR=  THE_VAL";
  char *name = NULL, *val = NULL;
  rc = parse_env_line(env_var, &name, &val);
  fail_unless(rc == PBSE_NONE,"Invalid return from parse_env_line");
  fail_unless(name != NULL, "name should have a value");
  fail_unless(val != NULL, "val should have a value");
  fail_unless(strcmp("ONE_VAR", name) == 0, "[ONE_VAR] != [%s]", name);
  fail_unless(strcmp("THE_VAL", val) == 0, "[THE_VAL] != [%s]", val);

  char env_with_empty_val[] = "EMPTY_VAR=";
  name = val = NULL;
  rc = parse_env_line(env_with_empty_val, &name, &val);
  fail_unless(rc == PBSE_NONE,"Invalid return from parse_env_line2");
  fail_unless(name != NULL, "name should not be empty");
  fail_unless(val == NULL, "val should be empty");
  fail_unless(strcmp("EMPTY_VAR", name) == 0, "[EMPTY_VAR] != [%s]", name);
  }
END_TEST

START_TEST(test_parse_env_line_allspace)
  {
  int rc = PBSE_NONE;
  char env_var[] = "ONE_VAR=   ";
  char *name = NULL, *val = NULL;
  rc = parse_env_line(env_var, &name, &val);
  fail_unless(rc == PBSE_NONE,"Invalid return from parse_env_line");
  fail_unless(name != NULL, "name should have a value");
  fail_unless(val != NULL, "val should have a value");
  fail_unless(strcmp("ONE_VAR", name) == 0, "[ONE_VAR] != [%s]", name);
  fail_unless(strcmp("   ", val) == 0, "[   ] != [%s]", val);
  }
END_TEST

START_TEST(test_set_env_opts)
  {
  char *env_var[5];
  job_data_container env_hash;
  tc_num = 1;
  env_var[0] = (char *)calloc(1, 20);
  env_var[1] = (char *)calloc(1, 20);
  env_var[2] = (char *)calloc(1, 20);
  env_var[3] = (char *)calloc(1, 20);
  env_var[4] = NULL;
  strcpy(env_var[0], "ONE_VAR=ONE_VAL");
  strcpy(env_var[1], "TWO_VAR=TWO_VAL");
  strcpy(env_var[2], "THREE_VAR=   ");
  strcpy(env_var[3], "_=CMD");
  set_env_opts(&env_hash, env_var);
  fail_unless(env_add_call == 4, "Add should have been called 4 times (%d)", env_add_call);
  fail_unless(env_del_call == 0, "Del should have been called 1 times (%d)", env_del_call);
/*  fail_unless(env_find_call == 1, "Find should have been called 1 times (%d)", env_find_call); */
  }
END_TEST


START_TEST(test_parse_variable_list)
  {
  job_data_container dest_map;
  job_data_container src_map;
  job_data *item = NULL;
  int var_type = ENV_DATA;
  int op_type = SET;
  char list[] = "hi=there,help=me,me=";
  tc_num = 2;
  hash_add_or_exit(&src_map, "me", "notme", var_type);
  parse_variable_list(&dest_map, &src_map, var_type, op_type, list);
  hash_find(&dest_map, "hi", &item);
  fail_unless(item != NULL, "hi was not found in the map");
  hash_find(&dest_map, "help", &item);
  fail_unless(item != NULL, "help was not found in the map");
  hash_find(&dest_map, "me", &item);
  fail_unless(item != NULL, "me was not found in the map");
  fail_unless(strcmp(item->value.c_str(), "notme") == 0, "value of me was wrong \"notme\"!=\"%s\"", item->value.c_str());
  }
END_TEST

START_TEST(test_parse_variable_list_equalfirst)
  {
  job_data_container dest_map;
  job_data_container src_map;
  int       var_type = ENV_DATA;
  int       op_type = SET;
  char      list[] = "hi=there,=me";
  tc_num = 0;

  parse_variable_list(&dest_map, &src_map, var_type, op_type, list);
  }
END_TEST

START_TEST(test_parse_variable_list_noequal)
  {
  job_data_container dest_map;
  job_data_container src_map;
  int var_type = ENV_DATA;
  int op_type = SET;
  char list[] = "hi=there,helpme";
  tc_num = 0;
  parse_variable_list(&dest_map, &src_map, var_type, op_type, list);
  }
END_TEST

Suite *common_cmds_suite(void)
  {
  Suite *s = suite_create("common_cmds methods");
  TCase *tc_core = tcase_create("Core");
  tcase_add_test(tc_core, test_parse_env_line_nospace);
  tcase_add_test(tc_core, test_parse_env_line_space);
  tcase_add_test(tc_core, test_parse_env_line_allspace);
  tcase_add_test(tc_core, test_set_env_opts);
  tcase_add_test(tc_core, test_parse_variable_list);
  tcase_add_test(tc_core, test_parse_variable_list_equalfirst);
  tcase_add_test(tc_core, test_parse_variable_list_noequal);
  tcase_add_test(tc_core, test_add_env_value_to_string);
  suite_add_tcase(s, tc_core);
  return s;
  }

void rundebug()
  {
/*  */
/*   char *env_var[4]; */
/*   char *name = NULL, *val = NULL; */
/*   job_data *env_hash = NULL; */
/*   env_var[0] = (char *)calloc(1, 20); */
/*   env_var[1] = (char *)calloc(1, 20); */
/*   env_var[2] = (char *)calloc(1, 20); */
/*   env_var[3] = (char *)calloc(1, 20); */
/*   env_var[4] = NULL; */
/*   strcpy(env_var[0], "ONE_VAR=ONE_VAL"); */
/*   strcpy(env_var[1], "TWO_VAR=TWO_VAL"); */
/*   strcpy(env_var[2], "THREE_VAR=   "); */
/*   strcpy(env_var[2], "FOUR_VAR="); */
/*   strcpy(env_var[3], "_=CMD"); */
/*   set_env_opts(&env_hash, env_var); */
  char *env_var[5];
  job_data_container env_hash;
  tc_num = 1;
  env_var[0] = (char *)calloc(1, 20);
  env_var[1] = (char *)calloc(1, 20);
  env_var[2] = (char *)calloc(1, 20);
  env_var[3] = (char *)calloc(1, 20);
  env_var[4] = NULL;
  strcpy(env_var[0], "ONE_VAR=ONE_VAL");
  strcpy(env_var[1], "TWO_VAR=TWO_VAL");
  strcpy(env_var[2], "THREE_VAR=   ");
  strcpy(env_var[3], "_=CMD");
  set_env_opts(&env_hash, env_var);
  
/*  memmgr *mm;
  job_data *dest_map = NULL;
  job_data *src_map = NULL;
  job_data *item = NULL;
  int var_type = ENV_DATA;
  int op_type = SET;
  char list[] = "hi=there,help=me,me=";
  tc_num = 2;
  hash_add_or_exit(&mm, &src_map, "me", "notme", var_type);
  parse_variable_list(&mm, &dest_map, src_map, var_type, op_type, list);
  hash_find(dest_map, "hi", &item);
  hash_find(dest_map, "help", &item);
  hash_find(dest_map, "me", &item);
  */
  }

int main(void)
  {
  int number_failed = 0;
  SRunner *sr = NULL;
/*  rundebug(); */
  sr = srunner_create(common_cmds_suite());
  srunner_set_log(sr, "common_cmds_suite.log");
  srunner_run_all(sr, CK_NORMAL);
  number_failed = srunner_ntests_failed(sr);
  srunner_free(sr);
  return number_failed;
  }
