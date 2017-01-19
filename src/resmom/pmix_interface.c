
#include <pbs_config.h>

#ifdef ENABLE_PMIX

#include "pmix_server.h"
#include "pmix_tracker.hpp"
#include "pmix_operation.hpp"
#include "pbs_job.h"
#include "mom_func.h"

job *mom_find_job_by_int_string(const char *);
void report_fence_to_ms(job *pjob, char *data);
int start_process(task *ptask, char **argv, char **envp);
int send_tm_spawn_request(job *pjob, hnodent *remote_host, char **argv, char **env, int event, int fromtask, int *reply_ptr);

extern char   mom_alias[];
extern time_t pbs_tcp_timeout;

bool pmix_spawn_timed_out;

/*
 * pmix_client_connect()
 *
 * Connects a client instance of a pmix job to the mom
 */

pmix_status_t pmix_client_connect(

  const pmix_proc_t *proc,
  void              *svr_obj)
/* I think version 2 has a callback function and data
 * pmix_op_cbfunc_t   cbfunc,
  void              *cbdata)*/

  {
  pmix_status_t  rc = PMIX_SUCCESS;
  job           *pjob = mom_find_job_by_int_string(proc->nspace);
  pmix_tracker  *pt = get_pmix_tracker(proc->nspace);

  if (pjob != NULL)
    {
    if (pt == NULL)
      {
      pmix_tracker *pt = new pmix_tracker(pjob, proc->rank);
      tracker_list.push_back(pt);
      }
    else
      pt->check_and_add_task(pjob, proc->rank);
    }
  else
    rc = PMIX_ERR_NOT_FOUND;

/*  I think this is for version 2
 *  if (cbfunc != NULL)
    cbfunc(rc, cbdata);*/

  return(rc);
  } // END pmi_client_connect()



/*
 * pmix_client_finalize()
 *
 * Lets the mom know that the client pmix task is done
 */

pmix_status_t pmix_client_finalize(

  const pmix_proc_t *proc,
  void              *svr_obj,
  pmix_op_cbfunc_t   cbfunc,
  void              *cbdata)

  {
  pmix_status_t  rc = PMIX_ERR_NOT_FOUND;
  pmix_tracker  *pt = get_pmix_tracker(proc->nspace);

  if (pt != NULL)
    {
    pt->finalize_process(proc->rank);

    if (cbfunc != NULL)
      {
      cbfunc(PMIX_SUCCESS, cbdata);
      }

    rc = PMIX_SUCCESS;
    }

  return(rc);
  } // END pmix_client_finalize()



/*
 * pmix_server_abort()
 *
 * Aborts the current job that is being run via pmix
 */

pmix_status_t pmix_server_abort(

  const pmix_proc_t *proc,
  void              *svr_obj,
  int                status,
  const char         msg[],
  pmix_proc_t        procs_to_abort[],
  size_t             nprocs,
  pmix_op_cbfunc_t   cbfunc,
  void              *cbdata)

  {
  pmix_tracker  *pt = get_pmix_tracker(proc->nspace);
  pmix_status_t  rc = PMIX_ERR_NOT_FOUND;

  if (pt != NULL)
    {
    pt->abort(status, procs_to_abort, nprocs, msg);
    rc = PMIX_SUCCESS;
    }

  if (cbfunc != NULL)
    cbfunc(rc, cbdata);

  return(rc);
  } // END pmix_server_abort()



std::map<std::string, pmix_operation> pending_fences;



/*
 * check_and_record_fence()
 *
 * Adds this fence operation if it isn't already pending; if it isn't create it.
 * @return true if the fence operation is now complete
 */

bool check_and_record_fence(

  job                 *pjob,
  const std::string   &jobid,
  const pmix_proc_t    procs[],
  size_t               nprocs,
  char                *data,
  const std::string   &mom_host,
  pmix_modex_cbfunc_t  cbfunc,
  void                *cbdata)

  {
  std::string info;

  if (data != NULL)
    info = data;

  if (pending_fences.find(jobid) == pending_fences.end())
    {
    pmix_operation fence(pjob, procs, nprocs, info, cbfunc, cbdata);
    pending_fences[jobid] = fence;
    }

  return(pending_fences[jobid].mark_reported(mom_host));
  } // END check_and_record_fence()



void notify_fence_complete(

  job *pjob)

  {
  if (pending_fences.find(pjob->ji_qs.ji_jobid) != pending_fences.end())
    {
    pending_fences[pjob->ji_qs.ji_jobid].complete_operation(pjob, 0);
    }
  } // END notify_fence_complete()



/*
 * pmix_server_fencenb()
 *
 * Gives us a list of processes and a function to call once each of them has
 * called pmix_server_fencenb()
 */

pmix_status_t pmix_server_fencenb(

  const pmix_proc_t    procs[],
  size_t               nprocs,
  const pmix_info_t    info[],
  size_t               ninfo,
  char                *data,
  size_t               ndata,
  pmix_modex_cbfunc_t  cbfunc,
  void                *cbdata)

  {
  pmix_status_t rc = PMIX_SUCCESS;

  job *pjob = mom_find_job_by_int_string(procs[0].nspace);

  if (pjob != NULL)
    {
    if (am_i_mother_superior(*pjob) == false)
      {
      check_and_record_fence(NULL, procs[0].nspace, procs, nprocs, data, mom_alias, cbfunc, cbdata);
      report_fence_to_ms(pjob, data);
      }
    else if (check_and_record_fence(pjob, procs[0].nspace, procs, nprocs, data, mom_alias, cbfunc, cbdata) == true)
      {
      notify_fence_complete(pjob);
      }
    }
  else
    rc = PMIX_ERR_NOT_FOUND;

  return(rc);
  } // END pmix_server_fencenb()



pmix_status_t pmix_server_dmodex_req(

  const pmix_proc_t   *proc,
  const pmix_info_t    info[],
  size_t               ninfo,
  pmix_modex_cbfunc_t  cbfunc,
  void                *cbdata)

  {
  return(PMIX_ERR_NOT_SUPPORTED);
  } // END pmix_dmodex_req()



pmix_status_t pmix_server_publish(

  const pmix_proc_t *proc,
  const pmix_info_t  info[],
  size_t             ninfo,
  pmix_op_cbfunc_t   cbfunc,
  void              *cbdata)

  {
  return(PMIX_ERR_NOT_SUPPORTED);
  }



pmix_status_t pmix_server_lookup(

  const pmix_proc_t     *proc,
  char                 **keys,
  const pmix_info_t      info[],
  size_t                 ninfo,
  pmix_lookup_cbfunc_t   cbfunc,
  void                  *cbdata)

  {
  return(PMIX_ERR_NOT_SUPPORTED);
  }



pmix_status_t pmix_server_unpublish(

  const pmix_proc_t     *proc,
  char                 **keys,
  const pmix_info_t      info[],
  size_t                 ninfo,
  pmix_op_cbfunc_t       cbfunc,
  void                  *cbdata)

  {
  return(PMIX_ERR_NOT_SUPPORTED);
  }



bool host_is_part_of_job(

  job        *pjob,
  const char *hostname)

  {
  bool found = false;
  for (int i = 0; i < pjob->ji_numnodes; i++)
    {
    if (!strcmp(hostname, pjob->ji_hosts[i].hn_host))
      {
      found = true;
      break;
      }
    }

  return(found);
  }



pmix_status_t sanity_check_apps(

  const pmix_app_t  apps[],
  size_t            napps,
  job              *pjob)

  {
  pmix_status_t  rc = PMIX_SUCCESS;

  for (size_t i = 0; i < napps; i++)
    {
    for (size_t j = 0; j < apps[i].ninfo; j++)
      {
      if (!strcmp(apps[i].info[j].key, PMIX_HOSTNAME))
        {
        if (host_is_part_of_job(pjob, apps[i].info[j].value.data.string) == false)
          {
          // We don't support dynamic jobs at this point
          rc = PMIX_ERR_NOT_SUPPORTED;
          break;
          }
        }
      }
    }

  return(rc);
  } // END sanity_check_apps()



/*
 * launch_pmix_requested_application_locally()
 *
 * Launches an application requested via the PMIx interface on this local machine
 * @param pjob - the job this application is a part of
 * @param application - the pmix object describing the application
 * @return PBSE_NONE on SUCCESS or a PBSE_* errno
 */

int launch_pmix_requested_application_locally(

  job              *pjob,
  const pmix_app_t &application)

  {
  int   rc = PBSE_NONE;
  task *ptask = pbs_task_create(pjob, TM_NULL_TASK);

  if (ptask != NULL)
    {
    if ((rc = start_process(ptask, application.argv, application.env)) == PBSE_NONE)
      {
      (*pjob->ji_usages)[mom_alias].increment_usage(application.maxprocs);
      }
    }
  else
    rc = PBSE_SYSTEM;

  return(rc);
  } // END launch_pmix_requested_application_locally()



int launch_pmix_requested_application_remotely(

  job               *pjob,
  const pmix_app_t  &application,
  const std::string &remote_host,
  int                requestor_rank)

  {
  int      rc = PBSE_NONE;
  int      dummy;
  hnodent *np = NULL;

  for (int i = 0; i < pjob->ji_numnodes; i++)
    {
    if (remote_host == pjob->ji_hosts[i].hn_host)
      {
      np = pjob->ji_hosts + i;
      break;
      }
    }

  if (np == NULL)
    return(-1);

  rc = send_tm_spawn_request(pjob,
                             np,
                             application.argv,
                             application.env,
                             TM_NULL_EVENT,
                             requestor_rank,
                             &dummy);

  return(rc);
  } // END launch_pmix_requested_application_remotely()



int launch_pmix_requested_application(

  job              *pjob,
  const pmix_app_t &application,
  int               requestor_rank)

  {
  std::string  specified_host;
  int          rc = PMIX_SUCCESS;

  // Check for a remote host
  for (size_t i = 0; i < application.ninfo; i++)
    {
    if (!strcmp(application.info[i].key, PMIX_HOSTNAME))
      {
      specified_host = application.info[i].value.data.string;
      }
    }

  if (specified_host.size() == 0)
    {
    // No host specified, try this host first
    if ((*pjob->ji_usages)[mom_alias].can_fit(application.maxprocs))
      rc = launch_pmix_requested_application_locally(pjob, application);
    else
      {
      bool launched = false;

      for (std::map<std::string, job_host_data>::iterator it = pjob->ji_usages->begin();
           it != pjob->ji_usages->end();
           it++)
        {
        // Skip the local host since we already checked it
        if (it->first == mom_alias)
          continue;
        else if ((*pjob->ji_usages)[it->first].can_fit(application.maxprocs))
          {
          rc = launch_pmix_requested_application_remotely(pjob,
                                                          application,
                                                          it->first,
                                                          requestor_rank);
          launched = true;
          break;
          }
        }

      if (launched == false)
        rc = launch_pmix_requested_application_locally(pjob, application);
      }
    }
  else if (specified_host == mom_alias)
    {
    // Local host specified
    rc = launch_pmix_requested_application_locally(pjob, application);
    }
  else
    {
    // Remote host specified
    rc = launch_pmix_requested_application_remotely(pjob,
                                                    application,
                                                    specified_host,
                                                    requestor_rank);
    }

  return(rc);
  } // END launch_pmix_requested_application()



/*
 * Called by PMIx as a result of a client requesting to spawn some applications
 */

pmix_status_t pmix_server_spawn(

  const pmix_proc_t   *proc,
  const pmix_info_t    job_info[],
  size_t               ninfo,
  const pmix_app_t     apps[],
  size_t               napps,
  pmix_spawn_cbfunc_t  cbfunc,
  void                *cbdata)

  {
  pmix_status_t     rc = PMIX_SUCCESS;
  job              *pjob = mom_find_job_by_int_string(proc->nspace);
  struct timeval    timeout;
  time_t            start_time;
  time_t            global_tcp_timeout = 0;
  //bool            notify = false;

  memset(&timeout, 0, sizeof(timeout));

  if (pjob == NULL)
    rc = PMIX_ERR_NOT_FOUND;
  else
    {
    rc = sanity_check_apps(apps, napps, pjob);

    for (size_t i = 0; i < ninfo; i++)
      {
      if (!strcmp(job_info[i].key, PMIX_TIMEOUT))
        {
        memcpy(&timeout, &job_info[i].value.data.tv, sizeof(timeout));
        }
      /* NYI: this depends on functionality not yet fully implemented by PMIx
      else if (!strcmp(job_info[i].key, PMIX_NOTIFY_COMPLETION))
        {
        notify = true;
        } */
      }

    if (timeout.tv_sec != 0)
      {
      start_time = time(NULL);
      if (pbs_tcp_timeout > timeout.tv_sec)
        {
        global_tcp_timeout = pbs_tcp_timeout;
        pbs_tcp_timeout = timeout.tv_sec;
        }
      }

    for (size_t i = 0; i < napps; i++)
      {
      int pbs_rc = launch_pmix_requested_application(pjob, apps[i], proc->rank);
      if (pbs_rc != PBSE_NONE)
        {
        // Stop
        break;
        }
      else if (time(NULL) - start_time >= timeout.tv_sec)
        {
        rc = PMIX_ERR_TIMEOUT;
        break;
        }
      }

    if (global_tcp_timeout != 0)
      {
      pbs_tcp_timeout = global_tcp_timeout;
      }
    }

  return(rc);
  } // END pmix_server_spawn()



int matches_existing_connection(

  const pmix_proc_t procs[],
  size_t            nprocs)

  {
  for (std::map<unsigned int, pmix_operation>::iterator it = existing_connections.begin();
       it != existing_connections.end();
       it++)
    {
    if (it->second.ranks_match(procs, nprocs) == true)
      return(it->first);
    }

  return(0);
  } // END matches_existing_connection()



pmix_status_t pmix_server_connect(

  const pmix_proc_t  procs[],
  size_t             nprocs,
  const pmix_info_t  info[],
  size_t             ninfo,
  pmix_op_cbfunc_t   cbfunc,
  void              *cbdata)

  {
  pmix_status_t     rc = PMIX_SUCCESS;
  job              *pjob = mom_find_job_by_int_string(procs[0].nspace);
  struct timeval    timeout;
  bool              called_back = false;

  memset(&timeout, 0, sizeof(timeout));

  if (pjob != NULL)
    {
    for (size_t i = 0; i < ninfo; i++)
      {
      if (!strcmp(info[i].key, PMIX_TIMEOUT))
        {
        memcpy(&timeout, &info[i].value.data.tv, sizeof(timeout));
        }
      }

    if (matches_existing_connection(procs, nprocs) == 0)
      {
      pmix_operation connect(pjob, procs, nprocs, cbfunc, cbdata);
      existing_connections[connect.get_operation_id()] = connect;
      rc = connect.complete_operation(pjob, timeout.tv_sec);

      if (rc == PMIX_SUCCESS)
        called_back = true;
      }
    }
  else
    rc = PMIX_ERR_NOT_FOUND;

  if (called_back == false)
    cbfunc(rc, cbdata);

  return(rc);
  } // END pmix_server_connect()



pmix_status_t pmix_server_disconnect(

  const pmix_proc_t  procs[],
  size_t             nprocs,
  const pmix_info_t  info[],
  size_t             ninfo,
  pmix_op_cbfunc_t   cbfunc,
  void              *cbdata)

  {
  pmix_status_t     rc = PMIX_SUCCESS;
  job              *pjob = mom_find_job_by_int_string(procs[0].nspace);
  struct timeval    timeout;
  bool              called_back = false;

  memset(&timeout, 0, sizeof(timeout));

  if (pjob != NULL)
    {
    for (size_t i = 0; i < ninfo; i++)
      {
      if (!strcmp(info[i].key, PMIX_TIMEOUT))
        {
        memcpy(&timeout, &info[i].value.data.tv, sizeof(timeout));
        }
      }

    unsigned int op_id = matches_existing_connection(procs, nprocs);

    if (op_id == 0)
      {
      rc = PMIX_ERR_NOT_FOUND;
      }
    else
      {
      pmix_operation connect(pjob, procs, nprocs, cbfunc, cbdata, DISCONNECT_OPERATION);
      connect.set_id(op_id);
      rc = connect.complete_operation(pjob, timeout.tv_sec);

      // Remove this connection from our map
      std::map<unsigned int, pmix_operation>::iterator it = existing_connections.find(op_id);
      if (it != existing_connections.end())
        existing_connections.erase(it);

      if (rc == PMIX_SUCCESS)
        called_back = true;
      }
    }
  else
    rc = PMIX_ERR_NOT_FOUND;

  if (called_back == false)
    cbfunc(rc, cbdata);

  return(rc);
  } // END pmix_server_disconnect()



pmix_status_t pmix_server_register_events(

  const pmix_info_t     info[],
  size_t                ninfo,
  pmix_op_cbfunc_t      cbfunc,
  void                  *cbdata)

  {
  return(PMIX_ERR_NOT_SUPPORTED);
  }



pmix_status_t pmix_server_deregister_events(

  const pmix_info_t     info[],
  size_t                ninfo,
  pmix_op_cbfunc_t      cbfunc,
  void                  *cbdata)

  {
  return(PMIX_ERR_NOT_SUPPORTED);
  } // END pmix_server_deregister_event()

pmix_server_module_t psm =
  {
  pmix_client_connect,
  pmix_client_finalize,
  pmix_server_abort,
  pmix_server_fencenb,
  pmix_server_dmodex_req,
  pmix_server_publish,
  pmix_server_lookup,
  pmix_server_unpublish,
  pmix_server_spawn,
  pmix_server_connect,
  pmix_server_disconnect,
  pmix_server_register_events,
  pmix_server_deregister_events,
  NULL
  };



#endif

