#include "license_pbs.h" /* See here for the software license */
#include <stdlib.h>
#include <stdio.h> /* fprintf */

#include "batch_request.h" /* batch_request */
#include "libpbs.h" /* batch_reply */
const char *dis_emsg[10];
int LOGLEVEL = 0;


int decode_DIS_JobId(int sock, char *jobid)
  {
  fprintf(stderr, "The call to decode_DIS_JobId needs to be mocked!!\n");
  exit(1);
  }

int decode_DIS_Register(int sock, struct batch_request *preq)
  {
  fprintf(stderr, "The call to decode_DIS_Register needs to be mocked!!\n");
  exit(1);
  }

int decode_DIS_svrattrl(int sock, tlist_head *phead)
  {
  fprintf(stderr, "The call to decode_DIS_svrattrl needs to be mocked!!\n");
  exit(1);
  }

int decode_DIS_MessageJob(int sock, struct batch_request *preq)
  {
  fprintf(stderr, "The call to decode_DIS_MessageJob needs to be mocked!!\n");
  exit(1);
  }

int decode_DIS_JobObit(int sock, struct batch_request *preq)
  {
  fprintf(stderr, "The call to decode_DIS_JobObit needs to be mocked!!\n");
  exit(1);
  }

int decode_DIS_JobCred(int sock, struct batch_request *preq)
  {
  fprintf(stderr, "The call to decode_DIS_JobCred needs to be mocked!!\n");
  exit(1);
  }

void DIS_tcp_setup(int fd)
  {
  fprintf(stderr, "The call to DIS_tcp_setup needs to be mocked!!\n");
  exit(1);
  }

int decode_DIS_ReqExtend(int sock, struct batch_request *preq)
  {
  fprintf(stderr, "The call to decode_DIS_ReqExtend needs to be mocked!!\n");
  exit(1);
  }

int decode_DIS_QueueJob(int sock, struct batch_request *preq)
  {
  fprintf(stderr, "The call to decode_DIS_QueueJob needs to be mocked!!\n");
  exit(1);
  }

int decode_DIS_SignalJob(int sock, struct batch_request *preq)
  {
  fprintf(stderr, "The call to decode_DIS_SignalJob needs to be mocked!!\n");
  exit(1);
  }

int decode_DIS_AltAuthen(int sock, struct batch_request *preq)
  {
  fprintf(stderr, "The call to decode_DIS_AltAuthen needs to be mocked!!\n");
  exit(1);
  }

int decode_DIS_GpuCtrl(int sock, struct batch_request *preq)
  {
  fprintf(stderr, "The call to decode_DIS_GpuCtrl needs to be mocked!!\n");
  exit(1);
  }

int decode_DIS_Status(int sock, struct batch_request *preq)
  {
  fprintf(stderr, "The call to decode_DIS_Status needs to be mocked!!\n");
  exit(1);
  }

int decode_DIS_Manage(int sock, struct batch_request *preq)
  {
  fprintf(stderr, "The call to decode_DIS_Manage needs to be mocked!!\n");
  exit(1);
  }

int decode_DIS_TrackJob(int sock, struct batch_request *preq)
  {
  fprintf(stderr, "The call to decode_DIS_TrackJob needs to be mocked!!\n");
  exit(1);
  }

int decode_DIS_JobFile(int sock, struct batch_request *preq)
  {
  fprintf(stderr, "The call to decode_DIS_JobFile needs to be mocked!!\n");
  exit(1);
  }

char *pbse_to_txt(int err)
  {
  fprintf(stderr, "The call to pbse_to_txt needs to be mocked!!\n");
  exit(1);
  }

int decode_DIS_ReqHdr(int sock, struct batch_request *preq, int *proto_type, int *proto_ver)
  {
  fprintf(stderr, "The call to decode_DIS_ReqHdr needs to be mocked!!\n");
  exit(1);
  }

int decode_DIS_RunJob(int sock, struct batch_request *preq)
  {
  fprintf(stderr, "The call to decode_DIS_RunJob needs to be mocked!!\n");
  exit(1);
  }

void log_event(int eventtype, int objclass, char *objname, char *text)
  {
  fprintf(stderr, "The call to log_event needs to be mocked!!\n");
  exit(1);
  }

int decode_DIS_MoveJob(int sock, struct batch_request *preq)
  {
  fprintf(stderr, "The call to decode_DIS_MoveJob needs to be mocked!!\n");
  exit(1);
  }

const char *reqtype_to_txt(int reqtype)
  {
  fprintf(stderr, "The call to reqtype_to_txt needs to be mocked!!\n");
  exit(1);
  }

int decode_DIS_Authen(int sock, struct batch_request *preq)
  {
  fprintf(stderr, "The call to decode_DIS_Authen needs to be mocked!!\n");
  exit(1);
  }

int decode_DIS_Rescl(int sock, struct batch_request *preq)
  {
  fprintf(stderr, "The call to decode_DIS_Rescl needs to be mocked!!\n");
  exit(1);
  }

int decode_DIS_replySvr(int sock, struct batch_reply *reply)
  {
  fprintf(stderr, "The call to decode_DIS_replySvr needs to be mocked!!\n");
  exit(1);
  }

int decode_DIS_ShutDown(int sock, struct batch_request *preq)
  {
  fprintf(stderr, "The call to decode_DIS_ShutDown needs to be mocked!!\n");
  exit(1);
  }
