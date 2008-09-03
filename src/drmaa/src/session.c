/* $Id: session.c,v 1.12 2006/09/05 13:50:00 ciesnik Exp $ */
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

#ifdef HAVE_CONFIG_H
# include <pbs_config.h>
#endif

#include <stdlib.h>
#include <string.h>
#include <compat.h>

#include <drmaa_impl.h>
#include <jobs.h>
#include <attrib.h>

#ifndef lint
static char rcsid[]
# ifdef __GNUC__
__attribute__((unused))
# endif
= "$Id: session.c,v 1.12 2006/09/05 13:50:00 ciesnik Exp $";
#endif

pthread_mutex_t drmaa_session_mutex = PTHREAD_MUTEX_INITIALIZER;
drmaa_session_t *drmaa_session = NULL;

int
drmaa_init(const char *contact, char *errmsg, size_t errlen)
  {
  int rc = DRMAA_ERRNO_SUCCESS;
  DEBUG(("-> drmaa_init(%s)", contact));
  pthread_mutex_lock(&drmaa_session_mutex);

  if (drmaa_session != NULL)
    SET_DRMAA_ERROR(rc = DRMAA_ERRNO_ALREADY_ACTIVE_SESSION);

  if (!rc)
    rc = drmaa_create(&drmaa_session, contact, errmsg, errlen);

  pthread_mutex_unlock(&drmaa_session_mutex);

  DEBUG(("<- drmaa_init =%d", rc));

  return rc;
  }


int
drmaa_exit(char *errmsg, size_t errlen)
  {
  int rc = DRMAA_ERRNO_SUCCESS;
  DEBUG(("-> drmaa_exit"));
  pthread_mutex_lock(&drmaa_session_mutex);

  if (drmaa_session == NULL)
    {
    pthread_mutex_unlock(&drmaa_session_mutex);
    RAISE_DRMAA(DRMAA_ERRNO_NO_ACTIVE_SESSION);
    }

  rc = drmaa_destroy(drmaa_session, errmsg, errlen);

  drmaa_session = NULL;
  pthread_mutex_unlock(&drmaa_session_mutex);
  DEBUG(("<- drmaa_exit =%d", rc));
  return rc;
  }


/** Creates DRMAA session and opens connection with DRM. */
int
drmaa_create(drmaa_session_t **pc, const char *contact, char *errmsg, size_t errlen)
  {
  drmaa_session_t *c;

  c = malloc(sizeof(drmaa_session_t));

  if (c == NULL)
    RAISE_NO_MEMORY();

  c->pbs_conn = -1;

  c->contact = NULL;

  c->jt_list = NULL;

  c->job_hashtab = NULL;

  c->next_time_label = 0;

  pthread_mutex_init(&c->conn_mutex, NULL);

  pthread_mutex_init(&c->jobs_mutex, NULL);

  c->jt_list = (drmaa_job_template_t*)malloc(sizeof(drmaa_job_template_t));

  if (c->jt_list == NULL)
    {
    drmaa_destroy(c, errmsg, errlen);
    RAISE_NO_MEMORY();
    }

  c->jt_list->next = c->jt_list->prev = c->jt_list;

  c->job_hashtab = (drmaa_job_t**)calloc(HASHTAB_SIZE, sizeof(drmaa_job_t*));

  if (c->job_hashtab == NULL)
    {
    drmaa_destroy(c, errmsg, errlen);
    RAISE_NO_MEMORY();
    }

  c->pbs_conn = pbs_connect((char*)contact);

  DEBUG(("pbs_connect(%s)=%d", contact, c->pbs_conn));

  if (c->pbs_conn < 0)
    {
    drmaa_destroy(c, errmsg, errlen);
    RAISE_PBS();
    }

  if (contact)
    c->contact = strdup(contact);
  else
    c->contact = strdup(pbs_server);

  if (c->contact == NULL)
    {
    drmaa_destroy(c, errmsg, errlen);
    RAISE_NO_MEMORY();
    }

  *pc = c;

  return DRMAA_ERRNO_SUCCESS;
  }


/** Closes connection with DRM (if any) and destroys DRMAA session data. */
int
drmaa_destroy(drmaa_session_t *c, char *errmsg, size_t errlen)
  {
  int rc = 0;

  if (c->pbs_conn >= 0)
    rc = pbs_disconnect(c->pbs_conn);

  free(c->contact);

  if (c->jt_list)
    {
    drmaa_job_template_t *i;

    for (i = c->jt_list->next;  i != c->jt_list;)
      {
      drmaa_job_template_t *jt = i;
      i = i->next;
      drmaa_delete_async_job_template(jt);
      }

    free(c->jt_list);
    }

  drmaa_delete_job_hashtab(c->job_hashtab);

  pthread_mutex_destroy(&c->conn_mutex);
  pthread_mutex_destroy(&c->jobs_mutex);

  free(c);

  if (rc)
    RAISE_PBS();

  return DRMAA_ERRNO_SUCCESS;
  }


int
drmaa_allocate_job_template(drmaa_job_template_t **p_jt, char *errmsg, size_t errlen)
  {
  drmaa_session_t *c = NULL;
  drmaa_job_template_t *jt = NULL;

  DEBUG(("-> drmaa_allocate_job_template"));
  GET_DRMAA_SESSION(c);
  jt = (drmaa_job_template_t*)malloc(sizeof(drmaa_job_template_t));

  if (jt == NULL)
    {
    RELEASE_DRMAA_SESSION(c);
    RAISE_NO_MEMORY();
    }

  jt->session   = c;

  jt->attrib = (void**)calloc(N_DRMAA_ATTRIBS, sizeof(void*));

  if (jt->attrib == NULL)
    {
    free(jt);
    RELEASE_DRMAA_SESSION(c);
    RAISE_NO_MEMORY();
    }

  pthread_mutex_init(&jt->mutex, NULL);

  pthread_mutex_lock(& c->jobs_mutex);
  jt->next = c->jt_list->next;
  jt->prev = c->jt_list;
  jt->next->prev = jt;
  jt->prev->next = jt;
  pthread_mutex_unlock(& c->jobs_mutex);

  *p_jt = jt;
  RELEASE_DRMAA_SESSION(c);
  DEBUG(("<- drmaa_allocate_job_template"));
  return DRMAA_ERRNO_SUCCESS;
  }


int
drmaa_delete_job_template(drmaa_job_template_t *jt, char *errmsg, size_t errlen)
  {
  drmaa_session_t *c = jt->session;

  pthread_mutex_lock(&c->jobs_mutex);
  jt->prev->next = jt->next;
  jt->next->prev = jt->prev;
  pthread_mutex_unlock(&c->jobs_mutex);

  drmaa_delete_async_job_template(jt);
  return DRMAA_ERRNO_SUCCESS;
  }


/**
 * Frees memory associated with @a jt job template
 * without locking associated session.
 */
void
drmaa_delete_async_job_template(drmaa_job_template_t *jt)
  {
  if (jt->attrib != NULL)
    {
    unsigned i;

    for (i = 0;  i < N_DRMAA_ATTRIBS;  i++)
      if (drmaa_is_vector(&drmaa_attr_table[i]))
        drmaa_free_vector(jt->attrib[i]);
      else free(jt->attrib[i]);

    free(jt->attrib);
    }

  pthread_mutex_destroy(&jt->mutex);

  free(jt);
  }


int
drmaa_set_attribute(
  drmaa_job_template_t *jt,
  const char *name, const char *value,
  char *errmsg, size_t errlen
)
  {
  const drmaa_attrib_info_t *attr;
  int attr_no;
  attr = attr_by_drmaa_name(name);

  if (attr == NULL || drmaa_is_vector(attr))
    RAISE_DRMAA(DRMAA_ERRNO_INVALID_ARGUMENT);

  attr_no = attr->code;

  pthread_mutex_lock(&jt->mutex);

  if (jt->attrib[attr_no] != NULL)
    free(jt->attrib[attr_no]);

  jt->attrib[attr_no] = strdup(value);

  pthread_mutex_unlock(&jt->mutex);

  return DRMAA_ERRNO_SUCCESS;
  }


int
drmaa_get_attribute(
  drmaa_job_template_t *jt,
  const char *name, char *value, size_t value_len,
  char *errmsg, size_t errlen
)
  {
  const drmaa_attrib_info_t *attr;
  int attr_no;
  attr = attr_by_drmaa_name(name);

  if (attr == NULL || drmaa_is_vector(attr))
    RAISE_DRMAA(DRMAA_ERRNO_INVALID_ARGUMENT);

  attr_no = attr->code;

  pthread_mutex_lock(&jt->mutex);

  if (jt->attrib[attr_no] != NULL)
    strlcpy(value, jt->attrib[attr_no], value_len);
  else
    strlcpy(value, "", value_len);

  pthread_mutex_unlock(&jt->mutex);

  return DRMAA_ERRNO_SUCCESS;
  }


int
drmaa_set_vector_attribute(
  drmaa_job_template_t *jt,
  const char *name, const char *value[],
  char *errmsg, size_t errlen
)
  {
  const drmaa_attrib_info_t *attr;
  char **v;
  int attr_no;
  unsigned i, n_values;

  attr = attr_by_drmaa_name(name);

  if (attr == NULL || !drmaa_is_vector(attr))
    RAISE_DRMAA(DRMAA_ERRNO_INVALID_ARGUMENT);

  attr_no = attr->code;

  for (n_values = 0;  value[n_values] != NULL;  n_values++)
      {}

  v = (char**)calloc(n_values + 1, sizeof(char*));

  if (v == NULL)
    RAISE_NO_MEMORY();

  for (i = 0;  i < n_values;  i++)
    {
    v[i] = strdup(value[i]);

    if (v[i] == NULL)
      {
      drmaa_free_vector(v);
      RAISE_NO_MEMORY();
      }
    }

  v[n_values] = NULL;

  pthread_mutex_lock(&jt->mutex);

  if (jt->attrib[attr_no] != NULL)
    drmaa_free_vector((char**)jt->attrib[attr_no]);

  jt->attrib[attr_no] = v;

  pthread_mutex_unlock(&jt->mutex);

  return DRMAA_ERRNO_SUCCESS;
  }


int
drmaa_get_vector_attribute(
  drmaa_job_template_t *jt,
  const char *name, drmaa_attr_values_t **out_values,
  char *errmsg, size_t errlen
)
  {
  const drmaa_attrib_info_t *attr;
  unsigned i, n_values;
  char **v, **value;

  attr = attr_by_drmaa_name(name);

  if (attr == NULL || !drmaa_is_vector(attr))
    RAISE_DRMAA(DRMAA_ERRNO_INVALID_ARGUMENT);

  pthread_mutex_lock(&jt->mutex);

  value = (char**)jt->attrib[ attr->code ];

  pthread_mutex_unlock(&jt->mutex);

  if (value == NULL)
    {
    *out_values = NULL;
    return DRMAA_ERRNO_SUCCESS;
    }

  for (n_values = 0;  value[n_values] != NULL;  n_values++)
      {}

  v = (char**)calloc(n_values + 1, sizeof(char*));

  if (v == NULL)
    RAISE_NO_MEMORY();

  for (i = 0;  i < n_values;  i++)
    {
    v[i] = strdup(value[i]);

    if (v[i] == NULL)
      {
      drmaa_free_vector(v);
      RAISE_NO_MEMORY();
      }
    }

  v[n_values] = NULL;

  *out_values = malloc(sizeof(drmaa_attr_values_t));
  (*out_values)->list = (*out_values)->iter = v;
  return DRMAA_ERRNO_SUCCESS;
  }



int
drmaa_control(const char *job_id, int action, char *errmsg, size_t errlen)
  {
  drmaa_session_t *c = NULL;
  int rc = 0;

  DEBUG(("-> drmaa_control(job_id=%s,action=%d)", job_id, action));
  GET_DRMAA_SESSION(c);
  pthread_mutex_lock(&c->conn_mutex);

  switch (action)
    {
      /*
       * We cannot know whether we did suspend job
       * in other way than remembering this inside DRMAA session.
       */

    case DRMAA_CONTROL_SUSPEND:
      drmaa_find_job(c, job_id, NULL, DRMAA_JOB_SUSPENDED);
      rc = pbs_sigjob(c->pbs_conn, (char*)job_id, "SIGSTOP", NULL);
      break;

    case DRMAA_CONTROL_RESUME:
      drmaa_find_job(c, job_id, NULL, DRMAA_JOB_RESUMED);
      rc = pbs_sigjob(c->pbs_conn, (char*)job_id, "SIGCONT", NULL);
      break;

    case DRMAA_CONTROL_HOLD:
      rc = pbs_holdjob(c->pbs_conn, (char*)job_id, USER_HOLD, NULL);
      break;

    case DRMAA_CONTROL_RELEASE:
      rc = pbs_rlsjob(c->pbs_conn, (char*)job_id, USER_HOLD, NULL);
      break;

    case DRMAA_CONTROL_TERMINATE:
      rc = pbs_deljob(c->pbs_conn, (char*)job_id, NULL); /* deldelay=N
             -- delay between SIGTERM and SIGKILL (default 0)*/
      break;
    }

  pthread_mutex_unlock(&c->conn_mutex);

  RELEASE_DRMAA_SESSION(c);
  DEBUG(("<- drmaa_control() =%d", rc));

  if (rc)
    RAISE_PBS();
  else return DRMAA_ERRNO_SUCCESS;
  }


int
drmaa_job_ps(const char *job_id, int *remote_ps, char *errmsg, size_t errlen)
  {
  drmaa_session_t *c = NULL;
  int rc = DRMAA_ERRNO_SUCCESS;
  char pbs_state = 0;
  int exit_status = -1;

  struct batch_status *status;

  struct attropl *i;

  struct attropl attribs[2];
  memset(attribs, 0, sizeof(attribs));
  attribs[0].name = "job_state";
  attribs[1].name = "exit_status";
  attribs[0].next = & attribs[1];
#if 0
    { NULL, "exit_status", NULL, NULL, 0 }
    { attribs + 1, "exit_status", NULL, NULL, 0 },
  { attribs + 2, "resources_used", "walltime", NULL, 0 },
  { attribs + 3, "resources_used", "cput",     NULL, 0 },
  { attribs + 4, "resources_used", "mem",      NULL, 0 },
  { attribs + 5, "resources_used", "vmem",     NULL, 0 },
  { attribs + 6, "Resource_List",  "walltime", NULL, 0 },
  { attribs + 7, "Resource_List",  "cput",     NULL, 0 },
  { attribs + 8, "Resource_List",  "mem",      NULL, 0 },
  { NULL,      "Resource_List",  "vmem",     NULL, 0 }
#endif

#if DRMAA_DEBUG
  DEBUG(("-> drmaa_job_ps(job_id=%s)", job_id));

  for (i = (struct attropl*)attribs;  i != NULL;  i = i->next)
    {
    DEBUG(("query attr: name=%s, value=%s, resource=%s",
           i->name, i->value, i->resource));
    }

#endif

  GET_DRMAA_SESSION(c);

  pthread_mutex_lock(&c->conn_mutex);

  status = pbs_statjob(c->pbs_conn, (char*)job_id, (struct attrl*)attribs, NULL);

  pthread_mutex_unlock(&c->conn_mutex);

  RELEASE_DRMAA_SESSION(c);

  if (status == NULL)
    {
    if (drmaa_find_job(c, job_id, NULL, DRMAA_JOB_TERMINATED))
      {
      /* assumption is made that job was removed from queue */
      *remote_ps = DRMAA_PS_FAILED;
      return DRMAA_ERRNO_SUCCESS;
      }
    else
      RAISE_PBS();
    }

#if DRMAA_DEBUG
  DEBUG(("status of %s: name=%s, text=%s", job_id, status->name, status->text));

  for (i = (struct attropl*)status->attribs;  i != NULL;  i = i->next)
    {
    DEBUG(("attr: name=%s, value=%s, resource=%s",
           i->name, i->value, i->resource));
    }

#endif

  for (i = (struct attropl*)status->attribs;  i != NULL;  i = i->next)
    {
    const drmaa_attrib_info_t *attr;
    attr = attr_by_pbs_name(i->name);

    switch (attr->code)
      {

      case ATTR_JOB_STATE:
        pbs_state = i->value[0];
        break;

      case ATTR_EXIT_STATUS:
        exit_status = atoi(i->value);
        break;
      }
    }

  if (!rc)
    switch (pbs_state)
      {

      case 'C': /* Job is completed after having run. */

        if (!drmaa_find_job(c, job_id, NULL, DRMAA_JOB_TERMINATED))
          {
          SET_DRMAA_ERROR(rc = DRMAA_ERRNO_INVALID_JOB);
          break;
          }

        /* nobreak */

      case 'E': /* Job is exiting after having run. */
        if (exit_status == 0)
          *remote_ps = DRMAA_PS_DONE;
        else
          *remote_ps = DRMAA_PS_FAILED;

        break;

      case 'H': /* Job is held. */
        *remote_ps = DRMAA_PS_USER_ON_HOLD;

        break;

      case 'Q': /* Job is queued, eligible to run or routed. */
        *remote_ps = DRMAA_PS_QUEUED_ACTIVE;

        break;

      case 'R': /* Job is running. */
        {
        drmaa_job_t job;

        if (drmaa_find_job(c, job_id, &job, 0) && job.suspended)
          *remote_ps = DRMAA_PS_USER_SUSPENDED;
        else
          *remote_ps = DRMAA_PS_RUNNING;

        break;
        }

      case 'T': /* Job is being moved to new location. */

        *remote_ps = DRMAA_PS_RUNNING;
        break;  /* ?? */

      case 'W': /* Job is waiting for its execution time to be reached. */
        *remote_ps = DRMAA_PS_USER_ON_HOLD;
        break;

      case 'S': /* (Unicos only) job is suspend. */
        *remote_ps = DRMAA_PS_SYSTEM_SUSPENDED;
        break;  /* ?? */

      case 0:

      default:
        *remote_ps = DRMAA_PS_UNDETERMINED;
        break;
      }

  pbs_statfree(status);

  DEBUG(("<- drmaa_job_ps() =%d, remote_ps=0x%02x", rc, *remote_ps));
  return rc;
  }


