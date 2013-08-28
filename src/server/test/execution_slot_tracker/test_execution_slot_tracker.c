#include <stdio.h>
#include <stdlib.h>
#include <check.h>


#include "execution_slot_tracker.hpp"
#include "pbs_error.h"


START_TEST(test_reserve_slot)
  {
  execution_slot_tracker est;
  execution_slot_tracker est2;

  for (int i = 0; i < 10; i++)
    est.add_execution_slot();

  for (int i = 0; i < 10; i++)
    {
    fail_unless(est.reserve_execution_slot(i, est2) == PBSE_NONE);
    fail_unless(est.is_occupied(i) == true);
    fail_unless(est2.is_occupied(i) == true);
    }
  }
END_TEST


START_TEST(test_constructors)
  {
  execution_slot_tracker est;

  fail_unless(est.get_number_free() == 0, "number free not 0 by default");

  execution_slot_tracker est2(20);
  fail_unless(est2.get_number_free() == 20, "incorrect initialization");

  execution_slot_tracker est3(est2);
  fail_unless(est3.get_number_free() == est2.get_number_free(), "bad copy constructor");
  }
END_TEST


START_TEST(test_available)
  {
  execution_slot_tracker est;

  for (int i = 0; i < 10; i++)
    {
    est.add_execution_slot();
    fail_unless(est.get_number_free() == i + 1);
    fail_unless(est.get_total_execution_slots() == i + 1);
    }

  for (int i = 0; i < 10; i++)
    {
    est.mark_as_used(i);
    fail_unless(est.get_number_free() == 10 - i - 1);
    fail_unless(est.get_total_execution_slots() == 10);
    }

  for (int i = 0; i < 10; i++)
    {
    est.mark_as_free(i);
    fail_unless(est.get_number_free() == i + 1);
    }

  fail_unless(est.mark_as_free(10) != PBSE_NONE);
  fail_unless(est.get_number_free() == 10);
  
  fail_unless(est.mark_as_used(10) != PBSE_NONE);
  fail_unless(est.get_number_free() == 10);
  }
END_TEST


START_TEST(test_equals)
  {
  execution_slot_tracker est1(20);
 
    {
    execution_slot_tracker est2;

    est2 = est1;

    fail_unless(est2.get_number_free() == est1.get_number_free());
    }

  for (int i = 0; i < 5; i++)
    {
    est1.mark_as_used(i);
    }
  
  execution_slot_tracker est3 = est1;
  fail_unless(est3.get_number_free() == 15);
  }
END_TEST


START_TEST(test_reserving)
  {
  execution_slot_tracker e1(10);
  execution_slot_tracker e2(10);

  for (int i = 0; i < 10; i++)
    {
    e1.mark_as_used(i);
    if (i % 2 == 0)
      e2.mark_as_used(i);
    }

  e1.unset_subset(e2);
  fail_unless(e1.get_number_free() == 5);
  execution_slot_tracker e3(20);

  fail_unless(e1.unset_subset(e3) == SUBSET_TOO_LARGE);

  for (int i = 0; i < 10; i++)
    {
    e1.mark_as_free(i);
    e2.mark_as_free(i);
    }

  execution_slot_tracker e4;
  e1.reserve_execution_slots(4, e4);
  fail_unless(e4.get_total_execution_slots() == 10);
  fail_unless(e1.get_number_free() == 6);
  fail_unless(e4.get_number_free() == 6);

  e1.reserve_execution_slots(6, e2);
  fail_unless(e2.get_total_execution_slots() == 10);
  fail_unless(e2.get_number_free() == 4);
  fail_unless(e1.get_number_free() == 0);

  fail_unless(e1.reserve_execution_slots(1, e4) == INSUFFICIENT_FREE_EXECUTION_SLOTS);
  fail_unless(e4.get_number_free() == 6);

  fail_unless(e1.unreserve_execution_slots(e3) == SUBSET_TOO_LARGE);
  fail_unless(e1.get_number_free() == 0);

  e1.unreserve_execution_slots(e2);
  fail_unless(e1.get_number_free() == 6);
  fail_unless(e2.get_number_free() == 4);
  
  e1.unreserve_execution_slots(e4);
  fail_unless(e1.get_number_free() == 10);
  fail_unless(e4.get_number_free() == 6);
  }
END_TEST


START_TEST(test_occupied_iterator)
  {
  execution_slot_tracker e1(10);
  int                    iter = -1;
  int                    index;

  index = e1.get_next_occupied_index(iter);
  fail_unless(index == -1);
  fail_unless(iter >= 10);

  iter = -1;
  e1.mark_as_used(5);
  index = e1.get_next_occupied_index(iter);
  fail_unless(index == 5);
  fail_unless(iter == 6);
  index = e1.get_next_occupied_index(iter);
  fail_unless(index == -1);

  iter = -1;
  e1.mark_as_used(7);
  index = e1.get_next_occupied_index(iter);
  fail_unless(index == 5);
  fail_unless(iter == 6);
  index = e1.get_next_occupied_index(iter);
  fail_unless(index == 7);
  fail_unless(iter == 8);
  index = e1.get_next_occupied_index(iter);
  fail_unless(index == -1);

  iter = -1;
  e1.mark_as_used(3);
  index = e1.get_next_occupied_index(iter);
  fail_unless(index == 3);
  index = e1.get_next_occupied_index(iter);
  fail_unless(index == 5);
  fail_unless(iter == 6);
  index = e1.get_next_occupied_index(iter);
  fail_unless(index == 7);
  fail_unless(iter == 8);
  index = e1.get_next_occupied_index(iter);
  fail_unless(index == -1);
  }
END_TEST


Suite *execution_slot_tracker_suite(void)
  {
  Suite *s = suite_create("execution_slot_tracker test suite methods");
  TCase *tc_core = tcase_create("test_constructors");
  tcase_add_test(tc_core, test_constructors);
  suite_add_tcase(s, tc_core);
  
  tc_core = tcase_create("more_tests");
  tcase_add_test(tc_core, test_available);
  tcase_add_test(tc_core, test_equals);
  tcase_add_test(tc_core, test_reserving);
  tcase_add_test(tc_core, test_occupied_iterator);
  tcase_add_test(tc_core, test_reserve_slot);
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
  sr = srunner_create(execution_slot_tracker_suite());
  srunner_set_log(sr, "execution_slot_tracker_suite.log");
  srunner_run_all(sr, CK_NORMAL);
  number_failed = srunner_ntests_failed(sr);
  srunner_free(sr);
  return(number_failed);
  }
