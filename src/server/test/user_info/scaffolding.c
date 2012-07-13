#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "resizable_array.h"
#include "hash_table.h"
#include "user_info.h"

int insert_thing(resizable_array *ra, void *thing)
  {
  static int count = 0;
  count++;

  if (count == 2)
    return(-1);
  else
    return(0);
  }

int get_svr_attr_l(int index, long *l)
  {
  *l = 1;
  return(0);
  }

int add_hash(hash_table_t *ht, int index, void *thing)
  {
  return(0);
  }

int num_array_jobs(char *array_req)
  {
  return(11);
  }

int get_value_hash(hash_table_t *ht, void *thing)
  {
  char *str = (char *)thing;
  if (!strcmp(str, "tom"))
    return(1);
  else
    return(-1);
  }

resizable_array *initialize_resizable_array(int size)
  {
  static resizable_array ra;
  static slot            slots[2];
  static user_info       ui;

  ui.num_jobs_queued = 1;
  slots[1].item = &ui;
  ra.slots = slots;
  return(&ra);
  }

hash_table_t *create_hash(int size)
  {
  return((hash_table_t *)1);
  }

void log_err(int error, const char *func_id, char *msg) {}

