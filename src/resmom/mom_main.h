#ifndef _MOM_MAIN_H
#define _MOM_MAIN_H
#include "license_pbs.h" /* See here for the software license */

#include "resmon.h" /* rm_attribute, config */
#include "pbs_job.h" /* job, pjobexec_t */
#include "resource.h" /* resource */
#include "mom_hierarchy.h" /* mom_hierarchy_t */
#include "tcp.h" /* tcp_chan */

#define NO_LAYOUT_FILE      -10
#define BAD_LAYOUT_FILE     -505

#define PMAX_PORT                 32000
#define MAX_PORT_STRING_LEN       6
#define MAX_LOCK_FILE_NAME_LEN    15
#define MAX_RESEND_JOBS           512
#define DUMMY_JOB_PTR             1
#define MOM_THREADS               10
#define THREAD_INFINITE          -1

#ifndef MAX_LINE
#define MAX_LINE 1024
#endif

const char *nullproc(struct rm_attribute *attrib);

const char * getuname(void);

const char *reqgres(struct rm_attribute *attrib);

const char *loadave(struct rm_attribute *attrib);

struct config *rm_search(struct config *where, const char *what);

const char *dependent(const char *res, struct rm_attribute *attr);

void initialize(void);

void cleanup(void);

void die(int sig);

void memcheck(const char *buf);

void checkret(char **spot, long len);

char *skipwhite(char *str);

char *tokcpy(char *str, char *tok);

void rmnl(char *str);

u_long addclient(const char *name);

unsigned long aliasservername(const char *value);

unsigned long jobstarter(const char *value);

void check_log(void);

int read_config(char *file);

struct rm_attribute *momgetattr(char *str);

char *conf_res(char *resline, struct rm_attribute *attr);

void toolong(int sig);

void log_verbose(char *id, char *buf, int len);

int bad_restrict(u_long ipadd);

int rm_request(struct tcp_chan *chan, int version);

int do_tcp(int socket,struct sockaddr_in *pSockAddr);

void *tcp_request(void *sock_num);

const char *find_signal_name(int sig);

int kill_job(job *pjob, int sig, const char *killer_id_name, const char *why_killed_reason);

unsigned long getsize(resource *pres);

unsigned long gettime(resource *pres);

int job_over_limit(job *pjob);

void usage(char *prog);

char *MOMFindMyExe(char *argv0);

time_t MOMGetFileMtime(const char *fpath);

void MOMCheckRestart(void);

void initialize_globals(void);

char *mk_dirs(const char *base);

void parse_command_line(int argc, char *argv[]);

int setup_program_environment(void);

int TMOMJobGetStartInfo(job *pjob, pjobexec_t **TJEP);

int TMOMScanForStarting(void);

void examine_all_polled_jobs(void);

void examine_all_running_jobs(void);

void examine_all_jobs_to_resend(void);

void kill_all_running_jobs(void);

void prepare_child_tasks_for_delete();

void main_loop(void);

void restart_mom(int argc, char *argv[]);

int read_layout_file();

int setup_nodeboards();

int parse_mom_hierarchy_file(char *path, mom_hierarchy_t *nt);

void get_mom_job_dir_sticky_config(char *file);
#endif /* _MOM_MAIN_H */
