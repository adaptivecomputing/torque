#ifndef _REQUESTS_H
#define _REQUESTS_H
#include "license_pbs.h" /* See here for the software license */

#include "pbs_job.h" /* job */
#include "batch_request.h" /* batch_request */
#include "libpbs.h" /* job_file */
#include "list_link.h" /* tlist_head */

char *get_job_envvar(job *pjob, char *variable);

/* static pid_t fork_to_user(struct batch_request *preq, int SetUID, char *HDir, char *EMsg); */

/* static void add_bad_list(char **pbl, char *newtext, int nl); */

/* static int return_file(job *pjob, enum job_file which, int sock, int remove_file); */

/* static int wchost_match(const char *can, const char *master); */

/* static int told_to_cp(char *host, char *oldpath, char **newpath); */

/* static int local_or_remote(char **path); */

/* static int is_file_same(char *file1, char *file2); */

/* static int is_file_going_to_dir(char *file, char *destdir); */

void req_deletejob(struct batch_request *preq);

void mom_req_holdjob(struct batch_request *preq);

void req_checkpointjob(struct batch_request *preq);

void req_gpuctrl(struct batch_request *preq);

int message_job(job *pjob, enum job_file jft, char *text);

void req_messagejob(struct batch_request *preq);

void req_modifyjob(struct batch_request *preq);

void req_shutdown(struct batch_request *preq);

#ifdef _CRAY
/* static void cray_susp_resum(job *pjob, int which, struct batch_request *preq); */
#endif /* _CRAY */

int sigalltasks_sisters(job *pjob, int signum);

/* static void resume_suspend(job *pjob, int susp, struct batch_request *preq); */

void req_signaljob(struct batch_request *preq);

void encode_used(job *pjob, int perm, tlist_head *phead);

void encode_flagged_attrs(job *pjob, int perm, tlist_head *phead);

void req_stat_job(struct batch_request *preq);

/* static int del_files(struct batch_request *preq, char *HDir, int setuserenv, char **pbadfile); */

#endif /* _REQUESTS_H
