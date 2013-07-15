#ifndef _REQ_REGISTER_H
#define _REQ_REGISTER_H
#include "license_pbs.h" /* See here for the software license */

#include "batch_request.h" /* batch_request */
#include "array.h" /* job_array */
#include "attribute.h" /* pbs_attribute, batch_op */
#include "pbs_job.h" /* job */
#include "list_link.h" /* tlist_head */

int req_register(struct batch_request *preq);

int req_registerarray(struct batch_request *preq);

int register_array_depend(job_array *pa, struct batch_request *preq, int type, int num_jobs);

int depend_on_exec(job *pjob);

void depend_clrrdy(job *pjob);

int encode_depend(pbs_attribute *attr, tlist_head *phead, const char *atname, const char *rsname, int mode, int perm);

int set_depend(struct pbs_attribute *attr, struct pbs_attribute *newAttr, enum batch_op op);

int comp_depend(struct pbs_attribute *attr, struct pbs_attribute *with);

void free_depend(struct pbs_attribute *attr);

#endif /* _REQ_REGISTER_H */
