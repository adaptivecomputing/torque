#ifndef _REQ_SELECT_H
#define _REQ_SELECT_H
#include "license_pbs.h" /* See here for the software license */

#include "attribute.h" /* pbs_attribute */
#include "batch_request.h" /* batch_request */

int comp_checkpoint(pbs_attribute *attr, pbs_attribute *with);

int req_selectjobs(struct batch_request *preq);


#endif /* _REQ_SELECT_H */
