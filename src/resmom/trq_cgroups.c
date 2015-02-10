#include "license_pbs.h" /* See this file for software license */
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <iostream>
#include <string>
#include <sstream>
#include <set>
#include <boost/foreach.hpp>
#include <boost/tokenizer.hpp>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "log.h"
#include "pbs_job.h"

using namespace std;
using namespace boost;

#define PS_CMD "ps -eo pid,ppid,sess | grep "

enum cgroup_system
  {
  cg_cpu,
  cg_cpuset,
  cg_cpuacct,
  cg_memory,
  cg_devices,
  cg_subsys_count
  };

string cg_cpu_path;
string cg_cpuset_path;
string cg_cpuacct_path;
string cg_memory_path;
string cg_devices_path;

/* This array tracks if all of the hierarchies are mounted we need 
   to run our control groups */
bool subsys_online[cg_subsys_count];

/* initialize subsys_online to all false so we
   can know if any needed hierarcies are not mounted */
void trq_cg_init_subsys_online()
  {
  for (int i = cg_cpu; i < cg_subsys_count; i++)
    subsys_online[i] = false;
  subsys_online[cg_subsys_count] = true;

  return;
  }

/* We need to remove the torque cgroups when pbs_mom 
 * is unloaded. */
int cleanup_torque_cgroups()
  {
  struct stat buf;
  string torque_path;
  int rc = PBSE_NONE;

  torque_path = cg_cpu_path;
  rc = stat(torque_path.c_str(), &buf);
  if (rc == 0)
    {
    /* directory exists. Remove it */
    rc = rmdir(torque_path.c_str());
    if (rc != 0)
      {
      fprintf(stderr, "could not remove %s from cgroups\n", torque_path.c_str());
      rc = PBSE_SYSTEM;
      }
    }

  torque_path = cg_cpuacct_path;
  rc = stat(torque_path.c_str(), &buf);
  if (rc == 0)
    {
    /* directory exists. Remove it */
    rc = rmdir(torque_path.c_str());
    if (rc != 0)
      {
      fprintf(stderr, "could not remove %s from cgroups\n", torque_path.c_str());
      rc = PBSE_SYSTEM;
      }
    }

  torque_path = cg_cpuset_path;
  rc = stat(torque_path.c_str(), &buf);
  if (rc == 0)
    {
    /* directory exists. Remove it */
    rc = rmdir(torque_path.c_str());
    if (rc != 0)
      {
      fprintf(stderr, "could not remove %s from cgroups\n", torque_path.c_str());
      rc = PBSE_SYSTEM;
      }
    }

  torque_path = cg_memory_path;
  rc = stat(torque_path.c_str(), &buf);
  if (rc == 0)
    {
    /* directory exists. Remove it */
    rc = rmdir(torque_path.c_str());
    if (rc != 0)
      {
      fprintf(stderr, "could not remove %s from cgroups\n", torque_path.c_str());
      rc = PBSE_SYSTEM;
      }
    }

  torque_path = cg_devices_path;
  rc = stat(torque_path.c_str(), &buf);
  if (rc == 0)
    {
    /* directory exists. Remove it */
    rc = rmdir(torque_path.c_str());
    if (rc != 0)
      {
      fprintf(stderr, "could not remove %s from cgroups\n", torque_path.c_str());
      rc = PBSE_SYSTEM;
      }
    }

  return(rc);
  }


/* We need to add a torque directory to each subsystem mount point
 * so we have a place to put the jobs */
int init_torque_cgroups()
  {
  struct stat buf;
  string torque_path;
  int rc;

  torque_path = cg_cpu_path;
  rc = stat(torque_path.c_str(), &buf);
  if (rc != 0)
    {
    /* create the torque directory under cg_cpu_path */
    rc = mkdir( torque_path.c_str(), S_IRWXU | S_IRWXG | S_IROTH);
    if (rc != 0)
      return(rc);
    } 

  torque_path = cg_cpuacct_path;
  rc = stat(torque_path.c_str(), &buf);
  if (rc != 0)
    {
    /* create the torque directory under cg_cpu_path */
    rc = mkdir( torque_path.c_str(), S_IRWXU | S_IRWXG | S_IROTH);
    if (rc != 0)
      return(rc);
    } 

  torque_path = cg_cpuset_path;
  rc = stat(torque_path.c_str(), &buf);
  if (rc != 0)
    {
    /* create the torque directory under cg_cpu_path */
    rc = mkdir( torque_path.c_str(), S_IRWXU | S_IRWXG | S_IROTH);
    if (rc != 0)
      return(rc);
    } 

  torque_path = cg_memory_path;
  rc = stat(torque_path.c_str(), &buf);
  if (rc != 0)
    {
    /* create the torque directory under cg_cpu_path */
    rc = mkdir( torque_path.c_str(), S_IRWXU | S_IRWXG | S_IROTH);
    if (rc != 0)
      return(rc);
    } 

  torque_path = cg_devices_path;
  rc = stat(torque_path.c_str(), &buf);
  if (rc != 0)
    {
    /* create the torque directory under cg_cpu_path */
    rc = mkdir( torque_path.c_str(), S_IRWXU | S_IRWXG | S_IROTH);
    if (rc != 0)
      return(rc);
    } 

  return(PBSE_NONE);

  }




int check_mounted_subsystems()
  {

  for (int i = cg_cpu; i < cg_subsys_count; i++)
    {
    if (subsys_online[i] != true)
      {
      switch (i)
        {
        case cg_cpu:
          fprintf(stderr, "cgroup cpu subsystem not mounted. \n");
          break;
        case cg_cpuacct:
          fprintf(stderr, "cgroup cpuacct subsystem not mounted. \n");
          break;
        case cg_cpuset:
          fprintf(stderr, "cgroup cpuset subsystem not mounted. \n");
          break;
        case cg_memory:
          fprintf(stderr, "cgroup memory subsystem not mounted. \n");
          break;
        case cg_devices:
          fprintf(stderr, "cgroup devices subsystem not mounted. \n");
          break;
        }
      return(PBSE_CGROUPS_NOT_ENABLED);
      }
    }

  return(PBSE_NONE);
  }

int init_subsystems(string& sub_token, string& mount_point)
  {

  if (sub_token.compare("cpu") == 0)
    {
    cg_cpu_path = mount_point + "/torque/";
    subsys_online[cg_cpu] = true;
    }
  else if (sub_token.compare("cpuacct") == 0)
    {
    cg_cpuacct_path = mount_point + "/torque/";
    subsys_online[cg_cpuacct] = true;
    }
  else if (sub_token.compare("cpuset") == 0)
    {
    cg_cpuset_path = mount_point + "/torque/";
    subsys_online[cg_cpuset] = true;
    }
  else if (sub_token.compare("memory") == 0)
    {
    cg_memory_path = mount_point + "/torque/";
    subsys_online[cg_memory] = true;
    }
  else if (sub_token.compare("devices") == 0)
    {
    cg_devices_path = mount_point + "/torque/";
    subsys_online[cg_devices] = true;
    }
  return(0);
  }
  
int trq_cg_initialize_hierarchy()
  {
  char cmd[512];
  char reply[1024];
  FILE *fp;
  string std_reply;
  string subsystem;
  int rc = PBSE_NONE;

  trq_cg_init_subsys_online();

  /* lssubsys -am will return all of the mounted subsystems.
     If any are not present we need to know and we will mount 
     them */
  strcpy(cmd, "lssubsys -am");

  fp = popen(cmd, "r");
  if (fp == NULL)
    {
    printf("Something is wrong. popen failed: %d", errno);
    return(-1);
    }

  while(fgets(reply, 1024, fp) != NULL)
    {
    vector<string> tokenized;
    vector<string> subsystem_tokens;
    char_separator<char> sep("\n ");
    char_separator<char> subsystem_sep(",");
    string  subsystem;
    string  mount_point;


    /* convert the char array to a string for ease of 
       parsing the string for tokens */

    std_reply = reply;
    tokenizer< char_separator<char> > tokens(std_reply, sep);
    BOOST_FOREACH (const string& t, tokens)
      {
      /* We expect a mounted subsystem to have a name and 
         a mount point in the output returned by lssubsys */
      tokenized.push_back(t);
      }

    /* Check to see if we have a mount point. If there are not two
     * entries in our vector there is no mount point */
    if (tokenized.size() < 2)
      continue;

    vector<string>::iterator tok_ptr = tokenized.begin();

    /* the first entry will be the subsystem */
    subsystem = *tok_ptr;
    tok_ptr++;
    /* the second entry will be the mount point for the subsystem */
    mount_point = *tok_ptr;

    /* Multiple subsystems may be mounted at the same point. 
     * look for a , separated list of subsystems */
    tokenizer< char_separator<char> > sub_tokens(subsystem, subsystem_sep);
    BOOST_FOREACH (const string& t, sub_tokens)
      {
      subsystem_tokens.push_back(t);
      }


    /* We are looking for the subsystems cpu, cpuacct, cpuset, memory and device.
       If the mount point is not given by lssubsys then we know that subsystem
       has not been mounted. subsys_online for that subsystem will not get
       set. We will take care of it later. */
    for (vector<string>::iterator sub_tok_ptr = subsystem_tokens.begin(); sub_tok_ptr != subsystem_tokens.end(); sub_tok_ptr++)
      {
      init_subsystems(*sub_tok_ptr, mount_point);
      }
    }

  pclose(fp);

  /* check to see if any of our devices are not mounted */
  rc = check_mounted_subsystems();
  if (rc != PBSE_NONE)
    return(rc);

  /* create torque directory under each mounted subsystem */
  rc = init_torque_cgroups();
  if (rc != PBSE_NONE)
    return(rc);

  return(PBSE_NONE);
  }

int trq_cg_add_process_to_cgroup(string& cgroup_path, pid_t job_pid, pid_t new_pid)
  {
  int rc;
  char log_buf[LOCAL_LOG_BUF_SIZE];
  int cgroup_fd;
  char cgroup_name[256];
  char new_task_pid[256];
  string full_cgroup_path;
  string cgroup_task_path;

  sprintf(new_task_pid, "%d", new_pid);
  sprintf(cgroup_name, "%d", job_pid);
  full_cgroup_path = cgroup_path + cgroup_name + "/tasks";

  cgroup_fd = open(full_cgroup_path.c_str(), O_WRONLY);
  if (cgroup_fd < 0)
    {
    sprintf(log_buf, "failed to open %s for writing: %s", full_cgroup_path.c_str(), strerror(errno));
    log_err(-1, __func__, log_buf);
    return(PBSE_SYSTEM); 
    }

  rc = write(cgroup_fd, new_task_pid, strlen(new_task_pid));
  if (rc <= 0)
    {
    sprintf(log_buf, "failed to add porcess %s to cgroup %s", cgroup_name, full_cgroup_path.c_str());
    log_err(-1, __func__, log_buf);
    return(PBSE_SYSTEM); 
    }

  close(cgroup_fd);

  return(PBSE_NONE);
  }

int trq_cg_create_cgroup(string& cgroup_path, pid_t job_pid)
  {
  int rc;
  char log_buf[LOCAL_LOG_BUF_SIZE];
  int cgroup_fd;
  char cgroup_name[256];
  string full_cgroup_path;
  string cgroup_task_path;

  sprintf(cgroup_name, "%d", job_pid);
  full_cgroup_path = cgroup_path  + cgroup_name;

  /* create a cgroup with the pid as the directory name under the cpuacct subsystem */
  rc = mkdir(full_cgroup_path.c_str(), 0x644);
  if (rc != 0)
    {
    sprintf(log_buf, "failed to make directory %s for cgroup: %s", full_cgroup_path.c_str(), strerror(errno));
    log_err(-1, __func__, log_buf);
    return(PBSE_SYSTEM); 
    }

  rc = chmod(full_cgroup_path.c_str(), 0x666);
  if (rc != 0)
    {
    sprintf(log_buf, "failed to change mode for  %s for cgroup: %s", full_cgroup_path.c_str(), strerror(errno));
    log_err(-1, __func__, log_buf);
    return(PBSE_SYSTEM); 
    }


  cgroup_task_path = full_cgroup_path + "/tasks";
  cgroup_fd = open(cgroup_task_path.c_str(), O_WRONLY);
  if (cgroup_fd < 0)
    {
    sprintf(log_buf, "failed to open %s for writing: %s", cgroup_task_path.c_str(), strerror(errno));
    log_err(-1, __func__, log_buf);
    return(PBSE_SYSTEM); 
    }

  rc = write(cgroup_fd, cgroup_name, strlen(cgroup_name));
  if (rc <= 0)
    {
    sprintf(log_buf, "failed to add porcess %s to cgroup", cgroup_name);
    log_err(-1, __func__, log_buf);
    return(PBSE_SYSTEM); 
    }

  close(cgroup_fd);

  return(PBSE_NONE);

  }

/* Add a process to the cpuacct and memory subsystems 
 */
int trq_cg_add_process_to_cgroup_accts(pid_t job_pid)
  {
  int rc;

  /* create a directory for this process in the cpuacct/torque hierarchy */
  rc = trq_cg_create_cgroup(cg_cpuacct_path, job_pid);
  if (rc != PBSE_NONE)
    return(rc);


  /* create a directory for this process in the memory/torque hierarchy */
  rc = trq_cg_create_cgroup(cg_memory_path, job_pid);
  if (rc != PBSE_NONE)
    return(rc);

  return(PBSE_NONE);
  }


/* trq_cg_remove_process_from_cgroup
 * Remove the cgroup of the job_pid from the
 * cgroup_path given.
 */
int trq_cg_remove_process_from_cgroup(string cgroup_path, pid_t job_pid)
  {
  int rc;
  char log_buf[LOCAL_LOG_BUF_SIZE];
  char cgroup_name[256];
  string cgroup_path_name;

  sprintf(cgroup_name, "%d", job_pid);
  cgroup_path_name = cgroup_path + cgroup_name;

  /* TODO: We are waiting for cgroups to be done.
     How long should we wait? */
  do
    {
    rc = rmdir(cgroup_path_name.c_str());

    if (rc < 0 )
      {
      if (errno == EBUSY)
        {
        /* cgroups are still cleaning up. try again later. */
        if (LOGLEVEL >= 8)
          {
          sprintf(log_buf, "cgroup not finished %s from cgroups: %d ", cgroup_path_name.c_str(), errno);
          log_event(PBSEVENT_JOB, PBS_EVENTCLASS_JOB, __func__, log_buf);
          }
        sleep(1);
        }
      else
        {
        sprintf(log_buf, "failed to remove %s from cgroups: %d ", cgroup_path_name.c_str(), errno);
        log_err(-1, __func__, log_buf);
        break;
        }
      }
    else
      rc = PBSE_NONE;

    }while(rc != PBSE_NONE);

  return(rc);


  }

/* trq_cg_remove_process_from_accts: Remove the hierarchies for
   this jobs process from the cgroups directory. That is remove
   this job from its accounting cgroups */
void *trq_cg_remove_process_from_accts(void *vp)
  {
  int rc;
  char log_buf[LOCAL_LOG_BUF_SIZE];
  pid_t *pid = (pid_t *)vp;
  pid_t job_pid = *pid;


  /* remove job from the cpuacct cgroup */
  rc = trq_cg_remove_process_from_cgroup(cg_cpuacct_path, job_pid);
  if (rc != PBSE_NONE)
    {
    sprintf(log_buf, "Failed to remove cgroup cpuacct: process %d", job_pid);
    log_err(-1, __func__, log_buf);
    }

  /* remove job from the cpumemory cgroup */
  rc = trq_cg_remove_process_from_cgroup(cg_memory_path, job_pid);
  if (rc != PBSE_NONE)
    {
    sprintf(log_buf, "Failed to remove cgroup memory: process %d", job_pid);
    log_err(-1, __func__, log_buf);
    }


  return(PBSE_NONE);
  }

/*
 * trq_cg_set_swap_memory_limit()
 *
 * Sets the memory.limit_in_bytes to memory_limit for the cgroup of this process.
 *
 * @param pid  -  The process id of the cgroup
 * @param memory_limit - The memory limit for this cgroup 
 *
 * @return PBSE_NONE on success
 */

int trq_cg_set_swap_memory_limit(
  
  pid_t pid, 
  unsigned long memory_limit)

  {
  char   log_buf[LOCAL_LOG_BUF_SIZE];
  char   cgroup_name[256];
  char   mem_limit_string[64];
  string oom_control_name;
  FILE   *fd;
  size_t  bytes_written;
  
  /* Create a string with a path to the 
     memory.limit_in_bytes cgroup for the job */
  sprintf(cgroup_name, "%d", pid);
  oom_control_name = cg_memory_path + cgroup_name + "/memory.memsw.limit_in_bytes";

  /* open the memory.limit_in_bytes file and set it to memory_limit */
  fd = fopen(oom_control_name.c_str(), "r+");
  if (fd == NULL)
    {
    sprintf(log_buf, "failed to open cgroup path %s", oom_control_name.c_str());
    log_err(-1, __func__, log_buf);
    return(PBSE_SYSTEM);
    }

  sprintf(mem_limit_string, "%ld", memory_limit);
  bytes_written = fwrite(mem_limit_string, strlen(mem_limit_string) -1, strlen(mem_limit_string) -1, fd);
  if (bytes_written < 1)
    {
    sprintf(log_buf, "failed to write cgroup memory limit to  %s", oom_control_name.c_str());
    log_err(-1, __func__, log_buf);
    fclose(fd);
    return(PBSE_SYSTEM);
    }

  fclose(fd);

  return(PBSE_NONE);

  }

/*
 * trq_cg_set_resident_memory_limit()
 *
 * Sets the memory.limit_in_bytes to memory_limit for the cgroup of this process.
 *
 * @param pid  -  The process id of the cgroup
 * @param memory_limit - The memory limit for this cgroup 
 *
 * @return PBSE_NONE on success
 */

int trq_cg_set_resident_memory_limit(
  
  pid_t pid, 
  unsigned long memory_limit)

  {
  char   log_buf[LOCAL_LOG_BUF_SIZE];
  char   cgroup_name[256];
  char   mem_limit_string[64];
  string oom_control_name;
  FILE   *fd;
  size_t  bytes_written;
  
  /* Create a string with a path to the 
     memory.limit_in_bytes cgroup for the job */
  sprintf(cgroup_name, "%d", pid);
  oom_control_name = cg_memory_path + cgroup_name + "/memory.limit_in_bytes";

  /* open the memory.limit_in_bytes file and set it to memory_limit */
  fd = fopen(oom_control_name.c_str(), "r+");
  if (fd == NULL)
    {
    sprintf(log_buf, "failed to open cgroup path %s", oom_control_name.c_str());
    log_err(-1, __func__, log_buf);
    return(PBSE_SYSTEM);
    }

  sprintf(mem_limit_string, "%ld", memory_limit);
  bytes_written = fwrite(mem_limit_string, strlen(mem_limit_string) -1, strlen(mem_limit_string) -1, fd);
  if (bytes_written < 1)
    {
    sprintf(log_buf, "failed to write cgroup memory limit to  %s", oom_control_name.c_str());
    log_err(-1, __func__, log_buf);
    fclose(fd);
    return(PBSE_SYSTEM);
    }

  fclose(fd);

  return(PBSE_NONE);

  }

