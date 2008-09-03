/* $Id: drmaa.h,v 1.15 2006/09/08 18:18:08 ciesnik Exp $ */
/**
 * @file drmaa.h
 * DRMAA library for Torque/PBS
 * @author Lukasz Ciesnik <lukasz.ciesnik@gmail.com>
 *
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
 *
 *
 * Documentation taken from:
 *
 *  <i>Distributed Resource Management Application API C Bindings v1.0</i>
 *
 *  Copyright (C) Global Grid Forum (2003). All Rights Reserved.
 *
 *  This document and translations of it may be copied and furnished to
 *  others, and derivative works that comment on or otherwise explain it
 *  or assist in its implementation may be prepared, copied, published
 *  and distributed, in whole or in part, without restriction of any kind,
 *  provided that the above copyright notice and this paragraph are included
 *  on all such copies and derivative works.  However, this document itself
 *  may not be modified in any way, such as by removing the copyright notice
 *  or references to the GGF or other organizations, except as needed for the
 *  purpose of developing Grid Recommendations in which case the procedures
 *  for copyrights defined in the GGF Document process must be followed,
 *  or as required to translate it into languages other than English.
 *
 *  The limited permissions granted above are perpetual and will not be
 *  revoked by the GGF or its successors or assigns.
 *  This document and the information contained herein is provided on an
 *  "AS IS" basis and THE GLOBAL GRID FORUM DISCLAIMS ALL WARRANTIES, EXPRESS
 *  OR IMPLIED, INCLUDING BUT NOT LIMITED TO ANY WARRANTY THAT THE USE OF THE
 *  INFORMATION HEREIN WILL NOT INFRINGE ANY RIGHTS OR ANY IMPLIED WARRANTIES
 *  OF MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE.
 */

#ifndef __DRMAA_H
#define __DRMAA_H

#include <stddef.h>
#include <stdio.h>

/** @defgroup drmaa_jobt Job template operations. */

/** @defgroup drmaa  DRMAA interface. */
/* @{ */

typedef struct drmaa_job_template_s  drmaa_job_template_t;

typedef struct drmaa_attr_names_s    drmaa_attr_names_t;

typedef struct drmaa_attr_values_s   drmaa_attr_values_t;

typedef struct drmaa_job_ids_s       drmaa_job_ids_t;

#define DRMAA_ATTR_BUFFER         1024
#define DRMAA_CONTACT_BUFFER      1024
#define DRMAA_DRM_SYSTEM_BUFFER   1024
#define DRMAA_DRMAA_IMPL_BUFFER   1024
#define DRMAA_ERROR_STRING_BUFFER 1024
#define DRMAA_JOBNAME_BUFFER      1024
#define DRMAA_SIGNAL_BUFFER         32

#define DRMAA_TIMEOUT_NO_WAIT            0
#define DRMAA_TIMEOUT_WAIT_FOREVER       -1
#define DRMAA_PS_UNDETERMINED            0x00
#define DRMAA_PS_QUEUED_ACTIVE           0x10
#define DRMAA_PS_SYSTEM_ON_HOLD          0x11
#define DRMAA_PS_USER_ON_HOLD            0x12
#define DRMAA_PS_USER_SYSTEM_ON_HOLD     0x13
#define DRMAA_PS_RUNNING                 0x20
#define DRMAA_PS_SYSTEM_SUSPENDED        0x21
#define DRMAA_PS_USER_SUSPENDED          0x22
#define DRMAA_PS_USER_SYSTEM_SUSPENDED   0x23
#define DRMAA_PS_DONE                    0x30
#define DRMAA_PS_FAILED                  0x40
#define DRMAA_CONTROL_SUSPEND            0
#define DRMAA_CONTROL_RESUME             1
#define DRMAA_CONTROL_HOLD               2
#define DRMAA_CONTROL_RELEASE            3
#define DRMAA_CONTROL_TERMINATE          4
#define DRMAA_JOB_IDS_SESSION_ALL        "DRMAA_JOB_IDS_SESSION_ALL"
#define DRMAA_JOB_IDS_SESSION_ANY        "DRMAA_JOB_IDS_SESSION_ANY"

#define DRMAA_BLOCK_EMAIL             "drmaa_block_email"
#define DRMAA_DEADLINE_TIME           "drmaa_deadline_time"
#define DRMAA_DURATION_HLIMIT         "drmaa_duration_hlimit"
#define DRMAA_DURATION_SLIMIT         "drmaa_duration_slimit"
#define DRMAA_ERROR_PATH              "drmaa_error_path"
#define DRMAA_INPUT_PATH              "drmaa_input_path"
#define DRMAA_JOB_CATEGORY            "drmaa_job_category"
#define DRMAA_JOB_NAME                "drmaa_job_name"
#define DRMAA_JOIN_FILES              "drmaa_join_files"
#define DRMAA_JS_STATE                "drmaa_js_state"
#define DRMAA_NATIVE_SPECIFICATION    "drmaa_native_specification"
#define DRMAA_OUTPUT_PATH             "drmaa_output_path"
#define DRMAA_REMOTE_COMMAND          "drmaa_remote_command"
#define DRMAA_START_TIME              "drmaa_start_time"
#define DRMAA_TRANSFER_FILES          "drmaa_transfer_files"
#define DRMAA_V_ARGV                  "drmaa_v_argv"
#define DRMAA_V_EMAIL                 "drmaa_v_email"
#define DRMAA_V_ENV                   "drmaa_v_env"
#define DRMAA_WCT_HLIMIT              "drmaa_wct_hlimit"
#define DRMAA_WCT_SLIMIT              "drmaa_wct_slimit"
#define DRMAA_WD                      "drmaa_wd"

#define DRMAA_SUBMISSION_STATE_ACTIVE "drmaa_active"
#define DRMAA_SUBMISSION_STATE_HOLD   "drmaa_hold"
#define DRMAA_PLACEHOLDER_HD          "$drmaa_hd_ph$"
#define DRMAA_PLACEHOLDER_WD          "$drmaa_wd_ph$"
#define DRMAA_PLACEHOLDER_INCR        "$drmaa_incr_ph$"

#define DRMAA_ERRNO_SUCCESS                               0
#define DRMAA_ERRNO_INTERNAL_ERROR                        1
#define DRMAA_ERRNO_DRM_COMMUNICATION_FAILURE             2
#define DRMAA_ERRNO_AUTH_FAILURE                          3
#define DRMAA_ERRNO_INVALID_ARGUMENT                      4
#define DRMAA_ERRNO_NO_ACTIVE_SESSION                     5
#define DRMAA_ERRNO_NO_MEMORY                             6
#define DRMAA_ERRNO_INVALID_CONTACT_STRING                7
#define DRMAA_ERRNO_DEFAULT_CONTACT_STRING_ERROR          8
#define DRMAA_ERRNO_NO_DEFAULT_CONTACT_STRING_SELECTED    9
#define DRMAA_ERRNO_DRMS_INIT_FAILED                     10
#define DRMAA_ERRNO_ALREADY_ACTIVE_SESSION               11
#define DRMAA_ERRNO_DRMS_EXIT_ERROR                      12
#define DRMAA_ERRNO_INVALID_ATTRIBUTE_FORMAT             13
#define DRMAA_ERRNO_INVALID_ATTRIBUTE_VALUE              14
#define DRMAA_ERRNO_CONFLICTING_ATTRIBUTE_VALUES         15
#define DRMAA_ERRNO_TRY_LATER                            16
#define DRMAA_ERRNO_DENIED_BY_DRM                        17
#define DRMAA_ERRNO_INVALID_JOB                          18
#define DRMAA_ERRNO_RESUME_INCONSISTENT_STATE            19
#define DRMAA_ERRNO_SUSPEND_INCONSISTENT_STATE           20
#define DRMAA_ERRNO_HOLD_INCONSISTENT_STATE              21
#define DRMAA_ERRNO_RELEASE_INCONSISTENT_STATE           22
#define DRMAA_ERRNO_EXIT_TIMEOUT                         23
#define DRMAA_ERRNO_NO_RUSAGE                            24
#define DRMAA_ERRNO_NO_MORE_ELEMENTS                     25

#if defined(__cplusplus)
extern "C"
  {
#endif


  /**
   * The drmaa_init() function SHALL initialize DRMAA library and create
   * a new DRMAA session, using the contact parameter, if provided, to
   * determine to which DRMS to connect.  This function MUST be called
   * before any other DRMAA function, except for drmaa_get_DRM_system(),
   * drmaa_get_DRMAA_implementation(), drmaa_get_contact(), and
   * drmaa_strerror().  If @a contact is @c NULL, the default DRM system
   * SHALL be used, provided there is only one DRMAA implementation
   * in the provided binary module.  When there is more than one DRMAA
   * implementation in the binary module, drmaa_init() SHALL return
   * the DRMAA_ERRNO_NO_DEFAULT_CONTACT_STRING_SELECTED error code.
   * The drmaa_init() function SHOULD be called by only one of the threads.
   * The main thread is RECOMMENDED.  A call by another thread SHALL return
   * the DRMAA_ERRNO_ALREADY_ACTIVE_SESSION error code.
   */
  int drmaa_init(
    const char *contact,
    char *error_diagnosis, size_t error_diag_len
  );

  /**
   * The drmaa_exit() function SHALL disengage from DRMAA library and
   * allow the DRMAA library to perform any necessary internal cleanup.
   * This routine SHALL end the current DRMAA session but SHALL NOT
   * affect any jobs (e.g, queued and running jobs SHALL remain queued and
   * running).  drmaa_exit() SHOULD be called by only one of the threads.
   * The first call to call drmaa_exit() by a thread will operate normally.
   * All other calls from the same and other threads SHALL fail, returning
   * a DRMAA_ERRNO_NO_ACTIVE_SESSION error code.
   */
  int drmaa_exit(char *error_diagnosis, size_t error_diag_len);



  /**
   * The function drmaa_allocate_job_template() SHALL allocate a new job
   * template, returned in @a jt.  This template is used to describe the
   * job to be submitted.  This description is accomplished by setting the
   * desired scalar and vector attributes to their appropriate values. This
   * template is then used in the job submission process.
   * @addtogroup drmaa_jobt
   */
  int drmaa_allocate_job_template(
    drmaa_job_template_t **jt,
    char *error_diagnosis,
    size_t error_diag_len
  );

  /**
   * The function drmaa_delete_job_template() SHALL free the job template
   * pointed to by @a jt.
   * @addtogroup drmaa_jobt
   */
  int drmaa_delete_job_template(
    drmaa_job_template_t *jt,
    char *error_diagnosis, size_t error_diag_len
  );

  /**
   * The function drmaa_set_attribute() SHALL set the value of the scalar
   * attribute, @a name, in the job template, @a jt, to the value, @a value.
   * @addtogroup drmaa_jobt
   */
  int drmaa_set_attribute(
    drmaa_job_template_t *jt,
    const char *name, const char *value,
    char *error_diagnosis, size_t error_diag_len
  );

  /**
   * The function drmaa_get_attribute() SHALL fill the @a value buffer with
   * up to @a value_len characters of the scalar attribute, @a name's, value
   * in the given job template.
   */
  int drmaa_get_attribute(
    drmaa_job_template_t *jt,
    const char *name, char *value, size_t value_len,
    char *error_diagnosis, size_t error_diag_len
  );

  /**
   * The function drmaa_set_vector_attribute() SHALL set the vector attribute,
   * @a name, in the job template, @a jt, to the value(s), @a value.  The DRMAA
   * implementation MUST accept value values that are arrays of one or more
   * strings terminated by a @c NULL entry.
   */
  int drmaa_set_vector_attribute(
    drmaa_job_template_t *jt,
    const char *name, const char *value[],
    char *error_diagnosis, size_t error_diag_len
  );

  /**
   * The function drmaa_get_vector_attribute() SHALL store in @a values an
   * opaque values string vector containing the values of the vector attribute,
   * @a name's, value in the given job template.
   */
  int drmaa_get_vector_attribute(
    drmaa_job_template_t *jt,
    const char *name, drmaa_attr_values_t **values,
    char *error_diagnosis, size_t error_diag_len
  );



  /**
   * The function drmaa_get_attribute_names() SHALL return the set of supported
   * scalar attribute names in an opaque names string vector stored in
   * @a values.  This vector SHALL include all required scalar attributes, all
   * supported optional scalar attributes, all DRM-specific scalar attributes,
   * and no unsupported optional attributes.
   */
  int drmaa_get_attribute_names(
    drmaa_attr_names_t **values,
    char *error_diagnosis, size_t error_diag_len
  );

  /**
   * The function drmaa_get_vector_attribute_names() SHALL return the set
   * of supported vector attribute names in an opaque names string vector
   * stored in @a values.  This vector SHALL include all required vector
   * attributes, all supported optional vector attributes, all DRM-specific
   * vector attributes, and no unsupported optional attributes.
   */
  int drmaa_get_vector_attribute_names(
    drmaa_attr_names_t **values,
    char *error_diagnosis, size_t error_diag_len
  );

  /**
   * @defgroup drmaa_viter  Vector iteration functions.
   * @ingroup drmaa
   *
   * The drmaa_get_next_<i>X</i>() functions SHALL store up to @a value_len
   * bytes of the next attribute name / attribute value / job identifier
   * from the @a values opaque string vector in the @a value buffer.
   * The opaque string vector's internal iterator SHALL then be moved forward
   * to the next entry.  If there are no more values those functions return
   * @ref DRMAA_ERRNO_INVALID_ARGUMENT (but this is outside DRMAA specification).
   *
   * The drmaa_get_num_<i>X</i>() functions SHALL store the number of elements
   * in the space provided by @a size.
   *
   * The drmaa_release_<i>X</i>() functions free the memory used by the
   * @a values opaque string vector.  All memory used by strings contained
   * therein is also freed.
   */
  /* @addtogroup drmaa_viter @{ */
  int drmaa_get_next_attr_name(drmaa_attr_names_t* values,
                               char *value, size_t value_len);
  int drmaa_get_next_attr_value(drmaa_attr_values_t* values,
                                char *value, size_t value_len);
  int drmaa_get_next_job_id(drmaa_job_ids_t* values,
                            char *value, size_t value_len);
  int drmaa_get_num_attr_names(drmaa_attr_names_t* values, size_t *size);
  int drmaa_get_num_attr_values(drmaa_attr_values_t* values, size_t *size);
  int drmaa_get_num_job_ids(drmaa_job_ids_t* values, size_t *size);
  void drmaa_release_attr_names(drmaa_attr_names_t* values);
  void drmaa_release_attr_values(drmaa_attr_values_t* values);
  void drmaa_release_job_ids(drmaa_job_ids_t* values);
  /* @} */

  /**
   * The drmaa_run_job() function submits a single job with the attributes
   * defined in the job template, @a jt.  Upon success, up to @a job_id_len
   * characters of the submitted job's job identifier are stored in the buffer,
   * @a job_id.
   */
  int drmaa_run_job(
    char *job_id, size_t job_id_len, const drmaa_job_template_t *jt,
    char *error_diagnosis, size_t error_diag_len
  );

  /**
   * The drmaa_run_bulk_jobs() function submits a set of parametric jobs which
   * can be run concurrently.  The attributes defined in the job template,
   * @a jt are used for every parametric job in the set.  Each job in the
   * set is identical except for it's index.  The first parametric job has an
   * index equal to @a start.  The next job has an index equal to @a start +
   * @a incr, and so on.  The last job has an index equal to <code>start + n *
   * incr</code>, where @c n is equal to <code>(end - start) / incr</code>.
   * Note that the value of the last job's index may not be equal to end if
   * the difference between @a start and @a end is not evenly divisble by
   * @a incr.  The smallest valid value for @a start is 1.  The largest valid
   * value for @a end is 2147483647 (@f$2^{31}-1@f$).  The @a start value
   * must be less than or equal to the @a end value, and only positive index
   * numbers are allowed.  The index number can be determined by the job in
   * an implementation specific fashion.  On success, an opaque job id string
   * vector containing job identifiers for all submitted jobs SHALL be returned
   * into @a job_ids.  The job identifiers in the opaque job id string vector
   * can be extracted using the drmaa_get_next_job_id() function. The caller
   * is responsible for releasing the opaque job id string vector returned
   * into @a job_ids using the drmaa_release_job_ids() function.
   */
  int drmaa_run_bulk_jobs(
    drmaa_job_ids_t **jobids,
    const drmaa_job_template_t *jt,
    int start, int end, int incr,
    char *error_diagnosis,
    size_t error_diag_len
  );

  /**
   * The drmaa_control() function SHALL enact the action indicated
   * by @a action on the job specified by the job identifier, @a jobid.
   * The action parameter's value may be one of the following:
   *   - DRMAA_CONTROL_SUSPEND
   *   - DRMAA_CONTROL_RESUME
   *   - DRMAA_CONTROL_HOLD
   *   - DRMAA_CONTROL_RELEASE
   *   - DRMAA_CONTROL_TERMINATE
   * The drmaa_control() function SHALL return after the DRM system has
   * acknowledged the command, not necessarily after the desired action has
   * been performed.  If @a jobid is DRMAA_JOB_IDS_SESSION_ALL, this function
   * SHALL perform the specified action on all jobs submitted during this
   * session as of this function is called.
   */
  int drmaa_control(
    const char *job_id, int action,
    char *error_diagnosis, size_t error_diag_len
  );

  /**
   * The drmaa_job_ps() function SHALL store in @a remote_ps the program
   * status of the job identified by @a job_id. The possible values of
   * a program's staus are:
   *   - DRMAA_PS_UNDETERMINED
   *   - DRMAA_PS_QUEUED_ACTIVE
   *   - DRMAA_PS_SYSTEM_ON_HOLD
   *   - DRMAA_PS_USER_ON_HOLD
   *   - DRMAA_PS_USER_SYSTEM_ON_HOLD
   *   - DRMAA_PS_RUNNING
   *   - DRMAA_PS_SYSTEM_SUSPENDED
   *   - DRMAA_PS_USER_SUSPENDED
   *   - DRMAA_PS_DONE
   *   - DRMAA_PS_FAILED
   *
   * Terminated jobs have a status of DRMAA_PS_FAILED.
   */
  int drmaa_job_ps(
    const char *job_id, int *remote_ps,
    char *error_diagnosis, size_t error_diag_len
  );

  /**
   * The drmaa_synchronize() function SHALL cause the calling thread to
   * block until all jobs specified by @a job_ids have finished execution.
   * If @a job_ids contains DRMAA_JOB_IDS_SESSION_ALL, then this function
   * SHALL wait for all jobs submitted during this DRMAA session as of the
   * point in time when drmaa_synchronize() is called.  To avoid thread race
   * conditions in multithreaded applications, the DRMAA implementation user
   * should explicitly synchronize this call with any other job submission
   * calls or control calls that may change the number of remote jobs.
   *
   * The @a timeout parameter value indicates how many seconds to remain
   * blocked in this call waiting for results to become available, before
   * returning with a DRMAA_ERRNO_EXIT_TIMEOUT error code.  The value,
   * DRMAA_TIMEOUT_WAIT_FOREVER, MAY be specified to wait indefinitely for
   * a result.  The value, DRMAA_TIMEOUT_NO_WAIT, MAY be specified to return
   * immediately with a DRMAA_ERRNO_EXIT_TIMEOUT error code if no result is
   * available.  If the call exits before the timeout has elapsed, all the
   * jobs have been waited on or there was an interrupt.  The caller should
   * check system time before and after this call in order to be sure of how
   * much time has passed.  The @a dispose parameter specifies how to treat
   * the reaping of the remote job's internal data record, which includes a
   * record of the job's consumption of system resources during its execution
   * and other statistical information.  If the @a dispose parameter's value
   * is 1, the DRMAA implementation SHALL dispose of the job's data record at
   * the end of the drmaa_synchroniize() call.  If the @a dispose parameter's
   * value is 0, the data record SHALL be left for future access via the
   * drmaa_wait() method.
   */
  int drmaa_synchronize(
    const char *job_ids[], signed long timeout, int dispose,
    char *error_diagnosis, size_t error_diag_len
  );

  /**
   * The drmaa_wait() function SHALL wait for a job identified by @a job_id
   * to finish execution or fail. If the special string, JOB_IDS_SESSION_ANY,
   * is provided as the job_id, this function will wait for any job from the
   * session to finish execution or fail.  In this case, any job for which exit
   * status information is available will satisfy the requirement, including
   * jobs which preivously finished but have never been the subject of a
   * drmaa_wait() call.  This routine is modeled on the @c wait3 POSIX routine.
   *
   * The @a timeout parameter value indicates how many seconds
   * to remain blocked in this call waiting for a result, before
   * returning with a DRMAA_ERRNO_EXIT_TIMEOUT error code.  The value,
   * DRMAA_TIMEOUT_WAIT_FOREVER, MAY be specified to wait indefinitely for
   * a result.  The value, DRMAA_TIMEOUT_NO_WAIT, MAY be specified to return
   * immediately with a DRMAA_ERRNO_EXIT_TIMEOUT error code if no result is
   * available.  If the call exits before the timeout has elapsed, the job
   * has been successfully waited on or there was an interrupt.  The caller
   * should check system time before and after this call in order to be sure
   * of how much time has passed.
   *
   * Upon success, drmaa_wait() fills @a job_id_out with up to @a
   * job_id_out_len characters of the waited job's id, stat with the
   * a code that includes information about the conditions under which
   * the job terminated, and @a rusage with an array of \<name\>=\<value\>
   * strings that describe the amount of resources consumed by the job
   * and are implementation defined.  The @a stat parameter is further
   * described below.  The @a rusage parameter's values may be accessed via
   * drmaa_get_next_attr_value().
   *
   * The drmaa_wait() function reaps job data records on a successful
   * call, so any subsequent calls to drmaa_wait() will fail, returning
   * a DRMAA_ERRNO_INVALID_JOB error code, meaning that the job's data
   * record has already been reaped.  This error code is the same as
   * if the job were unknown.  If drmaa_wait() exists due to a timeout,
   * DRMAA_ERRNO_EXIT_TIMEOUT is returned and no rusage information is reaped.
   * (The only case where drmaa_wait() can be successfully called on a single
   * job more than once is when the previous call(s) to drmaa_wait() returned
   * DRMAA_ERRNO_EXIT_TIMEOUT.)
   *
   * The stat parameter, set by a successful call to drmaa_wait(), is used
   * to retrieve further input about the exit condition of the waited
   * job, identified by job_id_out, through the following functions:
   * drmaa_wifexited(), drmaa_wexitstatus(), drmaa_wifsignaled(),
   * drmaa_wtermsig(),drmaa_wcoredump() and drmaa_wifaborted().
   */
  int drmaa_wait(
    const char *job_id,
    char *job_id_out, size_t job_id_out_len, int *stat,
    signed long timeout, drmaa_attr_values_t **rusage,
    char *error_diagnosis, size_t error_diag_len
  );

  int drmaa_wifexited(int *exited, int stat,
                      char *error_diagnosis, size_t error_diag_len);
  int drmaa_wexitstatus(int *exit_status, int stat,
                        char *error_diagnosis, size_t error_diag_len);
  int drmaa_wifsignaled(int *signaled, int stat,
                        char *error_diagnosis, size_t error_diag_len);
  int drmaa_wtermsig(char *signal, size_t signal_len, int stat,
                     char *error_diagnosis, size_t error_diag_len);
  int drmaa_wcoredump(int *core_dumped, int stat,
                      char *error_diagnosis, size_t error_diag_len);
  int drmaa_wifaborted(int *aborted, int stat,
                       char *error_diagnosis, size_t error_diag_len);

  /**
   * The drmaa_strerror() function SHALL return the error string describing
   * the DRMAA error number @a drmaa_errno.
   */
  const char *drmaa_strerror(int drmaa_errno);

  /**
   * The drmaa_get_contact() function, if called before drmaa_init(), SHALL
   * return a string containing a comma-delimited list of default DRMAA
   * implementation contacts strings, one per DRM implementation provided.
   * If called after drmaa_init(), drmaa_get_contacts() SHALL return the
   * contact string for the DRM system for which the library has been
   * initialized.
   */
  int drmaa_get_contact(char *contact, size_t contact_len,
                        char *error_diagnosis, size_t error_diag_len);

  /**
   * The drmaa_version() function SHALL set major and minor to the major and
   * minor versions of the DRMAA C binding specification implemented by the
   * DRMAA implementation.
   */
  int drmaa_version(unsigned int *major, unsigned int *minor,
                    char *error_diagnosis, size_t error_diag_len);

  /**
   * The drmaa_get_DRM_system() function, if called before drmaa_init(),
   * SHALL return a string containing a comma-delimited list of DRM system
   * identifiers, one per DRM system implementation provided.  If called after
   * drmaa_init(), drmaa_get_DRM_system() SHALL return the selected DRM system.
   */
  int drmaa_get_DRM_system(char *drm_system, size_t drm_system_len,
                           char *error_diagnosis, size_t error_diag_len);

  /**
   * The drmaa_get_DRMAA_implementation() function, if called before
   * drmaa_init(), SHALL return a string containing a comma-delimited list of
   * DRMAA implementations, one per DRMAA implementation provided.  If called
   * after drmaa_init(), drmaa_get_DRMAA_implementation() SHALL return the
   * selected DRMAA implementation.
   */
  int drmaa_get_DRMAA_implementation(char *drmaa_impl, size_t drmaa_impl_len,
                                     char *error_diagnosis, size_t error_diag_len);

#if defined(__cplusplus)
  } /* extern "C" */

#endif

/* @} */


/*
 * Functions outside of DRMAA specification.
 */

/**
 * Specify place where goes log messages from library when they were enabled
 * at configure time.  By default they are written to standard error stream.
 * @param file   File to write to.
 */
void drmaa_set_logging_output(FILE *file);

#endif /* __DRMAA_H */

