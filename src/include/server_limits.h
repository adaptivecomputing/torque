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

/*
 * @(#) $Id$
 */

/*
 * This section contains size limit definitions
 *
 * BEWARE OF CHANGING THESE
 */
#ifndef SERVER_LIMITS_H
#define SERVER_LIMITS_H

/* removing outdated 14 char filename limit */
#define PBS_JOBBASE  	61	/* basename size for job file, 61 = 64 - 3  */

#define PBS_NUMJOBSTATE 6

#define PBS_MAX_HOPCOUNT 10	/* limit on number of routing hops per job */

#define PBS_SEQNUMTOP 99999999	/* top number for job sequence number, reset */
				/* to zero when reached, see req_quejob.c    */

#define TORQUE_LISTENQUEUE     10  /* used for X11 and stdout/stderr forwarding */

#define PBS_NET_RETRY_TIME     30 /* retry time between re-sending requests  */
#define PBS_NET_RETRY_LIMIT 14400 /* max retry time */
#define PBS_SCHEDULE_CYCLE    600 /* re-schedule even if no change, 10 min   */
#define PBS_RESTAT_JOB	       45 /* ask mom for status only once in 45 sec  */
#define PBS_STAGEFAIL_WAIT   1800 /* retry time after stage in failure */
#define PBS_NORMAL_PING_RATE  300 /* how often to ping the nodes */
#define PBS_TCPTIMEOUT          6 /* timeout for pbs-mom tcp connections */
#define PBS_JOBSTAT_MIN         4 /* minimum time between job stats */
#define PBS_POLLJOBS         TRUE /* enable pbs_server job polling */
#define PBS_LOG_CHECK_RATE    300 /* check log size every 5 min if 
                                     log_file_max_size is set */
/* Server Database information - path names */

#define PBS_SVR_PRIVATE		"server_priv"
#define PBS_ACCT		"accounting"
#define PBS_ARRAYDIR		"arrays"
#define PBS_JOBDIR		"jobs"
#define PBS_SPOOLDIR		"spool"
#define PBS_QUEDIR		"queues"
#define PBS_LOGFILES		"server_logs"
#define PBS_ACTFILES		"accounting"
#define PBS_SERVERDB		"serverdb"
#define PBS_SVRACL		"acl_svr"
#define PBS_TRACKING		"tracking"
#define NODE_DESCRIP		"nodes"
#define NODE_STATUS		"node_status"
#define NODE_NOTE		"node_note"


#define PBS_NET_MAX_CONNECTIONS 1024  /* increased from 256 */
#define PBS_LOCAL_CONNECTION PBS_NET_MAX_CONNECTIONS

/*
 * Security, Authentication, Authorization Control:
 *
 *	- What account is PBS mail from
 *	- Who is the default administrator (when none defined)
 *	- Is "root" always an batch adminstrator (manager) (YES/no)
 */

#define PBS_DEFAULT_MAIL	"adm"
#define PBS_DEFAULT_ADMIN	"root"
#define PBS_ROOT_ALWAYS_ADMIN	1


/* #define NO_SPOOL_OUTPUT 1	User output in home directory,not spool */

/* "simplified" network address structure */

#ifndef PBS_NET_TYPE
typedef unsigned long pbs_net_t;	/* for holding host addresses */
#define PBS_NET_TYPE
#endif	/* PBS_NET_TYPE */

/* defines for job moving (see net_move() ) */

#define MOVE_TYPE_Move  1	/* Move by user request */
#define MOVE_TYPE_Route 2	/* Route from routing queue */
#define MOVE_TYPE_Exec  3	/* Execution (move to MOM) */
#define MOVE_TYPE_MgrMv	4	/* Mover by privileged user, a manager */
#define MOVE_TYPE_Order 5	/* qorder command by user */

/*
 * server initialization modes
 */
#define RECOV_HOT    0		/* restart prior running jobs   */
#define RECOV_WARM   1		/* requeue/reschedule  all jobs */
#define RECOV_COLD   2 		/* discard all jobs		*/
#define RECOV_CREATE 4		/* discard all info		*/
#define RECOV_Invalid 5

#endif	/* SERVER_LIMITS_H */
