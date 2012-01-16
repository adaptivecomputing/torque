#include "license_pbs.h" /* See here for the software license */
#ifndef _REQ_GPUCTRL_H
#define _REQ_GPUCTRL_H

#include "batch_request.h" /* batch_request */
#include "work_task.h" /* work_task */

void *req_gpuctrl_svr(void *vp);

#ifdef NVIDIA_GPUS
static void process_gpu_request_reply(struct work_task *pwt);
#endif /* NVIDIA_GPUS */

#endif /* _REQ_GPUCTRL_H */
