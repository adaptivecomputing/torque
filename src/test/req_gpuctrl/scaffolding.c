#include "license_pbs.h" /* See here for the software license */
#include <stdlib.h>
#include <stdio.h> /* fprintf */

#include "batch_request.h" /* batch_request */

int LOGLEVEL = 7; /* force logging code to be exercised as tests run */

void req_reject(int code, int aux, struct batch_request *preq, const char *HostName, const char *Msg)
  {
  fprintf(stderr, "The call to log_ext to be mocked!!\n");
  exit(1);
  }

batch_request *get_remove_batch_request(

  char *br_id)

  {
  return(NULL);
  }

int get_batch_request_id(

  batch_request *preq)

  {
  return(0);
  }

void log_ext(int errnum, const char *routine, const char *text, int l) {}

int svr_connect(

  pbs_net_t        hostaddr,  /* host order */
  unsigned int     port,   /* I */
  int             *my_err,
  struct pbsnode  *pnode,
  void           *(*func)(void *))

  {
  return(0);
  }

int gpu_entry_by_id(

  struct pbsnode *pnode,  /* I */
  const char     *gpuid,
  int             get_empty)
  
  {
  return(0);
  }

int unlock_node(
    
  struct pbsnode *the_node,
  const char     *id,
  const char     *msg,
  int             logging)
  
  {
  return(0);
  }

int issue_Drequest(

  int                    conn,
  struct batch_request  *request,
  bool                   close_handle)
  
  {
  return(0);
  }

void reply_ack(batch_request *preq) {}

struct pbsnode *find_nodebyname(const char *node_id)
  {
  return(NULL);
  }


void log_err(int errnum, const char *routine, const char *text) {}


unsigned int pbs_mom_port = 0;
