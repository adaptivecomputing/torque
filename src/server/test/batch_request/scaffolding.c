#include <stdlib.h>
#include <stdio.h>

#include "resizable_array.h"
#include "hash_table.h"
#include "batch_request.h"

batch_request_holder    brh;

int insert_thing(
    
  resizable_array *ra,
  void             *thing)

  {
  return(0);
  }

int remove_thing_from_index(

  resizable_array *ra,
  int              index)

  {
  return(0);
  }

int add_hash(
    
  hash_table_t *ht,
  int           value,
  void         *key)

  {
  return(0);
  }

int get_value_hash(
    
  hash_table_t *ht,
  void         *key)

  {
  return(0);
  }

resizable_array *initialize_resizable_array(

  int               size)

  {
  return(NULL);
  }

hash_table_t *create_hash(

  int size)

  {
  return(NULL);
  }

void log_err(int errnum, const char *routine, char *text) {}

int remove_hash(
    
  hash_table_t *ht,
  char         *key)

  {
  return(0);
  }
