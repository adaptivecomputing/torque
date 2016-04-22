#include <stdio.h>
#include <pbs_config.h>
#include <stdlib.h>
#include <errno.h>
#include <dcgm_agent.h>

#include "pbs_log.h"
#include "pbs_job.h"
#include "mcom.h"
#include "DCGM_job_gpu_stats.hpp"
#include "nvml.h"

bool nvml_init_fail = false;

// Global declarations

dcgmHandle_t pDcgmHandle;
int MOMNvidiaDriverVersion = 0;
int use_nvidia_gpu = TRUE;
int LOGLEVEL=1;
char log_buffer[];
char mom_host[256];
time_t time_now;


nvmlReturn_t nvmlInit()
  {
   if (nvml_init_fail == false)
    return(NVML_SUCCESS);
  else
    return(NVML_ERROR_UNINITIALIZED);
  }

nvmlReturn_t nvmlDeviceGetCount(unsigned int *deviceCount)
  {
  *deviceCount = 2;

  if (nvml_init_fail == false)
    return(NVML_SUCCESS);
  else
    return(NVML_ERROR_UNINITIALIZED);
  }

int MXMLFromString(mxml_t **EP, char *XMLString, char **Tail, char *Emsg, int emsg_size)
  {
  return(0);
  }

void log_err(int errnum, const char *routine, char *test)
  {
  return;
  }

DCGM_GpuUsageInfo::DCGM_GpuUsageInfo()
  {}

DCGM_job_gpu_stats::DCGM_job_gpu_stats()
  {
  }

void DCGM_job_gpu_stats::initializeGpuJobInfo(

  dcgmJobInfo_t  &gpu_job_info)

  {
  return;
  }

DCGM_GpuUsageInfo::~DCGM_GpuUsageInfo()
  {
  }

int MXMLDestroyE(mxml_t **EP) 
  {
  return(0);
  }

dcgmReturn_t dcgmGroupAddDevice(dcgmHandle_t pDcgmHandle, dcgmGpuGrp_t groupId, unsigned int gpuId)
  {
  return(DCGM_ST_OK);
  }

dcgmReturn_t dcgmJobStopStats(dcgmHandle_t pDcgmHandle, char jobId[64])
  {
  return(DCGM_ST_OK);
  }

dcgmReturn_t DECLDIR dcgmGroupCreate(dcgmHandle_t pDcgmHandle, dcgmGroupType_t type, char *groupName,
            dcgmGpuGrp_t *pDcgmGrpId)
  {
  return(DCGM_ST_OK);
  }


dcgmReturn_t dcgmGroupGetInfo(dcgmHandle_t pDcgmHandle, dcgmGpuGrp_t groupId, dcgmGroupInfo_t *pDcgmGroupInfo)
  {
  return(DCGM_ST_OK);
  }


dcgmReturn_t DECLDIR dcgmGroupDestroy(dcgmHandle_t pDcgmHandle, dcgmGpuGrp_t groupId)
  {
  return(DCGM_ST_OK);
  }

dcgmReturn_t dcgmJobGetStats(dcgmHandle_t pDcgmHandle, char jobId[64], dcgmJobInfo_t *pJobInfo)
  {
  return(DCGM_ST_OK);
  }

dcgmReturn_t dcgmJobStartStats(dcgmHandle_t pDcgmHandle, dcgmGpuGrp_t groupId, char jobId[64])
  {
  return(DCGM_ST_OK);
  }

void capture_until_close_character(

      char        **start,
        std::string  &storage,
          char          end)

  {
  if ((start == NULL) ||
      (*start == NULL))
    return;

  char *val = *start;
  char *ptr = strchr(val, end);

  // Make sure we found a close quote and this wasn't an empty string
  if ((ptr != NULL) &&
      (ptr != val))
    {
    storage = val;
    storage.erase(ptr - val);
    *start = ptr + 1; // add 1 to move past the character
    }
  }


int is_whitespace(

  char c)

  {
  if ((c == ' ')  ||
      (c == '\n') ||
      (c == '\t') ||
      (c == '\r') ||
      (c == '\f'))
    return(TRUE);
  else
    return(FALSE);
  } /* END is_whitespace */



void move_past_whitespace(

  char **str)

  {
  if ((str == NULL) ||
      (*str == NULL))
    return;

  char *current = *str;

  while (is_whitespace(*current) == TRUE)
    current++;

  *str = current;
  } // END move_past_whitespace()
          

void add_range_to_string(

  std::string &range_string,
  int          begin,
  int          end)

  {
  char buf[MAXLINE];

  if (begin == end)
    {
    if (range_string.size() == 0)
      sprintf(buf, "%d", begin);
    else
      sprintf(buf, ",%d", begin);
    }
  else
    {
    if (range_string.size() == 0)
      sprintf(buf, "%d-%d", begin, end);
    else
      sprintf(buf, ",%d-%d", begin, end);
    }

  range_string += buf;
  }

void translate_vector_to_range_string(

  std::string            &range_string,
  const std::vector<int> &indices)

  {
  // range_string starts empty
  range_string.clear();
  
  if (indices.size() == 0)
    return;
  
  int first = indices[0];
  int prev = first;
  
  for (unsigned int i = 1; i < indices.size(); i++)
    {
    if (indices[i] == prev + 1)
      {
      // Still in a consecutive range
      prev = indices[i];
      }
    else
      {
      add_range_to_string(range_string, first, prev);
  
      first = prev = indices[i];
      }
    }
  
  // output final piece
  add_range_to_string(range_string, first, prev);
    //
  }


void translate_range_string_to_vector(

  const char       *range_string,
  std::vector<int> &indices)

  {
  char *str = strdup(range_string);
  char *ptr = str;
  int   prev = 0;
  int   curr;

  while (is_whitespace(*ptr))
    ptr++;

  while (*ptr != '\0')
    {
    prev = strtol(ptr, &ptr, 10);

    if (*ptr == '-')
      {
      ptr++;
      curr = strtol(ptr, &ptr, 10);

      while (prev <= curr)
        {
        indices.push_back(prev);
        prev++;
        }

      while ((*ptr == ',') ||
             (is_whitespace(*ptr)))
        ptr++;
      }
    else
     {
     indices.push_back(prev);

      while ((*ptr == ',') ||
           (is_whitespace(*ptr)))
      ptr++;
     }

    if (str == ptr)
      break;
    }

  free(str);
  }

bool task_hosts_match(const char *task_host, const char *this_hostname)
  {
  if (strcmp(task_host, this_hostname))
    {
    /* see if the short name might match */
    char task_hostname[PBS_MAXHOSTNAME];
    char local_hostname[PBS_MAXHOSTNAME];
    char *dot_ptr;

    strcpy(task_hostname, task_host);
    strcpy(local_hostname, this_hostname);

    dot_ptr = strchr(task_hostname, '.');
    if (dot_ptr != NULL)
      *dot_ptr = '\0';

    dot_ptr = strchr(local_hostname, '.');
    if (dot_ptr != NULL)
      *dot_ptr = '\0';

    if (strcmp(task_hostname, local_hostname))
      {
      /* this task does not belong to this host. Go to the next one */
      return(false);
      }
    }
  return(true);
  }

#include "../../lib/Libattr/complete_req.cpp"
#include "../../lib/Libattr/req.cpp"
#include "../../lib/Libutils/allocation.cpp"
//#include "../../lib/Libattr/DCGM_GpuUsageInfo.cpp"
//#include "../../lib/Libattr/DCGM_job_gpu_stats.cpp"

bool have_incompatible_dash_l_resource(job *pjob)
  {
  return(false);
  }

void log_err(int errnum, const char *routine, const char *text)
  {
  return;
  }

void log_ext(int errnum, const char *routine, const char *text, int severity)
  {
  return;
  }

void log_event(int eventtype, int objclass, const char *objname, const char *text)
  {
  return;
  }

void send_update_soon()
  {
  return;
  }

void get_device_indices(

  const char *device_str,
  std::vector<unsigned int> &device_indices,
  const char *suffix)
  {}
