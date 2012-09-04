#include "license_pbs.h" /* See here for the software license */
#include <stdlib.h>
#include <stdio.h> /* fprintf */

#include "pbs_ifl.h" /* PBS_MAXSERVERNAME */
#include "server.h" /* server */
#include "work_task.h" /* work_task, work_type */
#include "batch_request.h" /* batch_request */

char *path_track;
char server_name[PBS_MAXSERVERNAME + 1];
struct server server;
int LOGLEVEL = 0;


struct batch_request *alloc_br(int type) 
  {
  fprintf(stderr, "The call to alloc_br to be mocked!!\n");
  exit(1);
  }

void reply_ack(struct batch_request *preq) 
  {
  fprintf(stderr, "The call to reply_ack to be mocked!!\n");
  exit(1);
  }

struct work_task *set_task(enum work_type type, long event_id, void (*func)(), void *parm, int get_lock)
  {
  fprintf(stderr, "The call to set_task to be mocked!!\n");
  exit(1);
  }

ssize_t write_nonblocking_socket( int fd, const void *buf, ssize_t count) 
  {
  fprintf(stderr, "The call to write_nonblocking_socket to be mocked!!\n");
  exit(1);
  }

void req_reject(int code, int aux, struct batch_request *preq, char *HostName, char *Msg) 
  {
  fprintf(stderr, "The call to req_reject to be mocked!!\n");
  exit(1);
  }

void release_req(struct work_task *pwt)
  {
  fprintf(stderr, "The call to release_req to be mocked!!\n");
  exit(1);
  }

int issue_to_svr(char *servern, struct batch_request *preq, void (*replyfunc)(struct work_task *))
  {
  fprintf(stderr, "The call to issue_to_svr to be mocked!!\n");
  exit(1);
  }

void free_br(batch_request *preq) {}
