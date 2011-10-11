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
/* $Id$ */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "pbs_error.h"
#include "pbs_ifl.h"
#include "log.h"

#include "toolkit.h"
#include "gblxvars.h"

extern int connector;

static int schd_alterserver(int sv_conn, char *name, char *value);
int schd_alterjob(int sv_conn, Job *job, char *name, char *value,
                  char *resource);

/*
 * Record the reason that the current candidate job cannot currently run.
 * When it is decided that the job will remain queued, place the reason
 * string in the comment field of the job structure.
 */
void
schd_comment_job(Job *job, char *reason, int optional)
  {
  char   *id = "schd_comment_job";
  char   *msg_ptr;
  char   *old_msg;

  /*
   * If the 'optional' argument is true, then this comment is optional.
   * Do not bother commenting this job if this is not the first time it
   * has been seen, and it has been recently modified (hopefully it was
   * a comment change).  If there is no comment for the job, comment it
   * this time.
   */

  if (optional &&
      (!schd_FirstRun) &&
      (job->comment != NULL) &&
      !(job->flags & JFLAGS_FIRST_SEEN) &&
      (MIN_COMMENT_AGE && ((schd_TimeNow - job->mtime) < MIN_COMMENT_AGE)))
    {
    return;
    }

  if (reason == NULL)
    reason = "";

  old_msg = job->comment;

  /* If there is no old message, or they are different, set it. */
  if ((old_msg == NULL) || (strcmp(reason, old_msg) != 0))
    {

    msg_ptr = schd_strdup(reason);

    /* Alter PBS' view of the job. */
    schd_alterjob(connector, job, ATTR_comment, msg_ptr, NULL);

    /* Copy the new comment into the job field. */

    if (job->comment)
      free(job->comment);

    job->comment = msg_ptr;

    if (job->comment == NULL)
      {
      log_record(PBSEVENT_SYSTEM, PBS_EVENTCLASS_SERVER, id,
                 "schd_strdup(job->comment)");
      return;
      }
    }

  return;
  }

void
schd_comment_server(char *reason)
  {
  char   *id = "schd_comment_server";
  char   *msg_ptr;
  static char *old_msg = NULL;

  if (reason == NULL)
    msg_ptr = "";
  else
    msg_ptr = reason;

  if (old_msg == NULL)
    {
    old_msg = (char *)malloc(MAX_TXT);

    if (old_msg == NULL)
      {
      log_record(PBSEVENT_SYSTEM, PBS_EVENTCLASS_SERVER, id,
                 "malloc(old_msg)");
      return;
      }

    old_msg[0] = '\0';
    }

  if (strncmp(msg_ptr, old_msg, MAX_TXT - 1) != 0)
    {
    schd_alterserver(connector, ATTR_comment, msg_ptr);
    strncpy(old_msg, msg_ptr, MAX_TXT - 1);
    }

  return;
  }

/* Alter a job's actual attributes. */
int
schd_alterjob(int sv_conn, Job *job, char *name, char *value, char *rsrc)
  {
  char   *id = "schd_alterjob";
  int     err = 0;
  int     local_errno;
  AttrList atp;

  /* Fill the attribute structure with function parameters */
  atp.resource = rsrc;
  atp.value    = value;
  atp.name     = name;
  atp.next     = NULL;

  err = pbs_alterjob_err(sv_conn, job->jobid, &atp, NULL, &local_errno);

  if (err)
    {
    (void)sprintf(log_buffer,
                  "pbs_alterjob(%s, %s, %s, %s) failed: %d",
                  job->jobid, name, value, rsrc, local_errno);
    log_record(PBSEVENT_SYSTEM, PBS_EVENTCLASS_SERVER, id, log_buffer);
    }

  return (err);
  }

static int
schd_alterserver(int sv_conn, char *name, char *value)
  {
  char   *id = "schd_alterserver";
  int     err;
  int     local_errno;
  AttrOpList alist;

  /* Fill in the attribute struct with appropriate parameters */

  alist.resource = NULL;
  alist.value    = value;
  alist.name     = name;
  alist.next     = NULL;
  alist.op       = SET;

  err = pbs_manager_err(sv_conn, MGR_CMD_SET, MGR_OBJ_SERVER, "", &alist,
                    NULL, &local_errno);

  if (err)
    {
    (void)sprintf(log_buffer,
                  "pbs_alterserver(%s, %s) failed: %d", name, value, local_errno);
    log_record(PBSEVENT_SYSTEM, PBS_EVENTCLASS_SERVER, id, log_buffer);
    }

  return (err);
  }
