#include <vector>
#include <stdlib.h>
#include <stdio.h>

#include "allocation.hpp"
#include "numa_node.hpp"
#include <check.h>

extern char cpulist[];

START_TEST(test_in_this_numa_node)
  {
  numa_node nn;

  std::string str("0-1,8-9");
  nn.parse_cpu_string(str);

  fail_unless(nn.in_this_numa_node(0) >= 0);
  fail_unless(nn.in_this_numa_node(1) >= 0);
  fail_unless(nn.in_this_numa_node(8) >= 0);
  fail_unless(nn.in_this_numa_node(9) >= 0);
  fail_unless(nn.in_this_numa_node(2) < 0);
  fail_unless(nn.in_this_numa_node(3) < 0);
  }
END_TEST


START_TEST(test_recover_reservation)
  {
  numa_node nn("../../../../test/test_files", 0);

  allocation a;
  strcpy(cpulist, "0-1");
  nn.recover_reservation(2, 1024, "1.napali", a);

  fail_unless(a.cpus == 2);
  fail_unless(a.memory == 1024);

  strcpy(cpulist, "2-3");
  allocation a2;
  nn.recover_reservation(2, 1024, "2.naplali", a2);

  fail_unless(a2.cpus == 0);
  fail_unless(a2.memory == 0);

  allocation a3("3.napali");
  fail_unless(!strcmp(a3.jobid.c_str(), "3.napali"));

  }
END_TEST


START_TEST(test_parse_cpu_string)
  {
  numa_node nn;
  std::string str("0-1,8-9");
  nn.parse_cpu_string(str);
  fail_unless(nn.get_total_cpus() == 4);
  fail_unless(nn.get_available_cpus() == 4);

  std::string str2("0,8");
  numa_node n2;
  n2.parse_cpu_string(str2);
  fail_unless(n2.get_total_cpus() == 2);
  fail_unless(n2.get_available_cpus() == 2);

  numa_node nn2("torque2", 0);
  fail_unless(nn2.get_total_cpus() == 2);
  fail_unless(nn2.get_available_cpus() == 2);

  numa_node nn4("torque4", 0);
  fail_unless(nn4.get_total_cpus() == 4);
  fail_unless(nn4.get_available_cpus() == 4);
  }
END_TEST


START_TEST(test_completely_fits)
  {
  numa_node n2("../../../../test/test_files", 0);

  fail_unless(n2.completely_fits(1,1) == true);
  fail_unless(n2.completely_fits(4,1024*5) == true);
  fail_unless(n2.completely_fits(4,1024*5 + 1) == false);
  fail_unless(n2.completely_fits(5,1024*5) == false);
  }
END_TEST


START_TEST(test_constructor)
  {
  numa_node nn;

  fail_unless(nn.get_total_cpus() == 0);
  fail_unless(nn.get_total_memory() == 0);
  fail_unless(nn.get_available_memory() == 0);
  fail_unless(nn.get_available_cpus() == 0);
  fail_unless(nn.get_my_index() == 0);

  // verify that this doesn't segfault
  nn.get_meminfo(NULL);
  nn.get_meminfo("/proc/meminfo");
  fail_unless(nn.get_total_memory() == 1024 * 5);
  fail_unless(nn.get_available_memory() == nn.get_total_memory());

  numa_node n2("../../../../test/test_files", 0);
  fail_unless(n2.get_total_memory() == 1024 * 5);
  fail_unless(n2.get_total_cpus() == 4);

  numa_node n3(n2);
  fail_unless(n3.get_total_memory() == 1024 * 5);
  fail_unless(n3.get_total_cpus() == 4);
  fail_unless(n3.get_available_cpus() == n2.get_available_cpus());
  fail_unless(n3.get_available_memory() == n2.get_available_memory());
  }
END_TEST


START_TEST(test_reserve)
  {
  numa_node n("../../../../test/test_files", 0);
  std::vector<int> cpu_indices;
  allocation a;

  n.reserve(2, 2048, "1.napali", a);
  fail_unless(a.cpus == 2);
  fail_unless(a.memory == 2048);

  allocation a2;
  n.reserve(1, 2048, "2.napali", a2);
  fail_unless(a2.cpus == 1);
  fail_unless(a2.memory == 2048);
 
  allocation a3;
  n.reserve(2, 2048, "3.napali", a3);
  fail_unless(a3.cpus == 1);
  fail_unless(a3.memory == 1024);

  n.get_job_indices("2.napali", cpu_indices, true);
  fail_unless(cpu_indices.size() == 1);
  printf("index: %d\n", cpu_indices[0]);
  fail_unless(cpu_indices[0] == 8);

  cpu_indices.clear();
  n.get_job_indices("2.napali", cpu_indices, false);
  fail_unless(cpu_indices[0] == 0);

  cpu_indices.clear();
  n.get_job_indices("1.napali", cpu_indices, true);
  fail_unless(cpu_indices.size() == 2);
  fail_unless(cpu_indices[0] == 0);
  fail_unless(cpu_indices[1] == 1);
  
  cpu_indices.clear();
  n.get_job_indices("3.napali", cpu_indices, true);
  fail_unless(cpu_indices.size() == 1);
  fail_unless(cpu_indices[0] == 9);

  n.remove_job("2.napali");
  cpu_indices.clear();
  n.get_job_indices("2.napali", cpu_indices, true);
  fail_unless(cpu_indices.size() == 0);

  fail_unless(n.get_available_cpus() == 1);
  fail_unless(n.get_available_memory() == 2048);

  allocation a4;
  n.reserve(1, 1024, "4.napali", a4);
  fail_unless(a4.cpus == 1);
  fail_unless(a4.memory == 1024);
  cpu_indices.clear();
  n.get_job_indices("4.napali", cpu_indices, true);
  fail_unless(cpu_indices[0] == 8);
  }
END_TEST


Suite *numa_node_suite(void)
  {
  Suite *s = suite_create("numa_node test suite methods");
  TCase *tc_core = tcase_create("test_numa_node");
  tcase_add_test(tc_core, test_constructor);
  tcase_add_test(tc_core, test_parse_cpu_string);
  tcase_add_test(tc_core, test_completely_fits);
  tcase_add_test(tc_core, test_reserve);
  tcase_add_test(tc_core, test_in_this_numa_node);
  tcase_add_test(tc_core, test_recover_reservation);
  suite_add_tcase(s, tc_core);

  /*tc_core = tcase_create("test_allocation");
  tcase_add_test(tc_core, test_allocation_constructors);
  suite_add_tcase(s, tc_core);*/
  
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
  sr = srunner_create(numa_node_suite());
  srunner_set_log(sr, "numa_node_suite.log");
  srunner_run_all(sr, CK_NORMAL);
  number_failed = srunner_ntests_failed(sr);
  srunner_free(sr);
  return number_failed;
  }
