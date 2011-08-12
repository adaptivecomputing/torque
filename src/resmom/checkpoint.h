#ifndef _CHECKPOINT_H
#define _CHECKPOINT_H
#include "license_pbs.h" /* See here for the software license */

#include "pbs_job.h" /* job, job_file_delete_info */
#include "batch_request.h" /* batch_request */
#include "mom_func.h" /* var_table */

int mom_checkpoint_job_is_checkpointable(job *pjob);

int mom_checkpoint_execute_job(job *pjob, char *shell, char *arg[], struct var_table *vtable);

int mom_checkpoint_init(void);

void mom_checkpoint_set_directory_path(char *str);

unsigned long mom_checkpoint_set_checkpoint_interval(char *value);

unsigned long mom_checkpoint_set_checkpoint_script(char *value);

unsigned long mom_checkpoint_set_restart_script(char *value);

unsigned long mom_checkpoint_set_checkpoint_run_exe_name(char *value);

void get_jobs_default_checkpoint_dir(char *prefix, char *defaultpath);

void get_chkpt_dir_to_use(job *pjob, char *chkpt_dir);

int replace_checkpoint_path(char *path);

int in_remote_checkpoint_dir(char *ckpt_path);

void delete_blcr_checkpoint_files(job *pjob);

void get_blcr_chkpt(job_file_delete_info *jfdi, char *chkpt_dir);

void delete_blcr_files(job_file_delete_info *jfdi);

void mom_checkpoint_delete_files(job_file_delete_info *jfdi);

void mom_checkpoint_recover(job *pjob);

void mom_checkpoint_check_periodic_timer(job *pjob);

int blcr_checkpoint_job(job *pjob, int abort, struct batch_request *preq);

int mom_checkpoint_job(job *pjob, int abort);

void post_checkpoint(job *pjob, int ev);

int start_checkpoint(job *pjob, int abort, struct batch_request *preq);

void checkpoint_partial(job *pjob);

int blcr_restart_job(job *pjob);

int mom_restart_job(job *pjob);

void mom_checkpoint_init_job_periodic_timer(job *pjob);

int mom_checkpoint_job_has_checkpoint(job *pjob);

int mom_checkpoint_start_restart(job *pjob);

int create_missing_files(job *pjob);

#endif /* _CHECKPOINT_H */
