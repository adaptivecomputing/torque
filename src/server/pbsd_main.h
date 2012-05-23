#ifndef _PBSD_MAIN_H
#define _PBSD_MAIN_H
#include "license_pbs.h" /* See here for the software license */
#include <pthread.h> /* mutex_t */

#include "server_limits.h" /* pbs_net_t. Also defined in net_connect.h */
#include "work_task.h" /* work_task */
#include "attribute.h" /* pbs_attribute */
#include "server.h" /* mutex_t */

/* static void need_y_response(int type); */

void process_pbs_server_port_scheduler(int *sock);

void clear_listeners(void);

int add_listener(pbs_net_t l_addr, unsigned int l_port);

int PBSShowUsage(char *EMsg);

void parse_command_line(int argc, char *argv[]);

/* static int start_hot_jobs(void); */

void main_loop(void);

void initialize_globals(void);

void set_globals_from_environment(void);

/* int main(int argc, char *argv[]); */

void check_job_log(struct work_task *ptask);

void check_log(struct work_task *ptask);

void check_acct_log(struct work_task *ptask);

/* static int get_port(char *arg, unsigned int *port, pbs_net_t *addr); */

char *extract_dir(char *FullPath, char *Dir, int DirSize);

int is_ha_lock_file_valid(char *lockfile);

int release_file_lock(char *Lockfile, int *LockFD);

int acquire_file_lock(char *LockFile, int *LockFD, char *FileType);

void *update_ha_lock_thread(void *Arg);

int start_update_ha_lock_thread();

int mutex_lock(mutex_t *Mutex);

int mutex_unlock(mutex_t *Mutex);

/* static void lock_out_ha(); */

/* static int daemonize_server(int DoBackground, int *sid); */

int get_file_info(char *FileName, unsigned long *ModifyTime, long *FileSize, unsigned char *IsExe, unsigned char *IsDir);

int get_full_path(char *Cmd, char *GoodCmd, int GoodCmdLen);

int svr_restart();

void restore_attr_default(struct pbs_attribute *attr);

#endif /* _PBSD_MAIN_H */
