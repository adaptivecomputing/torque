#include <stdlib.h>
#include <stdio.h>

#include "resizable_array.h"
#include "hash_table.h"
#include "pbs_job.h"

struct all_jobs        array_summary;
char                   server_name[PBS_MAXSERVERNAME + 1]; /* host_name[:service|port] */
struct all_jobs        alljobs;
int                    LOGLEVEL;

void log_err(int errno, const char *ident, const char *msg) {}
void log_event(int type, int cls, const char *ident, const char *msg) {}


int get_svr_attr_l(int index, long *l)
  {
  return(0);
  }

int lock_ji_mutex(job *pjob, const char *id, const char *msg, int logging)
  {
  return(0);
  }

int unlock_ji_mutex(job *pjob, const char *id, const char *msg, int logging)
  {
  return(0);
  }

int add_hash(hash_table_t *ht, int value, void *key)
  {
  return(0);
  }
  
int is_svr_attr_set(int index)
  {
  return(0);
  }

int get_value_hash(hash_table_t *ht, const void *key)
  {
  return(0);
  }

hash_table_t *create_hash(int size)
  {
  return(NULL);
  }
  
int get_svr_attr_str(int index, char **str)
  {
  return(0);
  }

int remove_hash(hash_table_t *ht, const char *key)
  {
  return(0);
  }

void change_value_hash(hash_table_t *ht, char *key, int new_value) {}

job *job_alloc(void)
  {
  job *pj = (job *)calloc(1, sizeof(job));

  if (pj == NULL)
    {
    return(NULL);
    }

  pj->ji_mutex = (pthread_mutex_t *)calloc(1, sizeof(pthread_mutex_t));
  pthread_mutex_init(pj->ji_mutex,NULL);
  lock_ji_mutex(pj, __func__, NULL, LOGLEVEL);

  pj->ji_qs.qs_version = PBS_QS_VERSION;

  CLEAR_HEAD(pj->ji_rejectdest);
  pj->ji_is_array_template = FALSE;

  pj->ji_momhandle = -1;

  return(pj);
  }
