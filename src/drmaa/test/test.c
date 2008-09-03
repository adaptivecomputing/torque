/* $Id: test.c,v 1.5 2006/06/08 01:27:39 ciesnik Exp $ */
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

#include <unistd.h>
#include <limits.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <errno.h>

#include <drmaa.h>

#define ERR_LEN DRMAA_ERROR_STRING_BUFFER

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

const char sleeper_job[] = "./sleeper_job";
const char exit_job[]    = "./exit_job";
const char suicide_job[] = "./suicide_job";
char working_dir[ PATH_MAX ];

void test_exit_status(int exit_value);
void signal_test(int signal);


int main(int argc, char *argv[])
  {
  char err_msg[ ERR_LEN ] = "";
  int rc = DRMAA_ERRNO_SUCCESS;

  if (getcwd(working_dir, PATH_MAX) == NULL)
    errno_error("getcwd");

  rc = drmaa_init(NULL, err_msg, ERR_LEN);

    {
    int tab[5] = { 0, 1, 3, 100, 21 };
    int i;

    for (i = 0;  i < 5;  i++)
      test_exit_status(tab[i]);
    }

  signal_test(SIGTERM);

  signal_test(SIGABRT);
  signal_test(SIGKILL);
  rc = drmaa_exit(err_msg, ERR_LEN);
  return 0;
  }



drmaa_job_template_t *
create_simple_job(char *argv[], int no)
  {
  int rc = DRMAA_ERRNO_SUCCESS;
  drmaa_job_template_t *job = NULL;
  char err_msg[ ERR_LEN ] = "";
  int i;

  rc = drmaa_allocate_job_template(&job, err_msg, ERR_LEN);

  if (rc)  drmaa_error("drmaa_allocate_job_template");

  rc = drmaa_set_attribute(job, DRMAA_WD, working_dir, err_msg, ERR_LEN);

  rc = drmaa_set_attribute(job, DRMAA_REMOTE_COMMAND, argv[0], err_msg, ERR_LEN);

  rc = drmaa_set_vector_attribute(job, DRMAA_V_ARGV, (const char**)argv + 1, err_msg, ERR_LEN);

  for (i = 0;  i < 3;   i++)
    {
    static const char *paths[3] =
      {
      "job.%d.input", "job.%d.output", "job.%d.error"
      };
    static const char *attrs[3] =
      {
      DRMAA_INPUT_PATH, DRMAA_OUTPUT_PATH, DRMAA_ERROR_PATH
      };
    char path[ PATH_MAX ];
    sprintf(path, paths[i], no);
    rc = drmaa_set_attribute(job, attrs[i], path, err_msg, ERR_LEN);
    }

  if (rc)
    drmaa_error("drmaa_set_attribute");

  return job;
  }


void
test_exit_status(int exit_value)
  {
  char err_msg[ DRMAA_ERROR_STRING_BUFFER ] = "";
  int rc = DRMAA_ERRNO_SUCCESS;
  drmaa_job_template_t *job = NULL;
  char jobid[ 128 ];
  char *argv[3];
  int stat;
  drmaa_attr_values_t *rusage;

  argv[0] = strdup(exit_job);
  argv[1] = strdup("     ");
  argv[2] = NULL;
  sprintf(argv[1], "%d", exit_value);

  job = create_simple_job(argv, 1);
  rc = drmaa_run_job(jobid, sizeof(jobid), job, err_msg, ERR_LEN);

  if (rc)  drmaa_error("drmaa_run_job");

  rc = drmaa_wait(jobid, NULL, 0, &stat, DRMAA_TIMEOUT_WAIT_FOREVER, &rusage, err_msg, ERR_LEN);

  if (rc)  drmaa_error("drmaa_wait");

  rc = drmaa_delete_job_template(job, err_msg, ERR_LEN);

  if (rc)  drmaa_error("drmaa_delete_job_template");

  free(argv[0]);

  free(argv[1]);
  }

void
signal_test(int signal)
  {
  char err_msg[ DRMAA_ERROR_STRING_BUFFER ] = "";
  int rc = DRMAA_ERRNO_SUCCESS;
  drmaa_job_template_t *job = NULL;
  char jobid[ 128 ];
  char *argv[3];
  int stat;
  drmaa_attr_values_t *rusage;

  argv[0] = strdup(suicide_job);
  argv[1] = strdup("     ");
  argv[2] = NULL;
  sprintf(argv[1], "%d", signal);

  job = create_simple_job(argv, 1);
  rc = drmaa_run_job(jobid, sizeof(jobid), job, err_msg, ERR_LEN);

  if (rc)  drmaa_error("drmaa_run_job");

  rc = drmaa_wait(jobid, NULL, 0, &stat, DRMAA_TIMEOUT_WAIT_FOREVER, &rusage, err_msg, ERR_LEN);

  if (rc)  drmaa_error("drmaa_wait");

  rc = drmaa_delete_job_template(job, err_msg, ERR_LEN);

  if (rc)  drmaa_error("drmaa_delete_job_template");

  free(argv[0]);

  free(argv[1]);
  }

