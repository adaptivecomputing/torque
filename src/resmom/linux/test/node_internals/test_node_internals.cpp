#include <vector>
#include <stdio.h>
#include <stdlib.h>

#include "node_internals.hpp"
#include <check.h>

extern int  reserve_called;
extern int  remove_called;
extern int  get_job_indices_called;
extern int  recover_mode;
extern int  recover_called;
extern bool does_it_fit;

START_TEST(test_constructor)
  {
  node_internals ni;

  fail_unless(ni.num_numa_nodes() != 0);

  numa_node n0("../../../../test/test_files", 0);
  numa_node n1("../../../../test/test_files", 1);
  std::vector<numa_node> nodes;
  nodes.push_back(n0);
  nodes.push_back(n1);

  node_internals ni2(nodes);
  fail_unless(ni2.num_numa_nodes() == 2);

  node_internals ni3(ni2);
  fail_unless(ni3.num_numa_nodes() == 2);
  }
END_TEST


START_TEST(test_recover_reservation)
  {
  numa_node n0("../../../../test/test_files", 0);
  numa_node n1("../../../../test/test_files", 1);
  std::vector<numa_node> nodes;
  nodes.push_back(n0);
  nodes.push_back(n1);
  node_internals ni(nodes);

  recover_mode = 0;
  recover_called = 0;
  ni.recover_reservation(2, 2048, "1.napali");
  fail_unless(recover_called == 1);

  recover_called = 0;
  reserve_called = 0;
  recover_mode = 1;
  ni.recover_reservation(2, 2048, "1.napali");
  fail_unless(recover_called == 2);
  
  recover_called = 0;
  reserve_called = 0;
  recover_mode = 1;
  
  char buf[1024];
  fail_unless(reserve_called == 0);
  ni.recover_reservation(3, 2049, "1.napali");
  fail_unless(recover_called == 2);
  snprintf(buf, sizeof(buf), "reserve has been called %d times", reserve_called);
  fail_unless(reserve_called > 0, buf);
  }
END_TEST


START_TEST(test_reserve)
  {
  numa_node n0("../../../../test/test_files", 0);
  numa_node n1("../../../../test/test_files", 1);
  std::vector<numa_node> nodes;
  nodes.push_back(n0);
  nodes.push_back(n1);
  node_internals ni(nodes);

  does_it_fit = false;
  reserve_called = 0;
  ni.reserve(6, 100000, "1.napali");
  fail_unless(reserve_called == 2);

  does_it_fit = true;
  reserve_called = 0;
  ni.reserve(6, 100000, "1.napali");
  fail_unless(reserve_called == 1);
  }
END_TEST


START_TEST(test_remove)
  {
  numa_node n0("../../../../test/test_files", 0);
  numa_node n1("../../../../test/test_files", 1);
  std::vector<numa_node> nodes;
  nodes.push_back(n0);
  nodes.push_back(n1);
  node_internals ni(nodes);

  remove_called = 0;
  ni.remove_job("1.napali");
  fail_unless(remove_called == 2);
  }
END_TEST


START_TEST(test_getting_indices)
  {
  numa_node n0("../../../../test/test_files", 0);
  numa_node n1("../../../../test/test_files", 1);
  std::vector<numa_node> nodes;
  nodes.push_back(n0);
  nodes.push_back(n1);
  node_internals ni(nodes);

  get_job_indices_called = 0;
  ni.get_cpu_indices("1.napali");
  fail_unless(get_job_indices_called == 2);

  get_job_indices_called = 0;
  ni.get_memory_indices("1.napali");
  fail_unless(get_job_indices_called == 2);
  }
END_TEST


Suite *node_internals_suite(void)
  {
  Suite *s = suite_create("node_internals test suite methods");
  TCase *tc_core = tcase_create("test_constructor");
  tcase_add_test(tc_core, test_constructor);
  tcase_add_test(tc_core, test_reserve);
  tcase_add_test(tc_core, test_remove);
  tcase_add_test(tc_core, test_getting_indices);
  tcase_add_test(tc_core, test_recover_reservation);
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
  sr = srunner_create(node_internals_suite());
  srunner_set_log(sr, "node_internals_suite.log");
  srunner_run_all(sr, CK_NORMAL);
  number_failed = srunner_ntests_failed(sr);
  srunner_free(sr);
  return number_failed;
  }
