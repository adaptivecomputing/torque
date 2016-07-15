#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <check.h>

#include "pbs_config.h"
#include "pbs_ifl.h"
#include "attribute.h"
#include "pbs_error.h"
#include "DCGM_job_gpu_stats.hpp"


START_TEST(test_set_dcgm_gpu_use)
  {
  pbs_attribute patr;
  pbs_attribute new_patr;
  DCGM_job_gpu_stats  gpuStats;
  DCGM_job_gpu_stats *copiedGpuStats;
  unsigned int version;
  int numGpus;
  int rc;

  memset(&patr, 0, sizeof(pbs_attribute));
  memset(&new_patr, 0, sizeof(pbs_attribute));

  /* new_patr.at_val.at_ptr is NULL */
  rc = set_dcgm_gpu_use(&patr, &new_patr, SET);
  fail_unless(rc == PBSE_BAD_PARAMETER);

  new_patr.at_val.at_ptr = &gpuStats;
  /* patr.at_val.at_ptr is NULL. It should not be
   * NULL when returned */
  rc = set_dcgm_gpu_use(&patr, &new_patr, SET);
  fail_unless(rc == PBSE_NONE);
  fail_unless(patr.at_val.at_ptr != NULL);

  /* free the dcgm attribute and the
   * set a new one again and set a real
   * value
   */
  free_dcgm_gpu_use(&patr);
  fail_unless(patr.at_val.at_ptr == NULL);

  gpuStats.set_value("version", "10");
  rc = set_dcgm_gpu_use(&patr, &new_patr, SET);
  fail_unless(rc == PBSE_NONE);
  fail_unless(patr.at_val.at_ptr != NULL);


  copiedGpuStats = (DCGM_job_gpu_stats *)patr.at_val.at_ptr;
  copiedGpuStats->get_dcgm_version(version);
  fail_unless(version == 10);

  /* If it is set again there should be two values */
  gpuStats.set_value("numGpus", "2");
  rc = set_dcgm_gpu_use(&patr, &new_patr, SET);
  fail_unless(rc == PBSE_NONE);
  fail_unless(patr.at_val.at_ptr != NULL);
  copiedGpuStats->get_dcgm_version(version);
  fail_unless(version == 10);
  copiedGpuStats->get_dcgm_num_gpus(numGpus);
  fail_unless(numGpus == 2);

  /* Test unset for the attribute. It should 
   * set patr.at_val.at_ptr to NULL 
   */
  rc = set_dcgm_gpu_use(&patr, &new_patr, UNSET);
  fail_unless(rc == PBSE_NONE);
  

  }
END_TEST


START_TEST(test_decode_encode_dcgm_gpu_use)
  {
  pbs_attribute patr;
  const char    name[] = "dcgm_gpu_use";
  char          rescn[30];
  char          val[2048];
  int           perm;
  int           rc;
  DCGM_job_gpu_stats *dcgmOutput;

  strcpy(rescn, "version");
  strcpy(val, "1678361");

  memset(&patr, 0, sizeof(pbs_attribute));

  rc = decode_dcgm_gpu_use(&patr, name, rescn, val, perm);
  fail_unless(rc == PBSE_NONE);

  dcgmOutput = (DCGM_job_gpu_stats *)patr.at_val.at_ptr;
  unsigned int version;
  dcgmOutput->get_dcgm_version(version);
  fail_unless(version == 1678361);

  strcpy(rescn, "numGpus");
  strcpy(val, "2");
  rc = decode_dcgm_gpu_use(&patr, name, rescn, val, perm);
  fail_unless(rc == PBSE_NONE);

  int num_gpus;
  dcgmOutput->get_dcgm_num_gpus(num_gpus);
  fail_unless(num_gpus == 2);

  strcpy(rescn, "summary");
  strcpy(val, "{\"gpuId\": 2147483632, \"energyConsumed\" :28723, \"pcieRxBandwidth\": {\"minValue\" : 9223372036854775792, \
      \"maxValue\" : 9223372036854775792,\"average\" : 9223372036854775792},\"pcieTxBandwidth\": {\"minValue\":1, \
      \"maxValue\":9223372036854775792,\"average\":9223372036854775792},\"pcieReplays\":0,\"startTime\":0,\"endTime\" \
      :1461268216,\"smUtilization\": {\"minValue\":2147483632,\"maxValue\":2147483632,\"average\":2147483632}, \
      \"memoryUtilization\": {\"minValue\":2147483632,\"maxValue\":2147483632,\"average\":2147483632},\"eccSingleBit\":0,\"eccDoubleBit\" \
      :0,\"memoryClock\": {\"minValue\":2147483632,\"maxValue\":2147483632,\"average\":2147483632},\"smClock\": \
      {\"minValue\":2147483632,\"maxValue\":2147483632,\"average\":0},\"numXidCriticalErrors\":0,\"numComputePids\":0,\
      \"numGraphicsPids\":0,\"maxGpuMemoryUsed\":0, \"powerViolationTime\":0, \"thermalViolationTime\":1,\
      \"reliabilityViolationTime\":1, \"boardLimitViolationTime\":1, \"lowUtilizationTime\":1, \
      \"syncBoostTime\":0}");
  rc = decode_dcgm_gpu_use(&patr, name, rescn, val, perm);
  fail_unless(rc == PBSE_NONE);

  unsigned int gpuId;
  dcgmOutput->get_summary_gpuId(gpuId);
  fail_unless(gpuId == 2147483632);

  strcpy(rescn, "GPU:0");
  strcpy(val, "{\"gpuId\": 0, \"energyConsumed\" :28723, \"pcieRxBandwidth\": {\"minValue\" : 9223372036854775792, \
      \"maxValue\" : 9223372036854775792,\"average\" : 9223372036854775792},\"pcieTxBandwidth\": {\"minValue\":1, \
      \"maxValue\":9223372036854775792,\"average\":9223372036854775792},\"pcieReplays\":0,\"startTime\":0,\"endTime\" \
      :1461268216,\"smUtilization\": {\"minValue\":2147483632,\"maxValue\":2147483632,\"average\":2147483632}, \
      \"memoryUtilization\": {\"minValue\":2147483632,\"maxValue\":2147483632,\"average\":2147483632},\"eccSingleBit\":0,\"eccDoubleBit\" \
      :0,\"memoryClock\": {\"minValue\":2147483632,\"maxValue\":2147483632,\"average\":2147483632},\"smClock\": \
      {\"minValue\":2147483632,\"maxValue\":2147483632,\"average\":0},\"numXidCriticalErrors\":0,\"numComputePids\":0, \
      \"numGraphicsPids\":0,\"maxGpuMemoryUsed\":0, \"powerViolationTime\":0, \"thermalViolationTime\":9223372036854775794, \
      \"reliabilityViolationTime\":1, \"boardLimitViolationTime\":1, \"lowUtilizationTime\":1, \
      \"syncBoostTime\":0}");

  rc = decode_dcgm_gpu_use(&patr, name, rescn, val, perm);
  fail_unless(rc == PBSE_NONE);

  strcpy(rescn, "GPU:1");
  strcpy(val, "{\"gpuId\": 1, \"energyConsumed\" :28724, \"pcieRxBandwidth\": {\"minValue\" : 9223372036854775792, \
      \"maxValue\" : 9223372036854775792,\"average\" : 9223372036854775792},\"pcieTxBandwidth\": {\"minValue\":1, \
      \"maxValue\":9223372036854775792,\"average\":9223372036854775792},\"pcieReplays\":0,\"startTime\":0,\"endTime\" \
      :1461268216,\"smUtilization\": {\"minValue\":2147483632,\"maxValue\":2147483632,\"average\":2147483632}, \
      \"memoryUtilization\": {\"minValue\":2147483632,\"maxValue\":2147483632,\"average\":2147483632},\"eccSingleBit\":0,\"eccDoubleBit\" \
      :0,\"memoryClock\": {\"minValue\":2147483632,\"maxValue\":2147483632,\"average\":2147483632},\"smClock\": \
      {\"minValue\":2147483632,\"maxValue\":2147483632,\"average\":0},\"numXidCriticalErrors\":0,\"numComputePids\":0, \
      \"numGraphicsPids\":0,\"maxGpuMemoryUsed\":0, \"powerViolationTime\":0, \"thermalViolationTime\":9223372036854775794, \
      \"reliabilityViolationTime\":1, \"boardLimitViolationTime\":1, \"lowUtilizationTime\":1, \
      \"syncBoostTime\":0}");

  rc = decode_dcgm_gpu_use(&patr, name, rescn, val, perm);
  fail_unless(rc == PBSE_NONE);

  /* Now test the encode functions */
  tlist_head attrs;
  rc = encode_dcgm_gpu_use(&patr, &attrs, ATTR_dcgm_gpu_use, NULL, 0,0);
  fail_unless(rc == PBSE_NONE);

  free_dcgm_gpu_use(&patr);
  fail_unless(patr.at_val.at_ptr == NULL);

  }
END_TEST



Suite *attr_fn_dcgm_gpu_use_suite(void)
  {
  Suite *s = suite_create("attr_fn_dcgm_gpu_use test suite methods");
  TCase *tc_core = tcase_create("test_free_dcgm_gpu_use");
  tcase_add_test(tc_core, test_set_dcgm_gpu_use);
  suite_add_tcase(s, tc_core);
  
  tc_core = tcase_create("test_decode_encode_dcgm_gpu_use");
  tcase_add_test(tc_core, test_decode_encode_dcgm_gpu_use);
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
  sr = srunner_create(attr_fn_dcgm_gpu_use_suite());
  srunner_set_log(sr, "attr_fn_dcgm_gpu_use_suite.log");
  srunner_run_all(sr, CK_NORMAL);
  number_failed = srunner_ntests_failed(sr);
  srunner_free(sr);
  return(number_failed);
  }
