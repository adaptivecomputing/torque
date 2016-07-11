#include <stdio.h>
#include <time.h>
#include "job_func.h"
#include "pbs_job.h"
#include "log.h"
#include "../lib/Liblog/pbs_log.h"
#include "../lib/Liblog/log_event.h"
#include "work_task.h"
#include "completed_jobs_map.h"
#include "threadpool.h"

// This is set to true once we've begun exiting. At this point, global destructors have been
// called and we want to ignore requests
extern bool exit_called;

// external functions called

void handle_complete_second_time(struct work_task*);



completed_jobs_map_class::completed_jobs_map_class()
  {
  completed_jobs_map.clear();
  pthread_mutex_init(&completed_jobs_map_mutex, NULL);
  }

completed_jobs_map_class::~completed_jobs_map_class()
  {
  completed_jobs_map.clear();
  }

// add a job to the map
bool completed_jobs_map_class::add_job(

  const char *jobid,
  time_t      complete_time)

  {
  bool rc = true;
  std::string jid_string = jobid;

  // lock the map
  pthread_mutex_lock(&completed_jobs_map_mutex);

  // add job to map if not present
  if (completed_jobs_map.find(jid_string) == completed_jobs_map.end())
    {
    // add job completion time
    completed_jobs_map[jid_string] = complete_time;
    }
  else
    {
    // job id already in map - not added
    rc = false;
    }

  // unlock the map
  pthread_mutex_unlock(&completed_jobs_map_mutex);

  return(rc);
  }

// delete a job from the map
//   assume caller has set a lock
bool completed_jobs_map_class::delete_job_unlocked(

  std::string jid_string)

  {
  if (exit_called == true)
    return(true);
  
  bool rc = true;

  // remove job from map if present
  if (completed_jobs_map.find(jid_string) != completed_jobs_map.end())
    {
    // remove job
    completed_jobs_map.erase(jid_string);
    }
  else
    {
    // job id not in map
    rc = false;
    }

  return(rc);
  }

// delete a job from the map
//   assume caller has not set lock
bool completed_jobs_map_class::delete_job(

  const char *jobid)

  {
  if (exit_called == true)
    return(true);
  
  bool rc;
  std::string jid_string = jobid;

  // lock the map
  pthread_mutex_lock(&completed_jobs_map_mutex);

  // remove job from map if present
  rc = this->delete_job_unlocked(jid_string);

  // unlock the map
  pthread_mutex_unlock(&completed_jobs_map_mutex);

  return(rc);
  }

// see if a job id in the map
bool completed_jobs_map_class::is_job_id_in_map(

  const char *jobid)

  {
  if (exit_called == true)
    return(true);
  
  bool rc = false;
  std::string jid_string = jobid;

  // lock the map
  pthread_mutex_lock(&completed_jobs_map_mutex);

  if (completed_jobs_map.find(jid_string) != completed_jobs_map.end())
    {
    rc = true;
    }

  // unlock the map
  pthread_mutex_unlock(&completed_jobs_map_mutex);

  return(rc);
  }

// iterate over the map and remove any jobs
//   whose completed time has been reached (or exceeded)
int completed_jobs_map_class::cleanup_completed_jobs(

  void)

  {
  if (exit_called == true)
    return(PBSE_NONE);
  
  time_t now;
  std::vector<std::string> to_remove;
  char log_buf[LOCAL_LOG_BUF_SIZE + 1];
  int rc = PBSE_NONE;

  // lock the map
  pthread_mutex_lock(&completed_jobs_map_mutex);

  // get the current time
  now = time(0);

  // clean up any jobs that are beyond their adjusted completion time
  for (std::map<std::string,time_t>::iterator it = completed_jobs_map.begin();
       it != completed_jobs_map.end();
       ++it)
    {
    bool rc = false;

    // if cleanup time is before/at now, then clean up
    if (it->second <= now)
      {
      int retcode = PBSE_NONE;
      work_task *pnew;

      // Does job exist
      
      while((rc = job_id_exists(it->first, &retcode)) == false)
        {
        if (retcode != 0)
          {
          // Someone else has a lock we want. give ours up and try again
          pthread_mutex_unlock(&completed_jobs_map_mutex);
          pthread_mutex_lock(&completed_jobs_map_mutex);
          continue;
          }
        else
          break;
        }

        if (rc == false)
          {
          // Job is gone, mark it for removal
          to_remove.push_back(it->first);

          continue;
          }

      // create a work task struct to be passed to handle_complete_second_time()
      if ((pnew = (struct work_task *)calloc(1, sizeof(struct work_task))) == NULL)
        {
        rc = -1;
        goto cleanup;
        }

      // copy the job id
      if ((pnew->wt_parm1 = (void *)strdup(it->first.c_str())) == NULL)
        {
        free(pnew);
        rc = -1;
        goto cleanup;
        }

      // create space for mutex
      if ((pnew->wt_mutex = (pthread_mutex_t*)calloc(1, sizeof(pthread_mutex_t))) == NULL)
        {
        free(pnew->wt_parm1);
        free(pnew);
        rc = -1;
        goto cleanup;
        }

      if (LOGLEVEL >= 10)
        {
        snprintf(log_buf, LOCAL_LOG_BUF_SIZE, "%s: calling handle_complete_second_time()", it->first.c_str());
        log_event(PBSEVENT_DEBUG, PBS_EVENTCLASS_SERVER, __func__, log_buf);
        }

      // call with work task structure
      //   will remove job id from map
      enqueue_threadpool_request((void *(*)(void *))handle_complete_second_time, pnew, task_pool);
      }
    }

  // Remove marked jobs from map
  for (unsigned int i = 0; i < to_remove.size(); i++)
    {
    if (LOGLEVEL >= 10)
      {
      snprintf(log_buf, LOCAL_LOG_BUF_SIZE, "%s: removing job from completed_job_map", to_remove[i].c_str());
      log_event(PBSEVENT_DEBUG, PBS_EVENTCLASS_SERVER, __func__, log_buf);
      }

    this->delete_job_unlocked(to_remove[i]);
    }

cleanup:

  // unlock the map
  pthread_mutex_unlock(&completed_jobs_map_mutex);

  return(rc);
  }

// print the map (mostly useful for debugging)
void completed_jobs_map_class::print_map(

  void)

  {
  if (exit_called == true)
    return;

  if (completed_jobs_map.empty())
    {
    printf("completed jobs map is empty\n");
    }
  else
    {
    printf("%20s %s\n", "Job Id", "Completion time");
    for (std::map<std::string,time_t>::iterator it = completed_jobs_map.begin();
      it != completed_jobs_map.end();
      ++it)
      {
      printf("%20s %ld\n", it->first.c_str(), it->second);
      }
    }
  }



void add_to_completed_jobs(
    
  work_task *ptask)

  {
  extern completed_jobs_map_class completed_jobs_map;
  char *jobid = (char *)ptask->wt_parm1;

  if (jobid != NULL)
    {
    long delay = ptask->wt_event;
    completed_jobs_map.add_job(jobid, time(NULL) + delay);
    free(jobid);
    }

  free(ptask->wt_mutex);
  free(ptask);

  } // END add_to_completed_jobs()

