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



#include <errno.h>
#include <pthread.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "dynamic_string.h"
#include "pbs_nodes.h"
#include "attribute.h"
#include "alps_constants.h"
#include "log.h"
#include "../lib/Libutils/u_lock_ctl.h"
#include "req_manager.h"
#include "node_manager.h"
#include "node_func.h"
#include "track_alps_reservations.h"
#include "login_nodes.h"
#include "svrfunc.h"
#include "issue_request.h"
#include "threadpool.h"
#include "ji_mutex.h"

/* Global Data */
extern int LOGLEVEL;
extern attribute_def    node_attr_def[];
extern int svr_clnodes;

/* Prototypes */
int save_node_status(struct pbsnode *current, pbs_attribute *temp);

struct pbsnode *find_alpsnode_by_name(

  struct pbsnode *parent,
  char           *node_id)

  {
  struct pbsnode *node = NULL;
  int             index;

  pthread_mutex_lock(parent->alps_subnodes.allnodes_mutex);

  index = get_value_hash(parent->alps_subnodes.ht, node_id);

  if (index >= 0)
    node = (struct pbsnode *)parent->alps_subnodes.ra->slots[index].item;

  pthread_mutex_unlock(parent->alps_subnodes.allnodes_mutex);

  if (node != NULL)
    lock_node(node, __func__, NULL, 0);

  return(node);
  } /* END find_alpsnode_by_name() */




struct pbsnode *create_alps_subnode(

  struct pbsnode *parent,
  char           *node_id)

  {
  struct pbsnode *subnode = calloc(1, sizeof(struct pbsnode));
  svrattrl       *plist = NULL;
  int             bad;
  int             rc;

  if (initialize_pbsnode(subnode, strdup(node_id), NULL, NTYPE_CLUSTER) != PBSE_NONE)
    {
    free(subnode);
    log_err(ENOMEM, __func__, "");
    return(NULL);
    }

  if (create_subnode(subnode) == NULL)
    {
    free(subnode);
    log_err(ENOMEM, __func__, "");
    return(NULL);
    }

  /* do we need to do something else here? */
  subnode->nd_addrs = parent->nd_addrs;

  rc = mgr_set_node_attr(subnode, 
      node_attr_def,
      ND_ATR_LAST,
      plist,
      ATR_DFLAG_MGRD | ATR_DFLAG_MGWR,
      &bad,
      (void *)subnode,
      ATR_ACTION_ALTER);

  if (rc != PBSE_NONE)
    {
    free(subnode);
    log_err(rc, __func__, "Couldn't set node attributes");
    return(NULL);
    }

  subnode->nd_ntype = NTYPE_CLUSTER;
  subnode->parent = parent;

  /* add any properties to the subnodes */
  copy_properties(subnode, parent);

  lock_node(subnode, __func__, NULL, 0);
    
  insert_node(&(parent->alps_subnodes), subnode);
  
  return(subnode);
  } /* END create_alps_subnode() */




void *check_if_orphaned(

  void *vp)

  {
  char                 *rsv_id = (char *)vp;
  struct batch_request *preq;
  int                   handle = -1;
  int                   retries = 0;
  struct pbsnode       *pnode;
  char                  log_buf[LOCAL_LOG_BUF_SIZE];

  if (is_orphaned(rsv_id) == TRUE)
    {
    preq = alloc_br(PBS_BATCH_DeleteReservation);
    preq->rq_extend = rsv_id;

    if ((pnode = get_next_login_node(NULL)) != NULL)
      {
      struct in_addr hostaddr;
      int            local_errno;
      pbs_net_t      momaddr;

      memcpy(&hostaddr, &pnode->nd_sock_addr.sin_addr, sizeof(hostaddr));
      momaddr = ntohl(hostaddr.s_addr);

      snprintf(log_buf, sizeof(log_buf),
        "Found orphan ALPS reservation ID %s; asking %s to remove it",
        rsv_id,
        pnode->nd_name);
      log_record(PBSEVENT_DEBUG, PBS_EVENTCLASS_SERVER, __func__, log_buf);

      while ((handle < 0) &&
             (retries < 3))
        {
        handle = svr_connect(momaddr, pnode->nd_mom_port, &local_errno, pnode, NULL, ToServerDIS);
        retries++;
        }

      /* unlock before the network transaction */
      unlock_node(pnode, __func__, NULL, 0);
      
      if (handle >= 0)
        issue_Drequest(handle, preq);
        
      free_br(preq);
      }
    }
  else
    free(rsv_id);

  return(NULL);
  } /* END check_if_orphaned() */




struct pbsnode *determine_node_from_str(

  char           *str,
  struct pbsnode *parent,
  struct pbsnode *current)

  {
  struct pbsnode *next = NULL;
  char           *node_id = str + strlen("node=");

  if ((current == NULL) || 
      (strcmp(node_id, current->nd_name)))
    {
    if (current != NULL)
      unlock_node(current, __func__, NULL, 0);

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
  char           *str)

  {
  int ncpus;
  int difference;
  int i;

  if (current == NULL)
    return(PBSE_BAD_PARAMETER);
  
  ncpus = atoi(str + cproc_eq_len);
  difference = ncpus - current->nd_nsn;

  for (i = 0; i < difference; i++)
    {
    if (create_subnode(current) == NULL)
      {
      log_err(ENOMEM, __func__, "");
      return(PBSE_SYSTEM);
      }

    svr_clnodes++;
    }

  return(PBSE_NONE);
  } /* END set_ncpus() */




int set_state(

  struct pbsnode *pnode,
  char           *str)

  {
  char *state_str = str + strlen("state=");

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




char *finish_gpu_status(

  char *str)

  {
  while ((str != NULL) &&
         (str[0] != '\0'))
    {
    if (!strcmp(str, CRAY_GPU_STATUS_END))
      break;

    str += strlen(str) + 1;
    }

  return(str);
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

  struct pbsnode  *pnode,
  char           **str_ptr)

  {
  char           *str = *str_ptr;
  pbs_attribute   temp;
  int             gpu_count = 0;
  int             rc;
  char            buf[MAXLINE * 2];
  dynamic_string *gpu_info;

  memset(&temp, 0, sizeof(temp));
  
  if ((gpu_info = get_dynamic_string(-1, NULL)) == NULL)
    {
    *str_ptr = finish_gpu_status(str);

    return(ENOMEM);
    }

  if ((rc = decode_arst(&temp, NULL, NULL, NULL, 0)) != PBSE_NONE)
    {
    log_record(PBSEVENT_DEBUG, PBS_EVENTCLASS_NODE, __func__, "cannot initialize attribute");

    *str_ptr = finish_gpu_status(str);
    free_dynamic_string(gpu_info);

    return(rc);
    }

  /* move past the initial gpu status */
  str += strlen(str) + 1;
  
  for (; str != NULL && *str != '\0'; str += strlen(str) + 1)
    {
    if (!strcmp(str, CRAY_GPU_STATUS_END))
      break;

    if (!strncmp(str, "gpu_id=", strlen("gpu_id=")))
      {
      snprintf(buf, sizeof(buf), "gpu[%d]=%s;", gpu_count, str);
      rc = append_dynamic_string(gpu_info, buf);
      gpu_count++;
      }
    else
      {
      rc = append_dynamic_string(gpu_info, str);
      rc = append_char_to_dynamic_string(gpu_info, ';');
      }

    if (rc != PBSE_NONE)
      {
      free_dynamic_string(gpu_info);

      *str_ptr = finish_gpu_status(str);

      return(rc);
      }
    }

  set_ngpus(pnode, gpu_count);
  decode_arst(&temp, NULL, NULL, gpu_info->str, 0);
  node_gpustatus_list(&temp, pnode, ATR_ACTION_ALTER);
  
  free_arst(&temp);
  free_dynamic_string(gpu_info);

  *str_ptr = str;

  return(PBSE_NONE);
  } /* END process_gpu_status() */






int record_reservation(

  struct pbsnode *pnode,
  char           *rsv_id)

  {
  struct pbssubn *sub_node;
  job            *pjob;
  int             found_job = FALSE;
  char            jobid[PBS_MAXSVRJOBID + 1];
  
  for (sub_node = pnode->nd_psn; sub_node != NULL; sub_node = sub_node->next)
    {
    if (sub_node->jobs != NULL)
      {
      strcpy(jobid, sub_node->jobs->jobid);

      unlock_node(pnode, __func__, NULL, 0);

      if ((pjob = svr_find_job(jobid, TRUE)) != NULL)
        {
        pjob->ji_wattr[JOB_ATR_reservation_id].at_val.at_str = strdup(rsv_id);
        pjob->ji_wattr[JOB_ATR_reservation_id].at_flags = ATR_VFLAG_SET;

        create_alps_reservation(pjob);
        found_job = TRUE;

        unlock_ji_mutex(pjob, __func__, "1", LOGLEVEL);
        lock_node(pnode, __func__, NULL, 0);
        break;
        }
      else
        lock_node(pnode, __func__, NULL, 0);
      }
    }

  if (found_job == FALSE)
    return(-1);

  return(PBSE_NONE);
  } /* END record_reservation() */




int process_reservation_id(
    
  struct pbsnode *pnode, 
  char           *rsv_id_str)

  {
  char           *rsv_id;

  if (pnode == NULL)
    return(PBSE_BAD_PARAMETER);
 
  rsv_id = strdup(rsv_id_str + strlen(reservation_id) + 1);

  if (already_recorded(rsv_id) == TRUE)
    enqueue_threadpool_request(check_if_orphaned, rsv_id);
  else if (record_reservation(pnode, rsv_id) != PBSE_NONE)
    enqueue_threadpool_request(check_if_orphaned, rsv_id);
  else
    free(rsv_id);

  return(PBSE_NONE);
  } /* END process_reservation_id() */




int process_alps_status(

  char           *nd_name,
  dynamic_string *status_info)

  {
  char           *str;
  char            node_index_buf[MAXLINE];
  int             node_index = 0;
  struct pbsnode *parent;
  struct pbsnode *current = NULL;
  int             rc;
  pbs_attribute   temp;
  hash_table_t   *rsv_ht;

  memset(&temp, 0, sizeof(temp));

  if ((rc = decode_arst(&temp, NULL, NULL, NULL, 0)) != PBSE_NONE)
    {
    log_record(PBSEVENT_DEBUG, PBS_EVENTCLASS_NODE, __func__, "cannot initialize attribute");
    return(rc);
    }

  /* if we can't find the parent node, ignore the update */
  if ((parent = find_nodebyname(nd_name)) == NULL)
    return(PBSE_NONE);

  /* keep track of reservations so that they're only processed once per update */
  rsv_ht = create_hash(INITIAL_RESERVATION_HOLDER_SIZE);

  /* loop over each string */
  for (str = status_info->str; str != NULL && *str != '\0'; str += strlen(str) + 1)
    {
    if (!strncmp(str, "node=", strlen("node=")))
      {
      if (str != status_info->str)
        {
        snprintf(node_index_buf, sizeof(node_index_buf), "node_index=%d", node_index++);
        decode_arst(&temp, NULL, NULL, node_index_buf, 0);
        save_node_status(current, &temp);
        }

      if ((current = determine_node_from_str(str, parent, current)) == NULL)
        break;
      else
        continue;
      }

    /* process the gpu status information separately */
    if (!strcmp(CRAY_GPU_STATUS_START, str))
      {
      process_gpu_status(current, &str);
      continue;
      }
    else if (!strncmp(reservation_id, str, strlen(reservation_id)))
      {
      char *just_rsv_id = str + strlen(reservation_id);

      if (get_value_hash(rsv_ht, just_rsv_id) == -1)
        {
        add_hash(rsv_ht, 1, strdup(just_rsv_id));
        process_reservation_id(current, str);
        }
      }
    /* save this as is to the status strings */
    else if ((rc = decode_arst(&temp, NULL, NULL, str, 0)) != PBSE_NONE)
      {
      free_arst(&temp);
      free_all_keys(rsv_ht);
      free_hash(rsv_ht);
      return(rc);
      }

    /* perform any special processing */
    if (!strncmp(str, cproc_eq, cproc_eq_len))
      {
      set_ncpus(current, str);
      }
    else if (!strncmp(str, state, strlen(state)))
      {
      set_state(current, str);
      }

    } /* END processing the status update */

  if (current != NULL)
    {
    snprintf(node_index_buf, sizeof(node_index_buf), "node_index=%d", node_index++);
    decode_arst(&temp, NULL, NULL, node_index_buf, 0);
    save_node_status(current, &temp);
    unlock_node(current, __func__, NULL, 0);
    }

  unlock_node(parent, __func__, NULL, 0);

  free_all_keys(rsv_ht);
  free_hash(rsv_ht);

  return(PBSE_NONE);
  } /* END process_alps_status() */


