#include <stdio.h>
#include <stdlib.h>
#include <sstream>
#include <check.h>

#include "pbs_job.h"

extern int encode_used_ctr;
extern int encode_flagged_attrs_ctr;
extern int MOMCudaVisibleDevices;

void add_job_status_information(job &pjob, std::stringstream &list);
u_long setcudavisibledevices(const char *value);


START_TEST(test_add_job_status_information)
  {
  std::stringstream list;
  job pjob;

  encode_used_ctr = 0;
  encode_flagged_attrs_ctr = 0;
  add_job_status_information(pjob, list);

  fail_unless(!strcmp(list.str().c_str(), "()"));
  }
END_TEST


START_TEST(test_setcudavisibledevices)
  {
  char  curr[3];
  char  *cp;
  std::stringstream output;

  setcudavisibledevices("1");
  fail_unless(MOMCudaVisibleDevices == 1, "did not set cuda_visble_devices to 1");

  setcudavisibledevices("0");
  fail_unless(MOMCudaVisibleDevices == 0, "did not set cuda_visble_devices to 0");
  }
END_TEST


Suite *parse_config_suite(void)
  {
  Suite *s = suite_create("parse_config test suite methods");
  TCase *tc_core = tcase_create("test_add_job_status_information");
  tcase_add_test(tc_core, test_add_job_status_information);
  suite_add_tcase(s, tc_core);

  tc_core = tcase_create("test_setcudavisibledevices");
  tcase_add_test(tc_core, test_setcudavisibledevices);
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
  sr = srunner_create(parse_config_suite());
  srunner_set_log(sr, "parse_config_suite.log");
  srunner_run_all(sr, CK_NORMAL);
  number_failed = srunner_ntests_failed(sr);
  srunner_free(sr);
  return(number_failed);
  }
