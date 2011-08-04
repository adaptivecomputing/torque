#include "license_pbs.h" /* See here for the software license */
#include "pbs_job.h" /* job */
#include "queue.h" /* queue */

/* from file site_allow_u.c */
int site_allow_u(char *user, char *host);

/* from file site_alt_rte.c */
int site_alt_router(job *jobp, pbs_queue *qp, long retry_time);

/* from file site_check_u.c */
int site_check_user_map(job *pjob, char *luser, char *EMsg); 
int site_acl_check(job *pjob, pbs_queue *pque);

/* from file site_map_usr.c */
char *site_map_user(char *uname, char *host); 

/* from file site_mom_chu.c */
int site_mom_chkuser(job *pjob);

/* from file site_mom_ckp.c */
int site_mom_postchk(job *pjob, int hold_type);
int site_mom_prerst(job *pjob);

/* from file site_mom_jst.c */
int site_job_setup(job *pjob);

