#include "machine.hpp"
#include <stdio.h>
#include <stdlib.h>
#include <check.h>
#include "log.h"
#include "hwloc.h"
#include "pbs_error.h"



START_TEST(test_initializeCore)
  {
  int rc = -1;
  Core new_core;
  hwloc_topology_t topology = NULL;
  hwloc_obj_t core_obj;
  hwloc_obj_t prev = NULL;

  
  hwloc_topology_init(&topology);
  hwloc_topology_set_flags(topology, HWLOC_TOPOLOGY_FLAG_WHOLE_SYSTEM);
  hwloc_topology_load(topology);

  core_obj = hwloc_get_next_obj_by_type(topology, HWLOC_OBJ_SOCKET, prev);
  rc = new_core.initializeCore(core_obj, topology);
  fail_unless(rc == PBSE_NONE, "Failed to initialize cores");

  rc = new_core.getNumberOfProcessingUnits();
  fail_unless(rc != 0, "Failed to get number of processing units in core");
  }
END_TEST


START_TEST(test_displayAsString)
  {
  Core c;
  c.add_processing_unit(CORE, 0);
  std::stringstream out;

  c.displayAsString(out);
  fail_unless(out.str() == "      Core 0 (1 threads)\n", out.str().c_str());
  }
END_TEST


START_TEST(test_reserving_and_freeing)
  {
  Core c;
  bool completely_free;

  // unit test init gives us os indexes 0 and 8
  c.unit_test_init();
  c.reserve_processing_unit(0);
  c.reserve_processing_unit(8);

  fail_unless(c.get_open_processing_unit() == -1);
  
  // make sure freeing an index we don't have does nothing
  fail_unless(c.free_pu_index(1, completely_free) == false);
  fail_unless(completely_free == false);
  fail_unless(c.get_open_processing_unit() == -1);

  // make sure an actual free works
  fail_unless(c.free_pu_index(0, completely_free) == true);
  fail_unless(completely_free == false);
  fail_unless(c.get_open_processing_unit() == 0);
  fail_unless(c.free_pu_index(0, completely_free) == true);
  fail_unless(completely_free == false);
  fail_unless(c.free_pu_index(8, completely_free) == true);
  fail_unless(completely_free == true);
  int index = c.get_open_processing_unit();
  fail_unless(index != -1, "Open processing unit is %d, shouldn't be -1");
  index = c.get_open_processing_unit();
  fail_unless(index != -1, "Open processing unit is %d, shouldn't be -1");
  }
END_TEST


START_TEST(test_recovering_allocations)
  {
  Core c;
  bool completely_free = false;

  // unit test init gives us os indexes 0 and 8
  c.unit_test_init();
  fail_unless(c.reserve_processing_unit(0) == true);
  fail_unless(c.reserve_processing_unit(8) == true);
  fail_unless(c.reserve_processing_unit(1) == false);
  fail_unless(c.reserve_processing_unit(9) == false);
  fail_unless(c.get_open_processing_unit() == -1);
  fail_unless(c.free_pu_index(0, completely_free) == true);
  fail_unless(completely_free == false);
  fail_unless(c.is_free() == false);
  fail_unless(c.free_pu_index(8, completely_free) == true);
  fail_unless(completely_free == true);
  fail_unless(c.is_free() == true);
  }
END_TEST


START_TEST(test_add_processing_unit)
  {
  Core c;

  fail_unless(c.getNumberOfProcessingUnits() == 0, "%d processing units", c.getNumberOfProcessingUnits());
  fail_unless(c.get_id() == -1);

  c.add_processing_unit(CORE, 0);
  c.add_processing_unit(THREAD, 1);
  fail_unless(c.getNumberOfProcessingUnits() == 2);
  fail_unless(c.get_id() == 0);
  fail_unless(c.add_processing_unit(CORE, 2) != 0);
  fail_unless(c.getNumberOfProcessingUnits() == 2);
  fail_unless(c.get_id() == 0);

  Core c2;
  c2.add_processing_unit(CORE, 4);
  c2.add_processing_unit(THREAD, 5);
  c2.add_processing_unit(THREAD, 6);
  c2.add_processing_unit(THREAD, 7);
  fail_unless(c2.getNumberOfProcessingUnits() == 4, "%d", c2.getNumberOfProcessingUnits());
  fail_unless(c2.get_id() == 4);
  }
END_TEST


Suite *numa_core_suite(void)
  {
  Suite *s = suite_create("numa_core test suite methods");
  TCase *tc_core = tcase_create("test_initializeCore");
  tcase_add_test(tc_core, test_initializeCore);
  tcase_add_test(tc_core, test_add_processing_unit);
  suite_add_tcase(s, tc_core);
  
  tc_core = tcase_create("test_displayAsString");
  tcase_add_test(tc_core, test_displayAsString);
  tcase_add_test(tc_core, test_reserving_and_freeing);
  tcase_add_test(tc_core, test_recovering_allocations);
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
  sr = srunner_create(numa_core_suite());
  srunner_set_log(sr, "numa_core_suite.log");
  srunner_run_all(sr, CK_NORMAL);
  number_failed = srunner_ntests_failed(sr);
  srunner_free(sr);
  return(number_failed);
  }
