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
#ifndef _AF_JOB_H
#define _AF_JOB_H

/* Feature test switches */

#ifndef _POSIX_SOURCE
#define _POSIX_SOURCE 1
#endif  /* _POSIX_SOURCE */

/* System headers */
/* Local Headers */
#include "af.h"

/* Macros */

/* Job States */
#define TRANSIT 0
#define QUEUED  1
#define HELD    2
#define WAITING 3
#define RUNNING 4
#define EXITING 5
#define DELETED 6

/* File Scope Variables */
/*  NONE  */
/* External Variables */

struct job_struct
  {
  char            *jobId;
  char            *jobName;
  char            *ownerName;
  char            *effectiveUserName;  /* user to execute job under */
  char            *effectiveGroupName; /* group to execute job under */
  int         state;          /* job state */
  int             priority;  /* job priority as assigned by user */
  int             rerunFlag;       /* Rerunnable attribute */
  int             interactiveFlag; /* is job interactive ? */
  DateTime dateTimeCreated;
  char   *emailAddr; /* for notification of job status */
  char  *stageinFiles;
  char  *stageoutFiles;

  struct  IntRes    *intResReq;

  struct SizeRes   *sizeResReq;

  struct  StringRes *stringResReq;

  struct  IntRes    *intResUse;

  struct SizeRes   *sizeResUse;

  struct  StringRes *stringResUse;

  void  *server; /* needed in order to run a job */
  /* need to instruct the appropriate */
  /* server to run the job it owns */
  void  *queue;  /* needed in order to accumulate */
  /* certain Que resources based on */
  /* resource value for job. */
  int  refCnt;  /* # of link references to this struct*/
  /* only used to determine if this */
  /* job struct should be freed */
  };

typedef struct job_struct Job;

struct  SetJobElement
  {

  struct SetJobElement  *nextptr;

  struct SetJobElement  *first; /* pointer to the first element */
  /* in set */

  Job             *job;
  };

struct  setJob_struct
  {

  struct SetJobElement *head;

  struct SetJobElement *tail; /* non-NULL tail */
  };

typedef struct setJob_struct SetJob;

/* External Functions */
/* GET functions: ======================================================= */
extern char *
  JobIdGet A_((Job *job));

extern char *
  JobNameGet A_((Job *job));

extern char *
  JobOwnerNameGet A_((Job *job));

extern char *
  JobEffectiveUserNameGet A_((Job *job));

extern char *
  JobEffectiveGroupNameGet A_((Job *job));

extern int
  JobStateGet A_((Job *job));

extern int
  JobPriorityGet A_((Job *job));

extern int
  JobRerunFlagGet A_((Job *job));

extern int
  JobInteractiveFlagGet A_((Job *job));

extern DateTime
  JobDateTimeCreatedGet A_((Job *job));

extern char *
  JobEmailAddrGet A_((Job *job));

extern void *
  JobServerGet A_((Job *job));

extern void *
  JobQueueGet A_((Job *job));

extern int
  JobRefCntGet A_((Job *job));

extern char *
  JobStageinFilesGet A_((Job *job));

extern char *
  JobStageoutFilesGet A_((Job *job));

extern int
  JobIntResReqGet A_((Job *job, char *name));

extern Size
  JobSizeResReqGet A_((Job *job, char *name));

extern char *
  JobStringResReqGet A_((Job *job, char *name));

extern int
  JobIntResUseGet A_((Job *job, char *name));

extern Size
  JobSizeResUseGet A_((Job *job, char *name));

extern char *
  JobStringResUseGet A_((Job *job, char *name));

/* PUT functions: ======================================================= */
extern void
  JobIdPut A_((Job *job, char *jobId));

extern void
  JobNamePut A_((Job *job, char *jobName));

extern void
  JobOwnerNamePut A_((Job *job, char *ownerName));

extern void
  JobEffectiveUserNamePut A_((Job *job, char *effectiveUserName));

extern void
  JobEffectiveGroupNamePut A_((Job *job, char *groupName));

extern void
  JobStatePut A_((Job *job, int state));

extern void
  JobPriorityPut A_((Job *job, int priority));

extern void
  JobRerunFlagPut A_((Job *job, int rerunFlag));

extern void
  JobInteractiveFlagPut A_((Job *job, int interactiveFlag));

extern void
  JobDateTimeCreatedPut A_((Job *job, DateTime cdate));

extern void
  JobEmailAddrPut A_((Job *job, char *emailAddr));

extern void
  JobServerPut A_((Job *job, void *server));

extern void
  JobQueuePut A_((Job *job, void *queue));

extern void
  JobRefCntPut A_((Job *job, int refCnt));

extern void
  JobStageinFilesPut A_((Job *job, char *stagein));

extern void
  JobStageoutFilesPut A_((Job *job, char *stageout));

extern void
  JobIntResReqPut A_((Job *job, char *name, int value));

extern void
  JobSizeResReqPut A_((Job *job, char *name, Size value));

extern void
  JobStringResReqPut A_((Job *job, char *name, char *value));

extern void
  JobIntResUsePut A_((Job *job, char *name, int value));

extern void
  JobSizeResUsePut A_((Job *job, char *name, Size value));

extern void
  JobStringResUsePut A_((Job *job, char *name, char *value));

/* Set of Jobs abstractions */
extern void
  SetJobInit A_((SetJob *sjob));

extern void
  SetJobAdd A_((SetJob *sjob, Job *job));

extern int
  SetJobSortInt A_((struct SetJobElement *sje, int (*key)(), int order));

extern int
  SetJobSortStr A_((struct SetJobElement *sje, char *(*key)(), int order));

extern int
  SetJobSortDateTime A_((struct SetJobElement *sje, DateTime(*key)(), int order));

extern int
  SetJobSortSize A_((struct SetJobElement *sje, Size(*key)(), int order));

extern int
  SetJobSortFloat A_((struct SetJobElement *s, double(*key)(), int order));

extern void
  SetJobRemove A_((SetJob *sjob, Job *job));

extern void
  SetJobFree A_((SetJob *sjob));

extern void
  SetJobPrint A_((struct SetJobElement    *sje));

extern int
  inSetJob A_((Job *job, struct SetJobElement *sje));
/* MISC functions: ======================================================= */
extern void
  firstJobPtr A_((struct SetJobElement **sjeptr, struct SetJobElement *first));

extern void
  nextJobPtr A_((struct SetJobElement **sjeptr));

extern void
  JobPrint A_((Job *job));

extern void
  JobInit  A_((Job *job));

extern void
  JobFree  A_((Job *job));

extern int
  strToJobState A_((char *val));

#endif /* _AF_JOB_H */
