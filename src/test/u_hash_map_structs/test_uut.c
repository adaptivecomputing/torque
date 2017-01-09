
#include "u_hash_map_structs.h"
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <check.h>

#include "pbs_error.h"
#include "pbs_ifl.h"
#include <pbs_constants.h>


extern int calloc_fail;

void initVars()
  {
  calloc_fail = 0;
  }

void callocVal(char **dest, const char *src)
  {
  *dest = (char *)calloc(1, strlen(src)+1);
  if (!dest)
    exit(1);
  strcpy(*dest, src);
  }


START_TEST(test_hash_priority_add_or_exit)
  {
  job_data_container map;

  hash_priority_add_or_exit(&map, "planet", "sel", CMDLINE_DATA);
  hash_priority_add_or_exit(&map, "planet", "nalthis", SCRIPT_DATA);

  job_data *stored;
  fail_unless(hash_find(&map, "planet", &stored) == 1);
  fail_unless(stored->value == "sel"); // command line is higher priority than script
  
  hash_priority_add_or_exit(&map, "planet", "roshar", ENV_DATA);
  fail_unless(hash_find(&map, "planet", &stored) == 1);
  fail_unless(stored->value == "roshar"); // environment data is higher priority than command line
  }
END_TEST


START_TEST(test_hash_add_item_new_count_clear)
  {
  int rc = FALSE;
  job_data_container *the_map = new job_data_container();
  char *name;
  char *value;
  initVars();
  callocVal(&name, "pos2");
  callocVal(&value, "val2");
  hash_add_item(the_map, name, value, ENV_DATA, SET);
  rc = hash_count(the_map);
  fail_unless(rc == 1, "There should be exactly one element in the map. %d found", rc);
  rc = hash_clear(the_map);
  fail_if(rc < 0, "There was an error clearing the hashmap");
  fail_unless(rc == 1, "The map consisted of one element, but %d elements were removed");
  delete the_map;
  }
END_TEST

START_TEST(test_hash_add_item_null)
  {
  int rc = 0;
  job_data_container the_map;
  job_data *tmp_item = NULL;
  char *name;
  initVars();
  callocVal(&name, "pos1");
  rc = hash_add_item(&the_map, name, NULL, ENV_DATA, SET);
  fail_unless(rc == TRUE, "The result should have been 0, but was %d", rc);
  rc = hash_count(&the_map);
  fail_unless(rc == 1, "The count in the map should be 1 but is %d", rc);

  hash_find(&the_map, "pos1", &tmp_item);
  fail_if(tmp_item == NULL, "The item was not found in the map");
  fail_unless(tmp_item->value.length() == 0, "The item should have been assigned an empty string as the value (was %s)", tmp_item->value.c_str());
  fail_unless(tmp_item->var_type == ENV_DATA, "var_type was invalid (%d != %d)", ENV_DATA, tmp_item->var_type);
  fail_unless(tmp_item->op_type == SET, "var_type was invalid (%d != %d)", SET, tmp_item->op_type);
  rc = hash_clear(&the_map);
  }
END_TEST

START_TEST(test_hash_add_item_add_find_add_find_del_cnt_del_find)
  {
  int rc = FALSE;
  job_data_container the_map;
  job_data *one_item = NULL;
  char *name;
  char *value;
  initVars();
  callocVal(&name, "pos1");
  callocVal(&value, "val1");
  rc = hash_add_item(&the_map, name, value, ENV_DATA, SET);
  fail_unless(rc == TRUE, "The result should have been 0, but was %d", rc);
  rc = hash_count(&the_map);
  fail_unless(rc == 1, "The count in the map should be 1 but is %d", rc);

  rc = hash_find(&the_map, "pos3", &one_item);
  fail_unless(one_item == NULL, "item found for non-existant key");

  rc = hash_find(&the_map, "pos1", &one_item);
  fail_if(one_item == NULL, "Item that was expected, not found in map");
  fail_unless(strcmp(one_item->value.c_str(), "val1") == 0, "invalid value in response item (val1 != %s)", one_item->value.c_str());

  callocVal(&name, "pos2");
  callocVal(&value, "val2");
  rc = hash_add_item(&the_map, name, value, ENV_DATA, SET);
  fail_unless(rc == TRUE, "The result should have been 0 but is %d", rc);

  rc = hash_count(&the_map);
  fail_unless(rc == 2, "The count in the map should be 2 but is %d", rc);

  rc = hash_del_item(&the_map, "pos3");
  fail_unless(rc == 0, "Nonexistant item found in hashmap");

  rc = hash_del_item(&the_map, "pos1");
  fail_unless(rc == 1, "Item that should be in the hashmap not found to delete");
  rc = hash_count(&the_map);
  fail_unless(rc == 1, "The count in the map should be 1 but is %d", rc);

  rc = hash_del_item(&the_map, "pos2");

  rc = hash_clear(&the_map);
  }
END_TEST

START_TEST(test_hash_print)
  {
  int rc = FALSE;
  job_data_container the_map;
  char *name;
  char *value;
  initVars();
  callocVal(&name, "pos1");
  callocVal(&value, "val1");
  rc = hash_add_item(&the_map, name, value, ENV_DATA, SET);
  callocVal(&name, "pos2");
  callocVal(&value, "val2");
  rc = hash_add_item(&the_map, name, value, ENV_DATA, SET);
  rc = hash_print(&the_map);
  fail_unless(rc == 2, "There should be 2 elements in the hash %d found", rc);
  rc = hash_clear(&the_map);
  }
END_TEST
/* Testing this involves forcing this to exit - causing a failure. Don't test.
START_TEST(test_add_or_exit)
  {
  job_data *the_map = NULL;
  memmgr *mm = NULL;
  const char *name = "simple_val";
  const char *value = "should fail";
  int var_type = 4;
  initVars();
  calloc_fail = 1;
  hash_add_or_exit(&mm, &the_map, name, value, var_type);
  }
END_TEST */

START_TEST(test_hash_add_hash)
  {
  int rc = FALSE;
  job_data_container map1;
  job_data_container map2;
  job_data *tmp_item = NULL;
  char *name;
  char *value;
  initVars();
  callocVal(&name, "name1");
  callocVal(&value, "value1");
  hash_add_item(&map1, name, value, ENV_DATA, SET);
  callocVal(&name, "name2");
  callocVal(&value, "value2");
  hash_add_item(&map1, name, value, ENV_DATA, SET);

  callocVal(&name, "name3");
  callocVal(&value, "value3");
  hash_add_item(&map2, name, value, ENV_DATA, SET);
  callocVal(&name, "name4");
  callocVal(&value, "value4");
  hash_add_item(&map2, name, value, ENV_DATA, SET);
  callocVal(&value,"othervalue4");
  fail_unless(hash_add_item(&map2, name, value, ENV_DATA, SET) == true);
  rc = hash_count(&map1);
  fail_unless(rc == 2, "2 values should exist in map, but only %d does", rc);
  rc = hash_count(&map2);
  fail_unless(rc == 2, "2 values should exist in map, but only %d does", rc);

  rc = hash_add_hash(&map1, &map2, 0);
  fail_unless(rc == 2, "2 values should have been added, but only %d was", rc);
  hash_clear(&map2);
  callocVal(&name, "name4");
  callocVal(&value, "value6");
  hash_add_item(&map2, name, value, ENV_DATA, SET);
  rc = hash_add_hash(&map1, &map2, 1);
  fail_unless(rc == 1, "1 value should have been added/updated, %d was", rc);
  rc = hash_count(&map1);
  fail_unless(rc == 4, "4 items should exist in the hash_map, %d do", rc);
  fail_unless(hash_find(&map1, "name4", &tmp_item) == TRUE, "hash item name4 not found");
  fail_unless(strcmp(tmp_item->value.c_str(), "value6") == 0, "value for name4 should have been value6, but was %s", tmp_item->value.c_str());
  }
END_TEST

Suite *u_hash_map_structs_suite(void)
  {
  Suite *s = suite_create("u_hash_map_structs methods");
  TCase *tc_core = tcase_create("u_hash_map_structs");
  tcase_add_test(tc_core, test_hash_add_item_new_count_clear);
  tcase_add_test(tc_core, test_hash_add_item_null);
  tcase_add_test(tc_core, test_hash_add_item_add_find_add_find_del_cnt_del_find);
  tcase_add_test(tc_core, test_hash_print);
  /*tcase_add_exit_test(tc_core, test_add_or_exit, 1);*/
  tcase_add_test(tc_core, test_hash_add_hash);
  tcase_add_test(tc_core, test_hash_priority_add_or_exit);

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
/*  rundebug(); */
  sr = srunner_create(u_hash_map_structs_suite());
  srunner_set_log(sr, "u_hash_map_structs_suite.log");
  srunner_run_all(sr, CK_NORMAL);
  number_failed = srunner_ntests_failed(sr);
  srunner_free(sr);
  return number_failed;
  }
