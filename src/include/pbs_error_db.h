/** @file This file contains the database of PBS error ids and messages
*
* HOWTO USE THIS
*
* Add new error:
*  - choose the appropriate section (client errors, rm errors)
*  - add new line before the ceiling, the error will be automatically numbered
*
* Remove error:
*  - rewrite the error into a gap (PBSE_GAPxxx or PBSE_RMGAPxxx) with (char*)0
*    as text part (see PBSE_GAP017 for example)
*
* Add new category:
*  - use the same structure for the new category
*  - create new name PbsErrSomething
*  - don't forget the ceiling and floor (and don't forget to give floor a value)
*  - create respective generators in pbs_error.h and pbs_messages.c
*
* @see pbs_error.h and pbs_messages.c for additional info
*
* @note NULL strings should be (const char*)0, but that is not possible right now
*/

#ifdef PbsErrClient
/* 15000 */
/* pbs client errors floor (min_client_err - 1) */
PbsErrClient(PBSE_FLOOR = 15000,     "no error") /* also works as no error */
/* Unknown Job Identifier */
PbsErrClient(PBSE_UNKJOBID,          "Unknown Job Id Error")
/* Undefined Attribute */
PbsErrClient(PBSE_NOATTR,            "Undefined attribute ")
/* attempt to set READ ONLY attribute */
PbsErrClient(PBSE_ATTRRO,            "Cannot set attribute, read only or insufficient permission ")
/* Invalid request */
PbsErrClient(PBSE_IVALREQ,           "Invalid request")
/* Unknown batch request */
PbsErrClient(PBSE_UNKREQ,            "Unknown request")
/* Too many submit retries */
PbsErrClient(PBSE_TOOMANY,           "Too many submit retries")
/* No permission */
PbsErrClient(PBSE_PERM,              "Unauthorized Request ")
/* pbs_iff not found */
PbsErrClient(PBSE_IFF_NOT_FOUND,     "pbs_iff command not found, unable to authenticate")
/* munge not found */
PbsErrClient(PBSE_MUNGE_NOT_FOUND,   "munge executable not found, unable to authenticate")
/* 15010 */
/* access from host not allowed */
PbsErrClient(PBSE_BADHOST,           "Access from host not allowed, or unknown host")
/* job already exists */
PbsErrClient(PBSE_JOBEXIST,          "Job with requested ID already exists")
/* system error occurred */
PbsErrClient(PBSE_SYSTEM,            " System error: ")
/* internal server error occurred */
PbsErrClient(PBSE_INTERNAL,          "PBS server internal error")
/* parent job of dependent in rte que */
PbsErrClient(PBSE_REGROUTE,          "Dependent parent job currently in routing queue")
/* unknown signal name */
PbsErrClient(PBSE_UNKSIG,            "Unknown/illegal signal name")
/* bad attribute value */
PbsErrClient(PBSE_BADATVAL,          "Illegal attribute or resource value for ")
/* Cannot modify attrib in run state */
PbsErrClient(PBSE_MODATRRUN,         "Cannot modify attribute while job running ")
/* request invalid for job state */
PbsErrClient(PBSE_BADSTATE,          "Request invalid for state of job")
/* gap filler */
PbsErrClient(PBSE_GAP017,            (char*)0)
/* 15020 */
/* Unknown queue name */
PbsErrClient(PBSE_UNKQUE,            "Unknown queue")
/* Invalid Credential in request */
PbsErrClient(PBSE_BADCRED,           "Invalid credential")
/* Expired Credential in request */
PbsErrClient(PBSE_EXPIRED,           "Expired credential")
/* Queue not enabled */
PbsErrClient(PBSE_QUNOENB,           "Queue is not enabled")
/* No access permission for queue */
PbsErrClient(PBSE_QACESS,            "Access to queue is denied")
/* Bad user - no password entry */
PbsErrClient(PBSE_BADUSER,           "Bad UID for job execution")
/* Max hop count exceeded */
PbsErrClient(PBSE_HOPCOUNT,          "Job routing over too many hops")
/* Queue already exists */
PbsErrClient(PBSE_QUEEXIST,          "Queue already exists")
/* incompatable queue attribute type */
PbsErrClient(PBSE_ATTRTYPE,          "Incompatible type")
/* Queue Busy (not empty) */
PbsErrClient(PBSE_QUEBUSY,           "Cannot delete busy queue")
/* 15030 */
/* Queue name too long */
PbsErrClient(PBSE_QUENBIG,           "Queue name too long")
/* Feature/function not supported */
PbsErrClient(PBSE_NOSUP,             "No support for requested service")
/* Cannot enable queue,needs add def */
PbsErrClient(PBSE_QUENOEN,           "Cannot enable queue, incomplete definition")
/* Protocol error */
PbsErrClient(PBSE_PROTOCOL,          "Batch protocol error")
/* Bad attribute list structure */
PbsErrClient(PBSE_BADATLST,          "Bad attribute list structure")
/* No free connections */
PbsErrClient(PBSE_NOCONNECTS,        "No free connections")
/* No server to connect to */
PbsErrClient(PBSE_NOSERVER,          "No server specified")
/* Unknown resource */
PbsErrClient(PBSE_UNKRESC,           "Unknown resource type ")
/* Job exceeds Queue resource limits */
PbsErrClient(PBSE_EXCQRESC,          "Job exceeds queue resource limits")
/* No Default Queue Defined */
PbsErrClient(PBSE_QUENODFLT,         "No default queue specified")
/* 15040 */
/* Job Not Rerunnable */
PbsErrClient(PBSE_NORERUN,           "job is not rerunnable")
/* Route rejected by all destinations */
PbsErrClient(PBSE_ROUTEREJ,          "Job rejected by all possible destinations (check syntax, queue resources, ...)")
/* Time in Route Queue Expired */
PbsErrClient(PBSE_ROUTEEXPD,         "Time in Route Queue Expired")
/* Request to MOM failed */
PbsErrClient(PBSE_MOMREJECT,         "Execution server rejected request")
/* (qsub) cannot access script file */
PbsErrClient(PBSE_BADSCRIPT,         "(qsub) cannot access script file")
/* Stage In of files failed */
PbsErrClient(PBSE_STAGEIN,           "Stage In of files failed")
/* Resources temporarily unavailable */
PbsErrClient(PBSE_RESCUNAV,          "Resource temporarily unavailable")
/* Bad Group specified */
PbsErrClient(PBSE_BADGRP,            "Bad GID for job execution")
/* Max number of jobs in queue */
PbsErrClient(PBSE_MAXQUED,           "Maximum number of jobs already in queue")
/* Checkpoint Busy, may be retries */
PbsErrClient(PBSE_CKPBSY,            "Checkpoint busy, may retry")
/* 15050 */
/* Limit exceeds allowable */
PbsErrClient(PBSE_EXLIMIT,           "Resource limit exceeds allowable")
/* Bad Account attribute value */
PbsErrClient(PBSE_BADACCT,           "Invalid Account")
/* Job already in exit state */
PbsErrClient(PBSE_ALRDYEXIT,         "Job already in exit state")
/* Job files not copied */
PbsErrClient(PBSE_NOCOPYFILE,        "Job files not copied")
/* unknown job id after clean init */
PbsErrClient(PBSE_CLEANEDOUT,        "unknown job id after clean init")
/* No Master in Sync Set */
PbsErrClient(PBSE_NOSYNCMSTR,        "No master found for sync job set")
/* Invalid dependency */
PbsErrClient(PBSE_BADDEPEND,         "Invalid Job Dependency")
/* Duplicate entry in List */
PbsErrClient(PBSE_DUPLIST,           "Duplicate entry in list ")
/* Bad DIS based Request Protocol */
PbsErrClient(PBSE_DISPROTO,          "Bad DIS based Request Protocol")
/* cannot execute there */
PbsErrClient(PBSE_EXECTHERE,         "Cannot execute at specified host because of checkpoint or stagein files")
/* 15060 */
/* sister rejected */
PbsErrClient(PBSE_SISREJECT,         "sister rejected")
/* sister could not communicate */
PbsErrClient(PBSE_SISCOMM,           "sister could not communicate")
/* req rejected -server shutting down */
PbsErrClient(PBSE_SVRDOWN,           "Request not allowed: Server shutting down")
/* not all tasks could checkpoint */
PbsErrClient(PBSE_CKPSHORT,          "not all tasks could checkpoint")
/* Named node is not in the list */
PbsErrClient(PBSE_UNKNODE,           "Unknown node ")
/* node-attribute not recognized */
PbsErrClient(PBSE_UNKNODEATR,        "Unknown node-attribute ")
/* Server has no node list */
PbsErrClient(PBSE_NONODES,           "Server has no node list")
/* Node name is too big */
PbsErrClient(PBSE_NODENBIG,          "Node name is too big")
/* Node name already exists */
PbsErrClient(PBSE_NODEEXIST,         "Node name already exists")
/* Bad node-attribute value */
PbsErrClient(PBSE_BADNDATVAL,        "Illegal value for ")
/* 15070 */
/* State values are mutually exclusive */
PbsErrClient(PBSE_MUTUALEX,          "Mutually exclusive values for ")
/* Error(s) during global modification of nodes */
PbsErrClient(PBSE_GMODERR,           "Modification failed for ")
/* could not contact Mom */
PbsErrClient(PBSE_NORELYMOM,         "Server could not connect to MOM")
/* no time-shared nodes */
PbsErrClient(PBSE_NOTSNODE,          "No time-share node available")
/* wrong job type (batch or interactive) */
PbsErrClient(PBSE_JOBTYPE,           "Wrong job type")
/* bad entry in ACL host list */
PbsErrClient(PBSE_BADACLHOST,        "Bad ACL entry in host list")
/* max number of jobs queued for user */
PbsErrClient(PBSE_MAXUSERQUED,       "Maximum number of jobs already in queue for user")
/* bad type in disallowed_types queue attribute */
PbsErrClient(PBSE_BADDISALLOWTYPE,   "Bad type in disallowed_types list")
/* interactive jobs not allowed in queue */
PbsErrClient(PBSE_NOINTERACTIVE,     "Queue does not allow interactive jobs")
/* batch jobs not allowed in queue */
PbsErrClient(PBSE_NOBATCH,           "Queue does not allow batch jobs")
/* 15080 */
/* rerunable jobs not allowed in queue */
PbsErrClient(PBSE_NORERUNABLE,       "Queue does not allow rerunable jobs")
/* nonrerunable jobs not allowed in queue */
PbsErrClient(PBSE_NONONRERUNABLE,    "Queue does not allow nonrerunable jobs")
/* unknown array id */
PbsErrClient(PBSE_UNKARRAYID,        "Unknown Array ID")
/* bad array request */
PbsErrClient(PBSE_BAD_ARRAY_REQ,     "Bad Job Array Request")
/* Bad file format for array */
PbsErrClient(PBSE_BAD_ARRAY_DATA,    "Bad data reading job array from file ")
/* Time out */
PbsErrClient(PBSE_TIMEOUT,           "Time out")
PbsErrClient(PBSE_JOBNOTFOUND,		 "Job not found")
/* fault tolerant jobs not allowed in queue */
PbsErrClient(PBSE_NOFAULTTOLERANT,   "Queue does not allow fault tolerant jobs")
/* only fault tolerant jobs allowed in queue */
PbsErrClient(PBSE_NOFAULTINTOLERANT, "Queue does not allow fault intolerant jobs")
PbsErrClient(PBSE_NOJOBARRAYS, "Queue does not allow job arrays")
/* 15090 */
PbsErrClient(PBSE_RELAYED_TO_MOM, "request was relayed to a MOM") /* */
PbsErrClient(PBSE_MEM_MALLOC, "Error allocating memory - out of memory")
PbsErrClient(PBSE_MUTEX, "Error allocating controling mutex (lock/unlock)")
PbsErrClient(PBSE_THREADATTR, "Error setting thread attributes")
PbsErrClient(PBSE_THREAD, "Error creating thread")
PbsErrClient(PBSE_SELECT, "Error in socket select")
PbsErrClient(PBSE_SOCKET_FAULT, "Error getting connection to socket")
PbsErrClient(PBSE_SOCKET_WRITE, "Error writing data to socket")
PbsErrClient(PBSE_SOCKET_READ, "Error reading data from socket")
PbsErrClient(PBSE_SOCKET_CLOSE, "Socket close detected")
/* 15100 */
PbsErrClient(PBSE_SOCKET_LISTEN, "Error listening on socket")
PbsErrClient(PBSE_AUTH_INVALID, "Invalid auth type in request")
PbsErrClient(PBSE_NOT_IMPLEMENTED, "This functionality is not yet implemented")
PbsErrClient(PBSE_QUENOTAVAILABLE, "Queue is currently not available")
PbsErrClient(PBSE_TMPDIFFOWNER, "tmpdir owned by another user")
PbsErrClient(PBSE_TMPNOTDIR, "tmpdir exists but is not a directory")
PbsErrClient(PBSE_TMPNONAME, "tmpdir cannot be named for job")
PbsErrClient(PBSE_CANTOPENSOCKET, "cannot open demux sockets")
PbsErrClient(PBSE_CANTCONTACTSISTERS, "cannot send join job to all sisters")
PbsErrClient(PBSE_CANTCREATETMPDIR, "cannot create tmpdir for job")
/* 15110 */
PbsErrClient(PBSE_BADMOMSTATE, "Mom is down, cannot run job")
PbsErrClient(PBSE_SOCKET_INFORMATION, "socket information is not accessible")
PbsErrClient(PBSE_SOCKET_DATA, "data on socket does not process correctly")
PbsErrClient(PBSE_CLIENT_INVALID, "client is not allowed/trusted")
PbsErrClient(PBSE_PREMATURE_EOF, "Premature End of File")
PbsErrClient(PBSE_CAN_NOT_SAVE_FILE, "Error saving file")
PbsErrClient(PBSE_CAN_NOT_OPEN_FILE, "Error opening file")
PbsErrClient(PBSE_CAN_NOT_WRITE_FILE, "Error writing file")
PbsErrClient(PBSE_JOB_FILE_CORRUPT, "Job file corrupt")
PbsErrClient(PBSE_JOB_RERUN, "Job can not be rerun")
/* 15120 */
PbsErrClient(PBSE_CONNECT, "Can not establish connection")
PbsErrClient(PBSE_JOBWORKDELAY, "Job function must be temporarily delayed")
PbsErrClient(PBSE_BAD_PARAMETER, "Parameter of function was invalid")
PbsErrClient(PBSE_CONTINUE, "Continue processing on job. (Not an error)")
PbsErrClient(PBSE_JOBSUBSTATE, "Current sub state does not allow trasaction.")
PbsErrClient(PBSE_CAN_NOT_MOVE_FILE, "Error moving file")
PbsErrClient(PBSE_JOB_RECYCLED,      "Job is being recycled")
PbsErrClient(PBSE_JOB_ALREADY_IN_QUEUE, "Job is already in destination queue.")


/* pbs client errors ceiling (max_client_err + 1) */
PbsErrClient(PBSE_CEILING,           (char*)0)
#endif

#ifdef PbsErrRm
/* pbs rm errors floor (min_rm_err - 1) */
PbsErrRm(PBSE_RMFLOOR = 15200,       (char*)0)
/* resource unknown */
PbsErrRm(PBSE_RMUNKNOWN,             "resource unknown")
/* parameter could not be used */
PbsErrRm(PBSE_RMBADPARAM,            "parameter could not be used")
/* a parameter needed did not exist */
PbsErrRm(PBSE_RMNOPARAM,             "a parameter needed did not exist")
/* something specified didn't exist */
PbsErrRm(PBSE_RMEXIST,               "something specified didn't exist")
/* a system error occured */
PbsErrRm(PBSE_RMSYSTEM,              "a system error occured")
/* only part of reservation made */
PbsErrRm(PBSE_RMPART,                "only part of reservation made")
/* pbs rm errors ceiling (max_rm_err + 1) */
PbsErrRm(PBSE_RMCEILING,             (char*)0)
#endif

