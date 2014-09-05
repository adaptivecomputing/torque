#include <stdio.h>
#include <stdlib.h>
#include <check.h>
#include "log.h"
#include "machine.hpp"
#include "hwloc.h"
#include "pbs_error.h"


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



  rc = new_socket.getMemoryInBytes();
  fail_unless(rc != 0, "failed to get socket memory");

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




START_TEST(test_two)
  {
  }
END_TEST




Suite *numa_socket_suite(void)
  {
  Suite *s = suite_create("numa_socket test suite methods");
  TCase *tc_core = tcase_create("test_initializeSocket");
  tcase_add_test(tc_core, test_initializeSocket);
  suite_add_tcase(s, tc_core);
  
  tc_core = tcase_create("test_two");
  tcase_add_test(tc_core, test_two);
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
