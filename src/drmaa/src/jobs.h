/* $Id: jobs.h,v 1.6 2006/09/08 18:18:08 ciesnik Exp $ */
/*
 *  DRMAA library for Torque/PBS
 *  Copyright (C) 2006  Poznan Supercomputing and Networking Center
 *                      DSP team <dsp-devel@hedera.man.poznan.pl>
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License as published by the Free Software Foundation; either
 *  version 2.1 of the License, or (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this library; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */

/**
 * @file jobs.h
 * Remembering submitted job in session.
 */

#ifndef __DRMAA__JOBS_H
#define __DRMAA__JOBS_H

#ifdef HAVE_CONFIG_H
# include <pbs_config.h>
#endif

#include <drmaa_impl.h>

/**
 * @defgroup jobs  Remembering submitted job in session.
 * @internal
 */
/** @{ */

typedef enum
  {
  DRMAA_JOB_SUSPENDED  =  1 << 0,
  DRMAA_JOB_RESUMED    =  1 << 1,
  DRMAA_JOB_TERMINATED =  1 << 2,
  DRMAA_JOB_DISPOSE    =  1 << 3
  } job_flag_t;

/**
 * Job data stored for each submitted job.
 * It is freed when job terminates and it's status is disposed
 * by drmaa_wait() or drmaa_synchronize().
 */

struct drmaa_job_s
  {
  drmaa_job_t *next;  /**< Next job in list or @c NULL. */
  char       *jobid;  /**< Job identifier from DRM. */
  /**
   * Job submission timestamp increased in DRMAA session
   * with each submitted job.
   */
  int time_label;
  /**
   * Whether we know that job terminated and its status
   * is waiting to rip.
   */
  bool terminated;
  /**
   * Whether job was suspended within session by drmaa_control().
   */
  bool suspended;
  };

typedef struct drmaa_job_iter_s drmaa_job_iter_t;

enum { HASHTAB_SIZE = 1024 };

/** Iterates over submitted jobs set. */

struct drmaa_job_iter_s
  {
  drmaa_session_t  *c;
  unsigned          hash;  /**< Hash value of job identifier. */
  drmaa_job_t     **iter;
  };

/**
 * Returns iterator to jobs held in DRMAA session.  Caller thread should have
 * drmaa_session_s#jobs_mutex acquired iterator remains valid until job list is
 * modified (or lock is released).
 */
void
drmaa_get_job_list_iter(drmaa_session_t *session, drmaa_job_iter_t *iter);

/** Returns next job identifier from set or @c NULL if set finished. */
drmaa_job_t *
drmaa_get_next_job(drmaa_job_iter_t *iter);

/**
 * Adds job identifier to session.
 * @param c    DRMAA session.
 * @param job  Malloced drmaa_job_t structure with filled jobid field (also
 * malloced).
 */
void
drmaa_add_job(
  drmaa_session_t *c,
  drmaa_job_t *job
);

/**
 * Checks if job with given identifier exist in hash table and optionally
 * removes it.
 * @param c           Opened DRMAA session.
 * @param jobid       Job identifier.
 * @param found       If not @c NULL and job was found
 *                    job session data will be stored here.
 * @param flags       Information to store into session.
 *    If DRMAA_JOB_DISPOSE bit is set session data will be removed.
 * @see job_flag_t
 */
bool
drmaa_find_job(
  drmaa_session_t  *c,
  const char       *jobid,
  drmaa_job_t      *found,
  unsigned          flags
);

void
drmaa_delete_job_hashtab(drmaa_job_t **tab);

/** @} */

#endif /* __DRMAA_JOBS_H */

