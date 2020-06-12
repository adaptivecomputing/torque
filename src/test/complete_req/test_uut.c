#include <stdio.h>
#include <stdlib.h>
#include <check.h>
#include <vector>

#include "complete_req.hpp"
#include "pbs_error.h"
#include "resource.h"

extern int called_log_event;


START_TEST(test_set_value_from_nodes)
  {
  complete_req c;
  int          task_count;

  // Make sure this doesn't segfault
  c.set_value_from_nodes(NULL, task_count);
  c.set_value_from_nodes("4", task_count);
  fail_unless(task_count == 4);
  fail_unless(c.req_count() == 1);

  complete_req c2;
  c2.set_value_from_nodes("bob+tim", task_count);
  fail_unless(task_count == 2);
  fail_unless(c2.req_count() == 2);
  }
END_TEST


START_TEST(test_set_get_value)
  {
  complete_req c;

  fail_unless(c.set_value(1, "task_count", "5", false) == PBSE_NONE);
  fail_unless(c.set_value(0, "task_count", "4", false) == PBSE_NONE);
  fail_unless(c.set_value(0, "lprocs", "4", false) == PBSE_NONE);
  fail_unless(c.set_value(1, "gpus", "2", false) == PBSE_NONE);
  fail_unless(c.set_value(-1, "blah", "blah", false) == PBSE_BAD_PARAMETER);
  fail_unless(c.req_count() == 2);

  std::vector<std::string> names;
  std::vector<std::string> values;

  c.get_values(names, values);

  fail_unless(names[0] == "task_count.0");
  fail_unless(names[1] == "lprocs.0");
  fail_unless(names[2] == "thread_usage_policy.0");
  fail_unless(names[3] == "task_count.1", names[3].c_str());
  fail_unless(names[4] == "lprocs.1", names[4].c_str());
  fail_unless(names[5] == "gpus.1", names[4].c_str());
  fail_unless(names[6] == "thread_usage_policy.1");

  fail_unless(values[0] == "4");
  fail_unless(values[1] == "4");
  fail_unless(values[3] == "5");
  fail_unless(values[4] == "1");
  fail_unless(values[5] == "2");
  }
END_TEST


void add_resource(

  std::vector<resource> &resources,
  const char            *name,
  const char            *str_val,
  long                   lval1,
  long                   lval2)

  {
  resource_def *rd = (resource_def *)calloc(1, sizeof(resource_def));
  rd->rs_name = strdup(name);
  resource r;

  r.rs_defin = rd;
  
  if (str_val != NULL)
    {
    r.rs_value.at_val.at_str = strdup(str_val);
    }
  else if (lval2 < 0)
    r.rs_value.at_val.at_long = lval1;
  else
    {
    r.rs_value.at_val.at_size.atsv_num = lval1;
    r.rs_value.at_val.at_size.atsv_shift = lval2;
    }
  
  resources.push_back(r);
  } // add_resource()


START_TEST(test_constructor)
  {
  complete_req c;

  req r1;
  req r2;

  r1.set_from_string("req[1]\ntask count: 6\nlprocs: all\n thread usage policy: usethreads\nplacement type: place numa");
  r2.set_from_string("req[1]\ntask count: 8\nlprocs: 4\n thread usage policy: usecores\nplacement type: place socket");

  c.add_req(r1);
  c.add_req(r2);

  complete_req copy(c);

  fail_unless(copy.req_count() == 2);
  
  std::string c_out;
  std::string copy_out;

  c.toString(c_out);
  copy.toString(copy_out);

  fail_unless(c_out == copy_out);

  complete_req equals;
  equals = c;

  fail_unless(equals.req_count() == 2);

  std::string equals_out;
  equals.toString(equals_out);

  fail_unless(equals_out == c_out);

  std::vector<resource> resources;
  add_resource(resources, "nodes", "1:ppn=2+2:gpus=1", 0, 0);
  add_resource(resources, "mem", NULL, 40, 20);
  complete_req list1(&resources, 2, false);

  fail_unless(list1.req_count() == 2);
  const req &rm1 = list1.get_req(0);
  fail_unless(rm1.get_total_memory() == 13653, "mem is %lu", rm1.get_total_memory());

  resources.clear();
  add_resource(resources, "size", NULL, 20, -1);
  add_resource(resources, "mem", NULL, 40, 10);
  complete_req list2(&resources, 2, true);
  fail_unless(list2.req_count() == 1);
  const req &r = list2.get_req(0);
  fail_unless(r.getTaskCount() == 20);
  fail_unless(r.getExecutionSlots() == 1);
  fail_unless(r.get_total_memory() == 40, "mem is %lu", r.get_total_memory());
 
  resources.clear();
  add_resource(resources, "ncpus", NULL, 16, -1);
  add_resource(resources, "mem", NULL, 40, 10);
  complete_req list3(&resources, 2, false);
  fail_unless(list3.req_count() == 1);
  const req &rl = list3.get_req(0);
  fail_unless(rl.getTaskCount() == 1);
  fail_unless(rl.getExecutionSlots() == 16);
  fail_unless(rl.get_total_memory() == 40, "mem is %lu", rl.get_total_memory());

  resources.clear();
  add_resource(resources, "nodes", "1:ppn=2", 16, -1);
  add_resource(resources, "pmem", NULL, 80, 10);
  complete_req list4(&resources, 2, true);
  fail_unless(list4.req_count() == 1);
  const req &rl2 = list4.get_req(0);
  fail_unless(rl2.getTaskCount() == 1);
  // pmem should get multiplied because nodes=1:ppn=2 
  fail_unless(rl2.get_total_memory() == 160, "pmem is %lu", rl2.get_total_memory());

  resources.clear();
  add_resource(resources, "nodes", "1:ppn=2", 16, -1);
  add_resource(resources, "pvmem", NULL, 80, 10);
  complete_req list5(&resources, 2, true);
  fail_unless(list5.req_count() == 1);
  const req &rl3 = list5.get_req(0);
  fail_unless(rl3.getTaskCount() == 1);
  fail_unless(rl3.get_total_memory() == 0, "pvmem is %lu", rl3.get_total_memory());
  // pvmem should get multiplied because nodes=1:ppn=2 is one task
  fail_unless(rl3.get_total_swap() == 160, "pvmem is %lu", rl3.get_total_swap());

  resources.clear();
  add_resource(resources, "nodes", "1:ppn=2", 16, -1);
  add_resource(resources, "vmem", NULL, 80, 10);
  complete_req list6(&resources, 2, true);
  fail_unless(list6.req_count() == 1);
  const req &rl4 = list6.get_req(0);
  fail_unless(rl4.getTaskCount() == 1);
  fail_unless(rl4.get_total_memory() == 0, "vmem is %lu", rl4.get_total_memory());
  fail_unless(rl4.get_total_swap() == 80, "vmem is %lu", rl4.get_total_swap());

  resources.clear();
  add_resource(resources, "procs", NULL, 2, -1);
  add_resource(resources, "pmem", NULL, 1024, 10);
  complete_req list7(&resources, 2, false);
  fail_unless(list7.req_count() == 1);
  const req &rl7 = list7.get_req(0);
  fail_unless(rl7.getTaskCount() == 2, "task count is %d", rl7.getTaskCount());
  // 1024 * 2 = 2048, multiply by 2 because procs=2
  fail_unless(rl7.get_total_memory() == 2048, "mem = %lu", rl7.get_total_memory());

  // Make sure that we'll set memory to the higher of pmem and mem, and set swap
  // as well for the same job
  resources.clear();
  add_resource(resources, "procs", NULL, 1, -1);
  add_resource(resources, "mem", NULL, 4096, 10);
  add_resource(resources, "pmem", NULL, 5, 10);
  add_resource(resources, "vmem", NULL, 8192, 10);
  complete_req list8(&resources, 1, false);
  fail_unless(list8.req_count() == 1);
  const req &rl8 = list8.get_req(0);
  fail_unless(rl8.getTaskCount() == 1);
  fail_unless(rl8.get_total_memory() == 4096);
  fail_unless(rl8.get_total_swap() == 8192);

  resources.clear();
  add_resource(resources, "procs", NULL, 1, -1);
  add_resource(resources, "mem", NULL, 100, 10);
  add_resource(resources, "pmem", NULL, 5000, 10);
  complete_req list9(&resources, 1, false);
  fail_unless(list9.req_count() == 1);
  const req &rl9 = list9.get_req(0);
  fail_unless(rl9.get_total_memory() == 5000);

  }
END_TEST


START_TEST(test_constructor_oldstyle_req)
  {
  std::vector<resource> resources;
  add_resource(resources, "vmem", NULL, 2048, 0);
  complete_req list1(&resources, 2, false);

  fail_unless(list1.req_count() == 1);
  const req &rm1 = list1.get_req(0);
  fail_unless(rm1.get_total_memory() == 0, "mem is %lu", rm1.get_total_memory());
  }
END_TEST


START_TEST(test_to_string)
  {
  complete_req c;

  req r1;
  req r2;

  r1.set_from_string("req[1]\ntask count: 6\nlprocs: all\n thread usage policy: usethreads\nplacement type: place numa");
  r2.set_from_string("req[1]\ntask count: 8\nlprocs: 4\n thread usage policy: usecores\nplacement type: place socket");

  c.add_req(r1);
  c.add_req(r2);

  fail_unless(c.req_count() == 2);

  std::string out_check;
  std::string out_master;

  r1.toString(out_master);
  out_master += '\n';
  r2.toString(out_master);
  c.toString(out_check);

  fail_unless(out_master == out_check, "master: \n%s \n!= produced: \n%s", out_master.c_str(),
      out_check.c_str());

  complete_req c2;

  c2.set_from_string(out_check);

  std::string c2_out;

  c2.toString(c2_out);

  fail_unless(c2_out == out_master, "master: \n%s \n!= produced: \n%s", out_master.c_str(), 
      c2_out.c_str());
  }
END_TEST

START_TEST(test_get_swap_memory_for_this_host)
  {
  complete_req c;
  std::string hostname = "kmn";
  unsigned long swap;

  req r1;
  req r2;

  r1.set_from_string("req[1]\ntask count: 6\nlprocs: 1\n swap: 1048576\n thread usage policy: usethreads\nplacement type: place numa\nhostlist: kmn:ppn=1");

  c.add_req(r1);

  swap = c.get_swap_memory_for_this_host(hostname);
  fail_unless(swap != 0);

  hostname = "right_said_fred";
  swap = c.get_swap_memory_for_this_host(hostname);
  fail_unless(swap == 0);

  }
END_TEST

START_TEST(test_get_num_reqs)
  {
  complete_req c;
  std::string hostname = "kmn";

  req r1;
  req r2;

  r1.set_from_string("req[1]\ntask count: 6\nlprocs: 1\n swap: 1048576\n thread usage policy: usethreads\nplacement type: place numa\nhostlist: kmn:ppn=1");
  r2.set_from_string("req[1]\ntask count: 6\nlprocs: 1\n swap: 1048576\n thread usage policy: usethreads\nplacement type: place numa\nhostlist: kmn:ppn=1:gpus=1");

  c.add_req(r1);
  c.add_req(r2);

  fail_unless(c.get_num_reqs() == 2);
  }
END_TEST


START_TEST(test_update_hostlist)
  {
  complete_req c;
  req r1;
  req r2;
  c.add_req(r1);

  fail_unless(c.update_hostlist("napali:ppn=6", 1) != PBSE_NONE);
  fail_unless(c.update_hostlist("napali:ppn=6", 0) == PBSE_NONE);

  c.add_req(r2);

  fail_unless(c.update_hostlist("waimea:ppn=4", -1) != PBSE_NONE);
  fail_unless(c.update_hostlist("waimea:ppn=4", 2) != PBSE_NONE);
  fail_unless(c.update_hostlist("waimea:ppn=4", 1) == PBSE_NONE);

  std::vector<std::string> list;
  req &r = c.get_req(0);
  fail_unless(r.getHostlist(list) == 0);
  fail_unless(list[0] == "napali:ppn=6");
  list.clear();
  r = c.get_req(1);
  fail_unless(r.getHostlist(list) == 0);
  fail_unless(list[0] == "waimea:ppn=4");
  }
END_TEST


START_TEST(test_set_hostlists)
  {
  complete_req c;
  req r1;
  req r2;
  c.add_req(r1);
  c.add_req(r2);

  // make sure we don't segfault
  c.set_hostlists(NULL, NULL);

  called_log_event = 0;
  c.set_hostlists("1.napali", "napali:ppn=32");
  fail_unless(called_log_event == 1);
  c.set_hostlists("1.napali", "napali:ppn=32|waimea:ppn=16");
  const req &r = c.get_req(0);
  std::vector<std::string> list;
  fail_unless(r.getHostlist(list) == 0);
  fail_unless(list[0] == "napali:ppn=32", list[0].c_str());
  fail_unless(list.size() == 1);
  const req &other = c.get_req(1);
  fail_unless(other.getHostlist(list) == 0);
  fail_unless(list[0] == "waimea:ppn=16", "it is '%s'", list[0].c_str());
  }
END_TEST


START_TEST(test_get_memory_for_this_host)
  {
  complete_req c;
  std::string hostname = "kmn";
  unsigned long swap;

  req r1;
  req r2;

  r1.set_from_string("req[1]\ntask count: 6\nlprocs: 1\n mem: 1048576\n thread usage policy: usethreads\nplacement type: place numa\nhostlist: kmn:ppn=1");

  c.add_req(r1);

  swap = c.get_memory_for_this_host(hostname);
  fail_unless(swap != 0);

  hostname = "right_said_fred";
  swap = c.get_memory_for_this_host(hostname);
  fail_unless(swap == 0);

  }
END_TEST

START_TEST(test_get_req_index_for_host)
  {
  complete_req c;
  std::string hostname = "kmn";
  unsigned int req_index;
  int rc;

  req r1;
  req r2;

  r1.set_from_string("req[1]\ntask count: 6\nlprocs: 1\n mem: 1048576\n thread usage policy: usethreads\nplacement type: place numa\nhostlist: kmn:ppn=1");
  c.add_req(r1);

  r2.set_from_string("req[1]\ntask count: 6\nlprocs: 1\n mem: 1048576\n thread usage policy: usethreads\nplacement type: place numa\nhostlist: pv-knielson-dt:ppn=1");
  c.add_req(r2);

  rc = c.get_req_index_for_host(hostname.c_str(), req_index);
  fail_unless(req_index == 0);
  fail_unless(rc==PBSE_NONE);

  rc = c.get_req_index_for_host("pv-knielson-dt", req_index);
  fail_unless(req_index == 1);
  fail_unless(rc==PBSE_NONE);

  rc = c.get_req_index_for_host("george", req_index);
  fail_unless(rc == PBSE_REQ_NOT_FOUND);
  }
END_TEST



Suite *complete_req_suite(void)
  {
  Suite *s = suite_create("complete_req test suite methods");
  TCase *tc_core = tcase_create("test_constructor");
  tcase_add_test(tc_core, test_constructor);
  suite_add_tcase(s, tc_core);

  tc_core = tcase_create("test_constructor_oldstyle_req");
  tcase_add_test(tc_core, test_constructor_oldstyle_req);
  suite_add_tcase(s, tc_core);
  
  tc_core = tcase_create("test_get_swap_memory_for_this_host");
  tcase_add_test(tc_core, test_get_swap_memory_for_this_host);
  tcase_add_test(tc_core, test_set_hostlists);
  suite_add_tcase(s, tc_core);

  tc_core = tcase_create("test_get_num_reqs");
  tcase_add_test(tc_core, test_get_num_reqs);
  suite_add_tcase(s, tc_core);

  tc_core = tcase_create("test_get_memory_for_this_host");
  tcase_add_test(tc_core, test_get_memory_for_this_host);
  tcase_add_test(tc_core, test_set_value_from_nodes);
  tcase_add_test(tc_core, test_get_req_index_for_host);
  suite_add_tcase(s, tc_core);

  tc_core = tcase_create("test_to_string");
  tcase_add_test(tc_core, test_to_string);
  tcase_add_test(tc_core, test_set_get_value);
  tcase_add_test(tc_core, test_update_hostlist);
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
  sr = srunner_create(complete_req_suite());
  srunner_set_log(sr, "complete_req_suite.log");
  srunner_run_all(sr, CK_NORMAL);
  number_failed = srunner_ntests_failed(sr);
  srunner_free(sr);
  return(number_failed);
  }
