#include "machine.hpp"
#include <stdio.h>
#include <stdlib.h>
#include <check.h>

#include "log.h"
#include "hwloc.h"
#include "pbs_error.h"
#include "allocation.hpp"
#include "req.hpp"

extern float tasks;
extern int placed;
extern int called_place;
extern int oscillate;
extern int called_store_pci;
extern int called_spread_place;
extern int called_spread_place_cores;
extern int called_spread_place_threads;
extern int place_amount;
extern int json_chip;
extern int placed_all;
extern int partially_placed;
extern bool completely_free;
extern std::string my_placement_type;


START_TEST(test_place_all_execution_slots)
  {
  Socket s;
  req        r;
  allocation a;
  s.addChip();
  s.addChip();

  placed_all = 0;
  s.place_all_execution_slots(r, a);
  fail_unless(placed_all == 2);
  }
END_TEST

START_TEST(test_spread_place_pu)
  {
  Socket s;
  req        r;
  allocation a;
  int        cores_remaining = 2;
  int        lprocs_remaining = 2;
  int        gpus_remaining = 0;
  int        mics_remaining = 0;
  s.addChip();
  s.addChip();

  called_spread_place_cores = 0;
  a.set_place_type(place_core);
  fail_unless(s.spread_place_pu(r, a, cores_remaining, lprocs_remaining, gpus_remaining, mics_remaining) == true);
  fail_unless(called_spread_place_cores == 2);
  
  }
END_TEST


START_TEST(test_spread_place)
  {
  Socket s;
  req        r;
  allocation a;
  int        remaining = 0;
  s.addChip();
  s.addChip();

  completely_free = true;
  called_spread_place = 0;
  fail_unless(s.spread_place(r, a, 5, remaining, false) == true);
  fail_unless(called_spread_place == 2);
  
  completely_free = false;
  fail_unless(s.spread_place(r, a, 5, remaining, false) == false);
  fail_unless(called_spread_place == 2);

  completely_free = true;
  oscillate = false;
  s.free_task("1.napali");
  fail_unless(s.spread_place(r, a, 5, remaining, true) == true);
  fail_unless(called_spread_place == 3);
  fail_unless(s.spread_place(r, a, 5, remaining, true) == true);
  fail_unless(called_spread_place == 4);

  }
END_TEST

START_TEST(test_accelerators_remaining)
  {
  Socket s;
  req    r;
  allocation a;
  int gpus_remaining;
  int mics_remaining;

  s.addChip();
  s.addChip();

  gpus_remaining = s.get_gpus_remaining();
  mics_remaining = s.get_mics_remaining();

  fail_unless(gpus_remaining == 0);
  fail_unless(mics_remaining == 0);
  }
END_TEST


START_TEST(test_basic_constructor)
  {
  int remainder = 0;
  Socket s(5, 1, remainder);

  fail_unless(s.getTotalChips() == 1);
  fail_unless(s.getAvailableChips() == 1);
  }
END_TEST


START_TEST(test_json_constructor)
  {
  const char *j1 = "\"socket\":{\"os_index\":12,\"numanode\":{\"os_index\":24,\"cores\":48-49,\"threads\":\"\",\"mem\"=1},\"numanode\":{\"os_index\":25,\"cores\":50-51,\"threads\":\"\",\"mem\"=1}}";
  const char *j2 = "\"socket\":{\"os_index\":0,\"numanode\":{\"os_index\":0,\"cores\":0-5,\"threads\":\"12-17\",\"mem\"=1024},\"numanode\":{\"os_index\":1,\"cores\":6-11,\"threads\":\"18-23\",\"mem\"=1024}}";
  const char *j3 = "\"socket\":{\"os_index\":2,\"numanode\":{\"os_index\":2,\"cores\":0-11,\"threads\":\"12-23\",\"mem\"=10241024}}";
  std::stringstream out;

  std::vector<std::string> valid_ids;

  Socket s1(j1, valid_ids);
  fail_unless(json_chip == 2, "%d times", json_chip);
  s1.displayAsJson(out, false);
  fail_unless(out.str() == "\"socket\":{\"os_index\":12,,}", out.str().c_str());

  out.str("");
  Socket s2(j2, valid_ids);
  fail_unless(json_chip == 4, "%d times", json_chip);
  s2.displayAsJson(out, false);
  fail_unless(out.str() == "\"socket\":{\"os_index\":0,,}", out.str().c_str());

  out.str("");
  Socket s3(j3, valid_ids);
  fail_unless(json_chip == 5, "%d times", json_chip);
  s3.displayAsJson(out, false);
  fail_unless(out.str() == "\"socket\":{\"os_index\":2,}", out.str().c_str());
  }
END_TEST


START_TEST(test_displayAsString)
  {
  std::stringstream out;
  int remainder = 0;
  Socket s(1, 1, remainder);
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
  int rc = s.place_task(r, a, 3, "napali");
  fail_unless(rc == 3);
  fail_unless(called_place = 1);

  // This call should use both nodes so place should get called twice and one should be leftover
  called_place = 0;
  a.place_type = exclusive_socket;
  tasks = 1;
  placed = 1;
  place_amount = 4; // make partially place do nothing
  int num = s.place_task(r, a, 3, "napali");
  fail_unless(num == 1, "Expected 1, got %d", num);
  fail_unless(called_place = 1);

  fail_unless(s.is_available() == false);
  oscillate = false;
  fail_unless(s.free_task("1.napali") == true);
  fail_unless(s.is_available() == true);

  // Make sure we'll place a task that fits on this socket but has to span numa nodes
  tasks = 0.5;
  placed = 0;
  place_amount = 3;
  partially_placed = 0;
  num = s.place_task(r, a, 1, "napali");
  fail_unless(num == 1, "Expected 1, got %d", num);
  fail_unless(partially_placed == 2);
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
  remaining.place_cpus = 0;

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
  int fit = s.how_many_tasks_fit(r, 0);
  fail_unless(fit == 4, "Only %d tasks fit", fit);
  tasks = 4;
  fit = s.how_many_tasks_fit(r, 0);
  fail_unless(fit == 8, "Only %d tasks fit, expected 8", fit);
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
  tcase_add_test(tc_core, test_json_constructor);
  tcase_add_test(tc_core, test_basic_constructor);
  tcase_add_test(tc_core, test_place_all_execution_slots);
  suite_add_tcase(s, tc_core);
  
  tc_core = tcase_create("test_displayAsString");
  tcase_add_test(tc_core, test_displayAsString);
  tcase_add_test(tc_core, test_how_many_tasks_fit);
  tcase_add_test(tc_core, test_partial_place);
  tcase_add_test(tc_core, test_store_pci_device_appropriately);
  tcase_add_test(tc_core, test_spread_place_pu);
  tcase_add_test(tc_core, test_spread_place);
  tcase_add_test(tc_core, test_accelerators_remaining);
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
