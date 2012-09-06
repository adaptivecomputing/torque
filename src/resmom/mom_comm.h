#ifndef _MOM_COMM_H
#define _MOM_COMM_H
#include "license_pbs.h" /* See here for the software license */

#include "pbs_job.h" /* task, hnodent, tm_task_id, job, eventent, fwdevent */
#include "tm_.h" /* tm_event_t */
#include "sys/socket.h" /* sockaddr_in */
#include "resource.h" /* resource */
#include "tcp.h" /* tcp_chan */

int task_save(task *ptask);

eventent *event_alloc(int command, hnodent *pnode, tm_event_t event, tm_task_id taskid);

task *pbs_task_create(job *pjob, tm_task_id taskid);

task *task_find(job *pjob, tm_task_id taskid);

task *task_check(job *pjob, tm_task_id taskid);

int task_recov(job *pjob);

int tm_reply(struct tcp_chan *chan, int com, tm_event_t event);

int im_compose(struct tcp_chan *chan, char *jobid, char *cookie, int command, tm_event_t event, tm_task_id taskid);

int send_sisters(job *pjob, int com, int using_radix);

hnodent *find_node(job *pjob, int stream, tm_node_id nodeid);

void job_start_error(job *pjob, int code, char *nodename);

void arrayfree(char **array);

void node_bailout(job *pjob, hnodent *np);

void term_job(job *pjob);

void im_eof(int stream, int ret);

int check_ms(struct tcp_chan *chan, job *pjob);

u_long resc_used(job *pjob, char *name, u_long(*func)(resource *));

infoent *task_findinfo(task *ptask, char *name);

void task_saveinfo(task *ptask, char *name, void *info, size_t len);

char *resc_string(job *pjob);

int contact_sisters(job *pjob, tm_event_t parent_event, int sister_count, char *radix_hosts, char *radix_ports);

void send_im_error(int err, int reply, job *pjob, char *cookie, tm_event_t event, tm_task_id fromtask);

int im_join_job_as_sister(struct tcp_chan *chan, char *jobid, struct sockaddr_in *addr, char *cookie, tm_event_t event, int fromtask, int command, int job_radix);

void im_kill_job_as_sister(job *pjob, tm_event_t event, unsigned int momport, int radix);

int im_spawn_task(struct tcp_chan *chan, char *cookie, tm_event_t event, struct sockaddr_in *addr, tm_task_id fromtask, job *pjob);

int im_signal_task(struct tcp_chan *chan, job *pjob, char *cookie, tm_event_t event, tm_task_id fromtask);

int im_obit_task(struct tcp_chan *chan, job *pjob, char *cookie, tm_event_t event, tm_task_id fromtask);

int im_get_info(struct tcp_chan *chan, job *pjob, char *cookie, tm_event_t event, tm_task_id fromtask);

int im_get_resc_as_sister(struct tcp_chan *chan, job *pjob, char *cookie, tm_event_t event, tm_task_id fromtask);

int get_reply_stream(job *pjob);

int get_radix_reply_stream(job *pjob);

int im_poll_job_as_sister(job *pjob, char *cookie, tm_event_t event, tm_task_id fromtask);

int im_abort_job(job *pjob, struct sockaddr_in *addr, char *cookie, tm_event_t event, tm_task_id fromtask);

int im_get_tid(job *pjob, char *cookie, tm_event_t event, tm_task_id fromtask);

int handle_im_join_job_response(struct tcp_chan *chan, job *pjob, struct sockaddr_in *addr);

int handle_im_kill_job_response(struct tcp_chan *chan, job *pjob, hnodent *np, int event_com, int nodeidx);

int handle_im_spawn_task_response(struct tcp_chan *chan, job *pjob, tm_task_id event_task, tm_event_t event);

int handle_im_signal_task_response(job *pjob, tm_task_id event_task, tm_event_t event);

int handle_im_get_tasks_response(struct tcp_chan *chan, job *pjob, tm_task_id event_task, tm_event_t event);

int handle_im_obit_task_response(struct tcp_chan *chan, job *pjob, tm_task_id event_task, tm_event_t event);

int handle_im_get_info_response(struct tcp_chan *chan, job *pjob, tm_task_id event_task, tm_event_t event);

int handle_im_get_resc_response(struct tcp_chan *chan, job *pjob, tm_task_id event_task, tm_event_t event);

int handle_im_poll_job_response(struct tcp_chan *chan, job *pjob, int nodeidx, hnodent *np);

int handle_im_get_tid_response(struct tcp_chan *chan, job *pjob, char *cookie, char **argv, char **envp, fwdevent *efwd);

void im_request(struct tcp_chan *chan, int version);

void tm_eof(int fd);

void tm_request_init(job *pjob, task *ptask, int *ret, int event, int prev_error);

int tm_postinfo(char *name, char *info, char *jobid, int fromtask, int prev_error, int event, int *ret, task *ptask, size_t *len);

int tm_spawn_request(struct tcp_chan *chan, job *pjob, int prev_error, int event, char *cookie, int *reply_ptr, int *ret, tm_task_id fromtask, hnodent *phost, int nodeid);

int tm_tasks_request(struct tcp_chan *chan, job *pjob, int prev_error, int event, char *cookie, int *reply_ptr, int *ret, tm_task_id fromtask, hnodent *phost, int nodeid);

int tm_signal_request(struct tcp_chan *chan, job *pjob, int prev_error, int event, char *cookie, tm_task_id fromtask, int *ret, int *reply_ptr, hnodent *phost, int nodeid);

int tm_obit_request(struct tcp_chan *chan, job *pjob, int prev_error, int event, char *cookie, int *reply_ptr, int *ret, tm_task_id fromtask, hnodent *phost, int nodeid);

int tm_getinfo_request(struct tcp_chan *chan, job *pjob, int prev_error, int event, char *cookie, int *reply_ptr, int *ret, tm_task_id fromtask, hnodent *phost, int nodeid);

int tm_resources_request(struct tcp_chan *chan, job *pjob, int prev_error, int event, char *cookie, int *reply_ptr, int *ret, tm_task_id fromtask, hnodent *phost, int nodeid);

int tm_request(struct tcp_chan *chan, int version);

/* static int adoptSession(pid_t sid, pid_t pid, char *id, int command, char *cookie); */

char *cat_dirs(char *root, char *base);

char *get_local_script_path(job *pjob, char *base);

int get_job_struct(job **pjob, char *jobid, int command, struct tcp_chan *chan, struct sockaddr_in *addr, tm_node_id nodeid);

int readit(int sock, int fd);

void demux_wait(int sig);

void fork_demux(job *pjob);

void send_update_soon();

int read_status_strings(struct tcp_chan *chan, int version);

#endif /* _MOM_COMM_H */
