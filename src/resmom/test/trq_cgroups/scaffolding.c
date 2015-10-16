#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <sys/stat.h>
#include <string>
#include "pbs_error.h"
#include "machine.hpp"

int  LOGLEVEL=6;

Machine this_node;

char mom_alias[PBS_MAXHOSTNAME + 1];


/* create a cgroup hierarchy where we know it will exist so we can control
   where the files will go */
int create_cgroup_hierarchy()
  {
  int rc = PBSE_NONE;
  struct stat buf;

  rc = stat("/tmp/cgroup", &buf);
  if (rc != 0)
    {
    rc = mkdir("/tmp/cgroup",  S_IRWXU | S_IRWXG | S_IROTH);
    if (rc != 0)
      {
      fprintf(stderr, "failed to make directory /tmp/cgroup: %d\n", errno);
      return(rc);
      }
    }

  rc = stat("/tmp/cgroup/cpuset", &buf);
  if (rc != 0)
    {
    rc = mkdir("/tmp/cgroup/cpuset",  S_IRWXU | S_IRWXG | S_IROTH);
    if (rc != 0)
      {
      fprintf(stderr, "failed to make directory /tmp/cgroup/cpuset: %d\n", errno);
      return(rc);
      }
    }

  rc = stat("/tmp/cgroup/cpu", &buf);
  if (rc != 0)
    {
    rc = mkdir("/tmp/cgroup/cpu",  S_IRWXU | S_IRWXG | S_IROTH);
    if (rc != 0)
      {
      fprintf(stderr, "failed to make directory /tmp/cgroup/cpu: %d\n", errno);
      return(rc);
      }
    }

  rc = stat("/tmp/cgroup/cpuacct", &buf);
  if (rc != 0)
    {
    rc = mkdir("/tmp/cgroup/cpuacct",  S_IRWXU | S_IRWXG | S_IROTH);
    if (rc != 0)
      {
      fprintf(stderr, "failed to make directory /tmp/cgroup/cpuacct: %d\n", errno);
      return(rc);
      }
    }

  rc = stat("/tmp/cgroup/memory", &buf);
  if (rc != 0)
    {
    rc = mkdir("/tmp/cgroup/memory",  S_IRWXU | S_IRWXG | S_IROTH);
    if (rc != 0)
      {
      fprintf(stderr, "failed to make directory /tmp/cgroup/memory: %d\n", errno);
      return(rc);
      }
    }

  rc = stat("/tmp/cgroup/devices", &buf);
  if (rc != 0)
    {
    rc = mkdir("/tmp/cgroup/devices",  S_IRWXU | S_IRWXG | S_IROTH);
    if (rc != 0)
      {
      fprintf(stderr, "failed to make directory /tmp/cgroup/devices: %d\n", errno);
      return(rc);
      }
    }

  /* mount the cgroup for each of these */
  rc = system("mount -t cgroup -o cpu test_cpu /tmp/cgroup/cpu");
  if (rc != -1)
    rc = 0;

  rc = system("mount -t cgroup -o cpuset test_cpuset /tmp/cgroup/cpuset");
  if (rc != -1)
    rc = 0;

  rc = system("mount -t cgroup -o cpuacct test_cpuacct /tmp/cgroup/cpuacct");
  if (rc != -1)
    rc = 0;

  rc = system("mount -t cgroup -o memory test_memory /tmp/cgroup/memory");
  if (rc != -1)
    rc = 0;

  rc = system("mount -t cgroup -o devices test_devices /tmp/cgroup/devices");
  if (rc != -1)
    rc = 0;

  return (rc);
  }
      
/* cleanup_cgroup_hierarchy()
   Cleanup our test cgroup hierarchy. We don't want to leave a mess behind */
int cleanup_cgroup_hierarchy()
  {
  int rc = PBSE_NONE;

  /* unmount the cgroups first. We do not care about the return codes */
   rc = system("umount test_cpu");
  if (rc != -1)
    rc = 0;

  rc = system("umount test_cpuset");
  if (rc != -1)
    rc = 0;

  rc = system("umount test_cpuacct");
  if (rc != -1)
    rc = 0;

  rc = system("umount test_memory");
  if (rc != -1)
    rc = 0;

  rc = system("umount test_devices");
  if (rc != -1)
    rc = 0;

  rc = system("rm -rf /tmp/cgroup");

  return (0);
  }

void log_event(

  int         eventtype,
  int         objclass,
  const char *objname,
  const char *text)
  {
  return;
  }

void log_err(

  int         errnum,  /* I (errno or PBSErrno) */
  const char *routine, /* I */
  const char *text)    /* I */
  {
  return;
  }




/* pbs_err_db generators
 *
 * @see pbs_error_db.h
 *
 * @note this constant should be const char*, but the current state of code
 *       doesn't allow this
 */
/* this construct generates array of text representations for error constants */
#define PbsErrClient(id, txt) txt,
static char * pbs_err_client_txt[] =
{
#include "pbs_error_db.h"
(char*)0
};
#undef PbsErrClient

#define PbsErrRm(id, txt) txt,
static char * pbs_err_rm_txt[] =
{
#include "pbs_error_db.h"
(char*)0
};
#undef PbsErrRm


/*
 * pbse_to_txt() - return a text message for an PBS error number
 * if it exists
 */

char *pbse_to_txt(

  int err)  /* I */

  {
  if (err == 0)
	  return pbs_err_client_txt[0];

  if (err > PBSE_FLOOR && err < PBSE_CEILING)
	  return pbs_err_client_txt[err - PBSE_FLOOR];

  if (err > PBSE_RMFLOOR && err < PBSE_RMCEILING)
	  return pbs_err_rm_txt[err - PBSE_RMFLOOR];

  return (char*)0;
  }  /* END pbse_to_txt() */



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


/*
 * a threadsafe tokenizing function - this also alters the input string
 * just like strtok does.
 *
 * @param str - a modified string pointer (advanced past the token)
 * @param delims - the delimiters to look for
 * @return - a pointer to the token
 */

char *threadsafe_tokenizer(

  char       **str,    /* M */
  const char  *delims) /* I */

  {
  char *current_char;
  char *start;

  if ((str == NULL) ||
      (*str == NULL))
    return(NULL);

  /* save start position */
  start = *str;

  /* return NULL at the end of the string */
  if (*start == '\0')
    return(NULL);

  /* begin at the start */
  current_char = start;

  /* advance to the end of the string or until you find a delimiter */
  while ((*current_char != '\0') &&
         (!strchr(delims, *current_char)))
    current_char++;

  /* advance str */
  if (*current_char != '\0')
    {
    /* not at the end of the string */
    *str = current_char + 1;
    *current_char = '\0';
    }
  else
    {
    /* at the end of the string */
    *str = current_char;
    }

  return(start);
  } /* END threadsafe_tokenizer() */



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


Chip::Chip() {}
Chip::~Chip() {}

Core::~Core() {}

PCI_Device::~PCI_Device() {}

Socket::~Socket () {}

Machine::~Machine() {}

Machine::Machine() {}

