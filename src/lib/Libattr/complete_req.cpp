
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>

#include "pbs_ifl.h"
#include "complete_req.hpp"
#include "pbs_error.h"
#include "log.h"
#include "resource.h"
#include "utils.h"


complete_req::complete_req() : reqs()
  {
  }



void complete_req::set_value_from_nodes(

  const char *node_val,
  int        &task_count)

  {
  if (node_val != NULL)
    {
    char *work_str = strdup(node_val);
    char *ptr = work_str;
    task_count = 0;

    while (ptr != NULL)
      {
      char *next = strchr(ptr, '+');

      if (next != NULL)
        {
        *next = '\0';
        next++;
        }

      req r(ptr);
      task_count += r.getTaskCount();
      this->add_req(r);

      ptr = next;
      }

    free(work_str);
    }
  } // END set_value_from_nodes()



/*
 * Constructor from a resource list (-l request)
 *
 * @param resc_ptr - a pointer to the vector of resources
 * @param ppn_needed - ppn required for the job described by this completed req
 * @param legacy_vmem - true if vmem should be considered per node, false if per job
 */

complete_req::complete_req(

  void  *resc_ptr,
  int    ppn_needed, // UNUSED
  bool   legacy_vmem) : reqs()

  {
  std::vector<resource> *resources = (std::vector<resource> *)resc_ptr;
  int                    task_count = 0;
  int                    execution_slots = 0;
  unsigned long long     mem_values[4];
  int                    active_index[2];

  if (resources == NULL)
    return;

  active_index[0] = _MEM_;
  active_index[1] = _VMEM_;
  memset(mem_values, 0, sizeof(mem_values));

  for (size_t i = 0; i < resources->size(); i++)
    {
    resource &r = resources->at(i);

    if (!strcmp(r.rs_defin->rs_name, "nodes"))
      {
      this->set_value_from_nodes(r.rs_value.at_val.at_str, task_count);
      }
    else if ((!strcmp(r.rs_defin->rs_name, "procs")) ||
             (!strcmp(r.rs_defin->rs_name, "size")))
      {
      task_count = r.rs_value.at_val.at_long;
      execution_slots = 1;
      }
    else if (!strcmp(r.rs_defin->rs_name, "ncpus"))
      {
      task_count = 1;
      execution_slots = r.rs_value.at_val.at_long;
      }
    else if ((!strcmp(r.rs_defin->rs_name, "pmem")) ||
             (!strcmp(r.rs_defin->rs_name, "mem")) ||
             (!strcmp(r.rs_defin->rs_name, "vmem")) ||
             (!strcmp(r.rs_defin->rs_name, "pvmem")))
      {
      int index;

      // pmem is per task
      if (r.rs_defin->rs_name[0] == 'm')
        index = _MEM_;
      else if (r.rs_defin->rs_name[0] == 'v')
        index = _VMEM_;
      else
        {
        // It is either pmem or pvmem at this point
        if (r.rs_defin->rs_name[1] == 'm')
          index = _PMEM_;
        else
          index = _PVMEM_;
        }

      // Make sure that we take mem over 
      mem_values[index] = r.rs_value.at_val.at_size.atsv_num;
      int shift = r.rs_value.at_val.at_size.atsv_shift;

      if (shift == 0)
        {
        // -l used in submission so convert
        //   bytes to kb
        mem_values[index] /= 1024;
        }
      else    
        {
        // Convert to kb
        while (shift > 10) 
          {
          mem_values[index] *= 1024;
          shift -= 10;  
          }
        }
      }
    }

  // Set mem and swap from mem_values
  unsigned long long mem = mem_values[_MEM_];
  if ((legacy_vmem == false) &&
      (task_count != 0))
    mem /= task_count;
  if (mem_values[_PMEM_] > mem)
    {
    active_index[0] = _PMEM_;
    mem = mem_values[_PMEM_];
    }

  unsigned long long vmem = mem_values[_VMEM_];
  if ((legacy_vmem == false) &&
      (task_count != 0))
    vmem /= task_count;
  if (mem_values[_PVMEM_] > vmem)
    {
    active_index[1] = _PVMEM_;
    vmem = mem_values[_PVMEM_];
    }
 
  if (this->reqs.size() == 0)
    {
    // Handle the case where no -lnodes request was made
    // Distribute the memory across the tasks as -l memory is per job
    req r;
    if (task_count != 0)
      {
      r.set_task_count(task_count);
      r.set_placement_type(place_legacy);
      }

    r.set_memory(mem);
    r.set_swap(vmem);

    if (execution_slots != 0)
      r.set_execution_slots(execution_slots);

    this->add_req(r);
    }
  else
    {
    // Handle the case where a -lnodes request was made
    if (mem != 0)
      {
      if (active_index[0] == _MEM_)
        {
        for (unsigned int i = 0; i < this->reqs.size(); i++)
          {
          req &r = this->reqs[i];
          r.set_memory(mem);
          }
        }
      else if (active_index[0] == _PMEM_)
        {
        for (unsigned int i = 0; i < this->reqs.size(); i++)
          {
          req &r = this->reqs[i];
          int  ppn_per_req = r.get_execution_slots();

          r.set_memory(mem * ppn_per_req);
          }
        }
      }
  
    if (vmem != 0)
      {
      // Handle the case where a -lnodes request was made
      if (active_index[1] == _VMEM_)
        {
        for (unsigned int i = 0; i < this->reqs.size(); i++)
          {
          req &r = this->reqs[i];
          r.set_swap(vmem);
          }
        }
      else if (active_index[1] == _PVMEM_)
        {
        for (unsigned int i = 0; i < this->reqs.size(); i++)
          {
          req &r = this->reqs[i];
          int ppn_per_req = r.get_execution_slots();

          r.set_swap(vmem * ppn_per_req);
          }
        }
      }
    }
  } // END constructor from resource list



complete_req::complete_req(
    
  const complete_req &other) : reqs(other.reqs)

  {
  }



complete_req &complete_req::operator =(

  const complete_req &other)

  {
  this->reqs = other.reqs;
  return *this;
  }



/*
 * add_req()
 *
 * adds a req to this complete req's list of reqs after setting its index
 *
 * @param r - the req to add
 */

void complete_req::add_req(

  req &r)

  {
  // start indices at 0
  r.set_index(reqs.size());
  reqs.push_back(r);
  }



/*
 * set_from_string()
 *
 * initializes this object from a string exactly
 * matching the output produced by toString()
 *
 * @param obj_string - the string produced by a call to toString()
 */

void complete_req::set_from_string(

  const std::string &obj_string)

  {
  static std::string req_start("req[");
  std::size_t curr_pos = obj_string.find(req_start, 1);
  std::size_t next_req = obj_string.find(req_start, curr_pos + 1);

  while (curr_pos != std::string::npos)
    {
    std::string one_req(obj_string.substr(curr_pos, next_req - curr_pos));
    req r;

    r.set_from_string(one_req);

    add_req(r);

    curr_pos = next_req;

    if (curr_pos != std::string::npos)
      next_req = obj_string.find(req_start, curr_pos + 1);
    }

  } // END set_from_string() 



void complete_req::toString(

  std::string &output) const

  {
  output.clear();

  for (unsigned int i = 0; i < this->reqs.size(); i++)
    {
    if (i > 0)
      output += '\n';

    this->reqs[i].toString(output);
    }
  } // END toString() 



/*
 * req_count()
 *
 * @return - the number of reqs in this complete req class
 */

int complete_req::req_count() const

  {
  return(this->reqs.size());
  } // END req_count()


/*
 * set_task_memory_used()
 *
 * Sets the memory used for a specific task
 *
 * @param req_index - index of req where task memory is to be set
 * @param task_indx - index of task inside of req to be set
 * @ mem_used       - Memory used by task
 *
 */

int complete_req::set_task_memory_used(
  int req_index, 
  int task_index, 
  const unsigned long long mem_used)

  {
  if (this->reqs.size() < req_index)
    return(PBSE_BAD_PARAMETER);

  return(this->reqs[req_index].set_memory_used(task_index, mem_used));
  }

/*
 * set_task_cput_used()
 *
 * Sets the cput used for a specific task
 *
 * @param req_index - index of req where task cpu time is to be set
 * @param task_indx - index of task inside of req to be set
 * @ cput_used       - cpu time used by task
 *
 */

int complete_req::set_task_cput_used(
  int req_index, 
  int task_index, 
  const unsigned long cput_used)

  {
  if ((this->reqs.size() == 0) || (this->reqs.size() < req_index))
    return(PBSE_BAD_PARAMETER);

  return(this->reqs[req_index].set_cput_used(task_index, cput_used));
  }


/*
 * set_value()
 *
 * Sets a value for a compound req attribute name such as task_usage.0.task.1
 *
 * @param name - the name of the value to be set
 * @param value - the value to be set
 * @return PBSE_NONE if the name is valid and the index is >= 0, PBSE_BAD_PARAMETER otherwise
 *
 */


int complete_req::set_task_value(

  const char *name,
  const char *value)
  
  {
  int   rc = PBSE_NONE;
  const char *dot1;
  const char *dot2;
  unsigned int   req_index;
  unsigned int   task_index;

  dot1 = strchr(name, '.');
  dot2 = strrchr(name, '.');

  if ((dot1 == NULL) || (dot2 == NULL))
    {
    return(PBSE_BAD_PARAMETER);
    }

  req_index = strtol(dot1 + 1, NULL, 10);
  task_index = strtol(dot2 + 1, NULL, 10);

  // Add new reqs if needed
  while (this->reqs.size() <= req_index)
    {
    req r;
    r.set_index(this->reqs.size());
    this->reqs.push_back(r);
    }

  rc = this->reqs[req_index].set_task_value(value, task_index);

  return(rc);
 
  }

/*
 * set_value()
 *
 * Sets a value for the req at the specified index, creating the index
 * if it doesn't already exist
 *
 * @param index - the index of the req to set this particular value
 * @param name - the name of the value to be set
 * @param value - the value to be set
 * @return PBSE_NONE if the name is valid and the index is >= 0, PBSE_BAD_PARAMETER otherwise
 *
 */

int complete_req::set_value(

  int         index,
  const char *name,
  const char *value,
  bool        is_default)

  {
  if (index < 0)
    return(PBSE_BAD_PARAMETER);

  while (this->reqs.size() <= (unsigned int)index)
    {
    req r;
    r.set_index(this->reqs.size());
    this->reqs.push_back(r);
    }

  return(this->reqs[index].set_value(name, value, is_default));
  } // END set_value()



/*
 * get_values()
 *
 * Gets the name and value of each attribute set for each req held
 *
 * @param names - the vector where each name is stored
 * @param values - the vector where each value is stored
 */

void complete_req::get_values(

  std::vector<std::string> &names,
  std::vector<std::string> &values) const

  {
  for (unsigned int i = 0; i < this->reqs.size(); i++)
    this->reqs[i].get_values(names, values);
  } // END get_values()
    

/* Get the memory request for the given req_index */
unsigned long long complete_req::get_memory_per_task(

  unsigned int req_index)

  {
  return(this->reqs[req_index].get_memory_per_task());
  }


/* Get the swap request for the given req_index */
unsigned long long complete_req::get_swap_per_task(

  unsigned int req_index)

  {
  return(this->reqs[req_index].get_swap_per_task());
  }


/* 
 * get_swap_memory_for_this_host()
 *
 * @return the amount of swap requested for this host in kb
 */

unsigned long long complete_req::get_swap_memory_for_this_host(
    
  const std::string &hostname) const

  {
  unsigned long long mem = 0;
  
  for (unsigned int i = 0; i < this->reqs.size(); i++)
    mem += this->reqs[i].get_swap_for_host(hostname);

  return(mem);
  } // END get_swap_memory_for_this_host()



/* 
 * get_memory_for_this_host()
 *
 * @return the amount of memory requested for this host in kb
 */

unsigned long long complete_req::get_memory_for_this_host(
    
  const std::string &hostname) const

  {
  unsigned long long mem = 0;
  
  for (unsigned int i = 0; i < this->reqs.size(); i++)
    mem += this->reqs[i].get_memory_for_host(hostname);

  return(mem);
  } // END get_memory_for_host()



req &complete_req::get_req(

  int i)

  {
  return(this->reqs[i]);
  } // END get_req()


/* get_req_index_for_host
 * 
 * This method is used to get the index of a request
 * when a job is completed. It relies on the hostlist
 * to be set.
 *
 * @param host - The host name for which the req index is to be returned
 * @param req_index - The request index of the host.
 *
 */

int complete_req::get_req_index_for_host(

  const char   *host,
  unsigned int &req_index)

  {
  char  log_buf[LOCAL_LOG_BUF_SIZE];
  std::vector<req>::iterator it = this->reqs.begin();
  
  req_index = 0; /* initialize */

  for (unsigned int i = 0; it != this->reqs.end(); i++, it++)
    {
    int rc;
    req current_req = this->get_req(i);
    std::vector<std::string> req_hostlist;
    char hostlist[PBS_MAXHOSTNAME];
    char *tok_ptr;

    rc = current_req.getHostlist(req_hostlist);
    if (rc == PBSE_EMPTY)
      {
      req_index++;
      continue;
      }

    for (unsigned int i = 0; i < req_hostlist.size(); i++)
      {
      strcpy(hostlist, req_hostlist[i].c_str());
      tok_ptr = strchr(hostlist, ':');
      if (tok_ptr != NULL)
        *tok_ptr = '\0';

      if (!strcmp(hostlist, host))
        {
        return(PBSE_NONE);
        }
      else
        {
        /* see if the short name matches */
        if (task_hosts_match(hostlist, host) == true)
          {
          return(PBSE_NONE);
          }
        }
      }
    req_index++;
    }

  sprintf(log_buf, "A req for host %s not found", host);
  log_event(PBSEVENT_JOB, PBS_EVENTCLASS_JOB, __func__, log_buf);
  return(PBSE_REQ_NOT_FOUND);

  }




/*
 * get_req_and_task_index
 *
 * get the req and task for the rank given. Used to find the cgroup
 * for a given process.
 *
 * @param rank  -  id given by tm_spawn when starting a new process
 * @param req_index - request index for the rank given.
 * @param task_index - task index for the rank given.
 *
 */

int complete_req::get_req_and_task_index(

  const int rank, 
  unsigned int &req_index, 
  unsigned int &task_index)

  {
  int tasks_counted = 0;

  for (unsigned int req_count = 0; req_count < this->req_count(); req_count++)
    {
    for (unsigned int task_count = 0; task_count < this->reqs[req_count].getTaskCount(); task_count++)
      {
      int rc;
      allocation al;
      rc = this->reqs[req_count].get_task_allocation(task_count, al);
      if (rc != PBSE_NONE)
        continue;

      for (unsigned int cpus_per_task = 0; cpus_per_task < al.cpu_indices.size(); cpus_per_task++)
        {
        if (tasks_counted == rank)
          {
          req_index = req_count;
          task_index = task_count;
          return(PBSE_NONE);
          }

        tasks_counted++;
        }
      }
    }

  return(PBSE_NO_PROCESS_RANK);
  }



/*
 * get_req_and_task_index_from_local_rank()
 *
 * From the local rank, determines which req and task this is a part of
 * @param local_rank (I) - the rank for this process among processes on this node
 * @param req_index (O) - we write the index of the req here
 * @param task_index (O) - we write the index of the task here
 * @param host - the current hostname. Do not count ranks from other nodes.
 * @return PBSE_NONE - if we could locate a local rank for this job, PBSE_NO_PROCESS_RANK otherwise.
 * Returning PBSE_NO_PROCESS_RANK should cause this to be placed in the cgroup for the entire host
 */

int complete_req::get_req_and_task_index_from_local_rank(
    
  int           local_rank,
  unsigned int &req_index,
  unsigned int &task_index,
  const char   *host) const

  {
  int rc = PBSE_NO_PROCESS_RANK;
  int tasks_counted = 0;

  for (unsigned int req_count = 0; req_count < this->req_count(); req_count++)
    {
    for (unsigned int task_count = 0; task_count < this->reqs[req_count].getTaskCount(); task_count++)
      {
      int rc;
      allocation al;
      rc = this->reqs[req_count].get_task_allocation(task_count, al);
      if (rc != PBSE_NONE)
        continue;

      if (al.hostname != host)
        continue;

      for (unsigned int cpus_per_task = 0; cpus_per_task < al.cpu_indices.size(); cpus_per_task++)
        {
        if (tasks_counted == local_rank)
          {
          req_index = req_count;
          task_index = task_count;
          return(PBSE_NONE);
          }

        tasks_counted++;
        }
      }
    }

  return(rc);
  } // END get_req_and_task_index_from_local_rank()



int complete_req::update_hostlist(
    
  const std::string &host_spec,
  int                req_index)

  {
  if ((req_index < 0) ||
      (req_index >= this->reqs.size()))
    {
    return(-1);
    }

  this->reqs[req_index].update_hostlist(host_spec);
  
  return(PBSE_NONE);
  } // END update_hostlist()



void complete_req::set_hostlists(

  const char *job_id,
  const char *host_list)

  {
  if ((job_id == NULL) ||
      (host_list == NULL))
    return;

  char *work_list = strdup(host_list);
  unsigned int   i = 0;
  char *bar = strchr(work_list, '|');
  char *current = work_list;
  char  log_buf[LOCAL_LOG_BUF_SIZE];

  while (current != NULL)
    {
    int   ppn = 0;     /* total ppn used so far per req */
    int   req_ppn = 0; /* the total requested ppn per req */
    int   task_count = 0;
    int   execution_slots = 0;
    if (i >= this->reqs.size())
      {
      // Moab seems to think there are more reqs than we do
      snprintf(log_buf, sizeof(log_buf),
                 "We received %d or more req assignments for job %s which has only %d reqs.",
                 i, job_id, (int)this->reqs.size());
      log_event(PBSEVENT_JOB, PBS_EVENTCLASS_SERVER, __func__, log_buf);
      break;
      }

    /* determine the number of requested ppn for this task.
       This will help us determine if the current
       req spans multiple nodes */
       
    task_count = this->reqs[i].getTaskCount();
    execution_slots = this->reqs[i].get_execution_slots();
    req_ppn = task_count * execution_slots;
    
    do
      {
      if (bar != NULL)
        *bar = '\0';

      char *ppn_ptr = strchr(current, ':');

      if (ppn_ptr != NULL)
        {
        /* We want to get the ppn=x part of the hostlist */
        /* make sure we have ppn=x */
        ppn_ptr = strstr(ppn_ptr, "ppn=");
        if (ppn_ptr == NULL)
          {
          ppn += 1;
          }
        else
          {
          /* Now get the ppn value */
          ppn_ptr = strchr(ppn_ptr, '=');
          ppn_ptr++;
          ppn += atoi(ppn_ptr);
          }
        }

      this->reqs[i].set_hostlist(current);

      // Advance the req to the next set of hosts
      if (bar != NULL)
        {
        current = bar + 1;
        bar = strchr(bar + 1, '|');
        }
      else
        {
        current = bar;
        }
      } while((ppn < req_ppn) && (current != NULL));


    i++;
    }

  if (i < this->reqs.size())
    {
    // We think there are more reqs than Moab does
    snprintf(log_buf, sizeof(log_buf),
      "We only received %d or req assignments for job %s which has %d reqs.",
      i, job_id, (int)this->reqs.size());
    log_event(PBSEVENT_JOB, PBS_EVENTCLASS_SERVER, __func__, log_buf);
    }

  free(work_list);
  } // END set_hostslists()



void complete_req::clear_allocations()

  {
  for (unsigned int i = 0; i < this->reqs.size(); i++)
    this->reqs[i].clear_allocations();
  } // END clear_allocations()


void complete_req::clear_reqs()

  {
  this->reqs.clear();
  }

int complete_req::get_task_stats(

  unsigned int                    &req_index, 
  std::vector<int>                &task_index, 
  std::vector<unsigned long>      &cput_used, 
  std::vector<unsigned long long> &mem_used,
  const char                      *hostname)

  {
  int rc = PBSE_NONE;
  char   buf[LOCAL_LOG_BUF_SIZE];

  rc = this->get_req_index_for_host(hostname, req_index);
  if (rc != PBSE_NONE)
    {
    sprintf(buf, "Could not find req for host %s", hostname);
    log_event(PBSEVENT_JOB, PBS_EVENTCLASS_JOB, __func__, buf);
    return(rc);
    }

  this->reqs[req_index].get_task_stats(task_index, cput_used, mem_used);

  return(rc);
  }



void complete_req::set_task_usage_stats(

  int                req_index, 
  int                task_index, 
  unsigned long      cput_used, 
  unsigned long long mem_used)

  {
  if (req_index > this->req_count())
    return;

  this->reqs[req_index].set_task_usage_stats(task_index, cput_used, mem_used);

  }

unsigned int complete_req::get_num_reqs()
  {
  return(this->reqs.size());
  }
