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
PbsErrClient(PBSE_FLOOR = 15000,     (char *)"no error") /* also works as no error */
/* Unknown Job Identifier */
PbsErrClient(PBSE_UNKJOBID,          (char *)"Unknown Job Id Error")
/* Undefined Attribute */
PbsErrClient(PBSE_NOATTR,            (char *)"Undefined attribute ")
/* attempt to set READ ONLY attribute */
PbsErrClient(PBSE_ATTRRO,            (char *)"Cannot set attribute, read only or insufficient permission ")
/* Invalid request */
PbsErrClient(PBSE_IVALREQ,           (char *)"Invalid request")
/* Unknown batch request */
PbsErrClient(PBSE_UNKREQ,            (char *)"Unknown request")
/* Too many submit retries */
PbsErrClient(PBSE_TOOMANY,           (char *)"Too many submit retries")
/* No permission */
PbsErrClient(PBSE_PERM,              (char *)"Unauthorized Request ")
/* pbs_iff not found */
PbsErrClient(PBSE_IFF_NOT_FOUND,     (char *)"trqauthd unable to authenticate. User or PID of calling process not found or does not match")
/* munge not found */
PbsErrClient(PBSE_MUNGE_NOT_FOUND,   (char *)"munge executable not found, unable to authenticate")
/* 15010 */
/* access from host not allowed */
PbsErrClient(PBSE_BADHOST,           (char *)"Access from host not allowed, or unknown host")
/* job already exists */
PbsErrClient(PBSE_JOBEXIST,          (char *)"Job with requested ID already exists")
/* system error occurred */
PbsErrClient(PBSE_SYSTEM,            (char *)" System error: ")
/* internal server error occurred */
PbsErrClient(PBSE_INTERNAL,          (char *)"PBS server internal error")
/* parent job of dependent in rte que */
PbsErrClient(PBSE_REGROUTE,          (char *)"Dependent parent job currently in routing queue")
/* unknown signal name */
PbsErrClient(PBSE_UNKSIG,            (char *)"Unknown/illegal signal name")
/* bad attribute value */
PbsErrClient(PBSE_BADATVAL,          (char *)"Illegal attribute or resource value for ")
/* Cannot modify attrib in run state */
PbsErrClient(PBSE_MODATRRUN,         (char *)"Cannot modify attribute while job running ")
/* request invalid for job state */
PbsErrClient(PBSE_BADSTATE,          (char *)"Request invalid for state of job")
/* gap filler */
PbsErrClient(PBSE_GAP017,            (char*)0)
/* 15020 */
/* Unknown queue name */
PbsErrClient(PBSE_UNKQUE,            (char *)"Unknown queue")
/* Invalid Credential in request */
PbsErrClient(PBSE_BADCRED,           (char *)"Invalid credential")
/* Expired Credential in request */
PbsErrClient(PBSE_EXPIRED,           (char *)"Expired credential")
/* Queue not enabled */
PbsErrClient(PBSE_QUNOENB,           (char *)"Queue is not enabled")
/* No access permission for queue */
PbsErrClient(PBSE_QACESS,            (char *)"Access to queue is denied")
/* Bad user - no password entry */
PbsErrClient(PBSE_BADUSER,           (char *)"Bad UID for job execution")
/* Max hop count exceeded */
PbsErrClient(PBSE_HOPCOUNT,          (char *)"Job routing over too many hops")
/* Queue already exists */
PbsErrClient(PBSE_QUEEXIST,          (char *)"Queue already exists")
/* incompatable queue attribute type */
PbsErrClient(PBSE_ATTRTYPE,          (char *)"Incompatible type")
/* Queue Busy (not empty) */
PbsErrClient(PBSE_QUEBUSY,           (char *)"Cannot delete busy queue")
/* 15030 */
/* Queue name too long */
PbsErrClient(PBSE_QUENBIG,           (char *)"Queue name too long")
/* Feature/function not supported */
PbsErrClient(PBSE_NOSUP,             (char *)"No support for requested service")
/* Cannot enable queue,needs add def */
PbsErrClient(PBSE_QUENOEN,           (char *)"Cannot enable queue, incomplete definition")
/* Protocol error */
PbsErrClient(PBSE_PROTOCOL,          (char *)"Batch protocol error")
/* Bad attribute list structure */
PbsErrClient(PBSE_BADATLST,          (char *)"Bad attribute list structure")
/* No free connections */
PbsErrClient(PBSE_NOCONNECTS,        (char *)"No free connections")
/* No server to connect to */
PbsErrClient(PBSE_NOSERVER,          (char *)"No server specified")
/* Unknown resource */
PbsErrClient(PBSE_UNKRESC,           (char *)"Unknown resource type ")
/* Job exceeds Queue resource limits */
PbsErrClient(PBSE_EXCQRESC,          (char *)"Job exceeds queue resource limits")
/* No Default Queue Defined */
PbsErrClient(PBSE_QUENODFLT,         (char *)"No default queue specified")
/* 15040 */
/* Job Not Rerunnable */
PbsErrClient(PBSE_NORERUN,           (char *)"job is not rerunnable")
/* Route rejected by all destinations */
PbsErrClient(PBSE_ROUTEREJ,          (char *)"Job rejected by all possible destinations (check syntax, queue resources, ...)")
/* Time in Route Queue Expired */
PbsErrClient(PBSE_ROUTEEXPD,         (char *)"Time in Route Queue Expired")
/* Request to MOM failed */
PbsErrClient(PBSE_MOMREJECT,         (char *)"Execution server rejected request")
/* (qsub) cannot access script file */
PbsErrClient(PBSE_BADSCRIPT,         (char *)"(qsub) cannot access script file")
/* Stage In of files failed */
PbsErrClient(PBSE_STAGEIN,           (char *)"Stage In of files failed")
/* Resources temporarily unavailable */
PbsErrClient(PBSE_RESCUNAV,          (char *)"Resource temporarily unavailable")
/* Bad Group specified */
PbsErrClient(PBSE_BADGRP,            (char *)"Bad GID for job execution")
/* Max number of jobs in queue */
PbsErrClient(PBSE_MAXQUED,           (char *)"Maximum number of jobs already in queue")
/* Checkpoint Busy, may be retries */
PbsErrClient(PBSE_CKPBSY,            (char *)"Checkpoint busy, may retry")
/* 15050 */
/* Limit exceeds allowable */
PbsErrClient(PBSE_EXLIMIT,           (char *)"Resource limit exceeds allowable")
/* Bad Account attribute value */
PbsErrClient(PBSE_BADACCT,           (char *)"Invalid Account")
/* Job already in exit state */
PbsErrClient(PBSE_ALRDYEXIT,         (char *)"Job already in exit state")
/* Job files not copied */
PbsErrClient(PBSE_NOCOPYFILE,        (char *)"Job files not copied")
/* unknown job id after clean init */
PbsErrClient(PBSE_CLEANEDOUT,        (char *)"unknown job id after clean init")
/* No Master in Sync Set */
PbsErrClient(PBSE_NOSYNCMSTR,        (char *)"No master found for sync job set")
/* Invalid dependency */
PbsErrClient(PBSE_BADDEPEND,         (char *)"Invalid Job Dependency")
/* Duplicate entry in List */
PbsErrClient(PBSE_DUPLIST,           (char *)"Duplicate entry in list ")
/* Bad DIS based Request Protocol */
PbsErrClient(PBSE_DISPROTO,          (char *)"Bad DIS based Request Protocol")
/* cannot execute there */
PbsErrClient(PBSE_EXECTHERE,         (char *)"Cannot execute at specified host because of checkpoint or stagein files")
/* 15060 */
/* sister rejected */
PbsErrClient(PBSE_SISREJECT,         (char *)"sister rejected")
/* sister could not communicate */
PbsErrClient(PBSE_SISCOMM,           (char *)"sister could not communicate")
/* req rejected -server shutting down */
PbsErrClient(PBSE_SVRDOWN,           (char *)"Request not allowed: Server shutting down")
/* not all tasks could checkpoint */
PbsErrClient(PBSE_CKPSHORT,          (char *)"not all tasks could checkpoint")
/* Named node is not in the list */
PbsErrClient(PBSE_UNKNODE,           (char *)"Unknown node ")
/* node-attribute not recognized */
PbsErrClient(PBSE_UNKNODEATR,        (char *)"Unknown node-attribute ")
/* Server has no node list */
PbsErrClient(PBSE_NONODES,           (char *)"Server has no node list")
/* Node name is too big */
PbsErrClient(PBSE_NODENBIG,          (char *)"Node name is too big")
/* Node name already exists */
PbsErrClient(PBSE_NODEEXIST,         (char *)"Node name already exists")
/* Bad node-attribute value */
PbsErrClient(PBSE_BADNDATVAL,        (char *)"Illegal value for ")
/* 15070 */
/* State values are mutually exclusive */
PbsErrClient(PBSE_MUTUALEX,          (char *)"Mutually exclusive values for ")
/* Error(s) during global modification of nodes */
PbsErrClient(PBSE_GMODERR,           (char *)"Modification failed for ")
/* could not contact Mom */
PbsErrClient(PBSE_NORELYMOM,         (char *)"Server could not connect to MOM")
/* no time-shared nodes */
PbsErrClient(PBSE_NOTSNODE,          (char *)"No time-share node available")
/* wrong job type (batch or interactive) */
PbsErrClient(PBSE_JOBTYPE,           (char *)"Wrong job type")
/* bad entry in ACL host list */
PbsErrClient(PBSE_BADACLHOST,        (char *)"Bad ACL entry in host list")
/* max number of jobs queued for user */
PbsErrClient(PBSE_MAXUSERQUED,       (char *)"Maximum number of jobs already in queue for user")
/* bad type in disallowed_types queue attribute */
PbsErrClient(PBSE_BADDISALLOWTYPE,   (char *)"Bad type in disallowed_types list")
/* interactive jobs not allowed in queue */
PbsErrClient(PBSE_NOINTERACTIVE,     (char *)"Queue does not allow interactive jobs")
/* batch jobs not allowed in queue */
PbsErrClient(PBSE_NOBATCH,           (char *)"Queue does not allow batch jobs")
/* 15080 */
/* rerunable jobs not allowed in queue */
PbsErrClient(PBSE_NORERUNABLE,       (char *)"Queue does not allow rerunable jobs")
/* nonrerunable jobs not allowed in queue */
PbsErrClient(PBSE_NONONRERUNABLE,    (char *)"Queue does not allow nonrerunable jobs")
/* unknown array id */
PbsErrClient(PBSE_UNKARRAYID,        (char *)"Unknown Array ID")
/* bad array request */
PbsErrClient(PBSE_BAD_ARRAY_REQ,     (char *)"Bad Job Array Request")
/* Bad file format for array */
PbsErrClient(PBSE_BAD_ARRAY_DATA,    (char *)"Bad data reading job array from file ")
/* Time out */
PbsErrClient(PBSE_TIMEOUT,           (char *)"Time out")
PbsErrClient(PBSE_JOBNOTFOUND,		 (char *)"Job not found")
/* fault tolerant jobs not allowed in queue */
PbsErrClient(PBSE_NOFAULTTOLERANT,   (char *)"Queue does not allow fault tolerant jobs")
/* only fault tolerant jobs allowed in queue */
PbsErrClient(PBSE_NOFAULTINTOLERANT, (char *)"Queue does not allow fault intolerant jobs")
PbsErrClient(PBSE_NOJOBARRAYS, (char *)"Queue does not allow job arrays")
/* 15090 */
PbsErrClient(PBSE_RELAYED_TO_MOM, (char *)"request was relayed to a MOM") /* */
PbsErrClient(PBSE_MEM_MALLOC, (char *)"Error allocating memory - out of memory")
PbsErrClient(PBSE_MUTEX, (char *)"Error allocating controling mutex (lock/unlock)")
PbsErrClient(PBSE_THREADATTR, (char *)"Error setting thread attributes")
PbsErrClient(PBSE_THREAD, (char *)"Error creating thread")
PbsErrClient(PBSE_SELECT, (char *)"Error in socket select")
PbsErrClient(PBSE_SOCKET_FAULT, (char *)"Unable to get connection to socket")
PbsErrClient(PBSE_SOCKET_WRITE, (char *)"Error writing data to socket")
PbsErrClient(PBSE_SOCKET_READ, (char *)"Error reading data from socket")
PbsErrClient(PBSE_SOCKET_CLOSE, (char *)"Socket close detected")
/* 15100 */
PbsErrClient(PBSE_SOCKET_LISTEN, (char *)"Error listening on socket")
PbsErrClient(PBSE_AUTH_INVALID, (char *)"Invalid auth type in request")
PbsErrClient(PBSE_NOT_IMPLEMENTED, (char *)"This functionality is not yet implemented")
PbsErrClient(PBSE_QUENOTAVAILABLE, (char *)"Queue is currently not available")
PbsErrClient(PBSE_TMPDIFFOWNER, (char *)"tmpdir owned by another user")
PbsErrClient(PBSE_TMPNOTDIR, (char *)"tmpdir exists but is not a directory")
PbsErrClient(PBSE_TMPNONAME, (char *)"tmpdir cannot be named for job")
PbsErrClient(PBSE_CANTOPENSOCKET, (char *)"cannot open demux sockets")
PbsErrClient(PBSE_CANTCONTACTSISTERS, (char *)"cannot send join job to all sisters")
PbsErrClient(PBSE_CANTCREATETMPDIR, (char *)"cannot create tmpdir for job")
/* 15110 */
PbsErrClient(PBSE_BADMOMSTATE, (char *)"Mom is down, cannot run job")
PbsErrClient(PBSE_SOCKET_INFORMATION, (char *)"socket information is not accessible")
PbsErrClient(PBSE_SOCKET_DATA, (char *)"data on socket does not process correctly")
PbsErrClient(PBSE_CLIENT_INVALID, (char *)"client is not allowed/trusted")
PbsErrClient(PBSE_PREMATURE_EOF, (char *)"Premature End of File")
PbsErrClient(PBSE_CAN_NOT_SAVE_FILE, (char *)"Error saving file")
PbsErrClient(PBSE_CAN_NOT_OPEN_FILE, (char *)"Error opening file")
PbsErrClient(PBSE_CAN_NOT_WRITE_FILE, (char *)"Error writing file")
PbsErrClient(PBSE_JOB_FILE_CORRUPT, (char *)"Job file corrupt")
PbsErrClient(PBSE_JOB_RERUN, (char *)"Job can not be rerun")
/* 15120 */
PbsErrClient(PBSE_CONNECT, (char *)"Can not establish connection")
PbsErrClient(PBSE_JOBWORKDELAY, (char *)"Job function must be temporarily delayed")
PbsErrClient(PBSE_BAD_PARAMETER, (char *)"Parameter of function was invalid")
PbsErrClient(PBSE_CONTINUE, (char *)"Continue processing on job. (Not an error)")
PbsErrClient(PBSE_JOBSUBSTATE, (char *)"Current sub state does not allow trasaction.")
PbsErrClient(PBSE_CAN_NOT_MOVE_FILE, (char *)"Error moving file")
PbsErrClient(PBSE_JOB_RECYCLED,      (char *)"Job is being recycled")
PbsErrClient(PBSE_JOB_ALREADY_IN_QUEUE, (char *)"Job is already in destination queue.")
PbsErrClient(PBSE_INVALID_MUTEX, (char *)"Mutex is NULL or otherwise invalid")
PbsErrClient(PBSE_MUTEX_ALREADY_LOCKED, (char *)"The mutex is already locked by this object")
/* 15130 */
PbsErrClient(PBSE_MUTEX_ALREADY_UNLOCKED, (char *)"The mutex has already been unlocked by this object")
PbsErrClient(PBSE_INVALID_SYNTAX, (char *)"Command syntax invalid")
PbsErrClient(PBSE_NODE_DOWN, (char *)"A node is down. Check the MOM and host")
PbsErrClient(PBSE_SERVER_NOT_FOUND, (char *)"Could not connect to batch server")
PbsErrClient(PBSE_SERVER_BUSY, (char *)"Server busy. Currently no available threads")
PbsErrClient(PBSE_CLIENT_CONN_NOT_FOUND, (char *)"Client connection not found. trqauthd unable to authorize user. Possible transient failure. Please try again")
PbsErrClient(PBSE_CANNOT_RESOLVE, (char *)"Could not resolve host name")
PbsErrClient(PBSE_DOMAIN_SOCKET_FAULT, (char *)"could not connect to trqauthd")
PbsErrClient(PBSE_JOB_NOT_IN_QUEUE, (char *)"Job not found in queue.")
PbsErrClient(PBSE_LOGIN_BUSY, (char *)"Login node is currently too busy to run a job")

/* pbs client errors ceiling (max_client_err + 1) */
PbsErrClient(PBSE_CEILING,           (char*)0)
#endif

#ifdef PbsErrRm
/* pbs rm errors floor (min_rm_err - 1) */
PbsErrRm(PBSE_RMFLOOR = 15200,       (char*)0)
/* resource unknown */
PbsErrRm(PBSE_RMUNKNOWN,             (char *)"resource unknown")
/* parameter could not be used */
PbsErrRm(PBSE_RMBADPARAM,            (char *)"parameter could not be used")
/* a parameter needed did not exist */
PbsErrRm(PBSE_RMNOPARAM,             (char *)"a parameter needed did not exist")
/* something specified didn't exist */
PbsErrRm(PBSE_RMEXIST,               (char *)"something specified didn't exist")
/* a system error occured */
PbsErrRm(PBSE_RMSYSTEM,              (char *)"a system error occured")
/* only part of reservation made */
PbsErrRm(PBSE_RMPART,                (char *)"only part of reservation made")
/* pbs rm errors ceiling (max_rm_err + 1) */
PbsErrRm(PBSE_RMCEILING,             (char*)0)
#endif

