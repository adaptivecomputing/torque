#ifndef __NVIDIA_H__
#define __NVIDIA_H__
#include "license_pbs.h"

int nvidia_dcgm_create_gpu_job_info(job *pjob);

int nvidia_dcgm_finalize_gpu_job_info(job *pjob); 

int nvidia_dcgm_get_sister_job_info(job *pjob, struct tcp_chan *chan);
#endif
