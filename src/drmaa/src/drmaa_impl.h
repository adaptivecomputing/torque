/* $Id: drmaa_impl.h,v 1.9 2006/09/08 18:18:08 ciesnik Exp $ */
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

#ifndef __DRMAA_IMPL_H
#define __DRMAA_IMPL_H

#ifdef HAVE_CONFIG_H
# include <pbs_config.h>
#endif

#include <sys/types.h>

#ifdef HAVE_STDBOOL_H
#include <stdbool.h>
#endif

#include <time.h>

#include <pthread.h>
#include "pbs_ifl.h"

#include <drmaa.h>
#include <error.h>
#include <compat.h>

typedef struct drmaa_session_s drmaa_session_t;

typedef struct drmaa_job_s drmaa_job_t;

/** Mutex for accessing @ref drmaa_session global variable. */
extern pthread_mutex_t drmaa_session_mutex;
extern drmaa_session_t *drmaa_session;


/** DRMAA session data. */

struct drmaa_session_s
  {
  int                   pbs_conn;    /**< PBS connection (or -1). */
  char                 *contact;     /**< Contact to PBS server -- `host[:port]'. */
  drmaa_job_template_t *jt_list;     /**< Cyclic list (with sentinel) of job
                        templates created in this DRMAA session. */
  drmaa_job_t         **job_hashtab; /**< Hash table of jobs
                       which have to be remembered in DRMAA
                     session (was submitted in this
                     session and its status was not removed). */
  int                   next_time_label;  /**< Will be assigned to next submitted job. */
  pthread_mutex_t       conn_mutex;  /**< Mutex for PBS connection. */
  pthread_mutex_t       jobs_mutex;  /**< Mutex for #jt_list, #job_list
                       and #next_time_label. */
  };

/** Job template data. */

struct drmaa_job_template_s
  {
  drmaa_session_t      *session; /**< DRMAA session in which job template was created. */
  drmaa_job_template_t *prev;    /**< Previous job template in list. */
  drmaa_job_template_t *next;    /**< Next job template in list. */
  void                **attrib;  /**< Table of DRMAA attributes.
  It is filled with N_DRMAA_ATTRIBS values which are either @c NULL
  (attribute not set) or string (scalar attribute) or @c NULL terminated
  array of strings (vector attribute). */
  pthread_mutex_t       mutex;   /**< Mutex for accessing job attributes. */
  };


struct drmaa_attr_names_s
  {
  char **list, **iter;
  };

struct drmaa_attr_values_s
  {
  char **list, **iter;
  };

struct drmaa_job_ids_s
  {
  char **list, **iter;
  };


typedef struct drmaa_submission_context_s drmaa_submission_context_t;

enum
  {
  DRMAA_PLACEHOLDER_MASK_HD    = 1 << 0,
  DRMAA_PLACEHOLDER_MASK_WD    = 1 << 1,
  DRMAA_PLACEHOLDER_MASK_INCR  = 1 << 2
  };

struct drmaa_submission_context_s
  {
  const drmaa_job_template_t *jt;

  struct attropl *pbs_attribs;
  char *script_filename;
  char *home_directory;
  char *working_directory;
  char *bulk_incr_no;
  };


/** @defgroup drmaa_session  Session managing function.
 @{ */
int
drmaa_create(drmaa_session_t **pc, const char *contact, char *errmsg, size_t errlen);

int
drmaa_destroy(drmaa_session_t *c, char *errmsg, size_t errlen);
/** @} */

/** @addtogroup drmaa_jobt */
void
drmaa_delete_async_job_template(drmaa_job_template_t *jt);



int
drmaa_impl_get_attribute_names(
  drmaa_attr_names_t **values,
  unsigned f_mask, unsigned flags,
  char *errmsg, size_t errlen
);

int
drmaa_run_job_impl(
  char *job_id, size_t job_id_len,
  const drmaa_job_template_t *jt, int bulk_no,
  char *errmsg, size_t errlen
);

int
drmaa_job_wait(
  const char *jobid, char *out_jobid, size_t out_jobid_size,
  int *stat, drmaa_attr_values_t **rusage,
  int dispose, time_t timeout_time,
  char *errmsg, size_t errlen
);

/**
 * Check if session is empty (there are no jobs
 * which was submitted and termination information is left).
 * @return @c true  if session is empty.
 *   Otherwise clears the session and returns @c false.
 */
bool
drmaa_check_empty_session(drmaa_session_t *c);



int
drmaa_create_submission_context(
  drmaa_submission_context_t **c,
  const drmaa_job_template_t *jt, int bulk_no,
  char *errmsg, size_t errlen);

void
drmaa_free_submission_context(drmaa_submission_context_t *c);

int
drmaa_set_job_std_attribs(
  drmaa_submission_context_t *c,
  char *errmsg, size_t errlen
);

int
drmaa_create_job_script(
  drmaa_submission_context_t *c,
  char *errmsg, size_t errlen
);

int
drmaa_set_job_files(
  drmaa_submission_context_t *c,
  char *errmsg, size_t errlen
);

int
drmaa_set_file_staging(
  drmaa_submission_context_t *c,
  char *errmsg, size_t errlen
);

int
drmaa_set_job_environment(
  drmaa_submission_context_t *c,
  char *errmsg, size_t errlen
);

char *
drmaa_translate_staging(const char *stage);

int
drmaa_set_job_email_notication(
  drmaa_submission_context_t *c,
  char *errmsg, size_t errlen
);

int
drmaa_set_job_submit_state(
  drmaa_submission_context_t *c,
  char *errmsg, size_t errlen
);

int
drmaa_add_pbs_attr(
  drmaa_submission_context_t *c,
  int attr, char *value,
  unsigned set,
  char *errmsg, size_t errlen
);

int
drmaa_write_tmpfile(
  char **filename, const char *content, size_t len,
  char *errmsg, size_t errlen
);

char *
drmaa_explode(const char **vector, char glue);

void
drmaa_free_vector(char **vector);

void
drmaa_free_attropl(struct attropl *attr);

char *
drmaa_expand_placeholders(
  drmaa_submission_context_t *c, char *input, unsigned set);

char *
drmaa_replace(char *input, const char *placeholder, const char *value);


#define GET_DRMAA_SESSION( session ) do{            \
    pthread_mutex_lock( &drmaa_session_mutex );       \
    if( drmaa_session == NULL )                       \
      {                                                \
      pthread_mutex_unlock( &drmaa_session_mutex );   \
      RAISE_DRMAA( DRMAA_ERRNO_NO_ACTIVE_SESSION );   \
      }                                                \
    session = drmaa_session;                          \
    pthread_mutex_unlock( &drmaa_session_mutex );     \
    }while(0)

#define RELEASE_DRMAA_SESSION( session ) /* nothing */


#endif /* __DRMAA_IMPL_H */

