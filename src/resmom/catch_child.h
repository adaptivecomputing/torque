#ifndef _CATCH_CHILD_H
#define _CATCH_CHILD_H

#include "pbs_job.h"
#include "tm_.h"

void catch_child(int sig);

hnodent *get_node(mom_job *pjob, tm_node_id nodeid);

int send_task_obit_response(mom_job *pjob, hnodent *pnode, char *cookie, obitent *pobit, int exitstat);

void scan_for_exiting(void);

int send_job_status(mom_job *pjob);

int post_epilogue(mom_job *pjob, int ev);

void *preobit_reply(void *new_sock);

void obit_reply(void *new_sock);

int has_exec_host_and_port(mom_job *pjob);

void init_abort_jobs(int recover);

int needs_and_ready_for_reply(mom_job *pjob);

int send_job_obit_to_ms(mom_job *pjob, int mom_radix);

void exit_mom_job(mom_job *pjob, int mom_radix);

#endif /* _CATCH_CHILD_H */
