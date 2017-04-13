
#ifndef _REQ_HPP
#define _REQ_HPP

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

#include <string>
#include <vector>

#include "list_link.h"
#include "allocation.hpp"

extern const int USE_CORES;
extern const int USE_THREADS;
extern const int ALLOW_THREADS;
extern const int USE_FAST_CORES;

extern const int PLACE_NO_PREFERENCE;
extern const int PLACE_NODE;
extern const int PLACE_SOCKET;
extern const int PLACE_NUMA_CHIP;

extern const int ALL_EXECUTION_SLOTS;

extern const char *use_cores;
extern const char *use_threads;
extern const char *allow_threads;
extern const char *use_fast_cores;
extern const char *place_node;
extern const char *place_socket;
extern const char *place_numa_node;
extern const char *place_core;
extern const char *place_thread;
extern const char *place_legacy;
extern const char *place_legacy2;

// This class is to hold all of the information for a single req from a job
// The concept of req(s) is only available under the new syntax
class req
  {
    int               execution_slots;
    unsigned long     total_mem;
    unsigned long     mem_per_task;
    unsigned long     total_swap;
    unsigned long     swap_per_task;
    unsigned long     disk;
    int               nodes;
    int               socket;
    int               numa_nodes;
    int               cores;
    int               threads;
    int               thread_usage_policy;
    std::string       thread_usage_str;
    int               gpus;
    int               mics;
    int               maxtpn;
    std::string       gres;
    std::string       os;
    std::string       arch;
    std::string       node_access_policy;
    std::string       features;
    std::string       placement_str;
    std::string       req_attr;
    std::string       gpu_mode;
    int               placement_type;
    int               task_count;
    bool              pack;
    bool              single_job_access;
    int               per_task_cgroup;
    // these are not set by user request
    int                     index;
    std::vector<std::string>     hostlist;   // set when the job is run
    std::vector<allocation> task_allocations;

  public:
    req();
    req(char *node_val);
    req(const req &other);
    req(const std::string &resource_request);
    req &operator =(const req &other);

    void          insert_hostname(const char *hostlist_name);
    void          set_placement_type(const std::string& place_type);
    int           set_place_value(const char *value);
    int           set_value_from_string(char *str);
    int           set_attribute(const char *str);
    int           set_name_value_pair(const char *name, const char *value);
    void          set_from_string(const std::string &req_str);
    int           set_from_submission_string(char *submission_str, std::string &error);
    void          set_index(int index);
    int           set_value(const char *name, const char *value, bool is_default);
    int           set_task_value(const char *value, unsigned int task_index);
    int           submission_string_precheck(char *str, std::string &error);
    bool          submission_string_has_duplicates(char *str, std::string &error);
    bool          has_conflicting_values(std::string &error);
    int           append_gres(const char *val);
    void          get_values(std::vector<std::string> &names, std::vector<std::string> &values) const;
    void          toString(std::string &str) const;
    int           getExecutionSlots() const;
    unsigned long get_total_memory() const;
    unsigned long get_total_swap() const;
    int           get_cores() const;
    int           get_threads() const;
    int           get_sockets() const;
    int           get_numa_nodes() const;
    int           getPlaceCores() const;
    int           getPlaceThreads() const;
    unsigned long getDisk() const;
    int           getMaxtpn() const;
    int           get_gpus() const;
    int           getMics() const;
    std::string   getGpuMode() const;
    std::string   getGres() const;
    std::string   getOS() const;
    std::string   getNodeAccessPolicy() const;
    std::string   getPlacementType() const;
    std::string   getReqAttr() const;
    std::string   get_gpu_mode() const;
    int           getTaskCount() const;
    int           getIndex() const;
    int           getHostlist(std::vector<std::string> &list) const;
    std::string   getFeatures() const;
    std::string   getThreadUsageString() const;
    int           get_num_tasks_for_host(int num_ppn) const;
    int           get_num_tasks_for_host(const std::string &host) const;
    bool          is_per_task() const;
    bool          cgroup_preference_set() const;
    int           get_task_allocation(unsigned int index, allocation &task_allocation) const;
    unsigned long long get_memory_for_host(const std::string &host) const;
    unsigned long long get_memory_per_task() const;
    unsigned long long get_swap_for_host(const std::string &host) const;
    unsigned long long get_swap_per_task() const;
    void          get_task_stats(std::vector<int> &task_index, std::vector<unsigned long> &cput_used,
                                 std::vector<unsigned long long> &mem_used);
    int           get_execution_slots() const;
    void          get_task_host_name(std::string &host, unsigned int task_index);
    int           get_req_allocation_count();
    int           get_place_type();
    int           set_cput_used(int task_index, const unsigned long cput_used);
    int           set_memory_used(int task_index, const unsigned long long mem_used);
    void          set_hostlist(const char *hostlist);
    void          update_hostlist(const std::string &host_spec);
    void          set_memory(unsigned long mem);
    void          set_swap(unsigned long mem);
    void          set_execution_slots(int execution_slots);
    void          set_task_count(int task_count);
    void          record_allocation(const allocation &a);
    void          clear_allocations();
    void          set_task_usage_stats(int task_index, unsigned long cput_used, unsigned long long mem_used);
  };


int read_mem_value(const char *value, unsigned long &parsed);

#endif /* _REQ_HPP */
