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
  DCGM_GpuUsageInfo summaryGpuInfo;
  DCGM_GpuUsageInfo gpuInfo;
  std::string gpu_data;

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
  fail_unless(summaryGpuInfo.set_value("summary", gpu_data.c_str()) == PBSE_NONE);
  fail_unless(summaryGpuInfo.get_gpuId() == 2147483632);

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
  fail_unless(gpuInfo.get_gpuId() == 0);

  fail_unless(gpuInfo.set_value("windows", "panes") == PBSE_IVALREQ);

  }
END_TEST


START_TEST(test_constructor)
  {
  DCGM_GpuUsageInfo  default_gpuInfo;
  DCGM_GpuUsageInfo *initedGpuInfo;
  long long energyConsumed;
  dcgmGpuUsageInfo_t gpuUsage;

  fail_unless(default_gpuInfo.get_gpuId() == 0);
  default_gpuInfo.get_energyConsumed(energyConsumed);
  fail_unless(energyConsumed == 0);

  gpuUsage.gpuId = 6;
  gpuUsage.energyConsumed = 12345678;
  gpuUsage.pcieRxBandwidth.minValue = 0;  
  gpuUsage.pcieRxBandwidth.maxValue = 100;  
  gpuUsage.pcieRxBandwidth.average = 50;  
  gpuUsage.pcieTxBandwidth.minValue = 100;   
  gpuUsage.pcieTxBandwidth.maxValue = 1000;   
  gpuUsage.pcieTxBandwidth.average = 550;   
  gpuUsage.pcieReplays = 0;                
  gpuUsage.startTime = 12345;                   
  gpuUsage.endTime = 67890;                      
  gpuUsage.smUtilization.minValue = 9;    
  gpuUsage.smUtilization.maxValue = 20;    
  gpuUsage.smUtilization.average = 13;    
  gpuUsage.memoryUtilization.minValue = 80; 
  gpuUsage.memoryUtilization.maxValue = 800; 
  gpuUsage.memoryUtilization.average = 500; 
  gpuUsage.eccSingleBit = 0;           
  gpuUsage.eccDoubleBit = 0;            
  gpuUsage.memoryClock.minValue = 10;    
  gpuUsage.memoryClock.maxValue = 20;    
  gpuUsage.memoryClock.average = 15;    
  gpuUsage.smClock.minValue = 1000;         
  gpuUsage.smClock.maxValue = 10000;
  gpuUsage.smClock.average = 5000;

gpuUsage.numXidCriticalErrors = 10;                
  gpuUsage.xidCriticalErrorsTs[0] = 1;        
  gpuUsage.xidCriticalErrorsTs[1] = 2;        
  gpuUsage.xidCriticalErrorsTs[2] = 3;        
  gpuUsage.xidCriticalErrorsTs[3] = 4;        
  gpuUsage.xidCriticalErrorsTs[4] = 5;        
  gpuUsage.xidCriticalErrorsTs[5] = 6;        
  gpuUsage.xidCriticalErrorsTs[6] = 7;        
  gpuUsage.xidCriticalErrorsTs[7] = 9;        
  gpuUsage.xidCriticalErrorsTs[8] = 10;        
  gpuUsage.xidCriticalErrorsTs[9] = 15;        

gpuUsage.numComputePids = 10;             
  gpuUsage.computePids[0] = 1;    
  gpuUsage.computePids[1] = 2;    
  gpuUsage.computePids[2] = 3;    
  gpuUsage.computePids[3] = 4;    
  gpuUsage.computePids[4] = 5;    
  gpuUsage.computePids[5] = 6;    
  gpuUsage.computePids[6] = 7;    
  gpuUsage.computePids[7] = 9;    
  gpuUsage.computePids[8] = 10;   
  gpuUsage.computePids[9] = 15;   

  gpuUsage.numGraphicsPids = 10;
  gpuUsage.graphicsPids[0] = 1;      
  gpuUsage.graphicsPids[1] = 2;      
  gpuUsage.graphicsPids[2] = 3;      
  gpuUsage.graphicsPids[3] = 4;      
  gpuUsage.graphicsPids[4] = 5;      
  gpuUsage.graphicsPids[5] = 6;      
  gpuUsage.graphicsPids[6] = 7;      
  gpuUsage.graphicsPids[7] = 9;      
  gpuUsage.graphicsPids[8] = 10;     
  gpuUsage.graphicsPids[9] = 15;     
  
  gpuUsage.maxGpuMemoryUsed = 12349876500;         

  gpuUsage.powerViolationTime = 0;
  gpuUsage.thermalViolationTime = 0;
  gpuUsage.reliabilityViolationTime = 0;    
  gpuUsage.boardLimitViolationTime = 0;      
  gpuUsage.lowUtilizationTime = 0;            
  gpuUsage.syncBoostTime = 0; 

  initedGpuInfo = new DCGM_GpuUsageInfo(gpuUsage);
  fail_unless(initedGpuInfo->get_gpuId() == 6);
  initedGpuInfo->get_energyConsumed(energyConsumed);
  fail_unless(energyConsumed == 12345678);
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
