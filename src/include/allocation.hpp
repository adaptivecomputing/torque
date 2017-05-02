#ifndef ALLOCATION_HPP
#define ALLOCATION_HPP

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

#include <vector>
#include <string>

#include "pbs_ifl.h"

extern const int MEM_INDICES;
extern const int CPU_INDICES;
extern const int GPU_INDICES;
extern const int MIC_INDICES;

extern const int exclusive_node;
extern const int exclusive_socket;
extern const int exclusive_chip;
extern const int exclusive_core;
extern const int exclusive_thread;
extern const int exclusive_legacy;
extern const int exclusive_legacy2;
extern const int exclusive_none;


class cgroup_info
  {
  public:
  
    std::string mem_string;
    std::string cpu_string;
    std::string gpu_string;
    std::string mic_string;

  cgroup_info() : mem_string(), cpu_string(), gpu_string(), mic_string() {}
  };


// forward declare req
class req;

class allocation
  {
  public:
  std::vector<int> cpu_place_indices; /* These are indices that are not bound to the 
                                         cpuset but were reserved because of a 
                                         place=core=x or place=thread=x request */
  std::vector<int> cpu_indices;
  std::vector<int> mem_indices;
  std::vector<int> gpu_indices;
  std::vector<int> mic_indices;
  unsigned long    memory;
  int              cpus;
  int              cores;
  int              threads;
  int              place_cpus;
  int              place_type;
  bool             cores_only;
  std::string      jobid;
  std::string      hostname;
  int              gpus;
  int              mics;
  unsigned long    task_cput_used;
  unsigned long long task_memory_used;

  allocation(const allocation &alloc);
  allocation(const req &r);
  allocation();
  allocation(const char *jobid);
  allocation &operator =(const allocation &other);
  int  add_allocation(const allocation &other);
  void set_cput_used(const unsigned long cput_used);
  void set_memory_used(const unsigned long long mem_used);
  void place_indices_in_string(std::string &output, int which);
  void place_indices_in_string(cgroup_info &output);
  void set_place_type(const std::string &place);
  void get_place_type(int &place_type);
  void write_task_information(std::string &task_info) const;
  void initialize_from_string(const std::string &task_info);
  void set_host(const char *hostname);
  void get_stats_used(unsigned long &cput_used, unsigned long long &mem_used);
  void set_task_usage_stats(unsigned long cput_used, unsigned long long mem_used);
  void get_task_host_name(std::string &host);
  void set_gpus_remaining(int gpus);
  void get_gpus_remaining(int &gpus);
  void set_mics_remaining(int mics);
  void get_mics_remaining(int &mics);
  bool fully_placed() const;
  bool partially_placed(const req &r) const;
  void clear();
  void adjust_for_spread(unsigned int quantity, bool use_modulo);
  void adjust_for_remainder(allocation &remainder);
  };

#endif 

