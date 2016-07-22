#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <vector>
#include <string>
#include <sstream>

#include "pbs_log.h"
#include <dcgm_agent.h>
#include <dcgm_fields.h>
#include "pbs_error.h"
#include "DCGM_job_gpu_stats.hpp"
#include "pbs_ifl.h"

char mom_name[PBS_MAXHOSTNAME+1];

DCGM_job_gpu_stats::DCGM_job_gpu_stats() : 
               version(0), numGpus(0), total_energy_consumed(0), total_eccSingleBit(0), total_eccDoubleBit(0)
  {
  this->summary.initializeSummary();
  }

DCGM_job_gpu_stats::DCGM_job_gpu_stats(dcgmJobInfo_t *jobInfo)
  {
  this->version = jobInfo->version;
  this->numGpus = jobInfo->numGpus;
  this->summary.initializeSummary(jobInfo->summary);

  for (unsigned int i = 0; i < numGpus; i++)
    {
    DCGM_GpuUsageInfo gpuInfo(jobInfo->gpus[i]);

    this->gpus.push_back(gpuInfo);
    }
  }

DCGM_job_gpu_stats::~DCGM_job_gpu_stats()
  {
  }

DCGM_job_gpu_stats& DCGM_job_gpu_stats::operator = (
  
  const DCGM_job_gpu_stats &other)

  {
  this->version = other.version;
  this->numGpus = other.numGpus;
  this->total_energy_consumed = other.total_energy_consumed;
  this->total_eccSingleBit = other.total_eccSingleBit;
  this->total_eccDoubleBit = other.total_eccDoubleBit;
  this->summary = other.summary;
  this->gpus = other.gpus;
  return *this;
  }


void DCGM_job_gpu_stats::get_values(

  std::vector<std::string> &names, 
  std::vector<std::string> &values)

  {
  char buf[128];
  std::string usage_info;

  if (this->version != 0)
    {
	  snprintf(buf, sizeof(buf), "version");
		names.push_back(buf);
		snprintf(buf, sizeof(buf), "%d", this->version);
		values.push_back(buf);
		}

  if (this->numGpus != 0)
	  {
		snprintf(buf, sizeof(buf), "numGpus");
		names.push_back(buf);
		snprintf(buf, sizeof(buf), "%d", this->numGpus);
		values.push_back(buf);
		}

  if (this->total_energy_consumed != 0)
    {
		snprintf(buf, sizeof(buf), "total_energy_consumed");
		names.push_back(buf);
		snprintf(buf, sizeof(buf), "%d", this->total_energy_consumed);
		values.push_back(buf);
		}

  snprintf(buf, sizeof(buf), "total_eccSingleBit_Errors");
  names.push_back(buf);
  snprintf(buf, sizeof(buf), "%d", this->total_eccSingleBit);
  values.push_back(buf);

  snprintf(buf, sizeof(buf), "total_eccDoubleBit_Errors");
  names.push_back(buf);
  snprintf(buf, sizeof(buf), "%d", this->total_eccDoubleBit);
  values.push_back(buf);

  unsigned int gpuId;
	this->summary.get_gpuId(gpuId);
	if (gpuId != 0)
	  {
		snprintf(buf, sizeof(buf), "summary");
		names.push_back(buf);
		this->summary.write_gpu_usage_info(usage_info);
		values.push_back(usage_info);
		}

  unsigned int i = 0;
  for (std::vector<DCGM_GpuUsageInfo>::iterator it = this->gpus.begin(); it != this->gpus.end(); it++)
    {
    unsigned int gpu_id;

    // If gpuId is large it is a summary
    gpu_id = it->get_gpuId();
    if (gpu_id > 8192)  // I don't know what the limit is. Right now you can have 16 gpus.
      {
      snprintf(buf, sizeof(buf), "summary:");
      names.push_back(buf);
      }
    else
      {
      snprintf(buf, sizeof(buf), "GPU:%d", i);
      names.push_back(buf);
      i++;
      }

    it->write_gpu_usage_info(usage_info);
    values.push_back(usage_info);
    }

	}


void DCGM_job_gpu_stats::initializeGpuJobInfo(

  dcgmJobInfo_t  &gpu_job_info)

	{
	this->version = gpu_job_info.version;
	this->numGpus = gpu_job_info.numGpus;
	this->summary.initializeSummary(gpu_job_info.summary);
  this->total_energy_consumed = 0;
  this->total_eccSingleBit = 0;
  this->total_eccDoubleBit = 0;
	
	for (unsigned int i = 0; i < this->numGpus; i++)
	  {
		DCGM_GpuUsageInfo gpuInfo(gpu_job_info.gpus[i]);

		this->gpus.push_back(gpuInfo);
		}
	}
  
void DCGM_job_gpu_stats::add_gpu_usage_info(

  DCGM_GpuUsageInfo *gpu_info)

  {
  this->gpus.push_back(*gpu_info);
  }

int DCGM_job_gpu_stats::set_value(

  const char *name, 
	const char *value) 

  {
  if (name == NULL || value == NULL)
    {
    return(PBSE_BAD_PARAMETER);
    }


  if (strcmp(name, "version"))
    {
    if (strcmp(name, "numGpus"))
      {
      if (strcmp(name, "total_energy_consumed"))
        {
        if (strcmp(name, "total_eccSingleBit_Errors"))
          {
          if (strcmp(name, "total_eccDoubleBit_Errors"))
            {
            if (strcmp(name, "summary"))
              {
              if (strcmp(name, "summary:")) // this is a subtle difference. This summary is in the gpus 
                {
                if (strncmp(name, "GPU:", 4))
                  {
                  return(PBSE_IVALREQ);
                  }
                else
                  {
                  DCGM_GpuUsageInfo *newGpu = new DCGM_GpuUsageInfo();

                  if (newGpu == NULL)
                    return(PBSE_MEM_MALLOC);

                  newGpu->set_value(name, value);
                  this->gpus.push_back(*newGpu);
                  }
                }
              else
                {
                DCGM_GpuUsageInfo *newGpu = new DCGM_GpuUsageInfo();

                if (newGpu == NULL)
                  return(PBSE_MEM_MALLOC);

                newGpu->set_value(name, value);
                this->gpus.push_back(*newGpu);
                }
              }
            else
              {
              this->summary.set_value(name, value);
              }
            }
          else
            {
            this->total_eccDoubleBit = strtoul(value, NULL, 10);
            }
          }
        else
          {
          this->total_eccSingleBit = strtoul(value, NULL, 10);
          }
        }
      else
        {
        this->total_energy_consumed = strtol(value, NULL, 10);
        }
      }
    else
      {
      this->numGpus = strtol(value, NULL, 10);
      }
    }
  else
    this->version = strtoul(value, NULL, 10);

  return(PBSE_NONE);
	}


void DCGM_job_gpu_stats::get_dcgm_version(

  unsigned int &version)

  {
  version = this->version;
  }

void DCGM_job_gpu_stats::get_dcgm_num_gpus(

  int& numGpus)

  {
  numGpus = this->numGpus;
  }

void DCGM_job_gpu_stats::get_summary_gpuId(

  unsigned int &gpuid)

  {
  gpuid = this->summary.get_gpuId();
  }

void DCGM_job_gpu_stats::get_totalEnergyConsumed(

  long long& total_energy_consumed)

  {
  total_energy_consumed = this->total_energy_consumed;
  }

void DCGM_job_gpu_stats::get_totalECCSingleBitErrors(

  unsigned int total_ecc_single_bit_errors)

  {
  total_ecc_single_bit_errors = this->total_eccSingleBit;
  }

void DCGM_job_gpu_stats::get_totalECCDoubleBitErrors(

  unsigned int total_ecc_double_bit_errors)

  {
  total_ecc_double_bit_errors = this->total_eccDoubleBit;
  }

