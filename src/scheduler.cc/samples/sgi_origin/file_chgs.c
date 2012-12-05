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

/*
 * Routines to check if files have been changed, deleted, created, etc. since
 * the last invocation.  Used to automatically load new configuration and
 * statistics data.
 */

#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <errno.h>

#include "pbs_error.h"
#include "pbs_ifl.h"
#include "log.h"

#include "toolkit.h"

struct filestatus
  {

  struct filestatus *next;
  char    *filename;
  time_t   ctime;
  int      exists;
  };

typedef struct filestatus FileStatus;

static FileStatus *filestats = NULL;

int
schd_register_file(char *filename)
  {
  char    *id = "schd_register_file";
  FileStatus *stats, *tail, *new_fs = NULL;

  /*
   * Look for the tail of the list.  While walking the list, check to see
   * that the filename is not already registered.
   */
  tail = NULL;

  for (stats = filestats; stats != NULL; stats = stats->next)
    {
    if (strcmp(filename, stats->filename) == 0)
      {
      sprintf(log_buffer, "%s: file %s already registered.", id,
              filename);
      log_record(PBSEVENT_SYSTEM, PBS_EVENTCLASS_SERVER, id, log_buffer);
      DBPRT(("%s: %s\n", id, log_buffer));
      return (-1);
      }

    tail = stats;
    }

  /* Create space for the new record. */
  new_fs = (FileStatus *) malloc(sizeof(FileStatus));

  if (new_fs == NULL)
    {
    sprintf(log_buffer,
            "%s: out of memory allocating FileStatus for file %s",
            id, filename);
    log_record(PBSEVENT_SYSTEM, PBS_EVENTCLASS_SERVER, id, log_buffer);
    return (-1);
    }

  /* Clear the record out -- this clears the ctime and next pointer. */
  memset(new_fs, 0, sizeof(FileStatus));

  /* Keep a copy of the filename around. */
  new_fs->filename = schd_strdup(filename);

  if (new_fs->filename == NULL)
    {
    log_record(PBSEVENT_SYSTEM, PBS_EVENTCLASS_SERVER, id,
               "schd_strdup(filename)");
    return (-1);
    }

  /*
   * If this is not the first element, tack it on the end of the list.
   * Otherwise, start the list with it.
   */
  if (tail)
    tail->next = new_fs;
  else
    filestats = new_fs;

  (void)sprintf(log_buffer, "%s: file %s registered.", id, filename);

  log_record(PBSEVENT_SYSTEM, PBS_EVENTCLASS_SERVER, id, log_buffer);

  /*
   * Load the new element with the initial values for the file.  Ignore
   * the return value - only setting up the timestamp and file existence
   * status are important.
   */
  (void)schd_file_has_changed(filename, 1);

  return (0);
  }

int
schd_file_has_changed(char *filename, int reset_stamp)
  {
  char   *id = "schd_file_has_changed";

  FileStatus *stats;

  struct stat stat_buffer;
  int     exists;

  /* Assume that the file has not changed, and that it exists. */
  exists      = 1;

  if (filename == NULL)
    {
    DBPRT(("%s: filename is null\n", id));
    return (-1);
    }

  for (stats = filestats; stats != NULL; stats = stats->next)
    {
    if (strcmp(filename, stats->filename) == 0)
      break;
    }

  if (stats == NULL)
    {
    DBPRT(("%s: filename %s not registered\n", id, filename));
    return (-1);
    }

  /* Get the file modification times from the filesystem. */
  if (stat(filename, &stat_buffer) == -1)
    {
    if (errno == ENOENT)
      {
      exists = 0;
      }
    else
      {
      (void)sprintf(log_buffer,
                    "%s: stat(%s) failed: %d", id, filename, errno);
      log_record(PBSEVENT_SYSTEM, PBS_EVENTCLASS_SERVER, id, log_buffer);
      return (-1);
      }
    }

  /*
   * Has file has changed state?
   */
  if (exists != stats->exists)
    {
    stats->exists = exists;

    if (exists && reset_stamp)
      stats->ctime = stat_buffer.st_ctime;

    return (1);
    }

  /*
   * If the ctime is different from the previously recorded one, the
   * file has changed.  stat(2) indicates that ctime will be changed
   * on every write, truncate, etc.  Update the ctime value for the
   * next call.
   */
  if (exists && (stat_buffer.st_ctime != stats->ctime))
    {
    if (reset_stamp)
      stats->ctime = stat_buffer.st_ctime;

    return (1);
    }

  /*
   * Either file still does not exist, or it has not been changed since
   * the last call.
   */

  return (0);
  }

/*
 * "Forget" about the file named by 'filename', or all files if 'filename'
 * is a NULL pointer.  Returns the number of files removed from the watch
 * list, or -1 if the file is not found (or the list was empty).
 */
int
schd_forget_file(char *filename)
  {
  /* char   *id = "schd_forget_file"; */
  FileStatus *stats, *prev, *next;
  int  count, found;

  count = 0;
  found = 0;
  prev  = NULL;

  /*
   * Remove entries in the list of file stats being watched that match
   * the supplied filename, or all entries if 'filename' is NULL.
   */

  for (stats = filestats; stats != NULL; stats = next)
    {

    next = stats->next;

    if (filename && (strcmp(filename, stats->filename) != 0))
      {
      prev = stats;
      continue;
      }

    found ++;

    if (stats == filestats)
      {
      filestats = stats->next;
      }
    else
      {
      prev->next = stats->next;
      }

    /* Free the schd_strdup()'d filename */
    free(stats->filename);

    free(stats);

    count ++;
    }

  if (found)
    return (count);
  else
    return (-1);
  }
