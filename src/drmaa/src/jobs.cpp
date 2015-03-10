/* $Id: jobs.c,v 1.6 2006/09/05 13:50:00 ciesnik Exp $ */
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

#include <string.h>
#include <stdlib.h>

#include <jobs.h>
#include <drmaa_impl.h>
#include <lookup3.h>

#ifndef lint
static char rcsid[]
# ifdef __GNUC__
__attribute__((unused))
# endif
= "$Id: jobs.c,v 1.6 2006/09/05 13:50:00 ciesnik Exp $";
#endif


void
drmaa_delete_job_hashtab(drmaa_job_t **tab)
  {
  int i;
  drmaa_job_t *j;

  for (i = 0;  i < HASHTAB_SIZE;  i++)
    for (j = tab[i];  j != NULL;)
      {
      drmaa_job_t *d = j;
      j = j->next;
      free(d->jobid);
      free(d);
      }

  free(tab);
  }

bool
drmaa_find_job(
  drmaa_session_t  *c,
  const char       *jobid,
  drmaa_job_t      *result,
  unsigned          flags
)
  {
  drmaa_job_t **i, **found = NULL;
  unsigned h = hashstr(jobid, strlen(jobid), 0);
  h &= HASHTAB_SIZE - 1;
  pthread_mutex_lock(&c->jobs_mutex);

  for (i = &c->job_hashtab[ h ];  *i != NULL;  i = &(*i)->next)
    {
    if (!strcmp((*i)->jobid, jobid))
      {
      found = i;
      break;
      }
    }

  if (found && result)
    {
    memcpy(result, *found, sizeof(drmaa_job_t));
    result->next  = NULL;
    result->jobid = NULL;
    }

  if (found)
    {
    if (flags & DRMAA_JOB_SUSPENDED)
      (*found)->suspended = true;

    if (flags & DRMAA_JOB_RESUMED)
      (*found)->suspended = false;

    if (flags & DRMAA_JOB_TERMINATED)
      (*found)->terminated = true;

    if (flags & DRMAA_JOB_DISPOSE)
      {
      drmaa_job_t *job = *found;
      *found = (*found)->next;
      free(job->jobid);
      free(job);
      }
    }

  pthread_mutex_unlock(&c->jobs_mutex);

  return found != NULL;
  }


void
drmaa_add_job(
  drmaa_session_t *c,
  drmaa_job_t *job
)
  {
  unsigned h = hashstr(job->jobid, strlen(job->jobid), 0);
  h &= HASHTAB_SIZE - 1;
  pthread_mutex_lock(&c->jobs_mutex);
  job->time_label     = c->next_time_label ++;
  job->next           = c->job_hashtab[ h ];
  c->job_hashtab[ h ] = job;
  pthread_mutex_unlock(&c->jobs_mutex);
  }


void
drmaa_get_job_list_iter(drmaa_session_t *c, drmaa_job_iter_t *i)
  {
  i->c    = c;
  i->hash = HASHTAB_SIZE - 1;
  i->iter = & c->job_hashtab[ i->hash ];
  }


drmaa_job_t *
drmaa_get_next_job(drmaa_job_iter_t *i)
  {
  drmaa_job_t *result;

  while (*(i->iter) == NULL)
    {
    if (i->hash != 0)
      i->iter = & i->c->job_hashtab[ --i->hash ];
    else return NULL;
    }

  result = *(i->iter);

  i->iter = & (*i->iter)->next;
  return result;
  }

