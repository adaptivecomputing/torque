#ifndef _SVR_JOBFUNC_H
#define _SVR_JOBFUNC_H
#include "license_pbs.h" /* See here for the software license */
#include <string>

/*Forward declarations*/
struct svr_job;
struct pbs_attribute;
struct resource;
struct resource_def;
struct pbs_queue;

char *get_variable(struct svr_job *pjob, const char *variable);

int chk_resc_limits(struct pbs_attribute *pattr, struct pbs_queue *pque, char *EMsg);

int svr_chkque(struct svr_job *pjob, struct pbs_queue *pque, char *hostname, int mtype, char *EMsg);

int job_set_wait(struct pbs_attribute *pattr, void *pjob, int mode);

const char *prefix_std_file(struct svr_job *pjob, std::string& ds, int key);

void set_resc_deflt(struct svr_job *pjob, struct pbs_attribute *ji_wattr, int has_queue_mutex);

void set_chkpt_deflt(struct svr_job *pjob, struct pbs_queue *pque);

void set_statechar(struct svr_job *pjob);


#ifndef NDEBUG
/* static void correct_ct(); */
#endif /* NDEBUG */

#endif /* _SVR_JOBFUNC_H */
