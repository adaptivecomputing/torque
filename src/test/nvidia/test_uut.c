#include "license_pbs.h" /* See here for the software license */
#include "mom_mach.h"
#include "test_nvidia.h"
#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include <list>
#include <nvml.h>

#include "pbs_error.h"
#include "pbs_job.h"

int MOMNvidiaDriverVersion = 0;
int LOGLEVEL = 0;
int use_nvidia_gpu = TRUE;
int global_gpu_count = 0;
int time_now = 0;
char mom_host[PBS_MAXHOSTNAME + 1];

int get_nvml_version();

START_TEST(test_get_nvml_version)
  {
  int version;

  nvmlInit();

  version = get_nvml_version();
  fail_unless(version == NVML_API_VERSION);

  nvmlShutdown();
  }
END_TEST

START_TEST(test_two)
  {
  }
END_TEST

Suite *nvidia_suite(void)
  {
  Suite *s = suite_create("nvidia_suite methods");
  TCase *tc_core = tcase_create("test_get_nvml_version");

  // give this test a bit of extra time since nvmlInit() can take awhile
  tcase_set_timeout(tc_core, 15);

  tcase_add_test(tc_core, test_get_nvml_version);
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
  sr = srunner_create(nvidia_suite());
  srunner_set_log(sr, "nvidia_suite.log");
  srunner_run_all(sr, CK_NORMAL);
  number_failed = srunner_ntests_failed(sr);
  srunner_free(sr);
  return number_failed;
  }
