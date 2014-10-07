#include "license_pbs.h" /* See here for the software license */
#include <stdlib.h>
#include <stdio.h> /* fprintf */
#include <errno.h>

#include "pbs_job.h" /* job_recycler, all_jobs, job */
#include "resizable_array.h"

job_recycler recycler;
int          LOGLEVEL;


#define rot(x,k) (((x)<<(k)) | ((x)>>(32-(k))))
#define mix(a,b,c) \
  { \
    a -= c;  a ^= rot(c, 4);  c += b; \
    b -= a;  b ^= rot(a, 6);  a += c; \
    c -= b;  c ^= rot(b, 8);  b += a; \
    a -= c;  a ^= rot(c,16);  c += b; \
    b -= a;  b ^= rot(a,19);  a += c; \
    c -= b;  c ^= rot(b, 4);  b += a; \
  }
#define final(a,b,c) \
  { \
    c ^= b; c -= rot(b,14); \
    a ^= c; a -= rot(c,11); \
    b ^= a; b -= rot(a,25); \
    c ^= b; c -= rot(b,16); \
    a ^= c; a -= rot(c,4);  \
    b ^= a; b -= rot(a,14); \
    c ^= b; c -= rot(b,24); \
  }


int enqueue_threadpool_request(void *(*func)(void *),void *arg)
  {
  fprintf(stderr, "The call to enqueue_threadpool_request to be mocked!!\n");
  exit(1);
  }



resizable_array *initialize_resizable_array(

  int               size)

  {
  resizable_array *ra = (resizable_array*)calloc(1, sizeof(resizable_array));
  size_t           amount = sizeof(slot) * size;

  ra->max       = size;
  ra->num       = 0;
  ra->next_slot = 1;
  ra->last      = 0;

  ra->slots = (slot *)calloc(1, amount);

  return(ra);
  } /* END initialize_resizable_array() */



hash_table_t *create_hash(

  int size)

  {
  hash_table_t *ht = (hash_table_t *)calloc(1, sizeof(hash_table_t));
  size_t        amount = sizeof(bucket *) * size;

  ht->size = size;
  ht->num  = 0;
  ht->buckets = (bucket **)calloc(1, amount);

  return(ht);
  } /* END create_hash() */


uint32_t hashlittle( const void *key, size_t length, uint32_t initval)
  {
  uint32_t a,b,c;                                          /* internal state */
  union { const void *ptr; size_t i; } u;     /* needed for Mac Powerbook G4 */
  
  /* Set up the internal state */
  a = b = c = 0xdeadbeef + ((uint32_t)length) + initval;
  
  u.ptr = key;
  if (HASH_LITTLE_ENDIAN && ((u.i & 0x3) == 0)) 
    {
    const uint32_t *k = (const uint32_t *)key;         /* read 32-bit chunks */
    const uint8_t  *k8;
    
    /*------ all but last block: aligned reads and affect 32 bits of (a,b,c) */
    while (length > 12)
      {
      a += k[0];
      b += k[1];
      c += k[2];
      mix(a,b,c);
      length -= 12;
      k += 3;
      }
    
    /*----------------------------- handle the last (probably partial) block */
    /* 
     * "k[2]&0xffffff" actually reads beyond the end of the string, but
     * then masks off the part it's not allowed to read.  Because the
     * string is aligned, the masked-off tail is in the same word as the
     * rest of the string.  Every machine with memory protection I've seen
     * does it on word boundaries, so is OK with this.  But VALGRIND will
     * still catch it and complain.  The masking trick does make the hash
     * noticably faster for short strings (like English words).
     */

    /* dbeer: slightly modified - I'm using only the valgrind friendly code */
    
    k8 = (const uint8_t *)k;
    switch(length)
      {
      case 12: c+=k[2]; b+=k[1]; a+=k[0]; break;
      case 11: c+=((uint32_t)k8[10])<<16;  /* fall through */
      case 10: c+=((uint32_t)k8[9])<<8;    /* fall through */
      case 9 : c+=k8[8];                   /* fall through */
      case 8 : b+=k[1]; a+=k[0]; break;
      case 7 : b+=((uint32_t)k8[6])<<16;   /* fall through */
      case 6 : b+=((uint32_t)k8[5])<<8;    /* fall through */
      case 5 : b+=k8[4];                   /* fall through */
      case 4 : a+=k[0]; break;
      case 3 : a+=((uint32_t)k8[2])<<16;   /* fall through */
      case 2 : a+=((uint32_t)k8[1])<<8;    /* fall through */
      case 1 : a+=k8[0]; break;
      case 0 : return c;
      }
    
    } 
  else if (HASH_LITTLE_ENDIAN && ((u.i & 0x1) == 0)) 
    {
    const uint16_t *k = (const uint16_t *)key;         /* read 16-bit chunks */
    const uint8_t  *k8;
    
    /*--------------- all but last block: aligned reads and different mixing */
    while (length > 12)
      {
      a += k[0] + (((uint32_t)k[1])<<16);
      b += k[2] + (((uint32_t)k[3])<<16);
      c += k[4] + (((uint32_t)k[5])<<16);
      mix(a,b,c);
      length -= 12;
      k += 6;
      }
    
    /*----------------------------- handle the last (probably partial) block */
    k8 = (const uint8_t *)k;
    switch(length)
      {
      case 12: 
        c+=k[4]+(((uint32_t)k[5])<<16);
        b+=k[2]+(((uint32_t)k[3])<<16);
        a+=k[0]+(((uint32_t)k[1])<<16);
        break;
      case 11: 
        c+=((uint32_t)k8[10])<<16;     /* fall through */  
      case 10: 
        c+=k[4]; 
        b+=k[2]+(((uint32_t)k[3])<<16);   
        a+=k[0]+(((uint32_t)k[1])<<16);
        break;
      case 9: 
        c+=k8[8];                      /* fall through */
      case 8: 
        b+=k[2]+(((uint32_t)k[3])<<16);
        a+=k[0]+(((uint32_t)k[1])<<16);
        break;
      case 7: 
        b+=((uint32_t)k8[6])<<16;      /* fall through */
      case 6: 
        b+=k[2];
        a+=k[0]+(((uint32_t)k[1])<<16);
        break;
      case 5:
        b+=k8[4];                      /* fall through */
      case 4:
        a+=k[0]+(((uint32_t)k[1])<<16);
        break;
      case 3:
        a+=((uint32_t)k8[2])<<16;      /* fall through */
      case 2:
        a+=k[0];
        break;
      case 1:
        a+=k8[0];
        break;
      case 0:
        return c;                     /* zero length requires no mixing */
      }
    } 
  else 
    {                        /* need to read the key one byte at a time */
    const uint8_t *k = (const uint8_t *)key;
    /*--------------- all but the last block: affect some 32 bits of (a,b,c) */
    while (length > 12)
      {
      a += k[0];
      a += ((uint32_t)k[1])<<8;
      a += ((uint32_t)k[2])<<16;
      a += ((uint32_t)k[3])<<24;
      b += k[4];
      b += ((uint32_t)k[5])<<8;
      b += ((uint32_t)k[6])<<16;
      b += ((uint32_t)k[7])<<24;
      c += k[8];
      c += ((uint32_t)k[9])<<8;
      c += ((uint32_t)k[10])<<16;
      c += ((uint32_t)k[11])<<24;
      mix(a,b,c);
      length -= 12;
      k += 12;
      }

    /*-------------------------------- last block: affect all 32 bits of (c) */
    switch(length)                   /* all the case statements fall through */
      {
      case 12:
        c+=((uint32_t)k[11])<<24;
      case 11: 
        c+=((uint32_t)k[10])<<16;
      case 10:
        c+=((uint32_t)k[9])<<8;
      case 9:
        c+=k[8];
      case 8:
        b+=((uint32_t)k[7])<<24;
      case 7:
        b+=((uint32_t)k[6])<<16;
      case 6:
        b+=((uint32_t)k[5])<<8;
      case 5:
        b+=k[4];
      case 4:
        a+=((uint32_t)k[3])<<24;
      case 3:
        a+=((uint32_t)k[2])<<16;
      case 2:
        a+=((uint32_t)k[1])<<8;
      case 1:
        a+=k[0];
        break;
      case 0:
        return c;
      }
    }
  
  final(a,b,c);
  return c;
  } /* END hashlittle() */


int get_hash(
    
  hash_table_t *ht,
  const void   *key)

  {
  size_t   len;
  uint32_t hash;
  uint32_t mask = 0;
  uint32_t counter;

  if (ht == NULL)
    return(-1);
  if (key == NULL)
    return(-1);

  const char *key_char = (const char *)key;

  len = strlen(key_char);
  counter = ht->size >> 1;

#ifdef HASH_LITTLE_ENDIAN
  hash = hashlittle(key,len,0);
#else
  hash = hashbig(key,len,0);
#endif

  for (mask = 0; counter != 0; counter = counter >> 1)
    mask |= counter;
  
  return(hash & mask);
  } /* END get_hash() */



int get_value_hash(
    
  hash_table_t *ht,
  const void   *key)

  {
  int     value = -1;
  int     hash_index;
  bucket *b;

  if (ht == NULL)
    return(-1);
  if (key == NULL)
    return(-1);

  hash_index = get_hash(ht, key);
  if (hash_index < 0)
    return(-1);

  b = ht->buckets[hash_index];
  while (b != NULL)
    {
    if (!strcmp(b->key, (char *)key))
      {
      value = b->value;
      break;
      }

    b = b->next;
    }

  return(value);
  } /* END get_value_from_hash() */


void add_to_bucket(

  bucket **buckets,
  int      index,
  void    *key,
  int      value)

  {
  if (buckets[index] == NULL)
    {
    /* empty bucket, just add it here */
    buckets[index] = (bucket *)calloc(1, sizeof(bucket));
    buckets[index]->value = value;
    buckets[index]->key   = (char *)key;
    buckets[index]->next  = NULL;
    }
  else
    {
    /* make this the last bucket at the end */
    bucket *b = buckets[index];

    while (b->next != NULL)
      b = b->next;

    b->next = (bucket *)calloc(1, sizeof(bucket));
    b->next->value = value;
    b->next->key   = (char *)key;
    b->next->next  = NULL;
    }
  } /* END add_to_bucket() */


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


int add_hash(
    
  hash_table_t *ht,
  int           value,
  void         *key)

  {
  int index;

  if (key == NULL)
    return(-1);

  /* check if we need to rehash */
  if (ht->size == ht->num)
    {
    int      old_bucket_size = ht->size;
    int      new_bucket_size = ht->size * 2;
    size_t   amount = new_bucket_size * sizeof(bucket *);
    bucket **tmp = (bucket **)calloc(1, amount);
    int      i;
    int      new_index;
      
    ht->size = new_bucket_size;

    /* rehash all of the hold values */
    for (i = 0; i < old_bucket_size; i++)
      {
      bucket *b = ht->buckets[i];

      while (b != NULL)
        {
        new_index = get_hash(ht, b->key);

        add_to_bucket(tmp,new_index,b->key,b->value);

        b = b->next;
        }
      } /* END foreach (old buckets) */

    free_buckets(ht->buckets, old_bucket_size);

    free(ht->buckets);

    ht->buckets = tmp;
    } /* END if (need to rehash) */

  index = get_hash(ht, key);

  add_to_bucket(ht->buckets,index,key,value);

  ht->num++;

  return(PBSE_NONE);
  } /* END add_hash() */


void update_next_slot(

  resizable_array *ra) /* M */

  {
  while ((ra->next_slot < ra->max) &&
         (ra->slots[ra->next_slot].item != NULL))
    ra->next_slot++;
  } /* END update_next_slot() */


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
      return(ENOMEM);
      }

    remaining = ra->max * sizeof(slot);

    memset(tmp + ra->max, 0, remaining);

    ra->slots = tmp;

    ra->max = ra->max * 2;
    }

  return(PBSE_NONE);
  } /* END check_and_resize() */



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



int insert_job(struct all_jobs *aj, job *pjob)
  {
  pthread_mutex_lock(aj->alljobs_mutex);

  int rc = insert_thing(aj->ra,pjob);
  if (rc == -1)
    {
    rc = -1;
    }
  else
    {
    add_hash(aj->ht, rc, pjob->ji_qs.ji_jobid);
    rc = PBSE_NONE;
    }

  pthread_mutex_unlock(aj->alljobs_mutex);

  return(rc);
  } 



job *next_job(struct all_jobs *aj, int *iter)
  {
  fprintf(stderr, "The call to next_job to be mocked!!\n");
  exit(1);
  }


void initialize_all_jobs_array(struct all_jobs *aj)
  {
  aj->ra = initialize_resizable_array(INITIAL_JOB_SIZE);
  aj->ht = create_hash(INITIAL_HASH_SIZE);

  aj->alljobs_mutex = (pthread_mutex_t*)calloc(1, sizeof(pthread_mutex_t));
  pthread_mutex_init(aj->alljobs_mutex, NULL);
  }

int remove_job(struct all_jobs *aj, job *pjob)
  {
  fprintf(stderr, "The call to remove_job to be mocked!!\n");
  exit(1);
  }

int unlock_ji_mutex(job *pjob, const char *id, const char *msg, int logging)
  {
  return(0);
  }

int lock_ji_mutex(job *pjob, const char *id, const char *msg, int logging)
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


void log_event(int eventtype, int objclass, const char *objname, const char *text) {}
void log_err(int objclass, const char *objname, const char *text) {}

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

void *next_thing(

  resizable_array *ra,
  int             *iter)

  {
  void *thing;
  int   i = *iter;

  if (i == -1)
    {
    /* initialize first */
    i = ra->slots[ALWAYS_EMPTY_INDEX].next;
    }

  thing = ra->slots[i].item;
  *iter = ra->slots[i].next;

  return(thing);
  } /* END next_thing() */
