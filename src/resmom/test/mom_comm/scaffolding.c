#include "license_pbs.h" /* See here for the software license */
#include <stdlib.h>
#include <stdio.h>
#include <netinet/in.h> /* sockaddr_in */

#include "mom_server.h" /* mom_server */
#include "resmon.h" /* PBS_MAXSERVER */
#include "attribute.h" /* attribute_def, attribute */
#include "net_connect.h" /* connection, PBS_NET_MAX_CONNECTIONS */
#include "log.h" /* LOG_BUF_SIZE */
#include "list_link.h" /* tlist_head */
#include "resource.h" /* resource_def */
#include "u_tree.h" /* AvlTree */
#include "resizable_array.h" /* resizable_array */
#include "pbs_job.h" /* job */
#include "mom_func.h" /* radix_buf */

char *path_jobs; /* mom_main.c */
int multi_mom = 1; /* mom_main.c */
int svr_resc_size = 0; /* resc_def_all.c */
u_long localaddr = 0; /* mom_main.c */
time_t time_now = 0; /* mom_main.c */
mom_server mom_servers[PBS_MAXSERVER]; /* mom_server.c */
attribute_def job_attr_def[10]; /* src/server/job_attr_def.c */
int exiting_tasks = 0; /* mom_main.c */
const char *dis_emsg[10]; /* dis.h */
char *path_prologuserp; /* mom_main.c */
struct connection svr_conn[PBS_NET_MAX_CONNECTIONS]; /* net_server.c */
tlist_head svr_newjobs; /* mom_main.c */
resource_def *svr_resc_def; /* resc_def_all.c */
int port_care = TRUE; /* mom_main.c */
unsigned int pbs_rm_port = 0; /* mom_main.c */
AvlTree okclients; /* mom_main.c */
tlist_head mom_polljobs; /* mom_main.c */
char *path_prologp; /* mom_main.c */
tlist_head svr_alljobs; /* mom_main.c */
int LOGLEVEL = 0; /* mom_main.c/pbsd_main.c */
char log_buffer[LOG_BUF_SIZE]; /* pbs_log.c */


int insert_thing(resizable_array *ra, void *thing)
  {
  fprintf(stderr, "The call to log_ext needs to be mocked!!\n");
  exit(1);
  }

#undef disrus
unsigned short disrus(int stream, int *retval)
  {
  fprintf(stderr, "The call to log_ext needs to be mocked!!\n");
  exit(1);
  }

int job_save(job *pjob, int updatetype, int mom_port)
  {
  fprintf(stderr, "The call to log_ext needs to be mocked!!\n");
  exit(1);
  }

void job_purge(job *pjob)
  {
  fprintf(stderr, "The call to log_ext needs to be mocked!!\n");
  exit(1);
  }

int decode_DIS_svrattrl(int sock, tlist_head *phead)
  {
  fprintf(stderr, "The call to log_ext needs to be mocked!!\n");
  exit(1);
  }

ssize_t read_nonblocking_socket(int fd, void *buf, ssize_t count)
  {
  fprintf(stderr, "The call to log_ext needs to be mocked!!\n");
  exit(1);
  }

char * netaddr(struct sockaddr_in *ap)
  {
  fprintf(stderr, "The call to log_ext needs to be mocked!!\n");
  exit(1);
  }

int TMakeTmpDir(job *pjob, char *tmpdir)
  {
  fprintf(stderr, "The call to log_ext needs to be mocked!!\n");
  exit(1);
  }

unsigned long gettime(resource *pres)
  {
  fprintf(stderr, "The call to log_ext needs to be mocked!!\n");
  exit(1);
  }

void exec_bail(job *pjob, int code)
  {
  fprintf(stderr, "The call to log_ext needs to be mocked!!\n");
  exit(1);
  }

int AVL_list(AvlTree tree, char *Buf, long BufSize )
  {
  fprintf(stderr, "The call to log_ext needs to be mocked!!\n");
  exit(1);
  }

int exec_job_on_ms(job *pjob)
  {
  fprintf(stderr, "The call to log_ext needs to be mocked!!\n");
  exit(1);
  }

struct passwd *check_pwd(job *pjob)
  {
  fprintf(stderr, "The call to log_ext needs to be mocked!!\n");
  exit(1);
  }

int mom_do_poll(job *pjob)
  {
  fprintf(stderr, "The call to log_ext needs to be mocked!!\n");
  exit(1);
  }

void delete_link(struct list_link *old)
  {
  fprintf(stderr, "The call to log_ext needs to be mocked!!\n");
  exit(1);
  }

void clear_dynamic_string(dynamic_string *ds)
  {
  fprintf(stderr, "The call to log_ext needs to be mocked!!\n");
  exit(1);
  }

int add_hash(hash_table_t *ht, int value, char *key)
  {
  fprintf(stderr, "The call to log_ext needs to be mocked!!\n");
  exit(1);
  }

char *get_job_envvar(job *pjob, char *variable)
  {
  fprintf(stderr, "The call to log_ext needs to be mocked!!\n");
  exit(1);
  }

void log_record(int eventtype, int objclass, const char *objname, char *text)
  {
  fprintf(stderr, "The call to log_ext needs to be mocked!!\n");
  exit(1);
  }

dynamic_string *get_dynamic_string(int initial_size, char *str)
  {
  fprintf(stderr, "The call to log_ext needs to be mocked!!\n");
  exit(1);
  }

int open_tcp_stream_to_sisters(job *pjob, int com, tm_event_t parent_event, int mom_radix, hnodent *hosts, struct radix_buf **sister_list, tlist_head *phead, int flag)
  {
  fprintf(stderr, "The call to log_ext needs to be mocked!!\n");
  exit(1);
  }

int open_demux(u_long addr, int port)
  {
  fprintf(stderr, "The call to log_ext needs to be mocked!!\n");
  exit(1);
  }

int mom_get_sample()
  {
  fprintf(stderr, "The call to log_ext needs to be mocked!!\n");
  exit(1);
  }

int run_pelog(int which, char *specpelog, job *pjog, int pe_io_type)
  {
  fprintf(stderr, "The call to log_ext needs to be mocked!!\n");
  exit(1);
  }

#undef disrul
unsigned long disrul(int stream, int *retval)
  {
  fprintf(stderr, "The call to log_ext needs to be mocked!!\n");
  exit(1);
  }

char * getuname(void )
  {
  fprintf(stderr, "The call to log_ext needs to be mocked!!\n");
  exit(1);
  }

size_t write_nonblocking_socket(int fd, const void *buf, ssize_t count)
  {
  fprintf(stderr, "The call to log_ext needs to be mocked!!\n");
  exit(1);
  }

void DIS_tcp_setup(int fd)
  {
  fprintf(stderr, "The call to log_ext needs to be mocked!!\n");
  exit(1);
  }

int find_attr(struct attribute_def *attr_def, char *name, int limit)
  {
  fprintf(stderr, "The call to log_ext needs to be mocked!!\n");
  exit(1);
  }

#undef disrui
unsigned disrui(int stream, int *retval)
  {
  fprintf(stderr, "The call to log_ext needs to be mocked!!\n");
  exit(1);
  }

int AVL_is_in_tree_no_port_compare(u_long key, uint16_t port, AvlTree tree)
  {
  fprintf(stderr, "The call to log_ext needs to be mocked!!\n");
  exit(1);
  }

int kill_task(struct task *task, int sig, int pg)
  {
  fprintf(stderr, "The call to log_ext needs to be mocked!!\n");
  exit(1);
  }

int get_value_hash(hash_table_t *ht, char *key)
  {
  fprintf(stderr, "The call to log_ext needs to be mocked!!\n");
  exit(1);
  }

int DIS_tcp_wflush(int fd)
  {
  fprintf(stderr, "The call to log_ext needs to be mocked!!\n");
  exit(1);
  }

int diswcs(int stream, const char *value, size_t nchars)
  {
  fprintf(stderr, "The call to log_ext needs to be mocked!!\n");
  exit(1);
  }

unsigned long getsize(resource *pres)
  {
  fprintf(stderr, "The call to log_ext needs to be mocked!!\n");
  exit(1);
  }

void *get_next(list_link pl, char *file, int line)
  {
  fprintf(stderr, "The call to log_ext needs to be mocked!!\n");
  exit(1);
  }

int add_host_to_sister_list(char *hostname, unsigned short port, struct radix_buf *rb)
  {
  fprintf(stderr, "The call to log_ext needs to be mocked!!\n");
  exit(1);
  }

int diswui(int stream, unsigned value)
  {
  fprintf(stderr, "The call to log_ext needs to be mocked!!\n");
  exit(1);
  }

char *disrcs(int stream, size_t *nchars, int *retval)
  {
  fprintf(stderr, "The call to log_ext needs to be mocked!!\n");
  exit(1);
  }

int start_process(task *tsk, char **argv, char **envp)
  {
  fprintf(stderr, "The call to log_ext needs to be mocked!!\n");
  exit(1);
  }

void free_sisterlist(struct radix_buf **list, int radix)
  {
  fprintf(stderr, "The call to log_ext needs to be mocked!!\n");
  exit(1);
  }

int write_tcp_reply(int sock, int protocol, int version, int command, int exit_code)
  {
  fprintf(stderr, "The call to log_ext needs to be mocked!!\n");
  exit(1);
  }

int mom_set_use(job *pjob)
  {
  fprintf(stderr, "The call to log_ext needs to be mocked!!\n");
  exit(1);
  }

void free_attrlist(tlist_head *pattrlisthead)
  {
  fprintf(stderr, "The call to log_ext needs to be mocked!!\n");
  exit(1);
  }

void attrl_fixlink(tlist_head *phead)
  {
  fprintf(stderr, "The call to log_ext needs to be mocked!!\n");
  exit(1);
  }

resource_def *find_resc_def(resource_def *rscdf, char *name, int limit)
  {
  fprintf(stderr, "The call to log_ext needs to be mocked!!\n");
  exit(1);
  }

struct radix_buf **allocate_sister_list(int radix)
  {
  fprintf(stderr, "The call to log_ext needs to be mocked!!\n");
  exit(1);
  }

char *disrst(int stream, int *retval)
  {
  fprintf(stderr, "The call to log_ext needs to be mocked!!\n");
  exit(1);
  }

int tcp_connect_sockaddr(struct sockaddr *sa, size_t sa_size)
  {
  fprintf(stderr, "The call to log_ext needs to be mocked!!\n");
  exit(1);
  }

void append_link(tlist_head *head, list_link *new, void *pobj)
  {
  fprintf(stderr, "The call to log_ext needs to be mocked!!\n");
  exit(1);
  }

void sister_job_nodes(job *pjob, char *radix_hosts, char *radix_ports )
  {
  fprintf(stderr, "The call to log_ext needs to be mocked!!\n");
  exit(1);
  }

int TTmpDirName(job *pjob, char *tmpdir)
  {
  fprintf(stderr, "The call to log_ext needs to be mocked!!\n");
  exit(1);
  }

void log_event(int eventtype, int objclass, const char *objname, char *text)
  {
  fprintf(stderr, "The call to log_ext needs to be mocked!!\n");
  exit(1);
  }

void job_nodes(job *pjob)
  {
  fprintf(stderr, "The call to log_ext needs to be mocked!!\n");
  exit(1);
  }

void log_err(int errnum, const char *routine, char *text)
  {
  fprintf(stderr, "The call to log_ext needs to be mocked!!\n");
  exit(1);
  }

void close_conn(int sd, int has_mutex)
  {
  fprintf(stderr, "The call to log_ext needs to be mocked!!\n");
  exit(1);
  }

int copy_to_end_of_dynamic_string(dynamic_string *ds, char *to_copy)
  {
  fprintf(stderr, "The call to log_ext needs to be mocked!!\n");
  exit(1);
  }

int diswul(int stream, unsigned long value)
  {
  fprintf(stderr, "The call to log_ext needs to be mocked!!\n");
  exit(1);
  }

int rpp_eom(int index)
  {
  fprintf(stderr, "The call to log_ext needs to be mocked!!\n");
  exit(1);
  }

resource *find_resc_entry(attribute *pattr, resource_def *rscdf)
  {
  fprintf(stderr, "The call to log_ext needs to be mocked!!\n");
  exit(1);
  }

job *find_job(char *jobid)
  {
  fprintf(stderr, "The call to log_ext needs to be mocked!!\n");
  exit(1);
  }

#undef diswsi
int diswsi(int stream, int value)
  {
  fprintf(stderr, "The call to log_ext needs to be mocked!!\n");
  exit(1);
  }

job *job_alloc(void )
  {
  fprintf(stderr, "The call to log_ext needs to be mocked!!\n");
  exit(1);
  }

int disrsi(int stream, int *retval)
  {
  fprintf(stderr, "The call to log_ext needs to be mocked!!\n");
  exit(1);
  }

int timeval_subtract(struct timeval *result, struct timeval *x, struct timeval *y)
  {
  fprintf(stderr, "The call to log_ext needs to be mocked!!\n");
  exit(1);
  }

int allocate_demux_sockets(job *pjob, int flag)
  {
  fprintf(stderr, "The call to log_ext needs to be mocked!!\n");
  exit(1);
  }

int kill_job(job *pjob, int sig, const char *killer_id_name, char *why_killed_reason)
  {
  fprintf(stderr, "The call to log_ext needs to be mocked!!\n");
  exit(1);
  }

