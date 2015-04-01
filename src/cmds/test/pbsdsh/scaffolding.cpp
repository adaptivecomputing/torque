#include "license_pbs.h" /* See here for the software license */
#include <stdlib.h>
#include <stdio.h> /* fprintf */
#include <string.h>
#include <unistd.h>
#include <netdb.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include "pbs_config.h"

#include "tm_.h" /* tm_node_id, tm_task_id, tm_event_t */
#include "tm.h" /* tm_roots */
int *tm_conn = NULL;


int tm_spawn(int argc, char **argv, char **envp, tm_node_id where, tm_task_id *tid, tm_event_t *event)
  { 
  fprintf(stderr, "The call to tm_spawn needs to be mocked!!\n");
  exit(1);
  }

int tm_poll(tm_event_t poll_event, tm_event_t *result_event, int wait, int *tm_errno)
  { 
  fprintf(stderr, "The call to tm_poll needs to be mocked!!\n");
  exit(1);
  }

ssize_t read_ac_socket(int fd, void *buf, ssize_t count)
  { 
  fprintf(stderr, "The call to read_nonblocking_socket needs to be mocked!!\n");
  exit(1);
  }

int tm_finalize(void)
  { 
  fprintf(stderr, "The call to tm_finalize needs to be mocked!!\n");
  exit(1);
  }

int tm_rescinfo(tm_node_id node, char *resource, int len, tm_event_t *event)
  { 
  fprintf(stderr, "The call to tm_rescinfo needs to be mocked!!\n");
  exit(1);
  }

int tm_nodeinfo(tm_node_id **list, int *nnodes)
  { 
  fprintf(stderr, "The call to tm_nodeinfo needs to be mocked!!\n");
  exit(1);
  }

int tm_init(void *info, struct tm_roots *roots)
  { 
  fprintf(stderr, "The call to tm_init needs to be mocked!!\n");
  exit(1);
  }

int tm_obit(tm_task_id tid, int *obitval, tm_event_t *event)
  { 
  fprintf(stderr, "The call to tm_obit needs to be mocked!!\n");
  exit(1);
  }

int tm_kill(tm_task_id tid, int sig, tm_event_t *event)
  { 
  fprintf(stderr, "The call to tm_kill needs to be mocked!!\n");
  exit(1);
  }

ssize_t read_blocking_socket(

  int      fd,
  void    *buf,
  ssize_t  count)

  {
    return(read(fd, buf, count));
  }
