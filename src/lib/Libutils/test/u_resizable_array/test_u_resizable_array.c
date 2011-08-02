#include "license_pbs.h"
#include "u_resizable_array.h"
#include "test_u_resizable_array.h"
#include <stdlib.h>
#include <stdio.h>
#include <check.h>

#include "pbs_error.h"
#include "scaffolding.h"
  
char *a[] = {"a", "b", "c", "d", "e", "f", "g", "h", "i" };

/* check that arrays are initialized and resized correctly */
START_TEST(initialize_and_resize)
  {
  int rc = 0;
  int start_size = 2;

  /* check initial values */
  resizable_array *ra = intitialize_resizable_array(start_size);

  fail_unless(ra->num == 0, "initial number for resizable array should be 0");
  fail_unless(ra->max == start_size, "max initialized incorrectly");
  fail_unless(ra->last == ALWAYS_EMPTY_INDEX, "last used index initialized incorrectly");
  fail_unless(ra->next_slot == 1, "next slot should be set to 1 initially");
  fail_unless(ra->slots[ALWAYS_EMPTY_INDEX].next != ra->next_slot, "initial next_slot is wrong");

  /* should not resize here */
  rc = insert_thing(ra, a[1]);

  fail_unless(rc >= 0, "insert_thing failed");
  fail_unless(ra->num == 1, "number of elements is incorrect");
  fail_unless(ra->max == start_size, "prematurely resized the array");
  fail_unless(ra->last == 1, "last index incorrect after first insert");
  fail_unless(ra->next_slot == 2, "next_slot updated incorrectly");
  fail_unless(ra->slots[rc].next == ALWAYS_EMPTY_INDEX, "new slot not pointing to the empty index");

  /* should resize here */
  rc = insert_thing(ra, a[2]);

  fail_unless(rc == 0, "insert_thing failed");
  fail_unless(ra->max == start_size * 2, "array should have resized but didn't");
  fail_unless(ra->num == 2, "number of elements is incorrect");
  fail_unless(ra->last == 2, "last index incorrect after two inserts");
  fail_unless(ra->next_slot == 3, "next_slot updated incorrectly");
  fail_unless(ra->slots[rc].next == ALWAYS_EMPTY_INDEX, "new slot not pointing to the empty index");
  }
END_TEST

START_TEST(test_two)
  {
  int rc = 0;
  return rc;
  }
END_TEST

Suite *u_resizable_array_suite(void)
  {
  Suite *s = suite_create("u_resizable_array_suite methods");
  TCase *tc_core = tcase_create("test_one");
  tcase_add_test(tc_core, test_one);
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
  rundebug();
  SRunner *sr = srunner_create(u_resizable_array_suite());
  srunner_set_log(sr, "u_resizable_array_suite.log");
  srunner_run_all(sr, CK_NORMAL);
  number_failed = srunner_ntests_failed(sr);
  srunner_free(sr);
  return number_failed;
  }
