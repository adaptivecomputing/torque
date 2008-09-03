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

#include <sys/types.h>
#include <sys/time.h>
#include <sys/times.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#include "pbs_ifl.h"
#include "log.h"

/* Scheduler header files */
#include "toolkit.h"

/*
 * POSIX/ANSI does not provide a portable method for finding the system time
 * of day at sub-second resolution.  gettimeofday() is not POSIX 1003.1
 * compliant, so it cannot be used if _POSIX_SOURCE is defined.
 *
 * POSIX 1003.1 provides the times() library call, which returns the number
 * of ticks since some fixed time.  From this number of ticks, an elapsed
 * time can be calculated to sub-second granularity.  Note that the system
 * must be asked how many ticks there are per second.
 */

#ifndef HAVE_GETTIMEOFDAY
#if (!defined(_POSIX_SOURCE) && !defined(_POSIX_C_SOURCE)) || \
 (defined(sgi) && (defined(_BSD_COMPAT) || defined(_BSD_TIME)))
#define HAVE_GETTIMEOFDAY
#endif /* ! _POSIX_SOURCE */
#endif /* ! HAVE_GETTIMEOFDAY */

void
schd_timestamp(char *msg)
  {
#ifdef DEBUG
  char *id = "schd_timestamp";
  static char  timestr[32];
  time_t sec;

  struct tms tms;
  static clock_t oticks = 0;
  clock_t ticks;
  clock_t dsec, dmsec;
  static int tickspersec = 0;

#ifdef HAVE_GETTIMEOFDAY

  struct timeval now;

  if (gettimeofday(&now, NULL))
    {
    DBPRT(("%s: gettimeofday() failed.\n", id));
    return;
    }

  sec = now.tv_sec;

#else /* HAVE_GETTIMEOFDAY */

  if (time(&sec) == (time_t)(-1))
    {
    DBPRT(("%s: gettimeofday() failed.\n", id));
    return;
    }

#endif /* else HAVE_GETTIMEOFDAY */

  strcpy(timestr, ctime(&sec));

  timestr[19] = '\0';

#ifdef HAVE_GETTIMEOFDAY
  DBPRT(("%s: %-10s %s.%03ld ", id, (msg ? msg : "TIMESTAMP"),
         &timestr[11], now.tv_usec / 1000));

#else /* HAVE_GETTIMEOFDAY */
  DBPRT(("%s: %-10s %s ", id, (msg ? msg : "TIMESTAMP"), &timestr[11]));

#endif /* else HAVE_GETTIMEOFDAY */

  /* If not already known, find the number of ticks per second. */
  if (tickspersec == 0)
    {
    tickspersec = (int)sysconf(_SC_CLK_TCK);

    if (tickspersec < 0)
      {
      DBPRT(("%s: sysconf(_SC_CLK_TCK) failed.\n", id));
      return;
      }
    }

  ticks = times(&tms);

  if (ticks == (clock_t)(-1))
    {
    DBPRT(("%s: times() failed.\n", id));
    return;
    }

  if (oticks)
    {
    dsec  = (ticks - oticks) / tickspersec;
    dmsec = (((ticks - oticks) % tickspersec) * 1000) / tickspersec;

    DBPRT(("elapsed %d.%03d\n", dsec, dmsec));
    }

  oticks = ticks;

#endif /* DEBUG */

  return;
  }

/*
 * Free a set of QueueList structures that reference (through qptr->queue)
 * an existing set of Queue structs.  The Queues themselves are not affected.
 */
int
schd_free_qlist(QueueList *qlist)
  {
  QueueList *qptr, *next;
  int   num = 0;

  for (qptr = qlist; qptr != NULL; qptr = next)
    {
    next = qptr->next;
    free(qptr);
    num++;
    }

  return (num);
  }

/*
 * This function differs from the schd_free_qlist() function in that it
 * actually frees the Queue's themselves, while schd_free_qlist() simply
 * destroys the list structures that point to the Queue's.
 */

int
schd_destroy_qlist(QueueList *list)
  {
  int num_items = 0;
  QueueList *qptr, *qnext;

  for (qptr = list; qptr != NULL; qptr = qnext)
    {
    qnext = qptr->next;

    if (qptr->queue->qname)
      free(qptr->queue->qname); /* Free strdup()'d storage. */

    if (qptr->queue->useracl)
      schd_free_useracl(qptr->queue->useracl);

    if (qptr->queue->exechost)
      free(qptr->queue->exechost);

    free(qptr->queue);   /* Free the queue struct. */

    free(qptr);    /* And the list that pointed to it. */

    num_items++;   /* Count it. */
    }

  return (num_items);
  }

/*
 * strdup(3) is not required for POSIX compliance, so we must provide a
 * "lookalike".
 */
char *
schd_strdup(char *string)
  {
  size_t  length;
  char   *copy;

  /*
   * Allocate new space for a copy of the string contents, and a
   * trailing '\0'.
   */
  length = strlen(string) + 1;

  copy = malloc(length);

  if (copy == NULL)
    return (NULL);

  memcpy(copy, string, length);

  return(copy);
  }

/*
 * Convert characters of a string to lowercase.  Return a pointer to
 * the original string.
 */
char *
schd_lowercase(char *string)
  {
  char   *p;

  for (p = string; *p != '\0'; p++)
    if (isupper(*p))
      *p = tolower(*p);

  return (string);
  }

/*
 * Return a pointer to a copy of the host part of the fqdn presented.  I.e.
 * if fqdn is "foo.bar.com", copy the host part and return a pointer to it,
 * with the contents "foo".  If it is already short, return a copy anyway.
 */
char *
schd_shorthost(char *fqdn)
  {
  char   *shorthost, *dotp;
  size_t  length;

  /* Check for bogus fqdn. */

  if (*fqdn == '.')
    return (NULL);

  if (dotp = strchr(fqdn, '.'))
    length = (size_t)(dotp - fqdn);
  else
    length = strlen(fqdn);

  if (shorthost = (char *)malloc(length + 1))
    {
    memcpy(shorthost, fqdn, length);
    shorthost[length] = '\0';
    }

  return (shorthost);
  }
