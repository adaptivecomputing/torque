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

void set_array_depend_holds(job_array *pa);

/* static void post_doq(struct work_task *pwt); */

/* static void alter_unreg(job *pjob, pbs_attribute *old, pbs_attribute *new); */

int depend_on_que(pbs_attribute *pattr, void *pjob, int mode);

/* static void post_doe(struct work_task *pwt); */

int depend_on_exec(job *pjob);

int depend_on_term(job *pjob);

/* static void release_cheapest(job *pjob, struct depend *pdep); */

/* static void set_depend_hold(job *pjob, pbs_attribute *pattr); */

void depend_clrrdy(job *pjob);

/* static struct depend *find_depend(int type, pbs_attribute *pattr); */

/* static struct depend *make_depend(int type, pbs_attribute *pattr); */

/* static int register_sync(struct depend *pdep, char *child, char *host, long cost); */

/* static int register_dep(pbs_attribute *pattr, struct batch_request *preq, int type, int *made); */

/* static int unregister_dep(pbs_attribute *pattr, struct batch_request *preq); */

/* static int unregister_sync(pbs_attribute *pattr, struct batch_request *preq); */

/* static struct depend_job *find_dependjob(struct depend *pdep, char *name); */

/* static struct depend_job *make_dependjob(struct depend *pdep, char *jobid, char *host); */

int decode_depend(pbs_attribute *patr, char *name, char *rescn, char *val, int perm);

/* static void cat_jobsvr(char **Dest, char *Src); */

/* static void fast_strcat(char **Dest, char *Src); */

/* static int dup_depend(pbs_attribute *pattr, struct depend *pd); */

int encode_depend(pbs_attribute *attr, tlist_head *phead, char *atname, char *rsname, int mode, int perm);

int set_depend(struct pbs_attribute *attr, struct pbs_attribute *new, enum batch_op op);

int comp_depend(struct pbs_attribute *attr, struct pbs_attribute *with);

void free_depend(struct pbs_attribute *attr);

/* static int build_depend(pbs_attribute *pattr, char *value); */

/* static void clear_depend(struct depend *pd, int type, int exist); */

/* static void del_depend(struct depend *pd); */

/* static void del_depend_job(struct depend_job *pdj); */

#endif /* _REQ_REGISTER_H */
