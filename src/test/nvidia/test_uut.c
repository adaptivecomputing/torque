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
#include "req.hpp"

int MOMNvidiaDriverVersion = 0;
int LOGLEVEL = 0;
int use_nvidia_gpu = TRUE;
int global_gpu_count = 0;
int time_now = 0;
char mom_host[PBS_MAXHOSTNAME + 1];

extern nvmlReturn_t global_nvmlDeviceGetFanSpeed_rc;
extern std::map<unsigned int, unsigned int> gpu_minor_to_gpu_index;
extern nvmlDevice_t global_device;
extern unsigned int global_device_minor_number;

int get_nvml_version();
void generate_server_gpustatus_nvml(std::vector<std::string> &gpu_status);
int build_gpu_minor_to_gpu_index_map(unsigned int);
int get_gpu_handle_by_minor(unsigned int, nvmlDevice_t*);

START_TEST(test_get_nvml_version)
  {
  int version;

  nvmlInit();

  version = get_nvml_version();
  fail_unless(version == NVML_API_VERSION);

  nvmlShutdown();
  }
END_TEST

START_TEST(test_generate_server_gpustatus_nvml)
  {
  std::vector<std::string> gpu_status;

  // force nvmlDeviceGetFanSpeed() to return error
  global_nvmlDeviceGetFanSpeed_rc = NVML_ERROR_UNKNOWN;
  LOGLEVEL = 2;

  generate_server_gpustatus_nvml(gpu_status);

  // expect log_buffer to hold something like
  //   "nvmlDeviceGetFanSpeed() called from generate_server_gpustatus_nvml (idx=0) Unknown error"
  fail_unless((strcasestr(log_buffer, "nvmlDeviceGetFanSpeed() called from generate_server_gpustatus_nvml") != NULL) &&
    (strcasestr(log_buffer, "unknown error") != NULL));
  }
END_TEST

START_TEST(test_build_gpu_minor_to_gpu_index_map)
  {
  nvmlDevice_t *handle;

  global_device_minor_number = 0;
  build_gpu_minor_to_gpu_index_map(2);
  fail_unless(global_device_minor_number == 3);
  fail_unless(gpu_minor_to_gpu_index[0] == 0);
  fail_unless(gpu_minor_to_gpu_index[1] == 1);

  get_gpu_handle_by_minor(0, handle);
  fail_unless(handle == &global_device);
  get_gpu_handle_by_minor(1, handle);
  fail_unless(handle == &global_device);
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

  tc_core = tcase_create("test_generate_server_gpustatus_nvml");
  tcase_add_test(tc_core, test_generate_server_gpustatus_nvml);
  suite_add_tcase(s, tc_core);

  tc_core = tcase_create("test_build_gpu_minor_to_gpu_index_map");
  tcase_add_test(tc_core, test_build_gpu_minor_to_gpu_index_map);
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
