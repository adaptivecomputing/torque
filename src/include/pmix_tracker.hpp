#ifndef PMIX_TRACKER_HPP
#define PMIX_TRACKER_HPP

#include <pbs_config.h>

#ifdef ENABLE_PMIX
#include <string>
#include <list>

#include "pbs_job.h"
#include "pmix_server.h"

class pmix_task
  {
  public:
  tm_task_id  tid; // Same as the MPI rank
  int         pid;

  pmix_task() : tid(0), pid(0) {}
  pmix_task(int t, int p) : tid(t), pid(p) {}
  };

class pmix_tracker
  {
  std::string              jid; // The id of the job
  std::map<int, pmix_task> tasks; // the key is the MPI rank

  public:
  pmix_tracker(job *pjob, int rank);
  int  finalize_process(int rank);
  int  end_job(int rank);
  int  abort(int status, pmix_proc_t proc_list[], size_t nprocs, const char abort_msg[]);
  void add_a_new_task(job *pjob, int pid);
  bool id_matches(const char *jobint_string);
  void check_and_add_task(job *pjob, int rank);
  };


extern std::list<pmix_tracker *> tracker_list;

pmix_tracker *get_pmix_tracker(const char *jobint_string);
void          register_pmix_tracker(pmix_tracker *pt);
int           remove_pmix_tracker(pmix_tracker *pt);

void          register_jobs_nspace(job *pjob, pjobexec_t *TJE);

#endif // ENABLE_PMIX
#endif // Header double include protection

