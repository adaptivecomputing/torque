#include <stdio.h>
#include <stdlib.h>
#include <check.h>
#include <errno.h>

#include "hash_map.h"

START_TEST(get_hash_map_test)
  {
  hash_map *hm = get_hash_map(2);
  fail_unless(hm != NULL, "Couldn't initialize hash map");
  }
END_TEST




START_TEST(free_hash_map_test)
  {
  hash_map *hm = get_hash_map(2);
  fail_unless(hm != NULL, "Couldn't initialize hash map");
  free_hash_map(hm);
  }
END_TEST




START_TEST(add_to_hash_map_test)
  {
  int      *obj1 = (int *)1;
  int      *obj2 = (int *)2;
  char     *key1 = "tom1";
  char     *key2 = "tom2";
  hash_map *hm = get_hash_map(2);

  fail_unless(add_to_hash_map(hm, obj1, key1) == ALREADY_IN_HASH_MAP, "re-added an object to the hash map");
  fail_unless(add_to_hash_map(hm, obj1, key2) == ENOMEM, "didn't handle OOM condition");
  fail_unless(add_to_hash_map(hm, obj2, key2) == 0, "failed to add a valid object");
  }
END_TEST




START_TEST(remove_from_hash_map_test)
  {
  char     *key1 = "tom1";
  char     *key2 = "tom2";
  hash_map *hm   = get_hash_map(2);

  fail_unless(remove_from_hash_map(hm, key2) == KEY_NOT_FOUND, "removed non-existent object?");
  fail_unless(remove_from_hash_map(hm, key1) == 0, "didn't remove object successfully");
  }
END_TEST




START_TEST(get_remove_from_hash_map_test)
  {
  char     *key1 = "tom1";
  char     *key2 = "tom2";
  hash_map *hm   = get_hash_map(2);
  
  fail_unless(get_remove_from_hash_map(hm, key2) == NULL, "removed non-existent object?");
  fail_unless(get_remove_from_hash_map(hm, key1) != NULL, "didn't remove object successfully");
  }
END_TEST




START_TEST(get_from_hash_map_test)
  {
  char     *key1 = "tom1";
  char     *key2 = "tom2";
  hash_map *hm   = get_hash_map(2);
  
  fail_unless(get_from_hash_map(hm, key2) == NULL, "got non-existent object?");
  fail_unless(get_from_hash_map(hm, key1) != NULL, "didn't get object successfully");
  }
END_TEST




START_TEST(next_from_hash_map_test)
  {
  hash_map *hm   = get_hash_map(-1);
  int       iter = -1;

  fail_unless(next_from_hash_map(hm, &iter) == NULL, "iterated from empty hash map?");
  }
END_TEST




Suite *u_hash_map_suite(void)
  {
  Suite *s = suite_create("u_hash_map test suite methods");
  TCase *tc_core = tcase_create("get_hash_map_test");
  tcase_add_test(tc_core, get_hash_map_test);
  suite_add_tcase(s, tc_core);
  
  tc_core = tcase_create("free_hash_map_test");
  tcase_add_test(tc_core, free_hash_map_test);
  suite_add_tcase(s, tc_core);

  tc_core = tcase_create("add_to_hash_map_test");
  tcase_add_test(tc_core, add_to_hash_map_test);
  suite_add_tcase(s, tc_core);

  tc_core = tcase_create("remove_from_hash_map_test");
  tcase_add_test(tc_core, remove_from_hash_map_test);
  suite_add_tcase(s, tc_core);

  tc_core = tcase_create("get_remove_from_hash_map_test");
  tcase_add_test(tc_core, get_remove_from_hash_map_test);
  suite_add_tcase(s, tc_core);

  tc_core = tcase_create("get_from_hash_map_test");
  tcase_add_test(tc_core, get_from_hash_map_test);
  suite_add_tcase(s, tc_core);

  tc_core = tcase_create("next_from_hash_map_test");
  tcase_add_test(tc_core, next_from_hash_map_test);
  suite_add_tcase(s, tc_core);
  
  return(s);
  }

void rundebug()
  {
  }

int main(void)
  {
  int number_failed = 0;
  SRunner *sr = NULL;
  rundebug();
  sr = srunner_create(u_hash_map_suite());
  srunner_set_log(sr, "u_hash_map_suite.log");
  srunner_run_all(sr, CK_NORMAL);
  number_failed = srunner_ntests_failed(sr);
  srunner_free(sr);
  return(number_failed);
  }
