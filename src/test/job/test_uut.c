#include "license_pbs.h" /* See here for the software license */
#include <stdlib.h>
#include <stdio.h>


#include "pbs_error.h"
#include "pbs_job.h"
#include "json/json.h"
#include <check.h>

extern attribute_def job_attr_def[];

void free_do_nothing(pbs_attribute *attr) {}

void init_job_attr_def()
  {
  for (int i = 0; i < 100; i++)
    {
    job_attr_def[i].at_free = free_do_nothing;

    }
  }

START_TEST(test_constructors)
  {
  }
END_TEST


START_TEST(test_plugin_things)
  {
  job pjob;
  const char *json_str = "{\"stormlight\": \"12broams\",\"metals\": \"10vials\",\"planets_visited\": \"2\"}";

  pjob.set_plugin_resource_usage_from_json(json_str);
  //tlist_head  phead;
  std::string acct_data;

  pjob.add_plugin_resource_usage(acct_data);
  fail_unless(acct_data.find("resources_used.stormlight=12broams") != std::string::npos, acct_data.c_str());
  fail_unless(acct_data.find("resources_used.metals=10vials") != std::string::npos);
  fail_unless(acct_data.find("resources_used.planets_visited=2") != std::string::npos);

  pjob.set_plugin_resource_usage("breaths", "10000");
  pjob.set_plugin_resource_usage("planets_visited", "3");
  acct_data.clear();
  pjob.add_plugin_resource_usage(acct_data);
  fail_unless(acct_data.find("resources_used.stormlight=12broams") != std::string::npos, acct_data.c_str());
  fail_unless(acct_data.find("resources_used.metals=10vials") != std::string::npos);
  fail_unless(acct_data.find("resources_used.planets_visited=3") != std::string::npos);
  fail_unless(acct_data.find("resources_used.breaths=10000") != std::string::npos);

  acct_data.clear();
  Json::Value usage;
  usage["aons_drawn"] = "20";
  usage["planets_visited"] = "4";
  pjob.set_plugin_resource_usage_from_json(usage);
  pjob.add_plugin_resource_usage(acct_data);
  fail_unless(acct_data.find("resources_used.stormlight=12broams") != std::string::npos, acct_data.c_str());
  fail_unless(acct_data.find("resources_used.metals=10vials") != std::string::npos);
  fail_unless(acct_data.find("resources_used.planets_visited=4") != std::string::npos);
  fail_unless(acct_data.find("resources_used.breaths=10000") != std::string::npos);
  fail_unless(acct_data.find("resources_used.aons_drawn=20") != std::string::npos);

  }
END_TEST



Suite *job_suite(void)
  {
  Suite *s = suite_create("job_suite methods");
  TCase *tc_core = tcase_create("constructors_test");
  tcase_add_test(tc_core, test_constructors);
  suite_add_tcase(s, tc_core);

  tc_core = tcase_create("test_plugin_things");
  tcase_add_test(tc_core, test_plugin_things);
  suite_add_tcase(s, tc_core);

  return s;
  }

void rundebug()
  {
  }

int main(void)
  {
  int number_failed = 0;
  SRunner *sr = NULL;

  init_job_attr_def();

  rundebug();
  sr = srunner_create(job_suite());
  srunner_set_log(sr, "job_suite.log");
  srunner_run_all(sr, CK_NORMAL);
  number_failed = srunner_ntests_failed(sr);
  srunner_free(sr);
  return number_failed;
  }
