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
/*
 * svr_manager.c
 *
 * Functions relating to the Manager Batch Request (qmgr).
 *
 * Included funtions are:
 *
 * mgr_set_attr()  - set attributes for an server/queue object
 * mgr_set_node_attr() - sets "node-attributes" (state,properties etc)
 *      on a node
 * mgr_queue_create() - create a queue
 * mgr_queue_delete() - delete a queue
 * mgr_server_set() - set server attributes
 * mgr_server_unset() - unset server attributes
 * mgr_queue_set()  - set queue attributes
 * mgr_queue_unset() - unset queue attributes
 * mgr_node_set()  - top level for setting "node-attributes" on
 *      a set of nodes
 *      mgr_node_delete()       - top level function for effectively deleting a node
 *      mgr_node_create()       - top level function for creating a node
 * req_manager()  - process manager request (top level)
 * disallowed_types_chk() - at action for queue disallowed_types pbs_attribute
 */
#include <pbs_config.h>   /* the master config generated by configure */
#include "req_manager.h"

#include <sys/types.h>
#include "libpbs.h"
#include <ctype.h>
#include <memory.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include "server_limits.h"
#include "list_link.h"
#include "attribute.h"
#include "resource.h"
#include "server.h"
#include "queue.h"
#include "credential.h"
#include "utils.h"
#include "batch_request.h"
#include "net_connect.h"
#include "pbs_error.h"
#include "log.h"
#include "../lib/Liblog/pbs_log.h"
#include "../lib/Liblog/log_event.h"
#include "svrfunc.h"
#include "pbs_job.h"
#include "pbs_nodes.h"
#include "work_task.h"
#include "mcom.h"
#include "../lib/Libattr/attr_node_func.h" /* free_prop_list */
#include "node_func.h" /* init_prop, find_nodebyname, reinitialize_node_iterator, recompute_ntype_cnts, effective_node_delete, create_pbs_node */
#include "node_manager.h" /* setup_notification */
#include "../lib/Libutils/u_lock_ctl.h" /* unlock_node */
#include "queue_func.h" /* find_queuebyname, que_alloc, que_free */
#include "queue_recov.h" /* que_save */
#include "mutex_mgr.hpp"

#define PERM_MANAGER (ATR_DFLAG_MGWR | ATR_DFLAG_MGRD)
#define PERM_OPorMGR (ATR_DFLAG_MGWR | ATR_DFLAG_MGRD | ATR_DFLAG_OPRD | ATR_DFLAG_OPWR)

extern attribute_def  node_attr_def[];   /* node attributes defs */

/*
 * mgr_set_node_attr - "set state, properties, ntype" for mgr_node_set function
 *
 * The set operation is performed as an atomic operation--  all specified
 * items must be successfully set for the node, or none are modified.
 *
 * Although a node doesn't, at present, have an array of attributes as one
 * of its structure members, we will proceed as though it did by using the
 *      node to derive a temporary array of attributes.  We use the "action"
 *      function of our artificial attribute_def array with a mode value of NEW
 *      in constructing this temporary node pbs_attribute array. Once this temporary
 *      array is constucted, the "svrattrl" request list is processed against it
 * to produce a new_attr "updated" pbs_attribute array.  When all the pbs_attribute
 *      processing completes, assuming it's successful, the updated pbs_attribute
 *      array is used to modify the node itself-- this can be done via each
 *      pbs_attribute's "action" function with the value of mode set to "ALTER".
 * Although it's a little twisted to use the idea of an pbs_attribute's
 * "action function" to move between node an pbs_attribute,it pushes the
 *      details down a level, below that which is taking place here-- a good
 *      thing.
 */

int mgr_modify_node(

  struct pbsnode **ppnode,  /* do some kind of "set" action on this node */
  attribute_def  *pdef,   /* pbsnode doesn't have attributes but in the
                             future it might  so, make  code similar to existing */
  int             limit,  /* length of the *pdef array      */
  svrattrl       *plist,  /* batch request's svrattrl list begins here */
  int             privil, /* requester's privilege          */
  int            *bad,    /* if there is a "bad pbs_attribute" pass back
                             position via this loc */
  int             mode)  /*passed to attrib's action func not used by
                           this func at this time*/

  {
  int              index;
  int              rc = PBSE_NONE;
  pbs_attribute   *new_attr;
  pbs_attribute   *unused = NULL;
  pbs_attribute   *pnew;
  long             dont_update_nodes = FALSE;

  if (plist == NULL)
    {
    return(0);  /* nothing to do, return success */
    }
  
  get_svr_attr_l(SRV_ATR_DontWriteNodesFile, &dont_update_nodes);

  /* Get heap space for a temporary node-pbs_attribute array and use the
   * various "node-attribute action" functions defined in the file
   * "node_attr_def.c" to set the current values of each entry's
   * structure members
   */

  new_attr = (pbs_attribute *)calloc((unsigned int)limit, sizeof(pbs_attribute));

  if (new_attr == NULL)
    {
    return(PBSE_SYSTEM);
    }

  /*
   * The function "attr_atomic_node_set" does the following:
   * successively decodes the new pbs_attribute modifications carried in the
   * the request list, "plist", into a temporary pbs_attribute (on stack).
   * Updates each pbs_attribute in the derived node-attribute array using the
   * decoded values.  If a failure of some sort occurs, as evidenced by
   * a non-zero return code (rc), call attr_atomic_kill() to undo
   * everything-- calls upon the "at_free" function for each pbs_attribute
   * to free hanging structures, then frees up the "new" array
   * return code (rc) shapes caller's reply
   */

  if ((rc = attr_atomic_node_set(plist,
                                 unused,
                                 new_attr,
                                 pdef,
                                 limit,
                                 -1,
                                 privil,
                                 bad,
                                 dont_update_nodes)) != 0)
    {
    attr_atomic_kill(new_attr, pdef, limit);

    return(rc);
    }

  for(index = ND_ATR_state;index < ND_ATR_LAST;index++)
    {
    pnew = new_attr + index;
    if (pnew->at_flags & ATR_VFLAG_MODIFY)
      {
      switch(index)
        {
        case ND_ATR_power_state:
          {
          rc = set_node_power_state(ppnode,pnew->at_val.at_short);
          }
          break;
        case ND_ATR_ttl:
          {
          snprintf((*ppnode)->nd_ttl, sizeof((*ppnode)->nd_ttl), "%s", pnew->at_val.at_str);
          rc = PBSE_NONE;
          }
          break;
        case ND_ATR_acl:
          {
          (*ppnode)->nd_acl = pnew->at_val.at_arst;
          rc = PBSE_NONE;
          }
          break;
        case ND_ATR_requestid:
          {
          *(*ppnode)->nd_requestid = pnew->at_val.at_str;
          rc = PBSE_NONE;
          }
          break;
        case ND_ATR_state:
        case ND_ATR_np:
        case ND_ATR_properties:
        case ND_ATR_ntype:
        case ND_ATR_jobs:
        case ND_ATR_status:
        case ND_ATR_note:
        case ND_ATR_mom_port:
        case ND_ATR_mom_rm_port:
        case ND_ATR_num_node_boards:
        case ND_ATR_numa_str:
        case ND_ATR_gpus:
        case ND_ATR_gpus_str:
        case ND_ATR_gpustatus:
        case ND_ATR_mics:
        case ND_ATR_micstatus:
#ifdef PENABLE_LINUX_CGROUPS
        case ND_ATR_total_sockets:
        case ND_ATR_total_numa_nodes:
        case ND_ATR_total_cores:
        case ND_ATR_total_threads:
        case ND_ATR_dedicated_sockets:
        case ND_ATR_dedicated_numa_nodes:
        case ND_ATR_dedicated_cores:
        case ND_ATR_dedicated_threads:
#endif
        default:
          rc = PBSE_IVALREQ;
          break;
        }
      }
    if((rc != PBSE_NONE)||(*ppnode == NULL))
      {
      break;
      }
    }

  free(new_attr);
  return(rc);
  }  /* END mgr_modify_node() */


/*
 * mgr_node_set - Set node "state" or "properties"
 *
 * Finds the node, Sets the requested state flags, properties, or type and returns
 * a reply to the sender of the batch_request
 */

void mgr_node_modify(

  struct batch_request *preq)  /* I */

  {
  int               need_todo = 0;

  int               rc;
  int               bad = 0;

  const char       *nodename = NULL;

  svrattrl         *plist;
  node_check_info   nci;

  struct pbsnode   *pnode = NULL;

  nodename = preq->rq_ind.rq_manager.rq_objname;
  pnode = find_nodebyname(nodename);

  if (pnode == NULL)
    {
    req_reject(PBSE_UNKNODE, 0, preq, NULL, NULL);

    return;
    }

  plist = (svrattrl *)GET_NEXT(preq->rq_ind.rq_manager.rq_attr);

  save_characteristic(pnode,&nci);

  rc = mgr_modify_node(
         &pnode,
         node_attr_def,
         ND_ATR_LAST,
         plist,
         preq->rq_perm,
         &bad,
         ATR_ACTION_ALTER);

  if (rc != 0)
    {
    /* In the specific node case, reply w/ error and return*/

    switch (rc)
      {
      case PBSE_INTERNAL:

      case PBSE_SYSTEM:

        req_reject(rc, bad, preq, NULL, NULL);

        break;

      case PBSE_NOATTR:

      case PBSE_ATTRRO:

      case PBSE_MUTUALEX:

      case PBSE_BADNDATVAL:

        reply_badattr(rc, bad, plist, preq);

        break;

      default:

        req_reject(rc, 0, preq, NULL, NULL);

        break;
      }

    if(pnode != NULL)
      {
      unlock_node(pnode, "mgr_node_set", (char *)"error", LOGLEVEL);
      pnode = NULL;
      }

    return;
    } /* END if (rc != 0) */
  else
    {
      /* modifications succeeded for this node */
    if(pnode != NULL)
      {
      chk_characteristic(pnode, &nci, &need_todo);
      }
    }

  if(pnode != NULL)
    {
    unlock_node(pnode, "mgr_node_set", (char *)"single_node", LOGLEVEL);
    pnode = NULL;
    }

  if (need_todo & WRITENODE_STATE)
    {
    /*some nodes set to "offline"*/
    write_node_state();

    need_todo &= ~(WRITENODE_STATE);
    }

  if (need_todo & WRITENODE_POWER_STATE)
    {
    /*some nodes changed power state*/
    write_node_power_state();

    need_todo &= ~(WRITENODE_POWER_STATE);
    }

  if (need_todo & WRITENODE_NOTE)
    {
    /*some nodes have new "note"s*/
    write_node_note();

    need_todo &= ~(WRITENODE_NOTE);
    }

  if (need_todo & WRITE_NEW_NODESFILE)
    {
    /*create/delete/prop/ntype change*/
    if (!update_nodes_file(NULL))
      need_todo &= ~(WRITE_NEW_NODESFILE);  /*successful on update*/
    }

  recompute_ntype_cnts();

  reply_ack(preq);  /*request completely successful*/

  return;
  }  /* END void mgr_node_set() */





/*
 * req_modify_node - the dispatch routine for a series of functions which
 * can send a request to a node to modify it.
 *
 * The privilege of the requester is checked against the type of
 * the object and the operation to be performed on it.  Then the
 * appropriate function is called to perform the operation.
 */

int req_modify_node(
    struct batch_request *preq)
  {
  int rc = PBSE_NONE;
  char log_buf[LOCAL_LOG_BUF_SIZE+1];

  switch (preq->rq_ind.rq_manager.rq_cmd)
    {

    case MGR_CMD_SET:

      if ((preq->rq_perm & PERM_OPorMGR) == 0)
        {
        rc = PBSE_PERM;
        snprintf(log_buf, LOCAL_LOG_BUF_SIZE,
            "error in permissions (PERM_OPorMGR)");
        req_reject(rc, 0, preq, NULL, log_buf);
        return rc;
        }

      switch (preq->rq_ind.rq_manager.rq_objtype)
        {

        case MGR_OBJ_NODE:

          mgr_node_modify(preq);

          break;

        default:

          req_reject(PBSE_IVALREQ, 0, preq, NULL, NULL);

          break;
        }

      break;


    default: /* batch_request specified an invalid command */

      req_reject(PBSE_IVALREQ, 0, preq, NULL, NULL);

      break;
    }  /* END switch() */

  return rc;
  }  /* END req_modify_node() */


/* END req_modify_node.c */

