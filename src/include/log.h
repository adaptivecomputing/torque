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
 * include file for error/event logging
 */

#define LOG_BUF_SIZE		4096

/* The following macro assist in sharing code between the Server and Mom */
#define LOG_EVENT log_event

/*
** Set up a debug print macro.
*/
#ifdef	NDEBUG
#define DBPRT(x)
#else
#define	DBPRT(x)	printf x;
#endif  /* END NDEBUG */

extern long *log_event_mask;

extern void log_close A_((int));
extern void log_err A_((int,char *,char *));
extern void log_event A_((int,int,char *,char *));
extern int  log_open A_((char *,char *));
extern void log_record A_((int,int,char *,char *));
extern char log_buffer[LOG_BUF_SIZE];

extern int  chk_file_sec A_((char *,int,int,int,int));
extern int  chk_path_sec A_((char *,int,int,int,int));
extern int  setup_env A_((char *));


/* Event types */

#define PBSEVENT_ERROR		0x0001		/* internal errors	      */
#define PBSEVENT_SYSTEM		0x0002		/* system (server) events     */
#define PBSEVENT_ADMIN		0x0004		/* admin events		      */
#define PBSEVENT_JOB		0x0008		/* job related events	      */
#define PBSEVENT_JOB_USAGE	0x0010		/* End of Job accounting      */
#define PBSEVENT_SECURITY	0x0020		/* security violation events  */
#define PBSEVENT_SCHED		0x0040		/* scheduler events	      */
#define PBSEVENT_DEBUG		0x0080		/* common debug messages      */
#define PBSEVENT_DEBUG2		0x0100		/* less needed debug messages */
#define PBSEVENT_FORCE		0x8000		/* set to force a messag      */

/* Event Object Classes, see array class_names[] in ../lib/Liblog/pbs_log.c   */

#define PBS_EVENTCLASS_SERVER	1	/* The server itself	*/
#define PBS_EVENTCLASS_QUEUE	2	/* Queues		*/
#define PBS_EVENTCLASS_JOB	3	/* Jobs			*/
#define PBS_EVENTCLASS_REQUEST	4	/* Batch Requests	*/
#define PBS_EVENTCLASS_FILE	5	/* A Job related File	*/
#define PBS_EVENTCLASS_ACCT	6	/* Accounting info	*/
#define PBS_EVENTCLASS_NODE	7	/* Nodes          	*/

/* Logging Masks */

#define PBSEVENT_MASK 	0x01ff
