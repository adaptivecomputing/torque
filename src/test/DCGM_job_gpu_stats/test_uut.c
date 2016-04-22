#include <stdio.h>
#include <stdlib.h>
#include <check.h>

#include "pbs_config.h"
#include "complete_req.hpp"
#include "pbs_error.h"
#include "DCGM_job_gpu_stats.hpp"
#include <dcgm_agent.h>

extern int called_log_event;


START_TEST(test_set_get_value)
  {
  DCGM_job_gpu_stats gpuInfo;
  std::string gpu_data;

  fail_unless(gpuInfo.set_value("version", "10") == PBSE_NONE);
  fail_unless(gpuInfo.set_value("numGpus", "2") == PBSE_NONE);
  gpu_data = "{\"gpuId\": 2147483632, \"energyConsumed\" :28723, \"pcieRxBandwidth\": {\"minValue\" : 9223372036854775792, \
               \"maxValue\" : 9223372036854775792,\"average\" : 9223372036854775792},\"pcieTxBandwidth\": {\"minValue\":1, \
               \"maxValue\":9223372036854775792,\"average\":9223372036854775792},\"pcieReplays\":0,\"startTime\":0,\"endTime\" \
               :1461268216,\"smUtilization\": {\"minValue\":2147483632,\"maxValue\":2147483632,\"average\":2147483632}, \
               \"memoryUtilization\": {\"minValue\":2147483632,\"maxValue\":2147483632,\"average\":2147483632},\"eccSingleBit\":0,\"eccDoubleBit\" \
               :0,\"memoryClock\": {\"minValue\":2147483632,\"maxValue\":2147483632,\"average\":2147483632},\"smClock\": \
               {\"minValue\":2147483632,\"maxValue\":2147483632,\"average\":0},\"numXidCriticalErrors\":0,\"numComputePids\":0,\
               \"numGraphicsPids\":0,\"maxGpuMemoryUsed\":0, \"powerViolationTime\":0, \"thermalViolationTime\":1,\
               \"reliabilityViolationTime\":1, \"boardLimitViolationTime\":1, \"lowUtilizationTime\":1, \
               \"syncBoostTime\":0}";
  fail_unless(gpuInfo.set_value("summary", gpu_data.c_str()) == PBSE_NONE);

  gpu_data = "{\"gpuId\": 0, \"energyConsumed\" :28723, \"pcieRxBandwidth\": {\"minValue\" : 9223372036854775792, \
               \"maxValue\" : 9223372036854775792,\"average\" : 9223372036854775792},\"pcieTxBandwidth\": {\"minValue\":1, \
               \"maxValue\":9223372036854775792,\"average\":9223372036854775792},\"pcieReplays\":0,\"startTime\":0,\"endTime\" \
               :1461268216,\"smUtilization\": {\"minValue\":2147483632,\"maxValue\":2147483632,\"average\":2147483632}, \
               \"memoryUtilization\": {\"minValue\":2147483632,\"maxValue\":2147483632,\"average\":2147483632},\"eccSingleBit\":0,\"eccDoubleBit\" \
               :0,\"memoryClock\": {\"minValue\":2147483632,\"maxValue\":2147483632,\"average\":2147483632},\"smClock\": \
               {\"minValue\":2147483632,\"maxValue\":2147483632,\"average\":0},\"numXidCriticalErrors\":0,\"numComputePids\":0, \
               \"numGraphicsPids\":0,\"maxGpuMemoryUsed\":0, \"powerViolationTime\":0, \"thermalViolationTime\":9223372036854775794, \
               \"reliabilityViolationTime\":1, \"boardLimitViolationTime\":1, \"lowUtilizationTime\":1, \
               \"syncBoostTime\":0}";
  fail_unless(gpuInfo.set_value("GPU:0", gpu_data.c_str()) == PBSE_NONE);

  gpu_data = "{\"gpuId\": 1, \"energyConsumed\" :28723, \"pcieRxBandwidth\": {\"minValue\" : 9223372036854775792, \
               \"maxValue\" : 9223372036854775792,\"average\" : 9223372036854775792},\"pcieTxBandwidth\": {\"minValue\":1, \
               \"maxValue\":9223372036854775792,\"average\":9223372036854775792},\"pcieReplays\":0,\"startTime\":0,\"endTime\" \
               :1461268216,\"smUtilization\": {\"minValue\":2147483632,\"maxValue\":2147483632,\"average\":2147483632}, \
               \"memoryUtilization\": {\"minValue\":2147483632,\"maxValue\":2147483632,\"average\":2147483632},\"eccSingleBit\":0,\"eccDoubleBit\" \
               :0,\"memoryClock\": {\"minValue\":2147483632,\"maxValue\":2147483632,\"average\":2147483632},\"smClock\": \
               {\"minValue\":2147483632,\"maxValue\":2147483632,\"average\":0},\"numXidCriticalErrors\":0,\"numComputePids\":0, \
               \"numGraphicsPids\":0,\"maxGpuMemoryUsed\":0, \"powerViolationTime\":0, \"thermalViolationTime\":9223372036854775794, \
               \"reliabilityViolationTime\":1, \"boardLimitViolationTime\":1, \"lowUtilizationTime\":1, \
               \"syncBoostTime\":0}";
  fail_unless(gpuInfo.set_value("GPU:", gpu_data.c_str()) == PBSE_NONE);
  fail_unless(gpuInfo.set_value("windows", "panes") == PBSE_IVALREQ);

  std::vector<std::string> names;
  std::vector<std::string> values;

  gpuInfo.get_values(names, values);

  fail_unless(names[0] == "version");
  fail_unless(names[1] == "numGpus");
  fail_unless(names[2] == "summary");
  fail_unless(names[3] == "GPU:0");
  fail_unless(names[4] == "GPU:1");

  }
END_TEST


START_TEST(test_constructor)
  {
  DCGM_job_gpu_stats gpuInfo;
  DCGM_job_gpu_stats *initedGpuInfo;
  unsigned int version;
  unsigned int gpuId;
  int numGpus;
  dcgmJobInfo_t jobInfo;

  /* The default constructor should have set everything to 0 */
  gpuInfo.get_dcgm_version(version);
  gpuInfo.get_dcgm_num_gpus(numGpus);

  fail_unless(version == 0);
  fail_unless(numGpus == 0);

  /* Create an instance with the dcgmJobInfo_t information */
  jobInfo.version= 10;
  jobInfo.numGpus = 3;
  jobInfo.summary.gpuId = 4;

  initedGpuInfo = new DCGM_job_gpu_stats(&jobInfo);
  initedGpuInfo->get_dcgm_version(version);
  initedGpuInfo->get_dcgm_num_gpus(numGpus);
  initedGpuInfo->get_summary_gpuId(gpuId);
  
  fail_unless(version == 10);
  fail_unless(numGpus == 3);
  fail_unless(gpuId == 4);

  }
END_TEST


Suite *complete_req_suite(void)
  {
  Suite *s = suite_create("complete_req test suite methods");
  TCase *tc_core = tcase_create("test_set_get_value");
  tcase_add_test(tc_core, test_set_get_value);
  suite_add_tcase(s, tc_core);

  tc_core = tcase_create("test_constructor");
  tcase_add_test(tc_core, test_constructor);
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
  sr = srunner_create(complete_req_suite());
  srunner_set_log(sr, "complete_req_suite.log");
  srunner_run_all(sr, CK_NORMAL);
  number_failed = srunner_ntests_failed(sr);
  srunner_free(sr);
  return(number_failed);
  }
