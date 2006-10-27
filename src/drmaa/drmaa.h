#ifndef __DRMAA_H
#define __DRMAA_H

#ifdef  __cplusplus
extern "C" {
#endif

/* 
 * these hooks are not part of the DRMAA interface 
 * see www.drmaa.org for the DRMAA specification 
 */ 
#ifdef DRMAA_HOOKS
extern int delay_after_submit;
#endif

#include <pthread.h>


/* ------------------- Constants ------------------- */
/* 
 * some not yet agreed buffer length constants 
 * these are recommended minimum values 
 */

/* drmaa_get_attribute() */
#define DRMAA_ATTR_BUFFER 1024

/* drmaa_get_contact() */
#define DRMAA_CONTACT_BUFFER 1024

/* drmaa_get_DRM_system() */
#define DRMAA_DRM_SYSTEM_BUFFER 1024

/* 
 * Agreed buffer length constants 
 * these are recommended minimum values 
 */
#define DRMAA_ERROR_STRING_BUFFER   1024
#define DRMAA_JOBNAME_BUFFER        1024
#define DRMAA_SIGNAL_BUFFER         32

/*
 * Agreed constants 
 */ 
#define DRMAA_TIMEOUT_WAIT_FOREVER  -1
#define DRMAA_TIMEOUT_NO_WAIT       0 

#define DRMAA_JOB_IDS_SESSION_ANY   "DRMAA_JOB_IDS_SESSION_ANY"
#define DRMAA_JOB_IDS_SESSION_ALL   "DRMAA_JOB_IDS_SESSION_ALL"

#define DRMAA_SUBMISSION_STATE_ACTIVE "drmaa_active"
#define DRMAA_SUBMISSION_STATE_HOLD   "drmaa_hold"

/*
 * Agreed placeholder names
 */
#define DRMAA_PLACEHOLDER_INCR       "$drmaa_incr_ph$"
#define DRMAA_PLACEHOLDER_HD         "$drmaa_hd_ph$"
#define DRMAA_PLACEHOLDER_WD         "$drmaa_wd_ph$"

/*
 * Agreed names of job template attributes 
 */
#define DRMAA_REMOTE_COMMAND         "drmaa_remote_command"
#define DRMAA_JS_STATE               "drmaa_js_state"
#define DRMAA_WD                     "drmaa_wd"
#define DRMAA_JOB_CATEGORY           "drmaa_job_category"
#define DRMAA_NATIVE_SPECIFICATION   "drmaa_native_specification"
#define DRMAA_BLOCK_EMAIL            "drmaa_block_email"
#define DRMAA_START_TIME             "drmaa_start_time"
#define DRMAA_JOB_NAME               "drmaa_job_name"
#define DRMAA_INPUT_PATH             "drmaa_input_path"
#define DRMAA_OUTPUT_PATH            "drmaa_output_path"
#define DRMAA_ERROR_PATH             "drmaa_error_path"
#define DRMAA_JOIN_FILES             "drmaa_join_files"
#define DRMAA_TRANSFER_FILES         "drmaa_transfer_files"
#define DRMAA_DEADLINE_TIME          "drmaa_deadline_time"
#define DRMAA_WCT_HLIMIT             "drmaa_wct_hlimit"
#define DRMAA_WCT_SLIMIT             "drmaa_wct_slimit"
#define DRMAA_DURATION_HLIMIT        "drmaa_duration_hlimit"
#define DRMAA_DURATION_SLIMIT        "drmaa_duration_slimit"

/* names of job template vector attributes */
#define DRMAA_V_ARGV                 "drmaa_v_argv"
#define DRMAA_V_ENV                  "drmaa_v_env"
#define DRMAA_V_EMAIL                "drmaa_v_email"

/* 
 * DRMAA errno values 
 *
 * the values in detail still need to be agreed  
 */
enum {
   /* -------------- these are relevant to all sections ---------------- */
   DRMAA_ERRNO_SUCCESS = 0, /* Routine returned normally with success. */
   DRMAA_ERRNO_INTERNAL_ERROR, /* Unexpected or internal DRMAA error like memory allocation, system call failure, etc. */
   DRMAA_ERRNO_DRM_COMMUNICATION_FAILURE, /* Could not contact DRM system for this request. */
   DRMAA_ERRNO_AUTH_FAILURE, /* The specified request is not processed successfully due to authorization failure. */
   DRMAA_ERRNO_INVALID_ARGUMENT, /* The input value for an argument is invalid. */
   DRMAA_ERRNO_NO_ACTIVE_SESSION, /* Exit routine failed because there is no active session */
   DRMAA_ERRNO_NO_MEMORY, /* failed allocating memory */

   /* -------------- init and exit specific --------------- */
   DRMAA_ERRNO_INVALID_CONTACT_STRING, /* Initialization failed due to invalid contact string. */
   DRMAA_ERRNO_DEFAULT_CONTACT_STRING_ERROR, /* DRMAA could not use the default contact string to connect to DRM system. */
   DRMAA_ERRNO_NO_DEFAULT_CONTACT_STRING_SELECTED, /* DRMAA could not use the default contact string to connect to DRM system. */
   DRMAA_ERRNO_DRMS_INIT_FAILED, /* Initialization failed due to failure to init DRM system. */
   DRMAA_ERRNO_ALREADY_ACTIVE_SESSION, /* Initialization failed due to existing DRMAA session. */
   DRMAA_ERRNO_DRMS_EXIT_ERROR, /* DRM system disengagement failed. */

   /* ---------------- job attributes specific -------------- */
   DRMAA_ERRNO_INVALID_ATTRIBUTE_FORMAT, /* The format for the job attribute value is invalid. */
   DRMAA_ERRNO_INVALID_ATTRIBUTE_VALUE, /* The value for the job attribute is invalid. */
   DRMAA_ERRNO_CONFLICTING_ATTRIBUTE_VALUES, /* The value of this attribute is conflicting with a previously set attributes. */
   DRMAA_ERRNO_NO_MORE_ELEMENTS, /* No more element in the attribute name/value list */

   /* --------------------- job submission specific -------------- */
   DRMAA_ERRNO_TRY_LATER, /* Could not pass job now to DRM system. A retry may succeed however (saturation). */
   DRMAA_ERRNO_DENIED_BY_DRM, /* The DRM system rejected the job. The job will never be accepted due to DRM configuration or job template settings. */

   /* ------------------------------- job control specific ---------------- */
   DRMAA_ERRNO_INVALID_JOB, /* The job specified by the 'jobid' does not exist. */
   DRMAA_ERRNO_RESUME_INCONSISTENT_STATE, /* The job has not been suspended. The RESUME request will not be processed. */
   DRMAA_ERRNO_SUSPEND_INCONSISTENT_STATE, /* The job has not been running, and it cannot be suspended. */
   DRMAA_ERRNO_HOLD_INCONSISTENT_STATE, /* The job cannot be moved to a HOLD state. */
   DRMAA_ERRNO_RELEASE_INCONSISTENT_STATE, /* The job is not in a HOLD state. */
   DRMAA_ERRNO_EXIT_TIMEOUT, /* We have encountered a time-out condition for drmaa_synchronize or drmaa_wait. */
   DRMAA_ERRNO_NO_RUSAGE,
   DRMAA_NO_ERRNO
};

/* 
 * Agreed DRMAA job states as returned by drmaa_job_ps() 
 */
enum {
 DRMAA_PS_UNDETERMINED          = 0x00, /* process status cannot be determined */
 DRMAA_PS_QUEUED_ACTIVE         = 0x10, /* job is queued and active */
 DRMAA_PS_SYSTEM_ON_HOLD        = 0x11, /* job is queued and in system hold */
 DRMAA_PS_USER_ON_HOLD          = 0x12, /* job is queued and in user hold */
 DRMAA_PS_USER_SYSTEM_ON_HOLD   = 0x13, /* job is queued and in user and system hold */
 DRMAA_PS_RUNNING               = 0x20, /* job is running */
 DRMAA_PS_SYSTEM_SUSPENDED      = 0x21, /* job is system suspended */
 DRMAA_PS_USER_SUSPENDED        = 0x22, /* job is user suspended */
 DRMAA_PS_USER_SYSTEM_SUSPENDED = 0x23, /* job is user and system suspended */
 DRMAA_PS_DONE                  = 0x30, /* job finished normally */
 DRMAA_PS_FAILED                = 0x40  /* job finished, but failed */
};

/* 
 * Agreed DRMAA actions for drmaa_control() 
 */
enum {
 DRMAA_CONTROL_SUSPEND = 0,
 DRMAA_CONTROL_RESUME,
 DRMAA_CONTROL_HOLD,
 DRMAA_CONTROL_RELEASE,
 DRMAA_CONTROL_TERMINATE
};

/* ------------------- Data types ------------------- */
/* 
 * Agreed opaque DRMAA job template 
 * struct drmaa_job_template_s is defined elsewhere 
 */
typedef struct drmaa_job_template_s drmaa_job_template_t;

/* ---------- C/C++ language binding specific interfaces -------- */

typedef struct drmaa_attr_names_s drmaa_attr_names_t;
typedef struct drmaa_attr_values_s drmaa_attr_values_t;
typedef struct drmaa_job_ids_s  drmaa_job_ids_t;

/*
 * get next string attribute from iterator 
 * 
 * returns DRMAA_ERRNO_SUCCESS or DRMAA_ERRNO_INVALID_ATTRIBUTE_VALUE
 * if no such exists 
 */

int drmaa_get_next_attr_name(drmaa_attr_names_t* values, char *value, int value_len);
int drmaa_get_next_attr_value(drmaa_attr_values_t* values, char *value, int value_len);
int drmaa_get_next_job_id(drmaa_job_ids_t* values, char *value, int value_len);

/* 
 * release opaque string vector 
 *
 * Opaque string vectors can be used without any constraint
 * until the release function has been called.
 */
void drmaa_release_attr_names( drmaa_attr_names_t* values );
void drmaa_release_attr_values( drmaa_attr_values_t* values );
void drmaa_release_job_ids( drmaa_job_ids_t* values );

/* ------------------- init/exit routines ------------------- */
/*
 * Initialize DRMAA API library and create a new DRMAA Session. 'Contact'
 * is an implementation dependent string which may be used to specify
 * which DRM system to use. This routine must be called before any
 * other DRMAA calls, except for drmaa_version().
 * If 'contact' is NULL, the default DRM system will be used.
 */
int drmaa_init(const char *contact, char *error_diagnosis, size_t error_diag_len);


/*
 * Disengage from DRMAA library and allow the DRMAA library to perform
 * any necessary internal clean up.
 * This routine ends this DRMAA Session, but does not effect any jobs (e.g.,
 * queued and running jobs remain queued and running).
 */
int drmaa_exit(char *error_diagnosis, size_t error_diag_len);

/* ------------------- job template routines ------------------- */

/* 
 * Allocate a new job template. 
 */
int drmaa_allocate_job_template(drmaa_job_template_t **jt, char *error_diagnosis, size_t error_diag_len);

/* 
 * Deallocate a job template. This routine has no effect on jobs.
 */
int drmaa_delete_job_template(drmaa_job_template_t *jt, char *error_diagnosis, size_t error_diag_len);


/* 
 * Adds ('name', 'value') pair to list of attributes in job template 'jt'.
 * Only non-vector attributes may be passed.
 */
int drmaa_set_attribute(drmaa_job_template_t *jt, const char *name, const char *value, char *error_diagnosis, size_t error_diag_len);


/* 
 * If 'name' is an existing non-vector attribute name in the job 
 * template 'jt', then the value of 'name' is returned; otherwise, 
 * NULL is returned.
 */ 
int drmaa_get_attribute(drmaa_job_template_t *jt, const char *name, char *value, size_t value_len, char *error_diagnosis, size_t error_diag_len);

/* Adds ('name', 'values') pair to list of vector attributes in job template 'jt'.
 * Only vector attributes may be passed.
 */
int drmaa_set_vector_attribute(drmaa_job_template_t *jt, const char *name, const char *value[], char *error_diagnosis, size_t error_diag_len);


/* 
 * If 'name' is an existing vector attribute name in the job template 'jt',
 * then the values of 'name' are returned; otherwise, NULL is returned.
 */
int drmaa_get_vector_attribute(drmaa_job_template_t *jt, const char *name, drmaa_attr_values_t **values, char *error_diagnosis, size_t error_diag_len);


/* 
 * Returns the set of supported attribute names whose associated   
 * value type is String. This set will include supported DRMAA reserved 
 * attribute names and native attribute names. 
 */
int drmaa_get_attribute_names( drmaa_attr_names_t **values, char *error_diagnosis, size_t error_diag_len);

/*
 * Returns the set of supported attribute names whose associated 
 * value type is String Vector.  This set will include supported DRMAA reserved 
 * attribute names and native attribute names. */
int drmaa_get_vector_attribute_names(drmaa_attr_names_t **values, char *error_diagnosis, size_t error_diag_len);

/* ------------------- job submission routines ------------------- */

/*
 * Submit a job with attributes defined in the job template 'jt'.
 * The job identifier 'job_id' is a printable, NULL terminated string,
 * identical to that returned by the underlying DRM system.
 */
int drmaa_run_job(char *job_id, size_t job_id_len, drmaa_job_template_t *jt, char *error_diagnosis, size_t error_diag_len);

/* 
 * Submit a set of parametric jobs, dependent on the implied loop index, each
 * with attributes defined in the job template 'jt'.
 * The job identifiers 'job_ids' are all printable,
 * NULL terminated strings, identical to those returned by the underlying
 * DRM system. Nonnegative loop bounds are mandated to avoid file names
 * that start with minus sign like command line options.
 * The special index placeholder is a DRMAA defined string
 * drmaa_incr_ph == $incr_pl$
 * that is used to construct parametric job templates.
 * For example:
 * drmaa_set_attribute(pjt, "stderr", drmaa_incr_ph + ".err" ); (C++/java string syntax used)
 */
int drmaa_run_bulk_jobs( drmaa_job_ids_t **jobids, drmaa_job_template_t *jt, int start, int end, int incr, char *error_diagnosis, size_t error_diag_len);

/* ------------------- job control routines ------------------- */

/*
 * Start, stop, restart, or kill the job identified by 'job_id'.
 * If 'job_id' is DRMAA_JOB_IDS_SESSION_ALL, then this routine
 * acts on all jobs *submitted* during this DRMAA session.
 * The legal values for 'action' and their meanings are:
 * DRMAA_CONTROL_SUSPEND: stop the job,
 * DRMAA_CONTROL_RESUME: (re)start the job,
 * DRMAA_CONTROL_HOLD: put the job on-hold,
 * DRMAA_CONTROL_RELEASE: release the hold on the job, and
 * DRMAA_CONTROL_TERMINATE: kill the job.
 * This routine returns once the action has been acknowledged by
 * the DRM system, but does not necessarily wait until the action
 * has been completed.
 */
int drmaa_control(const char *jobid, int action, char *error_diagnosis, size_t error_diag_len);


/* 
 * Wait until all jobs specified by 'job_ids' have finished
 * execution. If 'job_ids' is DRMAA_JOB_IDS_SESSION_ALL, then this routine
 * waits for all jobs *submitted* during this DRMAA session. To prevent
 * blocking indefinitely in this call the caller could use timeout specifying
 * after how many seconds to time out in this call.
 * If the call exits before timeout all the jobs have been waited on
 * or there was an interrupt.
 * If the invocation exits on timeout, the return code is DRMAA_ERRNO_EXIT_TIMEOUT.
 * The caller should check system time before and after this call
 * in order to check how much time has passed.
 * Dispose parameter specifies how to treat reaping information:
 * True=1 "fake reap", i.e. dispose of the rusage data
 * False=0 do not reap
 */ 
int drmaa_synchronize(const char *job_ids[], signed long timeout, int dispose, char *error_diagnosis, size_t error_diag_len);


/* 
 * This routine waits for a job with job_id to fail or finish execution. Passing a special string
 * DRMAA_JOB_IDS_SESSION_ANY instead job_id waits for any job. If such a job was
 * successfully waited its job_id is returned as a second parameter. This routine is
 * modeled on wait3 POSIX routine. To prevent
 * blocking indefinitely in this call the caller could use timeout specifying
 * after how many seconds to time out in this call.
 * If the call exits before timeout the job has been waited on
 * successfully or there was an interrupt.
 * If the invocation exits on timeout, the return code is DRMAA_ERRNO_EXIT_TIMEOUT.
 * The caller should check system time before and after this call
 * in order to check how much time has passed.
 * The routine reaps jobs on a successful call, so any subsequent calls
 * to drmaa_wait should fail returning an error DRMAA_ERRNO_INVALID_JOB meaning
 * that the job has been already reaped. This error is the same as if the job was
 * unknown. Failing due to an elapsed timeout has an effect that it is possible to
 * issue drmaa_wait multiple times for the same job_id.
 */
int drmaa_wait(const char *job_id, char *job_id_out, size_t job_id_out_len, int *stat, 
   signed long timeout, drmaa_attr_values_t **rusage, 
   char *error_diagnosis, size_t error_diagnois_len);

/* 
 * Evaluates into 'exited' a non-zero value if stat was returned for a
 * job that terminated normally. A zero value can also indicate that
 * altough the job has terminated normally an exit status is not available
 * or that it is not known whether the job terminated normally. In both
 * cases drmaa_wexitstatus() will not provide exit status information.
 * A non-zero 'exited' value indicates more detailed diagnosis can be provided
 * by means of drmaa_wifsignaled(), drmaa_wtermsig() and drmaa_wcoredump(). 
 */
int drmaa_wifexited(int *exited, int stat, char *error_diagnosis, size_t error_diag_len);

/* 
 * If the OUT parameter 'exited' of drmaa_wifexited() is non-zero,
 * this function evaluates into 'exit_code' the exit code that the
 * job passed to _exit() (see exit(2)) or exit(3C), or the value that
 * the child process returned from main. 
 */
int drmaa_wexitstatus(int *exit_status, int stat, char *error_diagnosis, size_t error_diag_len);

/* 
 * Evaluates into 'signaled' a non-zero value if status was returned
 * for a job that terminated due to the receipt of a signal. A zero value
 * can also indicate that altough the job has terminated due to the receipt
 * of a signal the signal is not available or that it is not known whether
 * the job terminated due to the receipt of a signal. In both cases
 * drmaa_wtermsig() will not provide signal information. 
 */
int drmaa_wifsignaled(int *signaled, int stat, char *error_diagnosis, size_t error_diag_len);

/* 
 * If the OUT parameter 'signaled' of drmaa_wifsignaled(stat) is
 * non-zero, this function evaluates into signal a string representation of the signal
 * that caused the termination of the job. For signals declared by POSIX, the symbolic
 * names are returned (e.g., SIGABRT, SIGALRM).
 * For signals not declared by POSIX, any other string may be returned. 
 */
int drmaa_wtermsig(char *signal, size_t signal_len, int stat, char *error_diagnosis, size_t error_diag_len);

/* 
 * If the OUT parameter 'signaled' of drmaa_wifsignaled(stat) is
 * non-zero, this function evaluates into 'core_dumped' a non-zero value
 * if a core image of the terminated job was created. 
 */
int drmaa_wcoredump(int *core_dumped, int stat, char *error_diagnosis, size_t error_diag_len);

/* 
 * Evaluates into 'aborted' a non-zero value if 'stat'
 * was returned for a job that ended before entering the running state. 
 */
int drmaa_wifaborted(int *aborted, int stat, char *error_diagnosis, size_t error_diag_len);



/* 
 * Get the program status of the job identified by 'job_id'.
 * The possible values returned in 'remote_ps' and their meanings are:
 * DRMAA_PS_UNDETERMINED = 00H : process status cannot be determined,
 * DRMAA_PS_QUEUED_ACTIVE = 10H : job is queued and active,
 * DRMAA_PS_SYSTEM_ON_HOLD = 11H : job is queued and in system hold,
 * DRMAA_PS_USER_ON_HOLD = 12H : job is queued and in user hold,
 * DRMAA_PS_USER_SYSTEM_ON_HOLD = 13H : job is queued and in user and system hold,
 * DRMAA_PS_RUNNING = 20H : job is running,
 * DRMAA_PS_SYSTEM_SUSPENDED = 21H : job is system suspended,
 * DRMAA_PS_USER_SUSPENDED = 22H : job is user suspended,
 * DRMAA_PS_USER_SYSTEM_SUSPENDED = 23H : job is user and system suspended,
 * DRMAA_PS_DONE = 30H : job finished normally, and
 * DRMAA_PS_FAILED = 40H : job finished, but failed.
 */
int drmaa_job_ps( const char *job_id, int *remote_ps, char *error_diagnosis, size_t error_diag_len);

/* ------------------- auxiliary routines ------------------- */
 
/*
 * Get the error message text associated with the errno number. 
 */
const char *drmaa_strerror(int drmaa_errno);

/* 
 * Current contact information for DRM system (string)
 */ 
int drmaa_get_contact(char *contact, size_t contact_len, char *error_diagnosis, size_t error_diag_len);

/* 
 * OUT major - major version number (non-negative integer)
 * OUT minor - minor version number (non-negative integer)
 * Returns the major and minor version numbers of the DRMAA library;
 * for DRMAA 1.0, 'major' is 1 and 'minor' is 0. 
 */
int drmaa_version(unsigned int *major, unsigned int *minor, char *error_diagnosis, size_t error_diag_len);


/* 
 * returns DRM system implementation information
 * Output (string) is implementation dependent and could contain the DRM system and the
 * implementation vendor as its parts.
 */
int drmaa_get_DRM_system(char *drm_system, size_t drm_system_len, char *error_diagnosis, size_t error_diag_len);

int drmaa_get_DRMAA_implementation(char *drmaa_implementation, size_t drmaa_implementation_len, char *error_diagnosis, size_t error_diag_len);

#ifdef  __cplusplus
}
#endif

#endif /* __DRMAA_H */
