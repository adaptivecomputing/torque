
#include "u_hash_map_structs_ct.h"
#include "u_hash_map_structs.h"
#include <stdlib.h>
#include <stdio.h>
#include "pbs_error.h"
#include "pbs_ifl.h"
#include "u_memmgr.h"

extern calloc_fail;

void initVars()
  {
  calloc_fail = 0;
  }

void callocVal(char **dest, char *src)
  {
  *dest = (char *)calloc(1, strlen(src)+1);
  if (!dest)
    exit(1);
  strcpy(*dest, src);
  }

START_TEST(test_hash_add_item_new_count_clear)
  {
  initVars();
  int rc = FALSE;
  job_data *the_map = NULL;
  char *name;
  char *value;
  memmgr *mm = NULL;
  memmgr_init(&mm, 0);
  callocVal(&name, "pos2");
  callocVal(&value, "val2");
  hash_add_item(&mm, &the_map, name, value, ENV_DATA, SET);
  rc = hash_count(the_map);
  fail_unless(rc == 1, "There should be exactly one element in the map. %d found", rc);
  rc = hash_clear(&mm, &the_map);
  fail_if(rc < 0, "There was an error clearing the hashmap");
  fail_unless(rc == 1, "The map consisted of one element, but %d elements were removed");
  memmgr_destroy(&mm);
  }
END_TEST

START_TEST(test_hash_add_item_null)
  {
  initVars();
  int rc = 0;
  job_data *the_map = NULL;
  job_data *tmp_item = NULL;
  memmgr *mm = NULL;
  memmgr_init(&mm, 0);
  char *name;
  callocVal(&name, "pos1");
  rc = hash_add_item(&mm, &the_map, name, NULL, ENV_DATA, SET);
  fail_unless(rc == TRUE, "The result should have been 0, but was %d", rc);
  rc = hash_count(the_map);
  fail_unless(rc == 1, "The count in the map should be 1 but is %d", rc);

  hash_find(the_map, "pos1", &tmp_item); 
  fail_if(the_map == NULL, "The item was not found in the map");
  fail_if(the_map->value == NULL, "The item should have been assigned an empty string as the value (was null)");
  fail_unless(strlen(the_map->value) == 0, "The item should have been assigned an empty string as the value (was %s)", the_map->value);
  fail_unless(tmp_item->var_type == ENV_DATA, "var_type was invalid (%d != %d)", ENV_DATA, tmp_item->var_type);
  fail_unless(tmp_item->op_type == SET, "var_type was invalid (%d != %d)", SET, tmp_item->op_type);
  rc = hash_clear(&mm, &the_map);
  memmgr_destroy(&mm);
  }
END_TEST

START_TEST(test_hash_add_item_add_find_add_find_del_cnt_del_find)
  {
  initVars();
  int rc = FALSE;
  job_data *the_map = NULL;
  job_data *one_item = NULL;
  memmgr *mm = NULL;
  memmgr_init(&mm, 0);
  char *name;
  char *value;
  callocVal(&name, "pos1");
  callocVal(&value, "val1");
  rc = hash_add_item(&mm, &the_map, name, value, ENV_DATA, SET);
  fail_unless(rc == TRUE, "The result should have been 0, but was %d", rc);
  rc = hash_count(the_map);
  fail_unless(rc == 1, "The count in the map should be 1 but is %d", rc);

  rc = hash_find(the_map, "pos3", &one_item);
  fail_unless(one_item == NULL, "item found for non-existant key");

  rc = hash_find(the_map, "pos1", &one_item);
  fail_if(one_item == NULL, "Item that was expected, not found in map");
  fail_if(one_item->name == NULL, "Item->name is NULL");
  fail_if(one_item->value == NULL, "Item->value is NULL");
  fail_unless(strcmp(one_item->value, "val1") == 0, "invalid value in response item (val1 != %s)", one_item->value);

  callocVal(&name, "pos2");
  callocVal(&value, "val2");
  rc = hash_add_item(&mm, &the_map, name, value, ENV_DATA, SET);
  fail_unless(rc == TRUE, "The result should have been 0 but is %d", rc);

  rc = hash_count(the_map);
  fail_unless(rc == 2, "The count in the map should be 2 but is %d", rc);

  rc = hash_del_item(&mm, &the_map, "pos3");
  fail_unless(rc == 0, "Nonexistant item found in hashmap");

  rc = hash_del_item(&mm, &the_map, "pos1");
  fail_unless(rc == 1, "Item that should be in the hashmap not found to delete");
  rc = hash_count(the_map);
  fail_unless(rc == 1, "The count in the map should be 1 but is %d", rc);

  rc = hash_del_item(&mm, &the_map, "pos2");
  fail_unless(the_map == NULL, "Deleting the last item should result in a null hash_map (but didn't)");

  rc = hash_clear(&mm, &the_map);
  memmgr_destroy(&mm);
  }
END_TEST

START_TEST(test_hash_print)
  {
  initVars();
  int rc = FALSE;
  job_data *the_map = NULL;
  memmgr *mm = NULL;
  memmgr_init(&mm, 0);
  char *name;
  char *value;
  callocVal(&name, "pos1");
  callocVal(&value, "val1");
  rc = hash_add_item(&mm, &the_map, name, value, ENV_DATA, SET);
  callocVal(&name, "pos2");
  callocVal(&value, "val2");
  rc = hash_add_item(&mm, &the_map, name, value, ENV_DATA, SET);
  rc = hash_print(the_map);
  fail_unless(rc == 2, "There should be 2 elements in the hash %d found", rc);
  rc = hash_clear(&mm, &the_map);
  memmgr_destroy(&mm);
  }
END_TEST

START_TEST(test_add_or_exit)
  {
  initVars();
  int rc = FALSE;
  job_data *the_map = NULL;
  memmgr *mm = NULL;
  char *name = "simple_val";
  char value[] = "should fail";
  int var_type = 4;
  calloc_fail = 1;
  hash_add_or_exit(&mm, &the_map, name, value, var_type);
  }
END_TEST

START_TEST(test_hash_add_hash)
  {
  initVars();
  int rc = FALSE;
  job_data *map1 = NULL;
  job_data *map2 = NULL;
  job_data *tmp_item = NULL;
  memmgr *mm = NULL;
  memmgr_init(&mm, 0);
  char *name;
  char *value;
  callocVal(&name, "name1");
  callocVal(&value, "value1");
  hash_add_item(&mm, &map1, name, value, ENV_DATA, SET);
  callocVal(&name, "name2");
  callocVal(&value, "value2");
  hash_add_item(&mm, &map1, name, value, ENV_DATA, SET);

  callocVal(&name, "name3");
  callocVal(&value, "value3");
  hash_add_item(&mm, &map2, name, value, ENV_DATA, SET);
  callocVal(&name, "name4");
  callocVal(&value, "value4");
  hash_add_item(&mm, &map2, name, value, ENV_DATA, SET);
  rc = hash_count(map1);
  fail_unless(rc == 2, "2 values should exist in map, but only %d does", rc);
  rc = hash_count(map2);
  fail_unless(rc == 2, "2 values should exist in map, but only %d does", rc);

  rc = hash_add_hash(&mm, &map1, map2, 0);
  fail_unless(rc == 2, "2 values should have been added, but only %d was", rc);
  hash_clear(&mm, &map2);
  callocVal(&name, "name4");
  callocVal(&value, "value6");
  hash_add_item(&mm, &map2, name, value, ENV_DATA, SET);
  rc = hash_add_hash(&mm, &map1, map2, 1);
  fail_unless(rc == 1, "1 value should have been added/updated, %d was", rc);
  rc = hash_count(map1);
  fail_unless(rc == 4, "4 items should exist in the hash_map, %d do", rc);
  fail_unless(hash_find(map1, "name4", &tmp_item) == TRUE, "hash item name4 not found");
  fail_unless(strcmp(tmp_item->value, "value6") == 0, "value for name4 should have been value6, but was %s", tmp_item->value);
  memmgr_destroy(&mm);
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
  tcase_add_exit_test(tc_core, test_add_or_exit, 1);
  tcase_add_test(tc_core, test_hash_add_hash);

  suite_add_tcase(s, tc_core);
  return s;
  }

void rundebug()
  {
  int rc = FALSE;
  job_data *the_map = NULL;
  memmgr *mm = NULL;
  memmgr_init(&mm, 0);
  char *name = "simple_val";
  char value[] = "should fail";
  int var_type = 4;
  calloc_fail = 1;
  hash_add_or_exit(&mm, &the_map, name, value, var_type);
  memmgr_destroy(&mm);
  }

int main(void)
  {
  int number_failed = 0;
/*  rundebug(); */
  SRunner *sr = srunner_create(u_hash_map_structs_suite());
  srunner_set_log(sr, "u_hash_map_structs_suite.log");
  srunner_run_all(sr, CK_NORMAL);
  number_failed = srunner_ntests_failed(sr);
  srunner_free(sr);
  return number_failed;
  }
