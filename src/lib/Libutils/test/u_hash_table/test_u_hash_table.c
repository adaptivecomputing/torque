#include "license_pbs.h" /* See here for the software license */
#include "lib_utils.h"
#include "test_u_hash_table.h"
#include <stdlib.h>
#include <stdio.h>


#include "pbs_error.h"
#include "hash_table.h"
#define DEFAULT_HASH_SIZE 101

char  buf[4096];
char *keys[] = {"a", "b", "c", "d", "e", "f", "g", "h", "i", "j", "k", "l", "m"};
char *more_keys[] = {"nostradamus", "slartibartfast", "zapphod beeblebrox", "arthur dent", "ford prefect", "trillian", "antidisestablishmentarianism", "highway", "horcamer", "a vowel", "a towel", "martin the robot", "mostly harmless"};
int   num_keys = 13;
int   vals[] = { 0,   1,   2,   3,   4,   5,   6,   7,   8,   9,  10,   11,  12};

START_TEST(create_insert_get)
  {
  hash_table_t *ht = create_hash(DEFAULT_HASH_SIZE);
  int           i;
  int           val;

  for (i = 0; i < num_keys; i++)
    fail_unless(add_hash(ht, vals[i], keys[i]) == PBSE_NONE, "add_hash failed");

  for (i = 0; i < num_keys; i++)
    {
    val = get_value_hash(ht, keys[i]);
    fail_unless(val == vals[i], "values don't match");

    val = get_value_hash(ht, more_keys[i]);
    fail_unless(val == -1, "supplied a value for a non-existent key");
    }

  free_hash(ht);
  }
END_TEST

START_TEST(rehash_test)
  {
  hash_table_t *ht = create_hash(2);
  int           i;
  int           val;

  for (i = 0; i < num_keys; i++)
    fail_unless(add_hash(ht, vals[i], more_keys[i]) == PBSE_NONE, "add_hash failed in rehash_test");

  for (i = 0; i < num_keys; i++)
    {
    val = get_value_hash(ht, more_keys[i]);
    fail_unless(val == vals[i], "values don't match in rehash_test");
    }

  free_hash(ht);
  }
END_TEST

START_TEST(remove_change)
  {
  hash_table_t *ht = create_hash(DEFAULT_HASH_SIZE);
  int           i;
  int           val;

  for (i = 0; i < num_keys; i++)
    fail_unless(add_hash(ht, vals[i], more_keys[i]) == PBSE_NONE, "add_hash failed in change test");

  for (i = 0; i < num_keys; i++)
    {
    change_value_hash(ht, more_keys[i], vals[i] + 100);
    val = get_value_hash(ht, more_keys[i]);
    fail_unless(val == vals[i] + 100, "values don't match in change test");
    }

  for (i = 0; i < num_keys; i++)
    {
    fail_unless(remove_hash(ht, keys[i]) == KEY_NOT_FOUND, "removing non-existent key!");
    fail_unless(remove_hash(ht, more_keys[i]) == PBSE_NONE, "failed removing key");
    }

  free_hash(ht);
  
  /* remove with collisions */
  ht = create_hash(2);
  add_hash(ht, vals[0], keys[0]);
  add_hash(ht, vals[1], keys[4]);
  fail_unless(remove_hash(ht, keys[0]) == PBSE_NONE, "failed removing key");
  fail_unless(remove_hash(ht, keys[4]) == PBSE_NONE, "failed removing key");

  add_hash(ht, vals[0], keys[0]);
  add_hash(ht, vals[1], keys[4]);
  change_value_hash(ht, keys[4], vals[2]);
  val = get_value_hash(ht, keys[4]);
  fail_unless(val == vals[2], "values don't match on collision change");
  fail_unless(remove_hash(ht, keys[4]) == PBSE_NONE, "failed removing key");
  fail_unless(remove_hash(ht, keys[0]) == PBSE_NONE, "failed removing key");
  }
END_TEST

Suite *u_hash_table_suite(void)
  {
  Suite *s = suite_create("u_hash_table_suite methods");
  TCase *tc_core = tcase_create("create_insert_get");
  tcase_add_test(tc_core, create_insert_get);
  suite_add_tcase(s, tc_core);

  tc_core = tcase_create("rehash_test");
  tcase_add_test(tc_core, rehash_test);
  suite_add_tcase(s, tc_core);

  tc_core = tcase_create("remove_change");
  tcase_add_test(tc_core, remove_change);
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
  sr = srunner_create(u_hash_table_suite());
  srunner_set_log(sr, "u_hash_table_suite.log");
  srunner_run_all(sr, CK_NORMAL);
  number_failed = srunner_ntests_failed(sr);
  srunner_free(sr);
  return number_failed;
  }
