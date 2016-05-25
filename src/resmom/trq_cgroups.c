#include "license_pbs.h" /* See this file for software license */
#include <pbs_config.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <set>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <dirent.h>
#include "log.h"
#include "trq_cgroups.h"
#include "utils.h"
#ifdef NVIDIA_GPUS
#include "nvml.h"
#endif
#include "mom_server_lib.h"
#ifdef PENABLE_LINUX_CGROUPS
#include <boost/tokenizer.hpp>
#include <boost/foreach.hpp>
#include "machine.hpp"
#include "complete_req.hpp"
#include "resource.h" /* struct resource */
#endif

using namespace std;
using namespace boost;

#define MAX_JOBID_LENGTH    1024
extern char mom_alias[];
extern unsigned int global_gpu_count;
extern uint32_t     global_mic_count;

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
string cg_prefix("cpuset.");

const int CPUS = 0;
const int MEMS = 1;
const int MAX_WRITE_RETRIES = 5;

#ifdef PENABLE_LINUX_CGROUPS
//----std::vector<std::string> incompatible_dash_l_resources;
extern Machine this_node;

/* This array tracks if all of the hierarchies are mounted we need 
   to run our control groups */
bool subsys_online[cg_subsys_count];

/* initialize subsys_online to all false so we
   can know if any needed hierarcies are not mounted */
void trq_cg_init_subsys_online(bool val)
  {
  for (int i = cg_cpu; i < cg_subsys_count; i++)
    subsys_online[i] = val;
  subsys_online[cg_subsys_count] = true;

  return;
  }

//----void initialize_incompatible_dash_l_resources(std::vector<std::string> &incompatible_resource_list)
//----  {
//----  /* These are the -l resources that are not compatible with the -L resource request */
//----  incompatible_resource_list.clear();
//----  incompatible_resource_list.push_back("mem");
//----  incompatible_resource_list.push_back("nodes");
//----  incompatible_resource_list.push_back("hostlis");
//----  incompatible_resource_list.push_back("ncpus");
//----  incompatible_resource_list.push_back("procs");
//----  incompatible_resource_list.push_back("pvmem");
//----  incompatible_resource_list.push_back("pmem");
//----  incompatible_resource_list.push_back("vmem");
//----  incompatible_resource_list.push_back("reqattr");
//----  incompatible_resource_list.push_back("software");
//----  incompatible_resource_list.push_back("geometry");
//----  incompatible_resource_list.push_back("opsys");
//----  incompatible_resource_list.push_back("tpn");
//----  incompatible_resource_list.push_back("trl");
//----  }



/*
 * write_to_file()
 *
 * Writes content to a file
 * @param path - the path to the file
 * @param content - what we're writing
 * @param err_msg - the error message, if any
 * @return PBSE_NONE on success or a PBSE_ error
 */

int write_to_file(

  const char        *path,
  const std::string &content,
  std::string       &err_msg)

  {
  if (path == NULL)
    return(PBSE_BAD_PARAMETER);

  int rc = PBSE_SYSTEM;
  int retries = 0;

  while (retries < MAX_WRITE_RETRIES)
    {
    FILE *fd = fopen(path, "w+");
    retries++;

    if (fd == NULL)
      {
      err_msg = "Could not open ";
      err_msg += path;
      }
    else
      {
      int  bytes_written;
      bool retry_write = true;
     
      while (((bytes_written = fwrite(content.c_str(), sizeof(char), content.size(), fd)) < 1) &&
             (retry_write == true))
        {
        switch (errno)
          {
          case EEXIST:
          case EINTR:

            retries++;

            break;

          default:

            // Retry all errors
            retry_write = false;
            break;
          }
        }
        
      fclose(fd);

      if (bytes_written >= 1)
        {
        rc = PBSE_NONE;
        break;
        }

      usleep(100);
      }
    }
      
  if (rc != PBSE_NONE)
    {
    if (err_msg.size() == 0)
      {
      err_msg = "Could not write '";
      err_msg += content + "' to ";
      err_msg += path;
      }
    }

  return(rc);
  } // END write_to_file()


#define MAX_RMDIR_RETRIES 30

/* We need to remove the torque cgroups when pbs_mom 
 * is unloaded. */
int trq_cg_cleanup_torque_cgroups()
  {
  struct stat buf;
  string torque_path;
  int rc = PBSE_NONE;

  torque_path = cg_cpu_path;
  rc = stat(torque_path.c_str(), &buf);
  if (rc == 0)
    {

    /* directory exists. Remove it */
    rc = rmdir_ext(torque_path.c_str(), MAX_RMDIR_RETRIES);
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
    rc = rmdir_ext(torque_path.c_str(), MAX_RMDIR_RETRIES);
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
    rc = rmdir_ext(torque_path.c_str(), MAX_RMDIR_RETRIES);
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
    rc = rmdir_ext(torque_path.c_str(), MAX_RMDIR_RETRIES);
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
    rc = rmdir_ext(torque_path.c_str(), MAX_RMDIR_RETRIES);
    if (rc != 0)
      {
      fprintf(stderr, "could not remove %s from cgroups\n", torque_path.c_str());
      rc = PBSE_SYSTEM;
      }
    }

  return(rc);
  } // END trq_cg_cleanup_torque_cgroups()



/*
 * trq_cg_initialize_cpuset_string()
 *
 * Add job_pid to the tasks file of the cgroup
 *
 * The purpose of this function is to get the value stored in the 
 * parent hierarcy for cpuset.cpus and cpuset.mems and then copy 
 * those values into the torque cgroup file cpuset.cpus and cpuset.mems.
 * This has to be done in order to enable cpusets for the lower cgroups.
 * @param filename - this is either cpuset.cpus or cpuset.mems.
 *
 * @return PBSE_NONE on success
 */


int trq_cg_initialize_cpuset_string(
    
  const string file_name)

  {
  char   log_buf[LOCAL_LOG_BUF_SIZE];
  FILE  *fd;
  char   buf[64];
  std::string cpus_path;
  std::string base_path;
  std::string key ("/torque");

  /* the newly created torque dir needs to have the cpuset.cpus and cpuset.mems
     files set to the same value as the parent cpuset directory */

  /* First take /torque off of the cg_cpuset_path string. This will
     give us the root directory for the cgroup hierarchy */
  base_path = cg_cpuset_path;
  std::size_t found = base_path.rfind(key);
  if (found == std::string::npos)
    {
    sprintf(log_buf, "Could not find sub-string \"/torque\" in %s", base_path.c_str());
    log_event(PBSEVENT_SYSTEM, PBS_EVENTCLASS_NODE, __func__, log_buf);
    return(PBSE_SYSTEM);
    }

  /* open the parent cgroup file as given by file_name. This file is 
     either cpuset.cpus or cpuset.mems */
  base_path.replace(found, key.length(), ""); 
  cpus_path = base_path + cg_prefix + file_name;
  fd = fopen(cpus_path.c_str(), "r");
  if (fd == NULL)
    {
    // Attempt to open the file without the 'cpuset.' prefix
    cpus_path = base_path + file_name;
    if ((fd = fopen(cpus_path.c_str(), "r")) == NULL)
      {
      sprintf(log_buf, "Could not open %s while initializing cpuset cgroups: error %d", cpus_path.c_str(), errno);
      log_event(PBSEVENT_SYSTEM, PBS_EVENTCLASS_NODE, __func__, log_buf);
      return(PBSE_SYSTEM);
      }
    else
      {
      // It worked, so make the prefix non-existent
      cg_prefix = "";
      }
    }

  /* read in the string from the file */
  memset(buf, 0, 64);
  fread(buf, sizeof(buf), 1, fd);
  if (strlen(buf) < 1)
    {
    sprintf(log_buf, "Could not read %s while initializing cpuset cgroups: error %d",
      cpus_path.c_str(), errno);
    log_event(PBSEVENT_SYSTEM, PBS_EVENTCLASS_NODE, __func__, log_buf);
    fclose(fd);
    return(PBSE_SYSTEM);
    }

  fclose(fd);

  /* Now write the value we got from the parent cgroup file to the torque cgroup file */
  cpus_path = cg_cpuset_path + cg_prefix + file_name;
  std::string err_msg;
  int rc = write_to_file(cpus_path.c_str(), buf, err_msg);

  if (rc != PBSE_NONE)
    log_err(errno, __func__, err_msg.c_str());

  return(rc);
  } // END trq_cg_initialize_cpuset_string()

/* make sure that the cgroup directories have the correct mode, in case 
 * they get created from a thread that has a non-zero umask set. */
int trq_cg_mkdir(const char* pathname, mode_t mode) {

    struct stat buf;
    int rc = mkdir(pathname, mode);
    if(rc != 0) {
        return rc;
    }
    rc = stat(pathname, &buf);
    if(rc != 0) {
        return rc;
    }
    if(buf.st_mode != mode) {
        rc = chmod(pathname, mode);
    }

} 

/* We need to add a torque directory to each subsystem mount point
 * so we have a place to put the jobs */
int init_torque_cgroups()
  {
  struct stat buf;
  string torque_path;
  string file_name;
  int    rc;
  char   log_buf[LOCAL_LOG_BUF_SIZE];

  torque_path = cg_cpu_path;
  rc = stat(torque_path.c_str(), &buf);
  if (rc != 0)
    {
    /* create the torque directory under cg_cpu_path */
    rc = trq_cg_mkdir( torque_path.c_str(), S_IRWXU | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH);
    if (rc != 0)
      return(rc);
    } 
    

  torque_path = cg_cpuacct_path;
  rc = stat(torque_path.c_str(), &buf);
  if (rc != 0)
    {
    /* create the torque directory under cg_cpu_path */
    rc = trq_cg_mkdir( torque_path.c_str(), S_IRWXU | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH);
    if (rc != 0)
      return(rc);
    }

  torque_path = cg_cpuset_path;
  rc = stat(torque_path.c_str(), &buf);
  if (rc != 0)
    {
     /* create the torque directory under cg_cpu_path */
    rc = trq_cg_mkdir( torque_path.c_str(), S_IRWXU | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH);
    if (rc != 0)
      {
      sprintf(log_buf, "Could not create %s for cgroups: error %d", torque_path.c_str(), errno);
      log_event(PBSEVENT_SYSTEM, PBS_EVENTCLASS_NODE, __func__, log_buf);
      return(rc);
      }

    file_name = "mems";
    rc = trq_cg_initialize_cpuset_string(file_name);
    if (rc != PBSE_NONE)
      return(rc);
 
    file_name = "cpus";
    rc = trq_cg_initialize_cpuset_string(file_name);
    if (rc != PBSE_NONE)
      return(rc);
    } 


  torque_path = cg_memory_path;
  rc = stat(torque_path.c_str(), &buf);
  if (rc != 0)
    {
    /* create the torque directory under cg_cpu_path */
    rc = trq_cg_mkdir( torque_path.c_str(), S_IRWXU | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH);
    if (rc != 0)
      return(rc);
    } 

  torque_path = cg_devices_path;
  rc = stat(torque_path.c_str(), &buf);
  if (rc != 0)
    {
    /* create the torque directory under cg_cpu_path */
    rc = trq_cg_mkdir( torque_path.c_str(), S_IRWXU | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH);
    if (rc != 0)
      return(rc);
    } 

  return(PBSE_NONE);

  } // END init_torque_cgroups()



/* 
 * check_mounted_subsystems()
 *
 * checks to see if cgroup subdirectories are mounted. 
 * fails if they are not.
 *
 */

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
  } // END check_mounted_subsystems()



int init_subsystems(
    
  string &sub_token,
  string &mount_point)

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
  } // END init_subsystems()




int trq_cg_get_cgroup_paths_from_system()

  {
  char cmd[512];
  char reply[1024];
  FILE *fp;
  string std_reply;
  string subsystem;

   /* lssubsys -am will return all of the mounted subsystems.
     If any are not present we need to know and we will mount 
     them */
  strcpy(cmd, "lssubsys -am");

  fp = popen(cmd, "r");
  if (fp == NULL)
    {
    printf("Something is wrong. popen failed: %d", errno);
    return(PBSE_SYSTEM);
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

  return(PBSE_NONE);
  } // END trq_cg_get_cgroup_paths_from_system()


 
int trq_cg_get_cgroup_paths_from_file()
  
  {
  std::string cgroup_path;
  cgroup_path = PBS_SERVER_HOME;
  cgroup_path = cgroup_path + "/trq-cgroup-paths";

  int rc;
  std::ifstream iFile;
  std::string   line;
  struct stat   stat_buf;
  typedef boost::tokenizer<boost::char_separator<char> > tokenizer;
  boost::char_separator<char> sep(" ");

  rc = stat(cgroup_path.c_str(), &stat_buf);
  if (rc != 0)
    return(PBSE_CAN_NOT_OPEN_FILE);
    
  iFile.open(cgroup_path.c_str(), std::ifstream::in);

  if (iFile.is_open())
    {
    while (getline(iFile, line))
      {
      tokenizer tokens(line, sep);
      for (tokenizer::iterator tok = tokens.begin(); tok != tokens.end(); ++ tok)
        {
        string subsys;
        string path;

        subsys = *tok;
        tok++;
        path = *tok;

        if (subsys.compare("cpu") == 0)
          {
          cg_cpu_path = path;
          init_subsystems(subsys, path);
          }
        else if (subsys.compare("cpuset") == 0)
          {
          cg_cpuset_path = path;
          init_subsystems(subsys, path);
          }
        else if (subsys.compare("cpuacct") == 0)
          {
          cg_cpuacct_path = path;
          init_subsystems(subsys, path);
          }
        else if (subsys.compare("memory") == 0)
          {
          cg_memory_path = path;
          init_subsystems(subsys, path);
          }
        else if (subsys.compare("devices") == 0)
          {
          cg_devices_path = path;
          init_subsystems(subsys, path);
          }

          break;
        }
      }

    iFile.close();
    }
  else
    return(PBSE_CAN_NOT_OPEN_FILE);

  return(PBSE_NONE);

  } // END trq_cg_get_cgroup_paths_from_file()



int trq_cg_initialize_hierarchy()
  {
 int rc = PBSE_NONE;

  trq_cg_init_subsys_online(false);

  /* Check to see if we have a user provided configuration file 
     for cgroups */
  rc = trq_cg_get_cgroup_paths_from_file();
  if (rc != PBSE_NONE)
    {
    /* No configuration file. Get the info from the system */
    rc = trq_cg_get_cgroup_paths_from_system();
    if (rc)
      return(rc);
    }

  /* check to see if any of our devices are not mounted */
  rc = check_mounted_subsystems();
  if (rc != PBSE_NONE)
    return(rc);

  /* create torque directory under each mounted subsystem */
  rc = init_torque_cgroups();
  if (rc != PBSE_NONE)
    return(rc);

  return(PBSE_NONE);
  } // END trq_cg_initialize_hierarchy()



int trq_cg_add_process_to_cgroup_path(
    
  string     &cgroup_path,
  pid_t       new_pid)

  {
  int  rc;
  char log_buf[LOCAL_LOG_BUF_SIZE];
  int  cgroup_fd;
  char new_task_pid[MAX_JOBID_LENGTH];

  sprintf(new_task_pid, "%d", new_pid);

  cgroup_fd = open(cgroup_path.c_str(), O_WRONLY);
  if (cgroup_fd < 0)
    {
    sprintf(log_buf, "failed to open %s for writing: %s",
      cgroup_path.c_str(), strerror(errno));
    log_err(errno, __func__, log_buf);
    return(PBSE_SYSTEM); 
    }

  rc = write(cgroup_fd, new_task_pid, strlen(new_task_pid));
  
  close(cgroup_fd);

  if (rc <= 0)
    {
    sprintf(log_buf, "failed to add process %s to cgroup %s: error: %d",
      new_task_pid, cgroup_path.c_str(), errno);
    log_err(errno, __func__, log_buf);
    return(PBSE_SYSTEM); 
    }

  return(PBSE_NONE);
  } // END trq_cg_add_process_to_cgroup()



/*
 * trq_cg_add_process_to_task_cgroup
 * 
 * Add the new process to its task cgroup
 *
 * @param cgroup_path  - path to cgroup directory
 * @param job_id       - id of job
 * @param new_pid      - process if of new task
 *
 */

int MAX_PID_LEN = 32;

int trq_cg_add_process_to_task_cgroup(

  string     &cgroup_path, 
  const char *job_id, 
  const unsigned int req_index,
  const unsigned int task_index,
  pid_t       new_pid)

  {
  char   req_task_number[MAX_JOBID_LENGTH];
  string req_task_path;

  sprintf(req_task_number, "/R%u.t%u/tasks", req_index, task_index);
  req_task_path = cgroup_path + job_id + req_task_number;

  return(trq_cg_add_process_to_cgroup_path(req_task_path, new_pid));
  } // END trq_cg_add_process_to_task_cgroup()



unsigned long long trq_cg_read_numeric_value(

  string &path,
  bool   &error)

  {
  int                fd;
  struct stat        stat_buf;
  unsigned long long val = 0;
  char               buf[LOCAL_LOG_BUF_SIZE];

  error = false;

  /* If we don't have a file or access to it return 0 */
  /* probably a -l request and we are looking for a Rx.ty directory */
  if (stat(path.c_str(), &stat_buf) == -1)
    return(0);

  fd = open(path.c_str(), O_RDONLY);
  if (fd <= 0)
    {
    sprintf(log_buffer, "failed to open %s: %s", path.c_str(), strerror(errno));
    log_err(errno, __func__, log_buffer);
    error = true;
    }
  else
    {
    int rc = read(fd, buf, LOCAL_LOG_BUF_SIZE);

    close(fd);

    if (rc == -1)
      {
      sprintf(buf, "read failed getting value from %s - %s", path.c_str(), strerror(errno));
      log_err(errno, __func__, buf);
      error = true;
      }
    else if (rc != 0)
      val = strtoull(buf, NULL, 10);
    }

  return(val);
  } // END trq_cg_read_numeric_value()



/* 
 * trq_cg_get_task_memory_stats
 *
 * Get get the resident memory and cpu time
 * for the task and record it in the allocation
 * object.
 *
 * @param cgroup_path - path to this jobs cgroup
 * @param job_id      - id of job
 * @param req_index   - req number
 * @param task_index  - task index of req
 * @param al          - allocation object of task
 *
 */

int trq_cg_get_task_memory_stats(

  const char         *job_id,
  const unsigned int  req_index,
  const unsigned int  task_index,
  unsigned long long &mem_used)

  {
  char               req_and_task[256];
  int                rc = PBSE_NONE;

  /* get memory first */
  sprintf(req_and_task, "/%s/R%u.t%u/memory.max_usage_in_bytes", job_id, req_index, task_index);

  string  cgroup_path = cg_memory_path + req_and_task;
  bool    error;

  mem_used = trq_cg_read_numeric_value(cgroup_path, error);
  
/*  if (mem_used == 0)
    {
    sprintf(log_buffer, "peak memory usage read from '%s' is 0, something appears to be incorrect.",
      cgroup_path.c_str());
    log_event(PBSEVENT_JOB, PBS_EVENTCLASS_JOB, __func__, log_buffer);
    }*/

  if (error)
    rc = PBSE_SYSTEM;

  return(rc);
  } // END trq_cg_get_task_memory_stats()



/* 
 * trq_cg_get_task_cput_stats
 *
 * Get get the resident memory and cpu time
 * for the task and record it in the allocation
 * object.
 *
 * @param cgroup_path - path to this jobs cgroup
 * @param job_id      - id of job
 * @param req_index   - req number
 * @param task_index  - task index of req
 * @param al          - allocation object of task
 *
 */

int trq_cg_get_task_cput_stats(

  const char         *job_id,
  const unsigned int  req_index,
  const unsigned int  task_index,
  unsigned long      &cput_used)

  {
  char   req_and_task[256];
  int    rc = PBSE_NONE;
  bool   error = PBSE_NONE;

  /* get cpu time used */
  sprintf(req_and_task, "/%s/R%u.t%u/cpuacct.usage", job_id, req_index, task_index);

  string cgroup_path = cg_cpuacct_path + req_and_task;

  cput_used = trq_cg_read_numeric_value(cgroup_path, error);

/*  if (cput_used == 0)
    {
    sprintf(log_buffer, "cpu time read from '%s' is 0, something appears to be incorrect.",
      cgroup_path.c_str());
    log_event(PBSEVENT_JOB, PBS_EVENTCLASS_JOB, __func__, log_buffer);
    }*/

  if (error)
    rc = PBSE_SYSTEM;
  
  return(rc);
  } // END trq_cg_get_task_cput_stats()



int trq_cg_add_process_to_cgroup(
    
  string     &cgroup_path,
  const char *job_id,
  pid_t       new_pid)

  {
  string full_cgroup_path = cgroup_path + job_id + "/tasks";

  return(trq_cg_add_process_to_cgroup_path(full_cgroup_path, new_pid));
  } // END trq_cg_add_process_to_cgroup()



int trq_cg_add_process_to_all_cgroups(

  const char *job_id,
  pid_t       job_pid)

  {
  int rc;
  if ((rc = trq_cg_add_process_to_cgroup(cg_cpuset_path, job_id, job_pid)) == PBSE_NONE)
    {
    if ((rc = trq_cg_add_process_to_cgroup(cg_cpuacct_path, job_id, job_pid)) == PBSE_NONE)
      {
      if((rc = trq_cg_add_process_to_cgroup(cg_memory_path, job_id, job_pid)) == PBSE_NONE)
        {
        rc = trq_cg_add_process_to_cgroup(cg_devices_path, job_id, job_pid);
        }
      }
    }

  return(rc);
  } // END trq_cg_add_process_to_all_cgroups()


/* 
 * have_incompatible_dash_l_resource
 *
 * Check to see if this is an incompatile -l resource
 * request for a -L syntax
 *
 * @param pjob  - the job structure we are working with
 *
 */
   
//----bool have_incompatible_dash_l_resource(
//----    
//----    job *pjob)
//----
//----  {
//----  resource *presl; /* for -l resource request */
//----
//----  presl = (resource *)GET_NEXT(pjob->ji_wattr[JOB_ATR_resource].at_val.at_list); 
//----  if (presl != NULL)
//----    {
//----    std::vector<std::string>::iterator it;
//----
//----    if (incompatible_dash_l_resources.size() == 0)
//----      initialize_incompatible_dash_l_resources(incompatible_dash_l_resources);
//----
//----    do
//----      {
//----      std::string pname = presl->rs_defin->rs_name;
//----      it = std::find(incompatible_dash_l_resources.begin(), incompatible_dash_l_resources.end(), pname);
//----      if (it != incompatible_dash_l_resources.end())
//----        {
        /* pname points to a string of an incompatible -l resource type */
//----        return(true);
//----        }
//----
//----      presl = (resource *)GET_NEXT(presl->rs_link);
//----      }while(presl != NULL); 
//----    }
//----  return(false);
//----  }


/* 
 * trq_cg_create_task_cgroups
 *
 * Detect if this is a -L resource request. If so
 * create a cgroup directory for each task in each 
 * reqeust.
 *
 * @param  cgroup_path - directory from where cgroup is made.
 * @param  job         - job structure
 */

int trq_cg_create_task_cgroups(

  string  cgroup_path, 
  job    *pjob)

  {
  int            rc;
  char           log_buf[LOCAL_LOG_BUF_SIZE];
  pbs_attribute *pattrL; /* for -L req_information request */


  if (have_incompatible_dash_l_resource(pjob) == true)
    return(PBSE_NONE);

  pattrL = &pjob->ji_wattr[JOB_ATR_req_information];

  if (pattrL == NULL)
    {
    return(PBSE_NONE);
    }

  if ((pattrL->at_flags & ATR_VFLAG_SET) == 0)
    {
    /* This is not a -L request. Just return */
    return(PBSE_NONE);
    }

  char   this_hostname[PBS_MAXHOSTNAME];

  gethostname(this_hostname, PBS_MAXHOSTNAME);
  complete_req *cr = (complete_req *)pattrL->at_val.at_ptr;

  for (unsigned int req_index = 0; req_index < cr->req_count(); req_index++)
    {
    unsigned int total_tasks;

    req &each_req = cr->get_req(req_index);
    total_tasks = each_req.getTaskCount();

    for (unsigned int task_index = 0; task_index < total_tasks; task_index++)
      {
      allocation al;
      string   req_task_path;
      char     req_task_number[MAX_JOBID_LENGTH];

      rc = each_req.get_task_allocation(task_index, al);
      if (rc != PBSE_NONE)
        {
        sprintf(log_buf, "Failed to get allocation for task_index %d: error %d", task_index, rc);
        log_err(-1, __func__, log_buf);
        return(rc);
        }

      std::string task_host;
      each_req.get_task_host_name(task_host, task_index);

      if (task_hosts_match(task_host.c_str(), this_hostname) == false)
        {
        /* this task does not belong to this host. Go to the next one */
        continue;
        }

      /* This task belongs on this host. Add the task cgroup */
      sprintf(req_task_number, "/R%u.t%u", req_index, task_index);
      req_task_path = cgroup_path + req_task_number;
      /* create a cgroup with the job_id.Ri.ty where req_index and task_index are the
         req and task reference */
      rc = trq_cg_mkdir(req_task_path.c_str(), S_IRWXU | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH);
      if ((rc != 0) &&
          (errno != EEXIST))
        {
        sprintf(log_buf, "failed to make directory %s for cgroup: %s",
          req_task_path.c_str(), strerror(errno));
        log_err(errno, __func__, log_buf);
        return(PBSE_SYSTEM); 
        }
      else if (LOGLEVEL >= 9)
        {
        sprintf(log_buf, "Successfully created '%s'", req_task_path.c_str());
        log_event(PBSEVENT_JOB, PBS_EVENTCLASS_JOB, __func__, log_buf);
        }

      }
    }

  return(PBSE_NONE);
  } /* trq_cg_create_task_cgroups*/


/*
 * trq_cg_create_cgroup
 *
 * Create cgroup at path indicated by cgroup_path. Check to see
 * if the job request uses -L. If so create sub-cgroups for
 * each task requested by -L resource request. Otherwise just 
 * create the cgroup at cgroup_path.
 *
 * @param cgroup_path - Path where cgroup will be created.
 * @param job         - job structure
 */

int trq_cg_create_cgroup(
    
  string     &cgroup_path,
  job        *pjob)

  {
  int           rc = PBSE_NONE;
  const char   *job_id = pjob->ji_qs.ji_jobid;
  char          log_buf[LOCAL_LOG_BUF_SIZE];
  string        full_cgroup_path(cgroup_path);

  full_cgroup_path += job_id;

  /* create a cgroup with the job_id as the directory name under the cgroup_path subsystem */
  rc = trq_cg_mkdir(full_cgroup_path.c_str(), S_IRWXU | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH);
  if ((rc != 0) &&
      (errno != EEXIST))
    {
    sprintf(log_buf, "failed to make directory %s for cgroup: %s", full_cgroup_path.c_str(), strerror(errno));
    log_err(errno, __func__, log_buf);
    return(PBSE_SYSTEM); 
    }

  /* create task sub-cgroups if -L resource request */
  rc = trq_cg_create_task_cgroups(full_cgroup_path, pjob);
  if (rc != PBSE_NONE)
    {
    sprintf(log_buf, "failed to create task cgroups: %d", rc);
    log_err(errno, __func__, log_buf);
    }

  return(rc);
  } // END trq_cg_create_cgroup()


/*
 * trq_cg_get_task_set_string
 *
 * creates a set string which can be used where ever
 * a comma delimited set string can be used.
 *
 * @param indices - vector of integers
 * @param set_string - returns a comma delimited string of integers 
 *
 */

int trq_cg_get_task_set_string(
    
  std::vector<int> indices, 
  std::string &set_string)

  {
  unsigned int set_size;
  char  task_element[256];

  /* Figure out the cpuset string */
  set_size = indices.size();

  set_string.clear();

  if (set_size != 0)
    {
    /* add the first element outside the loop because all
       other elements will be comma separated */
    sprintf(task_element, "%d", indices[0]);
    set_string = task_element;

    for (unsigned int i = 1; i < set_size; i++)
      {
      sprintf(task_element, ",%d", indices[i]);
      set_string += task_element;
      }
    }
  else
    {
    /* no set given */
    log_err(PBSE_INDICES_EMPTY, __func__, "No indices found for task set");
    return(PBSE_INDICES_EMPTY);
    }

  return(PBSE_NONE);
  }


/* 
 * trq_cg_write_task_memset_string
 *
 * Write the memset string for the task to cpuset.cpus
 *
 * @param cpuset_path - directory path to cpuset cgroup
 * @param req_num  - The request for this task
 * @param task_num - Task index
 * @param job_id   - Id of the job
 * @param allocation - object with the task cpuset information 
 *
 */

int trq_cg_write_task_memset_string(
    
  string       cpuset_path,
  unsigned int req_num, 
  unsigned int task_num, 
  const char *job_id, 
  allocation &al)

  {
  int          rc = PBSE_NONE;
  string       req_memset_task_path;
  string       memset_string;
  char         req_task_number[256];
  std::string  err_msg;

  sprintf(req_task_number, "%s/R%u.t%u/cpuset.mems", job_id, req_num, task_num);
  req_memset_task_path = cpuset_path + req_task_number;
  rc = trq_cg_get_task_set_string(al.mem_indices, memset_string);

  /* For parallel jobs there may be multiple tasks for the 
     job but only part of them will be for a node. Check to
     see if the task directory exists before trying to write to the file */
  struct stat stat_buf;

  rc = stat(req_memset_task_path.c_str(), &stat_buf);
  if (rc != 0 )
    return(PBSE_NONE);


  if (rc == PBSE_NONE)
    {
    rc = write_to_file(req_memset_task_path.c_str(), memset_string, err_msg);

    if (rc != PBSE_NONE)
      log_err(errno, __func__, err_msg.c_str());
    }

  return(rc);
  } // END trq_cg_write_task_memset_string()



/* 
 * trq_cg_write_task_cpuset_string
 *
 * Write the cpuset string for the task to cpuset.cpus
 *
 * @param cpuset_path - directory path to cpuset cgroup
 * @param req_num  - The request for this task
 * @param task_num - Task index
 * @param job_id   - Id of the job
 * @param allocation - object with the task cpuset information 
 *
 */

int trq_cg_write_task_cpuset_string(
    
  string       cpuset_path,
  unsigned int req_num, 
  unsigned int task_num, 
  const char *job_id, 
  allocation &al)

  {
  int      rc = PBSE_NONE;
  string   req_cpuset_task_path;
  string   cpuset_string;
  char     req_task_number[256];
  string   err_msg;

  sprintf(req_task_number, "%s/R%u.t%u/cpuset.cpus", job_id, req_num, task_num);
  req_cpuset_task_path = cpuset_path + req_task_number;
  
  rc = trq_cg_get_task_set_string(al.cpu_indices, cpuset_string);

  if (rc == PBSE_NONE)
    {
    rc = write_to_file(req_cpuset_task_path.c_str(), cpuset_string, err_msg);

    if (rc != PBSE_NONE)
      log_err(errno, __func__, err_msg.c_str());
    }

  return(rc);
  } // END trq_cg_write_task_cpuset_string()



/*
 * trq_cg_populate_task_cgroups()
 *
 * add cpuset or memory to tasks of jobs.
 *
 * @param which - perform task on memory or cpus
 * @param pjob  - The job structure we are working on.
 *
 */

int trq_cg_populate_task_cgroups(

  string cpuset_path,
  job   *pjob)

  {
  int            rc;
  char          *job_id = pjob->ji_qs.ji_jobid;
  pbs_attribute *pattr;

  /* See if the JOB_ATR_req_information is set. If not
     This was not a -L request */
  pattr = &pjob->ji_wattr[JOB_ATR_req_information];

  if (pattr == NULL)
    {
    return(PBSE_NONE);
    }

  if ( have_incompatible_dash_l_resource(pjob) == true)
    {
    /* this is not a -L request or there are incompatible
       -l resources requested */
    return(PBSE_NONE);
    }

  if ((pattr->at_flags & ATR_VFLAG_SET) == 0)
    {
    /* This is not a -L request. Just return */
    return(PBSE_NONE);
    }

  char   this_hostname[PBS_MAXHOSTNAME];

  gethostname(this_hostname, PBS_MAXHOSTNAME);
  complete_req *cr = (complete_req *)pattr->at_val.at_ptr;
  

  for (unsigned int req_index = 0; req_index < cr->req_count(); req_index++)
    {
    int    task_count;

    req &each_req = cr->get_req(req_index);
    task_count = each_req.getTaskCount();

    for (unsigned int task_index = 0; task_index < task_count; task_index++)
      {
      allocation al;

      rc = each_req.get_task_allocation(task_index, al);
      if (rc != PBSE_NONE)
        {
        return(rc);
        }

      /* Make sure the task is for this host */
      std::string task_host;
      each_req.get_task_host_name(task_host, task_index);

      if (task_hosts_match(task_host.c_str(), this_hostname) == false)
        {
        /* this task does not belong to this host. Go to the next one */
        continue;
        }

      /* This task belongs on this host. */
      rc = trq_cg_write_task_cpuset_string(cpuset_path, req_index, task_index, job_id, al);
      if (rc != PBSE_NONE)
        return(rc);
      rc = trq_cg_write_task_memset_string(cpuset_path, req_index, task_index, job_id, al);
      if (rc != PBSE_NONE)
        return(rc);
 
      }
    }
 
  return(PBSE_NONE);
  }

/*
 * trq_cg_populate_cgroup()
 *
 * Populates a cgroup with the indices specified in the string used
 * @param which - either CPUS or MEMS
 * @param job_id - the id of the job for which we're creating the cpuset
 * @param used - a string containing the indices to write to the file
 * @return PBSE_NONE on success
 */

int trq_cg_populate_cgroup(

  int         which,
  job        *pjob,
  string     &used)

  {
  string  path(cg_cpuset_path);
  string  err_msg;
  int     rc = PBSE_NONE;
  const char *job_id = pjob->ji_qs.ji_jobid;

  path += job_id;
  if (which == CPUS)
    path += "/" + cg_prefix + "cpus";
  else
    path += "/" + cg_prefix + "mems";
    
  rc = write_to_file(path.c_str(), used, err_msg);

  if (rc != PBSE_NONE)
    {
    if (errno == ENOENT)
      {
      /* some versions of Linux don't have cpuset.cpus or cpuset.mems.
         They just have cpus or mems. Try again */
      path = cg_cpuset_path;
      path += "/";
      if (which == CPUS)
        path += "cpus";
      else
        path += "mems";
  
      rc = write_to_file(path.c_str(), used, err_msg);
      }

    if (rc != PBSE_NONE)
      log_err(errno, __func__, err_msg.c_str());
    }

  return(rc);
  } // END trq_cg_populate_cgroup()



/*
 * find_range_in_cpuset_string()
 *
 * Parses a string in the format:
 *
 * host1:<range>[+host2:range[...]]
 * and grabs range to place in the cpuset
 */

int find_range_in_cpuset_string(

  std::string &source,
  std::string &output)

  {
  std::size_t pos = source.find(mom_alias);
  std::size_t end;

  if (pos == std::string::npos)
    {
    sprintf(log_buffer, "Mom '%s' not found in '%s'", mom_alias, source.c_str());
    log_err(-1, __func__, log_buffer);
    return(-1);
    }

  pos += strlen(mom_alias) + 1; // add 1 to pass the ':'
  end = source.find("+", pos);

  output = source.substr(pos, end - pos);

  if (output.size() < 1)
    {
    sprintf(log_buffer, "Mom '%s' has no values in '%s'", mom_alias, source.c_str());
    log_err(-1, __func__, log_buffer);
    return(-1);
    }

  return(PBSE_NONE);
  } // END find_range_in_cpuset_string()



int trq_cg_get_cpuset_and_mem(

  job         *pjob, 
  std::string &cpuset_string, 
  std::string &mem_string)

  {
  int rc = PBSE_NONE;

  if ((pjob->ji_wattr[JOB_ATR_cpuset_string].at_val.at_str == NULL) ||
      (pjob->ji_wattr[JOB_ATR_memset_string].at_val.at_str == NULL))
    {
    sprintf(log_buffer, "Job %s has an empty cpuset or memset string", pjob->ji_qs.ji_jobid);
    log_err(-1, __func__, log_buffer);

    return(-1);
    }

  std::string cpus(pjob->ji_wattr[JOB_ATR_cpuset_string].at_val.at_str);

  if (find_range_in_cpuset_string(cpus, cpuset_string) != 0)
    return(-1);

  std::string mems(pjob->ji_wattr[JOB_ATR_memset_string].at_val.at_str);
  
  if (find_range_in_cpuset_string(mems, mem_string) != 0)
    return(-1);

  return(rc);
  } // END trq_cg_get_cpuset_and_mem()



/*
 * trq_cg_create_all_cgroups()
 *
 * Creates all of the cgroups for a job
 * @param pjob - the job whose cgroups we're creating
 * @return PBSE_NONE on success
 */

int trq_cg_create_all_cgroups(

  job    *pjob)

  {
  int rc = trq_cg_create_cgroup(cg_cpuacct_path, pjob);

  if (rc == PBSE_NONE)
    {
    rc = trq_cg_create_cgroup(cg_memory_path, pjob);

    if (rc == PBSE_NONE)
      {
      std::string  cpus_used;
      std::string  mems_used;
      rc = trq_cg_create_cgroup(cg_cpuset_path, pjob);
      
      // Get the cpu and memory indices used by this job
      rc = trq_cg_get_cpuset_and_mem(pjob, cpus_used, mems_used);
      if (rc == PBSE_NONE)
        {
        rc = trq_cg_populate_cgroup(CPUS, pjob, cpus_used);

        if (rc == PBSE_NONE)
          rc = trq_cg_populate_cgroup(MEMS, pjob, mems_used);

        if (rc == PBSE_NONE)
          rc = trq_cg_populate_task_cgroups(cg_cpuset_path, pjob);
        }
      }

    if (rc == PBSE_NONE)
      rc = trq_cg_create_cgroup(cg_devices_path, pjob);

    if (rc == PBSE_NONE)
      rc = trq_cg_create_cgroup(cg_cpu_path, pjob);

    }

  return(rc);
  } // END trq_cg_create_all_cgroups()



/*
 * trq_cg_set_swap_memory_limit()
 *
 * Sets the memory.limit_in_bytes to memory_limit for the cgroup of this process.
 *
 * @param job_id  -  The job id of the job
 * @param memory_limit - The memory limit for this cgroup 
 *
 * @return PBSE_NONE on success
 */

int trq_cg_set_swap_memory_limit(
  
  const char    *job_id,
  unsigned long long  memory_limit)

  {
  char   mem_limit_string[64];
  string oom_control_name;
  unsigned long long memory_limit_in_bytes;
  std::string        err_msg;
  
  if (memory_limit == 0)
    return(PBSE_NONE);

  memory_limit_in_bytes = memory_limit * 1024;
  sprintf(mem_limit_string, "%lld", memory_limit_in_bytes);

  /* Create a string with a path to the 
     memory.limit_in_bytes cgroup for the job */
  oom_control_name = cg_memory_path + job_id + "/memory.memsw.limit_in_bytes";


  int rc = write_to_file(oom_control_name.c_str(), mem_limit_string, err_msg);
  if (rc != PBSE_NONE)
    {
    sprintf(log_buffer, 
            "%s Kernel may not have been built with CONFIG_MEMCG_SWAP and CONFIG_MEMCG_SWAP_ENABLED",
            err_msg.c_str());
    log_err(errno, __func__, log_buffer);
    }

  return(rc);
  } // END trq_cg_set_swap_memory_limit()



/*
 * trq_cg_set_task_swap_memory_limit()
 *
 * Sets the memory.limit_in_bytes to memory_limit for the cgroup of this process.
 *
 * @param job_id  -  The id of the job
 * @param req_index - the req index for this resource request
 * @param task_index - the task index 
 * @param memory_limit - The memory limit for this cgroup 
 *
 * @return PBSE_NONE on success
 */

int trq_cg_set_task_swap_memory_limit(
  
  const char    *job_id,
  unsigned int   req_index,
  unsigned int   task_index,
  unsigned long long  memory_limit)

  {
  char    mem_limit_string[64];
  string  oom_control_name;
  char    task_directory[1024]; 
  unsigned long long memory_limit_in_bytes;  /* memory_limit comes in as kb */
  string  err_msg;
  
  if (memory_limit == 0)
    return(PBSE_NONE);

  /* convert kb to bytes */
  memory_limit_in_bytes = 1024 * memory_limit;
  sprintf(mem_limit_string, "%lld", memory_limit_in_bytes);

  /* Create a string with a path to the 
     memory.limit_in_bytes cgroup for the job and task */
  sprintf(task_directory, "/R%u.t%u/memory.memsw.limit_in_bytes", req_index, task_index);
  oom_control_name = cg_memory_path + job_id + task_directory;

  /* For parallel jobs there may be multiple tasks for the 
     job but only part of them will be for a node. Check to
     see if the task directory exists before trying to write to the file */
  struct stat stat_buf;
  int         rc;

  rc = stat(oom_control_name.c_str(), &stat_buf);
  if (rc != 0 )
    return(PBSE_NONE);


  /* open the memory.limit_in_bytes file and set it to memory_limit */
  rc = write_to_file(oom_control_name.c_str(), mem_limit_string, err_msg);
  if (rc != PBSE_NONE)
    {
    sprintf(log_buffer, 
            "Warning: %s Kernel may not have been built with CONFIG_MEMCG_SWAP and CONFIG_MEMCG_SWAP_ENABLED",
            err_msg.c_str());
    log_err(errno, __func__, log_buffer);

    rc = PBSE_NONE;
    }

  return(rc);
  } // END trq_cg_set_task_swap_memory_limit()



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
  
  const char    *job_id, 
  unsigned long long  memory_limit)

  {
  char   mem_limit_string[64];
  string oom_control_name;
  string err_msg;
  unsigned long long memory_limit_in_bytes;
  int                rc = PBSE_NONE;
  
  if (memory_limit == 0)
    return(PBSE_NONE);

  // Convert kb to bytes
  memory_limit_in_bytes = 1024 * memory_limit;
  sprintf(mem_limit_string, "%lld", memory_limit_in_bytes);

  /* Create a string with a path to the 
     memory.limit_in_bytes cgroup for the job */
  oom_control_name = cg_memory_path + job_id + "/memory.limit_in_bytes";

  rc = write_to_file(oom_control_name.c_str(), mem_limit_string, err_msg);
  if (rc != PBSE_NONE)
    log_err(errno, __func__, err_msg.c_str());

  return(rc);
  } // END trq_cg_set_resident_memory_limit()

/*
 * trq_cg_set_task_resident_memory_limit()
 *
 * Sets the memory.limit_in_bytes to memory_limit for the task cgroup of this process.
 *
 * @param job_id  -  The id of the job
 * @param req_index - the req index for this resource request
 * @param task_index - the task index 
 * @param memory_limit - The memory limit for this cgroup 
 *
 * @return PBSE_NONE on success
 */

int trq_cg_set_task_resident_memory_limit(
  
  const char    *job_id, 
  unsigned int   req_index,
  unsigned int   task_index,
  unsigned long long memory_limit)

  {
  char   task_directory[1024];
  char   mem_limit_string[64];
  string oom_control_name;
  string err_msg;
  unsigned long long memory_limit_in_bytes;
  struct stat stat_buf;
  int    rc;
  
  if (memory_limit == 0)
    return(PBSE_NONE);

  memory_limit_in_bytes = memory_limit * 1024;
  snprintf(mem_limit_string, sizeof(mem_limit_string), "%llu", memory_limit_in_bytes);
 /* Create a string with a path to the 
     memory.limit_in_bytes cgroup for the job */
  sprintf(task_directory, "/R%u.t%u/memory.limit_in_bytes", req_index, task_index);
  oom_control_name = cg_memory_path + job_id + task_directory;

  /* This may be happening on a sister node. All tasks may not be preset 
     Check to see if the task directory exists first */
  rc = stat(oom_control_name.c_str(), &stat_buf);
  if (rc != 0)
    {
    /* not an error. We just don't have this task */
    return(PBSE_NONE);
    }

  rc = write_to_file(oom_control_name.c_str(), mem_limit_string, err_msg);
  if (rc != PBSE_NONE)
    log_err(errno, __func__, err_msg.c_str());

  return(rc);
  } // END trq_cg_set_task_resident_memory_limit()




/*
 * trq_cg_remove_task_dirs
 *
 * Removes all task directories from a cgroup
 *
 * @param torque_path - path to directory of jobs cgroup
 *
 */

void trq_cg_remove_task_dirs(
    
  const string &torque_path,
  bool          successfully_created)

  {
  DIR *pdir = NULL;
  struct dirent *dent;

  if ((pdir = opendir(torque_path.c_str())) != NULL)
    {
    while ((dent = readdir(pdir)) != NULL)
      {
      if (dent->d_name[0] == 'R')
        {
        std::string dir_name = torque_path + "/" + dent->d_name;
        if ((rmdir_ext(dir_name.c_str(), MAX_RMDIR_RETRIES) != PBSE_NONE) &&
            (successfully_created == true))
          {
          sprintf(log_buffer, "failed to remove directory %s", dir_name.c_str());
          log_err(errno, __func__, log_buffer);
          }
        }
      }

    closedir(pdir);
    }
  else if ((errno != ENOENT) &&
           (successfully_created == true))
    {
    sprintf(log_buffer, "Couldn't open directory '%s' for removal", torque_path.c_str());
    log_err(errno, __func__, log_buffer);
    }

  } // END trq_cg_remove_task_dirs()



/*
 * trq_cg_delete_cgroup_path()
 *
 * cgroup_path - the path to the cgroup which should be deleted
 */

void trq_cg_delete_cgroup_path(

  const string &cgroup_path,
  bool          successfully_created)

  {
  log_event(PBSEVENT_JOB, PBS_EVENTCLASS_JOB, __func__, cgroup_path.c_str());
  trq_cg_remove_task_dirs(cgroup_path, successfully_created);

  if ((rmdir_ext(cgroup_path.c_str(), MAX_RMDIR_RETRIES) != PBSE_NONE) &&
      (successfully_created == true))
    {
    sprintf(log_buffer, "failed to remove cgroup %s ", cgroup_path.c_str());
    log_err(errno, __func__, log_buffer);
    }

  } // END trq_cg_delete_cgroup_path()



/*
 * trq_cg_delete_job_cgroups()
 *
 * removes all of the cgroup directories for this job
 *
 * @param - job_id The id of the job for which to remove cgroups
 */

void trq_cg_delete_job_cgroups(
    
  const char *job_id,
  bool        successfully_created)

  {
  trq_cg_delete_cgroup_path(cg_cpu_path + job_id, successfully_created);

  trq_cg_delete_cgroup_path(cg_cpuacct_path + job_id, successfully_created);
  
  trq_cg_delete_cgroup_path(cg_cpuset_path + job_id, successfully_created);

  trq_cg_delete_cgroup_path(cg_memory_path + job_id, successfully_created);

  trq_cg_delete_cgroup_path(cg_devices_path + job_id, successfully_created);
  } // END trq_cg_delete_job_cgroups()


/*
 * trq_cg_set_forbidden_devices
 *
 * Add the indices in the forbidden list to the devices.deny file
 *
 * @param - forbidden_devices - gpus that are not to be used 
 *                           by this job
 * @param - job_devices_path - path to devices directory for the job
 *
 */

int trq_cg_set_forbidden_devices(std::vector<int> &forbidden_devices, std::string job_devices_path)
  {
  int         rc;
  char        log_buf[LOCAL_LOG_BUF_SIZE];
  std::string devices_deny;
  FILE       *f;

  devices_deny = job_devices_path + '/' + "devices.deny";

  rc = trq_cg_mkdir(job_devices_path.c_str(), S_IRWXU | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH);
  if ((rc != 0) &&
       (errno != EEXIST))
    {
    sprintf(log_buf, "failed to make directory %s for cgroup: %s", job_devices_path.c_str(), strerror(errno));
    log_err(errno, __func__, log_buf);
    return(PBSE_SYSTEM);
    }

  for (std::vector<int>::iterator it = forbidden_devices.begin(); it != forbidden_devices.end(); it++)
    {
    char   restricted_gpu[1024];

#ifdef NVIDIA_GPUS
    sprintf(restricted_gpu, "echo \"c 195:%d rwm\" > %s", *it, devices_deny.c_str());
#endif

#ifdef MIC
    sprintf(restricted_gpu, "echo \"c 245:%d rwm\" > %s", *it, devices_deny.c_str());
#endif

    f = popen(restricted_gpu, "r");
    if (f == NULL)
      {
      sprintf(log_buf, "Failed to add restricted gpu to devices.deny list: index %d in %s", *it, job_devices_path.c_str());
      log_err(errno, __func__, log_buf);
      return(PBSE_SYSTEM);
      }

    pclose(f);
    }

  return(PBSE_NONE);
  }


/*
 * trq_cg_add_devices_to_cgroups
 *
 * This routine adds any mic devices not in this 
 * job (found in the exec_mics list) into the devices.deny
 * list of the gpuset for this job
 *
 * @param pjob - job structure for job we are working on.
 *
 */

int trq_cg_add_devices_to_cgroup(
    
  job *pjob)

  {
  std::vector<unsigned int> device_indices;
  std::vector<int> forbidden_devices;
  std::string job_devices_path;
  char  log_buf[LOCAL_LOG_BUF_SIZE];
  int   rc = PBSE_NONE;
  char *device_str;
  char  suffix[20];
  unsigned int device_count = 0;
  int   index = 0;

#ifdef NVIDIA_GPUS
  device_count = global_gpu_count;
  strcpy(suffix, "-gpu");
  index = JOB_ATR_exec_gpus;
#endif

#ifdef MIC
  device_count = global_mic_count;
  strcpy(suffix, "-mic");
  index = JOB_ATR_exec_mics;
#endif

  /* First make sure we have gpus */
  if (device_count == 0)
    return(PBSE_NONE);

  job_devices_path = cg_devices_path + pjob->ji_qs.ji_jobid;


  /* if there are no gpus given, deny all gpus to the job */
  if (((pjob->ji_wattr[index].at_flags & ATR_VFLAG_SET) == 0) ||
      (pjob->ji_wattr[index].at_val.at_str == NULL))
    {
    for (unsigned int i = 0; i < device_count; i++)
      forbidden_devices.push_back(i);

    rc = trq_cg_set_forbidden_devices(forbidden_devices, job_devices_path);
    if (rc != PBSE_NONE)
      {
      sprintf(log_buf, "Failed to write devices.deny list for job %s", pjob->ji_qs.ji_jobid);
      log_event(PBSEVENT_JOB, PBS_EVENTCLASS_JOB, __func__, log_buf);
      return(rc);
      }

    return(rc);
    }


  device_str = pjob->ji_wattr[index].at_val.at_str;

  device_indices.clear();
  get_device_indices(device_str, device_indices, suffix);


  /* device_indices has the list of gpus for this job. 
     make a list of gpus that are not for this job
     using that list */
  for (unsigned int i = 0; i < device_count; i++)
    {
    bool found;

    found = false;
    for(std::vector<unsigned int>::iterator it = device_indices.begin(); it != device_indices.end(); it++)
      {
      if (*it == i)
        {
        found = true;
        break;
        }
      }
   if (found == false)
     forbidden_devices.push_back(i);

   }

  if (forbidden_devices.size() == 0)
    return(PBSE_NONE);

  rc = trq_cg_set_forbidden_devices(forbidden_devices, job_devices_path);
  if (rc != PBSE_NONE)
    {
    sprintf(log_buf, " 2 Failed to write devices.deny list for job %s", pjob->ji_qs.ji_jobid);
    log_event(PBSEVENT_JOB, PBS_EVENTCLASS_JOB, __func__, log_buf);
    return(rc);
    }
 
  return(rc);
  }

#endif
