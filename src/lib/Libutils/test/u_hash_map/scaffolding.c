#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "resizable_array.h"
#include "hash_table.h"

int insert_thing(resizable_array *ra, void *thing)
  {
  if ((int *)thing == (int *)1)
    {
    return(-1);
    }
  else
    {
    return(0);
    }
  }

int remove_thing_from_index(resizable_array *ra, int index)
  {
  return(0);
  }

int add_hash(hash_table_t *ht, int value, void *key)
  {
  return(0);
  }

void free_hash(hash_table_t *ht) {}

int get_value_hash(hash_table_t *ht, void *key)
  {
  if (!strcmp((char *)key, "tom1"))
    return(0);
  else
    return(-1);
  }

resizable_array *initialize_resizable_array(int size)
  {
  static resizable_array ra;
  static slot            slots[2];
  static int             a = 4;

  slots[0].item = &a;
  ra.slots = slots;

  return(&ra);
  }

void *next_thing(resizable_array *ra, int *iter)
  {
  return(NULL);
  }

hash_table_t *create_hash(int size)
  {
  return(NULL);
  }

void free_resizable_array(resizable_array *ra) {}

void log_err(int errnum, const char *routine, char *text) {}

int remove_hash(hash_table_t *ht, char *key) 
  {
  return(0);
  }
