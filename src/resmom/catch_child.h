#ifndef _CATCH_CHILD_H
#define _CATCH_CHILD_H
#ifdef TEST_FUNCTION
#define PBS_MOM
#endif
#include "pbs_job.h"
#include "tm_.h"

void catch_child(int sig);

hnodent *get_node(job *pjob, tm_node_id nodeid);

int send_task_obit_response(job *pjob, hnodent *pnode, char *cookie, obitent *pobit, int exitstat);

void scan_for_exiting(void);

int run_epilogues(job *pjob);

int send_job_status(job *pjob);

int post_epilogue(job *pjob, int ev);

void *preobit_reply(void *new_sock);

void *obit_reply(void *new_sock);

int has_exec_host_and_port(job *pjob);

void init_abort_jobs(int recover);

void mom_deljob(job *pjob);

int needs_and_ready_for_reply(job *pjob);

void run_any_epilogues(job *pjob);

int send_job_obit_to_ms(job *pjob, int mom_radix);

void exit_mom_job(job *pjob, int mom_radix);

#endif /* _CATCH_CHILD_H */
