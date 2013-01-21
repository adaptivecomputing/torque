#ifndef TM_H
#define TM_H
#include "license_pbs.h" /* See here for the software license */


/*
 * @(#) $Id$
 */

/*
** Header file defineing the datatypes and library visiable
** variables for paralell awareness.
*/

#include "tm_.h"
#include "tcp.h" /* struct tcp_chan */

/*
** The tm_roots structure contains data for the last
** tm_init call whose event has been polled.  <Me> is the
** caller's identity.  <Daddy> is the identity of the task that
** spawned the caller.  If <daddy> is the TM_NULL_TASK, the caller
** is the initial task of the job, running on job-relative
** node 0.
*/

struct tm_roots
  {
  tm_task_id tm_me;
  tm_task_id tm_parent;
  int  tm_nnodes;
  int  tm_ntasks;
  int  tm_taskpoolid;
  tm_task_id *tm_tasklist;
  };

/*
** The tm_whattodo structure contains data for the last
** tm_register event polled.  This is not implemented yet.
*/

typedef  struct tm_whattodo
  {
  int  tm_todo;
  tm_task_id tm_what;
  tm_node_id tm_where;
  } tm_whattodo_t;

/*
** Prototypes for all the TM API calls.
*/
#ifdef __cplusplus
extern "C"
{
#endif
int tm_init(void *info,
            struct tm_roots *roots);

int tm_poll(tm_event_t poll_event,
            tm_event_t *result_event,
            int  wait,
            int  *tm_errno);

int tm_notify(int  tm_signal);

int tm_spawn(int   argc,
             char  *argv[],
             char  *envp[],
             tm_node_id where,
             tm_task_id *tid,
             tm_event_t *event);

int tm_kill(tm_task_id tid,
            int  sig,
            tm_event_t *event);

int tm_obit(tm_task_id tid,
            int  *obitval,
            tm_event_t *event);

int tm_nodeinfo(tm_node_id **list,
                int  *nnodes);

int tm_taskinfo(tm_node_id node,
                tm_task_id *list,
                int  lsize,
                int  *ntasks,
                tm_event_t *event);

int tm_atnode(tm_task_id tid,
              tm_node_id *node);

int tm_rescinfo(tm_node_id node,
                char  *resource,
                int  len,
                tm_event_t *event);

int tm_publish(char  *name,
               void  *info,
               int  nbytes,
               tm_event_t *event);

int tm_subscribe(tm_task_id tid,
                 char  *name,
                 void  *info,
                 int  len,
                 int  *amount,
                 tm_event_t *event);

int tm_finalize(void);

int tm_alloc(char  *resources,
             tm_event_t *event);

int  tm_dealloc(tm_node_id node,
                tm_event_t *event);

int tm_create_event(tm_event_t *event);

int tm_destroy_event(tm_event_t *event);

int tm_register(tm_whattodo_t *what,
                tm_event_t *event);

/*
 *  DJH 15 Nov 2001.
 *  Generic "out-of-band" task adoption call for tasks parented by
 *  another job management system.  Minor security hole?
 *  Cannot be called with any other tm call.
 *  26 Feb 2002. Allows id to be jobid (adoptCmd = TM_ADOPT_JOBID)
 *  or some altid (adoptCmd = TM_ADOPT_ALTID)
 */
int tm_adopt(char *id, int adoptCmd, pid_t pid);
#ifdef __cplusplus
}
#endif

#endif /* TM_H */
