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
#ifndef _TM__H
#define _TM__H


/*
 * @(#) $Id$
 */

/*
** Header file defineing the datatypes and library visiable
** variables for parallel awareness.
*/

#include <sys/types.h>

typedef int  tm_host_id; /* physical node index  */
typedef int  tm_node_id; /* job-relative node id */
#define TM_ERROR_NODE ((tm_node_id)-1)

typedef int  tm_event_t; /* event handle, > 0 for real events */
#define TM_NULL_EVENT ((tm_event_t)0)
#define TM_ERROR_EVENT ((tm_event_t)-1)

typedef unsigned int tm_task_id;
#define TM_NULL_TASK (tm_task_id)0

/*
** Protocol message type defines
*/
#define TM_INIT  100 /* tm_init request */
#define TM_TASKS 101 /* tm_taskinfo request */
#define TM_SPAWN 102 /* tm_spawn request */
#define TM_SIGNAL 103 /* tm_signal request */
#define TM_OBIT  104 /* tm_obit request */
#define TM_RESOURCES 105 /* tm_rescinfo request */
#define TM_POSTINFO 106 /* tm_publish request */
#define TM_GETINFO 107 /* tm_subscribe request */
#define TM_GETTID 108 /* tm_gettasks request */
#define TM_REGISTER 109 /* tm_register request */
#define TM_RECONFIG 110 /* tm_register deferred reply */
#define TM_ACK  111 /* tm_register event acknowledge */
#define TM_FINALIZE 112 /* tm_finalize request, there is no reply */
#define TM_OKAY    0


#define TM_ERROR 999
#define TM_DONE  0

/*
** Error numbers returned from library
*/
#define TM_SUCCESS  0
#define TM_ESYSTEM  17000
#define TM_ENOEVENT  17001
#define TM_ENOTCONNECTED 17002
#define TM_EUNKNOWNCMD  17003
#define TM_ENOTIMPLEMENTED 17004
#define TM_EBADENVIRONMENT 17005
#define TM_ENOTFOUND  17006
#define TM_BADINIT  17007

#define TM_TODO_NOP 5000 /* Do nothing (the nodes value may be new) */
#define TM_TODO_CKPT 5001 /* Checkpoint <what> and continue it */
#define TM_TODO_MOVE 5002 /* Move <what> to <where> */
#define TM_TODO_QUIT 5003 /* Terminate <what> */
#define TM_TODO_STOP 5004 /* Suspend execution of <what> */

/*
** Features of this TM library
*/
#define TM_MULTIPLE_CONNS 1
#define TM_SPAWN_EXEC_ERROR 1

/*
 * DJH 27 Feb 2002. Task ids for adopted tasks start at this number
 * (local to each mom). The filenames for saved task info are the task
 * id sprintf()ed into a 10 digit number, so pick something less than
 * 9999999999.
 */
#define TM_ADOPTED_TASKID_BASE 900000000
#define IS_ADOPTED_TASK(taskid) ((taskid) >= TM_ADOPTED_TASKID_BASE)

/*
 * DJH 16 Nov 2001. tm_adopt request. Used instead of tm_init when a
 * non-PBS process wants PBS to adopt it as the nucleus of a job task
 *
 * DJH 4 March 2002. Now have two styles of task adoption.
 *   TM_ADOPT_ALTID  identifies task using an identfier provided
 *                   by the alternative task spawning system
 *                   Assumes PBS is configured to work with one
 *                   and only one alternative task spawning/
 *                   management system
 * . TM_ADOPT_JOBID  identifies task directly by PBS jobid
 */

#define TM_ADOPT_ALTID    113    /* tm_adopt request with alternative management system task id */
#define TM_ADOPT_JOBID    114     /* tm_adopt with jobid */

#endif /* _TM__H */
