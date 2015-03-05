#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <sys/stat.h>
#include "pbs_error.h"

int  LOGLEVEL=6;

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
