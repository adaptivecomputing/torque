#include "license_pbs.h" /* See here for the software license */
#include <stdlib.h>
#include <stdio.h> /* fprintf */
#include <pthread.h>

#include "pbs_ifl.h" /* MAXPATHLEN, PBS_MAXSERVERNAME */
#include "server.h" /* server, NO_BUFFER_SPACE */
#include "pbs_job.h" /* all_jobs, job_array, job */
#include "resizable_array.h" /* resizable_array */
#include "attribute.h" /* pbs_attribute, attribute_def */
#include "net_connect.h" /* pbs_net_t */
#include "list_link.h" /* list_link */
#include "hash_table.h" /* hash_table_t */
#include "batch_request.h" /* batch_request */
#include "work_task.h" /* all_tasks */
#include "array.h" /* ArrayEventsEnum */
#include "dynamic_string.h" /* dynamic_string */

/* This section is for manipulting function return values */
#include "test_job_func.h" /* *_SUITE */
#include "user_info.h"
int func_num = 0; /* Suite number being run */
int tc = 0; /* Used for test routining */
int iter_num = 0;

int valbuf_size = 0;
/* end manip */

char *path_jobs;
char path_checkpoint[MAXPATHLEN + 1];
char *job_log_file = NULL;
char server_name[PBS_MAXSERVERNAME + 1];
char *msg_abt_err = "Unable to abort Job %s which was in substate %d";
int queue_rank = 0;
char *path_spool;
char *msg_err_purgejob = "Unlink of job file failed";
struct server server;
struct all_jobs array_summary;
char *path_jobinfo_log;
int LOGLEVEL = 0;
pthread_mutex_t *job_log_mutex;

user_info_holder users;

int array_save(job_array *pa)
  {
  fprintf(stderr, "The call to array_save needs to be mocked!!\n");
  exit(1);
  }

int insert_thing(resizable_array *ra, void *thing)
  {
  fprintf(stderr, "The call to insert_thing needs to be mocked!!\n");
  exit(1);
  }

void account_record(int acctype, job *pjob, char *text)
  {
  fprintf(stderr, "The call to account_record needs to be mocked!!\n");
  exit(1);
  }

char *arst_string(char *str, pbs_attribute *pattr)
  {
  fprintf(stderr, "The call to arst_string needs to be mocked!!\n");
  exit(1);
  }

int job_save(job *pjob, int updatetype, int mom_port)
  {
  fprintf(stderr, "The call to job_save needs to be mocked!!\n");
  exit(1);
  }

void svr_mailowner(job *pjob, int mailpoint, int force, char *text)
  {
  fprintf(stderr, "The call to svr_mailowner needs to be mocked!!\n");
  exit(1);
  }

int remove_thing_from_index(resizable_array *ra, int index)
  {
  fprintf(stderr, "The call to remove_thing_from_index needs to be mocked!!\n");
  exit(1);
  }

ssize_t read_nonblocking_socket(int fd, void *buf, ssize_t count)
  {
  fprintf(stderr, "The call to read_nonblocking_socket needs to be mocked!!\n");
  exit(1);
  }

void clear_attr(pbs_attribute *pattr, attribute_def *pdef)
  {
  memset(pattr, 0, sizeof(pbs_attribute));

  pattr->at_type = pdef->at_type;

  if ((pattr->at_type == ATR_TYPE_RESC) ||
      (pattr->at_type == ATR_TYPE_LIST))
    {
    CLEAR_HEAD(pattr->at_val.at_list);
    }
  }

pbs_net_t get_hostaddr(int *local_errno, char *hostname)
  {
  fprintf(stderr, "The call to get_hostaddr needs to be mocked!!\n");
  exit(1);
  }

int log_job_record(char *buf)
  {
  int rc = 0;
  if ((func_num == RECORD_JOBINFO_SUITE) && (tc == 2))
    rc = -1;
  if ((func_num == RECORD_JOBINFO_SUITE) && (tc == 8))
    {
    if (iter_num == 1)
      rc = -1;
    iter_num++;
    }
  return rc;
  }

int job_log_open(char *filename, char *directory)
  {
  int rc = 0;
  if ((func_num == RECORD_JOBINFO_SUITE) && (tc == 1))
    rc = -1;
  return rc;
  }

void delete_link(struct list_link *old)
  {
  fprintf(stderr, "The call to delete_link needs to be mocked!!\n");
  exit(1);
  }

int add_hash(hash_table_t *ht, int value, void *key)
  {
  fprintf(stderr, "The call to add_hash needs to be mocked!!\n");
  exit(1);
  }

void free_br(struct batch_request *preq)
  {
  fprintf(stderr, "The call to free_br needs to be mocked!!\n");
  exit(1);
  }

struct work_task *set_task(enum work_type type, long event_id, void (*func)(), void *parm, int get_lock)
  {
  fprintf(stderr, "The call to work_task needs to be mocked!!\n");
  exit(1);
  }

int svr_dequejob(char *job_id, int val)
  {
  fprintf(stderr, "The call to svr_dequejob needs to be mocked!!\n");
  exit(1);
  }

ssize_t write_nonblocking_socket(int fd, const void *buf, ssize_t count)
  {
  fprintf(stderr, "The call to write_nonblocking_socket needs to be mocked!!\n");
  exit(1);
  }

void initialize_all_tasks_array(all_tasks *at)
  {
  fprintf(stderr, "The call to initialize_all_tasks_array needs to be mocked!!\n");
  exit(1);
  }

job_array *get_array(char *id)
  {
  fprintf(stderr, "The call to get_array needs to be mocked!!\n");
  exit(1);
  }

job *get_recycled_job()
  {
  fprintf(stderr, "The call to get_recycled_job needs to be mocked!!\n");
  exit(1);
  }

int get_value_hash(hash_table_t *ht, void *key)
  {
  fprintf(stderr, "The call to get_value_hash needs to be mocked!!\n");
  exit(1);
  }

void delete_task(struct work_task *ptask)
  {
  fprintf(stderr, "The call to delete_task needs to be mocked!!\n");
  exit(1);
  }

int depend_on_term(char *job_id)
  {
  fprintf(stderr, "The call to depend_on_term needs to be mocked!!\n");
  exit(1);
  }

int client_to_svr(pbs_net_t hostaddr, unsigned int port, int local_port, char *EMsg)
  {
  fprintf(stderr, "The call to client_to_svr needs to be mocked!!\n");
  exit(1);
  }

void *get_next(list_link pl, char *file, int line)
  {
  fprintf(stderr, "The call to get_next needs to be mocked!!\n");
  exit(1);
  }

int issue_signal(job *pjob, char *signame, void (*func)(struct work_task *), void *extra)
  {
  fprintf(stderr, "The call to issue_signal needs to be mocked!!\n");
  exit(1);
  }

resizable_array *initialize_resizable_array(int size)
  {
  fprintf(stderr, "The call to initialize_resizable_array needs to be mocked!!\n");
  exit(1);
  }

int svr_enquejob(job *pjob, int has_sv_qs_mutex, int prev_index)
  {
  fprintf(stderr, "The call to svr_enquejob needs to be mocked!!\n");
  exit(1);
  }

void update_array_values(job_array *pa, int old_state, enum ArrayEventsEnum event, char *job_id, long job_atr_hold, int job_exit_status)
  {
  fprintf(stderr, "The call to update_array_values needs to be mocked!!\n");
  exit(1);
  }

int array_delete(job_array *pa)
  {
  fprintf(stderr, "The call to array_delete needs to be mocked!!\n");
  exit(1);
  }

void *next_thing(resizable_array *ra, int *iter)
  {
  fprintf(stderr, "The call to next_thing needs to be mocked!!\n");
  exit(1);
  }

void release_req(struct work_task *pwt)
  {
  fprintf(stderr, "The call to release_req needs to be mocked!!\n");
  exit(1);
  }

hash_table_t *create_hash(int size)
  {
  fprintf(stderr, "The call to create_hash needs to be mocked!!\n");
  exit(1);
  }

work_task *next_task(all_tasks *at, int *iter)
  {
  fprintf(stderr, "The call to next_task needs to be mocked!!\n");
  exit(1);
  }

int swap_things(resizable_array *ra, void *thing1, void *thing2)
  {
  fprintf(stderr, "The call to swap_things needs to be mocked!!\n");
  exit(1);
  }

int insert_thing_after(resizable_array *ra, void *thing, int index)
  {
  fprintf(stderr, "The call to insert_thing_after needs to be mocked!!\n");
  exit(1);
  }

void issue_track(job *pjob)
  {
  fprintf(stderr, "The call to issue_track needs to be mocked!!\n");
  exit(1);
  }

int svr_setjobstate(job *pjob, int newstate, int newsubstate, int  has_queue_mute)
  {
  fprintf(stderr, "The call to svr_setjobstate needs to be mocked!!\n");
  exit(1);
  }

struct batch_request *setup_cpyfiles(struct batch_request *preq, job *pjob, char *from, char *to, int direction, int tflag)
  {
  fprintf(stderr, "The call to setup_cpyfiles needs to be mocked!!\n");
  exit(1);
  }

int insert_into_recycler(job *pjob)
  {
  fprintf(stderr, "The call to insert_into_recycler needs to be mocked!!\n");
  exit(1);
  }

int attr_to_str(struct dynamic_string *ds, attribute_def *attr_def,struct pbs_attribute attr,int XML)
  {
  int rc = 0;
  if ((func_num == RECORD_JOBINFO_SUITE) && (tc == 4))
    {
    if (iter_num == 0)
      {
      iter_num++;
      rc = NO_BUFFER_SPACE;
      }
    else if (iter_num == 1)
      {
      iter_num++;
      }
    }
  else if ((func_num == RECORD_JOBINFO_SUITE) && (tc == 5))
    {
    rc = NO_BUFFER_SPACE;
    }
  else if ((func_num == RECORD_JOBINFO_SUITE) && (tc == 6))
    {
    iter_num++;
    }
  return rc;
  }

void check_job_log(struct work_task *ptask)
  {
  fprintf(stderr, "The call to check_job_log needs to be mocked!!\n");
  exit(1);
  }

void svr_evaljobstate(job *pjob, int *newstate, int *newsub, int forceeval)
  {
  fprintf(stderr, "The call to svr_evaljobstate needs to be mocked!!\n");
  exit(1);
  }

int remove_hash(hash_table_t *ht, char *key)
  {
  fprintf(stderr, "The call to remove_hash needs to be mocked!!\n");
  exit(1);
  }

void *next_thing_from_back(resizable_array *ra, int *iter)
  {
  fprintf(stderr, "The call to next_thing_from_back needs to be mocked!!\n");
  exit(1);
  }

void change_value_hash(hash_table_t *ht, char *key, int new_value)
  {
  fprintf(stderr, "The call to change_value_hash needs to be mocked!!\n");
  exit(1);
  }

int lock_queue(struct pbs_queue *the_queue, const char *method_name, char *msg, int logging)
  {
  return(0);
  }


size_t need_to_grow(

  dynamic_string *ds,
  const char     *to_check)

  {
  size_t to_add = strlen(to_check) + 1;
  size_t to_grow = 0;

  if (ds->size < ds->used + to_add)
    {
    to_grow = to_add + ds->size;

    if (to_grow < (ds->size * 4))
      to_grow = ds->size * 4;
    }

  return(to_grow);
  } /* END need_to_grow() */




int resize_if_needed(

  dynamic_string *ds,
  const char     *to_check)

  {
  size_t  new_size = need_to_grow(ds, to_check);
  size_t  difference;
  char   *tmp;

  if (new_size > 0)
    {
    /* need to resize */
    difference = new_size - ds->size;

    if ((tmp = realloc(ds->str, new_size)) == NULL)
      return(-1);

    ds->str = tmp;
    /* zero out the new space as well */
    memset(ds->str + ds->size, 0, difference);
    ds->size = new_size;
    }

  return(PBSE_NONE);
  } /* END resize_if_needed() */

int append_dynamic_string(dynamic_string *ds, const char *to_append)
  {
  int len = strlen(to_append);
  int add_one = FALSE;
  int offset = ds->used;

  if (ds->used == 0)
    add_one = TRUE;
  else
    offset -= 1;

  resize_if_needed(ds, to_append);
  strcat(ds->str + offset, to_append);
    
  ds->used += len;

  if (add_one == TRUE)
    ds->used += 1;

  return(PBSE_NONE);
  }

void clear_dynamic_string(dynamic_string *ds)
  {
  fprintf(stderr, "The call to attr_to_str needs to be mocked!!\n");
  exit(1);
  }

dynamic_string *get_dynamic_string(int initial_size, const char *str)
  {
  dynamic_string *ds = calloc(1, sizeof(dynamic_string));

  if (ds == NULL)
    return(ds);

  if (initial_size > 0)
    ds->size = initial_size;
  else
    ds->size = DS_INITIAL_SIZE;
    
  ds->str = calloc(1, ds->size);

  if (ds->str == NULL)
    {
    free(ds);
    return(NULL);
    }
    
  /* initialize empty str */
  ds->used = 0;

  /* add the string if it exists */
  if (str != NULL)
    {
    if (append_dynamic_string(ds,str) != PBSE_NONE)
      {
      free_dynamic_string(ds);
      return(NULL);
      }
    }

  return(ds);
  }

void free_dynamic_string(dynamic_string *ds) {}

int get_svr_attr_l(int index, long *l)
  {
  return(0);
  }

int is_svr_attr_set(int index)
  {
  return(0);
  }

int get_svr_attr_str(int index, char **str)
  {
  return(0);
  }

int unlock_queue(struct pbs_queue *the_queue, const char *id, char *msg, int logging)
  {
  return(0);
  }

struct pbs_queue *lock_queue_with_job_held(

  struct pbs_queue  *pque,
  job       **pjob_ptr)

  {
  return(NULL);
  }

int decode_ll(

  pbs_attribute *patr,
  char          *name,  /* pbs_attribute name */
  char          *rescn, /* resource name, unused here */
  char          *val,   /* pbs_attribute value */
  int            perm)  /* only used for resources */

  {
  return(0);
  }

int set_ll(
   
  pbs_attribute *attr, 
  pbs_attribute *new,
  enum batch_op  op)

  {
  return(0);
  }

int encode_ll(

  pbs_attribute  *attr,   /* ptr to pbs_attribute */
  tlist_head     *phead,   /* head of attrlist list */
  char           *atname,  /* pbs_attribute name */
  char           *rsname,  /* resource name or null */
  int             mode,   /* encode mode, unused here */
  int             perm)  /* only used for resources */

  {
  return(0);
  }

int comp_ll(
   
  pbs_attribute *attr,
  pbs_attribute *with)

  {
  return(0);
  }

int remove_alps_reservation(
    
  char *rsv_id)

  {
  return(0);
  }

int unlock_ji_mutex(job *pjob, const char *id, char *msg, int logging)
  {
  return(0);
  }

int lock_ji_mutex(job *pjob, const char *id, char *msg, int logging)
  {
  return(0);
  }

int  decrement_queued_jobs(user_info_holder *uih, char *user_name)
  {
  return(0);
  }

int set_str(
    
  pbs_attribute *attr,
  pbs_attribute *new,
  enum batch_op  op)

  {
  char *new_value;
  char *p;
  size_t nsize;

  nsize = strlen(new->at_val.at_str) + 1; /* length of new string */

  if ((op == INCR) && (attr->at_val.at_str == NULL))
    op = SET; /* no current string, change INCR to SET */

  switch (op)
    {

    case SET: /* set is replace old string with new */

      if ((new_value = calloc(1, nsize)) == NULL)
        return (PBSE_SYSTEM);

      if (attr->at_val.at_str)
        (void)free(attr->at_val.at_str);
      attr->at_val.at_str = new_value;

      (void)strcpy(attr->at_val.at_str, new->at_val.at_str);

      break;

    case INCR: /* INCR is concatenate new to old string */

      nsize += strlen(attr->at_val.at_str);
      new_value = calloc(1, nsize + 1);

      if (new_value == NULL)
        return (PBSE_SYSTEM);

      strcat(new_value, attr->at_val.at_str);
      strcat(new_value, new->at_val.at_str);

      free(attr->at_val.at_str);
      attr->at_val.at_str = new_value;

      break;

    case DECR: /* DECR is remove substring if match, start at end */

      if (attr->at_val.at_str == NULL)
        break;

      if (--nsize == 0)
        break;

      p = attr->at_val.at_str + strlen(attr->at_val.at_str) - nsize;

      while (p >= attr->at_val.at_str)
        {
        if (strncmp(p, new->at_val.at_str, (int)nsize) == 0)
          {
          do
            {
            *p = *(p + nsize);
            }
          while (*p++);
          }

        p--;
        }

      break;

    default:
      return (PBSE_INTERNAL);
    }

  if ((attr->at_val.at_str != (char *)0) && (*attr->at_val.at_str != '\0'))
    attr->at_flags |= ATR_VFLAG_SET | ATR_VFLAG_MODIFY;
  else
    attr->at_flags &= ~ATR_VFLAG_SET;

  return (0);
  }


int lock_ai_mutex(job_array *pa, const char *func_id, char *msg, int logging)
  {
  return(0);
  }

int unlock_ai_mutex(job_array *pa, const char *func_id, char *msg, int logging)
  {
  return(0);
  }

int lock_alljobs_mutex(struct all_jobs *aj, const char *id, char *msg, int logging)
  {
  return(0);
  }

int unlock_alljobs_mutex(struct all_jobs *aj, const char *id, char *msg, int logging)
  {
  return(0);
  }
