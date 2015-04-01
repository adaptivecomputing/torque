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
#include "data_types.h"
#include "sort.h"
#include "job_info.h"
#include "misc.h"
#include "globals.h"
#include "fairshare.h"



/*
 * sort.c
 *
 * This file will hold the compare functions used by qsort
 * to sort the jobs
 *
 */


/*
 *
 *      cmp_queue_prio_dsc - sort queues in decending priority
 *
 */
int cmp_queue_prio_dsc(const void *q1, const void *q2)
  {
  if ((*(queue_info **) q1) -> priority < (*(queue_info **) q2) -> priority)
    return 1;
  else if ((*(queue_info **) q1) -> priority > (*(queue_info **) q2) -> priority)
    return -1;
  else
    return 0;
  }

/*
 *
 *      cmp_queue_prio_asc - sort queues by ascending priority
 *
 */
int cmp_queue_prio_asc(const void *q1, const void *q2)
  {
  if ((*(queue_info **) q1) -> priority < (*(queue_info **) q2) -> priority)
    return -1;
  else if ((*(queue_info **) q1) -> priority > (*(queue_info **) q2) -> priority)
    return 1;
  else
    return -1;
  }

/*
 *
 *      cmp_job_walltime_asc - sort jobs by requested walltime
 *     in ascending order.
 *
 */
int cmp_job_walltime_asc(const void *j1, const void *j2)
  {
  resource_req *req1, *req2;

  req1 = find_resource_req((*(job_info**) j1) -> resreq, "walltime");
  req2 = find_resource_req((*(job_info**) j2) -> resreq, "walltime");

  if (req1 != NULL && req2 != NULL)
    {
    if (req1 -> amount < req2 -> amount)
      return -1;
    else if (req1 -> amount == req2 -> amount)
      return 0;
    else
      return 1;
    }
  else
    return 0;
  }

/*
 *
 *      cmp_job_walltime_dsc - sort jobs by requested walltime
 *     in ascending order.
 *
 */
int cmp_job_walltime_dsc(const void *j1, const void *j2)
  {
  resource_req *req1, *req2;

  req1 = find_resource_req((*(job_info**) j1) -> resreq, "walltime");
  req2 = find_resource_req((*(job_info**) j2) -> resreq, "walltime");

  if (req1 != NULL && req2 != NULL)
    {
    if (req1 -> amount < req2 -> amount)
      return 1;
    else if (req1 -> amount == req2 -> amount)
      return 0;
    else
      return -1;
    }
  else
    return 0;
  }

/*
 *
 *      cmp_job_cput_asc - sort jobs by requested cput time in ascending order.
 *
 */
int cmp_job_cput_asc(const void *j1, const void *j2)
  {
  resource_req *req1, *req2;

  req1 = find_resource_req((*(job_info**) j1) -> resreq, "cput");
  req2 = find_resource_req((*(job_info**) j2) -> resreq, "cput");

  if (req1 != NULL && req2 != NULL)
    {
    if (req1 -> amount < req2 -> amount)
      return -1;
    else if (req1 -> amount == req2 -> amount)
      return 0;
    else
      return 1;
    }
  else
    return 0;
  }

/*
 *
 *      cmp_job_cput_dsc - sort jobs by requested cput time in descending order.
 *
 */
int cmp_job_cput_dsc(const void *j1, const void *j2)
  {
  resource_req *req1, *req2;

  req1 = find_resource_req((*(job_info**) j1) -> resreq, "cput");
  req2 = find_resource_req((*(job_info**) j2) -> resreq, "cput");

  if (req1 != NULL && req2 != NULL)
    {
    if (req1 -> amount < req2 -> amount)
      return 1;
    else if (req1 -> amount == req2 -> amount)
      return 0;
    else
      return -1;
    }
  else
    return 0;
  }

/*
 *
 *      cmp_job_mem_asc - sort jobs by requested mem time in ascending order.
 *
 */
int cmp_job_mem_asc(const void *j1, const void *j2)
  {
  resource_req *req1, *req2;

  req1 = find_resource_req((*(job_info**) j1) -> resreq, "mem");
  req2 = find_resource_req((*(job_info**) j2) -> resreq, "mem");

  if (req1 != NULL && req2 != NULL)
    {
    if (req1 -> amount < req2 -> amount)
      return -1;
    else if (req1 -> amount == req2 -> amount)
      return 0;
    else
      return 1;
    }
  else
    return 0;
  }

/*
 *
 *      cmp_job_mem_dsc - sort jobs by requested mem time in descending order.
 *
 */
int cmp_job_mem_dsc(const void *j1, const void *j2)
  {
  resource_req *req1, *req2;

  req1 = find_resource_req((*(job_info**) j1) -> resreq, "mem");
  req2 = find_resource_req((*(job_info**) j2) -> resreq, "mem");

  if (req1 != NULL && req2 != NULL)
    {
    if (req1 -> amount < req2 -> amount)
      return 1;
    else if (req1 -> amount == req2 -> amount)
      return 0;
    else
      return -1;
    }
  else
    return 0;
  }

/*
 *
 *      cmp_job_prio_asc - sort jobs by ascending priority
 *
 */
int cmp_job_prio_asc(const void *j1, const void *j2)
  {
  if ((*(job_info **) j1) -> priority < (*(job_info **) j2) -> priority)
    return -1;
  else if ((*(job_info **) j1) -> priority > (*(job_info **) j2) -> priority)
    return 1;
  else
    return 0;
  }

/*
 *
 *      cmp_job_prio_dsc - sort jobs by descending priority
 *
 */
int cmp_job_prio_dsc(const void *j1, const void *j2)
  {
  if ((*(job_info **) j1) -> priority < (*(job_info **) j2) -> priority)
    return 1;
  else if ((*(job_info **) j1) -> priority > (*(job_info **) j2) -> priority)
    return -1;
  else
    return 0;
  }

/*
 *
 * cmp_fair_share - compare on fair share percentage only.
 *    This is for strict priority.
 *
 */
int cmp_fair_share(const void *j1, const void *j2)
  {
  group_info *g1, *g2;

  g1 = (*(job_info **) j1) -> ginfo;
  g2 = (*(job_info **) j2) -> ginfo;

  if (g1 -> percentage > g2 -> percentage)
    return 1;
  else if (g1 -> percentage == g2 -> percentage)
    return 0;
  else
    return 1;
  }

/* multi keyed sorting
 * call compare function to sort for the first key
 * if the two keys are equal, call the compare funciton for the second key
 * repeat for all keys
 */


/*
 * multi_sort - a multi keyed sorting compare function
 */
int multi_sort(const void *j1, const void *j2)
  {
  int ret = 0;
  int i;

  for (i = 1; i <= num_sorts && ret == 0 && cstat.sort_by[i].sort != NO_SORT;i++)
    ret = cstat.sort_by[i].cmp_func(j1, j2);

  return ret;
  }

/*
 *
 * cmp_sort - entrypoint into job sort used by qsort
 *
 */
int cmp_sort(const void *v1, const void *v2)
  {
  /* sort on the internal scheduler priority (used for starvation code) */
  if ((*(job_info **) v1) -> sch_priority < (*(job_info **) v2) -> sch_priority)
    return 1;
  else if ((*(job_info **) v1)->sch_priority > (*(job_info **) v2)->sch_priority)
    return -1;
  else
    return cstat.sort_by -> cmp_func(v1, v2);
  }
