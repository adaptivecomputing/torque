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
#ifndef CONSTANT_H
#define CONSTANT_H

/* multipliers [bw] means either btye or word */
#define KILO  1024UL  /* number of [bw] in a kilo[bw] */
#define MEGATOKILO  1024UL  /* number of mega[bw] in a kilo[bw] */
#define GIGATOKILO  1048576UL /* number of giga[bw] in a kilo[bw] */
#define TERATOKILO  1073741824UL /* number of tera[bw] in a kilo[bw] */

/* extra constants */
#define UNSPECIFIED -1  /* unspecified value */
#define FREE_DEEP 1  /* constant to pass to free_*_list */
#define INITIALIZE -1
#define INFINITY -2  /* infinity value for resources */

/* for update_jobs_cant run */
#define START_BEFORE_JOB -1
#define START_WITH_JOB 0
#define START_AFTER_JOB 1

/* return codes for is_ok_to_run_* functions */
/* codes less then RET_BASE are indexes into the resources to check array */
#define RET_BASE 1000
#define SUCCESS (RET_BASE + 1)
#define SCHD_ERROR (RET_BASE + 2)
#define NOT_QUEUED (RET_BASE + 3)
#define QUEUE_NOT_STARTED (RET_BASE + 4)
#define QUEUE_NOT_EXEC (RET_BASE + 5)
#define QUEUE_JOB_LIMIT_REACHED (RET_BASE + 6)
#define SERVER_JOB_LIMIT_REACHED (RET_BASE + 7)
#define SERVER_USER_LIMIT_REACHED (RET_BASE + 8)
#define QUEUE_USER_LIMIT_REACHED (RET_BASE + 9)
#define SERVER_GROUP_LIMIT_REACHED (RET_BASE + 10)
#define QUEUE_GROUP_LIMIT_REACHED (RET_BASE + 11)
#define DED_TIME (RET_BASE + 12)
#define CROSS_DED_TIME_BOUNDRY (RET_BASE + 13)
#define NO_AVAILABLE_NODE (RET_BASE + 14)
#define NOT_ENOUGH_NODES_AVAIL (RET_BASE + 15)
#define JOB_STARVING (RET_BASE + 16)
#define SERVER_TOKEN_UTILIZATION (RET_BASE + 17)
#define QUEUE_IGNORED (RET_BASE + 18)
#define QUEUE_NEEDNODES (RET_BASE + 19)

/* for SORT_BY */
enum sort_type
  {
  NO_SORT,
  SHORTEST_JOB_FIRST,
  LONGEST_JOB_FIRST,
  SMALLEST_MEM_FIRST,
  LARGEST_MEM_FIRST,
  HIGH_PRIORITY_FIRST,
  LOW_PRIORITY_FIRST,
  LARGE_WALLTIME_FIRST,
  SHORT_WALLTIME_FIRST,
  FAIR_SHARE,
  MULTI_SORT
  };

#ifdef FALSE
#  undef FALSE
#endif

#ifdef TRUE
#  undef TRUE
#endif

enum { FALSE, TRUE };

enum { USER, GROUP };

enum prime_time
  {
  ALL,
  NONE,
  PRIME,
  NON_PRIME,
  HIGH_PRIME
  };

enum days
  {
  SUNDAY,
  SATURDAY,
  WEEKDAY,
  HIGH_DAY
  };

#endif
