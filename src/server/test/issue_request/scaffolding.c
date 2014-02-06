#include "license_pbs.h" /* See here for the software license */
#include <string>
#include <stdlib.h>
#include <stdio.h> /* fprintf */
#include <pthread.h>

#include "libpbs.h" /* connect_handle */
#include "net_connect.h" /* connection */
#include "work_task.h" /* all_tasks */
#include "batch_request.h" /* batch_request */
#include "pbs_ifl.h" /* attropl */
#include "net_connect.h" /* pbs_net_t */
#include "list_link.h" /* tlist_head, list_link */
#include "pbs_nodes.h"

int pbs_errno = 0;
const char *msg_daemonname = "unset";
struct connect_handle connection[10];
struct connection svr_conn[PBS_NET_MAX_CONNECTIONS];
const char *msg_norelytomom = "Server could not connect to MOM";
unsigned int pbs_server_port_dis;
int LOGLEVEL = 7; /* force logging code to be exercised as tests run */
all_tasks task_list_event;
const char *msg_issuebad = "attempt to issue invalid request of type %d";
std::string rq_id_str;

char *parse_servername(char *name, unsigned int *service)
  {
  fprintf(stderr, "The call to parse_servername needs to be mocked!!\n");
  return(NULL);
  }

int encode_DIS_Register(struct tcp_chan *chan, struct batch_request *preq)
  {
  fprintf(stderr, "The call to encode_DIS_Register needs to be mocked!!\n");
  return(1);
  }

int PBSD_gpu_put(int c, char *node, char *gpuid, int gpumode, int reset_perm, int reset_vol, char *extend)
  {
  fprintf(stderr, "The call to PBSD_gpu_put needs to be mocked!!\n");
  return(1);
  }

int PBSD_mgr_put(int c, int function, int command, int objtype, char *objname, struct attropl *aoplp, char *extend)
  {
  fprintf(stderr, "The call to PBSD_mgr_put needs to be mocked!!\n");
  return(1);
  }

int encode_DIS_JobId(struct tcp_chan *chan, char *jobid)
  {
  fprintf(stderr, "The call to encode_DIS_JobId needs to be mocked!!\n");
  return(1);
  }

pbs_net_t get_hostaddr(int *local_errno, char *hostname)
  {
  fprintf(stderr, "The call to get_hostaddr needs to be mocked!!\n");
  return(1);
  }

int DIS_reply_read(struct tcp_chan *chan, struct batch_reply *preply)
  {
  fprintf(stderr, "The call to DIS_reply_read needs to be mocked!!\n");
  return(1);
  }

char *netaddr_pbs_net_t(pbs_net_t ipadd)
  {
  return(strdup("Mock address."));
  }

void free_br(struct batch_request *preq)
  {
  fprintf(stderr, "The call to free_br needs to be mocked!!\n");
  return;
  }

int encode_DIS_ReturnFiles(struct tcp_chan *chan, struct batch_request *preq)
  {
  fprintf(stderr, "The call to encode_DIS_ReturnFiles needs to be mocked!!\n");
  return(1);
  }

struct work_task *set_task(

  enum work_type   type,
  long             event_id,  /* I - based on type can be time of event */
  void           (*func)(struct work_task *),
  void            *parm,
  int              get_lock)

  {
  work_task *pnew = (work_task *)calloc(1, sizeof(work_task));
  pnew->wt_mutex = (pthread_mutex_t *)calloc(1, sizeof(pthread_mutex_t));

  rq_id_str = (char *)parm;

  return(pnew);
  }  /* END set_task() */


void DIS_tcp_setup(int fd)
  {
  fprintf(stderr, "The call to DIS_tcp_setup needs to be mocked!!\n");
  return;
  }

int encode_DIS_TrackJob(struct tcp_chan *chan, struct batch_request *br)
  {
  fprintf(stderr, "The call to encode_DIS_TrackJob needs to be mocked!!\n");
  return(1);
  }

void svr_disconnect(int handle)
  {
  fprintf(stderr, "The call to svr_disconnect needs to be mocked!!\n");
  return;
  }

int DIS_tcp_wflush(tcp_chan *chan)
  {
  fprintf(stderr, "The call to DIS_tcp_wflush needs to be mocked!!\n");
  return(1);
  }

void delete_task(struct work_task *ptask)
  {
  fprintf(stderr, "The call to delete_task needs to be mocked!!\n");
  return;
  }

void *get_next(list_link pl, char *file, int line)
  {
  fprintf(stderr, "The call to get_next needs to be mocked!!\n");
  return(NULL);
  }

int encode_DIS_CopyFiles(struct tcp_chan *chan, struct batch_request *preq)
  {
  fprintf(stderr, "The call to encode_DIS_CopyFiles needs to be mocked!!\n");
  return(1);
  }

int encode_DIS_ReqHdr(struct tcp_chan *chan, int reqt, char *user)
  {
  fprintf(stderr, "The call to encode_DIS_ReqHdr needs to be mocked!!\n");
  return(1);
  }

void attrl_fixlink(tlist_head *phead)
  {
  fprintf(stderr, "The call to attrl_fixlink needs to be mocked!!\n");
  return;
  }

int PBSD_status_put(int c, int function, char *id, struct attrl *attrib, char *extend)
  {
  fprintf(stderr, "The call to PBSD_status_put needs to be mocked!!\n");
  return(1);
  }

work_task *next_task(all_tasks *at, int *iter)
  {
  fprintf(stderr, "The call to next_task needs to be mocked!!\n");
  return(NULL);
  }

int svr_connect(pbs_net_t hostaddr, unsigned int port, int *my_err, struct pbsnode  *pnode, void *(*func)(void *))
  {
  return PBS_LOCAL_CONNECTION;
  }

int dispatch_task(struct work_task *ptask)
  {
  fprintf(stderr, "The call to dispatch_task needs to be mocked!!\n");
  return(1);
  }

int dispatch_request(int sfds, struct batch_request *request)
  {
  return 0;
  }

int PBSD_msg_put(int c, char *jobid, int fileopt, char *msg, char *extend)
  {
  fprintf(stderr, "The call to PBSD_msg_put needs to be mocked!!\n");
  return(1);
  }

void close_conn(int sd, int has_mutex)
  {
  fprintf(stderr, "The call to close_conn needs to be mocked!!\n");
  return;
  }

struct pbsnode dummynode;

struct pbsnode *tfind_addr(const u_long key, uint16_t port, char *job_momname)
  {
  memset(&dummynode,0,sizeof(dummynode));
  return &dummynode;
  }

int encode_DIS_ReqExtend(struct tcp_chan *chan, char *extend)
  {
  fprintf(stderr, "The call to encode_DIS_ReqExtend needs to be mocked!!\n");
  return(1);
  }

int PBSD_sig_put(int c, char *jobid, char *signal, char *extend)
  {
  fprintf(stderr, "The call to PBSD_sig_put needs to be mocked!!\n");
  return(1);
  }

int unlock_node(struct pbsnode *the_node, const char *id, const char *msg, int logging)
  {
  return 0;
  }

void DIS_tcp_cleanup(struct tcp_chan *chan)
  {
  }

/*
 * alloc_br - allocate and clear a batch_request structure
 */

struct batch_request *alloc_br(

  int type)

  {

  struct batch_request *req = NULL;

  if ((req = (struct batch_request *)calloc(1, sizeof(struct batch_request))) == NULL)
    {
    fprintf(stderr, "failed to allocate batch request. alloc_br()\n");
    }
  else
    {

    req->rq_type = type;

    req->rq_conn = -1;  /* indicate not connected */
    req->rq_orgconn = -1;  /* indicate not connected */
    req->rq_time = time(NULL);
    req->rq_reply.brp_choice = BATCH_REPLY_CHOICE_NULL;
    req->rq_noreply = FALSE;  /* indicate reply is needed */
    }

  return(req);
  } /* END alloc_br() */

batch_request *get_remove_batch_request(

  char *br_id)

  {
  return(NULL);
  }

int get_batch_request_id(

  batch_request *preq)

  {
  preq->rq_id = strdup("321");
  return(0);
  }

job *svr_find_job(char *jobid, int get_subjob)
  {
  return(NULL);
  }

int unlock_ji_mutex(job *pjob, const char *id, const char *msg, int logging)
  {
  return(0);
  }

void log_err(int errnum, const char *routine, const char *text) {}
void log_record(int eventtype, int objclass, const char *objname, const char *text) {}
void log_event(int eventtype, int objclass, const char *objname, const char *text) {}

int ctnodes(char *spec)
  {
  fprintf(stderr, "The call to append_link needs to be mocked!!\n");
  return(1);
  }

batch_request *duplicate_request(batch_request *preq, int job_index)
  {
  batch_request *preq_tmp = alloc_br(preq->rq_type);
  char          *ptr1;
  char          *ptr2;
  char           newjobname[PBS_MAXSVRJOBID+1];

  if (preq_tmp == NULL)
    return(NULL);

  preq_tmp->rq_perm = preq->rq_perm;
  preq_tmp->rq_fromsvr = preq->rq_fromsvr;
  preq_tmp->rq_conn = preq->rq_conn;
  preq_tmp->rq_time = preq->rq_time;
  preq_tmp->rq_orgconn = preq->rq_orgconn;

  memcpy(preq_tmp->rq_ind.rq_manager.rq_objname,
  preq->rq_ind.rq_manager.rq_objname, PBS_MAXSVRJOBID + 1);

  strcpy(preq_tmp->rq_user, preq->rq_user);
  strcpy(preq_tmp->rq_host, preq->rq_host);

  if (preq->rq_extend != NULL)
    preq_tmp->rq_extend = strdup(preq->rq_extend);

  switch (preq->rq_type)
    {
    /* This function was created for a modify array request (PBS_BATCH_ModifyJob)
    the preq->rq_ind structure was allocated in dis_request_read. If other
    BATCH types are needed refer to that function to see how the rq_ind structure
    was allocated and then copy it here. */
    case PBS_BATCH_DeleteJob:
    case PBS_BATCH_HoldJob:
    case PBS_BATCH_CheckpointJob:
    case PBS_BATCH_ModifyJob:
    case PBS_BATCH_AsyModifyJob:

      /* based on how decode_DIS_Manage allocates data */
      CLEAR_HEAD(preq_tmp->rq_ind.rq_manager.rq_attr);

      preq_tmp->rq_ind.rq_manager.rq_cmd = preq->rq_ind.rq_manager.rq_cmd;
      preq_tmp->rq_ind.rq_manager.rq_objtype = preq->rq_ind.rq_manager.rq_objtype;

      if (job_index != -1)
        {
        /* If this is a job array it is possible we only have the array name
        and not the individual job. We need to find out what we have and
        modify the name if needed */
        ptr1 = strstr(preq->rq_ind.rq_manager.rq_objname, "[]");
        if (ptr1)
          {
          ptr1++;
          strcpy(newjobname, preq->rq_ind.rq_manager.rq_objname);
          ptr2 = strstr(newjobname, "[]");
          ptr2++;
          *ptr2 = 0;
          sprintf(preq_tmp->rq_ind.rq_manager.rq_objname,"%s%d%s",
                            newjobname, job_index, ptr1);
          }
        else
          strcpy(preq_tmp->rq_ind.rq_manager.rq_objname, preq->rq_ind.rq_manager.rq_objname);
        }
    
      /* copy the attribute list */
     // if (copy_attribute_list(preq, preq_tmp) != PBSE_NONE)
     //   return(NULL);
    
      break;
    
      case PBS_BATCH_SignalJob:
    
        strcpy(preq_tmp->rq_ind.rq_signal.rq_jid, preq->rq_ind.rq_signal.rq_jid);
        strcpy(preq_tmp->rq_ind.rq_signal.rq_signame, preq->rq_ind.rq_signal.rq_signame);
        preq_tmp->rq_extra = strdup((char *)preq->rq_extra);
    
        break;
    
      case PBS_BATCH_MessJob:
    
        strcpy(preq_tmp->rq_ind.rq_message.rq_jid, preq->rq_ind.rq_message.rq_jid);
        preq_tmp->rq_ind.rq_message.rq_file = preq->rq_ind.rq_message.rq_file;
        strcpy(preq_tmp->rq_ind.rq_message.rq_text, preq->rq_ind.rq_message.rq_text);
    
        break;
    
      case PBS_BATCH_RunJob:
    
        if (preq->rq_ind.rq_run.rq_destin)
          preq_tmp->rq_ind.rq_run.rq_destin = strdup(preq->rq_ind.rq_run.rq_destin);
    
        break;
    
      default:
    
        break;
      }
  
    return(preq_tmp);
  }
