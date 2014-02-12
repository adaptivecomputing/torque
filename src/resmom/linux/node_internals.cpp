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

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>


#include "node_internals.hpp"
#include "utils.h"

const char *base_path = "/sys/devices/system/node/";

node_internals::node_internals()
  {
  char        path[MAXLINE];
  bool        fail = false;
  struct stat buf;

  for (int i = 0; fail == false; i++)
    {
    snprintf(path, sizeof(path), "%snode%d", base_path, i);

    if (stat(path, &buf) == 0)
      {
      numa_node n(path, i);
      this->numa_nodes.push_back(n);
      }
    else
      fail = true;
    }
  }
  
node_internals::node_internals(
    
  const node_internals &ni) : numa_nodes(ni.numa_nodes)

  {
  }



node_internals::node_internals(
    
  const std::vector<numa_node> nodes) : numa_nodes(nodes)

  {
  }



int node_internals::num_numa_nodes() const

  {
  return(this->numa_nodes.size());
  }



void node_internals::reserve(

  int            num_cpus,
  unsigned long  memory,
  const char    *jobid)

  {
  bool done = false;

  for (unsigned int i = 0; i < this->numa_nodes.size(); i++)
    {
    if (this->numa_nodes[i].completely_fits(num_cpus, memory))
      {
      allocation alloc;
      this->numa_nodes[i].reserve(num_cpus, memory, jobid, alloc);
      done = true;
      break;
      }
    }

  if (done == false)
    {
    for (unsigned int i = 0; i < this->numa_nodes.size(); i++)
      {
      allocation alloc;
      this->numa_nodes[i].reserve(num_cpus, memory, jobid, alloc);
      num_cpus -= alloc.cpus;
      memory -= alloc.memory;

      if ((num_cpus <= 0) &&
          (memory <= 0))
        break;
      }
    }
  }



void node_internals::recover_reservation(

  int            num_cpus,
  unsigned long  memory,
  const char    *jobid)

  {

  for (unsigned int i = 0; i < this->numa_nodes.size(); i++)
    {
    allocation alloc;
    this->numa_nodes[i].recover_reservation(num_cpus, memory, jobid, alloc);
    num_cpus -= alloc.cpus;
    memory -= alloc.memory;

    if ((num_cpus <= 0) &&
        (memory <= 0))
      break;
    }

  if (memory > 0)
    reserve(num_cpus, memory, jobid);
  }



void node_internals::remove_job(

  const char *jobid)

  {
  for (unsigned int i = 0; i < this->numa_nodes.size(); i++)
    this->numa_nodes[i].remove_job(jobid);
  }



std::vector<int> *node_internals::get_cpu_indices(
    
  const char *jobid)

  {
  std::vector<int> *cpu_indices = new std::vector<int>();
    
  for (unsigned int i = 0; i < this->numa_nodes.size(); i++)
    this->numa_nodes[i].get_job_indices(jobid, *cpu_indices, true);

  return(cpu_indices);
  }



std::vector<int> *node_internals::get_memory_indices(
    
  const char *jobid)

  {
  std::vector<int> *memory_indices = new std::vector<int>();
    
  for (unsigned int i = 0; i < this->numa_nodes.size(); i++)
    this->numa_nodes[i].get_job_indices(jobid, *memory_indices, false);

  return(memory_indices);
  }
