#include "license_pbs.h" /* See here for the software license */
#include <stdlib.h>
#include <stdio.h> /* fprintf */
#include "tcp.h"
#include "libpbs.h" /* connect_handle, batch_reply */
#include "pbs_ifl.h" /* PBS_MAXUSER */

int pbs_errno = 0;
struct connect_handle connection[10];
char pbs_current_user[PBS_MAXUSER];
const char *dis_emsg[10];

int encode_DIS_MoveJob(struct tcp_chan *chan, char *jobid, char *destin)
  {
  fprintf(stderr, "The call to encode_DIS_JobId needs to be mocked!!\n");
  exit(1);
  }

tcp_chan *DIS_tcp_setup(int fd)
  {
  fprintf(stderr, "The call to DIS_tcp_setup needs to be mocked!!\n");
  exit(1);
  }

int DIS_tcp_wflush(tcp_chan *chan)
  {
  fprintf(stderr, "The call to DIS_tcp_wflush needs to be mocked!!\n");
  exit(1);
  }

struct batch_reply *PBSD_rdrpy(int *local_errno, int c)
  {
  fprintf(stderr, "The call to PBSD_rdrpy needs to be mocked!!\n");
  exit(1);
  }

int encode_DIS_ReqHdr(struct tcp_chan *chan, int reqt, char *user)
  {
  fprintf(stderr, "The call to encode_DIS_ReqHdr needs to be mocked!!\n");
  exit(1);
  }

void PBSD_FreeReply(struct batch_reply *reply)
  {
  fprintf(stderr, "The call to PBSD_FreeReply needs to be mocked!!\n");
  exit(1);
  }

int encode_DIS_ReqExtend(struct tcp_chan *chan, char *extend)
  {
  fprintf(stderr, "The call to encode_DIS_ReqExtend needs to be mocked!!\n");
  exit(1);
  }

void DIS_tcp_cleanup(tcp_chan *chan)
  {
  }
