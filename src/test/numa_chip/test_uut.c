#include "machine.hpp"
#include <stdio.h>
#include <stdlib.h>
#include <check.h>
#include "log.h"
#include "hwloc.h"
#include "pbs_error.h"
#include <sstream>

extern int recorded;

extern std::string my_placement_type;
extern std::string thread_type;

const char *alloc_json = "\"allocation\":{\"jobid\":\"666979[0].mgr.bwfor.privat\",\"cpus\":\"\",\"mem\":4203424,\"exclusive\":0,\"cores_only\":0}";



START_TEST(test_get_contiguous_thread_vector)
  {
  Chip             c;
  std::vector<int> list;

  // Make sure a memory only node doesn't crash
  fail_unless(c.getContiguousThreadVector(list, 34) == false);

  c.setId(0);
  c.setThreads(32);
  c.setCores(16);
  c.setMemory(6);
  c.setChipAvailable(true);
  for (int i = 0; i < 16; i++)
    c.make_core(i);

  fail_unless(c.getContiguousThreadVector(list, 34) == true);
  fail_unless(list.size() == 32);
  }
END_TEST


START_TEST(test_get_contiguous_core_vector)
  {
  Chip             c;
  std::vector<int> list;
  
  // Make sure a memory only node doesn't crash
  fail_unless(c.getContiguousCoreVector(list, 18) == false);

  c.setId(0);
  c.setThreads(32);
  c.setCores(16);
  c.setMemory(6);
  c.setChipAvailable(true);
  for (int i = 0; i < 16; i++)
    c.make_core(i);

  fail_unless(c.getContiguousCoreVector(list, 18) == true);
  fail_unless(list.size() == 16);
  }
END_TEST


START_TEST(test_initialize_cores_from_strings)
  {
  Chip        c;
  std::string cores;
  std::string threads;

  // Make sure this doesn't segfault for a memory only node
  c.initialize_cores_from_strings(cores, threads);
  fail_unless(c.getTotalCores() == 0);
  fail_unless(c.getTotalThreads() == 0);
  }
END_TEST


START_TEST(test_place_tasks_execution_slots)
  {
  const char        *jobid = "1.napali";

  allocation         a(jobid);
  Chip               c;
  c.setId(0);
  c.setThreads(32);
  c.setCores(16);
  c.setMemory(6);
  c.setChipAvailable(true);
  for (int i = 0; i < 16; i++)
    c.make_core(i);

  a.cores_only = true;

  c.place_tasks_execution_slots(2, 8, a, CORE);
  fail_unless(a.cpu_indices.size() == 2);
  fail_unless(a.cpu_place_indices.size() == 6);
  
  c.place_tasks_execution_slots(2, 8, a, CORE);
  fail_unless(a.cpu_indices.size() == 4);
  fail_unless(a.cpu_place_indices.size() == 12);
  fail_unless(c.free_core_count() == 0);

  Chip               c2;
  c2.setId(0);
  c2.setThreads(32);
  c2.setCores(16);
  c2.setMemory(6);
  c2.setChipAvailable(true);
  for (int i = 0; i < 16; i++)
    c2.make_core(i);

  allocation b(jobid);
  c2.place_tasks_execution_slots(0, 2, b, THREAD);
  fail_unless(b.cpu_indices.size() == 0);
  fail_unless(b.cpu_place_indices.size() == 2, "size is %u", b.cpu_place_indices.size());
  fail_unless(c2.free_core_count() == 15, "%d are free", c2.free_core_count());
  }
END_TEST


START_TEST(test_initialize_allocation)
  {
  Chip c;
  std::stringstream out;
  std::vector<std::string> valid_ids;


  // Make sure this doesn't last forever - we need to handle empty cpus: values
  c.initialize_allocation(strdup(alloc_json), valid_ids);
  c.displayAllocationsAsJson(out);
  fail_unless(out.str().find("666979[0].mgr.bwfor.privat") == std::string::npos);

  valid_ids.push_back("666979[0].mgr.bwfor.privat");
  c.initialize_allocation(strdup(alloc_json), valid_ids);
  c.displayAllocationsAsJson(out);

  fail_unless(out.str().find(alloc_json) != std::string::npos, "'%s' does not contain '%s'", out.str().c_str(), alloc_json);

  }
END_TEST


START_TEST(test_place_all_execution_slots)
  {
  const char        *jobid = "1.napali";
  req                r;

  allocation         a(jobid);
  Chip               c;
  c.setId(0);
  c.setThreads(32);
  c.setCores(16);
  c.setMemory(6);
  c.setChipAvailable(true);
  for (int i = 0; i < 16; i++)
    c.make_core(i);

  a.cores_only = true;
  c.place_all_execution_slots(r, a);
  fail_unless(a.cores == 16);
  fail_unless(a.threads == 32);
  c.free_task(jobid);

  allocation         a2(jobid);
  c.place_all_execution_slots(r, a2);
  fail_unless(a2.cores == 0);
  fail_unless(a2.threads == 32);
  }
END_TEST

START_TEST(test_spread_place_threads)
  {
  const char        *jobid = "1.napali";
  req                r;
  std::stringstream  out;

  allocation         a(jobid);
  Chip               c;
  c.setId(0);
  c.setThreads(32);
  c.setCores(16);
  c.setMemory(6);
  c.setChipAvailable(true);
  for (int i = 0; i < 16; i++)
    c.make_core(i);

  int                lprocs_remaining = 1;
  int                cores_remaining = 3;
  int                gpus_remaining = 0;
  int                mics_remaining = 0;
  a.place_type = exclusive_core;

  fail_unless(c.spread_place_threads(r, a, cores_remaining, lprocs_remaining, gpus_remaining, mics_remaining) == true);
  out.str("");
  c.displayAsJson(out, true);
  fail_unless(out.str() == "\"numanode\":{\"os_index\":0,\"cores\":\"0-15\",\"threads\":\"16-31\",\"mem\":6,\"allocation\":{\"jobid\":\"1.napali\",\"cpus\":\"16\",\"mem\":0,\"exclusive\":0,\"cores_only\":0}}", out.str().c_str());

  fail_unless(a.cpu_indices.size() == 1);
  fail_unless(a.cpu_indices[0] == 16);
  fail_unless(a.cpu_place_indices.size() == 2);
  fail_unless(a.cpu_place_indices[0] == 0);
  fail_unless(a.cpu_place_indices[1] == 1);

  c.free_task(jobid);
  a.cpu_indices.clear();
  a.cpu_place_indices.clear();

  lprocs_remaining = 3;
  cores_remaining = 5;

  fail_unless(c.spread_place_threads(r, a, cores_remaining, lprocs_remaining, gpus_remaining, mics_remaining) == true);

  fail_unless(a.cpu_indices.size() == 3);
  fail_unless(a.cpu_indices[0] == 0);
  fail_unless(a.cpu_indices[1] == 1);
  fail_unless(a.cpu_indices[2] != 3);
  fail_unless(a.cpu_place_indices.size() == 2);

  c.free_task(jobid);
  a.cpu_indices.clear();
  a.cpu_place_indices.clear();

  lprocs_remaining = 4;
  cores_remaining = 5;

  fail_unless(c.spread_place_threads(r, a, cores_remaining, lprocs_remaining, gpus_remaining, mics_remaining) == true);

  fail_unless(a.cpu_indices.size() == 4);
  fail_unless(a.cpu_indices[0] == 0);
  fail_unless(a.cpu_indices[1] != 1);
  fail_unless(a.cpu_indices[2] != 2 );
  fail_unless(a.cpu_indices[3] == 2);
  fail_unless(a.cpu_place_indices.size() == 1);
  fail_unless(a.cpu_place_indices[0] == 1);


  c.free_task(jobid);
  a.cpu_indices.clear();
  a.cpu_place_indices.clear();

  lprocs_remaining = 2;
  cores_remaining = 4;

  fail_unless(c.spread_place_cores(r, a, cores_remaining, lprocs_remaining, gpus_remaining, mics_remaining) == true);

  fail_unless(a.cpu_indices.size() == 2);
  fail_unless(a.cpu_indices[0] == 0);
  fail_unless(a.cpu_indices[1] == 2);
  fail_unless(a.cpu_place_indices.size() == 2);
  fail_unless(a.cpu_place_indices[0] == 1);
  fail_unless(a.cpu_place_indices[1] == 3);

  fail_unless(c.reserve_core(0,a) == false);
  fail_unless(c.reserve_core(1,a) == false);
  fail_unless(c.reserve_core(2,a) == false);
  fail_unless(c.reserve_core(3,a) == false);

  c.free_task(jobid);
  a.cpu_indices.clear();
  a.cpu_place_indices.clear();

  lprocs_remaining = 5;
  cores_remaining = 8;

  fail_unless(c.spread_place_threads(r, a, cores_remaining, lprocs_remaining, gpus_remaining, mics_remaining) == true);

  fail_unless(a.cpu_indices.size() == 5);
  fail_unless(a.cpu_indices[0] == 0);
  fail_unless(a.cpu_indices[1] == 1);
  fail_unless(a.cpu_indices[2] == 2);
  fail_unless(a.cpu_indices[3] == 3);
  fail_unless(a.cpu_indices[4] == 19);
  fail_unless(a.cpu_place_indices.size() == 3);
  fail_unless(a.cpu_place_indices[0] == 16);
  fail_unless(a.cpu_place_indices[1] == 17);
  fail_unless(a.cpu_place_indices[2] == 18);

  c.free_task(jobid);
  a.cpu_indices.clear();
  a.cpu_place_indices.clear();

  lprocs_remaining = 7;
  cores_remaining = 8;

  fail_unless(c.spread_place_threads(r, a, cores_remaining, lprocs_remaining, gpus_remaining, mics_remaining) == true);

  fail_unless(a.cpu_indices.size() == 7);
  fail_unless(a.cpu_indices[0] == 0);
  fail_unless(a.cpu_indices[1] == 16);
  fail_unless(a.cpu_indices[2] == 1);
  fail_unless(a.cpu_indices[3] == 2);
  fail_unless(a.cpu_indices[4] == 18);
  fail_unless(a.cpu_indices[5] == 3);
  fail_unless(a.cpu_indices[6] == 19);
  fail_unless(a.cpu_place_indices.size() == 1);
  fail_unless(a.cpu_place_indices[0] == 17);

  c.free_task(jobid);
  a.cpu_indices.clear();
  a.cpu_place_indices.clear();

  lprocs_remaining = 7;
  cores_remaining = 8;
  c.setChipAvailable(false);
  fail_unless(c.spread_place_threads(r, a, cores_remaining, lprocs_remaining, gpus_remaining, mics_remaining) == false);
 }
END_TEST



START_TEST(test_spread_place_cores)
  {
  const char        *jobid = "1.napali";
  req                r;
  std::stringstream  out;

  allocation         a(jobid);
  Chip               c;
  c.setId(0);
  c.setThreads(32);
  c.setCores(16);
  c.setMemory(6);
  c.setChipAvailable(true);
  for (int i = 0; i < 16; i++)
    c.make_core(i);

  int           lprocs_remaining = 1;
  int           cores_remaining = 3;
  int           gpus_remaining = 0;
  int           mics_remaining = 0;

  a.place_type = exclusive_core;

  fail_unless(c.spread_place_cores(r, a, cores_remaining, lprocs_remaining, gpus_remaining, mics_remaining) == true);
  out.str("");
  c.displayAsJson(out, true);
  fail_unless(out.str() == "\"numanode\":{\"os_index\":0,\"cores\":\"0-15\",\"threads\":\"16-31\",\"mem\":6,\"allocation\":{\"jobid\":\"1.napali\",\"cpus\":\"1\",\"mem\":0,\"exclusive\":0,\"cores_only\":1}}", out.str().c_str());

  fail_unless(c.reserve_core(0,a) == false);
  fail_unless(c.reserve_core(1,a) == false);
  fail_unless(c.reserve_core(2,a) == false);

  fail_unless(a.cpu_indices.size() == 1);
  fail_unless(a.cpu_indices[0] == 1);
  fail_unless(a.cpu_place_indices.size() == 2);
  fail_unless(a.cpu_place_indices[0] == 0);
  fail_unless(a.cpu_place_indices[1] == 2);

  c.free_task(jobid);
  a.cpu_indices.clear();
  a.cpu_place_indices.clear();

  lprocs_remaining = 3;
  cores_remaining = 5;

  fail_unless(c.spread_place_cores(r, a, cores_remaining, lprocs_remaining, gpus_remaining, mics_remaining) == true);

  fail_unless(a.cpu_indices.size() == 3);
  fail_unless(a.cpu_indices[0] == 0);
  fail_unless(a.cpu_indices[1] == 2);
  fail_unless(a.cpu_indices[2] == 4);
  fail_unless(a.cpu_place_indices.size() == 2);
  fail_unless(a.cpu_place_indices[0] == 1);
  fail_unless(a.cpu_place_indices[1] == 3);

  c.free_task(jobid);
  a.cpu_indices.clear();
  a.cpu_place_indices.clear();

  lprocs_remaining = 4;
  cores_remaining = 5;

  fail_unless(c.spread_place_cores(r, a, cores_remaining, lprocs_remaining, gpus_remaining, mics_remaining) == true);

  fail_unless(a.cpu_indices.size() == 4);
  fail_unless(a.cpu_indices[0] == 0);
  fail_unless(a.cpu_indices[1] == 1);
  fail_unless(a.cpu_indices[2] == 3);
  fail_unless(a.cpu_indices[3] == 4);
  fail_unless(a.cpu_place_indices.size() == 1);
  fail_unless(a.cpu_place_indices[0] == 2);


  c.free_task(jobid);
  a.cpu_indices.clear();
  a.cpu_place_indices.clear();

  lprocs_remaining = 2;
  cores_remaining = 4;

  fail_unless(c.spread_place_cores(r, a, cores_remaining, lprocs_remaining, gpus_remaining, mics_remaining) == true);

  fail_unless(a.cpu_indices.size() == 2);
  fail_unless(a.cpu_indices[0] == 0);
  fail_unless(a.cpu_indices[1] == 2);
  fail_unless(a.cpu_place_indices.size() == 2);
  fail_unless(a.cpu_place_indices[0] == 1);
  fail_unless(a.cpu_place_indices[1] == 3);

  fail_unless(c.reserve_core(0,a) == false);
  fail_unless(c.reserve_core(1,a) == false);
  fail_unless(c.reserve_core(2,a) == false);
  fail_unless(c.reserve_core(3,a) == false);

  c.free_task(jobid);
  a.cpu_indices.clear();
  a.cpu_place_indices.clear();

  lprocs_remaining = 5;
  cores_remaining = 8;

  fail_unless(c.spread_place_cores(r, a, cores_remaining, lprocs_remaining, gpus_remaining, mics_remaining) == true);

  fail_unless(a.cpu_indices.size() == 5);
  fail_unless(a.cpu_indices[0] == 0);
  fail_unless(a.cpu_indices[1] == 2);
  fail_unless(a.cpu_indices[2] == 4);
  fail_unless(a.cpu_indices[3] == 6);
  fail_unless(a.cpu_indices[4] == 7);
  fail_unless(a.cpu_place_indices.size() == 3);
  fail_unless(a.cpu_place_indices[0] == 1);
  fail_unless(a.cpu_place_indices[1] == 3);
  fail_unless(a.cpu_place_indices[2] == 5);

  c.free_task(jobid);
  a.cpu_indices.clear();
  a.cpu_place_indices.clear();

  lprocs_remaining = 7;
  cores_remaining = 8;

  fail_unless(c.spread_place_cores(r, a, cores_remaining, lprocs_remaining, gpus_remaining, mics_remaining) == true);

  fail_unless(a.cpu_indices.size() == 7);
  fail_unless(a.cpu_indices[0] == 0);
  fail_unless(a.cpu_indices[1] == 1);
  fail_unless(a.cpu_indices[2] == 2);
  fail_unless(a.cpu_indices[3] == 4);
  fail_unless(a.cpu_indices[4] == 5);
  fail_unless(a.cpu_indices[5] == 6);
  fail_unless(a.cpu_indices[6] == 7);
  fail_unless(a.cpu_place_indices.size() == 1);
  fail_unless(a.cpu_place_indices[0] == 3);

  c.free_task(jobid);
  a.cpu_indices.clear();
  a.cpu_place_indices.clear();

  lprocs_remaining = 7;
  cores_remaining = 8;
  c.setChipAvailable(false);
  fail_unless(c.spread_place_cores(r, a, cores_remaining, lprocs_remaining, gpus_remaining, mics_remaining) == false);
 }
END_TEST



START_TEST(test_spread_place)
  {
  const char        *jobid = "1.napali";
  const char        *jobid2 = "2.scadrial";
  req                r;
  std::stringstream  out;

  allocation         a(jobid);
  allocation         remaining;
  allocation         remainder;
  Chip               c;
  c.setId(0);
  c.setThreads(32);
  c.setCores(16);
  c.setMemory(6);
  c.setChipAvailable(true);
  c.set_gpus(2);
  for (int i = 0; i < 16; i++)
    c.make_core(i);

  a.place_type = exclusive_chip;

  // Make sure we get 4 evenly spread cores
  remaining.cpus = 4;
  fail_unless(c.spread_place(r, a, remaining, remainder) == true);
  out.str("");
  c.displayAsJson(out, true);
  fail_unless(out.str() == "\"numanode\":{\"os_index\":0,\"cores\":\"0-15\",\"threads\":\"16-31\",\"mem\":6,\"gpus\":\"0-1\",\"allocation\":{\"jobid\":\"1.napali\",\"cpus\":\"0,4,8,12\",\"mem\":0,\"exclusive\":3,\"cores_only\":1}}", out.str().c_str());

  // Make sure we do not place another task on this chip before freeing the old one
  fail_unless(c.reserve_core(0, a) == false);
  remaining.cpus = 4;
  remainder.cpus = 1;
  remaining.gpus = 1;
  remainder.gpus = 1;
  fail_unless(c.spread_place(r, a, remaining, remainder) == false);
  fail_unless(c.has_socket_exclusive_allocation() == false);
  c.free_task(jobid);
  
  // Check that we place 5 cores correctly
  fail_unless(c.spread_place(r, a, remaining, remainder) == true);
  fail_unless(remainder.cpus == 0, "remainder = %d", remainder.cpus);
  fail_unless(remainder.gpus == 0, "remainder = %d", remainder.gpus);
  fail_unless(remaining.gpus == 0, "remainder = %d", remaining.gpus);
  fail_unless(c.reserve_core(0, a) == false);
  fail_unless(c.reserve_core(3, a) == false);
  fail_unless(c.reserve_core(6, a) == false);
  fail_unless(c.reserve_core(10, a) == false);
  fail_unless(c.reserve_core(13, a) == false);

  c.free_task(jobid);
  // Make sure we get core 0 if we request 0 + 1 in the remainder
  remaining.cpus = 0;
  remainder.cpus = 1;
  fail_unless(c.spread_place(r, a, remaining, remainder) == true);
  fail_unless(c.reserve_core(0, a) == false);
  fail_unless(remainder.cpus == 0);
 
  // Check what happens we we place an empty set on the chip
  c.free_task(jobid);
  a.place_type = exclusive_socket;
  remaining.cpus = 0;
  fail_unless(c.spread_place(r, a, remaining, remainder) == true);
  fail_unless(c.getAvailableCores() == 0);
  fail_unless(c.getAvailableThreads() == 0);
  fail_unless(c.has_socket_exclusive_allocation() == true);

  c.free_task(jobid);
  allocation a2(jobid2);
  a2.place_type = exclusive_chip;
  remaining.cpus = 40;
  fail_unless(c.spread_place(r, a2, remaining, remainder) == false);
  remaining.cpus = 18;
  fail_unless(c.spread_place(r, a2, remaining, remainder) == true);
  fail_unless(c.getAvailableCores() == 0);
  fail_unless(c.getAvailableThreads() == 0);
  c.free_task(jobid2);
  
  req r2;
  allocation a3;
  const char *host = "scadrial";
  r2.set_value("lprocs", "2", false);
  r2.set_value("memory", "1kb", false);
  int tasks = c.place_task(r2, a3, 1, host);
  fail_unless(tasks == 1);
  // We shouldn't place anything in spread place unless we're completely free
  remaining.cpus = 18;
  fail_unless(c.spread_place(r, a2, remaining, remainder) == false);
  }
END_TEST


START_TEST(test_basic_constructor)
  {
  int remainder = 0;
  int pn_remainder = 0;
  Chip c(4, remainder, pn_remainder);
  c.setMemory(20);

  fail_unless(c.getTotalCores() == 4);
  fail_unless(c.getTotalThreads() == 4);
  fail_unless(c.getAvailableCores() == 4);
  fail_unless(c.getAvailableThreads() == 4);
  fail_unless(c.getMemory() == 20);
  fail_unless(c.getAvailableMemory() == 20);

  remainder = 1;
  pn_remainder = 1;
  Chip c2(4, remainder, pn_remainder);
  fail_unless(c2.getTotalCores() == 6);
  fail_unless(c2.getTotalThreads() == 6);
  fail_unless(c2.getAvailableCores() == 6);
  fail_unless(c2.getAvailableThreads() == 6);
  fail_unless(remainder == 0);
  fail_unless(pn_remainder == 0);
  }
END_TEST


START_TEST(test_json_constructor)
  {
  const char *j1 ="\"numanode\":{\"os_index\":1,\"cores\":\"0-3\",\"threads\":\"4-7\",\"mem\":3221225472,\"mics\":\"0-1\"}";
  const char *j2 ="\"numanode\":{\"os_index\":0,\"cores\":\"0-7\",\"threads\":\"8-15\",\"mem\":32,\"gpus\":\"0-3\"}";
  const char *j3 ="\"numanode\":{\"os_index\":12,\"cores\":\"0-5\",\"threads\":\"\",\"mem\":1024,\"gpus\":\"0-1\",\"mics\":\"2-3\"}";
  const char *j4 ="\"numanode\":{\"os_index\":0,\"cores\":\"0-15\",\"threads\":\"16-31\",\"mem\":1024,\"gpus\":\"0-1\",\"mics\":\"2-3\",\"allocation\":{\"jobid\":\"0.napali\",\"cpus\":\"0-3,16-19\",\"mem\":0,\"exclusive\":0,\"cores_only\":0,\"gpus\":\"0-1\"},\"allocation\":{\"jobid\":\"1.napali\",\"cpus\":\"4-15\",\"mem\":0,\"exclusive\":0,\"cores_only\":1,\"mics\":\"2-3\"}}";
  const char *j5 ="\"numanode\":{\"os_index\":0,\"cores\":\"0-15\",\"threads\":\"16-31\",\"mem\":1024,\"gpus\":\"0-1\",\"mics\":\"2-3\",\"allocation\":{\"jobid\":\"0.napali\",\"cpus\":\"0\",\"mem\":10,\"exclusive\":3,\"cores_only\":0}}";
  const char *j6 ="\"numanode\":{\"os_index\":0,\"cores\":\"0-15\",\"threads\":\"16-31\",\"mem\":1024,\"gpus\":\"0-1\",\"mics\":\"2-3\",\"allocation\":{\"jobid\":\"0.napali\",\"cpus\":\"0\",\"mem\":10,\"exclusive\":0,\"cores_only\":0},\"allocation\":{\"jobid\":\"1.napali\",\"cpus\":\"1-4\",\"mem\":100,\"exclusive\":0,\"cores_only\":0},\"allocation\":{\"jobid\":\"0.napali\",\"cpus\":\"5\",\"mem\":10,\"exclusive\":0,\"cores_only\":0},\"allocation\":{\"jobid\":\"2.napali\",\"cpus\":\"6-7\",\"mem\":1000,\"exclusive\":0,\"cores_only\":0}}";

  std::stringstream o1;
  std::stringstream o2;
  std::stringstream o3;

  std::vector<std::string> valid_ids;
  valid_ids.push_back("1.napali");
  valid_ids.push_back("0.napali");
  valid_ids.push_back("2.napali");
  
  Chip c1(j1, valid_ids);
  fail_unless(c1.get_id() == 1);
  fail_unless(c1.getTotalCores() == 4, "total %d", c1.getTotalCores());
  fail_unless(c1.getTotalThreads() == 8);
  fail_unless(c1.get_available_mics() == 2, "%d mics", c1.get_available_mics());
  fail_unless(c1.get_available_gpus() == 0);
  c1.displayAsJson(o1, false);
  fail_unless(o1.str() == j1, o1.str().c_str());

  Chip c2(j2, valid_ids);
  fail_unless(c2.get_id() == 0);
  fail_unless(c2.getTotalCores() == 8);
  fail_unless(c2.getTotalThreads() == 16);
  fail_unless(c2.get_available_mics() == 0);
  fail_unless(c2.get_available_gpus() == 4);
  c2.displayAsJson(o2, false);
  fail_unless(o2.str() == j2, o2.str().c_str());

  Chip c3(j3, valid_ids);
  fail_unless(c3.get_id() == 12);
  fail_unless(c3.getTotalCores() == 6);
  fail_unless(c3.getTotalThreads() == 6);
  fail_unless(c3.get_available_mics() == 2);
  fail_unless(c3.get_available_gpus() == 2);
  c3.displayAsJson(o3, false);
  fail_unless(o3.str() == j3, o3.str().c_str());

  Chip c4(j4, valid_ids);
  fail_unless(c4.get_id() == 0);
  fail_unless(c4.getTotalCores() == 16);
  fail_unless(c4.getTotalThreads() == 32);
  fail_unless(c4.getMemory() == 1024);
  fail_unless(c4.getAvailableMemory() == 1024);
  // Should be 4 because no one is specifically requesting cores
  fail_unless(c4.getAvailableCores() == 4, "%d available cores", c4.getAvailableCores());
  fail_unless(c4.getAvailableThreads() == 0);
  fail_unless(c4.get_available_mics() == 0, "%d available mics", c4.get_available_mics());
  fail_unless(c4.get_available_gpus() == 0, "%d available gpus", c4.get_available_gpus());
  
  Chip c5(j5, valid_ids);
  fail_unless(c5.get_id() == 0);
  fail_unless(c5.getTotalCores() == 16);
  fail_unless(c5.getTotalThreads() == 32);
  fail_unless(c5.getMemory() == 1024);
  // Should be 0 because we're exlcusive chip
  fail_unless(c5.getAvailableCores() == 0, "%d available cores", c5.getAvailableCores());
  fail_unless(c5.getAvailableThreads() == 0);
  fail_unless(c5.get_available_mics() == 0, "%d available mics", c5.get_available_mics());
  fail_unless(c5.get_available_gpus() == 0, "%d available gpus", c5.get_available_gpus());

  Chip c6(j6, valid_ids);
  std::stringstream o6;
  c6.displayAsJson(o6, true);
  fail_unless(o6.str() == j6, o6.str().c_str());
  c6.free_task("0.napali");
  o6.str("");
  c6.displayAsJson(o6, true);
  fail_unless(o6.str() == "\"numanode\":{\"os_index\":0,\"cores\":\"0-15\",\"threads\":\"16-31\",\"mem\":1024,\"gpus\":\"0-1\",\"mics\":\"2-3\",\"allocation\":{\"jobid\":\"1.napali\",\"cpus\":\"1-4\",\"mem\":100,\"exclusive\":0,\"cores_only\":0},\"allocation\":{\"jobid\":\"2.napali\",\"cpus\":\"6-7\",\"mem\":1000,\"exclusive\":0,\"cores_only\":0}}", o6.str().c_str());
  }
END_TEST


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



  rc = new_chip.getMemory();
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

  c.setMemory(2);
  c.setId(0);
  c.displayAsString(out);
  fail_unless(out.str() == "    Chip 0 (2KB)\n", out.str().c_str());
  }
END_TEST


START_TEST(test_how_many_tasks_fit)
  {
  req r;
  r.set_value("lprocs", "2", false);
  r.set_value("memory", "1kb", false);

  Chip c;
  c.setThreads(12);
  c.setMemory(20);
  c.setChipAvailable(true);

  // test against threads
  int tasks = c.how_many_tasks_fit(r, 0);
  fail_unless(tasks == 6, "%d tasks fit, expected 6", tasks);

  // Now memory should be the limiting factor
  c.setMemory(5);
  tasks = c.how_many_tasks_fit(r, 0);
  fail_unless(tasks == 5, "%d tasks fit, expected 5", tasks);
  
  // Make sure we handle memory per task
  req rpt;
  rpt.set_value("lprocs", "1", false);
  rpt.set_value("total_memory", "5kb", false);
  tasks = c.how_many_tasks_fit(rpt, 0);
  fail_unless(tasks == 1, "%d tasks fit, expected 1", tasks);

  rpt.set_value("task_count", "5", false);
  rpt.set_value("total_memory", "5kb", false);
  tasks = c.how_many_tasks_fit(rpt, 0);
  fail_unless(tasks == 5, "%d tasks fit, expected 5", tasks);

  thread_type = use_cores;
  // Cores are currently 0
  tasks = c.how_many_tasks_fit(r, 0);
  fail_unless(tasks == 0, "%d tasks fit, expected 0", tasks);

  // Make sure we can do fractional fits
  c.setCores(1);
  for (int i = 0; i < 1; i++)
    c.make_core(i);
  fail_unless(c.how_many_tasks_fit(r, 0) == 0.5);

  c.setCores(2);
  for (int i = 1; i < 2; i++)
    c.make_core(i);
  tasks = c.how_many_tasks_fit(r, 0);
  fail_unless(tasks == 1, "%d tasks fit, expected 1", tasks);

  // make sure that we can handle a request without memory
  req r2;
  r2.set_value("lprocs", "2", false);
  c.setCores(10);
  for (int i = 2; i < 10; i++)
    c.make_core(i);
  fail_unless(c.how_many_tasks_fit(r2, 0) == 5);

  // make sure we account for gpus and mics
  r2.set_value("gpus", "1", false);
  fail_unless(c.how_many_tasks_fit(r2, 0) == 0);
  r2.set_value("gpus", "0", false);
  fail_unless(c.how_many_tasks_fit(r2, 0) == 5);
  r2.set_value("mics", "1", false);
  fail_unless(c.how_many_tasks_fit(r2, 0) == 0);

  Chip large_mem;
  large_mem.setThreads(8);
  large_mem.setMemory(128849018880); // 256 GB
  large_mem.setCores(8);
  large_mem.setChipAvailable(true);
  for (int i = 0; i < 8; i++)
    large_mem.make_core(i);
  req r3;
  r3.set_value("lprocs", "8", false);
  r3.set_value("memory", "128849018880kb", false); // 256 gb
  float fitting_tasks = large_mem.how_many_tasks_fit(r3, exclusive_none);
  fail_unless(fitting_tasks == 1.0);
  }
END_TEST


START_TEST(test_exclusive_place)
  {
  const char *jobid = "1.napali";
  const char *host = "napali";
  req r;
  r.set_value("lprocs", "2", false);
  r.set_value("memory", "1kb", false);

  std::vector<std::string> valid_id;
  valid_id.push_back(jobid);

  allocation a(jobid);
  a.place_type = exclusive_chip;

  Chip c;
  c.setId(0);
  c.setThreads(32);
  c.setCores(16);
  c.setMemory(6);
  c.setChipAvailable(true);
  for (int i = 0; i < 16; i++)
    c.make_core(i);

  thread_type = use_cores;
  fail_unless(c.how_many_tasks_fit(r, 0) == 6);
  my_placement_type = place_numa_node;
  int num_fit = c.how_many_tasks_fit(r, exclusive_chip);
  fail_unless(num_fit == 1, "Expected 1, got %d", num_fit);
  recorded = 0;
  int tasks = c.place_task(r, a, 1, host);
  fail_unless(tasks == 1);
  fail_unless(recorded == 1);
  
  std::stringstream out;
  c.displayAsJson(out, true);
  fail_unless(out.str() == "\"numanode\":{\"os_index\":0,\"cores\":\"0-15\",\"threads\":\"16-31\",\"mem\":6,\"allocation\":{\"jobid\":\"1.napali\",\"cpus\":\"0-1\",\"mem\":1,\"exclusive\":3,\"cores_only\":1}}", out.str().c_str());
  my_placement_type.clear();
  
  a.place_type = exclusive_none;
  recorded = 0;
  tasks = c.place_task(r, a, 5, host);
  fail_unless(c.how_many_tasks_fit(r, 0) == 0);
  fail_unless(tasks == 0);
  fail_unless(recorded == 0);
  c.free_task(jobid);
  
  allocation a2(jobid);
  a2.place_type = exclusive_none;
  fail_unless(c.how_many_tasks_fit(r, 0) == 6);
  tasks = c.place_task(r, a2, 6, host);
  out.str("");
  c.displayAsJson(out, true);
  fail_unless(out.str() == "\"numanode\":{\"os_index\":0,\"cores\":\"0-15\",\"threads\":\"16-31\",\"mem\":6,\"allocation\":{\"jobid\":\"1.napali\",\"cpus\":\"0-11\",\"mem\":6,\"exclusive\":0,\"cores_only\":1}}", out.str().c_str());
  fail_unless(tasks == 6);
  fail_unless(recorded == 6);

  Chip c2(out.str(), valid_id);
  std::stringstream o2;
  c2.displayAsJson(o2, true);
  fail_unless(o2.str() == out.str(), o2.str().c_str());
  fail_unless(c2.getTotalCores() == 16, "%d total cores", c2.getTotalCores());
  fail_unless(c2.getAvailableCores() == 4, "%d cores available", c2.getAvailableCores());
  fail_unless(c2.getTotalThreads() == 32, "%d total threads", c2.getTotalThreads());
  fail_unless(c2.getMemory() == 6, "%d total memory", (int)c2.getMemory());
  fail_unless(c2.getAvailableMemory() == 0);
  fail_unless(c2.getAvailableThreads() == 8, "%d available", c2.getAvailableThreads());

  Chip c3;
  c3.setId(0);
  c3.setThreads(32);
  c3.setCores(16);
  c3.setMemory(32);
  c3.setChipAvailable(true);
  for (int i = 0; i < 16; i++)
    c3.make_core(i);
  
  req r2;
  allocation a3(jobid);
  a3.place_type = exclusive_none;
  r2.set_value("lprocs", "32", false);
  thread_type.clear();
  recorded = 0;
  tasks = c3.place_task(r2, a3, 1, host);
  fail_unless(tasks == 1, "%d tasks", tasks);
  fail_unless(recorded == 1);
  out.str("");
  c3.displayAsJson(out, true);
  fail_unless(out.str() == "\"numanode\":{\"os_index\":0,\"cores\":\"0-15\",\"threads\":\"16-31\",\"mem\":32,\"allocation\":{\"jobid\":\"1.napali\",\"cpus\":\"0,16,1,17,2,18,3,19,4,20,5,21,6,22,7,23,8,24,9,25,10,26,11,27,12,28,13,29,14,30,15,31\",\"mem\":0,\"exclusive\":0,\"cores_only\":0}}", out.str().c_str());

  Chip c4;
  c4.setId(0);
  c4.setThreads(16);
  c4.setCores(8);
  c4.setMemory(16);
  c4.setChipAvailable(true);
  for (int i = 0; i < 8; i++)
    c4.make_core(i);
  thread_type = use_cores;

  req r3;
  r3.set_value("lprocs", "16", false);
  allocation remaining(r3);
  allocation master("2.napali");

  recorded = 0;
  c4.partially_place_task(remaining, master);
  out.str("");
  c4.displayAsJson(out, true);
  fail_unless(out.str() == "\"numanode\":{\"os_index\":0,\"cores\":\"0-7\",\"threads\":\"16-23\",\"mem\":16,\"allocation\":{\"jobid\":\"2.napali\",\"cpus\":\"0-7\",\"mem\":0,\"exclusive\":0,\"cores_only\":1}}", out.str().c_str());
  // The reservations are recorded differently for partial placing
  fail_unless(recorded == 0, "actual %d", recorded);

  // Make sure exclusive socket works across restarts
  allocation a4(jobid);
  a4.place_type = exclusive_socket;
  c.free_task(jobid);
  recorded = 0;
  tasks = c.place_task(r, a4, 1, host);
  fail_unless(tasks == 1);
  fail_unless(recorded == 1);
  out.str("");
  c.displayAsJson(out, true);
  fail_unless(out.str() == "\"numanode\":{\"os_index\":0,\"cores\":\"0-15\",\"threads\":\"16-31\",\"mem\":6,\"allocation\":{\"jobid\":\"1.napali\",\"cpus\":\"0-1\",\"mem\":1,\"exclusive\":2,\"cores_only\":1}}", out.str().c_str());

  Chip copy_exclusive_socket(out.str(), valid_id);
  fail_unless(copy_exclusive_socket.getAvailableThreads() == 0);
  fail_unless(copy_exclusive_socket.getAvailableCores() == 0);

  allocation to_add("1.napali");
  to_add.cpu_indices.push_back(2);
  to_add.cpu_indices.push_back(3);
  c.aggregate_allocation(to_add);
  out.str("");
  c.displayAsJson(out, true);
  fail_unless(out.str() == "\"numanode\":{\"os_index\":0,\"cores\":\"0-15\",\"threads\":\"16-31\",\"mem\":6,\"allocation\":{\"jobid\":\"1.napali\",\"cpus\":\"0-3\",\"mem\":1,\"exclusive\":2,\"cores_only\":1}}", out.str().c_str());

  allocation one_more("2.napali");
  one_more.cpu_indices.push_back(9);
  one_more.mem_indices.push_back(2);
  c.aggregate_allocation(one_more);
  out.str("");
  c.displayAsJson(out, true);
  fail_unless(out.str() == "\"numanode\":{\"os_index\":0,\"cores\":\"0-15\",\"threads\":\"16-31\",\"mem\":6,\"allocation\":{\"jobid\":\"1.napali\",\"cpus\":\"0-3\",\"mem\":1,\"exclusive\":2,\"cores_only\":1},\"allocation\":{\"jobid\":\"2.napali\",\"cpus\":\"9\",\"mem\":0,\"exclusive\":0,\"cores_only\":0}}", out.str().c_str());
  }
END_TEST


START_TEST(test_partial_place)
  {
  Chip c;
  c.setId(0);
  c.setThreads(24);
  c.setCores(12);
  c.setMemory(6);
  c.setChipAvailable(true);
  for (int i = 0; i < 12; i++)
    c.make_core(i);

  allocation remaining;
  allocation master("1.napali");

  remaining.cpus = 6;
  remaining.memory = 2;

  c.partially_place_task(remaining, master);
  fail_unless(remaining.memory == 0);
  fail_unless(remaining.cpus == 0);

  // use the rest of the cpus
  remaining.cpus = 28;
  remaining.memory = 3;
  allocation m2("2.napali");
  c.partially_place_task(remaining, m2);
  fail_unless(remaining.memory == 0);
  fail_unless(remaining.cpus == 10, "%d remaining", remaining.cpus);

  allocation m3("3.napali");
  remaining.cpus = 4;
  remaining.memory = 6;

  // Make sure we'll still use that memory even without cpus
  c.partially_place_task(remaining, m3);
  fail_unless(remaining.cpus == 4);
  fail_unless(remaining.memory == 5);
  
  c.free_task("1.napali");
  c.free_task("2.napali");
  c.free_task("3.napali");

  remaining.cores_only = true;
  remaining.cpus = 13;
  remaining.memory = 12;
  allocation master2("1.napali");
  c.partially_place_task(remaining, master2);
  fail_unless(remaining.cpus == 1, "cpus %d", remaining.cpus);
  fail_unless(remaining.memory == 6);

  }
END_TEST


START_TEST(test_reserve_accelerators)
  {
  Chip c;
  c.setId(0);
  c.setThreads(24);
  c.setCores(12);
  c.setMemory(6);
  c.setChipAvailable(true);
  for (int i = 0; i < 12; i++)
    c.make_core(i);

  for (int i = 0; i < 2; i++)
    {
    PCI_Device d;
    fail_unless(c.store_pci_device_appropriately(d, true) == true);
    }

  fail_unless(c.get_available_mics() == 1);
  fail_unless(c.get_available_gpus() == 1);

  c.set_cpuset("0");

  for (int i = 0; i < 2; i++)
    {
    PCI_Device d;
    fail_unless(c.store_pci_device_appropriately(d, false) == true);
    }

  fail_unless(c.get_available_mics() == 2);
  fail_unless(c.get_available_gpus() == 2);

  // Make sure a non-matching cpuset doesn't store the pci devices
  c.set_cpuset("1");
  for (int i = 0; i < 2; i++)
    {
    PCI_Device d;
    c.store_pci_device_appropriately(d, false);
    }
  fail_unless(c.get_available_mics() == 2);
  fail_unless(c.get_available_gpus() == 2);

  allocation remaining;
  allocation a;

  remaining.mics = 2;
  remaining.gpus = 2;

  c.place_accelerators(remaining, a);
  fail_unless(c.get_available_mics() == 0);
  fail_unless(c.get_available_gpus() == 0);
  c.free_accelerators(a);
  fail_unless(c.get_available_mics() == 2);
  fail_unless(c.get_available_gpus() == 2);
  }
END_TEST


START_TEST(test_place_and_free_task)
  {
  const char *jobid = "1.napali";
  const char *host = "napali";
  req r;
  r.set_value("lprocs", "2", false);
  r.set_value("memory", "1kb", false);

  allocation a(jobid);

  Chip c;
  c.setId(0);
  c.setThreads(24);
  c.setCores(12);
  c.setMemory(6);
  c.setChipAvailable(true);
  for (int i = 0; i < 12; i++)
    c.make_core(i);
 
  thread_type = use_cores;
  // fill the node's memory
  int tasks = c.place_task(r, a, 6, host);
  fail_unless(tasks == 6, "Placed only %d tasks, expected 6", tasks);
  fail_unless(a.mem_indices.size() > 0);
  fail_unless(a.mem_indices[0] == 0);

  // Memory should be full now
  tasks = c.place_task(r, a, 6, host);
  fail_unless(tasks == 0, "Placed %d tasks, expected 0", tasks);

  // make sure we can free and replace
  c.free_task(jobid);
  a.clear();
  a.jobid = jobid;
  tasks = c.place_task(r, a, 6, host);
  fail_unless(tasks == 6, "Placed only %d tasks, expected 6", tasks);
  c.free_task(jobid);
  fail_unless(c.getAvailableCores() == 12);
  fail_unless(c.getAvailableThreads() == 24);
  
  // Now place by thread
  my_placement_type = "";
  thread_type = "";
  c.setMemory(40);

  // Fill up the threads with multiple jobs
  const char *jobid2 = "2.napali";
  const char *jobid3 = "3.napali";
  a.clear();
  a.jobid = jobid;
  tasks = c.place_task(r, a, 6, host);
  fail_unless(tasks == 6, "Expected 6 but placed %d", tasks);
  a.clear();
  a.jobid = jobid2;
  tasks = c.place_task(r, a, 3, host);
  fail_unless(tasks == 3, "Expected 3 but placed %d", tasks);
  a.clear();
  a.jobid = jobid3;
  tasks = c.place_task(r, a, 3, host);
  fail_unless(tasks == 3, "Expected 3 but placed %d", tasks);
  
  // Make sure we're full
  fail_unless(c.getAvailableCores() == 12, "%d available", c.getAvailableCores());
  fail_unless(c.getAvailableThreads() == 0);
  a.clear();
  tasks = c.place_task(r, a, 1, host);
  fail_unless(tasks == 0);

  // Make sure we free correctly
  fail_unless(c.free_task(jobid3) == false);
  fail_unless(c.getAvailableCores() == 12, "%d available", c.getAvailableCores());
  int threads = c.getAvailableThreads();
  fail_unless(threads == 6, "Expected 6 threads but got %d", threads);

  // Make sure a repeat free does nothing
  fail_unless(c.free_task(jobid3) == false);
  fail_unless(c.getAvailableCores() == 12);
  fail_unless(c.getAvailableThreads() == 6);
  
  fail_unless(c.free_task(jobid2) == false);
  fail_unless(c.getAvailableCores() == 12, "%d available", c.getAvailableCores());
  fail_unless(c.getAvailableThreads() == 12);
  
  // We should be free now
  fail_unless(c.free_task(jobid) == true);
  fail_unless(c.getAvailableCores() == 12, "%d available", c.getAvailableCores());
  fail_unless(c.getAvailableThreads() == 24);
  }
END_TEST


Suite *numa_socket_suite(void)
  {
  Suite *s = suite_create("numa_socket test suite methods");
  TCase *tc_core = tcase_create("test_initializeChip");
  tcase_add_test(tc_core, test_initializeChip);
  tcase_add_test(tc_core, test_how_many_tasks_fit);
  tcase_add_test(tc_core, test_partial_place);
  tcase_add_test(tc_core, test_reserve_accelerators);
  tcase_add_test(tc_core, test_spread_place_threads);
  tcase_add_test(tc_core, test_spread_place_cores);
  tcase_add_test(tc_core, test_spread_place);
  tcase_add_test(tc_core, test_initialize_cores_from_strings);
  suite_add_tcase(s, tc_core);
  
  tc_core = tcase_create("test_displayAsString");
  tcase_add_test(tc_core, test_displayAsString);
  tcase_add_test(tc_core, test_place_and_free_task);
  tcase_add_test(tc_core, test_exclusive_place);
  tcase_add_test(tc_core, test_json_constructor);
  tcase_add_test(tc_core, test_basic_constructor);
  tcase_add_test(tc_core, test_place_all_execution_slots);
  tcase_add_test(tc_core, test_initialize_allocation);
  tcase_add_test(tc_core, test_place_tasks_execution_slots);
  suite_add_tcase(s, tc_core);

  tc_core = tcase_create("test_get_contiguous_core_vector");
  tcase_add_test(tc_core, test_get_contiguous_core_vector);
  tcase_add_test(tc_core, test_get_contiguous_thread_vector);
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
