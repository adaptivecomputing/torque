#include "license_pbs.h" /* See here for the software license */
#include <stdlib.h>
#include <stdio.h>
#include <set>
#include <hwloc.h>

#include "pbs_job.h"
#include "test_cpuset.h"
#include "pbs_error.h"

extern bool no_memory;

extern int init_torque_cpuset(void);

bool cpuStringFound = false;
bool memStringFound = false;
bool check_event = false;
void event_data(const char *d)
  {
  if(!strcmp(d,"cpus = 0-4,9")) cpuStringFound = true;
  if(!strcmp(d,"mems = 0-2,7,12-14")) memStringFound = true;
  }


START_TEST(init_torque_cpuset_test)
  {
  check_event = true;
  system("mkdir ./dev");
  system("mkdir ./dev/cpuset");
  system("mkdir ./dev/cpuset/torque");
  system("mkdir ./dev/cpuset/torque/dev");
  system("mkdir ./dev/cpuset/torque/dev/cpuset");
  system("mkdir ./dev/cpuset/torque/dev/cpuset/torque");
  system("cp cpus ./dev/cpuset/torque/dev/cpuset/torque/");
  system("cp mems ./dev/cpuset/torque/dev/cpuset/torque/");
  fail_unless(init_torque_cpuset() == 0);
  fail_unless(cpuStringFound);
  fail_unless(memStringFound);
  system("rm -rf ./dev");
  check_event = false;
  }
END_TEST



Suite *cpuset_suite(void)
  {
  Suite *s = suite_create("cpuset_suite methods");
  TCase *tc_core = tcase_create("memory_tests");
  tcase_add_test(tc_core, init_torque_cpuset_test);
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
