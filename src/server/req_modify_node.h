#ifndef _REQ_MODIFY_NODE_H
#define _REQ_MODIFY_NODE_H
#include "license_pbs.h" /* See here for the software license */

#include "attribute.h" /* pbs_attribute, attribute_def, svrattrl, batch_op */
#include "batch_request.h" /* batch_request */
#include "work_task.h" /* work_task */

int req_modify_node(struct batch_request *preq);

#endif /* _REQ_MODIFY_NODE_H */
