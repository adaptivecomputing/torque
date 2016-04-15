
#include <pbs_config.h>

#ifdef ENABLE_PMIX
#ifndef PMIX_OPERATION_HPP
#define PMIX_OPERATION_HPP

#include <string>
#include <list>
#include <set>
#include <netinet/in.h>

#include <pmix_server.h>

#include "pbs_job.h"

extern const int    FENCE_OPERATION;
extern const int    CONNECT_OPERATION;
extern const int    DISCONNECT_OPERATION;
extern unsigned int pmix_connect_id;


class pmix_operation
  {
  std::string            jobid;
  std::string            data; // fence only
  std::set<std::string>  hosts_reported;
  std::set<std::string>  hosts_to_report;
  std::set<int>          local_ranks; // connect only
  std::set<int>          all_ranks; // connect only
  pmix_modex_cbfunc_t    to_call_fence;
  pmix_op_cbfunc_t       to_call_connect;
  unsigned int           op_id; // connect only
  bool                   complete;
  int                    type;
  void                  *cbdata;

  public:

  pmix_operation(job *pjob, const pmix_proc_t procs[], size_t nprocs, const std::string &data,
    pmix_modex_cbfunc_t cbfunc, void *cbdata); // used for fence
  pmix_operation();
  pmix_operation(job *pjob, const pmix_proc_t procs[], size_t nprocs, pmix_op_cbfunc_t to_call,
                 void *cbdata); // used for connect
  pmix_operation(char *data, job *pjob); // used for connect
  pmix_operation(job *pjob, const pmix_proc_t procs[], size_t nprocs, pmix_op_cbfunc_t to_call,
                 void *cbdata, int operation_type);
  pmix_operation(const pmix_operation &other);

  pmix_operation              &operator =(const pmix_operation &other);
  bool                         operator ==(const pmix_operation &other) const;
  bool                         mark_reported(const std::string &hostname);
  const std::set<std::string> &get_hosts_reported() const;
  bool                         safe_insert_rank(job *pjob, int rank);
  void                         add_data(const std::string &additional);
  void                         execute_callback();
  void                         notify_of_disconnect();
  void                         notify_of_termination();
  pmix_status_t                complete_operation(job *pjob, long timeout);
  unsigned int                 get_operation_id() const;
  int                          get_type() const;
  bool                         ranks_match(const pmix_proc_t procs[], size_t nprocs);
  bool                         has_local_rank(int rank) const;
  void                         populate_rank_string(std::string &ranks) const;
  void                         set_id(unsigned int id);
  int                          connection_process_terminated(job *pjob, 
                                                             struct sockaddr_in *source_addr,
                                                             bool am_i_ms);
  };

// jobid, fence operation object
extern std::map<std::string, pmix_operation> pending_fences;

// operation id, connection operation object
extern std::map<unsigned int, pmix_operation> existing_connections;

int  matches_existing_connection(const pmix_proc_t procs[], size_t nprocs);
int  clean_up_connection(job *pjob, struct sockaddr_in *source_addr, unsigned int op_id, bool ms);
void check_and_act_on_obit(job *pjob, int rank);

#endif
#endif

