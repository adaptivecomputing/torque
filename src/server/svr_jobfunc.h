#ifndef _SVR_JOBFUNC_H
#define _SVR_JOBFUNC_H
#include "license_pbs.h" /* See here for the software license */

#include "pbs_job.h" /* job */
#include "attribute.h" /* pbs_attribute */
#include "resource.h" /* resource_def */
#include "queue.h" /* pbs_queue */
#include "dynamic_string.h"

int svr_enquejob(job *pjob, int has_sv_qs_mutex, int);

int svr_dequejob(char *job_id, int);

int svr_setjobstate(job *pjob, int newstate, int newsubstate, int);

void svr_evaljobstate(job *pjob, int *newstate, int *newsub, int forceeval);

char *get_variable(job *pjob, char *variable);

resource *get_resource(pbs_attribute *p_queattr, pbs_attribute *p_svrattr, resource_def *rscdf, int *fromQueue);

/* static int chk_svr_resc_limit(pbs_attribute *jobatr, pbs_queue *pque, int qtype, char *EMsg); */

int count_queued_jobs(pbs_queue *pque, char *user);

int chk_resc_limits(pbs_attribute *pattr, pbs_queue *pque, char *EMsg);

int svr_chkque(job *pjob, pbs_queue *pque, char *hostname, int mtype, char *EMsg);

/* static void job_wait_over(struct work_task *pwt); */

int job_set_wait(pbs_attribute *pattr, void *pjob, int mode);

/* static void default_std(job *pjob, int key, char *to); */

char *prefix_std_file(job *pjob, dynamic_string *ds, int key);

char *add_std_filename(job *pjob, char * path, int key, dynamic_string *ds);

void get_jobowner(char *from, char *to);

/* static void set_deflt_resc(pbs_attribute *jb, pbs_attribute *dflt); */

void set_resc_deflt(job *pjob, pbs_attribute *ji_wattr, int has_queue_mutex);

void set_chkpt_deflt(job *pjob, pbs_queue *pque);

void set_statechar(job *pjob);

/* static void eval_checkpoint(pbs_attribute *jobckp, pbs_attribute *queckp); */

#ifndef NDEBUG
/* static void correct_ct(); */
#endif /* NDEBUG */

#endif /* _SVR_JOBFUNC_H */
