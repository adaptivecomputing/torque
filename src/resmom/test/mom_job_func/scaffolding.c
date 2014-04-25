#include "license_pbs.h" /* See here for the software license */
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <pwd.h> /* gid_t, uid_t */

#include "attribute.h" /* attribute_def, pbs_attribute */
#include "list_link.h" /* tlist_head, list_link */
#include "log.h" /* LOG_BUF_SIZE */
#include "pbs_ifl.h" /* MAXPATHLEN */
#include "net_connect.h" /* pbs_net_t */
#include "server_limits.h" /* pbs_net_t. Also defined in net_connect.h */
#include "pbs_job.h" /* job_file_delete_info */
#include "mom_job_cleanup.h"

int is_login_node = 0;
char *apbasil_path = NULL;
char *apbasil_protocol = NULL;
char *path_jobs; /* mom_main.c */
int multi_mom = 1; /* mom_main.c */
attribute_def job_attr_def[10]; /* job_attr_def.c */
char *path_aux; /* mom_main.c */
gid_t pbsgroup; /* mom_main.c */
int thread_unlink_calls; /* mom_main.c */
uid_t pbsuser; /* mom_main.c */
char tmpdir_basename[MAXPATHLEN]; /* mom_main.c */
char *msg_err_purgejob; /* pbs_messages.c */
int pbs_rm_port; /* mom_main.c */
tlist_head svr_alljobs; /* mom_main.c */
int LOGLEVEL = 7; /* force logging code to be exercised as tests run */ /* mom_main.c/pbsd_main.c */
char log_buffer[LOG_BUF_SIZE]; /* pbs_log.c */
int popped = 0;
int removed = 0;


void clear_attr(pbs_attribute *pattr, attribute_def *pdef)
  {
  fprintf(stderr, "The call to clear_attr needs to be mocked!!\n");
  exit(1);
  }

pbs_net_t get_hostaddr(int *local_errno, char *hostname)
  {
  fprintf(stderr, "The call to get_hostaddr needs to be mocked!!\n");
  exit(1);
  }

void delete_link(struct list_link *old)
  {
  fprintf(stderr, "The call to delete_link needs to be mocked!!\n");
  exit(1);
  }

void log_record(int eventtype, int objclass, const char *objname, const char *text)
  {
  fprintf(stderr, "The call to log_record needs to be mocked!!\n");
  exit(1);
  }

int enqueue_threadpool_request(void *(*func)(void *),void *arg)
  {
  fprintf(stderr, "The call to enqueue_threadpool_request needs to be mocked!!\n");
  exit(1);
  }

void log_ext(int errnum, const char *routine, const char *text, int severity)
  {
  fprintf(stderr, "The call to log_ext needs to be mocked!!\n");
  exit(1);
  }

void MOMCheckRestart(void)
  {
  fprintf(stderr, "The call to MOMCheckRestart needs to be mocked!!\n");
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

void nodes_free(job *pj)
  {
  fprintf(stderr, "The call to nodes_free needs to be mocked!!\n");
  exit(1);
  }

void mom_checkpoint_delete_files(job_file_delete_info *jfdi)
  {
  fprintf(stderr, "The call to mom_checkpoint_delete_files needs to be mocked!!\n");
  exit(1);
  }

void log_err(int errnum, const char *routine, const char *text)
  {
  fprintf(stderr, "The call to log_err needs to be mocked!!\n");
  exit(1);
  }

void close_conn(int sd, int has_mutex)
  {
  fprintf(stderr, "The call to close_conn needs to be mocked!!\n");
  exit(1);
  }

void DIS_tcp_cleanup(struct tcp_chan *chan) {}

int destroy_alps_reservation(

  char *reservation_id,
  char *apbasil_path,
  char *apbasil_protocol,
  int   retries)

  {
  return(0);
  }


/*
 * checks if the array needs to be resized, and resizes if necessary
 *
 * @return PBSE_NONE or ENOMEM
 */
int check_and_resize(

  resizable_array *ra)

  {
  slot        *tmp;
  size_t       remaining;
  size_t       size;

  if (ra->max == ra->num + 1)
    {
    /* double the size if we're out of space */
    size = (ra->max * 2) * sizeof(slot);

    if ((tmp = (slot *)realloc(ra->slots,size)) == NULL)
      {
      log_err(ENOMEM,__func__,"No memory left to resize the array");
      return(ENOMEM);
      }

    remaining = ra->max * sizeof(slot);

    memset(tmp + ra->max, 0, remaining);

    ra->slots = tmp;

    ra->max = ra->max * 2;
    }

  return(PBSE_NONE);
  } /* END check_and_resize() */

/* 
 * updates the next slot pointer if needed \
 */
void update_next_slot(

  resizable_array *ra) /* M */

  {
  while ((ra->next_slot < ra->max) &&
          (ra->slots[ra->next_slot].item != NULL))
    ra->next_slot++;
  } /* END update_next_slot() */



/*
 * fix the next pointer for the box pointing to this index 
 *
 * @param ra - the array we're fixing
 * @param index - index of the slot we're unlinking
 */
void unlink_slot(
  resizable_array *ra,
  int              index)

  {
  int prev = ra->slots[index].prev;
  int next = ra->slots[index].next;

  ra->slots[index].prev = ALWAYS_EMPTY_INDEX;
  ra->slots[index].next = ALWAYS_EMPTY_INDEX;
  ra->slots[index].item = NULL;

  ra->slots[prev].next = next;

  /* update last if necessary, otherwise update prev's next index */
  if (ra->last == index)
    ra->last = prev;
  else
    ra->slots[next].prev = prev;
  } /* END unlink_slot() */


/*
 * pop the first thing from the array
 *
 * @return the first thing in the array or NULL if empty
 */
extern resizable_array *exiting_job_list;

void *pop_thing(
    
  resizable_array *ra)

  {
  popped++;

  if (ra == exiting_job_list)
    return(NULL);

  void *thing = NULL;
  int   i = ra->slots[ALWAYS_EMPTY_INDEX].next;

  if (i != ALWAYS_EMPTY_INDEX)
    {
    /* get the thing we're returning */
    thing = ra->slots[i].item;

    /* handle the deletion and removal */
    unlink_slot(ra,i);

    ra->num--;

    /* reset the next slot index if necessary */
    if (i < ra->next_slot)
      {
      ra->next_slot = i;
      }
    }

  return(thing);
  } /* END pop_thing() */


int remove_thing_from_index(resizable_array *ra, int index)

  {
  removed++;
  return(0);
  }


/* 
 * inserts a thing after the thing in index
 * NOTE: index must represent a valid index
 */
int insert_thing_after(
  resizable_array *ra,
  void            *thing,
  int              index)

  {
  int rc;
  int next;

  /* check if the array must be resized */
  if ((rc = check_and_resize(ra)) != PBSE_NONE)
    {
    return(-1);
    }

  /* insert this element */
  ra->slots[ra->next_slot].item = thing;

  /* save the insertion point */
  rc = ra->next_slot;

  /* move pointers around */
  ra->slots[rc].prev = index;
  next = ra->slots[index].next;
  ra->slots[rc].next = next;
  ra->slots[index].next = rc;

  if (next != 0)
    {
    ra->slots[next].prev = rc;
    }

  /* update the last index if needed */
  if (ra->last == index)
    ra->last = rc;

  /* increase the count */
  ra->num++;

  update_next_slot(ra);

  return(rc);
  } /* END insert_thing_after() */


void free_resizable_array(resizable_array *ra) {}

resizable_array *initialize_resizable_array(

  int               size)

  {
  return(NULL);
  }

int insert_thing(
    
  resizable_array *ra,
  void             *thing)

  {
  return(0);
  }

void *next_thing(resizable_array *ra, int *iter)
  {
  int old_val = *iter;
  *iter = *iter + 1;

  if (old_val < 1)
    {
    exiting_job_info *eji = (exiting_job_info *)calloc(1, sizeof(exiting_job_info));
    snprintf(eji->jobid, sizeof(eji->jobid), "%d.napali", 1 + old_val);
    return(eji);
    }

  return(NULL);
  }

int is_present(

  resizable_array *ra,
  void            *thing)

  {
  return(0);
  }

int setuid_ext(uid_t uid, int set_euid)
  {
  return(0);
  }

int delete_cpuset(const char *, bool)
  {
  return 0;
  }
