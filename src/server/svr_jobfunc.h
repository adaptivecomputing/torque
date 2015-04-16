#ifndef _SVR_JOBFUNC_H
#define _SVR_JOBFUNC_H
#include "license_pbs.h" /* See here for the software license */
#include <string>

/*Forward declarations*/
struct job;
struct pbs_attribute;
struct resource;
struct resource_def;
struct pbs_queue;

char *get_variable(struct job *pjob, const char *variable);

int chk_resc_limits(struct pbs_attribute *pattr, struct pbs_queue *pque, char *EMsg);

int svr_chkque(struct job *pjob, struct pbs_queue *pque, char *hostname, int mtype, char *EMsg);

int job_set_wait(struct pbs_attribute *pattr, void *pjob, int mode);

const char *prefix_std_file(struct job *pjob, std::string& ds, int key);

const char *add_std_filename(struct job *pjob, char * path, int key, std::string& ds);

void get_jobowner(char *from, char *to);

void set_resc_deflt(struct job *pjob, struct pbs_attribute *ji_wattr, int has_queue_mutex);

void set_chkpt_deflt(struct job *pjob, struct pbs_queue *pque);

void set_statechar(struct job *pjob);


#ifndef NDEBUG
/* static void correct_ct(); */
#endif /* NDEBUG */

#endif /* _SVR_JOBFUNC_H */
