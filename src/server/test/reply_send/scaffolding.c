#include "license_pbs.h" /* See here for the software license */
#include <stdlib.h>
#include <stdio.h> /* fprintf */

#include "work_task.h" /* all_tasks */
#include "libpbs.h" /* batch_reply */
#include "batch_request.h" /* batach_request */
#include "list_link.h" /* list_link */

const char *msg_daemonname = "unset";
int LOGLEVEL = 7; /* force logging code to be exercised as tests run */
all_tasks task_list_event;


int encode_DIS_reply(struct tcp_chan *chan, struct batch_reply *reply)
  {
  fprintf(stderr, "The call to encode_DIS_reply needs to be mocked!!\n");
  exit(1);
  }

void free_br(struct batch_request *preq)
  {
  fprintf(stderr, "The call to free_br needs to be mocked!!\n");
  exit(1);
  }

void DIS_tcp_setup(int fd)
  {
  fprintf(stderr, "The call to DIS_tcp_setup needs to be mocked!!\n");
  exit(1);
  }

int DIS_tcp_wflush(tcp_chan *chan)
  {
  fprintf(stderr, "The call to DIS_tcp_wflush needs to be mocked!!\n");
  exit(1);
  }

void *get_next(list_link pl, char *file, int line)
  {
  fprintf(stderr, "The call to get_next needs to be mocked!!\n");
  exit(1);
  }

void free_attrlist(tlist_head *pattrlisthead)
  {
  fprintf(stderr, "The call to free_attrlist needs to be mocked!!\n");
  exit(1);
  }

char *pbse_to_txt(int err)
  {
  fprintf(stderr, "The call to pbse_to_txt needs to be mocked!!\n");
  exit(1);
  }

work_task *next_task(all_tasks *at, int *iter)
  {
  fprintf(stderr, "The call to next_task needs to be mocked!!\n");
  exit(1);
  }

void close_conn(int sd, int has_mutex)
  {
  fprintf(stderr, "The call to close_conn needs to be mocked!!\n");
  exit(1);
  }

int remove_task(all_tasks *at, work_task *wt)
  {
  fprintf(stderr, "The call to remove_task needs to be mocked!!\n");
  exit(1);
  }

const char *reqtype_to_txt(int reqtype)
  {
  fprintf(stderr, "The call to reqtype_to_txt needs to be mocked!!\n");
  exit(1);
  }

int safe_strncat(char *str, const char *to_append, size_t space_remaining)
  {
  return(0);
  }

void DIS_tcp_cleanup(struct tcp_chan *chan) {}

void log_err(int errnum, const char *routine, const char *text) {}
void log_record(int eventtype, int objclass, const char *objname, const char *text) {}
void log_event(int eventtype, int objclass, const char *objname, const char *text) {}
