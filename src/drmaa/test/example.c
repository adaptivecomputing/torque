/* $Id: example.c,v 1.2 2006/09/04 22:03:51 ciesnik Exp $ */
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
 * @file test.c
 * DRMAA library test / usage example.
 */

#include <drmaa.h>

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <limits.h>
#include <errno.h>

#define SEND_MAIL     0
#define WITH_BULK     0
#define EXPORT_ENVS   0
#define DEBUG         0
#define N_JOBS        1

#define ERR_LEN      DRMAA_ERROR_STRING_BUFFER
#define JOB_ID_LEN   256

#define errno_error( func ) do{                      \
    fprintf( stderr, "%s:%d: %s: %s\n",               \
             __FILE__, __LINE__, func, strerror(errno) );  \
    exit( 1 );                                        \
    } while(0)

#define drmaa_error( func ) do{                      \
    fprintf( stderr, "%s:%d: %s: %s\n> %s\n",        \
             __FILE__, __LINE__, func,                    \
             drmaa_strerror(rc), err_msg );               \
    exit(1);                                         \
    }while(0)

void
test_1(void);

drmaa_job_template_t *
construct_job(char *argv[], char *fds[3], const char *cwd);

void
time_fmt(time_t t, char *buf, size_t buflen);


int
main(int argc, char *argv[])
  {
  test_1();
  return 0;
  }

void
test_1(void)
  {
  char err_msg[ ERR_LEN ] = "";
  char cwd[ PATH_MAX ];
  char job_ids[ N_JOBS ][ JOB_ID_LEN ];
#if WITH_BULK
  drmaa_job_ids_t *bulk_job_ids = NULL;
#endif
  int rc;
  int i;
  drmaa_job_template_t *jobs[ N_JOBS ];
  drmaa_job_template_t *bulk_job = NULL;

  if (getcwd(cwd, PATH_MAX) == NULL)
    errno_error("getcwd");

  printf("connecting\n");

  rc = drmaa_init(NULL, err_msg, ERR_LEN);

  if (rc)  drmaa_error("drmaa_init");

  for (i = 0;  i < N_JOBS;  i++)
    {
    char arg[ 3 ];
    char job_path[ PATH_MAX ];
    char *argv[3];
    char fds_t[3][64];
    char *fds[3];
    int j;
    printf("constructing %d\n", i);
    sprintf(arg, "%d", i);
    sprintf(job_path, "./test_job");
    argv[0] = job_path;
    argv[1] = arg;
    argv[2] = NULL;

    for (j = 0;  j < 3;  j++)
      fds[j] = fds_t[j];

    sprintf(fds[0], "$drmaa_wd_ph$/%d.stdin",  i);

    sprintf(fds[1], "$drmaa_wd_ph$/%d.stdout", i);

    sprintf(fds[2], "$drmaa_wd_ph$/%d.stderr", i);

    jobs[i] = construct_job(argv, fds, cwd);

#if 0
    char start_time_buf[ 20 ];

    time_fmt(time(NULL) + 15*i, start_time_buf, sizeof(start_time_buf));

    printf("%d: drmaa_start_time=%s\n", i, start_time_buf);

    rc = drmaa_set_attribute(jobs[i], "drmaa_start_time", start_time_buf,
                             err_msg, ERR_LEN);

    if (rc)  drmaa_error("drmaa_set_attribute(\"drmaa_set_attribute\")");

#endif
    }

  for (i = 0;  i < N_JOBS;  i++)
    {
    char job_id[ DRMAA_JOBNAME_BUFFER ];
    int stat;
    drmaa_attr_values_t *rusage;

    printf("running %d\n", i);
    rc = drmaa_run_job(job_ids[i], JOB_ID_LEN, jobs[ i ], err_msg, ERR_LEN);

    if (rc)  drmaa_error("drmaa_run_job");

    sleep(1);

    rc = drmaa_wait(job_ids[i], job_id, sizeof(job_id), &stat, DRMAA_TIMEOUT_WAIT_FOREVER, &rusage, err_msg, ERR_LEN);

    if (rc)
      drmaa_error("drmaa_job_wait");

    printf("stat=%d\n", stat);
    }

#if WITH_BULK
    {
    char *argv[] = { "./job", "bulk", "$drmaa_incr_ph$", NULL };
    char *fds[]  = { "b.$drmaa_incr_ph$.stdin", "b.$drmaa_incr_ph$.stdout",
                     "b.$drmaa_incr_ph$.stderr" };
    bulk_job = construct_job(argv, fds, WD);
    }
  rc = drmaa_run_bulk_jobs(&bulk_job_ids, bulk_job, 1, N_JOBS, 1, err_msg, ERR_LEN);

  if (rc)  drmaa_error("drmaa_run_bulk_jobs");

  if (bulk_job_ids != NULL)
    drmaa_release_job_ids(bulk_job_ids);

#endif

  for (i = 0;  i < N_JOBS;  i++)
    {
    rc = drmaa_delete_job_template(jobs[i], err_msg, ERR_LEN);

    if (rc)  drmaa_error("drmaa_delete_job_template");
    }

  if (bulk_job != NULL)
    {
    rc = drmaa_delete_job_template(bulk_job, err_msg, ERR_LEN);

    if (rc)  drmaa_error("drmaa_delete_job_template");
    }

  rc = drmaa_exit(err_msg, ERR_LEN);

  if (rc)  drmaa_error("drmaa_exit");
  }



drmaa_job_template_t *
construct_job(char *argv[], char *fds[3], const char *cwd)
  {
#if 0
  const char *scalar_attrs[] =
    {
    DRMAA_WD            , cwd,
    DRMAA_REMOTE_COMMAND, argv[0],
    DRMAA_JOB_NAME      , "jobname",
    DRMAA_INPUT_PATH    , fds[0],
    DRMAA_OUTPUT_PATH   , fds[1],
    DRMAA_ERROR_PATH    , fds[2],
    /*
    DRMAA_DURATION_SLIMIT , "00:00:30",
    DRMAA_DURATION_HLIMIT , "00:01:00",
    DRMAA_WCT_SLIMIT      , "00:00:02",
    DRMAA_WCT_HLIMIT      , "00:00:02", */
    NULL                , NULL
    };
#endif
  const char *scalar_attrs[] =
    {
    DRMAA_WD            , NULL,
    DRMAA_REMOTE_COMMAND, NULL,
    DRMAA_JOB_NAME      , NULL,
    DRMAA_INPUT_PATH    , NULL,
    DRMAA_OUTPUT_PATH   , NULL,
    DRMAA_ERROR_PATH    , NULL,
    NULL                , NULL
    };
#if EXPORT_ENVS
  const char *exported_envs[] =
    {
    "HOME", "PATH", "LD_LIBRARY_PATH",
    "USER", "USERNAME",
    "LANG", "LC_ALL", "LC_CTYPE", "LC_MESSAGES", "NLSPATH",
    "DISPLAY",
    NULL
    };
  char *env[ 32 ];
#endif

  drmaa_job_template_t *job;
  char err_msg[ ERR_LEN ] = "";
  const char *const *i;
  int rc;

  scalar_attrs[ 1] = cwd;
  scalar_attrs[ 3] = argv[0];
  scalar_attrs[ 5] = "jobname";
  scalar_attrs[ 7] = fds[0];
  scalar_attrs[ 9] = fds[1];
  scalar_attrs[11] = fds[2];

  printf("allocaing template\n");
  rc = drmaa_allocate_job_template(&job, err_msg, ERR_LEN);

  if (rc)  drmaa_error("drmaa_allocate_job_template");

    {
    for (i = scalar_attrs;  *i;)
      {
      const char *name  = *i++;
      const char *value = *i++;
      printf("set %s=%s\n", name, value);
      rc = drmaa_set_attribute(job, name, value, err_msg, ERR_LEN);

      if (rc)  drmaa_error("drmaa_set_attribute");
      }

#  if EXPORT_ENVS
    for (i = exported_envs, j = env;  *i != NULL;  i++)
      {
      const char *value = getenv(*i);

      if (value != NULL)
        {
        *j = malloc(strlen(*i) + 1 + strlen(value) + 1);
        sprintf(*j, "%s=%s", *i, value);
        printf("env: %s\n", *j);
        j++;
        }
      }

    *j = NULL;

#  endif
    }

  rc = drmaa_set_vector_attribute(job, DRMAA_V_ARGV, (const char**)argv + 1, err_msg, ERR_LEN);

  if (rc)  drmaa_error("drmaa_set_vector_attribute");

#if SEND_MAIL
  char *mail_list[] = { "lukasz.ciesnik@gmail.com", NULL };

  rc = drmaa_set_vector_attribute(job, DRMAA_V_EMAIL, (const char**)mail_list, err_msg, ERR_LEN);

  if (rc)  drmaa_error("drmaa_set_vector_attribute(DRMAA_V_EMAIL)");

#endif

#if EXPORT_ENVS
  rc = drmaa_set_vector_attribute(job, DRMAA_V_ENV, (const char**)env,  err_msg, ERR_LEN);

  if (rc)  drmaa_error("drmaa_set_vector_attribute(DRMAA_V_ENV)");

#endif

  return job;
  }


void
time_fmt(time_t t, char *buf, size_t buflen)
  {

  struct tm timem;
  gmtime_r(&t, &timem);
  strftime(buf, buflen, "%Y/%m/%d %H:%M:%S", &timem);
  }



