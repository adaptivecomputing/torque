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
#include "log.h"

bool return_addr = true;
bool local_connect = true;
bool net_rc_retry = false;
bool connect_error = false;

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

int pthread_mutex_lock(pthread_mutex_t *mutex) throw()
  {
  return(0);
  }

int pthread_mutex_unlock(pthread_mutex_t *mutex) throw()
  {
  return(0);
  }

char *parse_servername(const char *name, unsigned int *service)
  {
  return(NULL);
  }

int encode_DIS_Register(struct tcp_chan *chan, struct batch_request *preq)
  {
  return(0);
  }

int PBSD_gpu_put(int c, char *node, char *gpuid, int gpumode, int reset_perm, int reset_vol, char *extend)
  {
  return(0);
  }

int PBSD_mgr_put(int c, int function, int command, int objtype, char *objname, struct attropl *aoplp, char *extend)
  {
  return(0);
  }

int encode_DIS_JobId(struct tcp_chan *chan, char *jobid)
  {
  return(0);
  }

int encode_DIS_PowerState(struct tcp_chan *chan, unsigned short powerState)
  {
  return(0);
  }

pbs_net_t get_hostaddr(int *local_errno, const char *hostname)
  {
  if (return_addr == true)
    {
    *local_errno = 0;
    return(123445);
    }

  *local_errno = PBS_NET_RC_RETRY;
  return(0);
  }

int DIS_reply_read(struct tcp_chan *chan, struct batch_reply *preply)
  {
  return(0);
  }

char *netaddr_pbs_net_t(pbs_net_t ipadd)
  {
  return(strdup("Mock address."));
  }

void free_br(struct batch_request *preq)
  {
  }

int encode_DIS_ReturnFiles(struct tcp_chan *chan, struct batch_request *preq)
  {
  return(0);
  }

int enqueue_threadpool_request(

  void *(*func)(void *),
  void *arg)
  
  {
  return(PBSE_NONE);
  }


int insert_timed_task(

    work_task *wt)

  {
  return(PBSE_NONE);
  }


struct work_task *set_task(

  enum work_type   type,
  long             event_id,  /* I - based on type can be time of event */
  void           (*func)(struct work_task *),
  void            *parm,
  int              get_lock)

  {
  work_task *pnew;

  if ((pnew = (struct work_task *)calloc(1, sizeof(struct work_task))) == NULL)
    {
    return(NULL);
    }

  pnew->wt_event    = event_id;
  pnew->wt_type     = type;
  pnew->wt_func     = func;
  pnew->wt_parm1    = parm;

  if (type == WORK_Immed)
    {
    enqueue_threadpool_request((void *(*)(void *))func,pnew);
    }
  else
    {
    if ((pnew->wt_mutex = (pthread_mutex_t*)calloc(1, sizeof(pthread_mutex_t))) == NULL)
      {
      free(pnew);
      return(NULL);
      }

    pthread_mutex_init(pnew->wt_mutex,NULL);
    pthread_mutex_lock(pnew->wt_mutex);

    insert_timed_task(pnew);

    /* only keep the lock if they want it */
    if (get_lock == FALSE)
      pthread_mutex_unlock(pnew->wt_mutex);
    }

  return(pnew);
  }  /* END set_task() */


void DIS_tcp_setup(int fd)
  {
  return;
  }

int encode_DIS_TrackJob(struct tcp_chan *chan, struct batch_request *br)
  {
  return(0);
  }

void svr_disconnect(int handle)
  {
  return;
  }

int DIS_tcp_wflush(tcp_chan *chan)
  {
  return(0);
  }

void delete_task(struct work_task *ptask)
  {
  return;
  }

void *get_next(list_link pl, char *file, int line)
  {
  return(NULL);
  }

int encode_DIS_CopyFiles(struct tcp_chan *chan, struct batch_request *preq)
  {
  return(0);
  }

int encode_DIS_ReqHdr(struct tcp_chan *chan, int reqt, char *user)
  {
  return(0);
  }

void attrl_fixlink(tlist_head *phead)
  {
  return;
  }

int PBSD_status_put(int c, int function, char *id, struct attrl *attrib, char *extend)
  {
  return(0);
  }

work_task *next_task(all_tasks *at, int *iter)
  {
  return(NULL);
  }

int svr_connect(pbs_net_t hostaddr, unsigned int port, int *my_err, struct pbsnode  *pnode, void *(*func)(void *))
  {
  if (local_connect == true)
    return PBS_LOCAL_CONNECTION;

  if (net_rc_retry == true)
    return(PBS_NET_RC_RETRY);

  if (connect_error == true)
    return(-5);
  
  return(10);
  }

int dispatch_task(struct work_task *ptask)
  {
  return(0);
  }

int dispatch_request(int sfds, struct batch_request *request)
  {
  return(PBSE_NONE);
  }

int PBSD_msg_put(int c, char *jobid, int fileopt, char *msg, char *extend)
  {
  return(0);
  }

void close_conn(int sd, int has_mutex)
  {
  return;
  }

struct pbsnode dummynode;

struct pbsnode *tfind_addr(const u_long key, uint16_t port, char *job_momname)
  {
  memset(&dummynode,0,sizeof(dummynode));
  dummynode.nd_name = strdup("tmp");
  return &dummynode;
  }

int encode_DIS_ReqExtend(struct tcp_chan *chan, char *extend)
  {
  return(0);
  }

int PBSD_sig_put(int c, char *jobid, char *signal, char *extend)
  {
  return(0);
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
  char buf[MAXLINE];
  int  id = 325;

  sprintf(buf, "%d", id);

  preq->rq_id = strdup(buf);

  return(0);
  }

job *svr_find_job(const char *jobid, int get_subjob)
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
  return(0);
  }

int copy_attribute_list(

  batch_request *preq,
  batch_request *preq_tmp)

  {
  svrattrl             *pal = (svrattrl *)GET_NEXT(preq->rq_ind.rq_manager.rq_attr);
  tlist_head           *phead = &preq_tmp->rq_ind.rq_manager.rq_attr;
  svrattrl             *newpal = NULL;

  while (pal != NULL)
    {
    newpal = (svrattrl *)calloc(1, pal->al_tsize + 1);
    if (!newpal)
      {
      free_br(preq_tmp);
      return(PBSE_SYSTEM);
      }

    CLEAR_LINK(newpal->al_link);

    newpal->al_atopl.next = 0;
    newpal->al_tsize = pal->al_tsize + 1;
    newpal->al_nameln = pal->al_nameln;
    newpal->al_flags  = pal->al_flags;
    newpal->al_atopl.name = (char *)newpal + sizeof(svrattrl);
    strcpy((char *)newpal->al_atopl.name, pal->al_atopl.name);
    newpal->al_nameln = pal->al_nameln;
    newpal->al_atopl.resource = newpal->al_atopl.name + newpal->al_nameln;

    if (pal->al_atopl.resource != NULL)
      strcpy((char *)newpal->al_atopl.resource, pal->al_atopl.resource);

    newpal->al_rescln = pal->al_rescln;
    newpal->al_atopl.value = newpal->al_atopl.name + newpal->al_nameln + newpal->al_rescln;
    strcpy((char *)newpal->al_atopl.value, pal->al_atopl.value);
    newpal->al_valln = pal->al_valln;
    newpal->al_atopl.op = pal->al_atopl.op;

    pal = (struct svrattrl *)GET_NEXT(pal->al_link);
    }

  if ((phead != NULL) &&
       (newpal != NULL))
    append_link(phead, &newpal->al_link, newpal);

  return(PBSE_NONE);
  } /* END copy_attribute_list() */


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
  preq_tmp->rq_extsz = preq->rq_extsz;
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
      if (copy_attribute_list(preq, preq_tmp) != PBSE_NONE)
        return(NULL);
    
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

void update_failure_counts(

  const char *node_name,
  int         rc)

  {
  }

int PBSD_sig_put(int c, const char *jobid, const char *signal, char *extend)
  {
  return(0);
  }

int PBSD_mgr_put(int c, int function, int command, int objtype, const char *objname, struct attropl *aoplp, char *extend)
  {
  return(0);
  }

void set_reply_type(struct batch_reply *preply, int type)
  {
  preply->brp_choice = type;
  }
