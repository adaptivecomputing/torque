/* $Id: wait.c,v 1.13 2006/09/05 07:49:36 ciesnik Exp $ */
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

#include <assert.h>
#include <limits.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <attrib.h>
#include <compat.h>
#include <drmaa_impl.h>
#include <jobs.h>

#ifndef lint
static char rcsid[]
# ifdef __GNUC__
__attribute__((unused))
# endif
= "$Id: wait.c,v 1.13 2006/09/05 07:49:36 ciesnik Exp $";
#endif


int
drmaa_synchronize(
  const char **job_ids, signed long timeout,
  int dispose,
  char *errmsg, size_t errlen
)
  {
  drmaa_session_t *c = NULL;
  bool wait_for_all = false;
  time_t timeout_time;

  switch (timeout)
    {

    case DRMAA_TIMEOUT_NO_WAIT:
      timeout_time = time(NULL) - 1;
      break;

    case DRMAA_TIMEOUT_WAIT_FOREVER:
      timeout_time = (time_t)INT_MAX;
      break;

    default:
      timeout_time = time(NULL) + timeout;
      break;
    }

    {
    const char **j;

    for (j = job_ids;  *j != NULL;  j++)
      if (!strcmp(*j, DRMAA_JOB_IDS_SESSION_ALL))
        wait_for_all = true;
    }

  if (wait_for_all)
    {
    int timestamp;
    int rc = DRMAA_ERRNO_SUCCESS;
    bool more_jobs = true;

    DEBUG(("-> drmaa_synchronize(DRMAA_JOB_IDS_SESSION_ALL, timeout=%ld", timeout));
    GET_DRMAA_SESSION(c);
    pthread_mutex_lock(&c->jobs_mutex);
    timestamp = c->next_time_label;
    pthread_mutex_unlock(&c->jobs_mutex);

    do
      {
      drmaa_job_iter_t iter;
      char *jobid = NULL;

      pthread_mutex_lock(&c->jobs_mutex);
      drmaa_get_job_list_iter(c, &iter);

      do
        {
        drmaa_job_t *job;
        job = drmaa_get_next_job(&iter);

        if (job == NULL)
          break;

        if (!job->terminated  &&  job->time_label < timestamp)
          jobid = strdup(job->jobid);
        }
      while (!jobid);

      pthread_mutex_unlock(&c->jobs_mutex);

      more_jobs = (jobid != NULL);

      if (jobid)
        {
        rc = drmaa_job_wait(jobid, NULL, 0, NULL, NULL, dispose,
                            timeout_time, errmsg, errlen);
        free(jobid);
        }
      }
    while (more_jobs && !rc);

    RELEASE_DRMAA_SESSION(c);

    DEBUG(("<- drmaa_synchronize =%d", rc));

    return rc;
    }
  else /* ! wait_for_all */
    {
    const char **j;
    int rc = DRMAA_ERRNO_SUCCESS;
    DEBUG(("-> drmaa_synchronize( timeout=%ld )", timeout));

    for (j = job_ids;  *j != NULL;  j++)
      {
      DEBUG(("wait for %s; timeout=%ld", *j, (long)(timeout_time - time(NULL))));
      rc = drmaa_job_wait(*j, NULL, 0, NULL, NULL, dispose, timeout_time, errmsg, errlen);

      if (rc != DRMAA_ERRNO_SUCCESS)
        break;
      }

    DEBUG(("<- drmaa_synchronize =%d", rc));

    return rc;
    }
  }


int
drmaa_wait(
  const char *job_id, char *job_id_out, size_t job_id_out_len,
  int *stat, signed long timeout, drmaa_attr_values_t **rusage,
  char *errmsg, size_t errlen
)
  {
  time_t timeout_time;
  int rc;
  DEBUG(("-> drmaa_wait(%s)", job_id));

  switch (timeout)
    {

    case DRMAA_TIMEOUT_NO_WAIT:
      timeout_time = time(NULL) - 1;
      break;

    case DRMAA_TIMEOUT_WAIT_FOREVER:
      timeout_time = (time_t)INT_MAX;
      break;

    default:
      timeout_time = time(NULL) + timeout;
      break;
    }

  if (!strcmp(job_id, DRMAA_JOB_IDS_SESSION_ANY))
    job_id = NULL;

  rc = drmaa_job_wait(
         job_id, job_id_out, job_id_out_len,
         stat, rusage, 1, timeout_time, errmsg, errlen);

  DEBUG(("<- drmaa_wait =%d", rc));

  return rc;
  }


int
drmaa_get_job_status(
  const struct batch_status *status,
  bool *terminated, int *stat,
  char *errmsg, size_t errlen
)
  {

  struct attropl *i;
  char pbs_state = 0;

  *stat = -1;

  for (i = (struct attropl*)status->attribs;  i != NULL;  i = i->next)
    {
    const drmaa_attrib_info_t *attr;
    /*DEBUG(( "attr: addr=%p, name=%s, value=%s, resource=%s",
       (void*)i, i->name, i->value, i->resource ));*/
    attr = attr_by_pbs_name(i->name);

    if (attr == NULL)
      continue;

    switch (attr->code)
      {

      case ATTR_JOB_STATE:
        pbs_state = i->value[0];
        break;

      case ATTR_EXIT_STATUS:
        *stat = atoi(i->value);
        break;
      }
    }

  DEBUG(("job %s in queue: state=%c, exit_status=%d", status->name, pbs_state, *stat));

  switch (pbs_state)
    {

    case 'C': /* Job is completed after having run. */

    case 'E': /* Job is exiting after having run. */
      *terminated = true;
      break;

    case 'H': /* Job is held. */

    case 'Q': /* Job is queued, eligible to run or routed. */

    case 'R': /* Job is running. */

    case 'T': /* Job is being moved to new location. */

    case 'W': /* Job is waiting for its execution time to be reached. */

    case 'S': /* (Unicos only) job is suspend. */
      *terminated = false;
      break;

    case 0:

    default:
      RAISE_DRMAA(DRMAA_ERRNO_INTERNAL_ERROR);
    }

  return DRMAA_ERRNO_SUCCESS;
  }


int
drmaa_get_job_rusage(
  drmaa_session_t *c,
  const char *jobid,
  drmaa_attr_values_t **rusage,
  char *errmsg, size_t errlen
)
  {
  int rc = DRMAA_ERRNO_SUCCESS;

  struct batch_status *pbs_status  = NULL;
  drmaa_attr_values_t *result = NULL;
  /*
  const char *submission_time =NULL;
  const char *start_time      =NULL;
  const char *end_time        =NULL;
  */
  const char *cpu_usage       = NULL;
  const char *mem_usage       = NULL;

  DEBUG(("-> drmaa_get_job_rusage(%s)", jobid));

  result = (drmaa_attr_values_t*)malloc(sizeof(drmaa_attr_values_t));

  if (result == NULL)
    RAISE_NO_MEMORY();

  result->list = result->iter = (char**)calloc(6, sizeof(char*));

  if (result->list == NULL)
    {
    free(result);
    RAISE_NO_MEMORY();
    }

  pthread_mutex_lock(&c->conn_mutex);

  pbs_status = pbs_statjob(c->pbs_conn, (char*)jobid, NULL, NULL);

  if (pbs_status == NULL)
    rc = drmaa_get_pbs_error(errmsg, errlen);

  pthread_mutex_unlock(&c->conn_mutex);

  if (!rc && pbs_status->attribs == NULL)
    SET_DRMAA_ERROR(rc = DRMAA_ERRNO_NO_RUSAGE);

  if (!rc)
    {

    struct attropl *i;

    for (i = (struct attropl*)pbs_status->attribs;  i != NULL;  i = i->next)
      {
      DEBUG(("name=%s, resource=%s, value=%s", i->name, i->resource, i->value));

      if (!strcmp(i->name, "resources_used"))
        {
        if (!strcmp(i->resource, "cput"))
          cpu_usage = i->value;
        else if (!strcmp(i->resource, "mem"))
          mem_usage = i->value;
        }
      }
    }

  if (!rc)
    {
    long cpu = 0, mem = 0;
    int hours, minutes, seconds, i = 0;

    if (cpu_usage && sscanf(cpu_usage, "%d:%d:%d", &hours, &minutes, &seconds) == 3)
      cpu = 60 * (60 * hours + minutes) + seconds;

    /* TODO */
    asprintf(& result->list[i++], "submission_time=%ld", (long)time(NULL));

    asprintf(& result->list[i++], "start_time=%ld", (long)time(NULL));

    asprintf(& result->list[i++], "end_time=%ld", (long)time(NULL));

    asprintf(& result->list[i++], "cpu=%ld", cpu);

    asprintf(& result->list[i++], "mem=%ld", mem);
    }

  if (pbs_status)
    pbs_statfree(pbs_status);

  if (!rc)
    *rusage = result;
  else
    drmaa_release_attr_values(result);

  DEBUG(("<- drmaa_get_job_rusage =%d", rc));

  return rc;
  }


int
drmaa_job_wait(
  const char *jobid,
  char *out_jobid, size_t out_jobid_size,
  int *stat, drmaa_attr_values_t **rusage,
  int dispose,
  time_t timeout_time,
  char *errmsg, size_t errlen
)
  {

  struct attropl  *attribs = NULL;
  drmaa_session_t *c       = NULL;
  int rc                   = DRMAA_ERRNO_SUCCESS;
  bool terminated          = false;

  DEBUG(("-> drmaa_job_wait(jobid=%s)", jobid));
  GET_DRMAA_SESSION(c);

  if (!rc)
    {

    struct attropl *l;
    l = (struct attropl*)calloc(2, sizeof(struct attropl));

    if (l != NULL)
      {
      l[0].name     = "exit_status";
      l[0].next = &l[1];
      l[1].name     = "job_state";
      l[1].next = NULL;
      attribs = l;
      }
    else
      SET_DRMAA_ERROR(rc = DRMAA_ERRNO_NO_MEMORY);
    }

  do
    {

    struct batch_status *pbs_status  = NULL;

    struct batch_status *job_status  = NULL;
    int                  exit_status = 0;

    if (!rc)
      {
      pthread_mutex_lock(&c->conn_mutex);
      DEBUG(("** probing queue: pbs_statjob( %d, %s, %p, NULL )",
             c->pbs_conn, jobid, (void*)attribs));
      pbs_status = pbs_statjob(c->pbs_conn, (char*)jobid, (struct attrl*)attribs, NULL);
      pthread_mutex_unlock(&c->conn_mutex);
      }

    if (rc)
      break;

    if (jobid != NULL)
      {
      job_status = pbs_status;

      if (!rc && job_status && strcmp(job_status->name, jobid) != 0)
        SET_DRMAA_ERROR(rc = DRMAA_ERRNO_INTERNAL_ERROR);

      if (!rc && job_status)
        rc = drmaa_get_job_status(job_status, &terminated, &exit_status, errmsg, errlen);

      if (!rc)
        {
        unsigned action = 0;

        if (terminated)
          action |= DRMAA_JOB_TERMINATED;

        if ((terminated && dispose)  ||  !job_status)
          action |= DRMAA_JOB_DISPOSE;

        if (!drmaa_find_job(c, jobid, NULL, action))
          SET_DRMAA_ERROR(rc = DRMAA_ERRNO_INVALID_JOB);
        }
      }
    else /* jobid == NULL */
      {
      bool found_any = false;

      struct batch_status *i;

      for (i = pbs_status;  !rc && i;  i = i->next)
        {
        unsigned action = 0;
        rc = drmaa_get_job_status(i, &terminated, &exit_status,
                                  errmsg, errlen);

        if (rc)  break;

        if (!terminated  &&  found_any)
          continue;  /* we do not have to check if job is in session */

        if (terminated)
          action |= DRMAA_JOB_TERMINATED;

        if (terminated && dispose)
          action |= DRMAA_JOB_DISPOSE;

        if (drmaa_find_job(c, i->name, NULL, action))
          {
          job_status = i;
          found_any  = true;
          }
        else
          job_status = NULL;

        if (job_status && terminated)
          break;
        }

      if (!rc && !found_any)
        {
#    ifdef DRMAA_DEBUG
        drmaa_check_empty_session(c);
#    endif
        SET_DRMAA_ERROR(rc = DRMAA_ERRNO_INVALID_JOB);
        }
      } /* jobid == NULL */

    if (job_status)
      DEBUG(("job %s found in session data", job_status->name));

    if (!rc && !job_status)
      SET_DRMAA_ERROR(rc = DRMAA_ERRNO_NO_RUSAGE);

    if (!rc  &&  terminated)
      {
      strlcpy(out_jobid, job_status->name, out_jobid_size);

      if (stat)
        *stat = exit_status;

      if (rusage)
        {
#if 1
        rc = drmaa_get_job_rusage(c, job_status->name, rusage, errmsg, errlen);
#else
        *rusage = (drmaa_attr_values_t*)malloc(sizeof(drmaa_attr_values_t));
        (*rusage)->list = (*rusage)->iter = (char**)calloc(6, sizeof(char*));
        asprintf(&(*rusage)->list[0], "submission_time=%ld", (long)time(NULL));
        asprintf(&(*rusage)->list[1], "start_time=%ld", (long)time(NULL));
        asprintf(&(*rusage)->list[2], "end_time=%ld", (long)time(NULL));
        asprintf(&(*rusage)->list[3], "cpu=%d", 0);
        asprintf(&(*rusage)->list[4], "mem=%d", 0);
        (*rusage)->list[5] = NULL;
#endif
        }

      /*
       * Unfortunetly following code crashes Torque 2.1.2 :/
       */
#if 0
      if (dispose)
        {
        pthread_mutex_lock(&c->conn_mutex);
        /*
         * We need to force Torque to delete completed job from queue.
         * Otherwise (for unknown reason) it complains about
         * invalid job state.
         */
        DEBUG(("pbs_deljob(%s,DELPURGE=1)", job_status->name));
        pbs_deljob(c->pbs_conn, job_status->name, DELPURGE"1");
        pthread_mutex_unlock(&c->conn_mutex);
        }

#endif
      }

    if (pbs_status != NULL)
      pbs_statfree(pbs_status);

    if (!rc  &&  !terminated)
      {
      if (time(NULL) < timeout_time)
        sleep(1);    /* pooling interval */
      else
        SET_DRMAA_ERROR(rc = DRMAA_ERRNO_EXIT_TIMEOUT);
      }
    }
  while (!(rc  ||  terminated));

  free(attribs);

  RELEASE_DRMAA_SESSION(c);

  DEBUG(("<- drmaa_job_wait =%d", rc));

  return rc;
  }



bool
drmaa_check_empty_session(drmaa_session_t *c)
  {
  drmaa_job_iter_t it;
  bool jobs_in_session = false;
  drmaa_job_t *job;

  pthread_mutex_lock(&c->jobs_mutex);
  drmaa_get_job_list_iter(c, &it);

  while ((job = drmaa_get_next_job(&it)) != NULL)
    {
    DEBUG(("!!! job in session not in DRM queue: jobid=%s submit_label=%d",
           job->jobid, job->time_label));
    jobs_in_session = true;
    }

  drmaa_delete_job_hashtab(c->job_hashtab);

  c->job_hashtab = (drmaa_job_t**)calloc(HASHTAB_SIZE, sizeof(drmaa_job_t*));
  pthread_mutex_unlock(&c->jobs_mutex);

  assert(!jobs_in_session);
  return !jobs_in_session;
  }



int
drmaa_wifexited(int *exited, int stat, char *errmsg, size_t errlen)
  {
  *exited = (stat < 128);
  return DRMAA_ERRNO_SUCCESS;
  }

int
drmaa_wexitstatus(int *exit_status, int stat, char *errmsg, size_t errlen)
  {
  *exit_status = stat & 0x7f;
  return DRMAA_ERRNO_SUCCESS;
  }

int
drmaa_wifsignaled(int *signaled, int stat, char *errmsg, size_t errlen)
  {
  *signaled = (stat >= 128);
  return DRMAA_ERRNO_SUCCESS;
  }

int
drmaa_wtermsig(char *signal, size_t signal_len, int stat, char *errmsg, size_t errlen)
  {
  int sig = stat & 0x7f; /* stat - 128 */
#define DETECT_SIG( signame ) \
case signame:  strlcpy( signal, #signame, signal_len );  break;

  switch (sig)
    {
      /* signals described by POSIX.1 */
      DETECT_SIG(SIGHUP)
      DETECT_SIG(SIGINT)
      DETECT_SIG(SIGQUIT)
      DETECT_SIG(SIGILL)
      DETECT_SIG(SIGABRT)
      DETECT_SIG(SIGFPE)
      DETECT_SIG(SIGKILL)
      DETECT_SIG(SIGSEGV)
      DETECT_SIG(SIGPIPE)
      DETECT_SIG(SIGALRM)
      DETECT_SIG(SIGTERM)
      DETECT_SIG(SIGUSR1)
      DETECT_SIG(SIGUSR2)
      DETECT_SIG(SIGCHLD)
      DETECT_SIG(SIGCONT)
      DETECT_SIG(SIGSTOP)
      DETECT_SIG(SIGTSTP)
      DETECT_SIG(SIGTTIN)
      DETECT_SIG(SIGTTOU)
      /* signals described in SUSv2 and SUSv3 / POSIX 1003.1-2001 */
#ifdef SIGBUS
      DETECT_SIG(SIGBUS)
#endif
#ifdef SIGPOLL
      DETECT_SIG(SIGPOLL)
#endif
#ifdef SIGPROF
      DETECT_SIG(SIGPROF)
#endif
#ifdef SIGSYS
      DETECT_SIG(SIGSYS)
#endif
#ifdef SIGTRAP
      DETECT_SIG(SIGTRAP)
#endif
#ifdef SIGURG
      DETECT_SIG(SIGURG)
#endif
#ifdef SIGVTALRM
      DETECT_SIG(SIGVTALRM)
#endif
#ifdef SIGXCPU
      DETECT_SIG(SIGXCPU)
#endif
#ifdef SIGXFSZ
      DETECT_SIG(SIGXFSZ)
#endif

    default:
#  if HAVE_SYS_SIGLIST
      /* strsignal() supported on some systems (Linux, Solaris) is not thread safe
       * so we use sys_siglist global array whenever possible.
       */
      strlcpy(signal, sys_siglist[sig], signal_len);
#  else
      snprintf(signal, signal_len, "unknown signal=%d", sig);
#  endif
      break;
    }

  return DRMAA_ERRNO_SUCCESS;
  }


int
drmaa_wcoredump(int *core_dumped, int stat, char *errmsg, size_t errlen)
  {
  /* We have no way to find out whether core file was created. */
  *core_dumped = 0;
  return DRMAA_ERRNO_SUCCESS;
  }


int
drmaa_wifaborted(int *aborted, int stat, char *errmsg, size_t errlen)
  {
  if (stat < 128)
    *aborted = false;
  else switch (stat & 0x7f)
      {

      case SIGTERM:

      case SIGKILL:
        *aborted = true;
        break;

      default:
        *aborted = false;
        break;
      }

  return DRMAA_ERRNO_SUCCESS;
  }

