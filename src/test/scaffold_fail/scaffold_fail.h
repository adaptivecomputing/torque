/*
 * scaffold_fail.h
 *
 *  Created on: Jan 24, 2013
 *      Author: bdaw
 */

#include<stdlib.h>
#include<stdio.h>
#include<pbs_config.h>

#include "license_pbs.h"
#include "list_link.h"
#include "portability.h"
#include "pbs_job.h"
#include "queue.h"
#include "batch_request.h"
#include "resource.h"
#include <time.h> /* timeval */
#include <pthread.h> /* pthread_mutex_t */
#include "attribute.h" /* attribute_def, svrattrl, pbs_attribute */
#include "net_connect.h" /* pbs_net_t, conn_type */
#include "server_limits.h" /* RECOV_WARM */
#include "work_task.h" /* work _task, work_type, all_tasks */
#include "server.h" /* server */
#include "sched_cmds.h" /* SCH_SCHEDULE_NULL */
#include "array.h" /* job_array */
#include "pbs_ifl.h" /* batch_op */
#include "array.h" /* ArrayEventsEnum */
#include "pbs_nodes.h" /* pbsnode */
#include "queue.h" /* pbs_queue */
#include "mutex_mgr.hpp"




