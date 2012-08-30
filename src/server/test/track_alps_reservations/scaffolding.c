#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "pbs_job.h"
#include "utils.h"
#include "pbs_nodes.h"
#include "batch_request.h"

int LOGLEVEL;

int unlock_node(struct pbsnode *the_node, const char *method_name, char *msg, int logging)
  {
  return(0);
  }

void log_err(int errnum, const char *routine, char *text) {}

void free_resizable_array(

  resizable_array *ra)

  {
  free(ra->slots);
  free(ra);
  } /* END resizable_array() */

job *svr_find_job(

  char *job_id, int get_subjob)

  {
  static int  count = 0;
  job        *pjob;

  if (count++ == 0)
    return(NULL);
  else if (count <= 3)
    {
    pjob = calloc(1, sizeof(job));
    pjob->ji_qs.ji_state = JOB_STATE_RUNNING;
    pjob->ji_mutex = calloc(1, sizeof(pthread_mutex_t));
    return(pjob);
    }
  else
    {
    pjob = calloc(1, sizeof(job));
    pjob->ji_qs.ji_state = JOB_STATE_COMPLETE;
    pjob->ji_mutex = calloc(1, sizeof(pthread_mutex_t));
    return(pjob);
    }
  }


int check_and_resize(

  resizable_array *ra)

  {
  static char *id = "check_and_resize";
  slot        *tmp;
  size_t       remaining;
  size_t       size;

  if (ra->max == ra->num + 1)
    {
    /* double the size if we're out of space */
    size = (ra->max * 2) * sizeof(slot);

    if ((tmp = realloc(ra->slots,size)) == NULL)
      {
      log_err(ENOMEM,id,"No memory left to resize the array");
      return(ENOMEM);
      }

    remaining = ra->max * sizeof(slot);

    memset(tmp + ra->max, 0, remaining);

    ra->slots = tmp;

    ra->max = ra->max * 2;
    }

  return(PBSE_NONE);
  } /* END check_and_resize() */

void update_next_slot(

  resizable_array *ra) /* M */

  {
  while ((ra->next_slot < ra->max) &&
         (ra->slots[ra->next_slot].item != NULL))
    ra->next_slot++;
  } /* END update_next_slot() */

int insert_thing(
    
  resizable_array *ra,
  void             *thing)

  {
  int rc;
  
  /* check if the array must be resized */
  if ((rc = check_and_resize(ra)) != PBSE_NONE)
    {
    return(-1);
    }
    
  ra->slots[ra->next_slot].item = thing;
 
  /* save the insertion point */
  rc = ra->next_slot;

  /* handle the backwards pointer, next pointer is left at zero */
  ra->slots[rc].prev = ra->last;

  /* make sure the empty slot points to the next occupied slot */
  if (ra->last == ALWAYS_EMPTY_INDEX)
    {
    ra->slots[ALWAYS_EMPTY_INDEX].next = rc;
    }

  /* update the last index */
  ra->slots[ra->last].next = rc;
  ra->last = rc;

  /* update the new item's next index */
  ra->slots[rc].next = ALWAYS_EMPTY_INDEX;

  /* increase the count */
  ra->num++;

  update_next_slot(ra);

  return(rc);
  } /* END insert_thing() */

resizable_array *initialize_resizable_array(

  int               size)

  {
  resizable_array *ra = calloc(1, sizeof(resizable_array));
  size_t           amount = sizeof(slot) * size;

  ra->max       = size;
  ra->num       = 0;
  ra->next_slot = 1;
  ra->last      = 0;

  ra->slots = calloc(1, amount);

  return(ra);
  } /* END initialize_resizable_array() */

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

void add_to_bucket(

  bucket **buckets,
  int      index,
  char    *key,
  int      value)

  {
  if (buckets[index] == NULL)
    {
    /* empty bucket, just add it here */
    buckets[index] = calloc(1, sizeof(bucket));
    buckets[index]->value = value;
    buckets[index]->key   = key;
    buckets[index]->next  = NULL;
    }
  else
    {
    /* make this the last bucket at the end */
    bucket *b = buckets[index];

    while (b->next != NULL)
      b = b->next;

    b->next = calloc(1, sizeof(bucket));
    b->next->value = value;
    b->next->key   = key;
    b->next->next  = NULL;
    }
  } /* END add_to_bucket() */


int remove_thing_from_index(

  resizable_array *ra,
  int              index)

  {
  int rc = PBSE_NONE;

  if (ra->slots[index].item == NULL)
    rc = THING_NOT_FOUND;
  else
    {
    /* FOUND */
    unlink_slot(ra,index);

    ra->num--;

    if (index < ra->next_slot)
      ra->next_slot = index;
    }

  return(rc);
  } /* END remove_thing_from_index() */

void free_buckets(

  bucket **buckets,
  int      size)

  {
  int     i;
  bucket *b;
  bucket *tmp;

  for (i = 0; i < size; i++)
    {
    b = buckets[i];

    while (b != NULL)
      {
      tmp = b;
      b = b->next;

      free(tmp);
      }
    }
  } /* END free_buckets() */

int get_hash(
    
  hash_table_t *ht,
  void         *key)

  {
  return(atoi((char *)key) % ht->size);
  } /* END get_hash() */

int add_hash(
    
  hash_table_t *ht,
  int           value,
  void         *key)

  {
  int index;

  /* check if we need to rehash */
  if (ht->size == ht->num)
    {
    int      old_bucket_size = ht->size;
    int      new_bucket_size = ht->size * 2;
    size_t   amount = new_bucket_size * sizeof(bucket *);
    bucket **tmp = calloc(1, amount);
    int      i;
    int      new_index;
      
    ht->size = new_bucket_size;

    /* rehash all of the hold values */
    for (i = 0; i < old_bucket_size; i++)
      {
      bucket *b = ht->buckets[i];

      while (b != NULL)
        {
        new_index = get_hash(ht,b->key);

        add_to_bucket(tmp,new_index,b->key,b->value);

        b = b->next;
        }
      } /* END foreach (old buckets) */

    free_buckets(ht->buckets, old_bucket_size);

    free(ht->buckets);

    ht->buckets = tmp;
    } /* END if (need to rehash) */

  index = get_hash(ht,key);

  add_to_bucket(ht->buckets,index,key,value);

  ht->num++;

  return(PBSE_NONE);
  } /* END add_hash() */

int get_value_hash(
    
  hash_table_t *ht,
  void         *key)

  {
  int     value = -1;
  bucket *b = ht->buckets[get_hash(ht,key)];

  while (b != NULL)
    {
    if (!strcmp(b->key,key))
      {
      value = b->value;
      break;
      }

    b = b->next;
    }

  return(value);
  } /* END get_value_from_hash() */



hash_table_t *create_hash(

  int size)

  {
  hash_table_t *ht = calloc(1, sizeof(hash_table_t));
  size_t        amount = sizeof(bucket *) * size;

  ht->size = size;
  ht->num  = 0;
  ht->buckets = calloc(1, amount);

  return(ht);
  } /* END create_hash() */

char *threadsafe_tokenizer(

  char **str,    /* M */
  char  *delims) /* I */

  {
  char *current_char;
  char *start;

  if ((str == NULL) ||
      (*str == NULL))
    return(NULL);

  /* save start position */
  start = *str;

  /* return NULL at the end of the string */
  if (*start == '\0')
    return(NULL);

  /* begin at the start */
  current_char = start;

  /* advance to the end of the string or until you find a delimiter */
  while ((*current_char != '\0') &&
         (!strchr(delims, *current_char)))
    current_char++;

  /* advance str */
  if (*current_char != '\0')
    {
    /* not at the end of the string */
    *str = current_char + 1;
    *current_char = '\0';
    }
  else
    {
    /* at the end of the string */
    *str = current_char;
    }

  return(start);
  } /* END threadsafe_tokenizer() */

int remove_hash(
    
  hash_table_t *ht,
  char         *key)

  {
  int     rc = PBSE_NONE;
  int     index = get_hash(ht,key);
  bucket *b = ht->buckets[index];
  bucket *prev = NULL;

  while (b != NULL)
    {
    if (!strcmp(key,b->key))
      break;

    prev = b;
    b = b->next;
    }

  if (b == NULL)
    rc = KEY_NOT_FOUND;
  else
    {
    if (prev == NULL)
      {
      if (b->next == NULL)
        {
        free(b);
        ht->buckets[index] = NULL;
        }
      else
        {
        /* copy the next bucket here and release it */
        ht->buckets[index] = b->next;

        free(b);
        }
      }
    else
      {
      prev->next = b->next;
      free(b);
      }

    ht->num--;
    } /* END found */

  return(rc);
  } /* END remove_hash() */


int unlock_ji_mutex(job *pjob, const char *id, char *msg, int logging)
  {
  return(0);
  }
