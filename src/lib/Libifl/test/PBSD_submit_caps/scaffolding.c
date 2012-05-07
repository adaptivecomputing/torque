#include "license_pbs.h" /* See here for the software license */
#include <stdlib.h>
#include <stdio.h> /* fprintf */

#include "libpbs.h" /* connect_handle, batch_reply */
#include "u_memmgr.h" /* memmgr */
#include "u_hash_map_structs.h" /* job_data */
#include "attribute.h" /* attropl */

struct connect_handle connection[10];
char pbs_current_user[PBS_MAXUSER];
const char *dis_emsg[10];


ssize_t read_nonblocking_socket(int fd, void *buf, ssize_t count)
  {
  fprintf(stderr, "The call to read_nonblocking_socket needs to be mocked!!\n");
  exit(1);
  }

int encode_DIS_JobId(int sock, char *jobid)
  {
  fprintf(stderr, "The call to encode_DIS_JobId needs to be mocked!!\n");
  exit(1);
  }

char *memmgr_strdup(memmgr **mgr, char *value, int *size)
  {
  fprintf(stderr, "The call to memmgr_strdup needs to be mocked!!\n");
  exit(1);
  }

int encode_DIS_JobFile(int sock, int seq, char *buf, int len, char *jobid, int which)
  {
  fprintf(stderr, "The call to encode_DIS_JobFile needs to be mocked!!\n");
  exit(1);
  }

void DIS_tcp_setup(int fd)
  {
  fprintf(stderr, "The call to DIS_tcp_setup needs to be mocked!!\n");
  exit(1);
  }

int encode_DIS_QueueJob(int sock, char *jobid, char *destin, struct attropl *aoplp)
  {
  fprintf(stderr, "The call to encode_DIS_QueueJob needs to be mocked!!\n");
  exit(1);
  }

int DIS_tcp_wflush(int fd)
  {
  fprintf(stderr, "The call to DIS_tcp_wflush needs to be mocked!!\n");
  exit(1);
  }

struct batch_reply *PBSD_rdrpy(int *local_errno, int c)
  {
  fprintf(stderr, "The call to PBSD_rdrpy needs to be mocked!!\n");
  exit(1);
  }

int encode_DIS_QueueJob_hash(int sock, char *jobid, char *destin, memmgr **mm, job_data *job_attr, job_data *res_attr)
  {
  fprintf(stderr, "The call to encode_DIS_QueueJob_hash needs to be mocked!!\n");
  exit(1);
  }

void PBSD_FreeReply(struct batch_reply *reply)
  {
  fprintf(stderr, "The call to PBSD_FreeReply needs to be mocked!!\n");
  exit(1);
  }

int encode_DIS_ReqExtend(int sock, char *extend)
  {
  fprintf(stderr, "The call to encode_DIS_ReqExtend needs to be mocked!!\n");
  exit(1);
  }

int encode_DIS_ReqHdr(int sock, int reqt, char *user)
  {
  fprintf(stderr, "The call to encode_DIS_ReqHdr needs to be mocked!!\n");
  exit(1);
  }

