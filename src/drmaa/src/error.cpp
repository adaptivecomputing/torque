/* $Id: error.c,v 1.9 2006/09/08 18:18:08 ciesnik Exp $ */
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

#include <unistd.h>

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <errno.h>

#include <pthread.h>
#include "pbs_error.h"

#include <drmaa.h>
#include <error.h>
#include <drmaa_impl.h>

#ifndef lint
static char rcsid[]
# ifdef __GNUC__
__attribute__((unused))
# endif
= "$Id: error.c,v 1.9 2006/09/08 18:18:08 ciesnik Exp $";
#endif


static FILE *drmaa_logging_output = NULL;

/** Gets last system error message and returns its code. */
int
drmaa_get_errno_error(char *error_diagnosis, size_t error_diag_len)
  {
#if HAVE_STRERROR_R
  strerror_r(errno, error_diagnosis, error_diag_len);
#else /* assume strerror is thread safe */
  strlcpy(error_diagnosis, strerror(errno), error_diag_len);
#endif
  return errno;
  }

int
drmaa_get_drmaa_error(char *error_diagnosis, size_t error_diag_len,
                      int error_code)
  {
  snprintf(error_diagnosis, error_diag_len,
           "drmaa: %s", drmaa_strerror(error_code));
  return error_code;
  }

/** Retrieves last PBS error message. - no longer viable in multi-threaded torque */
int drmaa_get_pbs_error(
    
  char   *error_diagnosis,
  size_t  error_diag_len)
  {
  snprintf(error_diagnosis, error_diag_len,
           "pbs-drm: %s", "unknown error");
  return 1;
  }


void
drmaa_set_logging_output(FILE *file)
  {
  drmaa_logging_output = file;
  }


void
drmaa_log(const char *fmt, ...)
  {
  va_list args;
  char *linefmt = NULL;
  char *line    = NULL;
  FILE *out     = drmaa_logging_output;

  if (out == NULL)
    out = stderr;

  linefmt = (char*)malloc(strlen("drmaa pid=XXXXXXXXX: ") + strlen(fmt) + 2);

  sprintf(linefmt, "drmaa pid=%d: %s\n", (int)getpid(), fmt);

  va_start(args, fmt);

  if (-1 == vasprintf(&line, linefmt, args))
    {
    free(linefmt);
    return;
    }

  va_end(args);

  fwrite(line, 1, strlen(line), out);
  fflush(drmaa_logging_output);

  free(linefmt);
  free(line);
  }


const char *
drmaa_strerror(int drmaa_errno)
  {
  switch (drmaa_errno)
    {

    case DRMAA_ERRNO_SUCCESS:
      return "Success.";

    case DRMAA_ERRNO_INTERNAL_ERROR:
      return "Unexpected or internal DRMAA error.";

    case DRMAA_ERRNO_DRM_COMMUNICATION_FAILURE:
      return "Could not contact DRM system for this request.";

    case DRMAA_ERRNO_AUTH_FAILURE:
      return "Authorization failure.";

    case DRMAA_ERRNO_INVALID_ARGUMENT:
      return "Invalid argument value.";

    case DRMAA_ERRNO_NO_ACTIVE_SESSION:
      return "No active DRMAA session.";

    case DRMAA_ERRNO_NO_MEMORY:
      return "Not enough memory.";

    case DRMAA_ERRNO_INVALID_CONTACT_STRING:
      return "Invalid contact string.";

    case DRMAA_ERRNO_DEFAULT_CONTACT_STRING_ERROR:
      return "Can not determine default contact to DRM system.";

    case DRMAA_ERRNO_NO_DEFAULT_CONTACT_STRING_SELECTED:
      return "Contact to DRM must be set explicitly because there is no default.";

    case DRMAA_ERRNO_DRMS_INIT_FAILED:
      return "Unable to initialize DRM system.";

    case DRMAA_ERRNO_ALREADY_ACTIVE_SESSION:
      return "DRMAA session already exist.";

    case DRMAA_ERRNO_DRMS_EXIT_ERROR:
      return "Disengagement from the DRM system failed.";

    case DRMAA_ERRNO_INVALID_ATTRIBUTE_FORMAT:
      return "Invalid format of job attribute.";

    case DRMAA_ERRNO_INVALID_ATTRIBUTE_VALUE:
      return "Invalid value of job attribute.";

    case DRMAA_ERRNO_CONFLICTING_ATTRIBUTE_VALUES:
      return "Value of attribute conflicts with other attribute value.";

    case DRMAA_ERRNO_TRY_LATER:
      return "DRM system is overloaded.  Try again later.";

    case DRMAA_ERRNO_DENIED_BY_DRM:
      return "DRM rejected job due to its configuration or job attributes.";

    case DRMAA_ERRNO_INVALID_JOB:
      return "Job does not exist in DRMs queue.";

    case DRMAA_ERRNO_RESUME_INCONSISTENT_STATE:
      return "Can not resume job (not in valid state).";

    case DRMAA_ERRNO_SUSPEND_INCONSISTENT_STATE:
      return "Can not suspend job (not in valid state).";

    case DRMAA_ERRNO_HOLD_INCONSISTENT_STATE:
      return "Can not hold job (not in valid state).";

    case DRMAA_ERRNO_RELEASE_INCONSISTENT_STATE:
      return "Can not release job (not in valid state).";

    case DRMAA_ERRNO_EXIT_TIMEOUT:
      return "Waiting for job to terminate finished due to time-out.";

    case DRMAA_ERRNO_NO_RUSAGE:
      return "Job finished but resource usage information "
             "and/or termination status could not be provided.";

    case DRMAA_ERRNO_NO_MORE_ELEMENTS:
      return "Vector have no more elements.";

    default:
      return "?? unknown DRMAA error code ??";
    }
  }


/** Maps PBS error code into DMRAA code. */
int
drmaa_map_pbs_error(int pbs_errcode)
  {
  DEBUG(("-> drmaa_map_pbs_error(%d)", pbs_errcode));

  switch (pbs_errcode)
    {

    case PBSE_NONE:  /* no error */
      return DRMAA_ERRNO_SUCCESS;

    case PBSE_UNKJOBID:  /* Unknown Job Identifier */

    case PBSE_UNKQUE:  /* Unknown queue name */
      return DRMAA_ERRNO_INVALID_JOB;

    case PBSE_NOATTR: /* Undefined Attribute */

    case PBSE_ATTRRO: /* attempt to set READ ONLY attribute */

    case PBSE_IVALREQ:  /* Invalid request */

    case PBSE_UNKREQ:  /* Unknown batch request */
      return DRMAA_ERRNO_INTERNAL_ERROR;

    case PBSE_PERM:  /* No permission */

    case PBSE_BADHOST:  /* access from host not allowed */
      return DRMAA_ERRNO_AUTH_FAILURE;

    case PBSE_JOBEXIST:  /* job already exists */
      return DRMAA_ERRNO_DENIED_BY_DRM;

    case PBSE_SYSTEM:  /* system error occurred */

    case PBSE_INTERNAL:  /* internal server error occurred */

    case PBSE_REGROUTE:  /* parent job of dependent in rte que */

    case PBSE_UNKSIG:  /* unknown signal name */
      return DRMAA_ERRNO_INTERNAL_ERROR;

    case PBSE_BADATVAL:  /* bad attribute value */
      return DRMAA_ERRNO_INVALID_ATTRIBUTE_VALUE;

    case PBSE_MODATRRUN:  /* Cannot modify attrib in run state */

    case PBSE_BADSTATE:  /* request invalid for job state */

    case PBSE_BADCRED:  /* Invalid Credential in request */

    case PBSE_EXPIRED:  /* Expired Credential in request */

    case PBSE_QUNOENB:  /* Queue not enabled */
      return DRMAA_ERRNO_INTERNAL_ERROR;

    case PBSE_QACESS:  /* No access permission for queue */

    case PBSE_BADUSER:  /* Bad user - no password entry */
      return DRMAA_ERRNO_AUTH_FAILURE;

    case PBSE_HOPCOUNT:  /* Max hop count exceeded */

    case PBSE_QUEEXIST:  /* Queue already exists */

    case PBSE_ATTRTYPE:  /* incompatable queue attribute type */
      return DRMAA_ERRNO_INTERNAL_ERROR;

    case PBSE_QUEBUSY:  /* Queue Busy (not empty) */

    case PBSE_MAXQUED:  /* Max number of jobs in queue */

    case PBSE_NOCONNECTS:  /* No free connections */

    case PBSE_TOOMANY:  /* Too many submit retries */
      return DRMAA_ERRNO_TRY_LATER;

    case 111:
      return DRMAA_ERRNO_DRM_COMMUNICATION_FAILURE;
#if 0

    case PBSE_QUENBIG:  /* Queue name too long */

    case PBSE_NOSUP:  /* Feature/function not supported */

    case PBSE_QUENOEN:  /* Cannot enable queue,needs add def */

    case PBSE_PROTOCOL:  /* Protocol (ASN.1) error */

    case PBSE_BADATLST:  /* Bad attribute list structure */

    case PBSE_NOSERVER:  /* No server to connect to */

    case PBSE_UNKRESC:  /* Unknown resource */

    case PBSE_EXCQRESC:  /* Job exceeds Queue resource limits */

    case PBSE_QUENODFLT:  /* No Default Queue Defined */

    case PBSE_NORERUN:  /* Job Not Rerunnable */

    case PBSE_ROUTEREJ:  /* Route rejected by all destinations */

    case PBSE_ROUTEEXPD:  /* Time in Route Queue Expired */

    case PBSE_MOMREJECT:  /* Request to MOM failed */

    case PBSE_BADSCRIPT:  /* (qsub) cannot access script file */

    case PBSE_STAGEIN:  /* Stage In of files failed */

    case PBSE_RESCUNAV:  /* Resources temporarily unavailable */

    case PBSE_BADGRP:  /* Bad Group specified */

    case PBSE_CKPBSY:  /* Checkpoint Busy, may be retries */

    case PBSE_EXLIMIT:  /* Limit exceeds allowable */

    case PBSE_BADACCT:  /* Bad Account attribute value */

    case PBSE_ALRDYEXIT:  /* Job already in exit state */

    case PBSE_NOCOPYFILE:  /* Job files not copied */

    case PBSE_CLEANEDOUT:  /* unknown job id after clean init */

    case PBSE_NOSYNCMSTR:  /* No Master in Sync Set */

    case PBSE_BADDEPEND:  /* Invalid dependency */

    case PBSE_DUPLIST:  /* Duplicate entry in List */

    case PBSE_DISPROTO:  /* Bad DIS based Request Protocol */

    case PBSE_EXECTHERE:  /* cannot execute there */

    case PBSE_SISREJECT:  /* sister rejected */

    case PBSE_SISCOMM:  /* sister could not communicate */

    case PBSE_SVRDOWN:  /* req rejected -server shutting down */

    case PBSE_CKPSHORT:  /* not all tasks could checkpoint */

    case PBSE_UNKNODE:  /* Named node is not in the list */

    case PBSE_UNKNODEATR:  /* node-attribute not recognized */

    case PBSE_NONODES:  /* Server has no node list */

    case PBSE_NODENBIG:  /* Node name is too big */

    case PBSE_NODEEXIST:  /* Node name already exists */

    case PBSE_BADNDATVAL:  /* Bad node-attribute value */

    case PBSE_MUTUALEX:  /* State values are mutually exclusive */

    case PBSE_GMODERR:  /* Error(s) during global modification of nodes */

    case PBSE_NORELYMOM:  /* could not contact Mom */

    case PBSE_NOTSNODE:  /* no time-shared nodes */
      return DRMAA_ERRNO_INTERNAL_ERROR;
#endif

    default:
      return DRMAA_ERRNO_INTERNAL_ERROR;
    }
  }


