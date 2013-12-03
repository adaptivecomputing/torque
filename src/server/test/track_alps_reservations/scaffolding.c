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

void log_err(int errnum, const char *routine, const char *text) {}

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
    pjob = (job *)calloc(1, sizeof(job));
    pjob->ji_qs.ji_state = JOB_STATE_RUNNING;
    pjob->ji_mutex = (pthread_mutex_t *)calloc(1, sizeof(pthread_mutex_t));
    return(pjob);
    }
  else
    {
    pjob = (job *)calloc(1, sizeof(job));
    pjob->ji_qs.ji_state = JOB_STATE_COMPLETE;
    pjob->ji_mutex = (pthread_mutex_t *)calloc(1, sizeof(pthread_mutex_t));
    return(pjob);
    }
  }


int check_and_resize(

  resizable_array *ra)

  {
  static const char *id = "check_and_resize";
  slot        *tmp;
  size_t       remaining;
  size_t       size;

  if (ra->max == ra->num + 1)
    {
    /* double the size if we're out of space */
    size = (ra->max * 2) * sizeof(slot);

    if ((tmp = (slot *)realloc(ra->slots,size)) == NULL)
      {
      log_err(ENOMEM,id, (char *)"No memory left to resize the array");
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
  resizable_array *ra = (resizable_array *)calloc(1, sizeof(resizable_array));
  size_t           amount = sizeof(slot) * size;

  ra->max       = size;
  ra->num       = 0;
  ra->next_slot = 1;
  ra->last      = 0;

  ra->slots = (slot *)calloc(1, amount);

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

char *threadsafe_tokenizer(

  char **str,    /* M */
  const char  *delims) /* I */

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

int unlock_ji_mutex(job *pjob, const char *id, const char *msg, int logging)
  {
  return(0);
  }
