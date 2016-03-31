
#include <pbs_config.h>

#ifdef ENABLE_PMIX
#include <stdio.h>
#include <signal.h>

#include "pmix_operation.hpp"
#include "log.h"
#include "mom_func.h"

extern char mom_alias[];

void remote_notify_pmix_operation(job *pjob, const char *remote_host, const std::string &data,
                                  int op, struct sockaddr_in *dont_contact);

const int FENCE_OPERATION = 0;
const int CONNECT_OPERATION = 1;
const int DISCONNECT_OPERATION = 2;

// Start at 1 so that 0 can be considered uninitialized
unsigned int pmix_connect_id = 1;

std::map<unsigned int, pmix_operation> existing_connections;



bool pmix_operation::safe_insert_rank(

  job *pjob,
  int  rank)

  {
  bool inserted = false;

  if (rank < pjob->ji_numvnod)
    {
    const char *hostname = pjob->ji_vnods[rank].vn_host->hn_host;
    
    if (this->type == FENCE_OPERATION)
      {
      this->hosts_to_report.insert(hostname);
      }
    else
      {
      this->hosts_reported.insert(hostname);
      if (pjob->ji_vnods[rank].vn_host->hn_node == pjob->ji_nodeid)
        this->local_ranks.insert(rank);

      this->all_ranks.insert(rank);
      }
    
    inserted = true;
    }

  return(inserted);
  } // END safe_insert_rank()


// Constructor used for fence operations
pmix_operation::pmix_operation(

  job                 *pjob,
  const pmix_proc_t    procs[],
  size_t               nprocs,
  const std::string   &info,
  pmix_modex_cbfunc_t  cbfunc,
  void                *rdata) : data(info), hosts_reported(), hosts_to_report(), local_ranks(),
                                all_ranks(), to_call_fence(cbfunc), to_call_connect(NULL),
                                op_id(0), complete(false), type(FENCE_OPERATION), cbdata(rdata)

  {
  // if pjob is NULL, we are not mother superior
  if (pjob != NULL)
    {
    for (size_t i = 0; i < nprocs; i++)
      {
      if (this->safe_insert_rank(pjob, procs[i].rank) == false)
        {
        // Fence is asking for a rank that shouldn't exist on this job?
        snprintf(log_buffer, sizeof(log_buffer),
          "Fence request from PMIx server requested rank %d but we only have %d ranks",
          procs[i].rank, pjob->ji_numvnod);
        log_err(-1, pjob->ji_qs.ji_jobid, log_buffer);
        }
      }
    }
  else
    {
    // We are a sister. Mother superior will send the data later
    this->data.clear();
    }
  } // END constructor used for fence requests



int get_pmix_connection_id()

  {
  return(pmix_connect_id++);
  }



/* 
 * Constructor used for connect operations on the moms that are being notified of the connect
 * 
 * @param connect_data - op_id:rank1,rank2[,rank3[...]]
 */
pmix_operation::pmix_operation(

  char *connect_data,
  job  *pjob) : data(), hosts_reported(), hosts_to_report(), local_ranks(), all_ranks(),
                to_call_fence(NULL), to_call_connect(NULL), op_id(0), complete(false),
                type(CONNECT_OPERATION), cbdata(NULL)

  {
  char *ptr = connect_data;

  this->op_id = strtol(ptr, &ptr, 10);

  if (*ptr == ':')
    {
    do 
      {
      ptr++;

      int rank = strtol(ptr, &ptr, 10);

      if (this->safe_insert_rank(pjob, rank) == false)
        {
        snprintf(log_buffer, sizeof(log_buffer),
          "Connect request from PMIx server via remote mom requested rank %d but we only have %d ranks",
          rank, pjob->ji_numvnod);
        log_err(-1, pjob->ji_qs.ji_jobid, log_buffer);
        }
      } while (*ptr == ',');
    }
  } // END constructor for remote connect moms



// Constructor used for connect operations
pmix_operation::pmix_operation(
    
  job               *pjob,
  const pmix_proc_t  procs[],
  size_t             nprocs,
  pmix_op_cbfunc_t   cbfunc,
  void              *rdata) : data(), hosts_reported(), hosts_to_report(), local_ranks(),
                              all_ranks(), to_call_fence(NULL), to_call_connect(cbfunc), op_id(0),
                              complete(false), type(CONNECT_OPERATION), cbdata(rdata)

  {
  for (size_t i = 0; i < nprocs; i++)
    {
    if (this->safe_insert_rank(pjob, procs[i].rank) == false)
      {
      // Connect is asking for a rank that shouldn't exist on this job?
      snprintf(log_buffer, sizeof(log_buffer),
        "Connect request from PMIx server requested rank %d but we only have %d ranks",
        procs[i].rank, pjob->ji_numvnod);
      log_err(-1, pjob->ji_qs.ji_jobid, log_buffer);
      }
    }

  this->op_id = get_pmix_connection_id();
  } // END constructor used for connection requests



// More generic connect/disconnect constructor
pmix_operation::pmix_operation(
    
  job               *pjob,
  const pmix_proc_t  procs[],
  size_t             nprocs,
  pmix_op_cbfunc_t   cbfunc,
  void              *rdata,
  int                op_type) : data(), hosts_reported(), hosts_to_report(), local_ranks(),
                              all_ranks(), to_call_fence(NULL), to_call_connect(cbfunc), op_id(0),
                              complete(false), type(op_type), cbdata(rdata)

  {
  for (size_t i = 0; i < nprocs; i++)
    {
    if (this->safe_insert_rank(pjob, procs[i].rank) == false)
      {
      // Connect is asking for a rank that shouldn't exist on this job?
      snprintf(log_buffer, sizeof(log_buffer),
        "Connect request from PMIx server requested rank %d but we only have %d ranks",
        procs[i].rank, pjob->ji_numvnod);
      log_err(-1, pjob->ji_qs.ji_jobid, log_buffer);
      }
    }
  } // END generic connect constructor



/*
 * mark_reported()
 *
 */

bool pmix_operation::mark_reported(
    
  const std::string &hostname)

  {
  std::set<std::string>::iterator it = this->hosts_to_report.find(hostname);
  bool done = false;

  if (it != this->hosts_to_report.end())
    {
    // Found
    this->hosts_to_report.erase(it);
    this->hosts_reported.insert(hostname);
    
    if (this->hosts_to_report.size() == 0)
      done = true;
    else if (LOGLEVEL >= 6)
      {
      snprintf(log_buffer, sizeof(log_buffer),
        "Still waiting for %lu hosts to report for %s's fence operation. %s reported in.",
        this->hosts_to_report.size(), this->jobid.c_str(), hostname.c_str());
      log_event(PBSEVENT_JOB, PBS_EVENTCLASS_REQUEST, __func__, log_buffer);
      }
    }

  return(done);
  } // END mark_reported()
  


pmix_operation::pmix_operation() : jobid(), data(), hosts_reported(), hosts_to_report(),
                                   local_ranks(), all_ranks(), to_call_fence(NULL),
                                   to_call_connect(NULL), op_id(), complete(false),
                                   type(FENCE_OPERATION)

  {
  }

pmix_operation::pmix_operation(
    
  const pmix_operation &other) : jobid(other.jobid), data(other.data), 
                                  hosts_reported(other.hosts_reported),
                                  hosts_to_report(other.hosts_to_report),
                                  local_ranks(other.local_ranks), all_ranks(other.all_ranks),
                                  to_call_fence(other.to_call_fence),
                                  to_call_connect(other.to_call_connect),
                                  op_id(other.op_id), complete(other.complete), type(other.type)

  {
  }


pmix_operation &pmix_operation::operator =(
    
  const pmix_operation &other)

  {
  this->jobid = other.jobid;
  this->data = other.data;
  this->hosts_reported = other.hosts_reported;
  this->hosts_to_report = other.hosts_to_report;
  this->to_call_fence = other.to_call_fence;
  this->to_call_connect = other.to_call_connect;
  this->local_ranks = other.local_ranks;
  this->all_ranks = other.all_ranks;
  this->to_call_connect = other.to_call_connect;
  this->op_id = other.op_id;
  this->complete = other.complete;
  this->type = other.type;
  this->cbdata = other.cbdata;
  return(*this);
  }

const std::set<std::string> &pmix_operation::get_hosts_reported() const

  {
  return(this->hosts_reported);
  }


void pmix_operation::populate_rank_string(

  std::string &ranks) const

  {
  char buf[MAXLINE];

  for (std::set<int>::iterator it = this->all_ranks.begin(); it != this->all_ranks.end(); it++)
    {
    if (ranks.size() != 0)
      sprintf(buf, ",%d", *it);
    else
      sprintf(buf, "%u:%d", this->op_id, *it);

    ranks += buf;
    }
  }



/*
 * complete_operation()
 *
 * Completes this objects pmix operation, which as of now is either fence or connect
 * For either, this includes notifying all participating moms and, for me, executing the callback
 *
 * @param pjob - the job this pmix operation pertains to
 * @param timeout - a timeout, or 0 if no timeout should be enforced
 * @return PMIX_SUCCESS if successful or PMIX_ERR_TIMEOUT if we time out
 */

pmix_status_t pmix_operation::complete_operation(
    
  job  *pjob,
  long  timeout)

  {
  std::string   ranks;
  time_t        start = time(NULL);
  pmix_status_t rc = PMIX_SUCCESS;

  if (this->type == CONNECT_OPERATION)
    this->populate_rank_string(ranks);

  for (std::set<std::string>::iterator it = this->hosts_reported.begin();
       it != this->hosts_reported.end();
       it++)
    {
    if (timeout > 0)
      {
      if (time(NULL) - start >= timeout)
        {
        rc = PMIX_ERR_TIMEOUT;

        break;
        }
      }

    if (*it == mom_alias)
      {
      continue;
      }
    else
      {
      if (this->type == FENCE_OPERATION)
        remote_notify_pmix_operation(pjob, it->c_str(), this->data, IM_FENCE, NULL);
      else if (this->type == CONNECT_OPERATION)
        remote_notify_pmix_operation(pjob, it->c_str(), ranks, IM_CONNECT, NULL);
      else
        {
        char buf[MAXLINE];
        sprintf(buf, "%d", this->op_id);
        remote_notify_pmix_operation(pjob, it->c_str(), buf, IM_DISCONNECT, NULL);
        }
      }
    }

  if (rc == PMIX_SUCCESS)
    this->execute_callback();
  
  this->complete = true;

  return(rc);
  } // END complete_operation()



/*
 * execute_callback()
 *
 * Performs the local callback for this operation
 */

void pmix_operation::execute_callback()

  {
  if (this->type == FENCE_OPERATION)
    {
    if (this->to_call_fence != NULL)
      this->to_call_fence(PMIX_SUCCESS, this->data.c_str(), this->data.size(), this->cbdata,
                          NULL, NULL);
    }
  else if (this->to_call_connect != NULL)
    {
    this->to_call_connect(PMIX_SUCCESS, this->cbdata);
    }
  }



void pmix_operation::add_data(

  const std::string &additional)

  {
  if (this->data.size() > 0)
    {
    this->data += ",";
    this->data += additional;
    }
  }



bool pmix_operation::operator ==(
    
  const pmix_operation &other) const

  {
  if (this->op_id != 0)
    return(this->op_id == other.op_id);
  else
    return(this->jobid == other.jobid);
  }

bool pmix_operation::ranks_match(

  const pmix_proc_t procs[],
  size_t            nprocs)

  {
  bool match = true;

  if (nprocs == this->all_ranks.size())
    {
    for (size_t i = 0; i < nprocs; i++)
      if (this->all_ranks.find(procs[i].rank) == this->all_ranks.end())
        {
        match = false;
        break;
        }
    }
  else
    match = false;

  return(match);
  }

unsigned int pmix_operation::get_operation_id() const
  {
  return(this->op_id);
  }

void pmix_operation::set_id(

  unsigned int id)

  {
  this->op_id = id;
  }

int pmix_operation::get_type() const
  
  {
  return(this->type);
  }

bool pmix_operation::has_local_rank(
    
  int rank) const

  {
  return(this->local_ranks.find(rank) != this->local_ranks.end());
  }
  


int pmix_operation::connection_process_terminated(

  job                *pjob,
  struct sockaddr_in *source_addr,
  bool                am_i_ms)

  {
  int rc = PBSE_NONE;

  // Terminate all local tasks
  for (std::set<int>::iterator it = this->local_ranks.begin(); it != this->local_ranks.end(); it++)
    {
    task *ptask = task_find(pjob, *it);

    if (ptask != NULL)
      {
      int task_rc = kill_task(pjob, ptask, SIGKILL, 0);
      if ((task_rc != PBSE_NONE) &&
          (rc == PBSE_NONE))
        rc = task_rc;
      }
    }

  if (am_i_ms)
    {
    char buf[MAXLINE];
    sprintf(buf, "%u", this->op_id);

    // Mother superior needs to tell all other members of this connection to terminate
    for (std::set<std::string>::iterator it = this->hosts_reported.begin();
         it != this->hosts_reported.end();
         it++)
      {
      if (*it == mom_alias)
        continue;
        
      remote_notify_pmix_operation(pjob, it->c_str(), buf, IM_CONNECT, source_addr);
      }
    }

  return(rc);
  }



/*
 * clean_up_connection()
 *
 * A process that is part of this connection has exited. Terminate the connection
 *
 * @param pjob - the relevant job
 * @param source_addr - the ip address that told us we need to clean up
 * @param op_id - the id of the operation that is this connection
 * @param am_i_ms - true if I am mother superior
 * @return PBSE_NONE on success
 */

int clean_up_connection(

  job                *pjob,
  struct sockaddr_in *source_addr,
  unsigned int        op_id,
  bool                am_i_ms)

  {
  int rc = PBSE_NONE;

  std::map<unsigned int, pmix_operation>::iterator it = existing_connections.find(op_id);

  if (it != existing_connections.end())
    {
    rc = existing_connections[op_id].connection_process_terminated(pjob, source_addr, am_i_ms);
    existing_connections.erase(it);
    }
  else
    rc = -1;

  return(rc);
  } // END clean_up_connection()


void check_and_act_on_obit(

  job *pjob,
  int  rank)

  {
  std::vector<unsigned int> connection_ids;

  for (std::map<unsigned int, pmix_operation>::iterator it = existing_connections.begin();
       it != existing_connections.end();
       it++)
    {
    if (it->second.has_local_rank(rank) == true)
      {
      connection_ids.push_back(it->second.get_operation_id());
      break;
      }
    }

  for (unsigned int i = 0; i < connection_ids.size(); i++)
    {
    clean_up_connection(pjob, NULL, connection_ids[i], am_i_mother_superior(*pjob));
    }
  } // END check_and_act_on_obit()

#endif

