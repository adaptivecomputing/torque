#include <stdlib.h>
#include <stdio.h>
#include "queue.h"
#include "threadpool.h"

queue_recycler  q_recycler;
threadpool_t   *task_pool;
bool            exit_called;
int             LOGLEVEL=3;

int lock_queue(

  struct pbs_queue *the_queue,
  const char       *id,
  const char       *msg,
  int               logging)

  {
  return(0);
  }

int insert_queue(

  all_queues *aq,
  pbs_queue  *pque)

  {
  return(0);
  }

int remove_queue(

  all_queues *aq,
  pbs_queue  *pque)

  {
  return(0);
  }


int enqueue_threadpool_request(

  void         *(*func)(void *),
  void         *arg,
  threadpool_t *tp)

  {
  return(0);
  }


