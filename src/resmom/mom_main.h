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
#define CHECK_POLL_TIME     45

#define PMAX_PORT                 32000
#define MAX_PORT_STRING_LEN       6
#define MAX_LOCK_FILE_NAME_LEN    15
#define MAX_RESEND_JOBS           512
#define DUMMY_JOB_PTR             1
#define MOM_THREADS               2
#define THREAD_INFINITE          -1

#ifndef MAX_LINE
#define MAX_LINE 1024
#endif

char *nullproc(struct rm_attribute *attrib);

/* static char *arch(struct rm_attribute *attrib); */

/* static char *opsys(struct rm_attribute *attrib); */

char * getuname(void);

/* static char *reqmsg(struct rm_attribute *attrib); */

/* static char *getjoblist(struct rm_attribute *attrib); */

/* static char *reqvarattr(struct rm_attribute *attrib); */

char *reqgres(struct rm_attribute *attrib);

/* static char *reqstate(struct rm_attribute *attrib); */

/* static char *requname(struct rm_attribute *attrib); */

/* static char *validuser(struct rm_attribute *attrib); */

char *loadave(struct rm_attribute *attrib);

struct config *rm_search(struct config *where, char *what);

char *dependent(char *res, struct rm_attribute *attr);

void initialize(void);

void cleanup(void);

void die(int sig);

void memcheck(char *buf);

void checkret(char **spot, long len);

char *skipwhite(char *str);

char *tokcpy(char *str, char *tok);

void rmnl(char *str);

/* static int setbool(char *value); */

u_long addclient(char *name);

/* static u_long setpbsclient(char *value); */

/* static u_long setpbsserver(char *value); */

/* static u_long settmpdir(char *Value); */

/* static u_long setxauthpath(char *Value); */

/* static u_long setrcpcmd(char *Value); */

/* static u_long setlogevent(char *value); */

/* static u_long restricted(char *name); */

/* static u_long configversion(char *Value); */

/* static u_long setdownonerror(char *value); */

/* static u_long setenablemomrestart(char *value); */

/* static u_long cputmult(char *value); */

/* static u_long wallmult(char *value); */

/* static u_long usecp(char *value); */

/* static unsigned long prologalarm(char *value); */

/* static unsigned long setloglevel(char *value); */

/* static unsigned long setumask(char *value); */

/* static unsigned long setpreexec(char *value); */

/* static unsigned long setsourceloginbatch(char *value); */

/* static unsigned long setsourcelogininteractive(char *value); */

/* static unsigned long jobstartblocktime(char *value); */

/* static unsigned long setstatusupdatetime(char *value); */

/* static unsigned long setcheckpolltime(char *value); */

/* static void add_static(char *str, char *file, int linenum); */

/* static unsigned long setidealload(char *value); */

/* static unsigned long setignwalltime(char *value); */

/* static unsigned long setignmem(char *value); */

/* static unsigned long setigncput(char *value); */

/* static unsigned long setignvmem(char *value); */

/* static unsigned long setautoidealload(char *value); */

/* static unsigned long setallocparcmd(char *value); */

/* static unsigned long setautomaxload(char *value); */

/* static unsigned long setmaxconnecttimeout(char *value); */

/* static unsigned long setreduceprologchecks(char *value); */

/* static unsigned long setnodecheckscript(char *value); */

/* static unsigned long setnodecheckinterval(char *value); */

/* static unsigned long settimeout(char *value); */

/* static unsigned long setmaxload(char *value); */

/* static unsigned long setlogfilemaxsize(char *value); */

unsigned long rppthrottle(char *value);

/* static unsigned long setlogfilerolldepth(char *value); */

/* static unsigned long setlogdirectory(char *value); */

/* static unsigned long setlogfilesuffix(char *value); */

/* static unsigned long setlogkeepdays(char *value); */

/* static u_long setvarattr(char *value); */

/* static unsigned long setthreadunlinkcalls(char *value); */

/* static unsigned long setnodefilesuffix(char *value); */

/* static unsigned long setmomhost(char *value); */

/* static u_long setrreconfig(char *value); */

/* static unsigned long setnospooldirlist(char *value); */

unsigned long aliasservername(char *value);

/* static unsigned long setspoolasfinalname(char *value); */

unsigned long jobstarter(char *value);

/* static unsigned long setremchkptdirlist(char *value); */

void check_log(void);

int read_config(char *file);

/* static u_long setusesmt(char *value); */

/* static u_long setmempressthr(char *value); */

/* static u_long setmempressdur(char *value); */

struct rm_attribute *momgetattr(char *str);

char *conf_res(char *resline, struct rm_attribute *attr);

/* static void catch_abort(int sig); */

/* static void catch_hup(int sig); */

/* static void process_hup(void); */

void toolong(int sig);

void log_verbose(char *id, char *buf, int len);

int bad_restrict(u_long ipadd);

/* static void mom_lock(int fds, int op); */

int rm_request(struct tcp_chan *chan, int version);

int do_tcp(int socket);

void *tcp_request(void *sock_num);

char *find_signal_name(int sig);

int kill_job(job *pjob, int sig, const char *killer_id_name, char *why_killed_reason);

unsigned long getsize(resource *pres);

unsigned long gettime(resource *pres);

int job_over_limit(job *pjob);

void usage(char *prog);

char *MOMFindMyExe(char *argv0);

time_t MOMGetFileMtime(const char *fpath);

void MOMCheckRestart(void);

void initialize_globals(void);

/* static void stop_me(int sig); */

/* static void PBSAdjustLogLevel(int sig); */

char *mk_dirs(char *base);

void parse_command_line(int argc, char *argv[]);

int setup_program_environment(void);

int TMOMJobGetStartInfo(job *pjob, pjobexec_t **TJEP);

int TMOMScanForStarting(void);

void examine_all_polled_jobs(void);

void examine_all_running_jobs(void);


void examine_all_jobs_to_resend(void);

void kill_all_running_jobs(void);

int mark_for_resend(job *pjob);

void prepare_child_tasks_for_delete();

void main_loop(void);

void restart_mom(int argc, char *argv[]);

int read_layout_file();

int setup_nodeboards();

/* int main(int argc, char *argv[]); */

int parse_mom_hierarchy_file(char *path, mom_hierarchy_t *nt);

#endif /* _MOM_MAIN_H */
