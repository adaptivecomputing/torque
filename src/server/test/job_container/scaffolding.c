#include <stdlib.h>
#include <stdio.h>

#include "resizable_array.h"
#include "hash_table.h"
#include "pbs_job.h"

struct all_jobs        array_summary;
char                   server_name[PBS_MAXSERVERNAME + 1]; /* host_name[:service|port] */
struct all_jobs        alljobs;
int                    LOGLEVEL;

void log_err(int errno, const char *ident, char *msg) {}
void log_event(int type, int class, const char *ident, char *msg) {}


int insert_thing(resizable_array *ra, void *thing)
  {
  return(0);
  }

int get_svr_attr_l(int index, long *l)
  {
  return(0);
  }

int remove_thing_from_index(resizable_array *ra, int index)
  {
  return(0);
  }

int lock_ji_mutex(job *pjob, const char *id, char *msg, int logging)
  {
  return(0);
  }

int unlock_ji_mutex(job *pjob, const char *id, char *msg, int logging)
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

int get_value_hash(hash_table_t *ht, void *key)
  {
  return(0);
  }

resizable_array *initialize_resizable_array(int size)
  {
  return(NULL);
  }

void *next_thing(resizable_array *ra, int *iter)
  {
  return(NULL);
  }

hash_table_t *create_hash(int size)
  {
  return(NULL);
  }
  
int swap_things(resizable_array *ra, void *thing1, void *thing2)
  {
  return(0);
  }

int insert_thing_after(resizable_array *ra, void *thing, int index)
  {
  return(0);
  }

int get_svr_attr_str(int index, char **str)
  {
  return(0);
  }

int remove_hash(hash_table_t *ht, char *key)
  {
  return(0);
  }

void *next_thing_from_back(resizable_array *ra, int  *iter)
  {
  return(NULL);
  }

void change_value_hash(hash_table_t *ht, char *key, int new_value) {}
