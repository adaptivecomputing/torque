#include "machine.hpp"
#include <check.h>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include "log.h"
#include "hwloc.h"
#include "pbs_error.h"
#include "pbs_job.h"
#include "complete_req.hpp"

extern int hardware_style;

int get_hardware_style(hwloc_topology_t topology);

extern int my_req_count;
extern int num_tasks_fit;
extern int num_placed;
extern int called_free_task;
extern int called_place_task;
extern int called_partially_place;
extern int called_fits_on_socket;
extern int num_for_host;
extern int called_store_pci;
extern int json_socket;
extern bool socket_fit;
extern bool partially_placed;

START_TEST(test_displayAsString)
  {
  Machine           new_machine;
  std::stringstream out;

  new_machine.setMemory(2);
  new_machine.displayAsString(out);
  fail_unless(out.str() == "Machine (2KB)\n", out.str().c_str());
  }
END_TEST


START_TEST(test_json_constructor)
  {
  const char *j1 = "\"node\":{\"socket\":{\"os_index\":12,\"numachip\":{\"os_index\":24,\"cores\":48-49,\"threads\":\"\",\"mem\"=1},\"numachip\":{\"os_index\":25,\"cores\":50-51,\"threads\":\"\",\"mem\"=1}},\"socket\":{\"os_index\":13,\"numachip\":{\"os_index\":26,\"cores\":52-53,\"threads\":\"\",\"mem\"=1},\"numachip\":{\"os_index\":26,\"cores\":54-55,\"threads\":\"\",\"mem\"=1}}}";
  const char *j2 = "\"node\":{\"socket\":{\"os_index\":0,\"numachip\":{\"os_index\":0,\"cores\":0-5,\"threads\":\"12-17\",\"mem\"=1024},\"numachip\":{\"os_index\":1,\"cores\":6-11,\"threads\":\"18-23\",\"mem\"=1024}}}";
  std::stringstream out;

  Machine m1(j1);
  fail_unless(json_socket == 2, "%d times", json_socket);
  m1.displayAsJson(out);
  fail_unless(out.str() == "{\"node\":{,}}", out.str().c_str());

  out.str("");
  Machine m2(j2);
  fail_unless(json_socket == 3, "%d times", json_socket);
  m2.displayAsJson(out);
  fail_unless(out.str() == "{\"node\":{}}", out.str().c_str());
  }
END_TEST


START_TEST(test_store_pci_device_on_appropriate_chip)
  {
  Machine m;
  m.addSocket(2);

  PCI_Device d;
  called_store_pci = 0;

  // Since this is non numa it should be forced to socket 0 
  m.setIsNuma(false);
  m.store_device_on_appropriate_chip(d);
  fail_unless(called_store_pci == 1);
 
  // Since this is numa it should place on the sockets until it returns true,
  // which due to the scaffolding is never, so once per socket
  m.setIsNuma(true);
  m.store_device_on_appropriate_chip(d);
  fail_unless(called_store_pci == 3);
  }
END_TEST


START_TEST(test_get_hardware_style)
  {
  int style;
  hwloc_topology_t topology = NULL;

  hwloc_topology_init(&topology);
  hwloc_topology_set_flags(topology, HWLOC_TOPOLOGY_FLAG_WHOLE_SYSTEM);
  hwloc_topology_load(topology);

  style = get_hardware_style(topology);

  /* I know this is redundant but it counts the test case */
  if (style == NON_NUMA)
    fail_unless(style == NON_NUMA, "Non NUMA failed");
  else if (style == INTEL)
    fail_unless(style == INTEL, "Intel Style failed");
  else if (style == AMD)
    fail_unless(style == AMD, "AMD Style failed");
  else
    fail_unless(style==AMD, "failed to get style");

  hwloc_topology_destroy(topology);

  }
END_TEST


START_TEST(test_initializeMachine)
  {
  hwloc_topology_t topology;
  int rc;
  hwloc_uint64_t memory;
  Machine new_machine;

  hwloc_topology_init(&topology);
  hwloc_topology_set_flags(topology, HWLOC_TOPOLOGY_FLAG_WHOLE_SYSTEM);
  hwloc_topology_load(topology);

  rc = new_machine.initializeMachine(topology);
  fail_unless(rc == PBSE_NONE, "machine initialization failed");

  rc = new_machine.getNumberOfSockets();
  fail_unless(rc != 0, "Failed to get machine number of sockets");

  rc = new_machine.getTotalChips();
  fail_unless(rc != 0, "Failed to get machine number of chips");

  rc = new_machine.getTotalCores();
  fail_unless(rc != 0, "Failed to get machine number of cores");

  rc = new_machine.getTotalThreads();
  fail_unless(rc != 0, "Failed to get machine number of Threads");

  rc = new_machine.getAvailableSockets();
  fail_unless(rc != 0, "Failed to get machine available of sockets");

  rc = new_machine.getAvailableChips();
  fail_unless(rc != 0, "Failed to get machine available of Chips");

  /* these two are just pass through calls to the sockets
  rc = new_machine.getAvailableCores();
  fail_unless(rc != 0, "Failed to get machine available of Cores");

  rc = new_machine.getAvailableThreads();
  fail_unless(rc != 0, "Failed to get machine available of threads"); */

  memory = new_machine.getTotalMemory();
  fail_unless(memory != 0, "Failed to get machine total memory");

  hwloc_topology_destroy(topology);
  }
END_TEST


START_TEST(test_place_and_free_job)
  {
  std::string cpu;
  std::string mem;
  Machine m;
  m.addSocket(2);
  job pjob;
  complete_req cr;
  pjob.ji_wattr[JOB_ATR_req_information].at_val.at_ptr = &cr;

  // Make the job fit on one socket so we call place once per task
  called_place_task = 0;
  my_req_count = 2;
  num_for_host = 4;
  num_tasks_fit = 4;
  num_placed = 4;
  m.place_job(&pjob, cpu, mem);
  fail_unless(called_place_task == 2, "Expected 2 calls but got %d", called_place_task);

  // Make sure we call free tasks once per socket
  called_free_task = 0;
  m.free_job_allocation("1.napali");
  fail_unless(called_free_task == 2);

  // Get a job that is placed on multiple sockets
  my_req_count = 1;
  num_for_host = 8;
  num_tasks_fit = 4;
  num_placed = 4;
  called_place_task = 0;
  m.place_job(&pjob, cpu, mem);
  fail_unless(called_place_task == 2, "Expected 2 calls but got %d", called_place_task);

  num_tasks_fit = 0;
  num_placed = 0;
  my_req_count = 1;
  num_for_host = 1;
  socket_fit = true;
  partially_placed = true;
  called_partially_place = 0;
  called_fits_on_socket = 0;
  m.place_job(&pjob, cpu, mem);
  fail_unless(called_partially_place == 1, "called %d", called_partially_place);
  fail_unless(called_fits_on_socket == 1);
  
  socket_fit = false;
  num_tasks_fit = 0;
  num_placed = 0;
  my_req_count = 1;
  num_for_host = 1;

  m.place_job(&pjob, cpu, mem);
  fail_unless(called_partially_place == 2, "called %d", called_partially_place);
  fail_unless(called_fits_on_socket == 3);
  }
END_TEST



Suite *machine_suite(void)
  {
  Suite *s = suite_create("machine test suite methods");
  TCase *tc_core = tcase_create("test_get_hardware_style");
  tcase_add_test(tc_core, test_get_hardware_style);
  suite_add_tcase(s, tc_core);

  tc_core = tcase_create("test_initializeMachine");
  tcase_add_test(tc_core, test_initializeMachine);
  tcase_add_test(tc_core, test_displayAsString);
  tcase_add_test(tc_core, test_json_constructor);
  suite_add_tcase(s, tc_core);
  
  tc_core = tcase_create("test_place_and_free_job");
  tcase_add_test(tc_core, test_place_and_free_job);
  tcase_add_test(tc_core, test_store_pci_device_on_appropriate_chip);
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
  sr = srunner_create(machine_suite());
  srunner_set_log(sr, "machine_suite.log");
  srunner_run_all(sr, CK_NORMAL);
  number_failed = srunner_ntests_failed(sr);
  srunner_free(sr);
  return(number_failed);
  }
