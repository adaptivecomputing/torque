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

#ifndef CONFIG_H
#define CONFIG_H

#include "constant.h"

/* resources can get too large for a 32 bit number, so the ability to use the
 * nonstandard type long long is necessary.
 */
#define RESOURCE_TYPE long

/* the size (in bytes) of a word.  All resources are kept in kilobytes
 * internally in the server.  If any specification is in words, it will be
 * converted into kilobytes with this constant
 */
#define SIZE_OF_WORD 8

/* name of config file */
#define CONFIG_FILE "sched_config"
#define USAGE_FILE "usage"
#define HOLIDAYS_FILE "holidays"
#define RESGROUP_FILE "resource_group"
#define DEDTIME_FILE "dedicated_time"

/* parsing -
 * names that appear on the left hand side in the sched config file
 */
#define PARSE_ROUND_ROBIN "round_robin"
#define PARSE_BY_QUEUE "by_queue"
#define PARSE_STRICT_FIFO "strict_fifo"
#define PARSE_FAIR_SHARE "fair_share"
#define PARSE_HALF_LIFE "half_life"
#define PARSE_SYNC_TIME "sync_time"
#define PARSE_UNKNOWN_SHARES "unknown_shares"
#define PARSE_SORT_BY "sort_by"
#define PARSE_KEY "key"
#define PARSE_LOG_FILTER "log_filter"
#define PARSE_DEDICATED_PREFIX "dedicated_prefix"
#define PARSE_LOAD_BALENCING "load_balancing"
#define PARSE_LOAD_BALENCING_RR "load_balancing_rr"
#define PARSE_HELP_STARVING_JOBS "help_starving_jobs"
#define PARSE_MAX_STARVE "max_starve"
#define PARSE_SORT_QUEUES "sort_queues"
#define PARSE_IGNORE_QUEUE "ignore_queue"

/* max sizes */
#define MAX_HOLIDAY_SIZE 50
#define MAX_DEDTIME_SIZE 50
#define MAX_COMMENT_SIZE 100
#define MAX_LOG_SIZE 100
#define MAX_RES_NAME_SIZE 256
#define MAX_RES_RET_SIZE 256
#define MAX_IGNORED_QUEUES 16


/* messages -
 *  INFO - messages printed via info_msg
 * COMMENT - messages for comments
 */

#define INFO_QUEUE_NOT_STARTED "Queue not started"
#define INFO_QUEUE_NOT_EXEC "Queue not an execution queue"
#define INFO_QUEUE_JOB_LIMIT "Queue job limit reached"
#define INFO_SERVER_JOB_LIMIT "Server job limit reached"
#define INFO_SERVER_USER_LIMIT "Server user limit reached"
#define INFO_QUEUE_USER_LIMIT "Queue user limit reached"
#define INFO_SERVER_GROUP_LIMIT "Server group limit reached"
#define INFO_QUEUE_GROUP_LIMIT "Queue group limit reached"
#define INFO_CROSS_DED_TIME "Job would not finish before dedicated time"
#define INFO_DED_TIME   "Dedicated Time"
#define INFO_NO_AVAILABLE_NODE  "No available node to run job"
#define INFO_NOT_QUEUED  "Job is not in queued state"
#define INFO_NOT_ENOUGH_NODES_AVAIL "Not enough of the right type of nodes available"
#define INFO_JOB_STARVING "Draining system to allow %s to run"
#define INFO_SCHD_ERROR "Internal Scheduling Error"
#define INFO_TOKEN_UTILIZATION "Max token usage reached"
#define INFO_QUEUE_IGNORED "Queue is configured to be ignored"

#define COMMENT_QUEUE_NOT_STARTED "Not Running: Queue not started."
#define COMMENT_QUEUE_NOT_EXEC    "Not Running: Queue not an execution queue."
#define COMMENT_QUEUE_JOB_LIMIT "Not Running: Queue job limit has been reached."
#define COMMENT_SERVER_JOB_LIMIT "Not Running: Server job limit has been reached."
#define COMMENT_SERVER_USER_LIMIT "Not Running: User has reached server running job limit."
#define COMMENT_QUEUE_USER_LIMIT "Not Running: User has reached queue running job limit."
#define COMMENT_SERVER_GROUP_LIMIT "Not Running: Group has reached server running limit."
#define COMMENT_QUEUE_GROUP_LIMIT "Not Running: Group has reached queue running limit."
#define COMMENT_STRICT_FIFO "Not Running: Strict fifo order"
#define COMMENT_CROSS_DED_TIME "Not Running: Job would cross dedicated time boundry"
#define COMMENT_DED_TIME "Not Running: Dedicated time conflict"
#define COMMENT_NO_AVAILABLE_NODE "Not Running: All timesharing nodes are too loaded to run job"
#define COMMENT_NOT_QUEUED "Not Running: Job not in queued state"
#define COMMENT_NOT_ENOUGH_NODES_AVAIL "Not Running: Not enough of the right type of nodes are available"
#define COMMENT_JOB_STARVING "Not Running: Draining system to allow starving job to run"
#define COMMENT_TOKEN_UTILIZATION "Not Running: Max token usage reached"
#define COMMENT_SCHD_ERROR "Not Running: An internal scheduling error has occured"
#define COMMENT_QUEUE_IGNORED "Not Running: Queue is configured to be ignored"

#endif
