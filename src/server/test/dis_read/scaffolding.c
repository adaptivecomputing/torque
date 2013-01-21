#include "license_pbs.h" /* See here for the software license */
#include <stdlib.h>
#include <stdio.h> /* fprintf */

#include "batch_request.h" /* batch_request */
#include "libpbs.h" /* batch_reply */
const char *dis_emsg[10];
int LOGLEVEL = 7; /* force logging code to be exercised as tests run */


int decode_DIS_JobId(struct tcp_chan *chan, char *jobid)
  {
  fprintf(stderr, "The call to decode_DIS_JobId needs to be mocked!!\n");
  exit(1);
  }

int decode_DIS_Register(struct tcp_chan *chan, struct batch_request *preq)
  {
  fprintf(stderr, "The call to decode_DIS_Register needs to be mocked!!\n");
  exit(1);
  }

int decode_DIS_svrattrl(struct tcp_chan *chan, tlist_head *phead)
  {
  fprintf(stderr, "The call to decode_DIS_svrattrl needs to be mocked!!\n");
  exit(1);
  }

int decode_DIS_MessageJob(struct tcp_chan *chan, struct batch_request *preq)
  {
  fprintf(stderr, "The call to decode_DIS_MessageJob needs to be mocked!!\n");
  exit(1);
  }

int decode_DIS_JobObit(struct tcp_chan *chan, struct batch_request *preq)
  {
  fprintf(stderr, "The call to decode_DIS_JobObit needs to be mocked!!\n");
  exit(1);
  }

int decode_DIS_JobCred(struct tcp_chan *chan, struct batch_request *preq)
  {
  fprintf(stderr, "The call to decode_DIS_JobCred needs to be mocked!!\n");
  exit(1);
  }

void DIS_tcp_setup(int fd)
  {
  fprintf(stderr, "The call to DIS_tcp_setup needs to be mocked!!\n");
  exit(1);
  }

int decode_DIS_ReqExtend(struct tcp_chan *chan, struct batch_request *preq)
  {
  fprintf(stderr, "The call to decode_DIS_ReqExtend needs to be mocked!!\n");
  exit(1);
  }

int decode_DIS_QueueJob(struct tcp_chan *chan, struct batch_request *preq)
  {
  fprintf(stderr, "The call to decode_DIS_QueueJob needs to be mocked!!\n");
  exit(1);
  }

int decode_DIS_SignalJob(struct tcp_chan *chan, struct batch_request *preq)
  {
  fprintf(stderr, "The call to decode_DIS_SignalJob needs to be mocked!!\n");
  exit(1);
  }

int decode_DIS_AltAuthen(struct tcp_chan *chan, struct batch_request *preq)
  {
  fprintf(stderr, "The call to decode_DIS_AltAuthen needs to be mocked!!\n");
  exit(1);
  }

int decode_DIS_GpuCtrl(struct tcp_chan *chan, struct batch_request *preq)
  {
  fprintf(stderr, "The call to decode_DIS_GpuCtrl needs to be mocked!!\n");
  exit(1);
  }

int decode_DIS_Status(struct tcp_chan *chan, struct batch_request *preq)
  {
  fprintf(stderr, "The call to decode_DIS_Status needs to be mocked!!\n");
  exit(1);
  }

int decode_DIS_Manage(struct tcp_chan *chan, struct batch_request *preq)
  {
  fprintf(stderr, "The call to decode_DIS_Manage needs to be mocked!!\n");
  exit(1);
  }

int decode_DIS_TrackJob(struct tcp_chan *chan, struct batch_request *preq)
  {
  fprintf(stderr, "The call to decode_DIS_TrackJob needs to be mocked!!\n");
  exit(1);
  }

int decode_DIS_JobFile(struct tcp_chan *chan, struct batch_request *preq)
  {
  fprintf(stderr, "The call to decode_DIS_JobFile needs to be mocked!!\n");
  exit(1);
  }

char *pbse_to_txt(int err)
  {
  fprintf(stderr, "The call to pbse_to_txt needs to be mocked!!\n");
  exit(1);
  }

int decode_DIS_ReqHdr(struct tcp_chan *chan, struct batch_request *preq, int *proto_type, int *proto_ver)
  {
  fprintf(stderr, "The call to decode_DIS_ReqHdr needs to be mocked!!\n");
  exit(1);
  }

int decode_DIS_RunJob(struct tcp_chan *chan, struct batch_request *preq)
  {
  fprintf(stderr, "The call to decode_DIS_RunJob needs to be mocked!!\n");
  exit(1);
  }

int decode_DIS_MoveJob(struct tcp_chan *chan, struct batch_request *preq)
  {
  fprintf(stderr, "The call to decode_DIS_MoveJob needs to be mocked!!\n");
  exit(1);
  }

const char *reqtype_to_txt(int reqtype)
  {
  fprintf(stderr, "The call to reqtype_to_txt needs to be mocked!!\n");
  exit(1);
  }

int decode_DIS_Authen(struct tcp_chan *chan, struct batch_request *preq)
  {
  fprintf(stderr, "The call to decode_DIS_Authen needs to be mocked!!\n");
  exit(1);
  }

int decode_DIS_Rescl(struct tcp_chan *chan, struct batch_request *preq)
  {
  fprintf(stderr, "The call to decode_DIS_Rescl needs to be mocked!!\n");
  exit(1);
  }

int decode_DIS_replySvr(struct tcp_chan *chan, struct batch_reply *reply)
  {
  fprintf(stderr, "The call to decode_DIS_replySvr needs to be mocked!!\n");
  exit(1);
  }

int decode_DIS_ShutDown(struct tcp_chan *chan, struct batch_request *preq)
  {
  fprintf(stderr, "The call to decode_DIS_ShutDown needs to be mocked!!\n");
  exit(1);
  }

void log_err(int errnum, const char *routine, const char *text) {}
void log_record(int eventtype, int objclass, const char *objname, const char *text) {}
void log_event(int eventtype, int objclass, const char *objname, const char *text) {}
