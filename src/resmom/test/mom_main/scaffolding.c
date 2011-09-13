#include "license_pbs.h" 
#include <stdlib.h>
#include <stdio.h>
#include <netdb.h> /* hostent */

#include "mom_func.h" /* sig_tbl */
#include "pbs_job.h" /* job, pjobexec_t, task, pjobexec_t */
#include "u_tree.h" /* AvlTree */
#include "libpbs.h" /* job_file */
#include "mom_hierarchy.h" /* mom_hierarchy_t */
#include "server_limits.h" /* pbs_net_t. Also defined in net_connect.h */
#include "threadpool.h" /* threadpool_t */
#include "list_link.h" /* list_link, tlist_head */
#include "resizable_array.h" /* resizable_array */
#include "pbs_nodes.h" /* pbsnode */
#include "attribute.h" /* attribute */
#include "resource.h" /* resource_def */
#include "prolog.h" /* PBS_PROLOG_TIME */
#include "log.h" /* LOG_BUF_SIZE */

resizable_array *received_statuses;
int mom_server_count = 0;
char *msg_daemonname = "unset";
struct sig_tbl sig_tbl[2];
char pbs_current_user[PBS_MAXUSER];
char *server_alias = NULL;
const char *dis_emsg[10];
long *log_event_mask = NULL;
int rpp_dbprt = 0;
unsigned int pe_alarm_time = PBS_PROLOG_TIME;
struct connection svr_conn[PBS_NET_MAX_CONNECTIONS];
struct config standard_config[2];
struct config dependent_config[2];
long MaxConnectTimeout = 5000000;
tlist_head svr_requests;
char *msg_info_mom = "Torque Mom Version = %s, loglevel = %d";
hash_table_t *received_table;
threadpool_t *request_pool;
AvlTree okclients;
time_t pbs_tcp_timeout = 20;
time_t wait_time = 10;
char log_buffer[LOG_BUF_SIZE];



int log_remove_old(char *DirPath, unsigned long ExpireTime)
  {
  fprintf(stderr, "The call to long needs to be mocked!!\n");
  exit(1);
  }

int mom_close_poll(void)
  {
  fprintf(stderr, "The call to mom_close_poll needs to be mocked!!\n");
  exit(1);
  }

void init_abort_jobs(int recover)
  {
  fprintf(stderr, "The call to init_abort_jobs needs to be mocked!!\n");
  exit(1);
  }

int mom_server_add(char *value)
  {
  fprintf(stderr, "The call to mom_server_add needs to be mocked!!\n");
  exit(1);
  }

int job_save(job *pjob, int updatetype, int mom_port)
  {
  fprintf(stderr, "The call to job_save needs to be mocked!!\n");
  exit(1);
  }

void job_purge(job *pjob)
  {
  fprintf(stderr, "The call to job_purge needs to be mocked!!\n");
  exit(1);
  }

void scan_non_child_tasks(void)
  {
  fprintf(stderr, "The call to scan_non_child_tasks needs to be mocked!!\n");
  exit(1);
  }

unsigned int get_svrport(char *service_name, char *ptype, unsigned int pdefault)
  {
  fprintf(stderr, "The call to get_svrport needs to be mocked!!\n");
  exit(1);
  }

int get_la(double *rv)
  {
  fprintf(stderr, "The call to get_la needs to be mocked!!\n");
  exit(1);
  }

ssize_t read_nonblocking_socket(int fd, void *buf, ssize_t count)
  {
  fprintf(stderr, "The call to read_nonblocking_socket needs to be mocked!!\n");
  exit(1);
  }

int get_parent_and_child(char *start, char **parent, char **child, char **end)
  {
  fprintf(stderr, "The call to get_parent_and_child needs to be mocked!!\n");
  exit(1);
  }

int MUStrNCat(char **BPtr, int *BSpace, char *Src)
  {
  fprintf(stderr, "The call to MUStrNCat needs to be mocked!!\n");
  exit(1);
  }

void scan_for_exiting(void)
  {
  fprintf(stderr, "The call to scan_for_exiting needs to be mocked!!\n");
  exit(1);
  }

void mom_checkpoint_set_directory_path(char *str)
  {
  fprintf(stderr, "The call to mom_checkpoint_set_directory_path needs to be mocked!!\n");
  exit(1);
  }

void exec_bail(job *pjob, int code)
  {
  fprintf(stderr, "The call to exec_bail needs to be mocked!!\n");
  exit(1);
  }

int AVL_list(AvlTree tree, char *Buf, long BufSize)
  {
  fprintf(stderr, "The call to AVL_list needs to be mocked!!\n");
  exit(1);
  }

unsigned long mom_checkpoint_set_checkpoint_interval(char *value)
  {
  fprintf(stderr, "The call to mom_checkpoint_set_checkpoint_interval needs to be mocked!!\n");
  exit(1);
  }

int TMomCheckJobChild(pjobexec_t *TJE, int Timeout, int *Count, int *RC)
  {
  fprintf(stderr, "The call to TMomCheckJobChild needs to be mocked!!\n");
  exit(1);
  }

int message_job(job *pjob, enum job_file jft, char *text)
  {
  fprintf(stderr, "The call to message_job needs to be mocked!!\n");
  exit(1);
  }

int IamRoot()
  {
  fprintf(stderr, "The call to IamRoot needs to be mocked!!\n");
  exit(1);
  }

int add_network_entry(mom_hierarchy_t *nt, char *name, struct addrinfo *ai, unsigned short rm_port, unsigned short service_port, int path, int level)
  {
  fprintf(stderr, "The call to add_network_entry needs to be mocked!!\n");
  exit(1);
  }

int log_init(char *suffix, char *hostname)
  {
  fprintf(stderr, "The call to log_init needs to be mocked!!\n");
  exit(1);
  }

int post_epilogue(job *pjob, int ev)
  {
  fprintf(stderr, "The call to post_epilogue needs to be mocked!!\n");
  exit(1);
  }

char *get_job_envvar(job *pjob, char *variable)
  {
  fprintf(stderr, "The call to get_job_envvar needs to be mocked!!\n");
  exit(1);
  }

void log_record(int eventtype, int objclass, char *objname, char *text)
  {
  fprintf(stderr, "The call to log_record needs to be mocked!!\n");
  exit(1);
  }

char *netaddr_pbs_net_t(pbs_net_t ipadd)
  {
  fprintf(stderr, "The call to netaddr_pbs_net_t needs to be mocked!!\n");
  exit(1);
  }

unsigned long mom_checkpoint_set_checkpoint_script(char *value)
  {
  fprintf(stderr, "The call to mom_checkpoint_set_checkpoint_script needs to be mocked!!\n");
  exit(1);
  }

int mom_get_sample(void)
  {
  fprintf(stderr, "The call to mom_get_sample needs to be mocked!!\n");
  exit(1);
  }

void set_rpp_throttle_sleep_time(long sleep_time)
  {
  fprintf(stderr, "The call to set_rpp_throttle_sleep_time needs to be mocked!!\n");
  exit(1);
  }

int run_pelog(int which, char *specpelog, job *pjog, int pe_io_type)
  {
  fprintf(stderr, "The call to run_pelog needs to be mocked!!\n");
  exit(1);
  }

char *pbs_get_server_list(void)
  {
  fprintf(stderr, "The call to pbs_get_server_list needs to be mocked!!\n");
  exit(1);
  }

int tm_request(int fd, int version)
  {
  fprintf(stderr, "The call to tm_request needs to be mocked!!\n");
  exit(1);
  }

int csv_length(char *csv_str)
  {
  fprintf(stderr, "The call to csv_length needs to be mocked!!\n");
  exit(1);
  }

char *csv_nth(char *csv_str, int n)
  {
  fprintf(stderr, "The call to csv_nth needs to be mocked!!\n");
  exit(1);
  }

void dep_cleanup(void)
  {
  fprintf(stderr, "The call to dep_cleanup needs to be mocked!!\n");
  exit(1);
  }

size_t write_nonblocking_socket(int fd, const void *buf, ssize_t count)
  {
  fprintf(stderr, "The call to write_nonblocking_socket needs to be mocked!!\n");
  exit(1);
  }

void DIS_tcp_setup(int fd)
  {
  fprintf(stderr, "The call to DIS_tcp_setup needs to be mocked!!\n");
  exit(1);
  }

int MUSNPrintF(char **BPtr, int *BSpace, char *Format, ...)
  {
  fprintf(stderr, "The call to MUSNPrintF needs to be mocked!!\n");
  exit(1);
  }

int mom_over_limit(job *pjob)
  {
  fprintf(stderr, "The call to mom_over_limit needs to be mocked!!\n");
  exit(1);
  }

int AVL_is_in_tree_no_port_compare(u_long key, uint16_t port, AvlTree tree)
  {
  fprintf(stderr, "The call to AVL_is_in_tree_no_port_compare needs to be mocked!!\n");
  exit(1);
  }

void net_close(int but)
  {
  fprintf(stderr, "The call to net_close needs to be mocked!!\n");
  exit(1);
  }

int kill_task(task *ptask, int sig, int pg)
  {
  fprintf(stderr, "The call to kill_task needs to be mocked!!\n");
  exit(1);
  }

void mom_server_all_send_state(void)
  {
  fprintf(stderr, "The call to mom_server_all_send_state needs to be mocked!!\n");
  exit(1);
  }

void mom_server_all_init(void)
  {
  fprintf(stderr, "The call to mom_server_all_init needs to be mocked!!\n");
  exit(1);
  }

int DIS_tcp_wflush(int fd)
  {
  fprintf(stderr, "The call to DIS_tcp_wflush needs to be mocked!!\n");
  exit(1);
  }

int diswcs(int stream, const char *value, size_t nchars)
  {
  fprintf(stderr, "The call to diswcs needs to be mocked!!\n");
  exit(1);
  }

void mom_checkpoint_check_periodic_timer(job *pjob)
  {
  fprintf(stderr, "The call to mom_checkpoint_check_periodic_timer needs to be mocked!!\n");
  exit(1);
  }

int chk_file_sec(char *path, int isdir, int sticky, int disallow, int fullpath, char *SEMsg)
  {
  fprintf(stderr, "The call to chk_file_sec needs to be mocked!!\n");
  exit(1);
  }

int TMomFinalizeJob3(pjobexec_t *TJE, int ReadSize, int ReadErrno, int *SC)
  {
  fprintf(stderr, "The call to TMomFinalizeJob3 needs to be mocked!!\n");
  exit(1);
  }

unsigned long mom_checkpoint_set_restart_script(char *value)
  {
  fprintf(stderr, "The call to mom_checkpoint_set_restart_script needs to be mocked!!\n");
  exit(1);
  }

void mom_server_all_update_stat(void)
  {
  fprintf(stderr, "The call to mom_server_all_update_stat needs to be mocked!!\n");
  exit(1);
  }

void check_busy(double mla)
  {
  fprintf(stderr, "The call to check_busy needs to be mocked!!\n");
  exit(1);
  }

void is_request(int stream, int version, int *cmdp)
  {
  fprintf(stderr, "The call to is_request needs to be mocked!!\n");
  exit(1);
  }

int mom_checkpoint_init(void)
  {
  fprintf(stderr, "The call to mom_checkpoint_init needs to be mocked!!\n");
  exit(1);
  }

int wait_request(time_t waittime, long *SState)
  {
  fprintf(stderr, "The call to wait_request needs to be mocked!!\n");
  exit(1);
  }

void log_close(int msg)
  {
  fprintf(stderr, "The call to log_close needs to be mocked!!\n");
  exit(1);
  }

void *get_next(list_link pl, char *file, int line)
  {
  fprintf(stderr, "The call to get_next needs to be mocked!!\n");
  exit(1);
  }

resizable_array *initialize_resizable_array(int size)
  {
  fprintf(stderr, "The call to initialize_resizable_array needs to be mocked!!\n");
  exit(1);
  }

int log_open(char *filename, char *directory)
  {
  fprintf(stderr, "The call to log_open needs to be mocked!!\n");
  exit(1);
  }

int send_sisters(job *pjob, int com, int using_radix)
  {
  fprintf(stderr, "The call to send_sisters needs to be mocked!!\n");
  exit(1);
  }

int mom_set_use(job *pjob)
  {
  fprintf(stderr, "The call to mom_set_use needs to be mocked!!\n");
  exit(1);
  }

int initialize_threadpool(threadpool_t **pool, int min_threads, int max_threads, int max_idle_time)
  {
  fprintf(stderr, "The call to initialize_threadpool needs to be mocked!!\n");
  exit(1);
  }

void term_job(job *pjob)
  {
  fprintf(stderr, "The call to term_job needs to be mocked!!\n");
  exit(1);
  }

struct passwd * getpwnam_ext(char * user_name)
  {
  fprintf(stderr, "The call to getpwnam_ext needs to be mocked!!\n");
  exit(1);
  }

char *disrst(int stream, int *retval)
  {
  fprintf(stderr, "The call to disrst needs to be mocked!!\n");
  exit(1);
  }

int mom_open_poll(void)
  {
  fprintf(stderr, "The call to mom_open_poll needs to be mocked!!\n");
  exit(1);
  }

void dep_main_loop_cycle(void)
  {
  fprintf(stderr, "The call to dep_main_loop_cycle needs to be mocked!!\n");
  exit(1);
  }

int init_network(unsigned int socket, void *(*readfunc)(void *))
  {
  fprintf(stderr, "The call to init_network needs to be mocked!!\n");
  exit(1);
  }

void dep_initialize(void)
  {
  fprintf(stderr, "The call to dep_initialize needs to be mocked!!\n");
  exit(1);
  }

void append_link(tlist_head *head, list_link *new, void *pobj)
  {
  fprintf(stderr, "The call to append_link needs to be mocked!!\n");
  exit(1);
  }

void log_roll(int max_depth)
  {
  fprintf(stderr, "The call to log_roll needs to be mocked!!\n");
  exit(1);
  }

hash_table_t *create_hash(int size)
  {
  fprintf(stderr, "The call to create_hash needs to be mocked!!\n");
  exit(1);
  }

int init_resc_defs(void)
  {
  fprintf(stderr, "The call to init_resc_defs needs to be mocked!!\n");
  exit(1);
  }

int swap_things(resizable_array *ra, void *thing1, void *thing2)
  {
  fprintf(stderr, "The call to swap_things needs to be mocked!!\n");
  exit(1);
  }

int get_fullhostname(char *shortname, char *namebuf, int bufsize, char *EMsg)
  {
  fprintf(stderr, "The call to get_fullhostname needs to be mocked!!\n");
  exit(1);
  }

void scan_for_terminated(void)
  {
  fprintf(stderr, "The call to scan_for_terminated needs to be mocked!!\n");
  exit(1);
  }

long log_size(void)
  {
  fprintf(stderr, "The call to log_size needs to be mocked!!\n");
  exit(1);
  }

int task_save(task *ptask)
  {
  fprintf(stderr, "The call to task_save needs to be mocked!!\n");
  exit(1);
  }

void log_event(int eventtype, int objclass, char *objname, char *text)
  {
  fprintf(stderr, "The call to log_event needs to be mocked!!\n");
  exit(1);
  }

void *mom_process_request(void *sock_num)
  {
  fprintf(stderr, "The call to mom_process_request needs to be mocked!!\n");
  exit(1);
  }

void log_err(int errnum, char *routine, char *text)
  {
  fprintf(stderr, "The call to log_err needs to be mocked!!\n");
  exit(1);
  }

int setup_env(char *filen)
  {
  fprintf(stderr, "The call to setup_env needs to be mocked!!\n");
  exit(1);
  }

void close_conn(int sd, int has_mutex)
  {
  fprintf(stderr, "The call to close_conn needs to be mocked!!\n");
  exit(1);
  }

void catch_child(int sig)
  {
  fprintf(stderr, "The call to catch_child needs to be mocked!!\n");
  exit(1);
  }

AvlTree AVL_insert(u_long key, uint16_t port, struct pbsnode *node, AvlTree tree)
  {
  fprintf(stderr, "The call to AVL_insert needs to be mocked!!\n");
  exit(1);
  }

resource *find_resc_entry(attribute *pattr, resource_def *rscdf)
  {
  fprintf(stderr, "The call to find_resc_entry needs to be mocked!!\n");
  exit(1);
  }

job *find_job(char *jobid)
  {
  fprintf(stderr, "The call to find_job needs to be mocked!!\n");
  exit(1);
  }

void DIS_tcp_settimeout(long timeout)
  {
  fprintf(stderr, "The call to DIS_tcp_settimeout needs to be mocked!!\n");
  exit(1);
  }

mom_hierarchy_t *initialize_mom_hierarchy(void)
  {
  fprintf(stderr, "The call to initialize_mom_hierarchy needs to be mocked!!\n");
  exit(1);
  }

int diswsi(int stream, int value)
  {
  fprintf(stderr, "The call to diswsi needs to be mocked!!\n");
  exit(1);
  }

void mom_server_all_diag(char **BPtr, int *BSpace)
  {
  fprintf(stderr, "The call to mom_server_all_diag needs to be mocked!!\n");
  exit(1);
  }

int disrsi(int stream, int *retval)
  {
  fprintf(stderr, "The call to disrsi needs to be mocked!!\n");
  exit(1);
  }

unsigned long mom_checkpoint_set_checkpoint_run_exe_name(char *value)
  {
  fprintf(stderr, "The call to mom_checkpoint_set_checkpoint_run_exe_name needs to be mocked!!\n");
  exit(1);
  }

void im_request(int stream, int version)
  {
  fprintf(stderr, "The call to im_request needs to be mocked!!\n");
  exit(1);
  }

int lock_init()
  {
  fprintf(stderr, "The call to lock_init needs to be mocked!!\n");
  exit(1);
  }

