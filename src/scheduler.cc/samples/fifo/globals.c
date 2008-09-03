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

#include <stdio.h>
#include "globals.h"
#include "constant.h"
#include "sort.h"

/*
 * res_to_check[] - a list of resources which controls what resources
 *    are checked by the scheduler.
 *
 * Format: resource_name comment_message debug_message
 *
 * resource_name: name the server knows the resource as
 * comment_message: what the comment should be set to if there is not
 *    enough of this resource available
 * debug_message: what should be logged as the debug message if there
 *         is not enough of this resource available
 *
 */

const struct rescheck res_to_check[] =
  {
    {"mem",     "Not Running: Not enough memory available", "Not enough memory available"
    },

  { "ncpus",  "Not Running: Not enough cpus available", "Not enough cpus available"},
  { "nodect", "Not Running: Not enough nodes available", "Not enough nodes available"}
  };

/*
 *
 * sorting_info[] - holds information about all the different ways you
 *    can sort the jobs
 *
 * Format: { sort_type, config_name, cmp_func_ptr }
 *
 *   sort_type    : an element from the enum sort_type
 *   config_name  : the name which appears in the scheduling policy config
 *           file (sched_config)
 *   cmp_func_ptr : function pointer the qsort compare function
 *    (located in sort.c)
 *
 */

const struct sort_info sorting_info[] =
  {
    {
    NO_SORT, "no_sort", NULL
    },

  {SHORTEST_JOB_FIRST, "shortest_job_first", cmp_job_cput_asc},
  {LONGEST_JOB_FIRST, "longest_job_first", cmp_job_cput_dsc},
  {SMALLEST_MEM_FIRST, "smallest_memory_first", cmp_job_mem_asc},
  {LARGEST_MEM_FIRST, "largest_memory_first", cmp_job_mem_dsc},
  {HIGH_PRIORITY_FIRST, "high_priority_first", cmp_job_prio_dsc},
  {LOW_PRIORITY_FIRST, "low_priority_first", cmp_job_prio_asc},
  {LARGE_WALLTIME_FIRST, "large_walltime_first", cmp_job_walltime_dsc},
  {SHORT_WALLTIME_FIRST, "short_walltime_first", cmp_job_walltime_asc},
  {FAIR_SHARE, "fair_share", cmp_fair_share},
  {MULTI_SORT, "multi_sort", multi_sort}
  };

/*
 * res_to_get - resources to get from each nodes mom
 */
const char *res_to_get[] =
  {
  "ncpus",  /* number of CPUS */
  "arch",  /* the architecture of the machine */
  "physmem",  /* the amount of physical memory */
  "loadave",  /* the current load average */
  "max_load",  /* static max_load value */
  "ideal_load"  /* static ideal_load value */
  };

/* number of indicies in the res_to_check array */
const int num_res = sizeof(res_to_check) / sizeof(struct rescheck);

/* number of indicies in the sorting_info array */
const int num_sorts = sizeof(sorting_info) / sizeof(struct sort_info);

/* number of indicies in the res_to_get array */
const int num_resget = sizeof(res_to_get) / sizeof(char *);


struct config conf;

struct status cstat;
