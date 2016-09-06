/*
*         OpenPBS (Portable Batch System) v2.3 Software License
*
* Copyright (c) 1999-2000 Veridian Information Solutions, Inc.
* All rights reserved.
*
* ---------------------------------------------------------------------------
* For a license to use or redistribute the OpenPBS software under conditions
* other than those described below, or to purchase support for this software,
* please contact Veridian Systems, PBS Products Department ("Licensor") at:
*
*    www.OpenPBS.org  +1 650 967-4675                  sales@OpenPBS.org
*                        877 902-4PBS (US toll-free)
* ---------------------------------------------------------------------------
*
* This license covers use of the OpenPBS v2.3 software (the "Software") at
* your site or location, and, for certain users, redistribution of the
* Software to other sites and locations.  Use and redistribution of
* OpenPBS v2.3 in source and binary forms, with or without modification,
* are permitted provided that all of the following conditions are met.
* After December 31, 2001, only conditions 3-6 must be met:
*
* 1. Commercial and/or non-commercial use of the Software is permitted
*    provided a current software registration is on file at www.OpenPBS.org.
*    If use of this software contributes to a publication, product, or
*    service, proper attribution must be given; see www.OpenPBS.org/credit.html
*
* 2. Redistribution in any form is only permitted for non-commercial,
*    non-profit purposes.  There can be no charge for the Software or any
*    software incorporating the Software.  Further, there can be no
*    expectation of revenue generated as a consequence of redistributing
*    the Software.
*
* 3. Any Redistribution of source code must retain the above copyright notice
*    and the acknowledgment contained in paragraph 6, this list of conditions
*    and the disclaimer contained in paragraph 7.
*
* 4. Any Redistribution in binary form must reproduce the above copyright
*    notice and the acknowledgment contained in paragraph 6, this list of
*    conditions and the disclaimer contained in paragraph 7 in the
*    documentation and/or other materials provided with the distribution.
*
* 5. Redistributions in any form must be accompanied by information on how to
*    obtain complete source code for the OpenPBS software and any
*    modifications and/or additions to the OpenPBS software.  The source code
*    must either be included in the distribution or be available for no more
*    than the cost of distribution plus a nominal fee, and all modifications
*    and additions to the Software must be freely redistributable by any party
*    (including Licensor) without restriction.
*
* 6. All advertising materials mentioning features or use of the Software must
*    display the following acknowledgment:
*
*     "This product includes software developed by NASA Ames Research Center,
*     Lawrence Livermore National Laboratory, and Veridian Information
*     Solutions, Inc.
*     Visit www.OpenPBS.org for OpenPBS software support,
*     products, and information."
*
* 7. DISCLAIMER OF WARRANTY
*
* THIS SOFTWARE IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND. ANY EXPRESS
* OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
* OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, AND NON-INFRINGEMENT
* ARE EXPRESSLY DISCLAIMED.
*
* IN NO EVENT SHALL VERIDIAN CORPORATION, ITS AFFILIATED COMPANIES, OR THE
* U.S. GOVERNMENT OR ANY OF ITS AGENCIES BE LIABLE FOR ANY DIRECT OR INDIRECT,
* INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
* LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,
* OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
* LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
* NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
* EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*
* This license will be governed by the laws of the Commonwealth of Virginia,
* without reference to its choice of law rules.
*/


#include <pbs_config.h>
#include <string>
#include <vector>
#include <errno.h>
#include <pthread.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "pbs_nodes.h"
#include "pbs_ifl.h"
#include "attribute.h"
#include "alps_constants.h"
#include "log.h"
#include "../lib/Libutils/u_lock_ctl.h"
#include "req_manager.h"
#include "node_manager.h"
#include "node_func.h"
#include "track_alps_reservations.hpp"
#include "login_nodes.h"
#include "svrfunc.h"
#include "issue_request.h"
#include "threadpool.h"
#include "ji_mutex.h"
#include "mutex_mgr.hpp"
#include "server.h"
#include <string>
#include <vector>
#include "container.hpp"
#include "id_map.hpp"

/* Global Data */
extern int LOGLEVEL;
extern attribute_def    node_attr_def[];
extern int svr_clnodes;

/* Prototypes */
int save_node_status(struct pbsnode *current, pbs_attribute *temp);

struct pbsnode *find_alpsnode_by_name(

  struct pbsnode *parent,
  const char    *node_id)

  {
  struct pbsnode *node = NULL;

  parent->alps_subnodes->lock();

  node = parent->alps_subnodes->find(node_id);

  parent->alps_subnodes->unlock();

  if (node != NULL)
    node->lock_node( __func__, NULL, LOGLEVEL);

  return(node);
  } /* END find_alpsnode_by_name() */



/*
 * create_alps_subnode()
 *
 * @param parent - the sdb node or parent of this compute node
 * @param node_id - the name of the new alps compute node
 */

struct pbsnode *create_alps_subnode(

  struct pbsnode *parent,
  const char     *node_id)

  {
  pbsnode *subnode;
  svrattrl       *plist = NULL;
  int             bad;
  int             rc = PBSE_NONE;

  subnode = new pbsnode(node_id, NULL, false);

  if (subnode->get_error() != PBSE_NONE)
    {
    delete subnode;
    log_err(ENOMEM, __func__, "");
    return(NULL);
    }

  // all nodes have at least 1 core
  add_execution_slot(subnode);
  
  // we need to increment this count for accuracy  
  svr_clnodes++;

  /* do we need to do something else here? */
  subnode->nd_addrs = parent->nd_addrs;

  rc = mgr_set_node_attr(subnode, 
      node_attr_def,
      ND_ATR_LAST,
      plist,
      ATR_DFLAG_MGRD | ATR_DFLAG_MGWR,
      &bad,
      (void *)subnode,
      ATR_ACTION_ALTER,
      false);

  if (rc != PBSE_NONE)
    {
    free(subnode);
    log_err(rc, __func__, "Couldn't set node attributes");
    return(NULL);
    }

  subnode->nd_ntype = NTYPE_CLUSTER;
  subnode->parent = parent;
  subnode->nd_state &= ~INUSE_NOHIERARCHY;

  /* add any properties to the subnodes */
  parent->copy_properties(subnode);

  subnode->lock_node(__func__, NULL, LOGLEVEL);
    
  insert_node(parent->alps_subnodes, subnode);
  
  return(subnode);
  } /* END create_alps_subnode() */



void *check_if_orphaned(

  void *vp)

  {
  char           *node_name = (char *)vp;
  char           *rsv_id = NULL;
  std::string     job_id;
  batch_request  *preq;
  int             handle = -1;
  int             retries = 0;
  struct pbsnode *pnode;
  char            log_buf[LOCAL_LOG_BUF_SIZE];

  if ((rsv_id = strchr(node_name, ':')) != NULL)
    {
    *rsv_id = '\0';
    rsv_id++;
    }
  else
    {
    free(node_name);
    return(NULL);
    }

  if (alps_reservations.is_orphaned(rsv_id, job_id) == true)
    {
    // Make sure the node with the orphan is not available for jobs
    if ((pnode = find_nodebyname(node_name)) != NULL)
      {
      if ((pnode->nd_state & (INUSE_BUSY | INUSE_DOWN)) == 0)
        {
        snprintf(log_buf, sizeof(log_buf),
          "Node %s has an orphan but wasn't marked as busy. Marking as busy now.",
          node_name);
        log_err(-1, __func__, log_buf);

        update_node_state(pnode, INUSE_BUSY);
        }

      pnode->unlock_node(__func__, NULL, LOGLEVEL);
      }

    if ((preq = alloc_br(PBS_BATCH_DeleteReservation)) == NULL)
      {
      free(node_name);
      alps_reservations.remove_from_orphaned_list(rsv_id);
      return(NULL);
      }

    preq->rq_extend = strdup(rsv_id);

    if ((pnode = get_next_login_node(NULL)) != NULL)
      {
      struct in_addr hostaddr;
      int            local_errno;
      pbs_net_t      momaddr;

      memcpy(&hostaddr, &pnode->nd_sock_addr.sin_addr, sizeof(hostaddr));
      momaddr = ntohl(hostaddr.s_addr);

      snprintf(log_buf, sizeof(log_buf),
        "Found orphan ALPS reservation ID %s for job %s; asking %s to remove it",
        rsv_id,
        job_id.c_str(),
        pnode->get_name());
      log_record(PBSEVENT_DEBUG, PBS_EVENTCLASS_SERVER, __func__, log_buf);

      while ((handle < 0) &&
             (retries < 3))
        {
        handle = svr_connect(momaddr, pnode->nd_mom_port, &local_errno, pnode, NULL);
        retries++;
        }

      /* unlock before the network transaction */
      pnode->unlock_node(__func__, NULL, LOGLEVEL);
      
      if (handle >= 0)
        issue_Drequest(handle, preq, true);
        
      free_br(preq);
      }

    alps_reservations.remove_from_orphaned_list(rsv_id);
    }

  free(node_name);

  return(NULL);
  } /* END check_if_orphaned() */




struct pbsnode *determine_node_from_str(

  const char     *str,
  struct pbsnode *parent,
  struct pbsnode *current)

  {
  struct pbsnode *next = NULL;
  const char     *node_id = str + strlen("node=");

  if ((current == NULL) || 
      (strcmp(node_id, current->get_name())))
    {
    if (current != NULL)
      current->unlock_node(__func__, NULL, LOGLEVEL);

    if ((next = find_alpsnode_by_name(parent, node_id)) == NULL)
      {
      /* create the node */
      next = create_alps_subnode(parent, node_id);
      }
    
    }
  else
    {
    next = current;
    }
  
  if (next != NULL)
    next->nd_lastupdate = time(NULL);

  return(next);
  } /* END determine_node_from_str() */




int set_ncpus(

  struct pbsnode *current,
  struct pbsnode *parent,
  int             ncpus)

  {
  int difference;
  int i;
  int orig_svr_clnodes;

  if (current == NULL)
    return(PBSE_BAD_PARAMETER);
  
  difference = ncpus - current->nd_slots.get_total_execution_slots();
  orig_svr_clnodes = svr_clnodes;

  for (i = 0; i < abs(difference); i++)
    {
    if (difference > 0)
      {
      add_execution_slot(current); 

      svr_clnodes++;
      }
    else if (difference < 0)
      {
      delete_a_subnode(current);
      svr_clnodes--;
      }
    }
   
  if (difference < 0)
    {
    snprintf(log_buffer, sizeof(log_buffer), "ncpus was reduced from %d to %d", orig_svr_clnodes, svr_clnodes);
    log_record(PBSEVENT_SYSTEM, PBS_EVENTCLASS_NODE, __func__, log_buffer);
    }
  else if (current->nd_slots.get_total_execution_slots() > parent->max_subnode_nppn)
    parent->max_subnode_nppn = current->nd_slots.get_total_execution_slots();

  return(PBSE_NONE);
  } /* END set_ncpus() */




int set_state(

  struct pbsnode *pnode,
  const char     *str)

  {
  const char *state_str = str + strlen("state=");

  if (pnode == NULL)
    return(PBSE_BAD_PARAMETER);

  if (!strcmp(state_str, "UP"))
    update_node_state(pnode, INUSE_FREE);
  else if (!strcmp(state_str, "DOWN"))
    update_node_state(pnode, INUSE_DOWN);
  else if (!strcmp(state_str, "BUSY"))
    update_node_state(pnode, INUSE_BUSY);

  return(PBSE_NONE);
  } /* END set_state() */




void finish_gpu_status(

  unsigned int             &i,
  std::vector<std::string> &status_info)

  {
  while (i < status_info.size())
    {
    if (!strcmp(status_info[i].c_str(), CRAY_GPU_STATUS_END))
      break;

    i++;
    }
  } /* END finish_gpu_status() */




int set_ngpus(

  struct pbsnode *pnode,
  int             gpu_count)

  {
  int difference = gpu_count - pnode->nd_ngpus;
  int i;

  for (i = 0; i < difference; i++)
    {
    if (create_a_gpusubnode(pnode) != PBSE_NONE)
      {
      log_err(ENOMEM, __func__, "");
      return(PBSE_SYSTEM);
      }
    }

  return(PBSE_NONE);
  } /* END set_ngpus() */




int process_gpu_status(

  struct pbsnode           *pnode,
  unsigned int             &i,
  std::vector<std::string> &status_info)

  {
  pbs_attribute   temp;
  int             gpu_count = 0;
  int             rc = PBSE_NONE;
  char            buf[MAXLINE * 2];
  std::string     gpu_info = "";

  memset(&temp, 0, sizeof(temp));
  
  if ((rc = decode_arst(&temp, NULL, NULL, NULL, 0)) != PBSE_NONE)
    {
    log_record(PBSEVENT_DEBUG, PBS_EVENTCLASS_NODE, __func__, "cannot initialize attribute");

    finish_gpu_status(i, status_info);

    return(rc);
    }

  /* move past the initial gpu status */
  i++;
  
  for (; i < status_info.size(); i++)
    {
    if (!strcmp(status_info[i].c_str(), CRAY_GPU_STATUS_END))
      break;

    if (!strncmp(status_info[i].c_str(), "gpu_id=", strlen("gpu_id=")))
      {
      snprintf(buf, sizeof(buf), "gpu[%d]=%s;", gpu_count, status_info[i].c_str());
      gpu_info += buf;
      gpu_count++;
      }
    else
      {
      gpu_info += status_info[i].c_str();
      gpu_info += ';';
      }
    }

  set_ngpus(pnode, gpu_count);
  decode_arst(&temp, NULL, NULL, gpu_info.c_str(), 0);
  node_gpustatus_list(&temp, pnode, ATR_ACTION_ALTER);
  
  free_arst(&temp);

  return(rc);
  } /* END process_gpu_status() */



/*
 * record_reservation()
 *
 * @pre-cond: pnode and rsv_id must be valid pointers
 * @post-cond: the reservation will be recorded in pbs_server's tracking mechanism
 * and on the job which has the node reserved, or -1 is returned and the reservation
 * is not recorded.
 * @param - pnode the node which is reporting the reservation
 * @param - rsv_id the id of the reservation being reported
 * @return - PBSE_NONE if the reservation was successfully recorded, -1 otherwise
 */
int record_reservation(

  struct pbsnode *pnode,
  const char     *rsv_id)

  {
  job            *pjob;
  bool            found_job = false;

  for (unsigned int i = 0; i < pnode->nd_job_usages.size(); i++)
    {
    /* cray only allows one job per node, so any valid job will be the job that is 
     * reserving this node. */
    const job_usage_info &jui = pnode->nd_job_usages[i];
    int                   internal_job_id = jui.internal_job_id;

    pnode->unlock_node(__func__, NULL, LOGLEVEL);

    if ((pjob = svr_find_job_by_id(internal_job_id)) != NULL)
      {
      mutex_mgr job_mutex(pjob->ji_mutex, true);
      pjob->ji_wattr[JOB_ATR_reservation_id].at_val.at_str = strdup(rsv_id);
      pjob->ji_wattr[JOB_ATR_reservation_id].at_flags = ATR_VFLAG_SET;

      /* add environment variable BATCH_PARTITION_ID */
      char buf[1024];
      snprintf(buf, sizeof(buf), "BATCH_PARTITION_ID=%s", rsv_id);
      pbs_attribute  tempattr;
      clear_attr(&tempattr, &job_attr_def[JOB_ATR_variables]);
      job_attr_def[JOB_ATR_variables].at_decode(&tempattr,
        NULL, NULL, buf, 0);

      job_attr_def[JOB_ATR_variables].at_set(
        &pjob->ji_wattr[JOB_ATR_variables], &tempattr, INCR);

      job_attr_def[JOB_ATR_variables].at_free(&tempattr);

      alps_reservations.track_alps_reservation(pjob);
      found_job = true;

      job_mutex.unlock(); 
      pnode->lock_node(__func__, NULL, LOGLEVEL);
      break;
      }
    else
      pnode->lock_node(__func__, NULL, LOGLEVEL);
    }

  if (found_job == false)
    return(-1);

  return(PBSE_NONE);
  } /* END record_reservation() */



int process_reservation_id(
    
  struct pbsnode *pnode, 
  const char     *rsv_id_str)

  {
  const char *rsv_id = rsv_id_str + strlen(reservation_id) + 1;

  if (pnode == NULL)
    return(PBSE_BAD_PARAMETER);
 
  std::string info(pnode->get_name());
  info += ":";
  info += rsv_id;

  if (alps_reservations.already_recorded(rsv_id) == TRUE)
    enqueue_threadpool_request(check_if_orphaned, strdup(info.c_str()), task_pool);
  else if (record_reservation(pnode, rsv_id) != PBSE_NONE)
    enqueue_threadpool_request(check_if_orphaned, strdup(info.c_str()), task_pool);

  return(PBSE_NONE);
  } /* END process_reservation_id() */



#ifdef PENABLE_LINUX_CGROUPS
int set_total_memory(

  pbsnode    *pnode,
  const char *mem_str)

  {
  // Advance the string past "totmem=" so add 7
  long long mem = strtoll(mem_str + 7, NULL, 10);

  pnode->nd_layout.setMemory(mem);

  return(PBSE_NONE);
  } // END set_total_memory()
#endif



int process_alps_status(

  const char               *nd_name,
  std::vector<std::string> &status_info)

  {
  const char    *ccu_p = NULL;
  char           *current_node_id = NULL;
  struct pbsnode *parent;
  struct pbsnode *current = NULL;
  int             rc;
#ifdef PENABLE_LINUX_CGROUPS
  int             numa_nodes = 0;
  int             sockets = 0;
#endif
  pbs_attribute   temp;
  container::item_container<const char *> rsv_ht;
  char            log_buf[LOCAL_LOG_BUF_SIZE];

  memset(&temp, 0, sizeof(temp));

  if ((rc = decode_arst(&temp, NULL, NULL, NULL, 0)) != PBSE_NONE)
    {
    log_record(PBSEVENT_DEBUG, PBS_EVENTCLASS_NODE, __func__, "cannot initialize attribute");
    return(rc);
    }

  /* if we can't find the parent node, ignore the update */
  if ((parent = find_nodebyname(nd_name)) == NULL)
    return(PBSE_NONE);

  /* loop over each string */
  for (unsigned int i = 0; i < status_info.size(); i++)
    {
    const char *str = status_info[i].c_str();

    if (!strncmp(str, "node=", strlen("node=")))
      {
      if (i != 0)
        {
        if (current != NULL)
          save_node_status(current, &temp);
        }

      if ((current = determine_node_from_str(str, parent, current)) == NULL)
        break;
      else
        {
#ifdef PENABLE_LINUX_CGROUPS
        sockets = 0;
        numa_nodes = 0;
#endif

        continue;
        }
      }

    if (current == NULL)
      continue;

    /* process the gpu status information separately */
    if (!strcmp(CRAY_GPU_STATUS_START, str))
      {
      rc = process_gpu_status(current, i, status_info);
      continue;
      }
    else if (!strncmp(reservation_id, str, strlen(reservation_id)))
      {
      const char *just_rsv_id = str + strlen(reservation_id);

      rsv_ht.lock();
      if (rsv_ht.find(just_rsv_id) == NULL)
        {
        rsv_ht.insert(just_rsv_id,just_rsv_id);
        rsv_ht.unlock();

        /* sub-functions will attempt to lock a job, so we must unlock the
         * reporter node */
        parent->unlock_node(__func__, NULL, LOGLEVEL);

        process_reservation_id(current, str);

        current_node_id = strdup(current->get_name());
        current->unlock_node(__func__, NULL, LOGLEVEL);

        /* re-lock the parent */
        if ((parent = find_nodebyname(nd_name)) == NULL)
          {
          /* reporter node disappeared - this shouldn't be possible */
          log_err(PBSE_UNKNODE, __func__, "Alps reporter node disappeared while recording a reservation");
          free_arst(&temp);
          free(current_node_id);
          return(PBSE_NONE);
          }

        if ((current = find_node_in_allnodes(parent->alps_subnodes, current_node_id)) == NULL)
          {
          /* current node disappeared, this shouldn't be possible either */
          parent->unlock_node(__func__, NULL, LOGLEVEL);
          snprintf(log_buf, sizeof(log_buf), "Current node '%s' disappeared while recording a reservation",
            current_node_id);
          log_err(PBSE_UNKNODE, __func__, log_buf);
          free_arst(&temp);
          free(current_node_id);
          return(PBSE_NONE);
          }

        free(current_node_id);
        current_node_id = NULL;
        }
      else
        {
        rsv_ht.unlock();
        }
      }
    /* save this as is to the status strings */
    else if ((rc = decode_arst(&temp, NULL, NULL, str, 0)) != PBSE_NONE)
      {
      free_arst(&temp);
      return(rc);
      }

    /* perform any special processing */
    if (!strncmp(str, ccu_eq, ac_ccu_eq_len))
      {
      /* save compute unit count in case we need it */
      /* note: this string (ccu_eq (CCU=)) needs to be found before cprocs_eq (CPROCS=) */
      /*  for the node */
      ccu_p = str;
      }
    else if (!strncmp(str, cproc_eq, ac_cproc_eq_len))
      {
      int ncpus;
      long svr_nppcu_value = 0;

      /*
       * Get the server nppcu value which determines how Hyper-Threaded
       * cores are reported. When server nppcu value is:
       *
       *  0 - Let ALPS choose whether or not to use Hyper-Threaded cores 
       *      (report all cores)
       *  1 - Do not use Hyper-Threaded cores
       *      (report only physical core (compute unit count)
       *  2 - Use Hyper-Threaded cores
       *      (report all cores)
       */
      get_svr_attr_l(SRV_ATR_nppcu, &svr_nppcu_value);

      if (svr_nppcu_value == NPPCU_NO_USE_HT && ccu_p != NULL)
        {
        /* no HT (nppcu==1), so use compute unit count */
        ncpus = atoi(ccu_p + ac_ccu_eq_len);

        /* use CPROC value if we are using APBASIL protocol < 1.3 */
        if (ncpus == 0)
          ncpus = atoi(str + ac_cproc_eq_len);

        /* reset the pointer */
        ccu_p = NULL;
        }
      else
        {
        /* let ALPS choose (nppcu==0) or use HT (nppcu==2), use actual processor count */
        ncpus = atoi(str + ac_cproc_eq_len);
        }

      set_ncpus(current, parent, ncpus);

#ifdef PENABLE_LINUX_CGROUPS
      if (numa_nodes == 0)
        numa_nodes = 1;

      if ((current->nd_layout.is_initialized() == false) ||
          (current->nd_layout.getTotalThreads() != current->nd_slots.get_total_execution_slots()))
        {
        Machine m(current->nd_slots.get_total_execution_slots(), numa_nodes, sockets);
        current->nd_layout = m;
        }
#endif
      }
    else if (!strncmp(str, state, strlen(state)))
      {
      set_state(current, str);
      }
#ifdef PENABLE_LINUX_CGROUPS
    else if (!strncmp(str, "totmem", 6))
      {
      set_total_memory(current, str);
      }
    else if (!strncmp(str, numas, 10))
      {
      // 11 is strlen("numa_nodes=")
      numa_nodes = strtol(str + 11, NULL, 10);
      }
    else if (!strncmp(str, "socket", 6))
      {
      // 7 is strlen("socket=")
      sockets = strtol(str + 7, NULL, 10);
      }
#endif

    } /* END processing the status update */

  if (current != NULL)
    {
    save_node_status(current, &temp);
    current->unlock_node(__func__, NULL, LOGLEVEL);
    }

  parent->unlock_node(__func__, NULL, LOGLEVEL);

  return(PBSE_NONE);
  } /* END process_alps_status() */


