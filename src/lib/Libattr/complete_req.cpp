
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "complete_req.hpp"
#include "pbs_error.h"
#include "log.h"
#include "resource.h"


complete_req::complete_req() : reqs()
  {
  }



void complete_req::set_value_from_nodes(

  const char *node_val)

  {
  char *work_str = strdup(node_val);
  char *ptr = work_str;

  while (ptr != NULL)
    {
    char *next = strchr(ptr, '+');

    if (next != NULL)
      {
      *next = '\0';
      next++;
      }

    req r(ptr);
    this->add_req(r);

    ptr = next;
    }

  free(work_str);
  } // END set_value_from_nodes()



complete_req::complete_req(

  tlist_head &resources) : reqs()

  {
  resource      *pr = (resource *)GET_NEXT(resources);
  int            task_count = 0;
  int            execution_slots = 0;
  unsigned long  mem = 0;

  while (pr != NULL)
    {
    if (!strcmp(pr->rs_defin->rs_name, "nodes"))
      {
      this->set_value_from_nodes(pr->rs_value.at_val.at_str);
      }
    else if ((!strcmp(pr->rs_defin->rs_name, "procs")) ||
             (!strcmp(pr->rs_defin->rs_name, "size")))
      {
      task_count = pr->rs_value.at_val.at_long;
      execution_slots = 1;
      }
    else if (!strcmp(pr->rs_defin->rs_name, "ncpus"))
      {
      task_count = 1;
      execution_slots = pr->rs_value.at_val.at_long;
      }
    else if ((!strcmp(pr->rs_defin->rs_name, "pmem")) ||
             (!strcmp(pr->rs_defin->rs_name, "vmem")) ||
             (!strcmp(pr->rs_defin->rs_name, "mem")))
      {
      mem = pr->rs_value.at_val.at_size.atsv_num;
      int shift = pr->rs_value.at_val.at_size.atsv_shift;

      // Convert to kb
      while (shift > 10)
        {
        mem *= 1024;
        shift -= 10;
        }
      }

    pr = (resource *)GET_NEXT(pr->rs_link);
    }
 
  if (this->reqs.size() == 0)
    {
    // Handle the case where no -lnodes request was made
    // Distribute the memory across the tasks as -l memory is per job
    if (task_count > 1)
      mem /= task_count;
    
    req r;
    if (task_count != 0)
      r.set_task_count(task_count);

    r.set_memory(mem);

    if (execution_slots != 0)
      r.set_execution_slots(execution_slots);

    this->add_req(r);
    }
  else if (mem != 0)
    {
    // Handle the case where a -lnodes request was made
    int           total_tasks = 0;
    unsigned long mem_per_task;
    for (unsigned int i = 0; i < this->reqs.size(); i++)
      total_tasks += this->reqs[i].getTaskCount();

    mem_per_task = mem / total_tasks;

    for (unsigned int i = 0; i < this->reqs.size(); i++)
      {
      req &r = this->reqs[i];
      r.set_memory(mem_per_task);
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
  const char *value)

  {
  if (index < 0)
    return(PBSE_BAD_PARAMETER);

  while (this->reqs.size() <= (unsigned int)index)
    {
    req r;
    r.set_index(this->reqs.size());
    this->reqs.push_back(r);
    }

  return(this->reqs[index].set_value(name, value));
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
    
/* 
 * get_swap_memory_for_this_host()
 */

unsigned long complete_req::get_swap_memory_for_this_host(
    
  const std::string &hostname) const

  {
  unsigned long mem = 0;
  
  for (unsigned int i = 0; i < this->reqs.size(); i++)
    mem += this->reqs[i].get_swap_for_host(hostname);

  return(mem);
  } // END get_memory_for_host()



/* 
 * get_memory_for_this_host()
 */

unsigned long complete_req::get_memory_for_this_host(
    
  const std::string &hostname) const

  {
  unsigned long mem = 0;
  
  for (unsigned int i = 0; i < this->reqs.size(); i++)
    mem += this->reqs[i].get_memory_for_host(hostname);

  return(mem);
  } // END get_memory_for_host()



req &complete_req::get_req(

  int i)

  {
  return(this->reqs[i]);
  } // END get_req()



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
    if (i >= this->reqs.size())
      {
      // Moab seems to think there are more reqs than we do
      snprintf(log_buf, sizeof(log_buf),
                 "We received %d or more req assignments for job %s which has only %d reqs.",
                 i, job_id, (int)this->reqs.size());
      log_event(PBSEVENT_JOB, PBS_EVENTCLASS_SERVER, __func__, log_buf);
      break;
      }

    if (bar != NULL)
      *bar = '\0';

    this->reqs[i].set_hostlist(current);

    // Advance the req to the next set of hosts
    if (bar != NULL)
      {
      current = bar + 1;
      bar = strchr(bar + 1, '|');
      }
    else
      current = bar;

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

