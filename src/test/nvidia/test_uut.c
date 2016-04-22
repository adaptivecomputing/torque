#include "license_pbs.h" /* See here for the software license */
#include "pbs_config.h"
#include "lib_ifl.h"
#include <stdlib.h>
#include <stdio.h>
#include <nvml.h>
#include <hwloc/nvml.h>
#include <hwloc.h>
#include <check.h>


#include "pbs_log.h"
#include "pbs_error.h"
#include "mcom.h"
#include "mom_server_lib.h"
#include <dcgm_agent.h>
#include <nvml.h>

extern bool nvml_init_fail;


START_TEST(test_init_nvidia_nvml)
  {
  int rc;
  unsigned int device_count;

  nvml_init_fail = false;
  rc = init_nvidia_nvml(device_count);
  fail_unless(rc == NVML_SUCCESS);
  fail_unless(device_count == 2);

  // Test the failure case for nvmlInit()
  nvml_init_fail = true;
  rc = init_nvidia_nvml(device_count);
  fail_unless(rc == NVML_ERROR_UNINITIALIZED);

  }
END_TEST


Suite *dec_attrl_suite(void)
  {
  Suite *s = suite_create("nvidia methods");
  TCase *tc_core = tcase_create("test_init_nvidia_nvml");
  tcase_add_test(tc_core, test_init_nvidia_nvml);
  suite_add_tcase(s, tc_core);


//  tc_core = tcase_create("test_two");
//  tcase_add_test(tc_core, test_two);
//  suite_add_tcase(s, tc_core);

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
  sr = srunner_create(dec_attrl_suite());
  srunner_set_log(sr, "nvidia.log");
  srunner_run_all(sr, CK_NORMAL);
  number_failed = srunner_ntests_failed(sr);
  srunner_free(sr);
  return number_failed;
  }
