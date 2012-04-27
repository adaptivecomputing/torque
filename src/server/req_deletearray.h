#include "license_pbs.h" /* See here for the software license */
#ifndef _REQ_DELETEARRAY_H
#define _REQ_DELETEARRAY_H

#include "work_task.h" /* work_task */
#include "batch_request.h" /* batch_request */

int attempt_delete(void *j);

int req_deletearray(struct batch_request *preq);

void array_delete_wt(struct work_task *ptask);

#endif /* _REQ_DELETEARRAY_H */
