#include "license_pbs.h" /* See here for the software license */
#include "lib_utils.h"
#include "test_u_resizable_array.h"
#include <stdlib.h>
#include <stdio.h>

#include "pbs_error.h"
  
char  buf[4096];
char *a[] = {"a", "b", "c", "d", "e", "f", "g", "h", "i" };
int   num_elements = 9;

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



START_TEST(ordering_tests)
  {
  int              i;
  int              iter;
  int              backwards_iter = -1;
  int              start_size = 2;
  int              ones_index;
  int              twos_index;
  int              sevens_index;
  void            *thing;
  resizable_array *ra = initialize_resizable_array(start_size);

  /* insert all elements */
  for (i = 0; i < num_elements; i++)
    insert_thing(ra, a[i]);

  i = 8;

  while ((thing = next_thing_from_back(ra, &backwards_iter)) != NULL)
    {
    fail_unless(thing == a[i], "out of order when iterating backwards");
    i--;
    }

  free_resizable_array(ra);
  ra = initialize_resizable_array(start_size);

  insert_thing(ra, a[0]);
  twos_index = insert_thing(ra, a[2]);
  ones_index = insert_thing(ra, a[1]);
  sevens_index = insert_thing(ra, a[7]);

  backwards_iter = -1;
  thing = next_thing_from_back(ra, &backwards_iter);
  fail_unless(thing == a[7], "not iterating backwards in order");
  thing = next_thing_from_back(ra, &backwards_iter);
  fail_unless(thing == a[1], "not iterating backwards in order");
  thing = next_thing_from_back(ra, &backwards_iter);
  fail_unless(thing == a[2], "not iterating backwards in order");
  thing = next_thing_from_back(ra, &backwards_iter);
  fail_unless(thing == a[0], "not iterating backwards in order");

  /* add some things using the ordered adds */
  insert_thing_after(ra, a[3], ones_index);
  insert_thing_before(ra, a[4], sevens_index);
  insert_thing_after(ra, a[5], twos_index);

  backwards_iter = -1;
  thing = next_thing_from_back(ra, &backwards_iter);
  fail_unless(thing == a[7], "ordered inserts malfunctioning");
  thing = next_thing_from_back(ra, &backwards_iter);
  fail_unless(thing == a[4], "ordered inserts malfunctioning");
  thing = next_thing_from_back(ra, &backwards_iter);
  fail_unless(thing == a[3], "ordered inserts malfunctioning");
  thing = next_thing_from_back(ra, &backwards_iter);
  fail_unless(thing == a[1], "ordered inserts malfunctioning");
  thing = next_thing_from_back(ra, &backwards_iter);
  fail_unless(thing == a[5], "ordered inserts malfunctioning");
  thing = next_thing_from_back(ra, &backwards_iter);
  fail_unless(thing == a[2], "ordered inserts malfunctioning");
  thing = next_thing_from_back(ra, &backwards_iter);
  fail_unless(thing == a[0], "ordered inserts malfunctioning");

  /* now remove some stuff */
  remove_last_thing(ra);
  remove_thing_from_index(ra, twos_index);
  remove_thing_from_index(ra, ones_index);
  
  backwards_iter = -1;
  thing = next_thing_from_back(ra, &backwards_iter);
  fail_unless(thing == a[4], "ordered inserts malfunctioning after removals");
  thing = next_thing_from_back(ra, &backwards_iter);
  fail_unless(thing == a[3], "ordered inserts malfunctioning after removals");
  thing = next_thing_from_back(ra, &backwards_iter);
  fail_unless(thing == a[5], "ordered inserts malfunctioning after removals");
  thing = next_thing_from_back(ra, &backwards_iter);
  fail_unless(thing == a[0], "ordered inserts malfunctioning after removals");

  /* swap some things */
  swap_things(ra, a[4], a[3]);
  swap_things(ra, a[5], a[0]);

  iter = -1;
  thing = next_thing(ra, &iter);
  fail_unless(thing == a[5], "swap is messing up the order");
  thing = next_thing(ra, &iter);
  fail_unless(thing == a[0], "swap is messing up the order");
  thing = next_thing(ra, &iter);
  fail_unless(thing == a[4], "swap is messing up the order");
  thing = next_thing(ra, &iter);
  fail_unless(thing == a[3], "swap is messing up the order");
  
  }
END_TEST



START_TEST(get_index_tests)
  {
  void            *thing;
  resizable_array *ra = initialize_resizable_array(20);
  int              index;
  int              i;
  int              iter;

  for (i = 0; i < num_elements; i++)
    {
    insert_thing(ra, a[i]);
    }

  for (i = 0; i < num_elements; i++)
    {
    index = get_index(ra, a[i]);
    thing = get_thing_from_index(ra, index);
    snprintf(buf, sizeof(buf), "Object at index not the same: %s != %s",
      a[i], (char *)thing);
    fail_unless(thing == a[i], buf);
    }

  i = num_elements;

  while (i > 0)
    {
    initialize_ra_iterator(ra, &iter);
    fail_unless(iter == ra->slots[ALWAYS_EMPTY_INDEX].next, "Iterator not initialized correctly");
    thing = pop_thing(ra);
    i--;
    }

  }
END_TEST




START_TEST(error_cases)
  {
  resizable_array *ra = initialize_resizable_array(20);
  int              i;
  int              old_last;
  int              rc;
  char            *tom = "tom";
  char            *bob = "bob";
  void            *thing;
  int              index = -1;

  for (i = 0; i < num_elements; i++)
    {
    insert_thing(ra, a[i]);
    }

  rc = swap_things(ra, a[0], tom);
  fail_unless(rc == THING_NOT_FOUND, "somehow thought unadded element was added");

  old_last = ra->last;
  insert_thing_after(ra, tom, ra->last);
  fail_unless(old_last != ra->last, "didn't properly insert thing after index");

  rc = remove_thing_from_index(ra, ra->last + 2);
  fail_unless(rc == THING_NOT_FOUND, "somehow found an extra element");

  rc = get_index(ra, bob);
  fail_unless(rc == THING_NOT_FOUND, "found element that doesn't exist");

  thing = get_thing_from_index(ra, index);
  fail_unless(thing == a[0], "order got messed up somewhere");

  index = 5000;
  thing = get_thing_from_index(ra, index);
  fail_unless(thing == NULL, "returned item for out of bounds index??");
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

  tc_core = tcase_create("ordering_tests");
  tcase_add_test(tc_core, ordering_tests);
  suite_add_tcase(s, tc_core);

  tc_core = tcase_create("get_index_tests");
  tcase_add_test(tc_core, get_index_tests);
  suite_add_tcase(s, tc_core);

  tc_core = tcase_create("error_cases");
  tcase_add_test(tc_core, error_cases);
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

