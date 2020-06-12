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

#include <string.h>
#include <stdio.h>
#include <sstream>

#include "allocation.hpp"
#include "req.hpp"
#include "pbs_error.h"
#include "utils.h"

const int MEM_INDICES = 0;
const int CPU_INDICES = 1;
const int GPU_INDICES = 2;
const int MIC_INDICES = 3;

const int exclusive_none   = 0;
const int exclusive_node   = 1;
const int exclusive_socket = 2;
const int exclusive_chip   = 3;
const int exclusive_core   = 4;
const int exclusive_thread = 5;
const int exclusive_legacy = 6; /* for the -l resource request. Direct pbs_server to allocate cores only */
const int exclusive_legacy2 = 7; /* for the -l resource request. Direct pbs_server to allocate cores and threads */

allocation::allocation(

  const allocation &alloc) : cpu_place_indices(alloc.cpu_place_indices), cpu_indices(alloc.cpu_indices), 
                             mem_indices(alloc.mem_indices), gpu_indices(alloc.gpu_indices), 
                             mic_indices(alloc.mic_indices), memory(alloc.memory), cpus(alloc.cpus), 
                             cores(alloc.cores), threads(alloc.threads), place_cpus(alloc.place_cpus),
                             place_type(alloc.place_type), cores_only(alloc.cores_only),
                             jobid(alloc.jobid), hostname(alloc.hostname),
                             gpus(alloc.gpus), mics(alloc.mics), task_cput_used(alloc.task_cput_used),
                             task_memory_used(alloc.task_memory_used)

  {
  }

allocation &allocation::operator =(

  const allocation &other)

  {
  this->cpu_place_indices = other.cpu_place_indices;
  this->cpu_indices = other.cpu_indices;
  this->mem_indices = other.mem_indices;
  this->gpu_indices = other.gpu_indices;
  this->mic_indices = other.mic_indices;

  this->memory = other.memory;
  this->cpus = other.cpus;
  this->cores = other.cores;
  this->threads = other.threads;
  this->place_cpus = other.place_cpus;
  this->place_type = other.place_type;
  this->cores_only = other.cores_only;
  this->jobid = other.jobid;
  this->hostname = other.hostname;
  this->gpus = other.gpus;
  this->mics = other.mics;
  this->task_cput_used = other.task_cput_used;
  this->task_memory_used = other.task_memory_used;

  return(*this);
  }

allocation::allocation() : cpu_place_indices(), cpu_indices(), mem_indices(), gpu_indices(), mic_indices(), memory(0),
                           cpus(0), cores(0), threads(0), place_cpus(0), place_type(exclusive_none),
                           cores_only(false), jobid(), hostname(), gpus(0), mics(0), task_cput_used(0),
                           task_memory_used(0)

  {
  }

allocation::allocation(

  const req &r) : cpu_place_indices(), cpu_indices(), mem_indices(), gpu_indices(), mic_indices(),
                  cores(0), threads(0),
                  place_cpus(0), place_type(exclusive_none), cores_only(false), jobid(), hostname(),
                  task_cput_used(0), task_memory_used(0)

  {
  this->cpus = r.getExecutionSlots();
  this->memory = r.get_memory_per_task();
  this->gpus = r.get_gpus();
  this->mics = r.getMics();

  if (r.getThreadUsageString() == use_cores)
    this->cores_only = true;

  this->set_place_type(r.getPlacementType());

  if (r.getPlaceCores() > 0)
    this->place_cpus = r.getPlaceCores();
  else if (r.getPlaceThreads() > 0)
    this->place_cpus = r.getPlaceThreads();
  }

allocation::allocation(

  const char *jid) : cpu_place_indices(), cpu_indices(), mem_indices(), gpu_indices(), mic_indices(), memory(0),
                     cpus(0), cores(0), threads(0), place_cpus(0), place_type(exclusive_none), cores_only(false),
                     jobid(jid), hostname(), gpus(0), mics(0), task_cput_used(0), task_memory_used(0)

  {
  }

int allocation::add_allocation(

  const allocation &other)

  {
  for (unsigned int i = 0; i < other.cpu_indices.size(); i++)
    this->cpu_indices.push_back(other.cpu_indices[i]);

  for (unsigned int i = 0; i < other.cpu_place_indices.size(); i++)
    this->cpu_place_indices.push_back(other.cpu_place_indices[i]);

  this->cpus += other.cpus;
  this->cores += other.cores;
  this->threads += other.threads;
  this->place_cpus += other.place_cpus;

  // Make sure mem indices are unique
  for (unsigned int i = 0; i < other.mem_indices.size(); i++)
    {
    bool found = false;

    for (unsigned int j = 0; j < this->mem_indices.size(); j++)
      {
      if (this->mem_indices[j] == other.mem_indices[i])
        {
        found = true;
        break;
        }
      }

    if (found == false)
      this->mem_indices.push_back(other.mem_indices[i]);
    }

  this->memory += other.memory;

  for (unsigned int i = 0; i < other.gpu_indices.size(); i++)
    this->gpu_indices.push_back(other.gpu_indices[i]);

  for (unsigned int i = 0; i < other.mic_indices.size(); i++)
    this->mic_indices.push_back(other.mic_indices[i]);

  return(PBSE_NONE);
  }


void allocation::place_indices_in_string(

  std::string &output,
  int          which)

  {
  switch (which)
    {
    case MEM_INDICES:
    
      translate_vector_to_range_string(output, this->mem_indices);
      break;

    case CPU_INDICES:
    
      translate_vector_to_range_string(output, this->cpu_indices);
      break;

    case GPU_INDICES:
    
      translate_vector_to_range_string(output, this->gpu_indices);
      break;

    case MIC_INDICES:
    
      translate_vector_to_range_string(output, this->mic_indices);
      break;
    }
  } // END place_indices_in_string()



void allocation::place_indices_in_string(

  cgroup_info &cgi)

  {
  this->place_indices_in_string(cgi.mem_string, MEM_INDICES);
  this->place_indices_in_string(cgi.cpu_string, CPU_INDICES);
  this->place_indices_in_string(cgi.gpu_string, GPU_INDICES);
  this->place_indices_in_string(cgi.mic_string, MIC_INDICES);
  } // END place_indices_in_string()



void allocation::set_place_type(

  const std::string &placement_str)

  {
  if (placement_str == place_node)
    this->place_type = exclusive_node;
  else if (placement_str.find(place_socket) == 0)
    this->place_type = exclusive_socket;
  else if (placement_str.find(place_numa_node) == 0)
    this->place_type = exclusive_chip;
  else if (placement_str.find(place_core) == 0)
    this->place_type = exclusive_core;
  else if (placement_str.find(place_thread) == 0)
    this->place_type = exclusive_thread;
  else if (placement_str.find(place_legacy2) == 0) // place_legacy2 must be evaluated before place_legacy
    this->place_type = exclusive_legacy2;
  else if (placement_str.find(place_legacy) == 0)
    this->place_type = exclusive_legacy;
  else
    this->place_type = exclusive_none;
  } // END set_place_type()


void allocation::get_place_type(

  int &place_type)

  {
  place_type = this->place_type;
  }


/*
 * write_task_information()
 *
 * Output the information for this task's usage to the string
 *
 * @param task_info - the output string of the task information
 */

void allocation::write_task_information(

  std::string &task_info) const

  {
  std::string cpus;
  std::string mems;
  std::string gpus;
  std::string mics;
  char        buf[256];

  translate_vector_to_range_string(cpus, this->cpu_indices);
  translate_vector_to_range_string(mems, this->mem_indices);
  task_info = "{\"task\":{\"cpu_list\":\"" + cpus;
  task_info += "\",\"mem_list\":\"" + mems;

  if (this->gpu_indices.size() > 0)
    {
    translate_vector_to_range_string(gpus, this->gpu_indices);
    task_info += "\",\"gpu_list\":\"" + gpus;
    }

  if (this->mic_indices.size() > 0)
    {
    translate_vector_to_range_string(mics, this->mic_indices);
    task_info += "\",\"mic_list\":\"" + mics;
    }

  if (this->task_cput_used != 0)
    {
    unsigned long cput_used = this->task_cput_used;
    snprintf(buf, sizeof(buf), "\",\"cpu_time_used\":%lu", cput_used);
    task_info += buf;
    }
  if (this->task_memory_used != 0)
    {
    snprintf(buf, sizeof(buf), "\",\"memory_used\":%llu", this->task_memory_used);
    task_info += buf;
    }
  snprintf(buf, sizeof(buf), "\",\"cores\":%d", this->cores);
  task_info += buf;
  snprintf(buf, sizeof(buf), ",\"threads\":%d", this->threads);
  task_info += buf;
  task_info += ",\"host\":\"" + this->hostname + "\"}}";
  } // END write_task_information()



void allocation::initialize_from_string(

  const std::string &task_info)

  {
  std::string  cpus;
  std::string  mems;
  char        *work_str = strdup(task_info.c_str());
  char        *val = work_str;
  char        *ptr = strstr(work_str, "cpu_list\":");
  std::string  storage;

/* The order these are evaluated makes a difference */
  if (ptr != NULL)
    {
    val = ptr + strlen("cpu_list\":") + 1; // add 1 for the open quote
    capture_until_close_character(&val, storage, '"');
    if (this->cpu_indices.size() == 0)
      translate_range_string_to_vector(storage.c_str(), this->cpu_indices);
    }

  if ((ptr = strstr(val, "mem_list\":")) != NULL)
    {
    val = ptr + strlen("mem_list\":") + 1; // add 1 for the open quote
    capture_until_close_character(&val, storage, '"');
    if (this->mem_indices.size() == 0)
      translate_range_string_to_vector(storage.c_str(), this->mem_indices);
    }
  
  if ((ptr = strstr(val, "cpu_time_used\":")) != NULL)
    {
    val = ptr + strlen("cpu_time_used\":");
    this->task_cput_used = strtol(val, &val, 10);
    }

  if ((ptr = strstr(val, "memory_used\":")) != NULL)
    {
    val = ptr + strlen("memory_used\":");
    this->task_memory_used = strtoll(val, &val, 10);
    }

  if ((ptr = strstr(val, "cores\":")) != NULL)
    {
    int new_val;

    val = ptr + strlen("cores\":");
    new_val = strtol(val, &val, 10);

    /* check for non-zero. Default values are 0. Don't set default
       values because they tend to overwrite exising valid values
       in the attr_atomic_set function */
    if (new_val != 0)
      this->cores = new_val;
    }

  if ((ptr = strstr(val, "threads\":")) != NULL)
    {
    int new_val;

    val = ptr + strlen("threads\":");
    new_val = strtol(val, &val, 10);

     /* check for non-zero. Default values are 0. Don't set default
       values because they tend to overwrite exising valid values
       in the attr_atomic_set function */
    if (new_val != 0)
      this->threads = new_val;
    }


  if ((ptr = strstr(val, "host\":")) != NULL)
    {
    val = ptr + strlen("host\":") + 1;
    storage.clear();
    capture_until_close_character(&val, storage, '"');
    if (storage.size() != 0)
      this->hostname = storage;
    }

  free(work_str);
  } // END initialize_from_string()



/*
 * clear() 
 * Resets this allocation - used for unit tests
 */

void allocation::clear()

  {
  this->cpu_place_indices.clear();
  this->cpu_indices.clear();
  this->mem_indices.clear();
  this->gpu_indices.clear();
  this->mic_indices.clear();
  this->memory = 0;
  this->cpus = 0;
  this->cores = 0;
  this->threads = 0;
  this->place_cpus = 0;
  this->place_type = 0;
  this->cores_only = 0;
  this->jobid.clear();
  this->hostname.clear();
  this->gpus = 0;
  this->mics = 0;
  this->task_cput_used = 0;
  this->task_memory_used = 0;
  }



void allocation::set_host(

  const char *hostname)

  {
  this->hostname = hostname;
  }

void allocation::set_memory_used(

  const unsigned long long mem_used)

  {
  this->task_memory_used = mem_used;
  }

void allocation::set_cput_used(

  const unsigned long cput_used)

  {
  this->task_cput_used = cput_used;
  }

void allocation::get_stats_used(

  unsigned long &cput_used, 
  unsigned long long &memory_used)

  {
  cput_used = this->task_cput_used;
  memory_used = this->task_memory_used;
  }

void allocation::set_task_usage_stats(

  unsigned long      cput_used,
  unsigned long long mem_used)

  {
  this->set_cput_used(cput_used);
  this->set_memory_used(mem_used);
  }

/* get_task_host_name()
 *
 * Return a copy of the host name for this task
 *
 * @param host - name of the host where this task should reside
 */

void allocation::get_task_host_name(

  std::string &host)

  {
  host = this->hostname;
  }

void allocation::set_gpus_remaining(

  int gpus_remaining)

  {
  this->gpus = gpus_remaining;
  }

void allocation::get_gpus_remaining(

  int &gpus_remaining)

  {
  gpus_remaining = this->gpus;
  }

void allocation::set_mics_remaining(

  int mics_remaining)

  {
  this->mics = mics_remaining;
  }

void allocation::get_mics_remaining(

  int &mics_remaining)

  {
  mics_remaining = this->mics;
  }

/*
 * fully_placed()
 *
 * Returns true if this allocation has nothing left to place
 * Nothing means no cpus, memory, mics, or gpus
 *
 * @return true if this allocation has nothing left to place, else false
 */

bool allocation::fully_placed() const
  {
  return((this->cpus == 0) &&
         (this->memory == 0) &&
         (this->mics == 0) &&
         (this->gpus == 0) &&
         (this->place_cpus == 0));
  }

/*
 * partially_placed()
 *
 * Tells if this allocation has been partially placed or not
 * Should be used with allocations contructed using allocation(req) constructor
 *
 * @param r - the req used to construct this allocation object
 * @return true if part of this allocation has been placed, false otherwise
 */

bool allocation::partially_placed(
    
  const req &r) const

  {
  return ((this->cpus != r.getExecutionSlots()) ||
          (this->memory != r.get_memory_per_task()) ||
          (this->gpus != r.get_gpus()) ||
          (this->mics != r.getMics()) ||
          ((r.getPlaceCores() > 0) &&
           (this->place_cpus != r.getPlaceCores())) ||
          ((r.getPlaceThreads() > 0) &&
           (this->place_cpus != r.getPlaceThreads())));
  }



/*
 * adjust_for_spread()
 *
 * Reduces the amount of resources to place for this allocation according to how
 * many things it is being spread across.
 *
 * @param quantity - the number of things it is being spread across
 * @param finding_remainder - tells us whether or not we're dividing or finding a remainder
 */

void allocation::adjust_for_spread(

  unsigned int quantity, 
  bool         finding_remainder)

  {
  if (quantity != 0)
    {
    if (finding_remainder == false)
      {
      this->cpus /= quantity;
      this->gpus /= quantity;
      this->mics /= quantity;
      }
    else
      {
      this->cpus %= quantity;
      this->gpus %= quantity;
      this->mics %= quantity;
      }
    }
  } // END adjust_for_spread()



/*
 * adjust_for_remainder()
 *
 * Optionally increases the amount of resources to place for this allocation according to the 
 * remainder parameter. Also, reduces remainder to account for the adjustment.
 *
 * @param remainder - the remainder that needs to be accounted for.
 */

void allocation::adjust_for_remainder(

  allocation &remainder)

  {
  if (remainder.cpus > 0)
    {
    this->cpus++;
    remainder.cpus--;
    }

  if (remainder.mics > 0)
    {
    this->mics++;
    remainder.mics--;
    }

  if (remainder.gpus > 0)
    {
    this->gpus++;
    remainder.gpus--;
    }
  } // END adjust_for_remainder()

