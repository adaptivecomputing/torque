
#include <pbs_config.h>

#include <pthread.h>
#include <string.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "array.h"
#include "list_link.h"
#include "svrfunc.h"
#include "server.h"
#include "utils.h"
#include "mutex_mgr.hpp"

int create_and_queue_array_subjob(job_array *pa, mutex_mgr &array_mgr, job *template_job,
                                  mutex_mgr &template_job_mgr, int index, std::string &prev_job_id,
                                  bool place_hold);


const int DEFAULT_IDLE_SLOT_LIMIT = 300;



// array_info empty constructor
array_info::array_info() : struct_version(ARRAY_QS_STRUCT_VERSION), array_size(0), num_jobs(0),
                           slot_limit(NO_SLOT_LIMIT), jobs_running(0), jobs_done(0), num_cloned(0),
                           num_started(0), num_failed(0), num_successful(0), num_purged(0),
                           num_idle(0), deps(),
                           idle_slot_limit(DEFAULT_IDLE_SLOT_LIMIT), highest_id_created(-1),
                           range_str()

  {
  memset(this->owner, 0, sizeof(this->owner));
  memset(this->parent_id, 0, sizeof(this->parent_id));
  memset(this->fileprefix, 0, sizeof(this->fileprefix));
  memset(this->submit_host, 0, sizeof(this->submit_host));
  }



array_info::~array_info() 

  {
  // free the dependencies if any exist
  for (std::list<array_depend *>::iterator it = this->deps.begin(); it != this->deps.end(); it++)
    {
    array_depend *pdep = *it;
    delete pdep;
    }
  } // END destructor()



// job_array empty constructor
job_array::job_array() : job_ids(NULL), jobs_recovered(0), ai_ghost_recovered(false), uncreated_ids(),
                         ai_mutex(NULL), ai_qs(), being_deleted(false)

  {
  this->ai_mutex = (pthread_mutex_t *)calloc(1, sizeof(pthread_mutex_t));
  pthread_mutex_init(this->ai_mutex, NULL);
  }



// Destructor
job_array::~job_array()

  {
  pthread_mutex_unlock(this->ai_mutex);
  free(this->ai_mutex);

  /* free the memory for the job pointers */
  if (this->job_ids != NULL)
    {
    for (int i = 0; i < this->ai_qs.array_size; i++)
      {
      if (this->job_ids[i] != NULL)
        free(this->job_ids[i]);
      }

    free(this->job_ids);
    }
  }

void job_array::set_array_id(

  const char *array_id)

  {
  snprintf(this->ai_qs.parent_id, sizeof(this->ai_qs.parent_id), "%s", array_id);
  }

void job_array::set_arrays_fileprefix(

  const char *file_prefix)

  {
  snprintf(this->ai_qs.fileprefix, sizeof(this->ai_qs.fileprefix), "%s", file_prefix);
  }

void job_array::set_owner(

  const char *owner)

  {
  snprintf(this->ai_qs.owner, sizeof(this->ai_qs.owner), "%s", owner);
  }

void job_array::set_submit_host(

  const char *submit_host)

  {
  snprintf(this->ai_qs.submit_host, sizeof(this->ai_qs.submit_host), "%s", submit_host);
  }



/* 
 * set_slot_limit()
 * sets how many jobs can be run from this array at once
 *
 * @param request (M) - the string array request
 * @param pa - the array to receive a slot limit
 *
 * @return 0 on SUCCESS
 */

int job_array::set_slot_limit(

  char *request)

  {
  char *pcnt;
  long  max_limit;
  char  log_buf[LOCAL_LOG_BUF_SIZE];

  /* check for a max slot limit */
  if (get_svr_attr_l(SRV_ATR_MaxSlotLimit, &max_limit) != PBSE_NONE)
    max_limit = NO_SLOT_LIMIT;

  if ((pcnt = strchr(request,'%')) != NULL)
    {
    /* remove '%' from the request, or else it can't be parsed */
    while (*pcnt == '%')
      {
      *pcnt = '\0';
      pcnt++;
      }

    /* read the number if one is given */
    if (strlen(pcnt) > 0)
      {
      this->ai_qs.slot_limit = strtol(pcnt, NULL, 10);

      if ((max_limit != NO_SLOT_LIMIT) &&
          (max_limit < this->ai_qs.slot_limit))
        {
        snprintf(log_buf,sizeof(log_buf),
          "Array %s requested a slot limit above the max limit %ld, rejecting\n",
          this->ai_qs.parent_id,
          max_limit);
        log_event(PBSEVENT_SYSTEM, PBS_EVENTCLASS_JOB, this->ai_qs.parent_id, log_buf);

        return(INVALID_SLOT_LIMIT);
        }
      }
    else
      {
      this->ai_qs.slot_limit = max_limit;
      }
    }
  else
    {
    this->ai_qs.slot_limit = max_limit;
    }

  return(PBSE_NONE);
  } /* END set_slot_limit() */



/*
 * set_idle_slot_limit()
 * Sets the idle slot limit for this job array.
 *
 * @param requested_limit - the user-requested limit, or -1 if none was requested
 * @return PBSE_NONE if the requested limit was valid or unset, -1 if invalid
 */

int job_array::set_idle_slot_limit(

  long requested_limit)

  {
  long max_idle_slot_limit = -1;
  
  if (get_svr_attr_l(SRV_ATR_IdleSlotLimit, &max_idle_slot_limit) == PBSE_NONE)
    {
    if (max_idle_slot_limit > 0)
      {
      if (requested_limit > max_idle_slot_limit)
        return(-1);
      }
    }
  else
    max_idle_slot_limit = DEFAULT_IDLE_SLOT_LIMIT;

  if (requested_limit > 0)
    this->ai_qs.idle_slot_limit = requested_limit;
  else
    this->ai_qs.idle_slot_limit = max_idle_slot_limit;

  return(PBSE_NONE);
  } // END set_idle_slot_limit()



/*
 * parse_array_request()
 *
 * Parses the array request, checks that we aren't above the maximum size, and allocates the
 * array for the job ids
 *
 * @param request - the user submitted array request
 * @return PBSE_NONE on success, or ARRAY_TOO_LARGE if we're above the maximum size, or
 *         PBSE_MEM_MALLOC, or the rc from translate_range_string_to_vector if an invalid range
 *         was submitted.
 */

int job_array::parse_array_request(
    
  const char *request)

  {
  int  rc = PBSE_NONE;
  long max_array_size;
  char log_buf[LOCAL_LOG_BUF_SIZE];
  this->uncreated_ids.clear();

  if ((rc = translate_range_string_to_vector(request, this->uncreated_ids)) != PBSE_NONE)
    return(rc);

  this->ai_qs.range_str = request;
  this->ai_qs.num_jobs = this->uncreated_ids.size();

  // size of array is the biggest index + 1
  this->ai_qs.array_size = this->uncreated_ids[this->uncreated_ids.size() - 1] + 1;

  if (get_svr_attr_l(SRV_ATR_MaxArraySize, &max_array_size) == PBSE_NONE)
    {
    if (max_array_size < this->ai_qs.num_jobs)
      {
      return(ARRAY_TOO_LARGE);
      }
    }

  /* initialize the array */
  this->job_ids = (char **)calloc(this->ai_qs.array_size, sizeof(char *));
  if (this->job_ids == NULL)
    {
    sprintf(log_buf, "Failed to alloc job_ids: job %s", this->ai_qs.parent_id);
    log_event(PBSEVENT_JOB, PBS_EVENTCLASS_JOB, __func__, log_buf);
    return(PBSE_MEM_MALLOC);
    }

  return(rc);
  } /* END parse_array_request() */



bool job_array::need_to_update_slot_limits() const

  {
  return(this->ai_qs.slot_limit > this->ai_qs.jobs_running);
  } // END need_to_update_slot_limits()



/*
 * get_next_index_to_create()
 *
 * Determines the index of the next subjob that should be created
 *
 * @param internal_index - the index of uncreated_ids that corresponds to the subjob index returned
 * @return the index of the next subjob to be created, or -1 if no job should be created
 * at this time.
 */

int job_array::get_next_index_to_create(
    
  int &internal_index)

  {
  int index = -1;
  internal_index = -1;

  // Don't instantiate new jobs after we've been deleted
  if (this->being_deleted == false)
    {
    if ((this->ai_qs.idle_slot_limit == NO_SLOT_LIMIT) ||
        (this->ai_qs.num_idle < this->ai_qs.idle_slot_limit))
      {
      for (size_t i = 0; i < this->uncreated_ids.size(); i++)
        {
        if (this->uncreated_ids[i] != -1)
          {
          index = this->uncreated_ids[i];
          internal_index = i;
          break;
          }
        }
      }
    }

  return(index);
  } // END get_next_index_to_create()



/*
 * create_job_if_needed()
 *
 * Creates a new array subjob if the array's idle count is below the idle slot limit
 */

void job_array::create_job_if_needed()

  {
  int  uncreated_ids_index;
  int  next_index = this->get_next_index_to_create(uncreated_ids_index);

  if (next_index >= 0)
    {
    job *template_job = svr_find_job(this->ai_qs.parent_id, FALSE);

    if (template_job != NULL)
      {
      mutex_mgr template_mgr(template_job->ji_mutex, true);
      mutex_mgr array_mgr(this->ai_mutex, true);

      char  old_id[PBS_MAXSVRJOBID + 1];
      char  prev_job_id[PBS_MAXSVRJOBID + 1];
      char *hostname_extension;
      char *bracket;

      strcpy(old_id, template_job->ji_qs.ji_jobid);
      hostname_extension = strchr(old_id, '.');
      bracket = strchr(old_id, '[');

      if (bracket != NULL)
        *bracket = '\0';

      if (hostname_extension != NULL)
        snprintf(prev_job_id, sizeof(prev_job_id), "%s[%d]%s",
          old_id, this->ai_qs.highest_id_created, hostname_extension);
      else
        snprintf(prev_job_id, sizeof(prev_job_id), "%s[%d]", old_id, this->ai_qs.highest_id_created);

      std::string prev_id(prev_job_id);
      
      int rc = create_and_queue_array_subjob(this, array_mgr, template_job, template_mgr,
                                             next_index, prev_id, false);

      if (rc == PBSE_NONE)
        {
        this->uncreated_ids[uncreated_ids_index] = -1;
        this->ai_qs.highest_id_created = next_index;
        }
      }
    }

  } // END create_job_if_needed()



/**
 * update_array_values()
 *
 * updates internal bookkeeping values for job arrays
 * @param old_state - the prior state for this job
 * @param event - code for what event just happened
 * @param job_id - the id of the job that this happened to
 * @param job_exit_status - the exit status, only used if the event is aeTerminate
 */

void job_array::update_array_values(

  int                   old_state, /* I */
  enum ArrayEventsEnum  event,     /* I */
  const char           *job_id,
  int                   job_exit_status)

  {
  bool  moab_compatible;

  switch (event)
    {
    case aeQueue:

      if (old_state >= JOB_STATE_RUNNING)
        this->ai_qs.num_idle++;

      break;

    case aeRun:

      if (old_state != JOB_STATE_RUNNING)
        {
        if (this->ai_qs.jobs_running < this->ai_qs.num_jobs)
          {
          this->ai_qs.jobs_running++;
          this->ai_qs.num_started++;
          }

        if (old_state < JOB_STATE_RUNNING)
          {
          this->ai_qs.num_idle--;
          this->create_job_if_needed();
          }
        }

      break;

    case aeTerminate:

      switch (old_state)
        {
        case JOB_STATE_QUEUED:
        case JOB_STATE_HELD:
        case JOB_STATE_WAITING:

          this->ai_qs.num_idle--;
          this->create_job_if_needed();

          break;

        case JOB_STATE_RUNNING:
        
          if (this->ai_qs.jobs_running > 0)
            this->ai_qs.jobs_running--;

          break;
        }

      this->ai_qs.jobs_done++;

      if (job_exit_status == 0)
        this->ai_qs.num_successful++;
      else
        this->ai_qs.num_failed++;

      array_save(this);

      /* update slot limit hold if necessary */
      if (get_svr_attr_b(SRV_ATR_MoabArrayCompatible, &moab_compatible) != PBSE_NONE)
        moab_compatible = false;

      if (moab_compatible == true)
        {
        if (this->need_to_update_slot_limits() == true)
          {
          int  i;
          int  newstate;
          int  newsub;
          job *pj;

          /* find the first held job and release its hold */
          for (i = 0; i < this->ai_qs.array_size; i++)
            {
            if (this->job_ids[i] == NULL)
              continue;

            if (!strcmp(this->job_ids[i], job_id))
              continue;

            if ((pj = svr_find_job(this->job_ids[i], TRUE)) == NULL)
              {
              free(this->job_ids[i]);
              this->job_ids[i] = NULL;
              }
            else
              {
              mutex_mgr pj_mutex = mutex_mgr(pj->ji_mutex, true);
              if (pj->ji_wattr[JOB_ATR_hold].at_val.at_long & HOLD_l)
                {
                pj->ji_wattr[JOB_ATR_hold].at_val.at_long &= ~HOLD_l;
                
                if (pj->ji_wattr[JOB_ATR_hold].at_val.at_long == 0)
                  {
                  pj->ji_wattr[JOB_ATR_hold].at_flags &= ~ATR_VFLAG_SET;
                  }
                
                svr_evaljobstate(*pj, newstate, newsub, 1);
                svr_setjobstate(pj, newstate, newsub, FALSE);
                job_save(pj, SAVEJOB_FULL, 0);
                
                break;
                }

              }
            }
          }
        }

      break;

    case aeRerun:

      if (old_state == JOB_STATE_RUNNING)
        {
        if (this->ai_qs.jobs_running > 0)
          this->ai_qs.jobs_running--;

        if (this->ai_qs.num_started > 0)
          this->ai_qs.num_started--;
        }

      if (old_state >= JOB_STATE_RUNNING)
        this->ai_qs.num_idle++;

    default:

      /* log error? */

      break;
    }

  set_array_depend_holds(this);

  array_save(this);
  } /* END update_array_values() */



/*
 * initialize_uncreated_ids()
 *
 * Populates the uncreated ids vector, usually called after a restart
 */

void job_array::initialize_uncreated_ids()

  {
  std::vector<int> temp;
  translate_range_string_to_vector(this->ai_qs.range_str.c_str(), temp);

  this->uncreated_ids.clear();

  for (size_t i = 0; i < temp.size(); i++)
    {
    if (temp[i] > this->ai_qs.highest_id_created)
      this->uncreated_ids.push_back(temp[i]);
    }
  }



void job_array::mark_deleted()
  {
  this->being_deleted = true;
  }

bool job_array::is_deleted() const
  {
  return(this->being_deleted);
  }



