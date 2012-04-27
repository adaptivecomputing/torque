#include "license_pbs.h" /* See here for the software license */
#ifndef _REQ_GPUCTRL_H
#define _REQ_GPUCTRL_H

#include "batch_request.h" /* batch_request */
#include "work_task.h" /* work_task */

int req_gpuctrl_svr(struct batch_request *preq);

#ifdef NVIDIA_GPUS
void process_gpu_request_reply(struct work_task *pwt);
#endif /* NVIDIA_GPUS */

#endif /* _REQ_GPUCTRL_H */
