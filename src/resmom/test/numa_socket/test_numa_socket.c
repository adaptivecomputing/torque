#include "machine.hpp"
#include <stdio.h>
#include <stdlib.h>
#include <check.h>

#include "log.h"
#include "hwloc.h"
#include "pbs_error.h"
#include "allocation.hpp"
#include "req.hpp"

extern int tasks;
extern int placed;
extern int called_place;
extern int oscillate;
extern int called_store_pci;
extern bool avail_oscillate;
extern int place_amount;
extern std::string my_placement_type;

START_TEST(test_displayAsString)
  {
  std::stringstream out;
  Socket s;
  s.setMemory(2);
  s.setId(0);

  s.displayAsString(out);
  fail_unless(out.str() == "  Socket 0 (2KB)\n", out.str().c_str());
  }
END_TEST


START_TEST(test_store_pci_device_appropriately)
  {
  PCI_Device d;
  Socket s;
  s.addChip();
  s.addChip();

  called_store_pci = 0;
  fail_unless(s.store_pci_device_appropriately(d, false) == false);
  fail_unless(called_store_pci == 2);
  fail_unless(s.store_pci_device_appropriately(d, true) == true);
  fail_unless(called_store_pci == 3);
  }
END_TEST


START_TEST(test_initializeSocket)
  {
  int rc = -1;
  int style;
  Socket new_socket;
  hwloc_topology_t topology = NULL;
  hwloc_obj_t socket_obj;
  hwloc_obj_t prev = NULL;

  
  hwloc_topology_init(&topology);
  hwloc_topology_set_flags(topology, HWLOC_TOPOLOGY_FLAG_WHOLE_SYSTEM);
  hwloc_topology_load(topology);

  style = get_hardware_style(topology);
  
  if (style == NON_NUMA)
    {
   socket_obj = hwloc_get_next_obj_by_type(topology, HWLOC_OBJ_SOCKET, prev);
    if (socket_obj != NULL)
      {
      rc = new_socket.initializeNonNUMASocket(socket_obj, topology);
      fail_unless(rc==PBSE_NONE, "could not initialize non NUMA socket");
      rc = new_socket.getMemory();
      fail_unless(rc != 0, "failed to get socket memory");
      }
    else
      {
      /* rc should be -1. This will flag the error for us */
      fail_unless(rc == PBSE_NONE, "Could not get socket object from hwloc");
      }
    }
  else if (style == INTEL)
    {
   socket_obj = hwloc_get_next_obj_by_type(topology, HWLOC_OBJ_SOCKET, prev);
    if (socket_obj != NULL)
      {
      rc = new_socket.initializeIntelSocket(socket_obj, topology);
      fail_unless(rc==PBSE_NONE, "could not initialize Intel  NUMA socket");
      rc = new_socket.getMemory();
      fail_unless(rc == 0, "failed to get socket memory");
      }
    else
      {
      /* rc should be -1. This will flag the error for us */
      fail_unless(rc == PBSE_NONE, "Could not get socket object from hwloc");
      }
    }
  else if (style == AMD)
    {
   socket_obj = hwloc_get_next_obj_by_type(topology, HWLOC_OBJ_SOCKET, prev);
    if (socket_obj != NULL)
      {
      rc = new_socket.initializeAMDSocket(socket_obj, topology);
      fail_unless(rc==PBSE_NONE, "could not initialize Intel  NUMA socket");
      rc = new_socket.getMemory();
      fail_unless(rc != 0, "failed to get socket memory");
      }
    else
      {
      /* rc should be -1. This will flag the error for us */
      fail_unless(rc == PBSE_NONE, "Could not get socket object from hwloc");
      }
    }
  else
    fail_unless(style==AMD, "Could not get hardware style");

  rc = new_socket.getTotalChips();
  fail_unless(rc != 0, "failed to get socket total chips");

  rc = new_socket.getTotalCores();
  fail_unless(rc != 0, "failed to get socket total cores");

  rc = new_socket.getTotalThreads();
  fail_unless(rc != 0, "failed to get socket total Threads");

  rc = new_socket.getAvailableChips();
  fail_unless(rc != 0, "failed to get socket available chips");

  rc = new_socket.getAvailableCores();
  fail_unless(rc != 0, "failed to get socket available cores");

  rc = new_socket.getAvailableThreads();
  fail_unless(rc != 0, "failed to get socket available Threads");


  hwloc_topology_destroy(topology);

  }
END_TEST


START_TEST(test_place_task)
  {
  Socket s;
  req r;
  allocation a;
  s.addChip();
  s.addChip();

  called_place = 0;

  // These should all fit on the first chip, so chip.place_task() should only be called once
  tasks = 4;
  placed = 3;
  int rc = s.place_task("1.napali", r, a, 3);
  fail_unless(rc == 3);
  fail_unless(called_place = 1);

  // This call should use both nodes so place should get called twice and one should be leftover
  called_place = 0;
  a.place_type = exclusive_socket;
  tasks = 1;
  placed = 1;
  fail_unless(s.place_task("1.napali", r, a, 3) == 2);
  fail_unless(called_place = 2);

  fail_unless(s.is_available() == false);
  oscillate = false;
  fail_unless(s.free_task("1.napali") == true);
  fail_unless(s.is_available() == true);

  // set the chips to alternate saying available or unavailable
  avail_oscillate = true;
  fail_unless(s.is_available() == false);
  }
END_TEST


START_TEST(test_partial_place)
  {
  Socket s;
  req r;
  allocation remaining;
  s.addChip();
  s.addChip();

  remaining.cores_only = true;
  remaining.memory = 1;
  remaining.cpus = 1;

  fail_unless(s.fits_on_socket(remaining) == true);
  remaining.cores_only = false;
  fail_unless(s.fits_on_socket(remaining) == true);
  remaining.memory = 1024;
  fail_unless(s.fits_on_socket(remaining) == false);

  // Tell it to fully place the job
  place_amount = 1;
  remaining.cpus = 8;
  remaining.memory = 1024;
  allocation master;
  fail_unless(s.partially_place(remaining, master) == true);

  // Tell it to partially place the job
  place_amount = 2;
  remaining.cpus = 8;
  remaining.memory = 1024;
  fail_unless(s.partially_place(remaining, master) == false);
  }
END_TEST


START_TEST(test_free_task)
  {
  Socket s;
  req r;
  allocation a;
  s.addChip();
  s.addChip();

  // Should return true because the Chip::free_task is stubbed to always return true
  oscillate = false;
  fail_unless(s.free_task("1.napali") == true);

  // Should return false because oscillate makes Chip::free_task return true then false
  oscillate = true;
  fail_unless(s.free_task("1.napali") == false);
  }
END_TEST


START_TEST(test_how_many_tasks_fit)
  {
  Socket s;
  req r;
  s.addChip();
  s.addChip();

  // should get the number of chips * tasks for how many fit
  tasks = 2;
  fail_unless(s.how_many_tasks_fit(r, 0) == 4);
  tasks = 4;
  fail_unless(s.how_many_tasks_fit(r, 0) == 8);
  my_placement_type = place_socket;
  fail_unless(s.how_many_tasks_fit(r, 0) == 1);
  }
END_TEST


Suite *numa_socket_suite(void)
  {
  Suite *s = suite_create("numa_socket test suite methods");
  TCase *tc_core = tcase_create("test_initializeSocket");
  tcase_add_test(tc_core, test_initializeSocket);
  tcase_add_test(tc_core, test_place_task);
  tcase_add_test(tc_core, test_free_task);
  suite_add_tcase(s, tc_core);
  
  tc_core = tcase_create("test_displayAsString");
  tcase_add_test(tc_core, test_displayAsString);
  tcase_add_test(tc_core, test_how_many_tasks_fit);
  tcase_add_test(tc_core, test_partial_place);
  tcase_add_test(tc_core, test_store_pci_device_appropriately);
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
