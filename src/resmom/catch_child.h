
#ifndef _CATCH_CHILD_H
#define _CATCH_CHILD_H
#ifdef TEST_FUNCTION
#define PBS_MOM
#endif
#include "pbs_job.h"
#include "tm_.h"

/* Functionality
 * sets termin_child to indicate that a child process has terminated
 */
void catch_child(
  int sig);             /* Signal received from the child */

/* Functionality
 * Returns a hnodent from vnodent our of pjob's array of ji_vnods
 * pjob->vp[?]->vn_host
 * OR
 * returns null if not found
 *
 * This is only called from catch_child->scan_for_exiting, therefor no null checking on pjob is needed
 */
hnodent *get_node(
  job *pjob,             /* */
  tm_node_id nodeid);    /* */

/* Functionality
 *
 */
void scan_for_exiting(void);

/* Functionality
 *
 */
int post_epilogue(
  job *pjob,             /* */
  int ev);               /* */

/* Functionality
 *
 */
void preobit_reply(
  int sock);             /* */

/* Functionality
 *
 */
void obit_reply(
  int sock);             /* */

/* Functionality
 *
 */
void init_abort_jobs(
  int recover);          /* */

/* Functionality
 *
 */
void mom_deljob(
  job *pjob);            /* */

/* Functionality
 *
 */
void exit_mom_job(
    job *pjob,
    int mom_radix);

#endif /* _CATCH_CHILD_H */
