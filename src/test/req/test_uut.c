#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <check.h>

#include "req.hpp"
#include "pbs_error.h"

extern bool good_err;

int parse_positive_integer(const char *str, int &parsed);


START_TEST(test_get_cores_threads)
  {
  req r;

  r.set_value("lprocs", "2");

  fail_unless(r.get_threads() == 2);
  fail_unless(r.get_cores() == 0);

  r.set_value("placement_type", "core");
  fail_unless(r.get_threads() == 2);
  fail_unless(r.get_cores() == 2);
  }
END_TEST


START_TEST(test_append_gres)
  {
  req r;

  r.set_value("gres", "A=3");

  fail_unless(r.append_gres("B=2") == PBSE_NONE);
  fail_unless(r.append_gres("B=2") == PBSE_NONE);
  fail_unless(r.append_gres("A=2") == PBSE_NONE);

  fail_unless(r.getGres() == "A=2:B=2", r.getGres().c_str());
  }
END_TEST


START_TEST(test_string_constructor)
  {
  req r1(strdup("6:ppn=2:bigmem"));
  fail_unless(r1.getTaskCount() == 6);
  fail_unless(r1.getExecutionSlots() == 2);

  req r2(strdup("3:ppn=4:bigmem:mics=1"));
  fail_unless(r2.getTaskCount() == 3);
  fail_unless(r2.getExecutionSlots() == 4);
  fail_unless(r2.getMics() == 1);

  req r3(strdup("2:ppn=8:fast:gpus=2"));
  fail_unless(r3.getTaskCount() == 2);
  fail_unless(r3.getExecutionSlots() == 8);
  fail_unless(r3.getGpus() == 2);
  }
END_TEST

START_TEST(test_get_task_stats)
  {
  }
END_TEST


START_TEST(test_get_set_values)
  {
  req r;

  r.set_value("index", "0");
  r.set_value("lprocs", "all");
  r.set_value("memory", "1024mb");
  r.set_value("swap", "1024kb");
  r.set_value("disk", "10000000kb");
  r.set_value("socket", "1");
  r.set_value("gpus", "2");
  r.set_value("task_count", "5");
  r.set_value("gpu_mode", "exclusive_thread");
  r.set_value("mics", "1");
  r.set_value("thread_usage_policy", "use threads");
  r.set_value("reqattr", "matlab>7");
  r.set_value("gres", "gresA");
  r.set_value("opsys", "ubuntu");
  r.set_value("arch", "64bit");
  r.set_value("hostlist", "napali:ppn=32");
  r.set_value("features", "fast");
  r.set_value("single_job_access", "true");

  std::vector<std::string> names;
  std::vector<std::string> values;

  fail_unless(r.get_sockets() == 1);

  r.get_values(names, values);

  fail_unless(names[0] == "task_count.0");
  fail_unless(names[1] == "lprocs.0");
  fail_unless(names[2] == "memory.0");
  fail_unless(names[3] == "swap.0");
  fail_unless(names[4] == "disk.0");
  fail_unless(names[5] == "socket.0");
  fail_unless(names[6] == "gpus.0");
  fail_unless(names[7] == "gpu_mode.0");
  fail_unless(names[8] == "mics.0");
  fail_unless(names[9] == "thread_usage_policy.0", names[9].c_str());
  fail_unless(names[10] == "reqattr.0");
  fail_unless(names[11] == "gres.0");
  fail_unless(names[12] == "opsys.0");
  fail_unless(names[13] == "arch.0");
  fail_unless(names[14] == "features.0", names[14].c_str());
  fail_unless(names[15] == "single_job_access.0");
  fail_unless(names[16] == "hostlist.0", names[16].c_str());

  fail_unless(values[0] == "5");
  fail_unless(values[1] == "all");
  fail_unless(values[2] == "1048576kb", "value: %s", values[2].c_str());
  fail_unless(values[3] == "1024kb");
  fail_unless(values[4] == "10000000kb");
  fail_unless(values[5] == "1");
  fail_unless(values[6] == "2");
  fail_unless(values[7] == "exclusive_thread");
  fail_unless(values[8] == "1");
  fail_unless(values[9] == "use threads");
  fail_unless(values[10] == "matlab>7");
  fail_unless(values[11] == "gresA");
  fail_unless(values[12] == "ubuntu");
  fail_unless(values[13] == "64bit");
  fail_unless(values[14] == "fast");
  fail_unless(values[15] == "true");
  fail_unless(values[16] == "napali:ppn=32");

  req r2;
  r2.set_value("lprocs", "2");
  r2.set_value("numanode", "1");
  r2.set_value("maxtpn", "1");
  r2.set_value("placement_type", "core");
  r2.set_value("pack", "true");

  names.clear();
  values.clear();

  fail_unless(r2.get_numa_nodes() == 1);

  r2.get_values(names, values);

  fail_unless(names[0] == "task_count.0");
  fail_unless(names[1] == "lprocs.0");
  fail_unless(names[2] == "numanode.0");
  fail_unless(names[3] == "core.0");
  fail_unless(names[4] == "maxtpn.0", "%s", names[4].c_str());
  fail_unless(names[5] == "thread_usage_policy.0");
  fail_unless(names[6] == "pack.0");
  fail_unless(values[0] == "1");
  fail_unless(values[1] == "2");
  fail_unless(values[2] == "1");
  fail_unless(values[3] == "1");
  fail_unless(values[4] == "1");
  fail_unless(values[5] == "usecores", values[5].c_str());
  fail_unless(values[6] == "true");

  fail_unless(r2.set_value("bob", "tom") != PBSE_NONE);

  int i;
  fail_unless(parse_positive_integer("1.0", i) != PBSE_NONE);

  req r3;
  r3.set_value("task_count", "1");
  r3.set_value("placement_type", "place node");
  r3.set_value("lprocs", "all");
  allocation a;
  a.cores = 8;
  a.threads = 16;
  a.hostname = "napali";
  for (int i = 0; i < 16; i++)
    a.cpu_indices.push_back(i);

  a.mem_indices.push_back(0);
  a.mem_indices.push_back(1);
  r3.record_allocation(a);

  names.clear();
  values.clear();

  r3.get_values(names, values);
  fail_unless(names[0] == "task_count.0");
  fail_unless(names[1] == "lprocs.0");
  fail_unless(names[3] == "task_usage.0.task.0", names[3].c_str());
  fail_unless(values[0] == "1");
  fail_unless(values[1] == "all");
  fail_unless(values[3] == "{\"task\":{\"cpu_list\":\"0-15\",\"mem_list\":\"0-1\",\"cores\":8,\"threads\":16,\"host\":\"napali\"}}", values[3].c_str());

  req clone;
  std::vector<std::string> names_clone;
  std::vector<std::string> values_clone;

  for (unsigned int i = 0; i < names.size() - 1; i++)
    clone.set_value(names[i].c_str(), values[i].c_str());
  clone.set_value("task_usage", values[names.size() - 1].c_str(), 0);

  clone.get_values(names_clone, values_clone);
  
  for (unsigned int i = 0; i < names.size(); i++)
    {
    fail_unless(names[i] == names_clone[i]);
    fail_unless(values[i] == values_clone[i], "%s != %s", values[i].c_str(), values_clone[i].c_str());
    }
  
  fail_unless(names.size() == names_clone.size());
  fail_unless(values.size() == values_clone.size());

  // Test removing and recording allocations
  allocation save;
  save.initialize_from_string(values[3]);
  names.clear();
  values.clear();
  r3.clear_allocations();
  r3.get_values(names, values);
  fail_unless(values[3] != "\"cpu_list\":\"0-15\",\"mem_list\":\"0-1\",\"cores\":8,\"threads\":16", values[3].c_str());
  names.clear();
  values.clear();
  r3.record_allocation(save);
  r3.get_values(names, values);
  fail_unless(values[3] == "{\"task\":{\"cpu_list\":\"0-15\",\"mem_list\":\"0-1\",\"cores\":8,\"threads\":16,\"host\":\"napali\"}}", values[3].c_str());
  }
END_TEST


START_TEST(test_submission_string_has_duplicates)
  {
  char *str = strdup("5:lprocs=4:memory=12gb:place=socket=2:usecores:pack:gpus=2:mics=1:gres=matlab=1:feature=fast");
  char *str2 = strdup("5:feature=featureA");
  char *err_str1 = strdup("4:lprocs=2:lprocs=all");
  char *err_str2 = strdup("3:usecores:usefastcores");
  char *err_str3 = strdup("2:shared:shared");
  char *err_str4 = strdup("2:swap=8gb:swap=4gb");
  char *err_str5 = strdup("2:usethreads:place=numanode");
  char *err_str6 = strdup("2:usefastcores:place=socket");
  char *err_str7 = strdup("2:usethreads:place=node");
  char *non_err_str1 = strdup("2:reqattr=matlab>7:reqattr=<9");
  char *non_err_str2 = strdup("2:usecores:place=numanode");

  std::string error;
  req         r;

  fail_unless(r.submission_string_has_duplicates(str, error) == false);
  fail_unless(r.submission_string_has_duplicates(str2, error) == false);
  fail_unless(r.submission_string_has_duplicates(err_str1, error) == true);
  fail_unless(r.submission_string_has_duplicates(err_str2, error) == true);
  fail_unless(r.submission_string_has_duplicates(err_str3, error) == true);
  fail_unless(r.submission_string_has_duplicates(err_str4, error) == true);
  fail_unless(r.submission_string_has_duplicates(err_str5, error) == true);
  fail_unless(r.submission_string_has_duplicates(err_str6, error) == true);
  fail_unless(r.submission_string_has_duplicates(err_str7, error) == true);
  fail_unless(r.submission_string_has_duplicates(non_err_str1, error) == false);
  fail_unless(r.submission_string_has_duplicates(non_err_str2, error) == false);
  fail_unless(r.submission_string_precheck(strdup("5:lprocs=4+2:lprocs=2"), error) != PBSE_NONE);
  fail_unless(r.set_from_submission_string(strdup("1.0"), error) != PBSE_NONE);
  }
END_TEST


START_TEST(test_constructors)
  {
  req r;
  std::vector<std::string> l;

  fail_unless(r.getHostlist(l) == PBSE_EMPTY);
  fail_unless(l.size() == 0);
  fail_unless(r.getTaskCount() == 1);
  fail_unless(r.getPlacementType() == "");
  fail_unless(r.getNodeAccessPolicy().size() == 0, r.getNodeAccessPolicy().c_str());
  fail_unless(r.getOS().size() == 0);
  fail_unless(r.getGres().size() == 0);
  fail_unless(r.getDisk() == 0);
  fail_unless(r.getSwap() == 0);
  fail_unless(r.getMemory() == 0);
  fail_unless(r.getExecutionSlots() == 1, "slots: %d", r.getExecutionSlots());

  std::string req2("5:lprocs=4:memory=12gb:place=socket=2:usecores:pack:gpus=2:mics=1:gres=matlab=1:feature=fast");
  req r2(req2);
  fail_unless(r2.getTaskCount() == 5, "task count is %d", r2.getTaskCount());
  fail_unless(r2.getPlacementType() == "socket=2", "value %s", r2.getPlacementType().c_str());
  fail_unless(r2.getNodeAccessPolicy().size() == 0);
  fail_unless(r2.getOS().size() == 0);
  fail_unless(r2.getGres() == "matlab=1");
  fail_unless(r2.getDisk() == 0);
  fail_unless(r2.getSwap() == 0);
  fail_unless(r2.getMemory() == 12 * 1024 * 1024);
  fail_unless(r2.getExecutionSlots() == 4);
  fail_unless(r2.getFeatures() == "fast", "features '%s'", r2.getFeatures().c_str());

  req copy_r2(r2);
  fail_unless(copy_r2.getTaskCount() == 5);
  fail_unless(copy_r2.getPlacementType() == r2.getPlacementType(), "copy value '%s' first value '%s'", copy_r2.getPlacementType().c_str(), r2.getPlacementType().c_str());
  fail_unless(copy_r2.getNodeAccessPolicy().size() == 0);
  fail_unless(copy_r2.getOS().size() == 0);
  fail_unless(copy_r2.getGres() == "matlab=1");
  fail_unless(copy_r2.getDisk() == 0);
  fail_unless(copy_r2.getSwap() == 0);
  fail_unless(copy_r2.getMemory() == 12 * 1024 * 1024);
  fail_unless(copy_r2.getExecutionSlots() == 4);
  fail_unless(copy_r2.getFeatures() == "fast", "features '%s'", copy_r2.getFeatures().c_str());
 
  std::string req3("5:lprocs=4:memory=12gb:place=node:mics=1:feature=fast");
  req r3(req3);
  fail_unless(r3.getPlacementType() == "node");

  std::string req4("5:lprocs=4:memory=12gb:place=numanode=1:mics=1:feature=fast");
  req r4(req4);
  fail_unless(r4.getPlacementType() == "numanode=1", r4.getPlacementType().c_str());
 
  std::string req5("5:lprocs=4:memory=12gb:place=core:mics=1:feature=fast");
  req r5(req5);
  fail_unless(r5.getThreadUsageString() == "usecores", "thread usage '%s'", r5.getThreadUsageString().c_str());

  std::string req6("5:lprocs=4:place=thread");
  req r6(req6);
  fail_unless(r6.getThreadUsageString() == "usethreads", "thread usage '%s'", r6.getThreadUsageString().c_str());

  // make sure miss-spellings are caught
  std::string error;
  req str_set;

  fail_unless(str_set.set_from_submission_string(strdup("5:lproc=2"), error) != PBSE_NONE);
  good_err = true;
  fail_unless(str_set.set_from_submission_string(strdup("5:lprocs=2:place=sockets=2"), error) != PBSE_NONE);

  fail_unless(str_set.set_from_submission_string(strdup("4:lprocs=all"), error) != PBSE_NONE);
  fail_unless(str_set.set_from_submission_string(strdup("4:memory=12gb:memory=1024mb"), error) != PBSE_NONE);
  fail_unless(str_set.set_from_submission_string(strdup("-1:lprocs=4"), error) != PBSE_NONE);
  fail_unless(str_set.set_from_submission_string(strdup("1:lprocs=-4"), error) != PBSE_NONE);
  fail_unless(str_set.set_from_submission_string(strdup("1:lprocs=4:memory=0"), error) != PBSE_NONE);
  fail_unless(str_set.set_from_submission_string(strdup("3:gpus=2:reseterr:exclusive_thread:opsys=cent6"), error) == PBSE_NONE);
  fail_unless(str_set.set_from_submission_string(strdup("1:lprocs=all:place=core"), error) != PBSE_NONE);
  fail_unless(str_set.set_from_submission_string(strdup("1:lprocs=all:place=thread=2"), error) != PBSE_NONE);

  std::string req7("2:place=numanode");
  req r7(req7);
  r7.set_index(0);

  std::vector<std::string> names;
  std::vector<std::string> values;
  r7.get_values(names, values);

  fail_unless(names[0] == "task_count.0", names[0].c_str());
  fail_unless(names[1] == "lprocs.0", names[1].c_str());
  fail_unless(names[2] == "numanode.0", names[2].c_str());
  fail_unless(values[0] == "2");
  fail_unless(values[1] == "1");
  fail_unless(values[2] == "1");

  std::string req8("2:place=core");
  req r8(req8);
  r8.set_index(0);

  names.clear();
  values.clear();
  r8.get_values(names, values);

  fail_unless(names[0] == "task_count.0", names[0].c_str());
  fail_unless(names[1] == "lprocs.0", names[1].c_str());
  fail_unless(values[0] == "2");
  fail_unless(values[1] == "1");
  }
END_TEST


START_TEST(test_equals_operator)
  {
  req r;
  req r2;

  r.set_name_value_pair("lprocs", "all");
  r.set_name_value_pair("memory", "1tb");
  r.set_name_value_pair("maxtpn", "4");
  r.set_name_value_pair("disk", "4mb");
  r.set_name_value_pair("opsys", "ubuntu");
  r.set_name_value_pair("gpus", "2");
  r.set_attribute("exclusive_thread");
  r.set_name_value_pair("reqattr", "matlab>=7");
  r2 = r;

  fail_unless(r2.getExecutionSlots() == ALL_EXECUTION_SLOTS);
  fail_unless(r2.getMemory() == 1024 * 1024 * 1024);
  fail_unless(r2.getMaxtpn() == 4);
  fail_unless(r2.getGpuMode() == "exclusive_thread");
  fail_unless(r2.getReqAttr() == "matlab>=7", "reqattr: '%s'", r2.getReqAttr().c_str());

  // make sure this doesn't segfault
  r = r;
  fail_unless(r.getExecutionSlots() == ALL_EXECUTION_SLOTS);

  // make sure set_attribute fails when bad values are sent
  fail_unless(r2.set_attribute("bob") == PBSE_BAD_PARAMETER);
  }
END_TEST


START_TEST(test_set_from_string)
  {
  req r;

  std::string out;

  r.set_from_string("req[1]\ntask count: 1\nlprocs: 1\n thread usage policy: usethreads\nplacement type: place numa");

  r.toString(out);

  fail_unless(out.find("task count: 1") != std::string::npos);
  fail_unless(out.find("lprocs: 1") != std::string::npos);
  fail_unless(out.find("thread usage policy: usethreads") != std::string::npos, out.c_str());
  fail_unless(out.find("placement type: place numa") != std::string::npos);

  out.clear();
  r.set_from_string("req[2]\ntask count: 1\nlprocs: 1\n thread usage policy: usefastcores\nplacement type: place node");
  r.toString(out);

  fail_unless(out.find("task count: 1") != std::string::npos);
  fail_unless(out.find("lprocs: 1") != std::string::npos);
  fail_unless(out.find("thread usage policy: usefastcores") != std::string::npos);
  fail_unless(out.find("placement type: place node") != std::string::npos);

  r.set_from_string("req[2]\ntask count: 1\nlprocs: 1\n gpus: 2\n gpu mode: exclusive thread\n max tpn: 2\n thread usage policy: allowthreads\nplacement type: place node \nreqattr: matlab>=5");
  r.toString(out);

  fail_unless(out.find("task count: 1") != std::string::npos);
  fail_unless(out.find("lprocs: 1") != std::string::npos);
  fail_unless(out.find("thread usage policy: allowthreads") != std::string::npos, r.getThreadUsageString().c_str());
  fail_unless(out.find("placement type: place node") != std::string::npos);
  fail_unless(out.find("gpu mode: exclusive thread") != std::string::npos);
  fail_unless(out.find("gpus: 2") != std::string::npos);
  fail_unless(out.find("max tpn: 2") != std::string::npos);
  fail_unless(out.find("reqattr: matlab>=5") != std::string::npos);

  r.set_from_string("req[0]\ntask count: 10\nlprocs: all\nmem: 10000kb\nswap: 1024kb\ndisk: 10000000kb\nsocket: 1\nnumanode: 2\ngpus: 1\nmics: 1\nthread usage policy: usecores\nplacement type: place socket\ngres: matlab=1\nos: ubuntu\narch: 64\nhostlist: napali/0-31\nfeatures: fast\nsingle job access\npack");

  fail_unless(r.getThreadUsageString() == "usecores", r.getThreadUsageString().c_str());
  fail_unless(r.getFeatures() == "fast");
  fail_unless(r.getExecutionSlots() == ALL_EXECUTION_SLOTS);
  fail_unless(r.getMemory() == 10000);
  fail_unless(r.getSwap() == 1024);
  fail_unless(r.getDisk() == 10000000);
  fail_unless(r.getTaskCount() == 10);
  std::vector<std::string> l;
  fail_unless(r.getHostlist(l) == 0);
  fail_unless(l[0] == "napali/0-31");
  fail_unless(r.getPlacementType() == "place socket");
  fail_unless(r.getOS() == "ubuntu");

  out.clear();

  r.toString(out);
  fail_unless(out.find("req[0]") != std::string::npos);
  fail_unless(out.find("task count: 10") != std::string::npos);
  fail_unless(out.find("lprocs: all") != std::string::npos);
  fail_unless(out.find("mem: 10000kb") != std::string::npos);
  fail_unless(out.find("swap: 1024kb") != std::string::npos);
  fail_unless(out.find("disk: 10000000kb") != std::string::npos);
  fail_unless(out.find("socket: 1") != std::string::npos);
  fail_unless(out.find("numanode: 2") != std::string::npos);
  fail_unless(out.find("gpus: 1") != std::string::npos);
  fail_unless(out.find("mics: 1") != std::string::npos);
  fail_unless(out.find("thread usage policy: usecores") != std::string::npos);
  fail_unless(out.find("placement type: place socket") != std::string::npos, out.c_str());
  fail_unless(out.find("gres: matlab=1") != std::string::npos);
  fail_unless(out.find("os: ubuntu") != std::string::npos);
  fail_unless(out.find("arch: 64") != std::string::npos);
  fail_unless(out.find("hostlist: napali/0-31") != std::string::npos, out.c_str());
  fail_unless(out.find("features: fast") != std::string::npos);
  fail_unless(out.find("single job access") != std::string::npos);
  fail_unless(out.find("pack") != std::string::npos);

  }
END_TEST


START_TEST(test_set_attribute)
  {
  req r;

  r.set_attribute("usethreads");
  fail_unless(r.getThreadUsageString() == "usethreads", "thread usage '%s'", r.getThreadUsageString().c_str());

  r.set_attribute("allowthreads");
  fail_unless(r.getThreadUsageString() == "allowthreads", "thread usage '%s'", r.getThreadUsageString().c_str());

  r.set_attribute("usefastcores");
  fail_unless(r.getThreadUsageString() == "usefastcores", "thread usage '%s'", r.getThreadUsageString().c_str());

  r.set_index(5);
  fail_unless(r.getIndex() == 5);
  }
END_TEST

START_TEST(test_get_memory_for_host)
  {
  req r;
  std::string host = "napali";
  unsigned long mem;

  r.set_value("index", "0");
  r.set_value("lprocs", "all");
  r.set_value("memory", "1024kb");
  r.set_value("swap", "1024kb");
  r.set_value("task_count", "5");
  r.set_value("thread_usage_policy", "use threads");
  r.set_value("hostlist", "napali:ppn=32");

  mem = r.get_memory_for_host(host);
  fail_unless(mem != 0);

  host = "right said fred";
  mem = r.get_memory_for_host(host);
  fail_unless(mem == 0);

  }
END_TEST


START_TEST(test_update_hostlist)
  {
  req r;
  std::vector<std::string> list;

  r.update_hostlist("napali");
  fail_unless(r.getHostlist(list) == 0);
  fail_unless(list.size() == 1);
  fail_unless(list[0] == "napali");

  list.clear();
  r.update_hostlist("waimea");
  r.update_hostlist("lihue");
  fail_unless(r.getHostlist(list) == 0);
  fail_unless(list.size() == 3);
  fail_unless(list[0] == "napali");
  fail_unless(list[1] == "waimea");
  fail_unless(list[2] == "lihue");
  }
END_TEST


START_TEST(test_get_num_tasks_for_host)
  {
  req r;

  r.set_value("index", "0");
  r.set_value("lprocs", "2");
  r.set_value("task_count", "8");
  r.set_value("hostlist", "napali:ppn=16");
  r.set_value("placement_type", "core");
  int tasks = r.get_num_tasks_for_host("napali");
  fail_unless(tasks == 8, "Expected 8, got %d", tasks);
  fail_unless(r.get_num_tasks_for_host(16) == 8);

  req r2;
  r2.set_value("lprocs", "1");
  r2.set_hostlist("napali");
  tasks = r2.get_num_tasks_for_host("napali");
  fail_unless(tasks == 1, "Expected 1, got %d", tasks);
  fail_unless(r2.get_num_tasks_for_host(1) == 1);
  
  req r3;
  r3.set_value("hostlist", "napali/0-15");
  tasks = r3.get_num_tasks_for_host("napali");
  fail_unless(tasks == 16, "Expected 16, got %d", tasks);
  fail_unless(r3.get_num_tasks_for_host(16) == 16);
 
  req r4;
  r4.set_value("hostlist", "napali/0-15+wailua/0-15");
  r4.set_value("lprocs", "4");
  r4.set_value("task_count", "8");
  tasks = r4.get_num_tasks_for_host("napali");
  fail_unless(tasks == 4, "Expected 4, got %d", tasks);
  fail_unless(r4.get_num_tasks_for_host(16) == 4);
  
  req r5;
  r5.set_value("hostlist", "waimea/0-16+napali/0-15");
  r5.set_value("lprocs", "4");
  r5.set_value("task_count", "8");
  tasks = r5.get_num_tasks_for_host("napali");
  fail_unless(tasks == 4, "Expected 4, got %d", tasks);
  fail_unless(r5.get_num_tasks_for_host(16) == 4);
  }
END_TEST

START_TEST(test_set_place_value)
  {
  req r;
  int num_cores;

  r.set_place_value("core=4");
  num_cores = r.getPlaceCores();
  fail_unless(num_cores==4, "set_place core failed");

  int numanodes = 0;
  r.set_place_value("numanode=2");
  numanodes = r.get_numa_nodes();
  fail_unless(numanodes==2, "set_place numanode failed");

  int sockets = 0;
  r.set_place_value("socket=2");
  sockets = r.get_sockets();
  fail_unless(sockets==2, "set_place socket failed");

  int threads = 0;
  r.set_place_value("thread=4");
  threads = r.getPlaceThreads();
  fail_unless(threads==4, "set_place threads failed");

  }
END_TEST


START_TEST(test_get_swap_for_host)
  {
  req r;
  std::string host = "napali";
  unsigned long mem;

  r.set_value("index", "0");
  r.set_value("lprocs", "all");
  r.set_value("memory", "1024kb");
  r.set_value("swap", "1024kb");
  r.set_value("task_count", "5");
  r.set_value("thread_usage_policy", "use threads");
  r.set_value("hostlist", "napali:ppn=32");

  mem = r.get_swap_for_host(host);
  fail_unless(mem != 0);

  host = "right said fred";
  mem = r.get_swap_for_host(host);
  fail_unless(mem == 0);

  }
END_TEST

START_TEST(test_get_task_allocation)
  {
  int rc;
  req r3;
  allocation a1;

  r3.set_value("task_count", "1");
  r3.set_value("placement_type", "place node");
  r3.set_value("lprocs", "all");
  allocation a;
  a.cores = 8;
  a.threads = 16;
  for (int i = 0; i < 16; i++)
    a.cpu_indices.push_back(i);

  a.mem_indices.push_back(0);
  a.mem_indices.push_back(1);
  r3.record_allocation(a);

  rc = r3.get_task_allocation(0, a1);
  fail_unless(rc==0);

  fail_unless(a1.cores==8);
  fail_unless(a1.threads==16);
  fail_unless(a1.cpu_indices[1] == 1);
  fail_unless(a1.mem_indices[1] == 1);
  }
END_TEST


Suite *req_suite(void)
  {
  Suite *s = suite_create("req test suite methods");
  TCase *tc_core = tcase_create("test_constructors");
  tcase_add_test(tc_core, test_constructors);
  tcase_add_test(tc_core, test_equals_operator);
  tcase_add_test(tc_core, test_append_gres);
  tcase_add_test(tc_core, test_get_num_tasks_for_host);
  tcase_add_test(tc_core, test_string_constructor);
  tcase_add_test(tc_core, test_get_cores_threads);
  suite_add_tcase(s, tc_core);

  tc_core = tcase_create("test_set_from_string");
  tcase_add_test(tc_core, test_set_from_string);
  tcase_add_test(tc_core, test_set_attribute);
  tcase_add_test(tc_core, test_submission_string_has_duplicates);
  tcase_add_test(tc_core, test_get_set_values);
  tcase_add_test(tc_core, test_get_memory_for_host);
  tcase_add_test(tc_core, test_get_swap_for_host);
  tcase_add_test(tc_core, test_get_task_allocation);
  tcase_add_test(tc_core, test_update_hostlist);
  suite_add_tcase(s, tc_core);

  tc_core = tcase_create("test_setters");
  tcase_add_test(tc_core, test_set_place_value);
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
  sr = srunner_create(req_suite());
  srunner_set_log(sr, "req_suite.log");
  srunner_run_all(sr, CK_NORMAL);
  number_failed = srunner_ntests_failed(sr);
  srunner_free(sr);
  return(number_failed);
  }
