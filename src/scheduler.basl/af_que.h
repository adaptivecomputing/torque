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
/* Feature test switches */
#ifndef _AF_QUE_H
#define _AF_QUE_H

#ifndef _POSIX_SOURCE
#define _POSIX_SOURCE 1
#endif  /* _POSIX_SOURCE */

/* System headers */

/* Local Headers */
#include "af_job.h"

/* Macros */
#define MAXQ	16		/* maximum number of queues */

/* Queue Types */
#define QTYPE_E 0		/* An execution queue */
#define QTYPE_R 1		/* A routing queue */ 
#define QTYPE_U -1		/* Unknown queue type */

/* Queue States */
#define SCHED_DISABLED 0	/* Que is enabled for job execution selection */
#define SCHED_ENABLED  1	/* Que is enabled for job execution selection */
/* File Scope Variables */
/*  NONE  */
/* External Variables */

struct que_struct {
        struct que_struct *nextptr; /* to maintain a list of ques */
        char      *name;
        int	  type;  	/* type of queue: whether execution or */
                                /* routing */
        int       numJobs;
        int       priority;     /* priority of this queue against all */
                                /* other queues */
        int       maxRunJobs;   /* maximum # of jobs allowed to be selected */
                                /* from this queue */

	int	  maxRunJobsPerUser;
	int	  maxRunJobsPerGroup;
        int	  state; 	/* can jobs from this queue be scheduled */
                                /* for execution. This is the started attrib.*/
        struct    IntRes *intResAvail;
        struct    IntRes *intResAssign;
        struct    SizeRes *sizeResAvail;
        struct    SizeRes *sizeResAssign;
        struct    StringRes *stringResAvail;
        struct    StringRes *stringResAssign;
        SetJob 	 jobs;    	/* ptr to head of the job */
};
typedef struct que_struct Que;

struct SetQue_type {
	Que	*head;
	Que	*tail;
};

typedef struct SetQue_type SetQue;

/* External Variables */
/* NONE */
/* External Functions */
/* GET functions */
extern char *
QueNameGet A_((Que *que));

extern int
QueTypeGet A_((Que *que));

extern int
QueNumJobsGet A_((Que *que));

extern int
QueMaxRunJobsGet A_((Que *que));

extern int
QueMaxRunJobsPerUserGet A_((Que *que));

extern int
QueMaxRunJobsPerGroupGet A_((Que *que));

extern int
QuePriorityGet A_((Que *que));

extern int
QueStateGet A_((Que *que));

extern struct SetJobElement *
QueJobsGet A_((Que *que));

extern int
QueIntResAvailGet A_((Que *que, char *name));

extern int
QueIntResAssignGet A_((Que *que, char *name));

extern Size
QueSizeResAvailGet A_((Que *que, char *name));

extern Size
QueSizeResAssignGet A_((Que *que, char *name));

extern char *
QueStringResAvailGet A_((Que *que, char *name));

extern char *
QueStringResAssignGet A_((Que *que, char *name));

/* Put functions */
extern void
QueNamePut A_((Que *que, char *queue_name));

extern void
QueTypePut A_((Que *que, int type));

extern void
QueNumJobsPut A_((Que *que, int numJobs));

extern void
QueMaxRunJobsPut A_((Que *que, int maxRunJobs));

extern void
QueMaxRunJobsPerUserPut A_((Que *que, int maxRunJobsPerUser));

extern void
QueMaxRunJobsPerGroupPut A_((Que *que, int maxRunJobsPerGroup));

extern void
QuePriorityPut A_((Que *que, int priority));

extern void
QueStatePut A_((Que *que, int state));

extern void
QueIntResAvailPut A_((Que *que, char *name, int value));

extern void
QueIntResAssignPut A_((Que *que, char *name, int value));

extern void
QueSizeResAvailPut A_((Que *que, char *name, Size value));

extern void
QueSizeResAssignPut A_((Que *que, char *name, Size value));

extern void
QueStringResAvailPut A_((Que *que, char *name, char *value));

extern void
QueStringResAssignPut A_((Que *que, char *name, char *value));

extern void
QueNextptrPut A_((Que *que, Que *qptr));

/* Miscellaneous functions */
extern void
QueInit A_((Que *que));

extern void
QuePrint A_((Que *que));

extern void
QueFree A_((Que *que));

extern void
QueJobInsert A_((Que *que, Job *job));

extern void
QueJobDelete A_((Que *que, Job *job));

extern Job *
QueJobFindInt (Que *que, ...);

extern Job *
QueJobFindStr (Que *que, ...);

extern Job *
QueJobFindDateTime (Que *que, ...);

extern Job *
QueJobFindSize (Que *que, ...);

extern Que *
QueFilterInt A_((Que *que, int (*func)(), Comp operator, int value));

extern Que *
QueFilterStr A_((Que *que, char *(*strfunc)(), Comp operator, char *valuestr));

extern Que *
QueFilterDateTime A_((Que *que, DateTime (*datetfunc)(), Comp operator,
							   DateTime datet));

extern Que *
QueFilterSize A_((Que *que, Size (*sizefunc)(), Comp operator, Size size));

/* Set of Ques abstraction */
extern void
SetQueInit A_((SetQue *sq));

extern void
SetQueAdd A_((SetQue *sq, Que *q));

extern void
SetQueFree A_((SetQue *sq));

extern Que *
SetQueFindQueByName A_((SetQue *sq, char *que_name));

extern void
SetQuePrint A_((SetQue *sq));

extern int
inSetQue A_((Que *que, SetQue *sq));

extern int
SetQueSortInt A_((SetQue *s, int (*key)(), int order));

extern int
SetQueSortStr A_((SetQue *s, char *(*key)(), int order));

extern int
SetQueSortDateTime A_((SetQue *s, DateTime (*key)(), int order));

extern int
SetQueSortSize A_((SetQue *s, Size (*key)(), int order));

extern int
SetQueSortFloat A_((SetQue *s, double (*key)(), int order));

#endif	/* _AF_QUE_H */
