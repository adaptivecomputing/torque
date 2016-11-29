#include "license_pbs.h" /* See here for the software license */
#include <stdlib.h>
#include <stdio.h> /* fprintf */

#include "batch_request.h" /* batch_request */
#include "resource.h" /* resource_t */

int node_avail(char *spec, int *navail, int *nalloc, int *nresvd, int *ndown)
  {
  fprintf(stderr, "The call to node_avail to be mocked!!\n");
  exit(1);
  }

int reply_send_svr(struct batch_request *request)
  {
  fprintf(stderr, "The call to reply_send_svr to be mocked!!\n");
  exit(1);
  }

void reply_ack(struct batch_request *preq)
  {
  fprintf(stderr, "The call to reply_ack to be mocked!!\n");
  exit(1);
  }

void req_reject(int code, int aux, struct batch_request *preq, const char *HostName, const char *Msg)
  {
  fprintf(stderr, "The call to req_reject to be mocked!!\n");
  exit(1);
  }

void node_unreserve(resource_t handle)
  {
  fprintf(stderr, "The call to node_unreserve to be mocked!!\n");
  exit(1);
  }

int node_reserve(char *nspec, resource_t tag)
  {
  fprintf(stderr, "The call to node_reserve to be mocked!!\n");
  exit(1);
  }

void set_reply_type(struct batch_reply *preply, int type)
  {
  preply->brp_choice = type;
  }
