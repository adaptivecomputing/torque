#include "license_pbs.h" /* See here for the software license */
#include <stdlib.h>
#include <stdio.h> /* fprintf */
#include <sys/ioctl.h> /* winsize */
#include <termios.h> /* termios */
#include <vector>
#include "log.h"
#include "mcom.h"
#include <nvml.h>
#include <string.h>
#include <string>

#include "attribute.h"
#include "resource.h"
#include "complete_req.hpp"

char log_buffer[LOG_BUF_SIZE];
nvmlReturn_t global_nvmlDeviceGetFanSpeed_rc = NVML_SUCCESS;
nvmlDevice_t global_device;
unsigned int global_device_minor_number = 0;
std::string global_string = "foo";

int MXMLFromString(

  mxml_t **EP,        /* O (populate or create) */
  char    *XMLString, /* I */
  char   **Tail,      /* O (optional) */
  char    *EMsg,      /* O (optional) */
  int      emsg_size) /* I */

  {
  return(0);
  }

int MXMLDestroyE(mxml_t **EP) {return(0); }

void log_ext(int errnum, const char *routine, const char *text, int severity) { }

void log_event(int eventtype, int objclass, const char *objname, const char *text) {}

void send_update_soon() { return; }

void get_device_indices(const char *device_str, std::vector<int> &device_indices, const char *suffix) {}

nvmlReturn_t nvmlDeviceGetFanSpeed(nvmlDevice_t device_hndl, unsigned int *i)
  {
  return(global_nvmlDeviceGetFanSpeed_rc);
  }

nvmlReturn_t nvmlDeviceGetHandleByIndex(unsigned int index, nvmlDevice_t *device_hndl)
  {
  *device_hndl = global_device;

  return(NVML_SUCCESS);
  }

nvmlReturn_t nvmlDeviceGetMinorNumber(nvmlDevice_t device_hndl, unsigned int *minor)
  {
  *minor = global_device_minor_number++;

  return(NVML_SUCCESS);
  }

void log_err(
       int         errnum,  /* I (errno or PBSErrno) */
       const char *routine, /* I */
       const char *text)    /* I */

  {
  snprintf(log_buffer, sizeof(log_buffer), "%s %s", routine, text);
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

int find_range_in_cpuset_string(

  std::string &source,
  std::string &output)

  {
  output = global_string;
  return(0);
  }

void translate_range_string_to_vector(

  const char       *range_string,
  std::vector<int> &indices)

  {
  char *str = strdup(range_string);
  char *ptr = str;
  int   prev;
  int   curr;

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

      if ((*ptr == ',') ||
          (is_whitespace(*ptr)))
        ptr++;
      }
    else
      {
      indices.push_back(prev);

      if ((*ptr == ',') ||
          (is_whitespace(*ptr)))
        ptr++;
      }
    }

  free(str);
  } /* END translate_range_string_to_vector() */

bool have_incompatible_dash_l_resource(

    pbs_attribute *pattr)

  {
  return(false);
  }

unsigned int complete_req::get_num_reqs()
  {
  return(1);
  }

req &complete_req::get_req(int i)
  {
  static req r;

  return(r);
  }

int req::get_gpus() const

  {
  return(0);
  }

req::req() {}
req::req(const req &other) {}
req &req::operator =(const req &other)
  {
  return(*this);
  }
std::string req::get_gpu_mode() const

  {
  return(this->gpu_mode);
  }

