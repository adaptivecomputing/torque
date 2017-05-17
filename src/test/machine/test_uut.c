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
#include "pbs_config.h"

extern int hardware_style;

int get_hardware_style(hwloc_topology_t topology);

extern int my_req_count;
extern int my_core_count;
extern int num_tasks_fit;
extern int num_placed;
extern int called_free_task;
extern int called_place_task;
extern int called_partially_place;
extern int called_fits_on_socket;
extern int called_spread_place;
extern int called_spread_place_cores;
extern int num_for_host;
extern int called_store_pci;
extern int json_socket;
extern int sockets;
extern int numa_node_count;
extern int exec_slots;
extern int placed_all;
extern bool socket_fit;
extern bool partially_placed;
extern bool spreaded;
extern int  my_placement_type;
extern int  req_mem;
extern int  sock_mem;

START_TEST(test_check_if_possible)
  {
  Machine m;
  m.addSocket(2);

  int sockets = 3;
  int numanodes = 0;
  int cores = -1;
  int threads = -1;

  fail_unless(m.check_if_possible(sockets, numanodes, cores, threads) == false);

  numanodes = 1;
  cores = 3;
  threads = 3;
  fail_unless(m.check_if_possible(sockets, numanodes, cores, threads) == false);

  sockets = 2;
  numanodes = 0;
  cores = -1;
  threads = -1;
  fail_unless(m.check_if_possible(sockets, numanodes, cores, threads) == true);
  }
END_TEST


START_TEST(test_place_all_execution_slots)
  {
  Machine m;
  job     pjob;
  cgroup_info cgi;
  complete_req cr;
  pjob.ji_wattr[JOB_ATR_req_information].at_val.at_ptr = &cr;

  m.addSocket(2);
  sockets = 0;
  numa_node_count = 0;
  placed_all = 0;
  exec_slots = -1;
  num_for_host = 1;
  m.place_job(&pjob, cgi, "napali", false);
  fail_unless(placed_all == 2, "placed all: %d", placed_all);
  }
END_TEST


START_TEST(test_spread_place)
  {
  Machine m;
  job     pjob;
  cgroup_info cgi;
  complete_req cr;
  pjob.ji_wattr[JOB_ATR_req_information].at_val.at_ptr = &cr;

  m.addSocket(2);

  sockets = 1;
  my_placement_type = 2;
  numa_node_count = 0;
  called_spread_place = 0;
  num_for_host = 1;
  spreaded = true;

  // Make sure we call spread place once for each successfully placed task
  m.place_job(&pjob, cgi, "napali", false);
  fail_unless(called_spread_place == 1, "called %d", called_spread_place);

  num_for_host = 3;
  called_spread_place = 0;
  m.place_job(&pjob, cgi, "napali", false);
  fail_unless(called_spread_place == 3);

  // Now we're multiple instead of one so it should multiply the calls
  sockets = 0;
  my_placement_type = 3;
  numa_node_count = 2;
  num_for_host = 2;
  called_spread_place = 0;
  m.place_job(&pjob, cgi, "napali", false);
  fail_unless(called_spread_place == 4, "called %d times", called_spread_place);
  
  sockets = 2;
  my_placement_type = 2;
  numa_node_count = 0;
  num_for_host = 3;
  called_spread_place = 0;
  m.place_job(&pjob, cgi, "napali", false);
  fail_unless(called_spread_place == 6);
  
  called_spread_place = 0;
  sockets = 1;
  req_mem = 10;
  sock_mem = 5;
  num_for_host = 1;

  // the req is set to need 10 memory and each socket is set to only have 5, so even
  // though we set the req to need one socket, we should get two
  m.place_job(&pjob, cgi, "napali", false);
  fail_unless(called_spread_place == 2, "called %d times", called_spread_place);
  
  req_mem = 0;
  sock_mem = 0;
  }
END_TEST


START_TEST(test_displayAsString)
  {
  Machine           new_machine(1, 1, 1);
  std::stringstream out;

  new_machine.setMemory(2);
  new_machine.displayAsString(out);
  fail_unless(out.str() == "Machine (2KB)\n", out.str().c_str());
  }
END_TEST


START_TEST(test_basic_constructor)
  {
  Machine m(3, 1, 1);

  fail_unless(m.getTotalSockets() == 1);
  fail_unless(m.getAvailableSockets() == 1);
  fail_unless(m.getTotalCores() == 3);
  fail_unless(m.getTotalThreads() == 3);

  Machine m2(12, 2, 2);
  fail_unless(m2.getTotalSockets() == 2);
  fail_unless(m2.getAvailableSockets() == 2);
  fail_unless(m2.getTotalCores() == 12);
  fail_unless(m2.getTotalThreads() == 12);
  }
END_TEST


START_TEST(test_json_constructor)
  {
  const char *j1 = "\"node\":{\"socket\":{\"os_index\":12,\"numanode\":{\"os_index\":24,\"cores\":48-49,\"threads\":\"\",\"mem\"=1},\"numanode\":{\"os_index\":25,\"cores\":50-51,\"threads\":\"\",\"mem\"=1}},\"socket\":{\"os_index\":13,\"numanode\":{\"os_index\":26,\"cores\":52-53,\"threads\":\"\",\"mem\"=1},\"numanode\":{\"os_index\":26,\"cores\":54-55,\"threads\":\"\",\"mem\"=1}}}";
  const char *j2 = "\"node\":{\"socket\":{\"os_index\":0,\"numanode\":{\"os_index\":0,\"cores\":0-5,\"threads\":\"12-17\",\"mem\"=1024},\"numanode\":{\"os_index\":1,\"cores\":6-11,\"threads\":\"18-23\",\"mem\"=1024}}}";
  std::stringstream out;

  std::vector<std::string> valid_ids;

  Machine m1(j1, valid_ids);
  fail_unless(json_socket == 2, "%d times", json_socket);
  m1.displayAsJson(out, false);
  fail_unless(out.str() == "{\"node\":{,}}", out.str().c_str());
  fail_unless(m1.getTotalSockets() == 2);

  out.str("");
  Machine m2(j2, valid_ids);
  fail_unless(json_socket == 3, "%d times", json_socket);
  m2.displayAsJson(out, false);
  fail_unless(out.str() == "{\"node\":{}}", out.str().c_str());
  fail_unless(m2.getTotalSockets() == 1);
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
  m.store_device_on_appropriate_chip(d, true);
  fail_unless(called_store_pci == 1);
 
  // Since this is numa it should place on the sockets until it returns true,
  // which due to the scaffolding is never, so once per socket
  m.setIsNuma(true);
  m.store_device_on_appropriate_chip(d, false);
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

  rc = new_machine.getTotalSockets();
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
  cgroup_info cgi;
  Machine m;
  m.addSocket(2);
  job pjob;
  complete_req cr;
  pjob.ji_wattr[JOB_ATR_req_information].at_val.at_ptr = &cr;
  strcpy(pjob.ji_qs.ji_jobid, "1.napali");

  // Check how many tasks fit
  req r;
  num_tasks_fit = 8;
  fail_unless(m.how_many_tasks_can_be_placed(r) == 16);
  num_tasks_fit = 2;
  fail_unless(m.how_many_tasks_can_be_placed(r) == 4);

  // Make the job fit on one socket so we call place once per task
  called_place_task = 0;
  my_req_count = 2;
  num_for_host = 4;
  num_tasks_fit = 4;
  num_placed = 4;
  m.place_job(&pjob, cgi, "napali", false);
  fail_unless(called_place_task == 2, "Expected 2 calls but got %d", called_place_task);

  std::vector<std::string> job_ids;
  m.populate_job_ids(job_ids);
  fail_unless(job_ids.size() == 1);
  fail_unless(job_ids[0] == "1.napali", "id: '%s'", job_ids[0].c_str());

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
  m.place_job(&pjob, cgi, "napali", false);
  fail_unless(called_place_task == 2, "Expected 2 calls but got %d", called_place_task);

  num_tasks_fit = 0;
  num_placed = 0;
  my_req_count = 1;
  num_for_host = 1;
  socket_fit = true;
  partially_placed = true;
  called_partially_place = 0;
  called_fits_on_socket = 0;
  m.place_job(&pjob, cgi, "napali", false);
  fail_unless(called_partially_place == 1, "called %d", called_partially_place);
  fail_unless(called_fits_on_socket == 1);
  
  socket_fit = false;
  num_tasks_fit = 0;
  num_placed = 0;
  my_req_count = 1;
  num_for_host = 1;

  m.place_job(&pjob, cgi, "napali", false);
  fail_unless(called_partially_place == 2, "called %d", called_partially_place);
  fail_unless(called_fits_on_socket == 3, "called %d times", called_fits_on_socket);
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
  tcase_add_test(tc_core, test_basic_constructor);
  tcase_add_test(tc_core, test_check_if_possible);
  suite_add_tcase(s, tc_core);
  
  tc_core = tcase_create("test_place_and_free_job");
  tcase_add_test(tc_core, test_place_and_free_job);
  tcase_add_test(tc_core, test_store_pci_device_on_appropriate_chip);
  tcase_add_test(tc_core, test_spread_place);
  tcase_add_test(tc_core, test_place_all_execution_slots);
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
