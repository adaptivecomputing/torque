
#include <pbs_config.h>

#ifdef ENABLE_PMIX
#include <sys/types.h>
#include <signal.h>
#include <stdio.h>

#include "pmix_tracker.hpp"
#include "log.h"
#include "mom_func.h"
#include "pbs_error.h"

int kill_job(job *pjob, int sig, const char *killer_id, const char *kill_reason);

extern char mom_alias[];

std::list<pmix_tracker *> tracker_list;



void pmix_tracker::check_and_add_task(

  job *pjob,
  int  rank)

  {
  task *ptask;

  if ((ptask = task_find(pjob, rank)) != NULL)
    {
    // Found it
    pmix_task pt(ptask->ti_qs.ti_task, ptask->ti_qs.ti_sid);
    this->tasks[rank] = pt;
    }
  else
    {
    // Add a new task
    pbs_task_create(pjob, rank);
    pmix_task pt(rank, -1);
    this->tasks[rank] = pt;
    }
  }



pmix_tracker::pmix_tracker(

  job *pjob,
  int  rank) : jid(pjob->ji_qs.ji_jobid), tasks()

  {
  this->check_and_add_task(pjob, rank);

  tracker_list.push_back(this);
  } // END constructor()



int pmix_tracker::finalize_process(
    
  int pid)

  {
  std::map<int, pmix_task>::iterator it = this->tasks.find(pid);

  if (it != this->tasks.end())
    this->tasks.erase(it);

  return(PBSE_NONE);
  } // END finalize_process()



/*
 * find_and_kill_task()
 * Finds the task with a matching rank and terminates it
 *
 * @param pjob - the job whose task we're killing
 * @param rank - the rank of the task
 * @return PBSE_NONE on SUCCESS, -1 if the task wasn't found, or a return from kill_task
 */

int find_and_kill_task(

  job *pjob,
  int  rank)

  {
  int   rc = -1;
  task *ptask;

  if ((ptask = task_find(pjob, rank)) != NULL)
    rc = kill_task(pjob, ptask, SIGKILL, 0);

  return(rc);
  } // END find_and_kill_task()



int pmix_tracker::abort(
    
  int         status,
  pmix_proc_t proc_list[],
  size_t      nprocs,
  const char  abort_msg[])

  {
  int  rc = PBSE_JOBNOTFOUND;
  job *pjob = mom_find_job(this->jid.c_str());

  if (pjob != NULL)
    {
    pjob->ji_qs.ji_un.ji_exect.ji_exitstat = status;

    if (proc_list == NULL)
      rc = kill_job(pjob, SIGKILL, "pmix_server", abort_msg);
    else
      {
      // Abort just the processes in the list
      for (size_t i = 0; i < nprocs; i++)
        find_and_kill_task(pjob, proc_list[i].rank);
      }
    }

  return(rc);
  } // END abort()
  


bool pmix_tracker::id_matches(
    
  const char *jobint_string)

  {
  bool matches = strncmp(jobint_string, this->jid.c_str(), strlen(jobint_string)) == 0;

  return(matches);
  } // END id_matches()



pmix_tracker *get_pmix_tracker(
    
  const char *jobint_string)

  {
  pmix_tracker *pt = NULL;

  std::list<pmix_tracker *>::iterator it;

  for (it = tracker_list.begin(); it != tracker_list.end(); it++)
    {
    if ((*it)->id_matches(jobint_string))
      pt = *it;
    }

  return(pt);
  } // END get_pmix_tracker()




#endif
