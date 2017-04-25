#include "license_pbs.h" /* See here for the software license */
#include <pbs_config.h>
#include <stdlib.h>
#include <stdio.h>

#include "pbs_error.h"
#include "pbs_job.h"
#include "resource.h"

#include <check.h>

int parse_rur_line(const std::string &line, mom_job *pjob);
const char *l1 = "2013-11-02T11:09:49.457770-05:00 c0-0c1s1n2 RUR 2417 p0-201311-1t153028 [RUR@34] uid: 12345, apid: 86989, jobid: 0, cmdname: /lus/tmp/rur01.2338/./CPU01-2338 plugin: taskstats ['utime', 100, 'stime', 100, 'max_rss', 1984, 'rchar', 107480, 'wchar', 90, 'exitcode:signal', ['0:0'], 'core', 0]";
const char *l2 = "2013-11-02T11:09:49.457770-05:00 c0-0c1s1n2 RUR 2417 p0-201311-1t153028 [RUR@34] uid: 12345, apid: 86989, jobid: 0, cmdname: /lus/tmp/rur01.2338/./CPU01-2338 plugin: taskstats ['utime', 300, 'stime', 0, 'max_rss', 4444, 'rchar', 107480, 'wchar', 90, 'exitcode:signal', ['0:0'], 'core', 0]";

START_TEST(test_parse_rur_line)
  {
  mom_job pjob;

  fail_unless(parse_rur_line(l1, &pjob) == PBSE_NONE);
  resource_def *rd = find_resc_def(svr_resc_def, "cput", svr_resc_size);
  resource *pres = add_resource_entry(pjob.get_attr(JOB_ATR_resc_used), rd);
  fail_unless(pres != NULL);
  fail_unless(pres->rs_value.at_val.at_long == 200);

  rd = find_resc_def(svr_resc_def, "mem", svr_resc_size);
  pres = add_resource_entry(pjob.get_attr(JOB_ATR_resc_used), rd);
  fail_unless(pres != NULL);
  fail_unless(pres->rs_value.at_val.at_size.atsv_num == 1984,
    "%llu", pres->rs_value.at_val.at_size.atsv_num);
 
  // Make sure we increase the value if we are using more later
  fail_unless(parse_rur_line(l2, &pjob) == PBSE_NONE);
  rd = find_resc_def(svr_resc_def, "cput", svr_resc_size);
  pres = add_resource_entry(pjob.get_attr(JOB_ATR_resc_used), rd);
  fail_unless(pres != NULL);
  fail_unless(pres->rs_value.at_val.at_long == 300);

  rd = find_resc_def(svr_resc_def, "mem", svr_resc_size);
  pres = add_resource_entry(pjob.get_attr(JOB_ATR_resc_used), rd);
  fail_unless(pres != NULL);
  fail_unless(pres->rs_value.at_val.at_size.atsv_num == 4444);

  // Make sure the values don't decrease if we find less later
  fail_unless(parse_rur_line(l1, &pjob) == PBSE_NONE);
  rd = find_resc_def(svr_resc_def, "cput", svr_resc_size);
  pres = add_resource_entry(pjob.get_attr(JOB_ATR_resc_used), rd);
  fail_unless(pres != NULL);
  fail_unless(pres->rs_value.at_val.at_long == 300);

  rd = find_resc_def(svr_resc_def, "mem", svr_resc_size);
  pres = add_resource_entry(pjob.get_attr(JOB_ATR_resc_used), rd);
  fail_unless(pres != NULL);
  fail_unless(pres->rs_value.at_val.at_size.atsv_num == 4444);
  }
END_TEST

START_TEST(test_two)
  {


  }
END_TEST

Suite *cray_taskstats_suite(void)
  {
  Suite *s = suite_create("cray_taskstats_suite methods");
  TCase *tc_core = tcase_create("test_parse_rur_line");
  tcase_add_test(tc_core, test_parse_rur_line);
  suite_add_tcase(s, tc_core);

  tc_core = tcase_create("test_two");
  tcase_add_test(tc_core, test_two);
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
