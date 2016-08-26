
#ifndef _COMPLETE_REQ_HPP
#define _COMPLETE_REQ_HPP

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

#include "req.hpp"

#define _MEM_   0
#define _PMEM_  1
#define _VMEM_  2
#define _PVMEM_ 3


class complete_req
  {
    std::vector<req> reqs;

  public:

    complete_req();
    complete_req(tlist_head &at_list, bool legacy_vmem);
    complete_req(const complete_req &other);
    complete_req &operator =(const complete_req &other);

    void           add_req(req &r);
    void           set_from_string(const std::string &obj_string);
    int            set_value(int index, const char *name, const char *value, bool is_default);
    int            set_task_value(const char *name, const char *value);
    void           toString(std::string &output) const;
    int            req_count() const;
    void           get_values(std::vector<std::string> &names, std::vector<std::string> &values) const;
    unsigned long long get_memory_for_this_host(const std::string &hostname) const;
    unsigned long long get_memory_per_task(unsigned int req_index);
    unsigned long long get_swap_memory_for_this_host(const std::string &hostname) const;
    unsigned long long get_swap_per_task(unsigned int req_index);
    req           &get_req(int i);
    int            get_req_index_for_host(const char *host, unsigned int &req_index);
    int            get_req_and_task_index(const int rank, unsigned int &req_index, unsigned int &task_index);
    int            get_req_and_task_index_from_local_rank(int rank, unsigned int &req_index, unsigned int &task_index, const char *host) const;
    int            update_hostlist(const std::string &host_spec, int req_index);
    void           set_hostlists(const char *job_id, const char *host_list);
    void           set_value_from_nodes(const char *node_val, int &task_count);
    int            set_task_memory_used(int req_index, int task_index, const unsigned long long mem_used);
    int            set_task_cput_used(int req_index, int task_index, const unsigned long cput_used);
    void           clear_allocations();
    void           clear_reqs();
    int            get_task_stats(unsigned int &req_index, std::vector<int> &task_index,
                                  std::vector<unsigned long> &cput_used,
                                  std::vector<unsigned long long> &mem_used, const char *hostname);
    void           set_task_usage_stats(int req_index, int task_index, unsigned long cput_used, unsigned long long mem_used);
    unsigned int   get_num_reqs();
  };

#endif


