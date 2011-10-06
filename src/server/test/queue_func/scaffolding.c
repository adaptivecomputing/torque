#include "license_pbs.h" /* See here for the software license */
#include <stdlib.h>
#include <stdio.h> /* fprintf */

#include "queue.h" /* all_queues */
#include "server.h" /* server */
#include "resizable_array.h" /* resizable_array */
#include "attribute.h" /* attribute */
#include "hash_table.h" /* hash_table */

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

void clear_attr(attribute *pattr, attribute_def *pdef)
  {
  fprintf(stderr, "The call to clear_attr needs to be mocked!!\n");
  exit(1);
  }

int save_acl(attribute *attr, attribute_def *pdef, char *subdir, char *name)
  {
  fprintf(stderr, "The call to save_acl needs to be mocked!!\n");
  exit(1);
  }

int add_hash(hash_table_t *ht, int value, char *key)
  {
  fprintf(stderr, "The call to add_hash needs to be mocked!!\n");
  exit(1);
  }

void log_record(int eventtype, int objclass, char *objname, char *text)
  {
  fprintf(stderr, "The call to log_record needs to be mocked!!\n");
  exit(1);
  }

int get_value_hash(hash_table_t *ht, char *key)
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

void log_err(int errnum, char *routine, char *text)
  {
  fprintf(stderr, "The call to log_err needs to be mocked!!\n");
  exit(1);
  }

int remove_hash(hash_table_t *ht, char *key)
  {
  fprintf(stderr, "The call to remove_hash needs to be mocked!!\n");
  exit(1);
  }



