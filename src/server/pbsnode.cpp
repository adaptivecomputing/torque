#include <stdio.h>

#include "pbs_nodes.h"
#include "log.h"
#include "u_tree.h"
#include "id_map.hpp"
#include "mom_hierarchy_handler.h"
#include "../lib/Libnet/lib_net.h" /* pbs_getaddrinfo */

#define MSG_LEN_LONG 160

extern AvlTree          ipaddrs;

pbsnode::pbsnode() : nd_error(0), nd_properties(),
                     nd_mutex(), nd_id(-1), nd_addrs(NULL), nd_prop(NULL), nd_status(NULL),
                     nd_note(NULL),
                     nd_stream(-1),
                     nd_flag(okay), nd_mom_port(PBS_MOM_SERVICE_PORT),
                     nd_mom_rm_port(PBS_MANAGER_SERVICE_PORT), nd_sock_addr(),
                     nd_nprops(0), nd_nstatus(0),
                     nd_slots(), nd_job_usages(), nd_needed(0), nd_np_to_be_used(0),
                     nd_state(INUSE_DOWN), nd_ntype(0), nd_order(0),
                     nd_warnbad(0),
                     nd_lastupdate(0), nd_lastHierarchySent(0), nd_hierarchy_level(0),
                     nd_in_hierarchy(0), nd_ngpus(0), nd_gpus_real(0), nd_gpusn(NULL),
                     nd_ngpus_free(0), nd_ngpus_needed(0), nd_ngpus_to_be_used(0),
                     nd_gpustatus(NULL), nd_ngpustatus(0), nd_nmics(0),
                     nd_micstatus(NULL), nd_micjobs(NULL), nd_nmics_alloced(0),
                     nd_nmics_free(0), nd_nmics_to_be_used(0), parent(NULL),
                     num_node_boards(0), node_boards(NULL), numa_str(NULL),
                     gpu_str(NULL), nd_mom_reported_down(0), nd_is_alps_reporter(0),
                     nd_is_alps_login(0), nd_ms_jobs(), alps_subnodes(NULL),
                     max_subnode_nppn(0), nd_power_state(0),
                     nd_power_state_change_time(0), nd_acl(NULL),
                     nd_requestid(), nd_tmp_unlock_count(0)

  {
  if (hierarchy_handler.isHiearchyLoaded())
    {
    this->nd_state |= INUSE_NOHIERARCHY; //This is a dynamic add so don't allow
                                          //the node to be used until an updated node
                                          //list has been send to all nodes.
    }
  
  this->nd_first           = init_prop(this->nd_name.c_str());
  this->nd_last            = this->nd_first;
  this->nd_f_st            = init_prop(this->nd_name.c_str());
  this->nd_l_st            = this->nd_f_st;

  pthread_mutex_init(&this->nd_mutex,NULL);
  } // END empty constructor



pbsnode::pbsnode(

  const char *pname,
  u_long     *pul,
  bool        skip_address_lookup) : nd_error(0), nd_properties(), nd_mutex(), nd_prop(NULL),
                                     nd_status(NULL),
                                     nd_note(NULL),
                                     nd_stream(-1),
                                     nd_flag(okay),
                                     nd_mom_port(PBS_MOM_SERVICE_PORT),
                                     nd_mom_rm_port(PBS_MANAGER_SERVICE_PORT), nd_sock_addr(),
                                     nd_nprops(0), nd_nstatus(0),
                                     nd_slots(), nd_job_usages(), nd_needed(0), nd_np_to_be_used(0),
                                     nd_state(INUSE_DOWN), nd_ntype(0), nd_order(0),
                                     nd_warnbad(0),
                                     nd_lastupdate(0), nd_lastHierarchySent(0), nd_hierarchy_level(0),
                                     nd_in_hierarchy(0), nd_ngpus(0), nd_gpus_real(0), nd_gpusn(NULL),
                                     nd_ngpus_free(0), nd_ngpus_needed(0), nd_ngpus_to_be_used(0),
                                     nd_gpustatus(NULL), nd_ngpustatus(0), nd_nmics(0),
                                     nd_micstatus(NULL), nd_micjobs(NULL), nd_nmics_alloced(0),
                                     nd_nmics_free(0), nd_nmics_to_be_used(0), parent(NULL),
                                     num_node_boards(0), node_boards(NULL), numa_str(NULL),
                                     gpu_str(NULL), nd_mom_reported_down(0), nd_is_alps_reporter(0),
                                     nd_is_alps_login(0), nd_ms_jobs(), alps_subnodes(NULL),
                                     max_subnode_nppn(0), nd_power_state(0),
                                     nd_power_state_change_time(0), nd_acl(NULL),
                                     nd_requestid(), nd_tmp_unlock_count(0)

  {
  struct addrinfo *pAddrInfo;

  this->nd_name            = pname;
  this->nd_properties.push_back(this->nd_name);
  this->nd_id              = node_mapper.get_new_id(this->nd_name.c_str());
  this->nd_addrs           = pul;       /* list of host byte order */
  //this->nd_ntype           = ntype;
  this->nd_first           = init_prop(this->nd_name.c_str());
  this->nd_last            = this->nd_first;
  this->nd_f_st            = init_prop(this->nd_name.c_str());
  this->nd_l_st            = this->nd_f_st;

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

  pthread_mutex_init(&this->nd_mutex,NULL);
  } // END constructor



pbsnode::~pbsnode()

  {
  if (this->nd_first != NULL)
    free_prop_list(this->nd_first);

  if (this->nd_f_st != NULL)
    free_prop_list(this->nd_f_st);

  if (this->nd_addrs != NULL)
    {
    for (u_long *up = this->nd_addrs; *up != 0; up++)
      {
      /* del node's IP addresses from tree  */
      ipaddrs = AVL_delete_node(*up, this->nd_mom_port, ipaddrs);
      }

    if (this->nd_addrs != NULL)
      {
      /* remove array of IP addresses */
      free(this->nd_addrs);
      }
    }

  if (this->alps_subnodes != NULL)
    delete this->alps_subnodes;

  if (this->nd_acl != NULL)
    {
    if (this->nd_acl->as_buf != NULL)
      {
      free(this->nd_acl->as_buf);
      }

    free(this->nd_acl);
    }

  } // END destructor



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

  struct prop *props) const

  {
  struct prop *need;

  for (need = props; need != NULL; need = need->next)
    {
    if (need->mark == 0) /* not marked, skip */
      continue;

    bool found = false;

    for (unsigned int i = 0; i < this->nd_properties.size(); i++)
      {
      if (!strcmp(this->nd_properties[i].c_str(), need->name))
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
  }



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
    log_err(PBSE_BAD_PARAMETER, __func__, "NULL destanation pointer input");
    return(PBSE_BAD_PARAMETER);
    }

  /* copy features/properties */
  for (unsigned int i = 0; i < this->nd_properties.size(); i++)
    dest->nd_properties.push_back(this->nd_properties[i]);

  return(PBSE_NONE);
  } /* END copy_properties() */



