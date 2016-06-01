/*
*         OpenPBS (Portable Batch System) v2.3 Software License
*
* Copyright (c) 1999-2000 Veridian Information Solutions, Inc.
* All rights reserved.
*
* ---------------------------------------------------------------------------
* For a license to use or redistribute the OpenPBS software under conditions
* other than those described below, or to purchase support for this software,
* please contact Veridian Systems, PBS Products Department ("Licensor") at:
*
*    www.OpenPBS.org  +1 650 967-4675                  sales@OpenPBS.org
*                        877 902-4PBS (US toll-free)
* ---------------------------------------------------------------------------
*
* This license covers use of the OpenPBS v2.3 software (the "Software") at
* your site or location, and, for certain users, redistribution of the
* Software to other sites and locations.  Use and redistribution of
* OpenPBS v2.3 in source and binary forms, with or without modification,
* are permitted provided that all of the following conditions are met.
* After December 31, 2001, only conditions 3-6 must be met:
*
* 1. Commercial and/or non-commercial use of the Software is permitted
*    provided a current software registration is on file at www.OpenPBS.org.
*    If use of this software contributes to a publication, product, or
*    service, proper attribution must be given; see www.OpenPBS.org/credit.html
*
* 2. Redistribution in any form is only permitted for non-commercial,
*    non-profit purposes.  There can be no charge for the Software or any
*    software incorporating the Software.  Further, there can be no
*    expectation of revenue generated as a consequence of redistributing
*    the Software.
*
* 3. Any Redistribution of source code must retain the above copyright notice
*    and the acknowledgment contained in paragraph 6, this list of conditions
*    and the disclaimer contained in paragraph 7.
*
* 4. Any Redistribution in binary form must reproduce the above copyright
*    notice and the acknowledgment contained in paragraph 6, this list of
*    conditions and the disclaimer contained in paragraph 7 in the
*    documentation and/or other materials provided with the distribution.
*
* 5. Redistributions in any form must be accompanied by information on how to
*    obtain complete source code for the OpenPBS software and any
*    modifications and/or additions to the OpenPBS software.  The source code
*    must either be included in the distribution or be available for no more
*    than the cost of distribution plus a nominal fee, and all modifications
*    and additions to the Software must be freely redistributable by any party
*    (including Licensor) without restriction.
*
* 6. All advertising materials mentioning features or use of the Software must
*    display the following acknowledgment:
*
*     "This product includes software developed by NASA Ames Research Center,
*     Lawrence Livermore National Laboratory, and Veridian Information
*     Solutions, Inc.
*     Visit www.OpenPBS.org for OpenPBS software support,
*     products, and information."
*
* 7. DISCLAIMER OF WARRANTY
*
* THIS SOFTWARE IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND. ANY EXPRESS
* OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
* OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, AND NON-INFRINGEMENT
* ARE EXPRESSLY DISCLAIMED.
*
* IN NO EVENT SHALL VERIDIAN CORPORATION, ITS AFFILIATED COMPANIES, OR THE
* U.S. GOVERNMENT OR ANY OF ITS AGENCIES BE LIABLE FOR ANY DIRECT OR INDIRECT,
* INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
* LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,
* OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
* LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
* NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
* EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*
* This license will be governed by the laws of the Commonwealth of Virginia,
* without reference to its choice of law rules.
*/


#include <iostream>
#include <fstream>
#include <string>
#include <stdlib.h>

#include "pbs_config.h"

#include "numa_node.hpp"
#include "utils.h"
#include "mom_memory.h"

#ifdef PENABLE_LINUX26_CPUSETS
extern int MOMConfigUseSMT;
bool is_physical_core(unsigned int os_index);
extern char cpuset_prefix[MAXPATHLEN];

void get_cpu_list(const char *jobid, char *buf, int bufsize);
#endif



/*
 * parse_cpu_string
 * parses a string for indices in the format int[-int][,int[-int]...]
 *
 * @pre-cond: str is a valid string pointer
 * @post-cond: internal variables representing cpus are populated
 */

void numa_node::parse_cpu_string(

  std::string &line)

  {
  std::vector<int> indices;

  this->total_cpus = 0;
  this->available_cpus = 0;

  translate_range_string_to_vector(line.c_str(), indices);

  for (int i = 0; i < indices.size(); i++)
    {
#ifdef PENABLE_LINUX26_CPUSETS
    if ((MOMConfigUseSMT == 1) ||
        (is_physical_core(indices[i]) == true))
#endif
      {
      this->cpu_indices.push_back(indices[i]);
      this->cpu_avail.push_back(true);
      this->total_cpus++;
      this->available_cpus++;
      }
    }

  } /* END parse_cpu_string */



/*
 * get_cpuinfo()
 * opens the path to populate internal cpu values
 *
 * @pre-cond: path must be a valid string pointer
 * @post-cond: internal values for cpus are populated
 */
void numa_node::get_cpuinfo(
    
  const char *path)

  {
  if (path == NULL)
    return;

  std::string   line;
  std::ifstream myfile(path);

  if (myfile.is_open())
    {
    getline(myfile, line);

    /* format int[-int][,int[-int]...] */
    parse_cpu_string(line);
    }
  }



void numa_node::get_meminfo(

  const char *path)

  {
  if (path == NULL)
    return;

  proc_mem_t *memnode = get_proc_mem_from_path(path);

  if (memnode != NULL)
    {
    this->total_memory = memnode->mem_total / 1024;
    this->available_memory = this->total_memory;

    free(memnode);
    }
  }


numa_node::numa_node(

  const numa_node &nn) : total_cpus(nn.total_cpus), total_memory(nn.total_memory),
                         available_memory(nn.available_memory), available_cpus(nn.available_cpus),
                         my_index(nn.my_index), cpu_indices(nn.cpu_indices), cpu_avail(nn.cpu_avail),
                         allocations(nn.allocations)

  {
  }
  
numa_node::numa_node(
    
  const char *node_path,
  int         index) : my_index(index)

  {
  char path[MAXLINE];

  snprintf(path, sizeof(path), "%s/cpulist", node_path);
  get_cpuinfo(path);

  snprintf(path, sizeof(path), "%s/meminfo", node_path);
  get_meminfo(path);
  }


numa_node::numa_node() : total_cpus(0), total_memory (0), available_memory(0), available_cpus(0), my_index(0),
                         cpu_indices(), cpu_avail(), allocations()

  {
  }



bool numa_node::completely_fits(

  int           num_cpus,
  unsigned long memory) const

  {
  bool fits = false;

  if ((num_cpus <= this->available_cpus) &&
      (memory <= this->available_memory))
    fits = true;

  return(fits);
  }



void numa_node::mark_cpu_as_in_use(

  unsigned int  index,
  allocation   &alloc)

  {
  this->cpu_avail[index] = false;
  alloc.cpu_indices.push_back(this->cpu_indices[index]);
  alloc.cpus++;
  this->available_cpus--;
  } /* END mark_cpu_as_in_use() */



void numa_node::mark_memory_as_in_use(

  unsigned long  memory,
  allocation     &alloc)

  {
  if (memory <= this->available_memory)
    {
    alloc.memory += memory;
    this->available_memory -= memory;
    }
  else
    {
    alloc.memory += this->available_memory;
    this->available_memory = 0;
    }
  } /* END mark_memory_as_in_use() */



void numa_node::reserve(
    
  int            num_cpus,
  unsigned long  memory,
  const char    *jobid,
  allocation    &alloc)

  {
  alloc.jobid = jobid;
  
  for (unsigned int i = 0; i < this->cpu_indices.size() && alloc.cpus < num_cpus; i++)
    {
    if (this->cpu_avail[i] == true)
      {
      mark_cpu_as_in_use(i, alloc);
      }
    }

  mark_memory_as_in_use(memory, alloc);

  this->allocations.push_back(alloc);
  }



int numa_node::in_this_numa_node(

  int cpu_index)

  {
  int match = -1;

  for (int i = 0; i < this->cpu_indices.size(); i++)
    {
    if (cpu_index == this->cpu_indices[i])
      {
      match = i;
      break;
      }
    }

  return(match);
  }



void numa_node::recover_reservation(
    
  int            num_cpus,
  unsigned long  memory,
  const char    *jobid,
  allocation    &alloc)

  {
  char             cpuset_buf[MAXPATHLEN + 1];
  std::vector<int> indices;
  bool             matches_this_numa_node = false;

  alloc.jobid = jobid;

#ifdef PENABLE_LINUX26_CPUSETS
  get_cpu_list(jobid, cpuset_buf, sizeof(cpuset_buf));
#endif

  translate_range_string_to_vector(cpuset_buf, indices);

  for (int i = 0; i < indices.size(); i++)
    {
#ifdef PENABLE_LINUX26_CPUSETS
    if ((MOMConfigUseSMT == 1) ||
        (is_physical_core(indices[i]) == true))
#endif
      {
      int my_index = in_this_numa_node(indices[i]);

      if (my_index == -1)
        continue;

      matches_this_numa_node = true;

      if (this->cpu_avail[my_index] == true)
        {
        mark_cpu_as_in_use(my_index, alloc);
        }
      }
    }

  if (matches_this_numa_node == true)
    {
    mark_memory_as_in_use(memory, alloc);
    
    this->allocations.push_back(alloc);
    }
  } /* END recover_reservation() */
  


void numa_node::remove_job(
    
  const char *jobid)

  {
  for (unsigned int i = 0; i < this->allocations.size(); i++)
    {
    if (this->allocations[i].jobid == jobid)
      {
      allocation a = this->allocations[i];
      this->available_cpus   += a.cpus;
      this->available_memory += a.memory;

      /* find the job indices used by this allocation and release them */
      for (unsigned int k = 0; k < this->cpu_indices.size(); k++)
        {
        for (unsigned int j = 0; j < a.cpu_indices.size(); j++)
          {
          if (this->cpu_indices[k] == a.cpu_indices[j])
            this->cpu_avail[k] = true;
          }
        }

      this->allocations.erase(this->allocations.begin() + i);
      break;
      }
    }
  }



void numa_node::get_job_indices(

  const char       *jobid,
  std::vector<int> &indices,
  bool              cpus) const

  {
  for (unsigned int i = 0; i < this->allocations.size(); i++)
    {
    if (this->allocations[i].jobid == jobid)
      {
      if (cpus)
        {
        for (unsigned int j = 0; j < this->allocations[i].cpu_indices.size(); j++)
          indices.push_back(this->allocations[i].cpu_indices[j]);
        }
      else
        indices.push_back(this->my_index);
      }
    }
  }

unsigned int numa_node::get_total_cpus() const

  {
  return(this->total_cpus);
  }

unsigned long numa_node::get_total_memory() const

  {
  return(this->total_memory);
  }

unsigned long numa_node::get_available_memory() const

  {
  return(this->available_memory);
  }

unsigned int numa_node::get_available_cpus() const

  {
  return(this->available_cpus);
  }

unsigned int numa_node::get_my_index() const

  {
  return(this->my_index);
  }

