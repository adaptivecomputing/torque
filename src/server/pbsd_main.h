#ifndef _PBSD_MAIN_H
#define _PBSD_MAIN_H
#include "license_pbs.h" /* See here for the software license */
#include <pthread.h> /* mutex_t */

#include "server_limits.h" 
#include "work_task.h" /* work_task */
#include "attribute.h" /* pbs_attribute */
#include "server.h" /* mutex_t */


void clear_listeners(void);

int PBSShowUsage(const char *EMsg);

void parse_command_line(int argc, char *argv[]);

void main_loop(void);

void initialize_globals(void);

void set_globals_from_environment(void);

void check_job_log(struct work_task *ptask);

void check_log(struct work_task *ptask);

void check_acct_log(struct work_task *ptask);

char *extract_dir(char *FullPath, char *Dir, int DirSize);

int is_ha_lock_file_valid(char *lockfile);

int release_file_lock(char *Lockfile, int *LockFD);

int acquire_file_lock(char *LockFile, int *LockFD, char *FileType);

void *update_ha_lock_thread(void *Arg);

int start_update_ha_lock_thread();

int mutex_lock(mutex_t *Mutex);

int mutex_unlock(mutex_t *Mutex);

int get_file_info(char *FileName, unsigned long *ModifyTime, long *FileSize, unsigned char *IsExe, unsigned char *IsDir);

int get_full_path(char *Cmd, char *GoodCmd, int GoodCmdLen);

int svr_restart();

void restore_attr_default(struct pbs_attribute *attr);

#endif /* _PBSD_MAIN_H */
