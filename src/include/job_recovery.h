#include "license_pbs.h" /* See here for the software license */
#ifndef JOB_RECOV_H
#define JOB_RECOV_H

#include "pbs_job.h" /* job */
#include <libxml/parser.h>
#include <libxml/tree.h>

#define JOB_TAG       "job"
#define ATTRIB_TAG    "attributes"
#define VERSION_TAG   "version"
#define STATE_TAG     "state"
#define SUBSTATE_TAG  "substate"
#define SRV_FLAGS_TAG "server_flags"
#define STIME_TAG     "start_time"
#define JOBID_TAG     "jobid"
#define FPREFIX_TAG   "fileprefix"
#define QUEUE_TAG     "queue"
#define DST_QUEUE     "destination_queue"
#define REC_TYPE_TAG  "record_type"
#define FROM_HOST_TAG "from_host"
#define FROM_SOCK_TAG "from_socket"
#define SCRT_SIZE_TAG "script_size"
#define MOM_ADDR_TAG  "mom_address"
#define MOM_PORT_TAG  "mom_port"
#define MOM_RPORT_TAG "mom_rmport"
#define QUE_TIME_TAG  "queue_time"
#define RQUE_TIME_TAG "requeue_time"
#define SVR_ADDR_TAG  "svr_addr"
#define EXIT_STAT_TAG "exit_status"
#define EXEC_UID_TAG  "execution_uid"
#define EXEC_GID_TAG  "execution_gid"
#define STDOUT_TAG    "socket_stdout"
#define STDERR_TAG    "socket_stderr"
#define TASKID_TAG    "taskid"
#define NODEID_TAG    "nodeid"
#define AL_FLAGS_ATTR "flags"

#endif // JOB_RECOV_H
