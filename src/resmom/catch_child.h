#ifndef _CATCH_CHILD_H
#define _CATCH_CHILD_H
#ifdef TEST_FUNCTION
#define PBS_MOM
#endif
#include "pbs_job.h"
#include "tm_.h"

void catch_child(int sig);

hnodent *get_node(job *pjob, tm_node_id nodeid);

void scan_for_exiting(void);

int post_epilogue(job *pjob, int ev);

void *preobit_reply(void *new_sock);

void *obit_reply(void *new_sock);

void init_abort_jobs(int recover);

void mom_deljob(job *pjob);

void exit_mom_job(job *pjob, int mom_radix);

#endif /* _CATCH_CHILD_H */
