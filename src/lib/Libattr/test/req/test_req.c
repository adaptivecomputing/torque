#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <check.h>

#include "req.hpp"
#include "pbs_error.h"

extern bool good_err;


START_TEST(test_submission_string_has_duplicates)
  {
  char *str = strdup("5:lprocs=4:memory=12gb:place=socket=2:usecores:pack:gpus=2:mics=1:gres=matlab=1:feature=fast");
  char *str2 = strdup("5:feature=featureA");
  char *err_str1 = strdup("4:lprocs=2:lprocs=all");
  char *err_str2 = strdup("3:usecores:usefastcores");
  char *err_str3 = strdup("2:shared:shared");

  std::string error;
  req         r;

  fail_unless(r.submission_string_has_duplicates(str, error) == false);
  fail_unless(r.submission_string_has_duplicates(str2, error) == false);
  fail_unless(r.submission_string_has_duplicates(err_str1, error) == true);
  fail_unless(r.submission_string_has_duplicates(err_str2, error) == true);
  fail_unless(r.submission_string_has_duplicates(err_str3, error) == true);
  }
END_TEST


START_TEST(test_constructors)
  {
  req r;

  fail_unless(r.getHostlist().size() == 0);
  fail_unless(r.getTaskCount() == 1);
  fail_unless(r.getPlacementTypeInt() == PLACE_NO_PREFERENCE);
  fail_unless(r.getPlacementType().size() == 0);
  fail_unless(r.getNodeAccessPolicy().size() == 0, r.getNodeAccessPolicy().c_str());
  fail_unless(r.getOS().size() == 0);
  fail_unless(r.getGres().size() == 0);
  fail_unless(r.getDisk() == 0);
  fail_unless(r.getSwap() == 0);
  fail_unless(r.getMemory() == 0);
  fail_unless(r.getExecutionSlots() == 1, "slots: %d", r.getExecutionSlots());

  req r2("5:lprocs=4:memory=12gb:place=socket=2:usecores:pack:gpus=2:mics=1:gres=matlab=1:feature=fast");
  fail_unless(r2.getTaskCount() == 5, "task count is %d", r2.getTaskCount());
  fail_unless(r2.getPlacementTypeInt() == PLACE_SOCKET, "placement type int %d", r2.getPlacementTypeInt());
  fail_unless(r2.getPlacementType() == "place socket", "value %s", r2.getPlacementType().c_str());
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
  fail_unless(copy_r2.getPlacementTypeInt() == PLACE_SOCKET, "placement type int %d", r2.getPlacementTypeInt());
  fail_unless(copy_r2.getPlacementType() == r2.getPlacementType(), "copy value '%s' first value '%s'", copy_r2.getPlacementType().c_str(), r2.getPlacementType().c_str());
  fail_unless(copy_r2.getNodeAccessPolicy().size() == 0);
  fail_unless(copy_r2.getOS().size() == 0);
  fail_unless(copy_r2.getGres() == "matlab=1");
  fail_unless(copy_r2.getDisk() == 0);
  fail_unless(copy_r2.getSwap() == 0);
  fail_unless(copy_r2.getMemory() == 12 * 1024 * 1024);
  fail_unless(copy_r2.getExecutionSlots() == 4);
  fail_unless(copy_r2.getFeatures() == "fast", "features '%s'", copy_r2.getFeatures().c_str());
  
  req r3("5:lprocs=4:memory=12gb:place=node:mics=1:feature=fast");
  fail_unless(r3.getPlacementType() == "place node");

  req r4("5:lprocs=4:memory=12gb:place=numachip=1:mics=1:feature=fast");
  fail_unless(r4.getPlacementType() == "place numa", r3.getPlacementType().c_str());
  
  req r5("5:lprocs=4:memory=12gb:place=core=4:mics=1:feature=fast");
  fail_unless(r5.getThreadUsageString() == "use cores", "thread usage '%s'", r5.getThreadUsageString().c_str());

  req r6("5:lprocs=4:place=thread=4");
  fail_unless(r6.getThreadUsageString() == "use threads", "thread usage '%s'", r6.getThreadUsageString().c_str());

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

  r.set_from_string("req[1]\ntask count: 1\nlprocs: 1\n thread usage policy: use threads\nplacement type: place numa");

  r.toString(out);

  fail_unless(out.find("task count: 1") != std::string::npos);
  fail_unless(out.find("lprocs: 1") != std::string::npos);
  fail_unless(out.find("thread usage policy: use threads") != std::string::npos);
  fail_unless(out.find("placement type: place numa") != std::string::npos);

  out.clear();
  r.set_from_string("req[2]\ntask count: 1\nlprocs: 1\n thread usage policy: use fast cores\nplacement type: place node");
  r.toString(out);

  fail_unless(out.find("task count: 1") != std::string::npos);
  fail_unless(out.find("lprocs: 1") != std::string::npos);
  fail_unless(out.find("thread usage policy: use fast cores") != std::string::npos);
  fail_unless(out.find("placement type: place node") != std::string::npos);

  r.set_from_string("req[2]\ntask count: 1\nlprocs: 1\n gpus: 2\n gpu mode: exclusive thread\n max tpn: 2\n thread usage policy: allow threads\nplacement type: place node \nreqattr: matlab>=5");
  r.toString(out);

  fail_unless(out.find("task count: 1") != std::string::npos);
  fail_unless(out.find("lprocs: 1") != std::string::npos);
  fail_unless(out.find("thread usage policy: allow threads") != std::string::npos, r.getThreadUsageString().c_str());
  fail_unless(out.find("placement type: place node") != std::string::npos);
  fail_unless(out.find("gpu mode: exclusive thread") != std::string::npos);
  fail_unless(out.find("gpus: 2") != std::string::npos);
  fail_unless(out.find("max tpn: 2") != std::string::npos);
  fail_unless(out.find("reqattr: matlab>=5") != std::string::npos);

  r.set_from_string("req[0]\ntask count: 10\nlprocs: all\nmem: 10000kb\nswap: 1024kb\ndisk: 10000000kb\nsockets: 1\nnuma chips: 2\ngpus: 1\nmics: 1\nthread usage policy: use cores\nplacement type: place socket\ngres: matlab=1\nos: ubuntu\narch: 64\nhostlist: napali/0-31\nfeatures: fast\nsingle job access\npack");

  fail_unless(r.getThreadUsageString() == "use cores");
  fail_unless(r.getFeatures() == "fast");
  fail_unless(r.getExecutionSlots() == ALL_EXECUTION_SLOTS);
  fail_unless(r.getMemory() == 10000);
  fail_unless(r.getSwap() == 1024);
  fail_unless(r.getDisk() == 10000000);
  fail_unless(r.getTaskCount() == 10);
  fail_unless(r.getHostlist() == "napali/0-31");
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
  fail_unless(out.find("sockets: 1") != std::string::npos);
  fail_unless(out.find("numa chips: 2") != std::string::npos);
  fail_unless(out.find("gpus: 1") != std::string::npos);
  fail_unless(out.find("mics: 1") != std::string::npos);
  fail_unless(out.find("thread usage policy: use cores") != std::string::npos);
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
  fail_unless(r.getThreadUsageString() == "use threads", "thread usage '%s'", r.getThreadUsageString().c_str());

  r.set_attribute("allowthreads");
  fail_unless(r.getThreadUsageString() == "allow threads", "thread usage '%s'", r.getThreadUsageString().c_str());

  r.set_attribute("usefastcores");
  fail_unless(r.getThreadUsageString() == "use fast cores", "thread usage '%s'", r.getThreadUsageString().c_str());

  r.set_index(5);
  fail_unless(r.getIndex() == 5);
  }
END_TEST


Suite *req_suite(void)
  {
  Suite *s = suite_create("req test suite methods");
  TCase *tc_core = tcase_create("test_constructors");
  tcase_add_test(tc_core, test_constructors);
  tcase_add_test(tc_core, test_equals_operator);
  suite_add_tcase(s, tc_core);
  
  tc_core = tcase_create("test_set_from_string");
  tcase_add_test(tc_core, test_set_from_string);
  tcase_add_test(tc_core, test_set_attribute);
  tcase_add_test(tc_core, test_submission_string_has_duplicates);
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
