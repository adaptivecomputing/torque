#ifndef RUNJOB_HELP_HPP
#define RUNJOB_HELP_HPP

class alps_req_data
  {
  public:
  std::string node_list;
  int         ppn;

  alps_req_data() : node_list(), ppn(1) {}
  };

class prop
  {
  public:
  std::string name;
  short       mark;

  prop() : name(), mark(0) {}
  prop(const std::string &n) : name(n), mark(1) {}
  };
 
class single_spec_data
  {
  public:
  int               nodes;   /* nodes needed for this req */
  int               ppn;     /* ppn for this req */
  int               gpu;     /* gpus for this req */
  int               mic;   /* mics for this req */
  int               req_id;  /* the id of this alps req - used only for cray */
  std::vector<prop> plist; /* node properties needed */

  single_spec_data() : nodes(1), ppn(1), gpu(0), mic(0), req_id(0), plist() {}
  };

class complete_spec_data
  {
  public:
  std::vector<single_spec_data>   reqs; /* array of data for each req */
  int                             num_reqs;    /* number of reqs (number of '+' in spec + 1) */
  int                             total_nodes; /* number of nodes for all reqs in a spec */
  /* pointer to start of req in spec, only valid until call of parse_req_data() */
  char                          **req_start;

  complete_spec_data() : reqs(), num_reqs(0), total_nodes(0), req_start(NULL) {}
  ~complete_spec_data()
    {
    if (req_start != NULL)
      free(req_start);
    }
  };

class node_job_add_info
  {
  public:
  int                       node_id;
  int                       ppn_needed;
  int                       gpu_needed;
  int                       mic_needed;
  int                       is_external;
  int                       req_rank;

  node_job_add_info() : node_id(-1), ppn_needed(0), gpu_needed(0),
                        mic_needed(0), is_external(0), req_rank(0) {}

  };


#endif
