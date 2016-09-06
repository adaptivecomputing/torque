#include "license_pbs.h" /* See here for the software license */
#include <stdlib.h>
#include <stdio.h> /* fprintf */
#include <ctype.h>

#include "batch_request.h" /* batch_request */
#include "pbs_job.h" /* job */
#include "server.h" /* server */
#include "attribute.h" /* svrattrl */
#include "list_link.h" /* tlist_head, list_link */
#include "array.h" /* job_array */
#include "work_task.h" /* work_task */
#include "queue.h"

const int DEFAULT_IDLE_SLOT_LIMIT = 300;
const char *msg_illregister = "Illegal op in register request received for job %s";
const char *msg_registerdel = "Job deleted as result of dependency on job %s";
char server_name[PBS_MAXSERVERNAME + 1];
const char *msg_err_malloc = "malloc failed";
const char *PJobState[] = {"hi", "hello"};
struct server server;
int LOGLEVEL = 10;
const char *msg_regrej = "Dependency request for job rejected by ";
const char *msg_registerrel = "Dependency on job %s released.";
int   i = 2;
int   svr = 2;
int   is_attr_set;


struct batch_request *alloc_br(int type)
  {
  fprintf(stderr, "The call to alloc_br to be mocked!!\n");
  exit(1);
  }

int job_save(job *pjob, int updatetype, int mom_port)
  {
  return(0);
  }

void svr_mailowner(job *pjob, int mailpoint, int force, const char *text)
  {
  fprintf(stderr, "The call to svr_mailowner to be mocked!!\n");
  exit(1);
  }

svrattrl *attrlist_create(const char *aname, const char *rname, int vsize)
  {
  svrattrl *pal;
  size_t    asz;
  size_t    rsz;

  asz = strlen(aname) + 1;     /* pbs_attribute name,allow for null term */

  if (rname == NULL)      /* resource name only if type resource */
    rsz = 0;
  else
    rsz = strlen(rname) + 1;

  pal = (svrattrl *)calloc(1, sizeof(svrattrl) + asz + rsz + vsize);

  CLEAR_LINK(pal->al_link); /* clear link */

  pal->al_atopl.next = 0;
  pal->al_tsize = sizeof(svrattrl) + asz + rsz + vsize;  /* set various string sizes */
  pal->al_nameln = asz;
  pal->al_rescln = rsz;
  pal->al_valln  = vsize;
  pal->al_flags  = 0;
  pal->al_op     = SET;

  /* point ptrs to name, resc, and val strings to memory after svrattrl struct */

  pal->al_name = (char *)pal + sizeof(svrattrl);

  pal->al_resc = NULL;

  pal->al_value = pal->al_name + asz + rsz;

  strcpy(pal->al_name, aname); /* copy name right after struct */

  return(pal);
  }

void reply_ack(struct batch_request *preq)
  {
  }

void delete_link(struct list_link *old)
  {
  if ((old->ll_prior != (list_link *)0) &&
      (old->ll_prior != old) && (old->ll_prior->ll_next == old))
    (old->ll_prior)->ll_next = old->ll_next;

  if ((old->ll_next != (list_link *)0) &&
      (old->ll_next != old) && (old->ll_next->ll_prior == old))
    (old->ll_next)->ll_prior = old->ll_prior;

  old->ll_next  = old;

  old->ll_prior = old;
  }

long calc_job_cost(job *pjob)
  {
  return(0);
  }

job_array *get_array(const char *id)
  {
  return(NULL);
  }

void req_reject(int code, int aux, struct batch_request *preq, const char *HostName, const char *Msg) {}

int svr_chk_owner(struct batch_request *preq, job *pjob)
  {
  return(0);
  }

int job_abt(struct job **pjobp, const char *text, bool b=false)
  {
  *pjobp = NULL;
  return(0);
  }

void *get_next(list_link pl, char *file, int line)
  {
  if ((pl.ll_next == NULL) ||
      ((pl.ll_next == &pl) && (pl.ll_struct != NULL)))
    {
    fprintf(stderr, "Assertion failed, bad pointer in link: file \"%s\", line %d\n",
      file, line);

    return(NULL);
    }

  return(pl.ll_next->ll_struct);
  }

void release_req(struct work_task *pwt)
  {
  fprintf(stderr, "The call to release_req to be mocked!!\n");
  exit(1);
  }

char *pbse_to_txt(int err)
  {
  return((char *)"");
  }

void append_link(tlist_head *head, list_link *new_link, void *pobj)
  {
  if (pobj != NULL)
    {
    new_link->ll_struct = pobj;
    }
  else
    {
    new_link->ll_struct = (void *)new_link;
    }

  new_link->ll_prior = head->ll_prior;

  new_link->ll_next  = head;
  head->ll_prior = new_link;
  new_link->ll_prior->ll_next = new_link; /* now visible to forward iteration */
  }

int issue_to_svr(const char *servern, struct batch_request **preq, void (*replyfunc)(struct work_task *))
  {
  fprintf(stderr, "The call to issue_to_svr to be mocked!!\n");
  exit(1);
  }

int que_to_local_svr(struct batch_request *preq)
  {
  fprintf(stderr, "The call to que_to_local_svr to be mocked!!\n");
  exit(1);
  }


int svr_setjobstate(job *pjob, int newstate, int newsubstate, int has_queue_mutex)
  {
  pjob->ji_qs.ji_state = newstate;
  pjob->ji_qs.ji_substate = newsubstate;
  return(0);
  }

job *pGlobalJob = NULL;

job *svr_find_job(const char *jobid, int get_subjob)
  {
  job *pjob = (job *)calloc(1, sizeof(job));
  strcpy(pjob->ji_qs.ji_jobid, jobid);
  CLEAR_HEAD(pjob->ji_wattr[JOB_ATR_depend].at_val.at_list);

  if (!strcmp(jobid, "1.napali"))
    {
    return(pjob);
    }
  else if (!strcmp(jobid, "2.napali"))
    {
    pjob->ji_qs.ji_state = JOB_STATE_COMPLETE;
    return(pjob);
    }
  else if ((pGlobalJob != NULL) &&(!strcmp(pGlobalJob->ji_qs.ji_jobid,jobid)))
    {
    return (pGlobalJob);
    }
  else
    return(NULL);
  }

void svr_evaljobstate(job &pjob, int &newstate, int &newsub, int forceeval)
  {
  newstate = 0;
  newsub = 0;
  }

char *parse_comma_string(char *start, char **ptr) 
  {
  char *pc; /* if start is null, restart from here */

  char     *back;
  char     *rv;

  if ((ptr == NULL) ||
      ((*ptr == NULL) && (start == NULL)))
    return(NULL);

  if (start != NULL)
    pc = start;
  else
    pc = *ptr;

  if (*pc == '\0')
    {
    return(NULL); /* already at end, no strings */
    }

  /* skip over leading white space */

  while ((*pc != '\n') && isspace((int)*pc) && *pc)
    pc++;

  rv = pc;  /* the start point which will be returned */

  /* go find comma or end of line */

  while (*pc)
    {
    if (*pc == '\\')
      {
      if (*++pc == '\0')
        break;
      }
    else if ((*pc == ',') || (*pc == '\n'))
      {
      break;
      }

    ++pc;
    }

  back = pc;

  while (isspace((int)*--back)) /* strip trailing spaces */
    *back = '\0';

  if (*pc != '\0')
    *pc++ = '\0'; /* if not end, terminate this and adv past */

  *ptr = pc;

  return(rv);
  }

int get_svr_attr_l(int index, long *l)
  {
  if (svr == 1)
    *l = SV_STATE_INIT;
  else if (svr == 10)
    *l = 0;
  else
    *l = SV_STATE_RUN;

  return(0);
  }

int get_svr_attr_b(int index, bool *b)
  {
  if (svr == 10)
    *b = false;

  return(0);
  }

pbs_queue *get_jobs_queue(job **pjob)
  {
  return((*pjob)->ji_qhdr);
  }

int unlock_queue(struct pbs_queue *the_queue, const char *id, const char *msg, int logging)
  {
  return(0);
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

void free_br(batch_request *preq) {} 

int unlock_ji_mutex(job *pjob, const char *id, const char *msg, int logging)
  {
  return(0);
  }

int unlock_ai_mutex(job_array *pa, const char *id, const char *msg, int logging)
  {
  return(0);
  }

void log_event(int eventtype, int objclass, const char *objname, const char *text) {}
void log_err(int errnum, const char *routine, const char *text) {}

int safe_strncat(char *str, const char *to_append, size_t space_remaining)
  {
  return(0);
  }

pbs_net_t get_hostaddr(

  int  *local_errno, /* O */    
  const char *hostname)    /* I */

  {
  return(0);
  }

job *job_alloc(void)
  {
  job *pj =  new job();
  return(pj);
  }

char *get_correct_jobname(const char *jobid)

  {
  char *rv = strdup(jobid);
  char *dot;

  if ((dot = strchr(rv, '.')) != NULL)
    *dot = '\0';

  return(rv);
  }

int is_svr_attr_set(int index)

  {
  return(is_attr_set);
  }

job::job() : ji_rejectdest()
  {
  }

job::~job() {}

array_info::array_info() : struct_version(ARRAY_QS_STRUCT_VERSION), array_size(0), num_jobs(0),
                           slot_limit(NO_SLOT_LIMIT), jobs_running(0), jobs_done(0), num_cloned(0),
                           num_started(0), num_failed(0), num_successful(0), num_purged(0),
                           num_idle(0), deps(),
                           idle_slot_limit(DEFAULT_IDLE_SLOT_LIMIT), highest_id_created(-1),
                           range_str()

  {
  }

job_array::job_array() : job_ids(NULL), jobs_recovered(0), ai_ghost_recovered(false), uncreated_ids(),
                         ai_mutex(NULL), ai_qs()

  {
  }
