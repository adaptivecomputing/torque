#include <stdio.h>
#include <stdlib.h>
#include <map>
#include <string>

#include <pbs_config.h>

#include "pmix_operation.hpp"
#include "pmix_tracker.hpp"

std::map<unsigned int, pmix_operation> existing_connections;

int    completed_op = 0;
bool   ms = false;
time_t pbs_tcp_timeout = 300;
const int DISCONNECT_OPERATION = 3;
char         mom_alias[PBS_MAXHOSTNAME + 1];

std::list<pmix_tracker *> tracker_list;

pmix_status_t pmix_operation::complete_operation(job *pjob, long timeout)
  {
  completed_op++;
  return(PMIX_SUCCESS);
  }

bool pmix_operation::mark_reported(const std::string &hostname)
  {
  return(true);
  }

pmix_operation::pmix_operation() {}
pmix_operation::pmix_operation(job *pjob, const pmix_proc_t procs[], size_t nprocs, const std::string &data,
    pmix_modex_cbfunc_t cbfunc, void *cbdata)
  {
  }
  
pmix_operation::pmix_operation(job *pjob, const pmix_proc_t procs[], size_t nprocs, pmix_op_cbfunc_t to_call,
                 void *cbdata)
  {
  }
  
pmix_operation::pmix_operation(job *pjob, const pmix_proc_t procs[], size_t nprocs, pmix_op_cbfunc_t to_call,
                               void *cbdata, int operation_type)
  {
  }

pmix_operation::pmix_operation(const pmix_operation &other)
  {
  }

pmix_operation &pmix_operation::operator =(const pmix_operation &other)
  {
  return(*this);
  }

bool pmix_operation::ranks_match(const pmix_proc_t procs[], size_t nprocs)
  {
  return(true);
  }

unsigned int pmix_operation::get_operation_id() const
  {
  return(0);
  }

void pmix_operation::set_id(unsigned int op_id) {}

pmix_tracker::pmix_tracker(job *pjob, int rank)
  {
  }

void pmix_tracker::check_and_add_task(job *pjob, int rank)
  {
  }

int pmix_tracker::finalize_process(int rank)
  {
  return(0);
  }

int pmix_tracker::abort(int status, pmix_proc_t proc_list[], size_t nprocs, const char abort_msg[])
  {
  return(0);
  }

void report_fence_to_ms(

  job  *pjob,
  char *data)

  {
  }

bool am_i_mother_superior(const job &pjob)
  {
  return(ms);
  }

int send_tm_spawn_request(

  job      *pjob,
  hnodent  *remote_host,
  char    **argv,
  char    **env,
  int       event,
  int       fromtask,
  int      *reply_ptr)

  {
  return(0);
  }

pmix_tracker *get_pmix_tracker(
    
  const char *jobint_string)

  {
  return(NULL);
  }

job *mom_find_job_by_int_string(

  const char *int_string)

  {
  return(NULL);
  }

task *pbs_task_create(

  job        *pjob,
  tm_task_id  taskid)

  {
  return(NULL);
  }

int start_process(

  task  *ptask,   /* I */
  char **argv,    /* I */
  char **envp)    /* I */

  {
  return(0);
  }

