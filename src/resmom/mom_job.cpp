
#include "pbs_job.h"
#include "start_exec.h"

/*
 * CONSTRUCTOR
 */

mom_job::mom_job() : ji_momstat(0), ji_momhandle(-1), ji_radix(0), ji_grpcache(NULL),
                     ji_checkpoint_time(0), ji_checkpoint_next(0), ji_sampletim(0),
                     ji_momsubt(0), ji_mompost(NULL), ji_preq(NULL), ji_numnodes(0),
                     ji_numsisternodes(0), ji_numvnod(0), ji_numsistervnod(0), ji_outstanding(0),
                     ji_im_nodeid(0), ji_nodeid(0), ji_taskid(TM_NULL_TASK + 1),
                     ji_obit(TM_NULL_EVENT), ji_intermediate_join_event(0), ji_hosts(NULL),
                     ji_sisters(NULL), ji_vnods(0), ji_resources(0), ji_tasks(), ji_usages(),
                     ji_nodekill(TM_ERROR_NODE), ji_flags(0), ji_portout(0), ji_porterr(0),
                     ji_stdout(0), ji_stderr(0), ji_im_stdout(0), ji_im_stderr(0), ji_im_portout(0),
                     ji_im_porterr(0), ji_job_is_being_rerun(false), ji_mempressure_curr(0),
                     ji_mempressure_cnt(0), ji_examined(0), ji_kill_started(0), ji_joins_sent(0),
                     ji_obit_busy_time(0), ji_obit_minus_one_time(0), ji_exited_time(0),
                     ji_obit_sent(0), ji_state_set(0), ji_joins_resent(0), ji_stats_done(false),
                     ji_job_pid_set(), ji_sigtermed_processes(), ji_cgroups_created(false),
                     ji_custom_usage_info(), ji_commit_done(false), maxAdoptedTaskId(0)

  {
  CLEAR_LINK(this->ji_jobque);
  CLEAR_LINK(this->ji_alljobs);

  memset(&this->ji_altid, 0, sizeof(this->ji_altid));
  memset(&this->ji_globid, 0, sizeof(this->ji_globid));
  }



/*
 * DESTRUCTOR
 */

mom_job::~mom_job() 
  {
  if (this->ji_grpcache)
    free(this->ji_grpcache);

  assert(this->ji_preq == NULL);

  nodes_free(this);

  // Delete each remaining task
  for (size_t i = 0; i < this->ji_tasks.size(); i++)
    {
    task *ptask = this->ji_tasks[i];
    if (ptask->ti_chan_reused == TRUE)
      ptask->ti_chan = NULL;
    delete ptask;
    }

  if (this->ji_resources)
    free(this->ji_resources);
  }

