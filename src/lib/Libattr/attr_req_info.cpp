#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <vector>
#include <string>
#include <sstream>

#include "pbs_error.h"
#include "attr_req_info.hpp"
#include "attribute.h"
#include "req.hpp"

const char *LPROCS     = "lprocs";
const char *NODES      = "node";
const char *SOCKETS    = "socket";
const char *NUMA_NODE = "numanode";
const char *CORES      = "core";
const char *THREADS    = "thread";
const char *MEMORY     = "memory";
const char *SWAP       = "swap";
const char *DISK       = "disk";


attr_req_info& attr_req_info::operator= (const attr_req_info& newattr_req_info)
  {
  if (this == &newattr_req_info)
    return *this;

  max_lprocs = newattr_req_info.max_lprocs;
  max_mem = newattr_req_info.max_mem;
  max_swap = newattr_req_info.max_swap;
  max_disk = newattr_req_info.max_disk;
  max_nodes = newattr_req_info.max_nodes;
  max_sockets = newattr_req_info.max_sockets;
  max_cores = newattr_req_info.max_cores;
  max_numa_nodes = newattr_req_info.max_numa_nodes;
  max_threads = newattr_req_info.max_threads;
  max_mem_value.atsv_num = newattr_req_info.max_mem_value.atsv_num;
  max_mem_value.atsv_shift = newattr_req_info.max_mem_value.atsv_shift;
  max_swap_value.atsv_num = newattr_req_info.max_swap_value.atsv_num;
  max_swap_value.atsv_shift = newattr_req_info.max_swap_value.atsv_shift;
  max_disk_value.atsv_num = newattr_req_info.max_disk_value.atsv_num;
  max_disk_value.atsv_shift = newattr_req_info.max_disk_value.atsv_shift;

  min_lprocs = newattr_req_info.min_lprocs;
  min_mem = newattr_req_info.min_mem;
  min_swap = newattr_req_info.min_swap;
  min_disk = newattr_req_info.min_disk;
  min_nodes = newattr_req_info.min_nodes;
  min_sockets = newattr_req_info.min_sockets;
  min_cores = newattr_req_info.min_cores;
  min_numa_nodes = newattr_req_info.min_numa_nodes;
  min_threads = newattr_req_info.min_threads;
  min_mem_value.atsv_num = newattr_req_info.min_mem_value.atsv_num;
  min_mem_value.atsv_shift = newattr_req_info.min_mem_value.atsv_shift;
  min_swap_value.atsv_num = newattr_req_info.min_swap_value.atsv_num;
  min_swap_value.atsv_shift = newattr_req_info.min_swap_value.atsv_shift;
  min_disk_value.atsv_num = newattr_req_info.min_disk_value.atsv_num;
  min_disk_value.atsv_shift = newattr_req_info.min_disk_value.atsv_shift;


  default_lprocs = newattr_req_info.default_lprocs;
  default_mem = newattr_req_info.default_mem;
  default_swap = newattr_req_info.default_swap;
  default_disk = newattr_req_info.default_disk;
  default_nodes = newattr_req_info.default_nodes;
  default_sockets = newattr_req_info.default_sockets;
  default_numa_nodes = newattr_req_info.default_numa_nodes;
  default_mem_value.atsv_num = newattr_req_info.default_mem_value.atsv_num;
  default_mem_value.atsv_shift = newattr_req_info.default_mem_value.atsv_shift;
  default_swap_value.atsv_num = newattr_req_info.default_swap_value.atsv_num;
  default_swap_value.atsv_shift = newattr_req_info.default_swap_value.atsv_shift;
  default_disk_value.atsv_num = newattr_req_info.default_disk_value.atsv_num;
  default_disk_value.atsv_shift = newattr_req_info.default_disk_value.atsv_shift;


  return *this; 
  }

attr_req_info::attr_req_info() : max_lprocs(0), max_mem(0), max_swap(0), max_disk(0), max_nodes(0),
                                 max_sockets(0), max_cores(0), max_numa_nodes(0), max_threads(0),
                                 min_lprocs(0), min_mem(0), min_swap(0), min_disk(0), min_nodes(0),
                                 min_sockets(0), min_cores(0), min_numa_nodes(0), min_threads(0),
                                 default_lprocs(0), default_mem(0), default_swap(0), default_disk(0),
                                 default_nodes(0), default_sockets(0), default_numa_nodes(0)
   {
   }

attr_req_info::~attr_req_info()
  {
  }


int attr_req_info::set_min_limit_value(

  const char *rescn,
  const char *val)

  {
  int ret;

  /* Just go through the list of keywords and set the appropriate value */
  if (!strncmp(rescn, LPROCS, strlen(LPROCS)))
    {
    min_lprocs = atoi(val);
    }
  else if (!strncmp(rescn, MEMORY, strlen(MEMORY)))
    {
    ret = to_size(val, &min_mem_value);
    if (ret != PBSE_NONE)
      return(ret);
    min_mem = min_mem_value.atsv_num << min_mem_value.atsv_shift;
    }
  else if (!strncmp(rescn, SWAP, strlen(SWAP)))
    {
    ret = to_size(val, &min_swap_value);
    if (ret != PBSE_NONE)
      return(ret);
    min_swap = min_swap_value.atsv_num << min_swap_value.atsv_shift;
    }
  else if (!strncmp(rescn, DISK, strlen(DISK)))
    {
    ret = to_size(val, &min_disk_value);
    if (ret != PBSE_NONE)
      return(ret);
    min_disk = min_disk_value.atsv_num << min_disk_value.atsv_shift;
    }
  else if (!strncmp(rescn, NODES, strlen(NODES)))
    {
    min_nodes = atoi(val);
    }
  else if (!strncmp(rescn, SOCKETS, strlen(SOCKETS)))
    {
    min_sockets = atoi(val);
    }
  else if (!strncmp(rescn, CORES, strlen(CORES)))
    {
    min_cores = atoi(val);
    }
  else if (!strncmp(rescn, NUMA_NODE, strlen(NUMA_NODE)))
    {
    min_numa_nodes = atoi(val);
    }
  else if (!strncmp(rescn, THREADS, strlen(THREADS)))
    {
    min_threads = atoi(val);
    }
  else
    return(PBSE_BAD_PARAMETER);

  return(PBSE_NONE);

  }


int attr_req_info::set_max_limit_value(

  const char *rescn,
  const char *val)

  {
  int ret;

  /* Just go through the list of keywords and set the appropriate value */
   if (!strncmp(rescn, LPROCS, strlen(LPROCS)))
    {
    max_lprocs = atoi(val);
    }
  else if (!strncmp(rescn, MEMORY, strlen(MEMORY)))
    {
    ret = to_size(val, &max_mem_value);
    if (ret != PBSE_NONE)
      return(ret);
    max_mem = max_mem_value.atsv_num << max_mem_value.atsv_shift;
    }
  else if (!strncmp(rescn, SWAP, strlen(SWAP)))
    {
    ret = to_size(val, &max_swap_value);
    if (ret != PBSE_NONE)
      return(ret);
    max_swap = max_swap_value.atsv_num << max_swap_value.atsv_shift;
    }
  else if (!strncmp(rescn, DISK, strlen(DISK)))
    {
    ret = to_size(val, &max_disk_value);
    if (ret != PBSE_NONE)
      return(ret);
    max_disk = max_disk_value.atsv_num << max_disk_value.atsv_shift;
    }
  else if (!strncmp(rescn, NODES, strlen(NODES)))
    {
    max_nodes = atoi(val);
    }
  else if (!strncmp(rescn, SOCKETS, strlen(SOCKETS)))
    {
    max_sockets = atoi(val);
    }
  else if (!strncmp(rescn, CORES, strlen(CORES)))
    {
    max_cores = atoi(val);
    }
  else if (!strncmp(rescn, NUMA_NODE, strlen(NUMA_NODE)))
    {
    max_numa_nodes = atoi(val);
    }
  else if (!strncmp(rescn, THREADS, strlen(THREADS)))
    {
    max_threads = atoi(val);
    }
  else
    return(PBSE_BAD_PARAMETER);

  return(PBSE_NONE);

  } // END set_max_limit_value()



int attr_req_info::set_default_limit_value(

  const char *rescn,
  const char *val)

  {
  int ret;

  /* Just go through the list of keywords and set the appropriate value */
  if (!strncmp(rescn, LPROCS, strlen(LPROCS)))
    {
    default_lprocs = atoi(val);
    }
  else if (!strncmp(rescn, MEMORY, strlen(MEMORY)))
    {
    ret = to_size(val, &default_mem_value);
    if (ret != PBSE_NONE)
      return(ret);
    default_mem = default_mem_value.atsv_num << default_mem_value.atsv_shift;
    }
  else if (!strncmp(rescn, SWAP, strlen(SWAP)))
    {
    ret = to_size(val, &default_swap_value);
    if (ret != PBSE_NONE)
      return(ret);
    default_swap = default_swap_value.atsv_num << default_swap_value.atsv_shift;
    }
  else if (!strncmp(rescn, DISK, strlen(DISK)))
    {
    ret = to_size(val, &default_disk_value);
    if (ret != PBSE_NONE)
      return(ret);
    default_disk = default_disk_value.atsv_num << default_disk_value.atsv_shift;
    }
  else if (!strncmp(rescn, NODES, strlen(NODES)))
    {
    default_nodes = atoi(val);
    }
  else if (!strncmp(rescn, SOCKETS, strlen(SOCKETS)))
    {
    default_sockets = atoi(val);
    }
  else if (!strncmp(rescn, NUMA_NODE, strlen(NUMA_NODE)))
    {
    default_numa_nodes = atoi(val);
    }
  else
    return(PBSE_BAD_PARAMETER);

  return(PBSE_NONE);

  } // END set_default_limit_value()



int attr_req_info::get_signed_min_limit_value(

  const char *rescn,
  int& value)

  { 
  if (!strncmp(rescn, LPROCS, strlen(LPROCS)))
    {
    value = min_lprocs;
    }
  else if (!strncmp(rescn, NODES, strlen(NODES)))
    {
    value = min_nodes;
    }
  else if (!strncmp(rescn, SOCKETS, strlen(SOCKETS)))
    {
    value = min_sockets;
    }
  else if (!strncmp(rescn, NUMA_NODE, strlen(NUMA_NODE)))
    {
    value = min_numa_nodes;
    }
  else if (!strncmp(rescn, CORES, strlen(CORES)))
    {
    value = min_cores;
    }
  else if (!strncmp(rescn, THREADS, strlen(THREADS)))
    {
    value = min_threads;
    }
  else
    return (PBSE_BAD_PARAMETER);

  return(PBSE_NONE);
  } // END get_signed_min_limit_value()



unsigned int attr_req_info::get_unsigned_min_limit_value(

  const char *rescn,
  unsigned long& value)

  {
  if (!strncmp(rescn, MEMORY, strlen(MEMORY)))
    {
    value = min_mem;
    }
  else if (!strncmp(rescn, SWAP, strlen(SWAP)))
    {
    value = min_swap;
    }
  else if (!strncmp(rescn, DISK, strlen(DISK)))
    {
    value = min_disk;
    }
  else
    return (PBSE_BAD_PARAMETER);

  return(PBSE_NONE);
  }



int attr_req_info::get_signed_max_limit_value(

  const char *rescn,
  int& value)

  {
  if (!strncmp(rescn, LPROCS, strlen(LPROCS)))
    {
    value = max_lprocs;
    }
  else if (!strncmp(rescn, NODES, strlen(NODES)))
    {
    value = max_nodes;
    }
  else if (!strncmp(rescn, SOCKETS, strlen(SOCKETS)))
    {
    value = max_sockets;
    }
  else if (!strncmp(rescn, NUMA_NODE, strlen(NUMA_NODE)))
    {
    value = max_numa_nodes;
    }
  else if (!strncmp(rescn, CORES, strlen(CORES)))
    {
    value = max_cores;
    }
  else if (!strncmp(rescn, THREADS, strlen(THREADS)))
    {
    value = max_threads;
    }
  else
    return (PBSE_BAD_PARAMETER);

  return(PBSE_NONE);
  }



unsigned int attr_req_info::get_unsigned_max_limit_value(

  const char *rescn,
  unsigned long& value)

  {
  if (!strncmp(rescn, MEMORY, strlen(MEMORY)))
    {
    value = max_mem;
    }
  else if (!strncmp(rescn, SWAP, strlen(SWAP)))
    {
    value = max_swap;
    }
  else if (!strncmp(rescn, DISK, strlen(DISK)))
    {
    value = max_disk;
    }
  else
    return (PBSE_BAD_PARAMETER);

  return(PBSE_NONE);
  }

int attr_req_info::get_signed_default_limit_value(

  const char *rescn,
  int& value)

  {
  if (!strncmp(rescn, LPROCS, strlen(LPROCS)))
    {
    value = default_lprocs;
    }
  else if (!strncmp(rescn, NODES, strlen(NODES)))
    {
    value = default_nodes;
    }
  else if (!strncmp(rescn, SOCKETS, strlen(SOCKETS)))
    {
    value = default_sockets;
    }
  else if (!strncmp(rescn, NUMA_NODE, strlen(NUMA_NODE)))
    {
    value = default_numa_nodes;
    }
  else
    return (PBSE_BAD_PARAMETER);

  return(PBSE_NONE);
  }

unsigned int attr_req_info::get_unsigned_default_limit_value(

  const char *rescn,
  unsigned long& value)

  {
  if (!strncmp(rescn, MEMORY, strlen(MEMORY)))
    {
    value = default_mem;
    }
  else if (!strncmp(rescn, SWAP, strlen(SWAP)))
    {
    value = default_swap;
    }
  else if (!strncmp(rescn, DISK, strlen(DISK)))
    {
    value = default_disk;
    }
  else
    return (PBSE_BAD_PARAMETER);

  return(PBSE_NONE);
  }


int attr_req_info::get_max_values(std::vector<std::string>& names, std::vector<std::string>& values)
  {
  char buf[100];

  if (max_lprocs != 0)
    {
    sprintf(buf, "%s", LPROCS );
    names.push_back(buf);
    sprintf(buf, "%d", max_lprocs);
    values.push_back(buf);
    }

  if (max_mem != 0)
    {
    sprintf(buf, "%s", MEMORY);
    names.push_back(buf);
    create_size_string(buf, max_mem_value);
    values.push_back(buf);
    }

  if (max_swap != 0)
    {
    sprintf(buf, "%s", SWAP);
    names.push_back(buf);
    create_size_string(buf, max_swap_value);
    values.push_back(buf);
    }

  if (max_disk != 0)
    {
    sprintf(buf, "%s", DISK);
    names.push_back(buf);
    create_size_string(buf, max_disk_value);
    values.push_back(buf);
    }

  if (max_nodes != 0)
    {
    sprintf(buf, "%s", NODES);
    names.push_back(buf);
    sprintf(buf, "%d", max_nodes);
    values.push_back(buf);
    }

  if (max_sockets != 0)
    {
    sprintf(buf, "%s", SOCKETS);
    names.push_back(buf);
    sprintf(buf, "%d", max_sockets);
    values.push_back(buf);
    }

  if (max_numa_nodes != 0)
    {
    sprintf(buf, "%s", NUMA_NODE);
    names.push_back(buf);
    sprintf(buf, "%d", max_numa_nodes);
    values.push_back(buf);
    }

  if (max_cores != 0)
    {
    sprintf(buf,"%s", CORES);
    names.push_back(buf);
    sprintf(buf, "%d", max_cores);
    values.push_back(buf);
    }

  if (max_threads != 0)
    {
    sprintf(buf, "%s", THREADS);
    names.push_back(buf);
    sprintf(buf, "%d", max_threads);
    values.push_back(buf);
    }

  return(PBSE_NONE);
  }

int attr_req_info::get_min_values(std::vector<std::string>& names, std::vector<std::string>& values)
  {
  char buf[100];

  if (min_lprocs != 0)
    {
    sprintf(buf, "%s", LPROCS);
    names.push_back(buf);
    sprintf(buf, "%d", min_lprocs);
    values.push_back(buf);
    }

  if (min_mem != 0)
    {
    sprintf(buf, "%s", MEMORY);
    names.push_back(buf);
    create_size_string(buf, min_mem_value);
    values.push_back(buf);
    }

  if (min_swap != 0)
    {
    sprintf(buf, "%s", SWAP);
    names.push_back(buf);
    create_size_string(buf, min_swap_value);
    values.push_back(buf);
    }

  if (min_disk != 0)
    {
    sprintf(buf, "%s", DISK);
    names.push_back(buf);
    create_size_string(buf, min_disk_value);
    values.push_back(buf);
    }

  if (min_nodes != 0)
    {
    sprintf(buf, "%s", NODES);
    names.push_back(buf);
    sprintf(buf, "%d", min_nodes);
    values.push_back(buf);
    }

  if (min_sockets != 0)
    {
    sprintf(buf, "%s", SOCKETS);
    names.push_back(buf);
    sprintf(buf, "%d", min_sockets);
    values.push_back(buf);
    }

  if (min_numa_nodes != 0)
    {
    sprintf(buf, "%s", NUMA_NODE);
    names.push_back(buf);
    sprintf(buf, "%d", min_numa_nodes);
    values.push_back(buf);
    }


  if (min_cores != 0)
    {
    sprintf(buf, "%s", CORES);
    names.push_back(buf);
    sprintf(buf, "%d", min_cores);
    values.push_back(buf);
    }

  if (min_threads != 0)
    {
    sprintf(buf, "%s", THREADS);
    names.push_back(buf);
    sprintf(buf, "%d", min_threads);
    values.push_back(buf);
    }

  return(PBSE_NONE);
  }



int attr_req_info::get_default_values(std::vector<std::string>& names, std::vector<std::string>& values)
  {
  char buf[100];

  if (default_lprocs != 0)
    {
    sprintf(buf, "%s", LPROCS);
    names.push_back(buf);
    sprintf(buf, "%d", default_lprocs);
    values.push_back(buf);
    }

  if (default_mem != 0)
    {
    sprintf(buf, "%s", MEMORY);
    names.push_back(buf);
    create_size_string(buf, default_mem_value);
    values.push_back(buf);
    }

  if (default_swap != 0)
    {
    sprintf(buf, "%s", SWAP);
    names.push_back(buf);
    create_size_string(buf, default_swap_value);
    values.push_back(buf);
    }

  if (default_disk != 0)
    {
    sprintf(buf, "%s", DISK);
    names.push_back(buf);
    create_size_string(buf, default_disk_value);
    values.push_back(buf);
    }

  if (default_nodes != 0)
    {
    sprintf(buf, "%s", NODES);
    names.push_back(buf);
    sprintf(buf, "%d", default_nodes);
    values.push_back(buf);
    }

  if (default_sockets != 0)
    {
    sprintf(buf, "%s", SOCKETS);
    names.push_back(buf);
    sprintf(buf, "%d", default_sockets);
    values.push_back(buf);
    }

  if (default_numa_nodes != 0)
    {
    sprintf(buf, "%s", NUMA_NODE);
    names.push_back(buf);
    sprintf(buf, "%d", default_numa_nodes);
    values.push_back(buf);
    }

  return(PBSE_NONE);
  }


int attr_req_info::check_max_values(

  std::vector<std::string>& names, 
  std::vector<std::string>& values)

  {
  int ret;
  int           signed_value;
  unsigned long unsigned_value;
  int           lprocs = 1;


  for (unsigned int i = 0; i < names.size(); i++)
    {
    signed_value = 0;
    unsigned_value = 0;
    
    if (names[i].find("lprocs") == 0)
      lprocs = strtol(values[i].c_str(), NULL, 10);
    else if (names[i].find("thread_usage_policy") == 0)
      {
      if (values[i] == use_cores)
        {
        if ((this->max_cores > 0) &&
            (this->max_cores < lprocs))
          return(PBSE_EXLIMIT);
        }

      if ((this->max_threads > 0) &&
          (this->max_threads < lprocs))
        return(PBSE_EXLIMIT);

      lprocs = 1;
      }

    ret = this->get_signed_max_limit_value(names[i].c_str(), signed_value);
    if ((ret == PBSE_NONE) && (signed_value != 0))
      {
      int val;

      val = atoi(values[i].c_str());
      if (val > signed_value)
        {
        return(PBSE_EXLIMIT);
        }
      }
        
    ret = this->get_unsigned_max_limit_value(names[i].c_str(), unsigned_value);
    if ((ret == PBSE_NONE) && (unsigned_value != 0))
      {
      struct size_value user_val;
      unsigned long uval;

      ret = to_size(values[i].c_str(), &user_val); 
      if (ret != PBSE_NONE)
        return(ret);

      uval = user_val.atsv_num << user_val.atsv_shift;

      if (uval > unsigned_value)
        {
        return(PBSE_EXLIMIT);
        }
      }
    }
  
  return(PBSE_NONE);
  }



int attr_req_info::check_min_values(

  std::vector<std::string>& names, 
  std::vector<std::string>& values)

  {
  int ret;
  int           signed_value;
  unsigned long unsigned_value;
  int           lprocs = 1;

  for (unsigned int i = 0; i < names.size(); i++)
    {
    signed_value = 0;
    unsigned_value = 0;

    if (names[i].find("lprocs") == 0)
      lprocs = strtol(values[i].c_str(), NULL, 10);
    else if (names[i].find("thread_usage_policy") == 0)
      {
      if (values[i] == use_cores)
        {
        if ((this->min_cores > 0) &&
            (this->min_cores > lprocs))
          return(PBSE_MINLIMIT);
        }
      else if (this->min_cores > 0)
        return(PBSE_MINLIMIT);

      if ((this->min_threads > 0) &&
          (this->min_threads > lprocs))
        return(PBSE_MINLIMIT);

      lprocs = 1;
      }

    ret = this->get_signed_min_limit_value(names[i].c_str(), signed_value);
    if ((ret == PBSE_NONE) &&
        (signed_value != 0))
      {
      int val;

      val = atoi(values[i].c_str());
      if ((val != 0) && (val < signed_value))
        {
        return(PBSE_MINLIMIT);
        }
      }
        
    ret = this->get_unsigned_min_limit_value(names[i].c_str(), unsigned_value);
    if ((ret == PBSE_NONE) && (unsigned_value != 0))
      {
      unsigned long uval;
      struct size_value user_value;

      ret = to_size(values[i].c_str(), &user_value);
      if (ret != PBSE_NONE)
        return(ret);

      uval = user_value.atsv_num << user_value.atsv_shift;

      if ((uval != 0) && (uval < unsigned_value))
        {
        return(PBSE_MINLIMIT);
        }
      }
    }
 
  return(PBSE_NONE);
  } // END check_min_values()



/* 
 * add_defalut_values
 *
 * This function takes a vector of strings which are names and values
 * of resources in the request. It then checks if any default values
 * have been set which are not in the list and adds them to the name_to_add
 * and values_to_add vectors 
 *
 * @param names  -  vector of strings with resource names that are part of a request
 * @param values -  vector of strings which are the values for values in the names parameter
 * @param names_to_add - a vector of strings containing the names of default values which were not 
                         in the user's request.
 * @param values_to_add - a vector of strings containing the values of default values which were
                          not set in the user's request.
 */
int attr_req_info::add_default_values(

  std::vector<std::string>& names,
  std::vector<std::string>& values,
  std::vector<std::string>& names_to_add,
  std::vector<std::string>& values_to_add)

  {
  std::vector<std::string> default_names;
  std::vector<std::string> default_values;

  /* Get all of the default values that have been set on the queue */
  this->get_default_values(default_names, default_values);

  /* iterate over each default value that is set in the queue and 
   * see if there is a match in the users req
   */
  for (unsigned int i = 0; i < default_names.size(); i++)
    {
    bool found;

    found = false;
    for (unsigned int j = 0; j < names.size(); j++)
      {
      std::string dflt_name = default_names[i].c_str();
      std::string req_name = names[j].c_str();

      if (!strcmp(dflt_name.c_str(), req_name.c_str()))
        {
        if (!strcmp(dflt_name.c_str(), LPROCS))
          {
          /* lprocs is a special case. We always get at least one lprocs. If set to one
             assume user did not request lprocs */
          if (!strcmp(values[j].c_str(), "1"))
            {
            continue;
            }
          }
        found = true;
        break;
        }

      }
    if (found == false)
      {
      names_to_add.push_back(default_names[i]);
      values_to_add.push_back(default_values[i]);
      }
    }

 
  return(PBSE_NONE);
  } // END add_default_values()


