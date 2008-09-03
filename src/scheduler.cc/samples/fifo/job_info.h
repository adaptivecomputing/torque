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

#ifndef JOB_INFO_H
#define JOB_INFO_H

#include "pbs_ifl.h"
#include "data_types.h"

/*
 *      query_jobs - create an array of jobs in a specified queue
 */
job_info **query_jobs(int pbs_sd, queue_info *qinfo);

/*
 * query_job_info - takes info from a batch_status about a job and puts
 */
job_info *query_job_info(struct batch_status *job, queue_info *qinfo);

/*
 * new_job_info  - allocate and initialize new job_info structure
 */
job_info *new_job_info();

/*
 * find_resource_req - find a resource_req from a resource_req list
 */
resource_req *find_resource_req(resource_req *reqlist, const char *name);

/*
 *      new_resource_req - allocate and initalize new resoruce_req
 */
resource_req *new_resource_req();

/*
 *  find_alloc_resource_req - find resource_req by name or allocate
 *      and initalize a new resource_req
 *      also adds new one to the list
 */
resource_req *find_alloc_resource_req(char *name, resource_req *reqlist);

/*
 * free_job_info - free all the memory used by a job_info structure
 */

void free_job_info(job_info *jinfo);

/*
 * free_jobs - free an array of jobs
 */

void free_jobs(job_info **jarr);

/*
 *      print_job_info - print out a job_info struct
 */

void print_job_info(job_info *jinfo, char brief);

/*
 *      set_state - set the state flag in a job_info structure
 *                      i.e. the is_* bit
 */
void set_state(char *state, job_info *jinfo);

/*
 *      update_job_on_run - update job information kept in a job_info
 *                              when a job is run
 */
void update_job_on_run(int pbs_sd, job_info *jinfo);

/*
 *      update_job_comment - update a jobs comment attribute
 */
int update_job_comment(int pbs_sd, job_info *jinfo, char *comment);

/*
 *      update_jobs_cant_run - update an array of jobs which can not run
 */
void update_jobs_cant_run(int pbs_sd, job_info **jinfo_arr, job_info *start,
                          char *comment, int start_where);

/*
 * job_filter - filters jobs on specified argument
 * NOTE: this function allocates a new array
 * filter_func prototype: int func( job_info *, void * )
 */
job_info **job_filter(job_info** jobs, int size,
                      int (*filter_func)(job_info*, void*), void *arg);

/*
 *      free_resource_req_list - frees memory used by a resource_req list
 */
void free_resource_req_list(resource_req *list);

/*
 *      translate_job_fail_code - translate the failure code of
 *                              is_ok_to_run_job into a comment and log message
 */
int translate_job_fail_code(int fail_code, char *comment_msg, char *log_msg);

/*
 *      calc_assn_resource - calcualte the assigned resource in a job array
 */
int calc_assn_resource(job_info **jinfo_arr, char *resstr);

#endif
