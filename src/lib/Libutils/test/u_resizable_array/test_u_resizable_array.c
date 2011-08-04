#include "license_pbs.h" /* See here for the software license */
#include "lib_utils.h"
#include "test_u_resizable_array.h"
#include <stdlib.h>
#include <stdio.h>

#include "pbs_error.h"
  
char  buf[4096];
char *a[] = {"a", "b", "c", "d", "e", "f", "g", "h", "i" };

/* check that arrays are initialized and resized correctly */
START_TEST(initialize_and_resize)
  {
  int rc = 0;
  int start_size = 2;

  /* check initial values */
  resizable_array *ra = initialize_resizable_array(start_size);

  fail_unless(ra->num == 0, "initial number for resizable array should be 0");
  fail_unless(ra->max == start_size, "max initialized incorrectly");
  fail_unless(ra->last == ALWAYS_EMPTY_INDEX, "last used index initialized incorrectly");
  fail_unless(ra->next_slot == 1, "next slot should be set to 1 initially");
  sprintf(buf, "initialize next slot is wrong, should be %d but is %d",
    ALWAYS_EMPTY_INDEX,
    ra->slots[ALWAYS_EMPTY_INDEX].next);
  fail_unless(ra->slots[ALWAYS_EMPTY_INDEX].next == ALWAYS_EMPTY_INDEX, buf);

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

  fail_unless(rc >= 0, "insert_thing failed");
  fail_unless(ra->max == start_size * 2, "array should have resized but didn't");
  fail_unless(ra->num == 2, "number of elements is incorrect");
  fail_unless(ra->last == 2, "last index incorrect after two inserts");
  fail_unless(ra->next_slot == 3, "next_slot updated incorrectly");
  fail_unless(ra->slots[rc].next == ALWAYS_EMPTY_INDEX, "new slot not pointing to the empty index");

  free(ra->slots);
  free(ra);
  }
END_TEST

START_TEST(add_and_remove)
  {
  int start_size = 4;
  resizable_array *ra = initialize_resizable_array(start_size);

  /* insert tests */
  fail_unless(insert_thing(ra, a[0]) >= 0, "insert_thing failed");
  fail_unless(insert_thing(ra, a[1]) >= 0, "insert_thing failed");
  fail_unless(insert_thing(ra, a[2]) >= 0, "insert_thing failed");
  fail_unless(insert_thing(ra, a[3]) >= 0, "insert_thing failed");

  /* is_present tests */
  fail_unless(is_present(ra, a[0]) == TRUE, "inserted but not present");
  fail_unless(is_present(ra, a[1]) == TRUE, "inserted but not present");
  fail_unless(is_present(ra, a[2]) == TRUE, "inserted but not present");
  fail_unless(is_present(ra, a[3]) == TRUE, "inserted but not present");

  /* remove tests */
  fail_unless(remove_thing(ra, a[0]) == PBSE_NONE, "couldn't remove object that is present");
  fail_unless(remove_thing(ra, a[0]) == THING_NOT_FOUND, "couldn't remove object that is present");
  fail_unless(remove_thing(ra, a[1]) == PBSE_NONE, "couldn't remove object that is present");
  fail_unless(remove_thing(ra, a[1]) == THING_NOT_FOUND, "couldn't remove object that is present");
  fail_unless(remove_thing(ra, a[2]) == PBSE_NONE, "couldn't remove object that is present");
  fail_unless(remove_thing(ra, a[2]) == THING_NOT_FOUND, "couldn't remove object that is present");
  fail_unless(remove_thing(ra, a[3]) == PBSE_NONE, "couldn't remove object that is present");
  fail_unless(remove_thing(ra, a[3]) == THING_NOT_FOUND, "couldn't remove object that is present");

  /* is_present tests */
  fail_unless(is_present(ra, a[0]) == FALSE, "inserted but not present");
  fail_unless(is_present(ra, a[1]) == FALSE, "inserted but not present");
  fail_unless(is_present(ra, a[2]) == FALSE, "inserted but not present");
  fail_unless(is_present(ra, a[3]) == FALSE, "inserted but not present");

  free(ra->slots);
  free(ra);
  }
END_TEST


START_TEST(iteration_tests)
  {
  int              var_index = 0;
  int              ra_index = -1;
  int              iter_count = 0;
  int              start_size = 2;
  void            *thing;
  resizable_array *ra = initialize_resizable_array(start_size);

  /* insert things */
  fail_unless(insert_thing(ra, a[0]) >= 0, "insert_thing failed");
  fail_unless(insert_thing(ra, a[1]) >= 0, "insert_thing failed");
  fail_unless(insert_thing(ra, a[2]) >= 0, "insert_thing failed");
  fail_unless(insert_thing(ra, a[3]) >= 0, "insert_thing failed");
  fail_unless(insert_thing(ra, a[4]) >= 0, "insert_thing failed");
  fail_unless(insert_thing(ra, a[5]) >= 0, "insert_thing failed");
  fail_unless(insert_thing(ra, a[6]) >= 0, "insert_thing failed");
  fail_unless(insert_thing(ra, a[7]) >= 0, "insert_thing failed");
  fail_unless(insert_thing(ra, a[8]) >= 0, "insert_thing failed");

  /* make sure iterations work */
  while ((thing = next_thing(ra,&ra_index)) != NULL)
    {
    sprintf(buf, "not iterating in order, should be %s but %s -- test 1", a[var_index], (char *)thing);
    fail_unless(thing == a[var_index], buf);
    var_index++;
    iter_count++;
    }

  sprintf(buf, "incorrect number of iterations, should be 9 but %d", iter_count);
  fail_unless(iter_count == 9, buf);

  /* remove something and check iterations again */
  fail_unless(remove_thing(ra, a[4]) == PBSE_NONE, "remove_thing failed");

  var_index = 0;
  iter_count = 0;
  ra_index = -1;

  while ((thing = next_thing(ra,&ra_index)) != NULL)
    {
    if (var_index == 4)
      var_index++;

    sprintf(buf, "not iterating in order, should be %s but %s -- test 2", a[var_index], (char *)thing);
    fail_unless(thing == a[var_index], buf);
    var_index++;
    iter_count++;
    }

  sprintf(buf, "incorrect number of iterations, should be 8 but %d", iter_count);
  fail_unless(iter_count == 8, buf);

  /* remove something and check iterations again */
  fail_unless(remove_thing(ra, a[0]) == PBSE_NONE, "remove_thing failed");

  var_index = 1;
  ra_index = -1;
  iter_count = 0;

  while ((thing = next_thing(ra, &ra_index)) != NULL)
    {
    if (var_index == 4)
      var_index++;

    sprintf(buf, "not iterating in order, should be %s but %s -- test 3", a[var_index], (char *)thing);
    fail_unless(thing == a[var_index], buf);
    var_index++;
    iter_count++;
    }

  sprintf(buf, "incorrect number of iterations, should be 7 but %d", iter_count);
  fail_unless(iter_count == 7, buf);

  /* add something and check iterations again */
  fail_unless(insert_thing(ra, a[4]) >= 0, "insert_thing failed");

  var_index = 1;
  ra_index = -1;
  iter_count = 0;

  while ((thing = next_thing(ra, &ra_index)) != NULL)
    {
    if (iter_count == 7)
      {
      fail_unless(thing == a[4], "iteration is out of order after re-insert");
      }
    else
      {
      if (var_index == 4)
        var_index++;
      
      sprintf(buf, "not iterating in order, should be %s but %s -- test 4", a[var_index], (char *)thing);
      fail_unless(thing == a[var_index], buf);
      }
      
    var_index++;
    iter_count++;
    }

  sprintf(buf, "incorrect number of iterations, should be 8 again but %d", iter_count);
  fail_unless(iter_count == 8, buf);

  }
END_TEST



Suite *u_resizable_array_suite(void)
  {
  Suite *s = suite_create("u_resizable_array_suite methods");
  TCase *tc_core = tcase_create("initialize_and_resize");
  tcase_add_test(tc_core, initialize_and_resize);
  suite_add_tcase(s, tc_core);

  tc_core = tcase_create("add_and_remove");
  tcase_add_test(tc_core, add_and_remove);
  suite_add_tcase(s, tc_core);

  tc_core = tcase_create("iteration tests");
  tcase_add_test(tc_core, iteration_tests);
  suite_add_tcase(s, tc_core);

  return s;
  }

void rundebug()
  {
  int start_size = 2;

  /* check initial values */
  resizable_array *ra = initialize_resizable_array(start_size);

  if (ra->slots[ALWAYS_EMPTY_INDEX].next != ALWAYS_EMPTY_INDEX)
    printf("always empty's next is %d, should be %d",
      ra->slots[ALWAYS_EMPTY_INDEX].next,
      ALWAYS_EMPTY_INDEX);
  }

int main(void)
  {
  int number_failed = 0;
  SRunner *sr = NULL;
  rundebug();
  sr = srunner_create(u_resizable_array_suite());
  srunner_set_log(sr, "u_resizable_array_suite.log");
  srunner_run_all(sr, CK_NORMAL);
  number_failed = srunner_ntests_failed(sr);
  srunner_free(sr);
  return number_failed;
  }
