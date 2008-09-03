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
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* PBS header files */
#include "pbs_ifl.h"
#include "log.h"

/* Toolkit header files */
#include "toolkit.h"

/*
 * Parse a string like "foo@*.bar.com,bar@qux.baz.org,fred@pox.com" into a
 * linked list of UserAcl's.  Each element's user [and possibly host] field
 * points to an individually schd_strdup()'d string.
 */
UserAcl *
schd_create_useracl(char *useracl)
  {
  char  *id = "schd_create_useracl";
  char  *useracl_copy, *user, *atsign;
  UserAcl *acl, *new, *acltail;

  /*
   * Copy the string.  This copy will be chopped up with '\0's to create
   * the strings pointed to by the array of UserAcl's pointed to by acl.
   */

  if ((useracl_copy = schd_strdup(useracl)) == NULL)
    {
    log_record(PBSEVENT_SYSTEM, PBS_EVENTCLASS_SERVER, id,
               "schd_strdup(useracl) failed");
    DBPRT(("schd_strdup(useracl) failed\n"));
    return (NULL);
    }

  acl     = NULL;

  acltail = NULL;

  user = strtok(useracl_copy, ",");

  while (user != NULL)
    {

    new = (UserAcl *)malloc(sizeof(UserAcl));

    if (new == NULL)
      {
      log_record(PBSEVENT_SYSTEM, PBS_EVENTCLASS_SERVER, id,
                 "malloc(UserAcl) failed");
      DBPRT(("malloc(UserAcl) failed\n"));

      if (acl)
        schd_free_useracl(acl);

      return (NULL);
      }

    /*
     * If a host string is given, change the '@' into a '\0' to terminate
     * the user string (for the strncpy() below) and place a reference to
     * a copy of the host string into the host pointer.
     */

    new->host = NULL;

    if ((atsign = strchr(user, '@')) != NULL)
      {
      *atsign = '\0';

      /* Skip forward to the start of the remaining host string. */
      atsign ++;

      new->host = schd_strdup(atsign);

      if (new->host == NULL)
        {
        log_record(PBSEVENT_SYSTEM, PBS_EVENTCLASS_SERVER, id,
                   "schd_strdup(host) failed");
        DBPRT(("schd_strdup(host) failed\n"));

        if (acl)
          schd_free_useracl(acl);

        free(new);

        return (NULL);
        }
      }

    /*
     * Copy the username into the static array in the UserAcl struct.
     */
    strncpy(new->user, user, PBS_MAXUSER);

    /*
     * Place the new ACL element on the tail of the list, or create it
     * if this is the first element.
     */
    if (acltail)
      acltail->next = new;
    else
      acl = new;

    acltail = new;

    acltail->next = NULL;

    /* Move on to the next user entry in the list. */
    user = strtok(NULL, ",");
    }

  /*
   * Free the storage used by the copy of the string that was strtok()'d.
   */
  free(useracl_copy);

  return (acl);
  }

int
schd_free_useracl(UserAcl *acl)
  {
  int      num = 0;
  UserAcl *aclptr, *aclnext;

  for (aclptr = acl; aclptr != NULL; aclptr = aclnext)
    {
    aclnext = aclptr->next;

    if (aclptr->host)
      free(aclptr->host);

    free(aclptr);

    num ++;
    }

  return (num);
  }

int
schd_useracl_okay(Job *job, Queue *queue, char *reason)
  {
  /* char    *id = "schd_useracl_okay"; */
  UserAcl *ptr;
  char    *jobowner;

  /* No ACL?  Don't disallow this user. */

  if (!(queue->flags & QFLAGS_USER_ACL))
    return 1;

  jobowner = job->owner;

  for (ptr = queue->useracl; ptr != NULL; ptr = ptr->next)
    {
    if (!strcmp(jobowner, ptr->user))
      {
      return (1);
      }

    /* Note: we are ignoring the host for now. */
    }

  if (reason)
    sprintf(reason, "Denied by queue %s access control list",
            queue->qname);

  return (0);
  }
