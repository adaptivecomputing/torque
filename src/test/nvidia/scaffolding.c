#include "license_pbs.h" /* See here for the software license */
#include <stdlib.h>
#include <stdio.h> /* fprintf */
#include <sys/ioctl.h> /* winsize */
#include <termios.h> /* termios */
#include <vector>
#include "log.h"
#include "mcom.h"
#include <nvml.h>

char log_buffer[LOG_BUF_SIZE];
nvmlReturn_t global_nvmlDeviceGetFanSpeed_rc = NVML_SUCCESS;

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

void get_device_indices(const char *device_str, std::vector<unsigned int> &device_indices, const char *suffix) {}

nvmlReturn_t nvmlDeviceGetFanSpeed(nvmlDevice_t device_hndl, unsigned int *i)
  {
  return(global_nvmlDeviceGetFanSpeed_rc);
  }

void log_err(
       int         errnum,  /* I (errno or PBSErrno) */
       const char *routine, /* I */
       const char *text)    /* I */

  {
  snprintf(log_buffer, sizeof(log_buffer), "%s %s", routine, text);
  }
