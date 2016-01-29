#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "pbs_job.h"
#include "utils.h"
#include "pbs_nodes.h"
#include "batch_request.h"
#include "id_map.hpp"

bool exit_called = false;
int LOGLEVEL;

int unlock_node(struct pbsnode *the_node, const char *method_name, char *msg, int logging)
  {
  return(0);
  }

void log_err(int errnum, const char *routine, const char *text) {}

job *svr_find_job(

  const char *job_id, int get_subjob)

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

void log_event(int eventtype, int objclass, const char *objname, const char *text)
  {
  }

job *svr_find_job_by_id(int id)
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

id_map::id_map() {}

id_map::~id_map() {}

const char *id_map::get_name(int id)
  {
  return(strdup("1.napali"));
  }

id_map job_mapper;
