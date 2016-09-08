#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <vector>
#include <string>
#include <sstream>

#include "../lib/Liblog/pbs_log.h"
#include "pbs_error.h"
#include <dcgm_agent.h>
#include <dcgm_fields.h>
#include "utils.h"
#include "DCGM_job_gpu_stats.hpp"

#define  DCGM_array_sizes 10 // At the time this code was ritten this was the hard coded array sizes
                             // for xidCriticalErrorsTs, computePids and graphicsPids

extern char mom_name[];

DCGM_GpuUsageInfo::DCGM_GpuUsageInfo () : 
               gpuId(0), energyConsumed(0), pcieReplays(0), startTime(0), endTime(0), eccSingleBit(0), 
               eccDoubleBit(0), numXidCriticalErrors(0), numComputePids(0), maxGpuMemoryUsed(0), powerViolationTime(0),
               thermalViolationTime(0), reliabilityViolationTime(0), boardLimitViolationTime(0),
							 lowUtilizationTime(0), syncBoostTime(0)
	
	{
	this->pcieRxBandwidth.minValue = 0;
	this->pcieRxBandwidth.maxValue = 0;
	this->pcieRxBandwidth.average = 0;
 
	this->pcieTxBandwidth.minValue = 0;
	this->pcieTxBandwidth.maxValue = 0;
	this->pcieTxBandwidth.average = 0;

	this->smUtilization.minValue = 0;
	this->smUtilization.maxValue = 0;
	this->smUtilization.average = 0;
 
	this->memoryUtilization.minValue = 0;
	this->memoryUtilization.maxValue = 0;
	this->memoryUtilization.average = 0;
 
	this->memoryClock.minValue = 0;
	this->memoryClock.maxValue = 0;
	this->memoryClock.average = 0;

	this->smClock.minValue = 0;
	this->smClock.maxValue = 0;
	this->smClock.average = 0;

	for (unsigned int i = 0; i < DCGM_array_sizes; i++)
	  {
		this->xidCriticalErrorsTs[i] = 0;
		this->computePids[i] = 0;
		this->graphicsPids[i] = 0;
		}
	}

DCGM_GpuUsageInfo::DCGM_GpuUsageInfo(

  dcgmGpuUsageInfo_t& gpuUsageInfo)
 
  {
  this->this_mom_name = mom_name;
  this->gpuId = gpuUsageInfo.gpuId;
	this->energyConsumed = gpuUsageInfo.energyConsumed;

	this->pcieRxBandwidth.minValue = gpuUsageInfo.pcieRxBandwidth.minValue;
	this->pcieRxBandwidth.maxValue = gpuUsageInfo.pcieRxBandwidth.maxValue;
	this->pcieRxBandwidth.average = gpuUsageInfo.pcieRxBandwidth.average;
	
	this->pcieTxBandwidth.minValue = gpuUsageInfo.pcieTxBandwidth.minValue;
	this->pcieTxBandwidth.maxValue = gpuUsageInfo.pcieTxBandwidth.maxValue;
	this->pcieTxBandwidth.average = gpuUsageInfo.pcieTxBandwidth.average;

  this->pcieReplays = gpuUsageInfo.pcieReplays;
  this->startTime = gpuUsageInfo.startTime;
  this->endTime = gpuUsageInfo.endTime;

  this->smUtilization.minValue = gpuUsageInfo.smUtilization.minValue;
  this->smUtilization.maxValue = gpuUsageInfo.smUtilization.maxValue;
  this->smUtilization.average = gpuUsageInfo.smUtilization.average;

  this->memoryUtilization.minValue = gpuUsageInfo.memoryUtilization.minValue;
  this->memoryUtilization.maxValue = gpuUsageInfo.memoryUtilization.maxValue;
  this->memoryUtilization.average = gpuUsageInfo.memoryUtilization.average;

  this->eccSingleBit = gpuUsageInfo.eccSingleBit;
  this->eccDoubleBit = gpuUsageInfo.eccDoubleBit;

  this->memoryClock.minValue = gpuUsageInfo.memoryClock.minValue;
  this->memoryClock.maxValue = gpuUsageInfo.memoryClock.maxValue;
  this->memoryClock.average = gpuUsageInfo.memoryClock.average;

  this->smClock.minValue = gpuUsageInfo.smClock.minValue;
  this->smClock.maxValue = gpuUsageInfo.smClock.maxValue;
  this->smClock.average = gpuUsageInfo.smClock.average;

  this->numXidCriticalErrors = gpuUsageInfo.numXidCriticalErrors;

	for (unsigned int i = 0; i < DCGM_array_sizes; i++)
					this->xidCriticalErrorsTs[i] = gpuUsageInfo.xidCriticalErrorsTs[i];

  this->numComputePids = gpuUsageInfo.numComputePids;

	for (unsigned int i = 0; i < DCGM_array_sizes; i++)
					this->computePids[i] = gpuUsageInfo.computePidInfo[i].pid;

  this->numGraphicsPids = gpuUsageInfo.numGraphicsPids;

	for (unsigned int i = 0; i < DCGM_array_sizes; i++)
					this->graphicsPids[i] = gpuUsageInfo.graphicsPidInfo[i].pid;

  this->maxGpuMemoryUsed = gpuUsageInfo.maxGpuMemoryUsed;
  this->powerViolationTime = gpuUsageInfo.powerViolationTime;
  this->thermalViolationTime = gpuUsageInfo.thermalViolationTime;
  this->reliabilityViolationTime = gpuUsageInfo.reliabilityViolationTime;
  this->boardLimitViolationTime = gpuUsageInfo.boardLimitViolationTime;
  this->lowUtilizationTime = gpuUsageInfo.lowUtilizationTime;
  this->syncBoostTime = gpuUsageInfo.syncBoostTime;

	}

DCGM_GpuUsageInfo::~DCGM_GpuUsageInfo()
  {
	}

DCGM_GpuUsageInfo& DCGM_GpuUsageInfo::operator = (

  const DCGM_GpuUsageInfo& other)

  {
  this->this_mom_name = other.this_mom_name;
  this->gpuId = other.gpuId;
	this->energyConsumed = other.energyConsumed;

	this->pcieRxBandwidth.minValue = other.pcieRxBandwidth.minValue;
	this->pcieRxBandwidth.maxValue = other.pcieRxBandwidth.maxValue;
	this->pcieRxBandwidth.average = other.pcieRxBandwidth.average;
	
	this->pcieTxBandwidth.minValue = other.pcieTxBandwidth.minValue;
	this->pcieTxBandwidth.maxValue = other.pcieTxBandwidth.maxValue;
	this->pcieTxBandwidth.average = other.pcieTxBandwidth.average;

  this->pcieReplays = other.pcieReplays;
  this->startTime = other.startTime;
  this->endTime = other.endTime;

  this->smUtilization.minValue = other.smUtilization.minValue;
  this->smUtilization.maxValue = other.smUtilization.maxValue;
  this->smUtilization.average = other.smUtilization.average;

  this->memoryUtilization.minValue = other.memoryUtilization.minValue;
  this->memoryUtilization.maxValue = other.memoryUtilization.maxValue;
  this->memoryUtilization.average = other.memoryUtilization.average;

  this->eccSingleBit = other.eccSingleBit;
  this->eccDoubleBit = other.eccDoubleBit;

  this->memoryClock.minValue = other.memoryClock.minValue;
  this->memoryClock.maxValue = other.memoryClock.maxValue;
  this->memoryClock.average = other.memoryClock.average;

  this->smClock.minValue = other.smClock.minValue;
  this->smClock.maxValue = other.smClock.maxValue;
  this->smClock.average = other.smClock.average;

  this->numXidCriticalErrors = other.numXidCriticalErrors;

	for (unsigned int i = 0; i < DCGM_array_sizes; i++)
					this->xidCriticalErrorsTs[i] = other.xidCriticalErrorsTs[i];

  this->numComputePids = other.numComputePids;

	for (unsigned int i = 0; i < DCGM_array_sizes; i++)
					this->computePids[i] = other.computePids[i];

  this->numGraphicsPids = other.numGraphicsPids;

	for (unsigned int i = 0; i < DCGM_array_sizes; i++)
					this->graphicsPids[i] = other.graphicsPids[i];

  this->maxGpuMemoryUsed = other.maxGpuMemoryUsed;
  this->powerViolationTime = other.powerViolationTime;
  this->thermalViolationTime = other.thermalViolationTime;
  this->reliabilityViolationTime = other.reliabilityViolationTime;
  this->boardLimitViolationTime = other.boardLimitViolationTime;
  this->lowUtilizationTime = other.lowUtilizationTime;
  this->syncBoostTime = other.syncBoostTime;

  return *this; 
	}

void DCGM_GpuUsageInfo::initializeSummary()

  {
	this->gpuId = 0;
	this->energyConsumed = 0;
	}

void DCGM_GpuUsageInfo::initializeSummary(

  dcgmGpuUsageInfo_t &gpu_usage_info)

  {
  this->gpuId = gpu_usage_info.gpuId;
	this->energyConsumed = gpu_usage_info.energyConsumed;

	this->pcieRxBandwidth.minValue = gpu_usage_info.pcieRxBandwidth.minValue;
	this->pcieRxBandwidth.maxValue = gpu_usage_info.pcieRxBandwidth.maxValue;
	this->pcieRxBandwidth.average = gpu_usage_info.pcieRxBandwidth.average;
	
	this->pcieTxBandwidth.minValue = gpu_usage_info.pcieTxBandwidth.minValue;
	this->pcieTxBandwidth.maxValue = gpu_usage_info.pcieTxBandwidth.maxValue;
	this->pcieTxBandwidth.average = gpu_usage_info.pcieTxBandwidth.average;

  this->pcieReplays = gpu_usage_info.pcieReplays;
  this->startTime = gpu_usage_info.startTime;
  this->endTime = gpu_usage_info.endTime;

  this->smUtilization.minValue = gpu_usage_info.smUtilization.minValue;
  this->smUtilization.maxValue = gpu_usage_info.smUtilization.maxValue;
  this->smUtilization.average = gpu_usage_info.smUtilization.average;

  this->memoryUtilization.minValue = gpu_usage_info.memoryUtilization.minValue;
  this->memoryUtilization.maxValue = gpu_usage_info.memoryUtilization.maxValue;
  this->memoryUtilization.average = gpu_usage_info.memoryUtilization.average;

  this->eccSingleBit = gpu_usage_info.eccSingleBit;
  this->eccDoubleBit = gpu_usage_info.eccDoubleBit;

  this->memoryClock.minValue = gpu_usage_info.memoryClock.minValue;
  this->memoryClock.maxValue = gpu_usage_info.memoryClock.maxValue;
  this->memoryClock.average = gpu_usage_info.memoryClock.average;

  this->smClock.minValue = gpu_usage_info.smClock.minValue;
  this->smClock.maxValue = gpu_usage_info.smClock.maxValue;
  this->smClock.average = gpu_usage_info.smClock.average;

  this->numXidCriticalErrors = gpu_usage_info.numXidCriticalErrors;

	for (unsigned int i = 0; i < DCGM_array_sizes; i++)
					this->xidCriticalErrorsTs[i] = gpu_usage_info.xidCriticalErrorsTs[i];

  this->numComputePids = gpu_usage_info.numComputePids;

	for (unsigned int i = 0; i < DCGM_array_sizes; i++)
					this->computePids[i] = gpu_usage_info.computePidInfo[i].pid;

  this->numGraphicsPids = gpu_usage_info.numGraphicsPids;

	for (unsigned int i = 0; i < DCGM_array_sizes; i++)
					this->graphicsPids[i] = gpu_usage_info.graphicsPidInfo[i].pid;

  this->maxGpuMemoryUsed = gpu_usage_info.maxGpuMemoryUsed;
  this->powerViolationTime = gpu_usage_info.powerViolationTime;
  this->thermalViolationTime = gpu_usage_info.thermalViolationTime;
  this->reliabilityViolationTime = gpu_usage_info.reliabilityViolationTime;
  this->boardLimitViolationTime = gpu_usage_info.boardLimitViolationTime;
  this->lowUtilizationTime = gpu_usage_info.lowUtilizationTime;
  this->syncBoostTime = gpu_usage_info.syncBoostTime;


  }
  
void DCGM_GpuUsageInfo::write_gpu_usage_info(std::string& usage_info)
  {
  char buf[256];

  if (this->this_mom_name.empty())
    this->this_mom_name = mom_name;

  snprintf(buf, sizeof(buf), "{\"mom_name\": \"%s\", ", this->this_mom_name.c_str());
  usage_info = buf;

  snprintf(buf, sizeof(buf), "\"gpuId\": %u, ", this->gpuId);
	usage_info += buf;

	snprintf(buf, sizeof(buf), "\"energyConsumed\" :%lld, ", this->energyConsumed);
	usage_info += buf;

	snprintf(buf, sizeof(buf), "\"pcieRxBandwidth\": {");
	usage_info += buf;
  snprintf(buf, sizeof(buf), "\"minValue\" : %lld,", this->pcieRxBandwidth.minValue);
  usage_info += buf;
  snprintf(buf, sizeof(buf), "\"maxValue\" : %lld,", this->pcieRxBandwidth.maxValue);
  usage_info += buf;
  snprintf(buf, sizeof(buf), "\"average\" : %lld},", this->pcieRxBandwidth.average);
  usage_info += buf;

	snprintf(buf, sizeof(buf), "\"pcieTxBandwidth\": {");
	usage_info += buf;
  snprintf(buf, sizeof(buf), "\"minValue\":%lld,", this->pcieTxBandwidth.minValue);
  usage_info += buf;
  snprintf(buf, sizeof(buf), "\"maxValue\":%lld,", this->pcieTxBandwidth.maxValue);
  usage_info += buf;
  snprintf(buf, sizeof(buf), "\"average\":%lld},", this->pcieTxBandwidth.average);
  usage_info += buf;

  snprintf(buf, sizeof(buf), "\"pcieReplays\":%lld,", this->pcieReplays);
  usage_info += buf;

  snprintf(buf, sizeof(buf), "\"startTime\":%lld,", this->startTime);
  usage_info += buf;
  snprintf(buf, sizeof(buf), "\"endTime\":%lld,", this->endTime);
  usage_info += buf;

 	snprintf(buf, sizeof(buf), "\"smUtilization\": {");
	usage_info += buf;
  snprintf(buf, sizeof(buf), "\"minValue\":%d,", this->smUtilization.minValue);
  usage_info += buf;
  snprintf(buf, sizeof(buf), "\"maxValue\":%d,", this->smUtilization.maxValue);
  usage_info += buf;
  snprintf(buf, sizeof(buf), "\"average\":%d},", this->smUtilization.average);
  usage_info += buf;

 	snprintf(buf, sizeof(buf), "\"memoryUtilization\": {");
	usage_info += buf;
  snprintf(buf, sizeof(buf), "\"minValue\":%d,", this->memoryUtilization.minValue);
  usage_info += buf;
  snprintf(buf, sizeof(buf), "\"maxValue\":%d,", this->memoryUtilization.maxValue);
  usage_info += buf;
  snprintf(buf, sizeof(buf), "\"average\":%d},", this->memoryUtilization.average);
  usage_info += buf;

  snprintf(buf, sizeof(buf), "\"eccSingleBit\":%u,", this->eccSingleBit);
  usage_info += buf;

  snprintf(buf, sizeof(buf), "\"eccDoubleBit\":%u,", this->eccDoubleBit);
  usage_info += buf;

 	snprintf(buf, sizeof(buf), "\"memoryClock\": {");
	usage_info += buf;
  snprintf(buf, sizeof(buf), "\"minValue\":%d,", this->memoryClock.minValue);
  usage_info += buf;
  snprintf(buf, sizeof(buf), "\"maxValue\":%d,", this->memoryClock.maxValue);
  usage_info += buf;
  snprintf(buf, sizeof(buf), "\"average\":%d},", this->memoryClock.average);
  usage_info += buf;

 	snprintf(buf, sizeof(buf), "\"smClock\": {");
	usage_info += buf;
  snprintf(buf, sizeof(buf), "\"minValue\":%d,", this->smClock.minValue);
  usage_info += buf;
  snprintf(buf, sizeof(buf), "\"maxValue\":%d,", this->smClock.maxValue);
  usage_info += buf;
  snprintf(buf, sizeof(buf), "\"average\":%d},", this->smClock.average);
  usage_info += buf;

  snprintf(buf, sizeof(buf), "\"numXidCriticalErrors\":%d,", this->numXidCriticalErrors);
  usage_info += buf;

  if (this->numXidCriticalErrors != 0)
    {
    snprintf(buf, sizeof(buf), "\"xidCriticalErrorsTs\": [");
    usage_info += buf;
    for (unsigned int i = 0; i < this->numXidCriticalErrors; i++)
		  {
			/* Don't put a comma after the last entry in the array */
			if (i == this->numXidCriticalErrors - 1)
        snprintf(buf, sizeof(buf), "{\"%d\": %lld}", i, this->xidCriticalErrorsTs[i]);
			else
        snprintf(buf, sizeof(buf), "{\"%d\": %lld},", i, this->xidCriticalErrorsTs[i]);
      usage_info += buf;
			}
	  snprintf(buf, sizeof(buf), "],");
    usage_info += buf;
		}
 	
  snprintf(buf, sizeof(buf), "\"numComputePids\":%d,", this->numComputePids);
  usage_info += buf;

  if (this->numComputePids != 0)
    {
    snprintf(buf, sizeof(buf), "\"computePids\": [");
    usage_info += buf;
    for (unsigned int i = 0; i < this->numComputePids; i++)
		  {
			/* Don't put a comma after the last entry in the array */
			if (i == this->numComputePids - 1)
        snprintf(buf, sizeof(buf), "{\"a%d\": %u}", i, this->computePids[i]);
			else
        snprintf(buf, sizeof(buf), "{\"a%d\": %u},", i, this->computePids[i]);
      usage_info += buf;
			}
	  snprintf(buf, sizeof(buf), "],");
    usage_info += buf;
		}
 
  snprintf(buf, sizeof(buf), "\"numGraphicsPids\":%d,", this->numGraphicsPids);
  usage_info += buf;

  if (this->numGraphicsPids != 0)
    {
    snprintf(buf, sizeof(buf), "\"graphicsPids\": [");
    usage_info += buf;
    for (unsigned int i = 0; i < this->numGraphicsPids; i++)
		  {
			/* Don't put a comma after the last entry in the array */
			if (i == this->numGraphicsPids - 1)
        snprintf(buf, sizeof(buf), "{\"%d\": %u}", i, this->graphicsPids[i]);
			else
        snprintf(buf, sizeof(buf), "{\"%d\": %u},", i, this->graphicsPids[i]);
      usage_info += buf;
			}
	  snprintf(buf, sizeof(buf), "],");
    usage_info += buf;
		}
 
	snprintf(buf, sizeof(buf), "\"maxGpuMemoryUsed\":%lld, ", this->maxGpuMemoryUsed);
	usage_info += buf;

	snprintf(buf, sizeof(buf), "\"powerViolationTime\":%lld, ", this->powerViolationTime);
	usage_info += buf;

	snprintf(buf, sizeof(buf), "\"thermalViolationTime\":%lld, ", this->thermalViolationTime);
	usage_info += buf;

	snprintf(buf, sizeof(buf), "\"reliabilityViolationTime\":%lld, ", this->reliabilityViolationTime);
	usage_info += buf;

	snprintf(buf, sizeof(buf), "\"boardLimitViolationTime\":%lld, ", this->boardLimitViolationTime);
	usage_info += buf;

	snprintf(buf, sizeof(buf), "\"lowUtilizationTime\":%lld, ", this->lowUtilizationTime);
	usage_info += buf;

	snprintf(buf, sizeof(buf), "\"syncBoostTime\":%lld} ", this->syncBoostTime);
	usage_info += buf;

}

unsigned int  DCGM_GpuUsageInfo::get_gpuId()

  {
  return(this->gpuId);
  }

void DCGM_GpuUsageInfo::get_gpuId(unsigned int &gpuId)

  {
	gpuId = this->gpuId;
	}


int DCGM_GpuUsageInfo::parse_gpu_usage_array(

  std::string name, 
	std::string value)

	{
  char *headObjPtr = strdup(value.c_str());
  char *objPtr;
	std::string element_name;
	std::string element_value;

	objPtr = headObjPtr;
	if (*objPtr == '[')
		{
		objPtr++;
    move_past_whitespace(&objPtr);
		if (*objPtr == '{')
			{
			objPtr++;

			while ((objPtr != NULL) && (*objPtr != 0))
				{
				unsigned int index;
				get_name_value_pair(&objPtr, element_name, element_value);

				index = strtoul(element_name.c_str(), NULL, 10);
				if (name == "computePids")
				  computePids[index] = strtol(element_value.c_str(), NULL, 10);
        else if (name == "xidCriticalErrorsTs")
				  xidCriticalErrorsTs[index] = strtol(element_value.c_str(), NULL, 10);
        else if (name == "graphicsPids")
				  graphicsPids[index] = strtol(element_value.c_str(), NULL, 9);
                

				/* See if we have another object of if we are done */
        move_past_whitespace(&objPtr);
				if (*objPtr == ',')
					{
					/* we should have another object */
					objPtr++;
          move_past_whitespace(&objPtr);
					if (*objPtr == '{')
					  objPtr++;

					}
        else if (*objPtr == ']')
          objPtr = NULL;
				}
			if (headObjPtr != NULL)
				free(headObjPtr);
			}
		}
	return(PBSE_NONE);
	}

int DCGM_GpuUsageInfo::parse_gpu_usage_object(

  std::string name,
  std::string value)

  {
  char *headObjPtr = strdup(value.c_str());
  char *objPtr;
  std::string element_name;
  std::string element_value;
  int rc = PBSE_NONE;

  objPtr = headObjPtr;
  if (*objPtr == '{')
    {  
    objPtr++;
    move_past_whitespace(&objPtr);

    while ((objPtr != NULL) && (*objPtr != 0))
      {
      get_name_value_pair(&objPtr, element_name, element_value);
      if(element_name == "minValue")
        {
        if (name == "pcieRxBandwidth")
          this->pcieRxBandwidth.minValue = strtol(element_value.c_str(), NULL, 10);
        else if (name == "pcieTxBandwidth")
          this->pcieTxBandwidth.minValue = strtol(element_value.c_str(), NULL, 10);
        else if (name == "smUtilization")
          this->smUtilization.minValue = strtol(element_value.c_str(), NULL, 10);
        else if (name == "memoryUtilization")
          this->memoryUtilization.minValue = strtol(element_value.c_str(), NULL, 10);
        else if (name == "memoryClock")
          this->memoryClock.minValue = strtol(element_value.c_str(), NULL, 10);
        else if (name == "smClock")
          this->smClock.minValue = strtol(element_value.c_str(), NULL, 10);
        }
      else if(element_name == "maxValue")
        {
        if (name == "pcieRxBandwidth")
          this->pcieRxBandwidth.maxValue = strtol(element_value.c_str(), NULL, 10);
        else if (name == "pcieTxBandwidth")
          this->pcieTxBandwidth.maxValue = strtol(element_value.c_str(), NULL, 10);
        else if (name == "smUtilization")
          this->smUtilization.maxValue = strtol(element_value.c_str(), NULL, 10);
        else if (name == "memoryUtilization")
          this->memoryUtilization.maxValue = strtol(element_value.c_str(), NULL, 10);
        else if (name == "memoryClock")
          this->memoryClock.maxValue = strtol(element_value.c_str(), NULL, 10);
        else if (name == "smClock")
          this->smClock.maxValue = strtol(element_value.c_str(), NULL, 10);
        }
      else if(element_name == "average")
        {
        if (name == "pcieRxBandwidth")
          this->pcieRxBandwidth.average = strtol(element_value.c_str(), NULL, 10);
        else if (name == "pcieTxBandwidth")
          this->pcieTxBandwidth.average = strtol(element_value.c_str(), NULL, 10);
        else if (name == "smUtilization")
          this->smUtilization.average = strtol(element_value.c_str(), NULL, 10);
        else if (name == "memoryUtilization")
          this->memoryUtilization.average = strtol(element_value.c_str(), NULL, 10);
        else if (name == "memoryClock")
          this->memoryClock.average = strtol(element_value.c_str(), NULL, 10);
        else if (element_name == "smClock")
          this->smClock.average = strtol(element_value.c_str(), NULL, 10);
        }
      }
    if(headObjPtr != NULL)
      free(headObjPtr);
    }
  else
    {
      rc = PBSE_SYSTEM;
    }

    return rc;
  }
  

int DCGM_GpuUsageInfo::set_value(

  const char *name, 
	const char *value)

  {
	int rc = PBSE_NONE;
	char *valueHead;
	char *ptr;
	std::string valName;
	std::string valValue;

	valueHead = strdup(value);
	ptr = valueHead;

	if ((!strncmp(name, "GPU:", 4)) || (!strcmp(name, "summary")) || (!strcmp(name, "summary:")))
    {

    /* The main object should always start with an open bracket */
		if (*ptr == '{')
		  ptr++;
	  else
		  {
			free(valueHead);
		  return(PBSE_IVALREQ);
			}


		while ((ptr != NULL) && (*ptr != '\0'))
		  {
			get_name_value_pair(&ptr, valName, valValue);

      if (valName == "mom_name")
        {
        if (valValue.empty())
          valValue = mom_name;
        this->this_mom_name = valValue;
        }
			else if (valName == "gpuId")
			  {
				this->gpuId = strtoul(valValue.c_str(), NULL, 10);
				}
		  else if (valName == "energyConsumed")
			  {
				this->energyConsumed = strtol(valValue.c_str(), NULL, 10);
				}
		  else if (valName == "pcieRxBandwidth")
			  {
        parse_gpu_usage_object(valName, valValue);
        }
			else if (valName == "pcieTxBandwidth")
			  {
        parse_gpu_usage_object(valName, valValue);
        }
      else if (valName == "pcieReplays")
			  {
				this->pcieReplays = strtol(valValue.c_str(), NULL, 10);
				}
      else if (valName == "startTime")
			  {
				this->startTime = strtol(valValue.c_str(), NULL, 10);
				}
      else if (valName == "endTime")
			  {
				this->endTime = strtol(valValue.c_str(), NULL, 10);
				}
  		else if (valName == "smUtilization")
			  {
        parse_gpu_usage_object(valName, valValue);
        }
   		else if (valName == "memoryUtilization")
			  {
        parse_gpu_usage_object(valName, valValue);
        }
      else if (valName == "eccSingleBit")
			  {
				this->eccSingleBit = strtoul(valValue.c_str(), NULL, 10);
				}
      else if (valName == "eccDoubleBit")
			  {
				this->eccDoubleBit = strtoul(valValue.c_str(), NULL, 10);
				}
	  	else if (valName == "memoryClock")
			  {
        parse_gpu_usage_object(valName, valValue);
        }
    	else if (valName == "smClock")
			  {
        parse_gpu_usage_object(valName, valValue);
        }
      else if (valName == "numXidCriticalErrors")
			  {
        this->numXidCriticalErrors = strtol(valValue.c_str(), NULL, 10);
			  }
      else if (valName == "xidCriticalErrorsTs")
			  {
        parse_gpu_usage_array(valName, valValue); 
  		  }
	    else if (valName == "numComputePids")
			  {
			  this->numComputePids = strtoul(valValue.c_str(), NULL, 10);
			  }
		  else if (valName == "computePids")
			  {
        parse_gpu_usage_array(valName, valValue); 
  		  }
      else if (valName == "numGraphicsPids")
        {
        this->numGraphicsPids = strtoul(valValue.c_str(), NULL, 10);
        }
      else if (valName == "graphicsPids")
        {
        parse_gpu_usage_array(valName, valValue);
        }
      else if (valName == "maxGpuMemoryUsed")
        {
        this->maxGpuMemoryUsed = strtol(valValue.c_str(), NULL, 10);
        }
      else if (valName == "powerViolationTime")
        {
        this->powerViolationTime = strtol(valValue.c_str(), NULL, 10); 
        }
      else if (valName == "thermalViolationTime")
        {
        this->thermalViolationTime = strtol(valValue.c_str(), NULL, 10); 
        }
      else if (valName == "reliabilityViolationTime")
        {
        this->reliabilityViolationTime = strtol(valValue.c_str(), NULL, 10);
        }
      else if (valName == "boardLimitViolationTime")
        {
        this->boardLimitViolationTime = strtol(valValue.c_str(), NULL, 10);
        }
      else if (valName == "lowUtilizationTime")
        {
        this->lowUtilizationTime = strtol(valValue.c_str(), NULL, 10);
        }
      else if (valName == "syncBoostTime")
        {
        this->syncBoostTime  = strtol(valValue.c_str(), NULL, 10);
        }
      move_past_whitespace(&ptr);
      }
    }
  else
		  rc = PBSE_IVALREQ;
	


  free(valueHead);
	return(rc);

	}


void DCGM_GpuUsageInfo::get_energyConsumed(

  long long& energyConsumed)

  {
  energyConsumed = this->energyConsumed;
  }


void DCGM_GpuUsageInfo::get_eccSingleBitErrors(

  unsigned int& single_bit_errors)

  {
  single_bit_errors = this->eccSingleBit;
  }

void DCGM_GpuUsageInfo::get_eccDoubleBitErrors(

  unsigned int& double_bit_errors)

  {
  double_bit_errors = this->eccDoubleBit;
  }

 
