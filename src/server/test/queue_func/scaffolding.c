#include "license_pbs.h" /* See here for the software license */
#include <stdlib.h>
#include <stdio.h> /* fprintf */

#include "queue.h" /* all_queues */
#include "server.h" /* server */
#include "resizable_array.h" /* resizable_array */
#include "attribute.h" /* pbs_attribute */
#include "hash_table.h" /* hash_table */
#include "pbs_job.h" /* job */

char *msg_err_unlink = "Unlink of %s file %s failed";
all_queues svr_queues;
attribute_def que_attr_def[10];
struct server server;
int LOGLEVEL = 0;
char *path_queues;

int insert_thing(resizable_array *ra, void *thing)
  {
  fprintf(stderr, "The call to insert_thing needs to be mocked!!\n");
  exit(1);
  }

int remove_thing_from_index(resizable_array *ra, int index)
  {
  fprintf(stderr, "The call to remove_thing_from_index needs to be mocked!!\n");
  exit(1);
  }

void clear_attr(pbs_attribute *pattr, attribute_def *pdef)
  {
  fprintf(stderr, "The call to clear_attr needs to be mocked!!\n");
  exit(1);
  }

int save_acl(pbs_attribute *attr, attribute_def *pdef, char *subdir, char *name)
  {
  fprintf(stderr, "The call to save_acl needs to be mocked!!\n");
  exit(1);
  }

int add_hash(hash_table_t *ht, int value, void *key)
  {
  fprintf(stderr, "The call to add_hash needs to be mocked!!\n");
  exit(1);
  }

int get_value_hash(hash_table_t *ht, void *key)
  {
  fprintf(stderr, "The call to get_value_hash needs to be mocked!!\n");
  exit(1);
  }

void initialize_all_jobs_array(struct all_jobs *aj)
  {
  fprintf(stderr, "The call to initialize_all_jobs_array needs to be mocked!!\n");
  exit(1);
  }

resizable_array *initialize_resizable_array(int size)
  {
  fprintf(stderr, "The call to initialize_resizable_array needs to be mocked!!\n");
  exit(1);
  }

void *next_thing(resizable_array *ra, int *iter)
  {
  fprintf(stderr, "The call to next_thing needs to be mocked!!\n");
  exit(1);
  }

hash_table_t *create_hash(int size)
  {
  fprintf(stderr, "The call to create_hash needs to be mocked!!\n");
  exit(1);
  }

int remove_hash(hash_table_t *ht, char *key)
  {
  fprintf(stderr, "The call to remove_hash needs to be mocked!!\n");
  exit(1);
  }

int insert_into_queue_recycler(pbs_queue *pq)
  {
  return(0);
  }

void free_hash(hash_table_t *ht) {}

void free_resizable_array(resizable_array *ra) {}

int get_svr_attr_str(int index, char **str)
  {
  return(0);
  }

job *svr_find_job(char *jobid)
  {
  fprintf(stderr, "The call to find_job needs to be mocked!!\n");
  exit(1);
  }

int unlock_sv_qs_mutex(pthread_mutex_t *sv_qs_mutex, const char *msg_string)
  {
  return(0);
  }

int lock_sv_qs_mutex(pthread_mutex_t *sv_qs_mutex, const char *msg_string)
  {
  return(0);
  }

int unlock_ji_mutex(job *pjob, const char *id, char *msg, int logging)
  {
  return(0);
  }
