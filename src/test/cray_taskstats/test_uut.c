#include "license_pbs.h" /* See here for the software license */
#include <pbs_config.h>
#include <stdlib.h>
#include <stdio.h>

#include "pbs_error.h"
#include "pbs_job.h"
#include "resource.h"

#include <check.h>

int  parse_rur_line(const std::string &line, std::set<std::string> &jobs_this_iteration);
void update_jobs_usage(job *pjob);
void read_rur_stats_file(const char *basepath);

const char *l1 = "2013-11-02T11:09:49.457770-05:00 c0-0c1s1n2 RUR 2417 p0-201311-1t153028 [RUR@34] uid: 12345, apid: 86989, jobid: 0, cmdname: /lus/tmp/rur01.2338/./CPU01-2338 plugin: taskstats ['utime', 100, 'stime', 100, 'max_rss', 1984, 'rchar', 107480, 'wchar', 90, 'exitcode:signal', ['0:0'], 'core', 0]";
const char *l2 = "2013-11-02T11:09:49.457770-05:00 c0-0c1s1n2 RUR 2417 p0-201311-1t153028 [RUR@34] uid: 12345, apid: 86989, jobid: 0, cmdname: /lus/tmp/rur01.2338/./CPU01-2338 plugin: taskstats ['utime', 300, 'stime', 0, 'max_rss', 4444, 'rchar', 107480, 'wchar', 90, 'exitcode:signal', ['0:0'], 'core', 0]";
const char *l3 = "2013-11-02T11:09:49.457770-05:00 c0-0c1s1n2 RUR 2417 p0-201311-1t153028 [RUR@34] uid: 12345, apid: 86989, jobid: 1, cmdname: /lus/tmp/rur01.2338/./CPU01-2338 plugin: taskstats ['utime', 300, 'stime', 300, 'max_rss', 555555, 'rchar', 107480, 'wchar', 90, 'exitcode:signal', ['0:0'], 'core', 0]";
const char *l4 = "2013-11-02T11:09:49.457770-05:00 c0-0c1s1n2 RUR 2417 p0-201311-1t153028 [RUR@34] uid: 12345, apid: 86989, jobid: 2, cmdname: /lus/tmp/rur01.2338/./CPU01-2338 plugin: taskstats ['utime', 400, 'stime', 300, 'max_rss', 5, 'rchar', 107480, 'wchar', 90, 'exitcode:signal', ['0:0'], 'core', 0]";



START_TEST(test_parse_rur_line)
  {
  job                   pjob;
  std::set<std::string> jobs_this_iteration;

  memset(&pjob, 0, sizeof(pjob));
  snprintf(pjob.ji_qs.ji_jobid, sizeof(pjob.ji_qs.ji_jobid), "0.roshar");

  fail_unless(parse_rur_line(l1, jobs_this_iteration) == PBSE_NONE);
  update_jobs_usage(&pjob);
  resource_def *rd = find_resc_def(svr_resc_def, "cput", svr_resc_size);
  resource *pres = add_resource_entry(pjob.ji_wattr + JOB_ATR_resc_used, rd);
  fail_unless(pres != NULL);
  fail_unless(pres->rs_value.at_val.at_long == 200);

  rd = find_resc_def(svr_resc_def, "mem", svr_resc_size);
  pres = add_resource_entry(pjob.ji_wattr + JOB_ATR_resc_used, rd);
  fail_unless(pres != NULL);
  fail_unless(pres->rs_value.at_val.at_size.atsv_num == 1984,
    "%llu", pres->rs_value.at_val.at_size.atsv_num);
 
  // Make sure we increase the value if we are using more later
  jobs_this_iteration.clear();
  fail_unless(parse_rur_line(l2, jobs_this_iteration) == PBSE_NONE);
  update_jobs_usage(&pjob);
  rd = find_resc_def(svr_resc_def, "cput", svr_resc_size);
  pres = add_resource_entry(pjob.ji_wattr + JOB_ATR_resc_used, rd);
  fail_unless(pres != NULL);
  fail_unless(pres->rs_value.at_val.at_long == 300);

  rd = find_resc_def(svr_resc_def, "mem", svr_resc_size);
  pres = add_resource_entry(pjob.ji_wattr + JOB_ATR_resc_used, rd);
  fail_unless(pres != NULL);
  fail_unless(pres->rs_value.at_val.at_size.atsv_num == 4444);

  // Make sure the values don't decrease if we find less later
  jobs_this_iteration.clear();
  fail_unless(parse_rur_line(l1, jobs_this_iteration) == PBSE_NONE);
  update_jobs_usage(&pjob);
  rd = find_resc_def(svr_resc_def, "cput", svr_resc_size);
  pres = add_resource_entry(pjob.ji_wattr + JOB_ATR_resc_used, rd);
  fail_unless(pres != NULL);
  fail_unless(pres->rs_value.at_val.at_long == 300);

  rd = find_resc_def(svr_resc_def, "mem", svr_resc_size);
  pres = add_resource_entry(pjob.ji_wattr + JOB_ATR_resc_used, rd);
  fail_unless(pres != NULL);
  fail_unless(pres->rs_value.at_val.at_size.atsv_num == 4444);
  }
END_TEST


START_TEST(test_read_rur_stats_file)
  {
  const char                                       *file = "sample_file";
  extern std::map<std::string, task_usage_info>     usage_information;
  std::map<std::string, task_usage_info>::iterator  it;

  usage_information.clear();

  read_rur_stats_file(file);

  it = usage_information.find("0");
  fail_unless(it != usage_information.end());
  fail_unless(it->second.cput == 30);
  fail_unless(it->second.mem == 30);
  
  it = usage_information.find("1");
  fail_unless(it != usage_information.end());
  fail_unless(it->second.cput == 1);
  fail_unless(it->second.mem == 2);
  
  it = usage_information.find("2");
  fail_unless(it != usage_information.end());
  fail_unless(it->second.cput == 13);
  fail_unless(it->second.mem == 8);
  
  it = usage_information.find("3");
  fail_unless(it != usage_information.end());
  fail_unless(it->second.cput == 8);
  fail_unless(it->second.mem == 9);
  }
END_TEST


Suite *cray_taskstats_suite(void)
  {
  Suite *s = suite_create("cray_taskstats_suite methods");
  TCase *tc_core = tcase_create("test_parse_rur_line");
  tcase_add_test(tc_core, test_parse_rur_line);
  suite_add_tcase(s, tc_core);

  tc_core = tcase_create("test_read_rur_stats_file");
  tcase_add_test(tc_core, test_read_rur_stats_file);
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
  rundebug();
  sr = srunner_create(cray_taskstats_suite());
  srunner_set_log(sr, "cray_taskstats_suite.log");
  srunner_run_all(sr, CK_NORMAL);
  number_failed = srunner_ntests_failed(sr);
  srunner_free(sr);
  return number_failed;
  }
