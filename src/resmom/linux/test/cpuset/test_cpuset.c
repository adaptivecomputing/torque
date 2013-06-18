#include "license_pbs.h" /* See here for the software license */
#include <stdlib.h>
#include <stdio.h>
#include <set>
#include <hwloc.h>

#include "pbs_job.h"
#include "test_cpuset.h"
#include "pbs_error.h"

void get_memory_requested_and_reserved(long long *requested, long long *reserved, std::set<int> &current_mems, job *pjob, hwloc_bitmap_t job_mems);
void add_extra_memory_nodes_if_needed(long long requested, long long reserved, hwloc_bitmap_t job_mems, hwloc_bitmap_t torque_root_mems, std::set<int> current_mem_ids);
int hwloc_bitmap_displaylist(char *buf, size_t buflen, hwloc_bitmap_t map);

extern bool no_memory;

START_TEST(get_memory_requested_and_reserved_test)
  {
  long long      mem_requested;
  long long      mem_reserved;
  std::set<int>  current_mem_ids;
  job            pjob;
  hwloc_bitmap_t job_mems = hwloc_bitmap_alloc();
  hwloc_bitmap_set(job_mems, 0);

  /* test to see if this segfaults */
  get_memory_requested_and_reserved(&mem_requested, NULL, current_mem_ids, &pjob, job_mems);

  no_memory = true;
  get_memory_requested_and_reserved(&mem_requested, &mem_reserved, current_mem_ids, &pjob, job_mems);
  fail_unless(mem_requested == 0);
  fail_unless(mem_reserved == 0);

  no_memory = false;
  get_memory_requested_and_reserved(&mem_requested, &mem_reserved, current_mem_ids, &pjob, job_mems);
  fail_unless(mem_requested > mem_reserved);
  }
END_TEST

START_TEST(add_extra_memory_nodes_if_needed_test)
  {
  long long      mem_requested;
  long long      mem_reserved;
  std::set<int>  current_mem_ids;
  hwloc_bitmap_t job_mems = hwloc_bitmap_alloc();
  hwloc_bitmap_t torque_root_mems = hwloc_bitmap_alloc();
  char           buf[1024];
  hwloc_bitmap_set(job_mems, 0);
  current_mem_ids.insert(0);
  hwloc_bitmap_set(torque_root_mems, 0);
  hwloc_bitmap_set(torque_root_mems, 1);
  mem_requested = 16 * 1024;
  mem_requested *= 1024;
  mem_requested *= 1024;
  mem_reserved = 15 * 1024;
  mem_reserved *= 1024;
  mem_reserved *= 1024;

  add_extra_memory_nodes_if_needed(mem_requested, mem_reserved, job_mems, torque_root_mems, current_mem_ids);
  fail_unless(hwloc_bitmap_weight(job_mems) == 2);
  
  hwloc_bitmap_displaylist(buf, sizeof(buf), job_mems);
  fail_unless(strchr(buf, '0') != NULL);
  fail_unless(strchr(buf, '1') != NULL);
  }
END_TEST

Suite *cpuset_suite(void)
  {
  Suite *s = suite_create("cpuset_suite methods");
  TCase *tc_core = tcase_create("memory_tests");
  tcase_add_test(tc_core, get_memory_requested_and_reserved_test);
  tcase_add_test(tc_core, add_extra_memory_nodes_if_needed_test);
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
  sr = srunner_create(cpuset_suite());
  srunner_set_log(sr, "cpuset_suite.log");
  srunner_run_all(sr, CK_NORMAL);
  number_failed = srunner_ntests_failed(sr);
  srunner_free(sr);
  return number_failed;
  }
