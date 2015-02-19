#include "machine.hpp"
#include <stdio.h>
#include <stdlib.h>
#include <check.h>
#include "log.h"
#include "hwloc.h"
#include "pbs_error.h"
#include <sstream>


START_TEST(test_initializeChip)
  {
  int rc = -1;
  int style;
  Chip new_chip;
  hwloc_topology_t topology = NULL;
  hwloc_obj_t chip_obj;
  hwloc_obj_t prev = NULL;

  
  hwloc_topology_init(&topology);
  hwloc_topology_set_flags(topology, HWLOC_TOPOLOGY_FLAG_WHOLE_SYSTEM);
  hwloc_topology_load(topology);

  style = get_hardware_style(topology);
  
  if (style == NON_NUMA)
    {
    hwloc_obj_t socket_obj;
    socket_obj = hwloc_get_next_obj_by_type(topology, HWLOC_OBJ_SOCKET, prev);
    if (socket_obj != NULL)
      {
      rc = new_chip.initializeNonNUMAChip(socket_obj, topology);
      fail_unless(rc==PBSE_NONE, "could not initialize non NUMA chip");
      }
    else
      {
      /* rc should be -1. This will flag the error for us */
      fail_unless(rc == PBSE_NONE, "Could not get chip object from hwloc");
      }
    }
  else if (style == INTEL || style == AMD)
    {
   chip_obj = hwloc_get_next_obj_by_type(topology, HWLOC_OBJ_NODE, prev);
    if (chip_obj != NULL)
      {
      rc = new_chip.initializeChip(chip_obj, topology);
      fail_unless(rc==PBSE_NONE, "could not initialize  NUMA chip");
      }
    else
      {
      /* rc should be -1. This will flag the error for us */
      fail_unless(rc == PBSE_NONE, "Could not get chip object from hwloc");
      }
    }
  else
    fail_unless(style==AMD, "Could not get hardware style");



  rc = new_chip.getMemoryInBytes();
  fail_unless(rc != 0, "failed to get chip memory");

  rc = new_chip.getTotalCores();
  fail_unless(rc != 0, "failed to get chip total cores");

  rc = new_chip.getTotalThreads();
  fail_unless(rc != 0, "failed to get chip total Threads");

  rc = new_chip.getAvailableCores();
  fail_unless(rc != 0, "failed to get chip available cores");

  rc = new_chip.getAvailableThreads();
  fail_unless(rc != 0, "failed to get chip available Threads");


  hwloc_topology_destroy(topology);

  }
END_TEST


START_TEST(test_displayAsString)
  {
  std::stringstream out;
  Chip c;

  c.setMemoryInBytes(2048);
  c.setId(0);
  c.displayAsString(out);
  fail_unless(out.str() == "    Chip 0 (2KB)\n", out.str().c_str());
  }
END_TEST


Suite *numa_socket_suite(void)
  {
  Suite *s = suite_create("numa_socket test suite methods");
  TCase *tc_core = tcase_create("test_initializeChip");
  tcase_add_test(tc_core, test_initializeChip);
  suite_add_tcase(s, tc_core);
  
  tc_core = tcase_create("test_displayAsString");
  tcase_add_test(tc_core, test_displayAsString);
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
  sr = srunner_create(numa_socket_suite());
  srunner_set_log(sr, "numa_socket_suite.log");
  srunner_run_all(sr, CK_NORMAL);
  number_failed = srunner_ntests_failed(sr);
  srunner_free(sr);
  return(number_failed);
  }
