#include "license_pbs.h" /* See here for the software license */
#ifndef _CATCH_CHILD_CT_H
#define _CATCH_CHILD_CT_H
#include <check.h>
#define PBS_MOM

#define CATCH_CHILD 1
Suite *catch_child_suite();
#define GET_NODE 2
Suite *get_node_suite();
#define INIT_ABORT_JOBS 3
Suite *init_abort_jobs_suite();
#define MOM_DELJOB 4
Suite *mom_deljob_suite();
#define OBIT_REPLY 5
Suite *obit_reply_suite();
#define POST_EPILOGUE 6
Suite *post_epilogue_suite();
#define PREOBIT_REPLY 7
Suite *preobit_reply_suite();
#define SCAN_FOR_EXITING 8
Suite *scan_for_exiting_suite();
#define EXIT_MOM_JOB 9
Suite *exit_mom_job_suite();

#endif /* _CATCH_CHILD_CT_H */
