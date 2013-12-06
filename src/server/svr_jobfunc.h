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

int svr_enquejob(struct job *pjob, int has_sv_qs_mutex, int, bool);

int svr_setjobstate(struct job *pjob, int newstate, int newsubstate, int);

char *get_variable(struct job *pjob, const char *variable);

int chk_resc_limits(struct pbs_attribute *pattr, struct pbs_queue *pque, char *EMsg);

int svr_chkque(struct job *pjob, struct pbs_queue *pque, char *hostname, int mtype, char *EMsg);

int job_set_wait(struct pbs_attribute *pattr, void *pjob, int mode);

char *prefix_std_file(struct job *pjob, struct dynamic_string *ds, int key);

char *add_std_filename(struct job *pjob, char * path, int key, struct dynamic_string *ds);

void get_jobowner(char *from, char *to);

void set_resc_deflt(struct job *pjob, struct pbs_attribute *ji_wattr, int has_queue_mutex);

void set_chkpt_deflt(struct job *pjob, struct pbs_queue *pque);

void set_statechar(struct job *pjob);


#ifndef NDEBUG
/* static void correct_ct(); */
#endif /* NDEBUG */

#endif /* _SVR_JOBFUNC_H */
