#ifndef _SVR_JOBFUNC_H
#define _SVR_JOBFUNC_H
#include "license_pbs.h" /* See here for the software license */

/*Forward declarations*/
struct job;
struct pbs_attribute;
struct resource;
struct resource_def;
struct pbs_queue;
struct dynamic_string;

/*static int insert_into_alljobs_by_rank(struct all_jobs *aj, struct job *pjob, char *jobid);*/

int svr_enquejob(struct job *pjob, int has_sv_qs_mutex, int);

int svr_dequejob(char *job_id, int);

int svr_setjobstate(struct job *pjob, int newstate, int newsubstate, int);

void svr_evaljobstate(struct job *pjob, int *newstate, int *newsub, int forceeval);

char *get_variable(struct job *pjob, char *variable);

/* static struct resource *get_resource(struct pbs_attribute *p_queattr, struct pbs_attribute *p_svrattr, struct resource_def *rscdf, int *fromQueue);*/

/* static int chk_svr_resc_limit(struct pbs_attribute *jobatr, struct pbs_queue *pque, int qtype, char *EMsg); */

/* static int count_queued_jobs(struct pbs_queue *pque, char *user);*/

int chk_resc_limits(struct pbs_attribute *pattr, struct pbs_queue *pque, char *EMsg);

int svr_chkque(struct job *pjob, struct pbs_queue *pque, char *hostname, int mtype, char *EMsg);

/* static void job_wait_over(struct work_task *pwt); */

int job_set_wait(struct pbs_attribute *pattr, void *pjob, int mode);

/* static void default_std(struct job *pjob, int key, char *to); */

char *prefix_std_file(struct job *pjob, struct dynamic_string *ds, int key);

char *add_std_filename(struct job *pjob, char * path, int key, struct dynamic_string *ds);

void get_jobowner(char *from, char *to);

/* static void set_deflt_resc(struct pbs_attribute *jb, struct pbs_attribute *dflt); */

void set_resc_deflt(struct job *pjob, struct pbs_attribute *ji_wattr, int has_queue_mutex);

void set_chkpt_deflt(struct job *pjob, struct pbs_queue *pque);

void set_statechar(struct job *pjob);

/* static void eval_checkpoint(struct pbs_attribute *jobckp, struct pbs_attribute *queckp); */

#ifndef NDEBUG
/* static void correct_ct(); */
#endif /* NDEBUG */

#endif /* _SVR_JOBFUNC_H */
