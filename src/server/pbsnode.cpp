#include <pbs_config.h>
#include <stdio.h>

#include "pbs_nodes.h"
#include "log.h"
#include "u_tree.h"
#include "id_map.hpp"
#include "mom_hierarchy_handler.h"
#include "../lib/Libnet/lib_net.h" /* pbs_getaddrinfo */
#include "alps_constants.h"
#include "policy_values.h"

#define MSG_LEN_LONG 160

void add_to_property_list(std::string &, const char *);
int record_node_property_list(std::string const &, tlist_head *);

extern AvlTree          ipaddrs;

pbsnode::pbsnode() : nd_error(0), nd_properties(), nd_version(0), nd_proximal_failures(0),
                     nd_consecutive_successes(0),
                     nd_mutex(), nd_id(-1), nd_f_st(), nd_addrs(), nd_prop(NULL), nd_status(NULL),
                     nd_note(),
                     nd_stream(-1),
                     nd_flag(okay), nd_mom_port(PBS_MOM_SERVICE_PORT),
                     nd_mom_rm_port(PBS_MANAGER_SERVICE_PORT), nd_sock_addr(),
                     nd_nprops(0), nd_nstatus(0),
                     nd_slots(), nd_job_usages(), nd_needed(0), nd_np_to_be_used(0),
                     nd_state(INUSE_DOWN), nd_ntype(0), nd_order(0),
                     nd_warnbad(0),
                     nd_lastupdate(0), nd_lastHierarchySent(0), nd_hierarchy_level(0),
                     nd_in_hierarchy(0), nd_ngpus(0), nd_gpus_real(0), nd_gpusn(),
                     nd_ngpus_free(0), nd_ngpus_needed(0), nd_ngpus_to_be_used(0),
                     nd_gpustatus(NULL), nd_ngpustatus(0), nd_nmics(0),
                     nd_micstatus(NULL), nd_micjobids(), nd_nmics_alloced(0),
                     nd_nmics_free(0), nd_nmics_to_be_used(0), parent(NULL),
                     num_node_boards(0), node_boards(NULL), numa_str(),
                     gpu_str(), nd_mom_reported_down(0), nd_is_alps_reporter(0),
                     nd_is_alps_login(0), nd_ms_jobs(NULL), alps_subnodes(NULL),
                     max_subnode_nppn(0), nd_power_state(0),
                     nd_power_state_change_time(0), nd_acl(NULL),
                     nd_requestid(), nd_tmp_unlock_count(0)
#ifdef PENABLE_LINUX_CGROUPS
                    , nd_layout()
#endif
  {
  if (hierarchy_handler.isHiearchyLoaded())
    {
    this->nd_state |= INUSE_NOHIERARCHY; //This is a dynamic add so don't allow
                                          //the node to be used until an updated node
                                          //list has been sent to all nodes.
    }
  
  memset(this->nd_ttl, 0, sizeof(this->nd_ttl));
  memset(this->nd_mac_addr, 0, sizeof(this->nd_mac_addr));

  pthread_mutex_init(&this->nd_mutex,NULL);
  } // END empty constructor



pbsnode::pbsnode(

  const char *pname,
  u_long     *pul,
  bool        skip_address_lookup) : nd_error(0), nd_properties(), nd_version(0),
                                     nd_proximal_failures(0), nd_consecutive_successes(0),
                                     nd_mutex(), nd_f_st(), nd_prop(NULL), nd_status(NULL),
                                     nd_note(),
                                     nd_stream(-1),
                                     nd_flag(okay),
                                     nd_mom_port(PBS_MOM_SERVICE_PORT),
                                     nd_mom_rm_port(PBS_MANAGER_SERVICE_PORT), nd_sock_addr(),
                                     nd_nprops(0), nd_nstatus(0),
                                     nd_slots(), nd_job_usages(), nd_needed(0), nd_np_to_be_used(0),
                                     nd_state(INUSE_DOWN), nd_ntype(0), nd_order(0),
                                     nd_warnbad(0),
                                     nd_lastupdate(0), nd_lastHierarchySent(0), nd_hierarchy_level(0),
                                     nd_in_hierarchy(0), nd_ngpus(0), nd_gpus_real(0), nd_gpusn(),
                                     nd_ngpus_free(0), nd_ngpus_needed(0), nd_ngpus_to_be_used(0),
                                     nd_gpustatus(NULL), nd_ngpustatus(0), nd_nmics(0),
                                     nd_micstatus(NULL), nd_micjobids(), nd_nmics_alloced(0),
                                     nd_nmics_free(0), nd_nmics_to_be_used(0), parent(NULL),
                                     num_node_boards(0), node_boards(NULL), numa_str(),
                                     gpu_str(), nd_mom_reported_down(0), nd_is_alps_reporter(0),
                                     nd_is_alps_login(0), nd_ms_jobs(NULL), alps_subnodes(NULL),
                                     max_subnode_nppn(0), nd_power_state(0),
                                     nd_power_state_change_time(0), nd_acl(NULL),
                                     nd_requestid(), nd_tmp_unlock_count(0)
#ifdef PENABLE_LINUX_CGROUPS
                                     , nd_layout()
#endif

  {
  struct addrinfo *pAddrInfo;

  this->nd_name            = pname;
  this->nd_properties.push_back(this->nd_name);
  this->nd_id              = node_mapper.get_new_id(this->nd_name.c_str());

  if (pul != NULL)
    {
    for (u_long *up = pul; *up != 0; up++)
      this->nd_addrs.push_back(*up);
    }

  //this->nd_ntype           = ntype;

  if (hierarchy_handler.isHiearchyLoaded())
    {
    this->nd_state |= INUSE_NOHIERARCHY; //This is a dynamic add so don't allow
                                          //the node to be used until an updated node
                                          //list has been send to all nodes.
    }

  if (!skip_address_lookup)
    {
    if (pbs_getaddrinfo(pname,NULL,&pAddrInfo))
      this->nd_error = PBSE_SYSTEM;
    else
      memcpy(&this->nd_sock_addr,pAddrInfo->ai_addr,sizeof(struct sockaddr_in));
    }

  memset(this->nd_ttl, 0, sizeof(this->nd_ttl));
  memset(this->nd_mac_addr, 0, sizeof(this->nd_mac_addr));

  pthread_mutex_init(&this->nd_mutex,NULL);
  } // END constructor



pbsnode::~pbsnode()

  {
  free_arst_value(this->nd_acl);
  free_arst_value(this->nd_prop);
  free_arst_value(this->nd_status);
  free_arst_value(this->nd_gpustatus);
  free_arst_value(this->nd_micstatus);
  } // END destructor



void pbsnode::copy_gpu_subnodes(

  const pbsnode &other)

  {
  for (unsigned int i = 0; i < other.nd_gpusn.size(); i++)
    this->nd_gpusn.push_back(other.nd_gpusn[i]);
  } // END copy_gpu_subnodes()



/*
 * = operator()
 */

pbsnode &pbsnode::operator =(

  const pbsnode &other)

  {
  this->nd_error = other.nd_error;
  this->nd_id = other.nd_id;
  this->nd_f_st = other.nd_f_st;
  this->nd_properties = other.nd_properties;
  this->nd_version = other.nd_version;
  this->nd_proximal_failures = other.nd_proximal_failures;
  this->nd_consecutive_successes = other.nd_consecutive_successes;
  
  free_arst_value(this->nd_prop);
  this->nd_prop = copy_arst(other.nd_prop);

  free_arst_value(this->nd_status);
  this->nd_status = copy_arst(other.nd_status);

  this->nd_note = other.nd_note;
  this->nd_addrs = other.nd_addrs;

  this->nd_flag = other.nd_flag;
  this->nd_mom_port = other.nd_mom_port;
  this->nd_mom_rm_port = other.nd_mom_rm_port;
  memcpy(&this->nd_sock_addr, &other.nd_sock_addr, sizeof(this->nd_sock_addr));
  this->nd_nprops = other.nd_nprops;
  this->nd_nstatus = other.nd_nstatus;
  this->nd_slots = other.nd_slots;
  this->nd_job_usages = other.nd_job_usages;
  this->nd_needed = other.nd_needed;
  this->nd_np_to_be_used = other.nd_np_to_be_used;
  this->nd_state = other.nd_state;
  this->nd_ntype = other.nd_ntype;
  this->nd_order = other.nd_order;
  this->nd_warnbad = other.nd_warnbad;
  this->nd_lastupdate = other.nd_lastupdate;
  this->nd_lastHierarchySent = other.nd_lastHierarchySent;
  this->nd_hierarchy_level = other.nd_hierarchy_level;
  this->nd_in_hierarchy = other.nd_in_hierarchy;
  this->nd_ngpus = other.nd_ngpus;
  this->nd_gpus_real = other.nd_gpus_real;
  this->nd_gpusn.clear();
  this->copy_gpu_subnodes(other);
  this->nd_ngpus_free = other.nd_ngpus_free;
  this->nd_ngpus_needed = other.nd_ngpus_needed;
  this->nd_ngpus_to_be_used = other.nd_ngpus_to_be_used;

  free_arst_value(this->nd_gpustatus);
  this->nd_gpustatus = copy_arst(other.nd_gpustatus);

  this->nd_ngpustatus = other.nd_ngpustatus;
  this->nd_nmics = other.nd_nmics;

  free_arst_value(this->nd_micstatus);
  this->nd_micstatus = copy_arst(other.nd_micstatus);

  for (unsigned int i = 0; i < other.nd_micjobids.size(); i++)
    this->nd_micjobids.push_back(other.nd_micjobids[i]);

  this->nd_nmics_alloced = other.nd_nmics_alloced;
  this->nd_nmics_free = other.nd_nmics_free;
  this->nd_nmics_to_be_used = other.nd_nmics_to_be_used;
  this->parent = other.parent;
  this->num_node_boards = other.num_node_boards;
  this->node_boards = other.node_boards;
  this->numa_str = other.numa_str;
  this->gpu_str = other.gpu_str;
  this->nd_mom_reported_down = other.nd_mom_reported_down;
  this->nd_is_alps_reporter = other.nd_is_alps_reporter;
  this->nd_is_alps_login = other.nd_is_alps_login;
  this->nd_ms_jobs = other.nd_ms_jobs;
  this->alps_subnodes = other.alps_subnodes;
  this->max_subnode_nppn = other.max_subnode_nppn;
  this->nd_power_state = other.nd_power_state;
  this->nd_power_state_change_time = other.nd_power_state_change_time;

  free_arst_value(this->nd_acl);
  this->nd_acl = copy_arst(other.nd_acl);

  this->nd_requestid = other.nd_requestid;
  this->nd_tmp_unlock_count = other.nd_tmp_unlock_count;
#ifdef PENABLE_LINUX_CGROUPS
  this->nd_layout = other.nd_layout;
#endif

  memcpy(this->nd_ttl, other.nd_ttl, sizeof(this->nd_ttl));
  memcpy(this->nd_mac_addr, other.nd_mac_addr, sizeof(this->nd_mac_addr));

  return(*this);
  } // END = operator()



pbsnode::pbsnode(

  const pbsnode &other) : nd_error(other.nd_error), nd_properties(other.nd_properties),
                          nd_version(other.nd_version),
                          nd_proximal_failures(other.nd_proximal_failures),
                          nd_consecutive_successes(other.nd_consecutive_successes), nd_mutex(),
                          nd_id(other.nd_id), nd_addrs(other.nd_addrs),
                          nd_note(other.nd_note), nd_stream(other.nd_stream),
                          nd_flag(other.nd_flag), nd_mom_port(other.nd_mom_port),
                          nd_mom_rm_port(other.nd_mom_rm_port), nd_sock_addr(), nd_nprops(0),
                          nd_nstatus(other.nd_nstatus), nd_slots(other.nd_slots),
                          nd_job_usages(other.nd_job_usages),
                          nd_needed(other.nd_needed), nd_np_to_be_used(other.nd_np_to_be_used),
                          nd_state(other.nd_state), nd_ntype(other.nd_ntype),
                          nd_order(other.nd_order), nd_warnbad(other.nd_warnbad),
                          nd_lastupdate(other.nd_lastupdate),
                          nd_lastHierarchySent(other.nd_lastHierarchySent),
                          nd_hierarchy_level(other.nd_hierarchy_level),
                          nd_in_hierarchy(other.nd_in_hierarchy), nd_ngpus(other.nd_ngpus),
                          nd_gpus_real(other.nd_gpus_real), nd_gpusn(),
                          nd_ngpus_free(other.nd_ngpus_free),
                          nd_ngpus_needed(other.nd_ngpus_needed),
                          nd_ngpus_to_be_used(other.nd_ngpus_to_be_used),
                          nd_ngpustatus(other.nd_ngpustatus),
                          nd_nmics(other.nd_nmics),
                          nd_micjobids(other.nd_micjobids),
                          nd_nmics_alloced(other.nd_nmics_alloced),
                          nd_nmics_free(other.nd_nmics_free),
                          nd_nmics_to_be_used(other.nd_nmics_to_be_used), parent(other.parent),
                          num_node_boards(other.num_node_boards), numa_str(other.numa_str),
                          gpu_str(other.gpu_str), nd_mom_reported_down(other.nd_mom_reported_down),
                          nd_is_alps_reporter(other.nd_is_alps_reporter),
                          nd_is_alps_login(other.nd_is_alps_login), nd_ms_jobs(other.nd_ms_jobs),
                          max_subnode_nppn(other.max_subnode_nppn),
                          nd_power_state(other.nd_power_state),
                          nd_power_state_change_time(other.nd_power_state_change_time),
                          nd_requestid(other.nd_requestid),
                          nd_tmp_unlock_count(other.nd_tmp_unlock_count)
#ifdef PENABLE_LINUX_CGROUPS
                          , nd_layout(other.nd_layout)
#endif

  {
  this->nd_prop = copy_arst(other.nd_prop);
  this->nd_status = copy_arst(other.nd_status);
  this->nd_gpustatus = copy_arst(other.nd_gpustatus);
  this->nd_micstatus = copy_arst(other.nd_micstatus);

  this->node_boards = other.node_boards;
  this->alps_subnodes = other.alps_subnodes;

  this->nd_acl = copy_arst(other.nd_acl);

  memcpy(&this->nd_sock_addr, &other.nd_sock_addr, sizeof(this->nd_sock_addr));
  memcpy(this->nd_ttl, other.nd_ttl, sizeof(this->nd_ttl));
  memcpy(this->nd_mac_addr, other.nd_mac_addr, sizeof(this->nd_mac_addr));
  this->copy_gpu_subnodes(other);
  }



/*
 * lock_node()
 *
 * Locks this node
 */

int pbsnode::lock_node(
    
  const char     *id,
  const char     *msg,
  int             logging)

  {
  int   rc = PBSE_NONE;
  char  err_msg[MSG_LEN_LONG + 1];
  char  stub_msg[] = "no pos";
  
  if (logging >= 10)
    {
    if (msg == NULL)
      msg = stub_msg;
    snprintf(err_msg, MSG_LEN_LONG, "locking start %s in method %s-%s", this->nd_name.c_str(), id, msg);
    log_record(PBSEVENT_DEBUG, PBS_EVENTCLASS_NODE, __func__, err_msg);
    }
  
  if (pthread_mutex_lock(&this->nd_mutex) != 0)
    {
    if (logging >= 10)
      {
      snprintf(err_msg, MSG_LEN_LONG, "ALERT: cannot lock node %s mutex in method %s",
          this->nd_name.c_str(), id);
      log_record(PBSEVENT_DEBUG, PBS_EVENTCLASS_NODE, __func__, err_msg);
      }
    rc = PBSE_MUTEX;
    }
  
  if (logging >= 10)
    {
    snprintf(err_msg, MSG_LEN_LONG, "locking complete %s in method %s", this->nd_name.c_str(), id);
    log_record(PBSEVENT_DEBUG, PBS_EVENTCLASS_NODE, __func__, err_msg);
    }

  return(rc);
  } /* END lock_node() */



int pbsnode::tmp_lock_node(

  const char     *id,
  const char     *msg,
  int             logging)

  {
  int rc = this->lock_node(id, msg, logging);
  if (rc == PBSE_NONE)
    this->nd_tmp_unlock_count--;

  return(rc);
  }



int pbsnode::unlock_node(
    
  const char     *id,
  const char     *msg,
  int             logging)

  {
  int   rc = PBSE_NONE;
  char  err_msg[MSG_LEN_LONG + 1];
  char  stub_msg[] = "no pos";

  if (logging >= 10)
    {
    if (msg == NULL)
      msg = stub_msg;
    snprintf(err_msg, MSG_LEN_LONG, "unlocking %s in method %s-%s", this->nd_name.c_str(), id, msg);
    log_record(PBSEVENT_DEBUG, PBS_EVENTCLASS_NODE, __func__, err_msg);
    }

  if (pthread_mutex_unlock(&this->nd_mutex) != 0)
    {
    if (logging >= 10)
      {
      snprintf(err_msg, MSG_LEN_LONG, "ALERT: cannot unlock node %s mutex in method %s",
          this->nd_name.c_str(), id);
      log_record(PBSEVENT_DEBUG, PBS_EVENTCLASS_NODE, __func__, err_msg);
      }
    rc = PBSE_MUTEX;
    }

  return rc;
  } /* END unlock_node() */



int pbsnode::tmp_unlock_node(

  const char     *id,
  const char     *msg,
  int             logging)

  {
  this->nd_tmp_unlock_count++;
  return(this->unlock_node(id, msg, logging));
  }



const char *pbsnode::get_name() const
  
  {
  return(this->nd_name.c_str());
  }



int pbsnode::get_error() const

  {
  return(this->nd_error);
  }



bool pbsnode::hasprop(

  std::vector<prop> *props) const

  {
  if (props == NULL)
    return(true);

  for (unsigned int i = 0; i < props->size(); i++)
    {
    prop &need = props->at(i);
    if (need.mark == 0) /* not marked, skip */
      continue;

    bool found = false;

    for (unsigned int i = 0; i < this->nd_properties.size(); i++)
      {
      if (this->nd_properties[i] == need.name)
        {
        found = true;
        break;
        }
      }

    if (found == false)
      return(found);
    }

  return(true);
  }  /* END hasprop() */


int pbsnode::encode_properties(tlist_head *phead)

  {
  std::string prop_str = "";

  if (phead == NULL)
    {
    log_err(PBSE_BAD_PARAMETER, __func__, "NULL input tlist_head pointer");
    return(PBSE_BAD_PARAMETER);
    }

  // build property string
  for (unsigned int i = 0; i < this->nd_properties.size(); i++)
    {
    // only copy properties not matching the node name
    if (this->nd_properties[i] != this->nd_name)
      add_to_property_list(prop_str, this->nd_properties[i].c_str());
    }

  // add it to the list
  return(record_node_property_list(prop_str, phead));
  } /* END encode_properties() */


void pbsnode::update_properties()

  {
  this->nd_properties.clear();

  if (this->nd_prop)
    {
    int nprops = this->nd_prop->as_usedptr;

    for (int i = 0; i < nprops; i++)
      {
      this->nd_properties.push_back(this->nd_prop->as_string[i]);
      }
    }

  /* now add in name as last prop */
  this->nd_properties.push_back(this->nd_name);
  } // END update_prop_list()


void pbsnode::change_name(

  const char *name)

  {
  // Overwrite the old property
  for (unsigned int i = 0; i < this->nd_properties.size(); i++)
    {
    if (this->nd_name == this->nd_properties[i])
      this->nd_properties[i] = name;
    }

  this->nd_name = name;
  } // END change_name()



void pbsnode::add_property(

  const std::string &prop)

  {
  this->nd_properties.push_back(prop);
  }


/*
 * write_compute_node_properties()
 *
 * writes out any extra properties or features that have been added to compute
 * nodes to the nodes file.
 * They are written in the format <node_name> cray_compute feature1[ feature2[...]]
 * 
 * @pre-cond: nin must be an open file pointer
 * @post-cond: all compute nodes with extra features have been written to nin
 *
 */

void pbsnode::write_compute_node_properties(

  FILE *nin) const

  {
  struct pbsnode    *alps_node;
  all_nodes_iterator *iter = NULL;

  while ((alps_node = next_host(this->alps_subnodes, &iter, NULL)) != NULL)
    {
    /* only write nodes that have more than just cray_compute as their properties.
     * Checking for > 2 properties should be sufficient -- all computes have 
     * cray_compute and all nodes have their name as a property */
    if (alps_node->nd_properties.size() > 2)
      {
      std::string line(alps_node->nd_name);

      for (unsigned int i = 0; i < alps_node->nd_properties.size(); i++)
        {
        if (strcmp(alps_node->get_name(), alps_node->nd_properties[i].c_str()))
          line += " " + alps_node->nd_properties[i];
        }

      fprintf(nin, "%s\n", line.c_str());
      }
      
    alps_node->unlock_node(__func__, "loop", LOGLEVEL);
    }
   
  if (iter != NULL)
    delete iter;
  } /* END write_compute_node_properties() */



void pbsnode::write_to_nodes_file(
    
  FILE *nin) const

  {
  /* ... write its name, and if time-shared, append :ts */
  fprintf(nin, "%s", this->nd_name.c_str()); /* write name */

  /* if number of subnodes is gt 1, write that; if only one,   */
  /* don't write to maintain compatability with old style file */
  if (this->nd_slots.get_total_execution_slots() > 1)
    fprintf(nin, " %s=%d", ATTR_NODE_np, this->nd_slots.get_total_execution_slots());

  /* if number of gpus is gt 0, write that; if none,   */
  /* don't write to maintain compatability with old style file */
  if (this->nd_ngpus > 0)
    fprintf(nin, " %s=%d", ATTR_NODE_gpus, this->nd_ngpus);

  /* write out the numa attributes if needed */
  if (this->num_node_boards > 0)
    {
    fprintf(nin, " %s=%d",
      ATTR_NODE_num_node_boards,
      this->num_node_boards);
    }

  if (this->numa_str.size() != 0)
    fprintf(nin, " %s=%s", ATTR_NODE_numa_str, this->numa_str.c_str());

  /* write out the ports if needed */
  if (this->nd_mom_port != PBS_MOM_SERVICE_PORT)
    fprintf(nin, " %s=%d", ATTR_NODE_mom_port, this->nd_mom_port);

  if (this->nd_mom_rm_port != PBS_MANAGER_SERVICE_PORT)
    fprintf(nin, " %s=%d", ATTR_NODE_mom_rm_port, this->nd_mom_rm_port);

  if (this->gpu_str.size() != 0)
    fprintf(nin, " %s=%s", ATTR_NODE_gpus_str, this->gpu_str.c_str());

  if(this->nd_ttl[0] != '\0')
    fprintf(nin, " %s=%s", ATTR_NODE_ttl, this->nd_ttl);

  if ((this->nd_acl != NULL) &&
      (this->nd_acl->as_usedptr != 0))
    {
    fprintf(nin, " %s=",ATTR_NODE_acl);
    for (int j = 0; j < this->nd_acl->as_usedptr; j++)
      {
      fprintf(nin, "%s", this->nd_acl->as_string[j]);
      if ((j + 1) != this->nd_acl->as_usedptr)
        fprintf(nin, ",");
      }
    }

  if (this->nd_requestid.length() != 0)
    fprintf(nin, " %s=%s", ATTR_NODE_requestid, this->nd_requestid.c_str());

  /* write out properties */
  for (int j = 0; j < this->nd_properties.size() - 1; j++)
    {
    /* Don't write out the cray_enabled features here */
    if ((this->nd_properties[j] != "cray_compute") &&
        (this->nd_properties[j] != alps_reporter_feature) &&
        (this->nd_properties[j] != alps_starter_feature))
      fprintf(nin, " %s", this->nd_properties[j].c_str());
    }

  if (this->nd_is_alps_reporter == TRUE)
    fprintf(nin, " %s", alps_reporter_feature);

  if (this->nd_is_alps_login == TRUE)
    fprintf(nin, " %s", alps_starter_feature);

  /* finish off line with new-line */
  fprintf(nin, "\n");

  if ((cray_enabled == true) &&
      (this == alps_reporter))
    this->write_compute_node_properties(nin);

  } // END write_to_nodes_file()



/*
 * copy the properties of node src to node dest
 *
 * @param dest (O) - the node where the properties will be copied to
 */

int pbsnode::copy_properties(

  pbsnode *dest) const

  {
  if (dest == NULL)
    {
    log_err(PBSE_BAD_PARAMETER, __func__, "NULL destination pointer input");
    return(PBSE_BAD_PARAMETER);
    }

  /* copy features/properties */
  for (unsigned int i = 0; i < this->nd_properties.size(); i++)
    {
    // only copy properties not matching the node name
    if (this->nd_properties[i] != this->nd_name)
      dest->nd_properties.push_back(this->nd_properties[i]);
    }

  return(PBSE_NONE);
  } /* END copy_properties() */



/*
 * update_internal_failure_counts()
 *
 * @param rc - the last network interaction return code
 */

bool pbsnode::update_internal_failure_counts(

  int rc)

  {
  bool held = false;
  char log_buf[LOCAL_LOG_BUF_SIZE];

  if (rc == PBSE_NONE)
    {
    this->nd_consecutive_successes++;

    if (this->nd_consecutive_successes > 1)
      {
      this->nd_proximal_failures = 0;

      if (this->nd_state & INUSE_NETWORK_FAIL)
        {
        snprintf(log_buf, sizeof(log_buf),
          "Node '%s' has had two or more consecutive network successes, marking online.",
          this->nd_name.c_str());
        log_record(PBSEVENT_SYSTEM, PBS_EVENTCLASS_NODE, __func__, log_buf);
        this->remove_node_state_flag(INUSE_NETWORK_FAIL);
        }
      }
    }
  else
    {
    this->nd_proximal_failures++;
    this->nd_consecutive_successes = 0;

    if ((this->nd_proximal_failures > 2) &&
        ((this->nd_state & INUSE_NETWORK_FAIL) == 0))
      {
      snprintf(log_buf, sizeof(log_buf),
        "Node '%s' has had %d failures in close proximity, marking offline.",
        this->nd_name.c_str(), this->nd_proximal_failures);
      log_record(PBSEVENT_SYSTEM, PBS_EVENTCLASS_NODE, __func__, log_buf);

      update_node_state(this, INUSE_NETWORK_FAIL);
      held = true;
      }
    }

  return(held);
  } // END update_internal_failure_counts()



/*
 * Removes a specific flag from the node state
 */

void pbsnode::remove_node_state_flag(
    
  int             flag)

  {
  this->nd_state &= ~flag;
  } // END remove_node_state_flag()

int pbsnode::get_version() const
  {
  return(this->nd_version);
  }



/*
 * set_version()
 *
 * Sets this node's version from a string in the format version=\d.\d.\d[.hotfixinformation]
 * The hotfix information, if any, is ignored
 */

void pbsnode::set_version(

  const char *version_str)

  {
  if (version_str != NULL)
    {
    char *work = strdup(version_str);
    char *ptr;
    int   version = strtol(work, &ptr, 10) * 100;

    if (*ptr == '.')
      {
      ptr++;
      version += strtol(ptr, &ptr, 10) * 10;

      if (*ptr == '.')
        {
        ptr++;
        version += strtol(ptr, &ptr, 10);

        // Only set the version if we had the correct format
        this->nd_version = version;
        }
      }
    else if (!strcmp(version_str, "master"))
      {
      this->nd_version = 1000;
      }

    free(work);
    }
  }



