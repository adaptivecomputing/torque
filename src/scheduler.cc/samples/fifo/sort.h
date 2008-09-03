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

#ifndef SORT_H
#define SORT_H

/*
 *
 *      cmp_queue_prio_dsc - compare function used by qsort to sort queues
 *                           by decending priority
 *
 */
int cmp_queue_prio_dsc(const void *q1, const void *q2);

/*
 *
 *      cmp_queue_prio_asc - compare function used by qsort to sort queues
 *                           by ascending priority
 *
 */
int cmp_queue_prio_asc(const void *q1, const void *q2);

/*
 *      cmp_job_walltime_asc - sort jobs by requested walltime
 *                                      in ascending order.
 */
int cmp_job_walltime_asc(const void *j1, const void *j2);

/*
 *      cmp_job_walltime_dsc - sort jobs by requested walltime
 *                                      in ascending order.
 */
int cmp_job_walltime_dsc(const void *j1, const void *j2);


/*
 *
 *      cmp_job_cput_asc - compare function used by qsort to sort the jobs by
 *                         requested cput time in ascending order.
 *
 */
int cmp_job_cput_asc(const void *j1, const void *j2);

/*
 *
 *      cmp_job_cput_dsc - compare function used by qsort to sort the jobs by
 *                         requested cput time in descending order.
 *
 */
int cmp_job_cput_dsc(const void *j1, const void *j2);

/*
 *
 *      cmp_job_mem_asc -  compare function used by qsort to sort the jobs by
 *                         requested mem time in ascending order.
 *
 */
int cmp_job_mem_asc(const void *j1, const void *j2);

/*
 *
 *      cmp_job_mem_dsc -  compare function used by qsort to sort the jobs by
 *                         requested mem time in descending order.
 *
 */
int cmp_job_mem_dsc(const void *j1, const void *j2);

/*
 *
 *      cmp_job_prio_asc - compare function used by qsort to sort jobs
 *                         by ascending priority
 *
 */
int cmp_job_prio_asc(const void *j1, const void *j2);

/*
 *
 *      cmp_job_prio_dsc - compare function used by qsort to sort jobs
 *                         by descending priority
 *
 */
int cmp_job_prio_dsc(const void *j1, const void *j2);

/*
 *      cmp_fair_share - compare function for the fair share algorithm
 */
int cmp_fair_share(const void *j1, const void *j2);


/*
 *      multi_sort - a multi keyed sortint compare function
 */
int multi_sort(const void *j1, const void *j2);

/*
 *
 *      cmp_sort - entrypoint into job sort used by qsort
 */
int cmp_sort(const void *v1, const void *v2);


#endif
