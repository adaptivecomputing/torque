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
#include "node_func.h" /* init_prop, find_nodebyname, reinitialize_node_iterator, recompute_ntype_cnts, effective_node_delete, create_pbs_dynamic_node */
#include "node_manager.h" /* setup_notification */
#include "queue_func.h" /* find_queuebyname, que_alloc, que_free */
#include "queue_recov.h" /* que_save */
#include "mutex_mgr.hpp"
#include "ji_mutex.h"
#include "req_rerun.h"
#include "req_delete.h"
#include "mom_hierarchy_handler.h"
#include "attr_req_info.hpp"


#define PERM_MANAGER (ATR_DFLAG_MGWR | ATR_DFLAG_MGRD)
#define PERM_OPorMGR (ATR_DFLAG_MGWR | ATR_DFLAG_MGRD | ATR_DFLAG_OPRD | ATR_DFLAG_OPWR)

/* Global Data Items: */

extern int            LOGLEVEL;
extern struct server  server;
extern attribute_def  que_attr_def[];
extern attribute_def  svr_attr_def[];
extern attribute_def  node_attr_def[];   /* node attributes defs */
extern int            svr_chngNodesfile;
extern char          *msg_attrtype;
extern char          *msg_daemonname;
extern char          *msg_manager;
extern char          *msg_man_cre;
extern char          *msg_man_del;
extern char          *msg_man_set;
extern char          *msg_man_uns;
extern time_t         pbs_incoming_tcp_timeout;
//extern mom_hierarchy_t *mh;


extern int que_purge(pbs_queue *);
extern void save_characteristic(struct pbsnode *, node_check_info *);
extern int chk_characteristic(struct pbsnode *, node_check_info *, int *);
extern int PNodeStateToString(int, char *, int);
extern job *get_job_from_job_usage_info(job_usage_info *jui, struct pbsnode *pnode);


//extern void add_all_nodes_to_hello_container();
//void        prepare_mom_hierarchy(std::vector<std::string> &hierarchy_holder);
//extern      std::vector<std::string> hierarchy_holder;
//extern       pthread_mutex_t         hierarchy_holder_Mutex;

/* private data */

static const char *all_quename = "_All_";
static const char *all_nodes_str = "_All_";

/*
 * check_que_attr - check if attributes in request are consistent with
 * the current queue type.  This is called when creating or setting
 * the attributes of a queue.
 *
 * Returns: NULL if all ok or name of bad pbs_attribute if not ok
 */

static const char *check_que_attr(
    
  pbs_queue *pque)

  {
  int   i;
  int   type;

  type = pque->qu_qs.qu_type;  /* current type of queue */

  for (i = 0; i < QA_ATR_LAST; ++i)
    {
    if (pque->qu_attr[i].at_flags & ATR_VFLAG_SET)
      {
      if (que_attr_def[i].at_parent == PARENT_TYPE_QUE_EXC)
        {
        if (type == QTYPE_Unset)
          type = QTYPE_Execution;
        else if (type != QTYPE_Execution)
          return (que_attr_def[i].at_name);

        }
      else if (que_attr_def[i].at_parent == PARENT_TYPE_QUE_RTE)
        {
        if (type == QTYPE_Unset)
          type = QTYPE_RoutePush;
        else if (type != QTYPE_RoutePush)
          return (que_attr_def[i].at_name);
        }
      }
    }

  return ((char *)0); /* all attributes are ok */
  } /* END check_que_attr() */




/*
 * check_que_enable - check if attempt to enable incompletely defined queue
 * This is the at_action() routine for QA_ATR_Enabled
 */

int check_que_enable(

  pbs_attribute *pattr,
  void          *pque, /* actually a queue* */
  int            mode)

  {
  pbs_attribute *datr;

  if (pattr->at_val.at_long != 0)
    {

    /*
     * admin attempting to  enabled queue,
     * is it completely defined
     */

    if (((pbs_queue *)pque)->qu_qs.qu_type == QTYPE_Unset)
      return (PBSE_QUENOEN);
    else if (((pbs_queue *)pque)->qu_qs.qu_type == QTYPE_RoutePush)
      {
      datr = &((pbs_queue *)pque)->qu_attr[QR_ATR_RouteDestin];

      if (!(datr->at_flags & ATR_VFLAG_SET) ||
          (datr->at_val.at_arst->as_usedptr == 0))
        return (PBSE_QUENOEN);
      }
    }

  return(PBSE_NONE); /* ok to enable queue */
  } /* END check_que_enable() */




/*
 * set_queue_type - check the requested value of the queue type pbs_attribute
 * and set qu_type accordingly.
 * This is the at_action() routine for QA_ATR_QType
 */

int set_queue_type(
    
  pbs_attribute *pattr,
  void          *pque,
  int            mode)

  {
  int   i;
  char *pca;
  const char *pcv;
  int   spectype;

  static struct
    {
    int   type;
    const char *name;
    } qt[2] =

    {
      { QTYPE_Execution, "Execution" },
    { QTYPE_RoutePush, "Route" }
    };

  if ((pattr->at_flags & ATR_VFLAG_SET) == 0)
    /* better be set or we shouldn't be here */
    return (PBSE_BADATVAL);

  /* does the requested value match a legal value? */

  for (i = 0; i < 2; i++)
    {
    spectype = qt[i].type;
    pca = pattr->at_val.at_str;
    pcv = qt[i].name;

    if (*pca == '\0')
      return (PBSE_BADATVAL);

    while (*pca)
      {
      if (toupper((int)*pca++) != toupper((int)*pcv++))
        {
        spectype = -1; /* no match */
        break;
        }
      }

    if (spectype != -1)   /* set up the pbs_attribute */
      {
      ((pbs_queue *)pque)->qu_qs.qu_type = spectype;
      free(pattr->at_val.at_str);
      pattr->at_val.at_str = (char *)calloc(1, strlen(qt[i].name) + 1);

      if (pattr->at_val.at_str == (char *)0)
        return (PBSE_SYSTEM);

      strcpy(pattr->at_val.at_str, qt[i].name);

      return(PBSE_NONE);
      }
    }

  return (PBSE_BADATVAL);
  } /* set_queue_type() */




/*
 * mgr_log_attr - log the change of an pbs_attribute
 */

void mgr_log_attr(

  const char      *msg,
  struct svrattrl *plist,
  int              logclass,           /* see log.h */
  const char      *objname)          /* object being modified */

  {
  const char *pstr;
  char  log_buf[LOCAL_LOG_BUF_SIZE];

  while (plist)
    {
    snprintf(log_buf, sizeof(log_buf), "%s%s", msg, plist->al_name);

    if (plist->al_rescln)
      {
      int len = strlen(log_buf);
      snprintf(log_buf + len, sizeof(log_buf) - len, ".%s", plist->al_resc);
      }

    if (plist->al_op == INCR)
      pstr = " + ";
    else if (plist->al_op == DECR)
      pstr = " - ";
    else
      pstr = " = ";

    safe_strncat(log_buf, pstr, sizeof(log_buf) - strlen(log_buf) - 1);

    if (plist->al_valln)
      safe_strncat(log_buf, plist->al_value, sizeof(log_buf) - strlen(log_buf) - 1);

    log_buf[LOCAL_LOG_BUF_SIZE-1] = '\0';

    log_event(PBSEVENT_ADMIN, logclass, objname, log_buf);

    plist = (struct svrattrl *)GET_NEXT(plist->al_link);
    }
  } /* END mgr_log_attr() */



int update_user_acls(

  pbs_attribute *pattr,
  enum batch_op  op)

  {
  int                   rc = PBSE_NONE;
  struct array_strings *pstr = pattr->at_val.at_arst;

  if ((op == UNSET) ||
      (op == SET))
    limited_acls.clear_users();

  if (pstr == NULL)
    {
    return(rc);
    }

  for (int i = 0; i < pstr->as_usedptr; i++)
    {
    if (op == DECR)
      limited_acls.remove_user_configuration(pstr->as_string[i]);
    else if ((op == INCR) ||
             (op == SET))
      limited_acls.add_user_configuration(pstr->as_string[i]);
    }

  return(rc);
  } // END update_user_acls()



void decrement_ident_acls(

  svrattrl *plist,
  int       which)

  {
  if (which == USER)
    limited_acls.remove_user_configuration(plist->al_value);
  else
    limited_acls.remove_group_configuration(plist->al_value);
  } // END decrement_ident_acls()



int update_group_acls(

  pbs_attribute *pattr,
  enum batch_op  op)

  {
  int rc = PBSE_NONE;
  struct array_strings *pstr = pattr->at_val.at_arst;

  if (pstr == NULL)
    {
    return(rc);
    }

  if ((op == UNSET) ||
      (op == SET))
    limited_acls.clear_groups();

  for (int i = 0; i < pstr->as_usedptr; i++)
    {
    if ((op == INCR) ||
             (op == SET))
      limited_acls.add_group_configuration(pstr->as_string[i]);
    }

  return(rc);
  }



/*
 * mgr_set_attr - set attributes for manager function
 *
 * The set operation is performed as an atomic operation: all specified
 * attributes must be successfully set, or none are modified.
 */

static int mgr_set_attr(

  pbs_attribute *pattr,  /* current attributes */
  attribute_def *pdef,
  int            limit,
  svrattrl      *plist,
  int            privil,
  int           *bad,
  void          *parent,
  int            mode)

  {
  int            index;
  pbs_attribute *new_attr;
  pbs_attribute *pnew;
  pbs_attribute *pold;
  int            rc;

  if (plist == (struct svrattrl *)0)
    {
    return(0);
    }

  new_attr = (pbs_attribute *)calloc((unsigned int)limit, sizeof(pbs_attribute));

  if (new_attr == NULL)
    {
    return(PBSE_SYSTEM);
    }

  /*
   * decode the new_attr pbs_attribute values which are in the request,
   * copy the corresponding current pbs_attribute into a holding array
   * and update it with the newly decoded value
   */

  if ((rc = attr_atomic_set(plist, pattr, new_attr, pdef, limit, -1, privil, bad)) != 0)
    {
    attr_atomic_kill(new_attr, pdef, limit);

    return(rc);
    }

  for (index = 0;index < limit;index++)
    {
    pnew = new_attr + index;

    pold = pattr + index;

    if (pnew->at_flags & ATR_VFLAG_MODIFY)
      {
      /*
       * for each pbs_attribute which is to be modified, call the
       * at_action routine for the pbs_attribute, if one exists, with the
       *  new value.  If the action fails, undo everything.
       */

      if ((pdef + index)->at_action)
        {
        if ((rc = (pdef + index)->at_action(new_attr + index, parent, mode)))
          {
          /* always allow removing from ACLs */
          if (!((plist->al_op == DECR) && (pdef + index)->at_type == ATR_TYPE_ACL))
            {
            attr_atomic_kill(new_attr, pdef, limit);

            return(rc);
            }
          }
        }

      if (pdef == svr_attr_def)
        {
        if (index == SRV_ATR_acl_users_hosts)
          {
          if (plist->al_op == DECR)
            decrement_ident_acls(plist, USER);
          else 
            update_user_acls(pnew, plist->al_op);
          }
        else if (index == SRV_ATR_acl_groups_hosts)
          {
          if (plist->al_op == DECR)
            decrement_ident_acls(plist, GROUP);
          else
            update_group_acls(pnew, plist->al_op);
          }
        else if (index == SRV_ATR_tcp_incoming_timeout)
          pbs_incoming_tcp_timeout = pnew->at_val.at_long;
        }

      /* now replace the old values with any modified new values */

      (pdef + index)->at_free(pold);

      if ((pold->at_type == ATR_TYPE_LIST) ||
          (pold->at_type == ATR_TYPE_RESC))
        {
        list_move(&pnew->at_val.at_list, &pold->at_val.at_list);
        }
      else if (pold->at_type == ATR_TYPE_ATTR_REQ_INFO)
        {
        pold->at_val.at_ptr = pnew->at_val.at_ptr;
        }
      else
        *pold = *pnew;

      pold->at_flags = pnew->at_flags; /* includes MODIFY */
      }
    }

  /*
   * we have moved all the "external" values to the old array, thus
   * we just free the new array, NOT call at_free on each.
   */

  free((char *)new_attr);

  return(0);
  }  /* END mgr_set_attr() */





/*
 * mgr_unset_attr - Unset (clear) attributes for manager function
 *
 * The set operation is performed as an atomic operation: all specified
 * attributes must be successfully set, or none are modified.
 */

int mgr_unset_attr(

  pbs_attribute *pattr,  /* current attributes */
  attribute_def *pdef,
  int            limit,
  svrattrl      *plist,
  int            privil,
  int           *bad)

  {
  int   index;
  int   ord;
  svrattrl *pl;
  resource_def *prsdef;
  resource *presc;

  /* first check the pbs_attribute exists and we have privilege to set */

  ord = 0;
  pl = plist;

  while (pl != NULL)
    {
    ord++;

    index = find_attr(pdef, pl->al_name, limit);

    if (index < 0)
      {
      *bad = ord;

      return(PBSE_NOATTR);
      }

    /* have we privilege to unset the pbs_attribute ? */

    if (((pdef + index)->at_flags & privil & ATR_DFLAG_WRACC) == 0)
      {
      *bad = ord;

      return(PBSE_ATTRRO);
      }

    if (((pdef + index)->at_type == ATR_TYPE_RESC) &&
        (pl->al_resc != NULL))
      {
      /* check the individual resource */

      prsdef = find_resc_def(
                 svr_resc_def,
                 pl->al_resc,
                 svr_resc_size);

      if (prsdef == NULL)
        {
        *bad = ord;

        return(PBSE_UNKRESC);
        }

      if ((prsdef->rs_flags & privil & ATR_DFLAG_WRACC) == 0)
        {
        *bad = ord;

        return(PBSE_PERM);
        }
      }

    pl = (svrattrl *)GET_NEXT(pl->al_link);
    }

  /* ok, now clear them */

  while (plist != NULL)
    {
    index = find_attr(pdef, plist->al_name, limit);
      
    if (pdef == svr_attr_def)
      {
      if (index == SRV_ATR_acl_users_hosts)
        update_user_acls(pattr + index, UNSET);
      else if (index == SRV_ATR_acl_groups_hosts)
        update_group_acls(pattr + index, UNSET);
      }

    if (((pdef + index)->at_type == ATR_TYPE_RESC) &&
        (plist->al_resc != NULL))
      {
      /* free resource member, not the pbs_attribute */

      prsdef = find_resc_def(
                 svr_resc_def,
                 plist->al_resc,
                 svr_resc_size);

      if ((presc = find_resc_entry(pattr + index, prsdef)))
        {
        if ((pdef->at_parent != PARENT_TYPE_SERVER) ||
            (index != SRV_ATR_resource_cost))
          {
          prsdef->rs_free(&presc->rs_value);
          }

        delete_link(&presc->rs_link);
        }

      free(presc);
      }
    else
      {
      /* free the whole pbs_attribute */

      (pdef + index)->at_free(pattr + index);

      (pattr + index)->at_flags |= ATR_VFLAG_MODIFY;
      }

    plist = (svrattrl *)GET_NEXT(plist->al_link);
    }  /* END while (plist != NULL) */

  return(0);
  }  /* END mgr_unset_attr() */





/*
 * update_subnode - update subnodes with state and props
 *
 * state information comes from the parent node
 * prop information taken from the 1st subnode
 *
 * Note, while it isn't a problem now, if the server is ever multithreaded,
 * this will have to locked out while subnodes are being allocated or
 * otherwise updated.
 */

void update_subnode(

  struct pbsnode *pnode)

  {

/*  unsigned short   state;

  state = pnode->nd_state & INUSE_COMMON_MASK;

  for (pnxtsn = pnode->nd_psn;pnxtsn;pnxtsn = pnxtsn->next)
    {
    pnxtsn->host  = pnode;

    pnxtsn->inuse = (pnxtsn->inuse & ~INUSE_COMMON_MASK) | state;
    } */

  return;
  }  /* END update_subnode() */






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

int mgr_set_node_attr(

  struct pbsnode *pnode,  /* do some kind of "set" action on this node */
  attribute_def  *pdef,   /* pbsnode doesn't have attributes but in the
                             future it might  so, make  code similar to existing */
  int             limit,  /* length of the *pdef array      */
  svrattrl       *plist,  /* batch request's svrattrl list begins here */
  int             privil, /* requester's privilege          */
  int            *bad,    /* if there is a "bad pbs_attribute" pass back 
                             position via this loc */
  void           *parent, /*may go unused in this function */
  int             mode,  /*passed to attrib's action func not used by 
                           this func at this time*/
  bool            dont_update_nodes)

  {
  int              index;
  int              rc = PBSE_NONE;
  pbs_attribute   *new_attr;
  pbs_attribute   *unused = NULL;
  pbs_attribute   *pnew;

  struct pbsnode   tnode;  /*temporary node*/

  char             log_buf[LOCAL_LOG_BUF_SIZE];

  if (plist == NULL)
    {
    return(0);  /* nothing to do, return success */
    }

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

  for (index = 0; index < limit; index++)
    {
    if ((pdef + index)->at_action)
      {
      if ((rc = (pdef + index)->at_action(new_attr + index, (void *)pnode, ATR_ACTION_NEW)))
        {
        /*call the "at_free" func for each member of*/
        /*new" and then free the entire "new" array */
        /*return code (rc) shapes caller's reply    */

        attr_atomic_kill(new_attr, pdef, limit);

        return(rc);
        }
      }
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

  /*
   * Use the updated pbs_attribute values in pbs_attribute array, "new", to update a
   * temporary (on stack) pbsnode, which is a copy of the one pointed to by
   * pnode.  If all goes well, we are home free-- just copy the information
   * in the temporary pbsnode, after it has gotten modified, back to the
   * node pointed to by "pnode" and free the pbs_attribute array "new".
   */

  tnode = *pnode;

  for (index = 0;index < limit;index++)
    {
    pnew = new_attr + index;

    if (pnew->at_flags & ATR_VFLAG_MODIFY)
      {
      /*
       * for each pbs_attribute which is to cause modification to the node, "tnode",
       * call the specified "at_action" routine for that pbs_attribute passing it
       * the address of the temporary pbs_attribute and the address of the temporary
       * pbsnode object.  If any of the update actions fail along the way, undo
       * everything.   Any calloc'd substructures should be hanging off the
       * appropriate pbs_attribute of the pbs_attribute array "new".
       */

      if ((pdef + index)->at_action)
        {
        if ((rc = (pdef + index)->at_action(new_attr + index, (void *)&tnode, ATR_ACTION_ALTER)))
          {
          attr_atomic_kill(new_attr, pdef, limit);

          return(rc);
          }
        }
      }
    }

  /*
   * we have successfully updated tnode; use it to modify pnode
   * if pnode has any calloc-ed storage that is being replaced
   * be sure to free the old.
   */

  if (pnode->nd_prop && (pnode->nd_prop != tnode.nd_prop))
    {
    if (pnode->nd_prop->as_buf)
      free(pnode->nd_prop->as_buf);

    free(pnode->nd_prop);

    pnode->nd_prop = NULL;
    }

  /* NOTE:  nd_status properly freed during pbs_attribute alter */

  if ((pnode->nd_state != tnode.nd_state))
    {
    char OrigState[1024];
    char FinalState[1024];

    if(pnode->nd_state&INUSE_NOHIERARCHY)
      {
      tnode.nd_state = pnode->nd_state;
      sprintf(log_buf,"Node state can not be changed until node list has been sent to all nodes.");
      log_err(PBSE_HIERARCHY_NOT_SENT,"State change not allowed.",log_buf);
      rc = PBSE_HIERARCHY_NOT_SENT;
      }
    else
      {

      /* changing node state */

      /* log change */

      PNodeStateToString(pnode->nd_state, OrigState, sizeof(OrigState));
      PNodeStateToString(tnode.nd_state, FinalState, sizeof(FinalState));

      sprintf(log_buf, "node %s state changed from %s to %s",
        pnode->get_name(),
        OrigState,
        FinalState);

      log_event(PBSEVENT_ADMIN, PBS_EVENTCLASS_NODE, pnode->get_name(), log_buf);
      }
    }

  /* NOTE:  nd_status properly freed during pbs_attribute alter */

  /*
    if ((pnode->nd_status != NULL) && (pnode->nd_status != tnode.nd_status))
      {
      if (pnode->nd_status->as_buf != NULL)
        free(pnode->nd_status->as_buf);

      free(pnode->nd_status);

      pnode->nd_status = NULL;
      }
  */

  *pnode              = tnode;        /* updates all data including linking in props */

  free(new_attr);  /*any new  prop list has been put on pnode*/

  /*dispense with the pbs_attribute array itself*/

  /* update prop list based on new prop array */
  pnode->update_properties();
  
  /* now update subnodes */

  update_subnode(pnode);

  return(rc);
  }  /* END mgr_set_node_attr() */




/*
 * mgr_queue_create - process request to create a queue
 *
 * Creates queue and calls mgr_set_attr to set queue attributes.
 */

void mgr_queue_create(

  struct batch_request *preq)

  {
  int        bad;
  const char      *badattr;
  svrattrl  *plist;
  pbs_queue *pque;
  int        rc;
  char       log_buf[LOCAL_LOG_BUF_SIZE];

  rc = strlen(preq->rq_ind.rq_manager.rq_objname);

  if ((rc > PBS_MAXQUEUENAME) || (rc == 0))
    {
    req_reject(PBSE_QUENBIG, 0, preq, NULL, NULL);

    return;
    }

  if ((pque = find_queuebyname(preq->rq_ind.rq_manager.rq_objname)))
    {
    unlock_queue(pque, "mgr_queue_create", (char *)"fail", LOGLEVEL);
    req_reject(PBSE_QUEEXIST, 0, preq, NULL, NULL);
    return;
    }

  pque = que_alloc(preq->rq_ind.rq_manager.rq_objname, FALSE);

  mutex_mgr que_mgr(pque->qu_mutex, true);

  /* set the queue attributes */
  plist = (svrattrl *)GET_NEXT(preq->rq_ind.rq_manager.rq_attr);

  rc = mgr_set_attr(
         pque->qu_attr,
         que_attr_def,
         QA_ATR_LAST,
         plist,
         preq->rq_perm,
         &bad,
         (void *)pque,
         ATR_ACTION_NEW);

  if (rc != 0)
    {
    reply_badattr(rc, bad, plist, preq);

    que_free(pque, FALSE);
    que_mgr.set_unlock_on_exit(false);
    }
  else
    {
    que_save(pque);
    svr_save(&server, SVR_SAVE_QUICK);

    sprintf(log_buf, msg_manager,
            msg_man_cre,
            preq->rq_user,
            preq->rq_host);

    log_event(PBSEVENT_ADMIN,PBS_EVENTCLASS_QUEUE,pque->qu_qs.qu_name,log_buf);

    mgr_log_attr(
      msg_man_set,
      plist,
      PBS_EVENTCLASS_QUEUE,
      preq->rq_ind.rq_manager.rq_objname);

    /* check the appropriateness of the attributes vs. queue type */
    if ((badattr = check_que_attr(pque)) != NULL)
      {
      /* mismatch, issue warning */
      sprintf(log_buf, msg_attrtype, pque->qu_qs.qu_name, badattr);

      reply_text(preq, PBSE_ATTRTYPE, log_buf);
      }
    else
      {
      reply_ack(preq);
      }
    }

  return;
  } /* END mgr_queue_create() */






/*
 * mgr_queue_delete - delete a queue
 *
 * The queue must be empty of jobs
 */

void mgr_queue_delete(

  struct batch_request *preq)

  {
  pbs_queue *pque;
  int        rc;
  char       log_buf[LOCAL_LOG_BUF_SIZE];

  pque = find_queuebyname(preq->rq_ind.rq_manager.rq_objname);

  if (pque == NULL)
    {
    /* FAILURE */
    req_reject(PBSE_UNKQUE, 0, preq, NULL, NULL);

    return;
    }
  mutex_mgr pque_mutex(pque->qu_mutex, true);

  if ((rc = que_purge(pque)) != PBSE_NONE)
    {
    /* FAILURE */
    pque_mutex.unlock();
    req_reject(rc, 0, preq, NULL, NULL);
    return;
    }
  pque_mutex.set_unlock_on_exit(false);

  svr_save(&server, SVR_SAVE_QUICK);

  sprintf(log_buf, msg_manager, msg_man_del, preq->rq_user, preq->rq_host);

  log_event(
    PBSEVENT_ADMIN,
    PBS_EVENTCLASS_QUEUE,
    preq->rq_ind.rq_manager.rq_objname,
    log_buf);

  reply_ack(preq);

  /* SUCCESS */

  return;
  }  /* END mgr_queue_delete() */




int hostname_check(

  char *hostname)

  {
  char  myhost[PBS_MAXHOSTNAME + 1];
  char  extension[PBS_MAXHOSTNAME + 1];
  char  ret_hostname[PBS_MAXHOSTNAME + 1];
  char *open_bracket;

  if (hostname == NULL)
    return(SUCCESS);
  else if (strchr(hostname,'*') != NULL)
    return(SUCCESS);

  snprintf(myhost, sizeof(myhost), "%s", hostname);
  open_bracket = strchr(myhost,'[');

  ret_hostname[0] = '\0';

  if (open_bracket == NULL)
    {
    /* handle normally */
    if ((get_fullhostname(hostname,ret_hostname,PBS_MAXHOSTNAME,NULL)) ||
        strncasecmp(hostname, ret_hostname, PBS_MAXHOSTNAME))
      {
      return(FAILURE);
      }
    }
  else
    {
    /* authenticate the range */
    int   low;
    int   high;
    char *dash;
    char *host_end;
    char *close_bracket = strchr(open_bracket,']');

    low = atoi(open_bracket+1);
    dash = strchr(open_bracket,'-');

    host_end = open_bracket + strlen(open_bracket);

    if ((dash == NULL) ||
        (close_bracket == NULL))
      {
      /* not a valid range */
      return(FAILURE);
      }

    high = atoi(dash+1);

    /* check for anything left over */
    extension[0] = '\0';

    if (close_bracket < host_end)
      {
      snprintf(extension, sizeof(extension), "%s", close_bracket+1);
      }

    /* now check each hostname */
    while (low < high)
      {
      sprintf(open_bracket,"%d%s",low,extension);

      if ((get_fullhostname(myhost,ret_hostname,PBS_MAXHOSTNAME,NULL)) ||
          (strncmp(myhost,ret_hostname,PBS_MAXHOSTNAME)))
        {
        return(FAILURE);
        }

      low++;
      }
    }

  return(SUCCESS);
  } /* END hostname_check() */





/*
 * mgr_server_set - Set Server Attribute Values
 *
 * Sets the requested attributes and returns a reply
 */

void mgr_server_set(

  struct batch_request *preq) /* I */

  {
  int       bad_attr = 0;
  svrattrl *plist;
  int       rc;
  char      log_buf[LOCAL_LOG_BUF_SIZE];

  plist = (svrattrl *)GET_NEXT(preq->rq_ind.rq_manager.rq_attr);

  pthread_mutex_lock(server.sv_attr_mutex);
  rc = mgr_set_attr(
         server.sv_attr,
         svr_attr_def,
         SRV_ATR_LAST,
         plist,
         preq->rq_perm,
         &bad_attr,
         (void *) & server,
         ATR_ACTION_ALTER);
  pthread_mutex_unlock(server.sv_attr_mutex);

  /* PBSE_BADACLHOST - lets show the user the first bad host in the ACL  */

  if (rc == PBSE_BADACLHOST)
    {
    char          *bad_host;
    char          *host_entry;
    pbs_attribute  temp;
    int            index;
    int            i;

    struct    array_strings *pstr;
    int       bhstrlen;

    bhstrlen = PBS_MAXHOSTNAME + 17;
    bad_host = (char *)calloc(sizeof(char), (bhstrlen + 1));

    if (bad_host == NULL)
      {
      reply_badattr(PBSE_BADHOST, bad_attr, plist, preq);

      return;
      }

    /* look into plist to find an offending attr */

    while (plist != NULL)
      {
      /* only concerned about operators and managers */

      if (strcmp(plist->al_name, ATTR_managers) && strcmp(plist->al_name, ATTR_operators))
        {
        plist = (svrattrl *)GET_NEXT(plist->al_link);

        continue;
        }

      if ((index = find_attr(svr_attr_def, plist->al_name, SRV_ATR_LAST)) < 0)
        {
        plist = (svrattrl *)GET_NEXT(plist->al_link);
        continue;
        }

      clear_attr(&temp, &svr_attr_def[index]);
      svr_attr_def[index].at_decode(&temp, plist->al_name, plist->al_resc, plist->al_value, 0);

      pstr = temp.at_val.at_arst;

      bad_host[0] = '\0';

      /* loop over all hosts in the request and perform same
         check as manager_oper_chk*/

      for (i = 0;i < pstr->as_usedptr;++i)
        {
        host_entry = strchr(pstr->as_string[i], (int)'@');
        
        if (hostname_check(host_entry+1) == FAILURE)
          {
          snprintf(bad_host, bhstrlen, "First bad host: %s",
            host_entry + 1);
        
          break;
          }
        }

      if (bad_host[0] != '\0')
        break;

      /* nothing wrong found in the request, let's try again
         with the server's list */

      pthread_mutex_lock(server.sv_attr_mutex);
      pstr = server.sv_attr[index].at_val.at_arst;

      for (i = 0; i < pstr->as_usedptr; ++i)
        {
        host_entry = strchr(pstr->as_string[i], (int)'@');

        if (hostname_check(host_entry+1) == FAILURE)
          {
          snprintf(bad_host,bhstrlen,"First bad host: %s",
            host_entry+1);

          break;
          }
        }
      pthread_mutex_unlock(server.sv_attr_mutex);

      if (bad_host[0] != '\0')
        break;

      plist = (svrattrl *)GET_NEXT(plist->al_link);
      }

    if (bad_host[0] != '\0') /* we found a fully qualified host that was bad */
      {
      req_reject(PBSE_BADACLHOST, 0, preq, NULL, bad_host);
      }
    else
      {
      /* this shouldn't happen (return PBSE_BADACLHOST, but now we can't
         find the bad host) */

      reply_badattr(PBSE_BADHOST, bad_attr, plist, preq);
      }

    free(bad_host);

    return;
    } /* END if (rc == PBSE_BADACLHOST) */
  else if (rc != 0)
    {
    reply_badattr(rc, bad_attr, plist, preq);

    return;
    }

  svr_save(&server, SVR_SAVE_FULL);

  sprintf(log_buf, msg_manager, msg_man_set, preq->rq_user, preq->rq_host);

  log_event(
    PBSEVENT_ADMIN,
    PBS_EVENTCLASS_SERVER,
    msg_daemonname,
    log_buf);

  mgr_log_attr(msg_man_set, plist, PBS_EVENTCLASS_SERVER, msg_daemonname);

  reply_ack(preq);

  return;
  }  /* END mgr_server_set() */






/*
 * mgr_server_unset - Unset (clear) Server Attribute Values
 *
 * Clears the requested attributes and returns a reply
 */

void mgr_server_unset(

  struct batch_request *preq)

  {
  int       bad_attr = 0;
  svrattrl *plist;
  int       rc;
  char      log_buf[LOCAL_LOG_BUF_SIZE];

  plist = (svrattrl *)GET_NEXT(preq->rq_ind.rq_manager.rq_attr);

  pthread_mutex_lock(server.sv_attr_mutex);
  rc = mgr_unset_attr(
         server.sv_attr,
         svr_attr_def,
         SRV_ATR_LAST,
         plist,
         preq->rq_perm,
         &bad_attr);
  pthread_mutex_unlock(server.sv_attr_mutex);

  if (rc != 0)
    {
    reply_badattr(rc, bad_attr, plist, preq);
    }
  else
    {
    svr_save(&server, SVR_SAVE_FULL);

    sprintf(log_buf, msg_manager, msg_man_uns, preq->rq_user, preq->rq_host);

    log_event(PBSEVENT_ADMIN,PBS_EVENTCLASS_SERVER,msg_daemonname,log_buf);

    mgr_log_attr(msg_man_uns, plist, PBS_EVENTCLASS_SERVER, msg_daemonname);

    reply_ack(preq);
    }

  return;
  }  /* END mgr_server_unset() */





/*
 * mgr_queue_set - Set Queue Attribute Values
 *
 * Finds the queue, Sets the requested attributes and returns a reply
 */

void mgr_queue_set(

  struct batch_request *preq)

  {
  int        allques;
  int        bad = 0;
  const char  *badattr;
  svrattrl  *plist;
  pbs_queue *pque;
  const char      *qname;
  int        rc;
  all_queues_iterator  *iter = NULL;
  char       log_buf[LOCAL_LOG_BUF_SIZE];

  if (LOGLEVEL >= 7)
    {
    sprintf(log_buf, "from user %s", preq->rq_user);
    log_event(PBSEVENT_SYSTEM, PBS_EVENTCLASS_QUEUE, __func__, log_buf);
    }

  svr_queues.lock();
  iter = svr_queues.get_iterator();
  svr_queues.unlock();

  if ((*preq->rq_ind.rq_manager.rq_objname == '\0') ||
      (*preq->rq_ind.rq_manager.rq_objname == '@'))
    {
    qname   = all_quename;
    allques = TRUE;
    pque = next_queue(&svr_queues,iter);
    }
  else
    {
    qname   = preq->rq_ind.rq_manager.rq_objname;
    allques = FALSE;

    pque = find_queuebyname(qname);
    }

  if (pque == NULL)
    {
    req_reject(PBSE_UNKQUE, 0, preq, NULL, NULL);

    delete iter;

    return;
    }
  /* set the attributes */
  sprintf(log_buf, msg_manager, msg_man_set, preq->rq_user, preq->rq_host);

  log_event(PBSEVENT_ADMIN, PBS_EVENTCLASS_QUEUE, qname, log_buf);

  plist = (svrattrl *)GET_NEXT(preq->rq_ind.rq_manager.rq_attr);

  while (pque != NULL)
    {
      {
      mutex_mgr que_mutex(pque->qu_mutex, true);

      rc = mgr_set_attr(
             pque->qu_attr,
             que_attr_def,
             QA_ATR_LAST,
             plist,
             preq->rq_perm,
             &bad,
             (void *)pque,
             ATR_ACTION_ALTER);

      if (rc != 0)
        {
        que_mutex.unlock();
        reply_badattr(rc, bad, plist, preq);

        delete iter;

        return;
        }

      que_save(pque);

      mgr_log_attr(msg_man_set, plist, PBS_EVENTCLASS_QUEUE, pque->qu_qs.qu_name);

      if (allques == FALSE)
        break;

      }

    pque = next_queue(&svr_queues,iter);
    }  /* END while (pque != NULL) */

  /* check the appropriateness of the attributes based on queue type */
  svr_queues.lock();

  delete iter;

  iter = svr_queues.get_iterator();
  svr_queues.unlock();

  if (allques == TRUE)
    pque = next_queue(&svr_queues,iter);

  while (pque != NULL)
    {
      {
      mutex_mgr que_mutex(pque->qu_mutex, true);
      if ((badattr = check_que_attr(pque)) != NULL)
        {
        sprintf(log_buf, msg_attrtype, pque->qu_qs.qu_name, badattr);
        que_mutex.unlock();
        reply_text(preq, PBSE_ATTRTYPE, log_buf);
        delete iter;
        return;
        }

      if (allques == FALSE)
        break;

      }
    pque = next_queue(&svr_queues,iter);
    }  /* END while (pque != NULL) */

  if (iter != NULL)
    delete iter;

  reply_ack(preq);

  return;
  }  /* END mgr_queue_set() */





/*
 * mgr_queue_unset - Unset (clear)  Queue Attribute Values
 *
 * Finds the queue, clears the requested attributes and returns a reply
 */

void mgr_queue_unset(

  struct batch_request *preq)

  {
  int    allques;
  int    bad_attr = 0;
  svrattrl  *plist;
  pbs_queue *pque;
  const char      *qname;
  int    rc;
  all_queues_iterator *iter = NULL;
  char   log_buf[LOCAL_LOG_BUF_SIZE];

  if (LOGLEVEL >= 7)
    {
    sprintf(log_buf, "from user %s", preq->rq_user);
    log_event(PBSEVENT_SYSTEM, PBS_EVENTCLASS_QUEUE, __func__, log_buf);
    }

  svr_queues.lock();
  iter = svr_queues.get_iterator();
  svr_queues.unlock();
  
  if ((*preq->rq_ind.rq_manager.rq_objname == '\0') ||
      (*preq->rq_ind.rq_manager.rq_objname == '@'))
    {
    qname   = all_quename;
    allques = TRUE;
    pque = next_queue(&svr_queues,iter);
    }
  else
    {
    allques = FALSE;
    qname   = preq->rq_ind.rq_manager.rq_objname;

    pque = find_queuebyname(qname);
    }

  if (pque == NULL)
    {
    req_reject(PBSE_UNKQUE, 0, preq, NULL, NULL);
    delete iter;

    return;
    }

  sprintf(log_buf, msg_manager, msg_man_uns, preq->rq_user, preq->rq_host);
  log_event(PBSEVENT_ADMIN,PBS_EVENTCLASS_QUEUE,qname,log_buf);

  plist = (svrattrl *)GET_NEXT(preq->rq_ind.rq_manager.rq_attr);

  while (pque != NULL)
    {
      {
      mutex_mgr que_mutex(pque->qu_mutex, true);

      rc = mgr_unset_attr(pque->qu_attr, que_attr_def, QA_ATR_LAST, plist, preq->rq_perm, &bad_attr);

      if (rc != 0)
        {
        que_mutex.unlock();
        reply_badattr(rc, bad_attr, plist, preq);
        delete iter;
        return;
        }

      que_save(pque);

      mgr_log_attr(msg_man_uns, plist, PBS_EVENTCLASS_QUEUE, pque->qu_qs.qu_name);

      if ((pque->qu_attr[QA_ATR_QType].at_flags & ATR_VFLAG_SET) == 0)
        pque->qu_qs.qu_type = QTYPE_Unset;
      }
    if (allques == FALSE)
      break;
    
    pque = next_queue(&svr_queues,iter);
    }

  delete iter;

  reply_ack(preq);

  return;
  } /* END mgr_queue_unset() */





/*
 * mgr_node_set - Set node "state" or "properties"
 *
 * Finds the node, Sets the requested state flags, properties, or type and returns
 * a reply to the sender of the batch_request
 */

void mgr_node_set(

  struct batch_request *preq)  /* I */

  {
  int               need_todo = 0;

  int               check_all = 0;
  int               propnodes = 0;
  int               rc;
  int               bad = 0;
  int               i;
  int               len;
  int               problem_cnt = 0;

  const char       *nodename = NULL;
  char             *problem_names;
  char              log_buf[LOCAL_LOG_BUF_SIZE];

  svrattrl         *plist;
  node_check_info   nci;
  node_iterator     iter;

  struct pbsnode   *pnode = NULL;
  struct pbsnode  **problem_nodes = NULL;
  prop              props;
  bool              dont_update_nodes = false;

  get_svr_attr_b(SRV_ATR_DontWriteNodesFile, &dont_update_nodes);

  if ((strcmp(preq->rq_ind.rq_manager.rq_objname, "all") == 0) ||
      (strcmp(preq->rq_ind.rq_manager.rq_objname, "ALL") == 0))
    strcpy(preq->rq_ind.rq_manager.rq_objname, ":ALL");

  if ((*preq->rq_ind.rq_manager.rq_objname == '\0') ||
      (*preq->rq_ind.rq_manager.rq_objname == '@') ||
      ((*preq->rq_ind.rq_manager.rq_objname == ':') &&
       (*(preq->rq_ind.rq_manager.rq_objname + 1) != '\0')))
    {
    /*In this instance the set node req is to apply to all */
    /*nodes at the local ('\0')  or specified ('@') server */

    if (svr_totnodes)
      {
      pnode = (struct pbsnode *)1;

      if ((*preq->rq_ind.rq_manager.rq_objname == ':') &&
          (strcmp(preq->rq_ind.rq_manager.rq_objname + 1, "ALL") != 0))
        {
        propnodes = 1;
        nodename = preq->rq_ind.rq_manager.rq_objname;
        props.name = nodename + 1;
        props.mark = 1;
        }
      else
        {
        check_all = 1;
        nodename = all_nodes_str;
        }
      }
    }
  else
    {
    nodename = preq->rq_ind.rq_manager.rq_objname;
    pnode = find_nodebyname(nodename);
    }

  if (pnode == NULL)
    {
    req_reject(PBSE_UNKNODE, 0, preq, NULL, NULL);

    return;
    }

  /*set "state", "properties", or type of node (nodes if check_all == 1)*/

  sprintf(log_buf, msg_manager, msg_man_set, preq->rq_user, preq->rq_host);

  log_event(PBSEVENT_ADMIN, PBS_EVENTCLASS_NODE, nodename, log_buf);

  plist = (svrattrl *)GET_NEXT(preq->rq_ind.rq_manager.rq_attr);

  if (check_all || propnodes)
    {
    /* handle scrolling over all nodes */
    problem_nodes = (struct pbsnode **)calloc(svr_totnodes, sizeof(struct pbsnode *));

    problem_cnt = 0;
  
    reinitialize_node_iterator(&iter);
    pnode = NULL;
    std::vector<prop> prop_list;
    prop_list.push_back(props);

    while ((pnode = next_node(&allnodes,pnode,&iter)) != NULL)
      {
      if ((propnodes == TRUE) && 
          (!pnode->hasprop(&prop_list)))
        {
        continue;
        }

      save_characteristic(pnode,&nci);

      rc = mgr_set_node_attr(
             pnode,
             node_attr_def,
             ND_ATR_LAST,
             plist,
             preq->rq_perm,
             &bad,
             (void *)pnode,
             ATR_ACTION_ALTER,
             dont_update_nodes);

      if (rc != 0)
        {
        if (problem_nodes) /* we have an array in which to save*/
          problem_nodes[problem_cnt] = pnode;
        
        ++problem_cnt;
        }
      else
        {
        /* modifications succeeded for this node */
        chk_characteristic(pnode, &nci, &need_todo);

        mgr_log_attr(msg_man_set, plist, PBS_EVENTCLASS_NODE, pnode->get_name());
        }
      }  /* END for each node */

    if (iter.node_index != NULL)
      delete iter.node_index;

    if (iter.alps_index != NULL)
      delete iter.alps_index;

    } /* END multiple node case */
  else
    {
    /* handle single node case */
    save_characteristic(pnode,&nci);

    rc = mgr_set_node_attr(
           pnode,
           node_attr_def,
           ND_ATR_LAST,
           plist,
           preq->rq_perm,
           &bad,
           (void *)pnode,
           ATR_ACTION_ALTER,
           dont_update_nodes);

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

      pnode->unlock_node(__func__, "error", LOGLEVEL);
      
      return;
      } /* END if (rc != 0) */ 
    else
      {
      /* modifications succeeded for this node */
      chk_characteristic(pnode, &nci, &need_todo);
      
      mgr_log_attr(msg_man_set, plist, PBS_EVENTCLASS_NODE, pnode->get_name());
      }

    pnode->unlock_node(__func__, "single_node", LOGLEVEL);
    } /* END single node case */

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

  if (check_all || propnodes)
    {
    /* modification was for all nodes */
    if (problem_cnt)
      {
      /* one or more problems encountered */

      for (len = 0, i = 0;i < problem_cnt;i++)
        len += strlen(problem_nodes[i]->get_name()) + 3;

      len += strlen(pbse_to_txt(PBSE_GMODERR));

      if ((problem_names = (char *)calloc(1, len)))
        {
        strcpy(problem_names, pbse_to_txt(PBSE_GMODERR));

        for (i = 0;i < problem_cnt;i++)
          {
          if (i)
            strcat(problem_names, ", ");

          strcat(problem_names, problem_nodes[i]->get_name());
          }

        reply_text(preq, PBSE_GMODERR, problem_names);

        free(problem_names);
        }
      else
        {
        reply_text(preq, PBSE_GMODERR, pbse_to_txt(PBSE_GMODERR));
        }
      }

    if (problem_nodes != NULL)
      free(problem_nodes);

    if (problem_cnt)
      {
      /* reply has already been sent */

      recompute_ntype_cnts();

      return;
      }
    }    /* END if (check_all || propnodes) */

  recompute_ntype_cnts();

  reply_ack(preq);  /*request completely successful*/

  return;
  }  /* END void mgr_node_set() */


static bool wait_for_job_state(int jobid,int newState,int timeout)
  {
  time_t end = time(NULL) + timeout;
  while(end > time(NULL))
    {
    job *pjob = svr_find_job_by_id(jobid);
    if(pjob == NULL) return true; //the job is gone.
    int jobState = pjob->ji_qs.ji_state;
    unlock_ji_mutex(pjob,__func__,NULL,LOGLEVEL);
    if(jobState == newState) return true; //The job has been deleted from the MOM
    sleep(2);
    }
  return false; //We timed out trying to delete the job from the MOM.
  }



static bool requeue_or_delete_jobs(
    
  struct pbsnode *pnode,
  batch_request  *preq)

  {
  std::vector<int> jids;
  bool             requeue_rc = true;
  long             delete_timeout = TIMEOUT_FOR_JOB_DEL_REQ;
  long             requeue_timeout = TIMEOUT_FOR_JOB_DEL_REQ;

  for(std::vector<job_usage_info>::iterator i = pnode->nd_job_usages.begin();i != pnode->nd_job_usages.end();i++)
    {
    jids.push_back(i->internal_job_id);
    }
  for(std::vector<int>::iterator jid = jids.begin();jid != jids.end();jid++)
    {
    pnode->tmp_unlock_node(__func__, NULL, LOGLEVEL);
    job *pjob = svr_find_job_by_id(*jid);
    pnode->tmp_lock_node(__func__, NULL, LOGLEVEL);

    if(pjob != NULL)
      {
      char *dup_jobid = strdup(pjob->ji_qs.ji_jobid);
      batch_request *brRerun = alloc_br(PBS_BATCH_Rerun);
      batch_request *brDelete = alloc_br(PBS_BATCH_DeleteJob);
      if((brRerun == NULL)||(brDelete == NULL))
        {
        free_br(brRerun);
        free_br(brDelete);
        free(dup_jobid);
        return true; //Ignoring this error.
        }
      strcpy(brRerun->rq_ind.rq_rerun,pjob->ji_qs.ji_jobid);
      strcpy(brDelete->rq_ind.rq_delete.rq_objname,pjob->ji_qs.ji_jobid);
      brRerun->rq_conn = PBS_LOCAL_CONNECTION;
      brDelete->rq_conn = PBS_LOCAL_CONNECTION;
      brRerun->rq_perm = preq->rq_perm;
      brDelete->rq_perm = preq->rq_perm;
      brDelete->rq_ind.rq_delete.rq_objtype = MGR_OBJ_JOB;
      brDelete->rq_ind.rq_delete.rq_cmd = MGR_CMD_DELETE;
      unlock_ji_mutex(pjob,__func__,NULL,LOGLEVEL);
      pnode->tmp_unlock_node(__func__, NULL, LOGLEVEL);
      int rc = req_rerunjob(brRerun);
      if(rc != PBSE_NONE)
        {
        rc = req_deletejob(brDelete);
        if(rc == PBSE_NONE)
          {
          get_svr_attr_l(SRV_ATR_TimeoutForJobDelete, &delete_timeout);

          if(!wait_for_job_state(*jid,JOB_STATE_COMPLETE,delete_timeout))
            {
            set_task(WORK_Immed, 0, ensure_deleted, dup_jobid, FALSE);
            dup_jobid = NULL;
            requeue_rc = false; //Timing out with the MOMs is the only error we care about.
            }
          }
        }
      else
        {
        free_br(brDelete);
        get_svr_attr_l(SRV_ATR_TimeoutForJobRequeue, &requeue_timeout);

        if(!wait_for_job_state(*jid,JOB_STATE_QUEUED,requeue_timeout))
          {
          set_task(WORK_Immed, 0, ensure_deleted, dup_jobid, FALSE);
          dup_jobid = NULL;
          requeue_rc = false; //Timing out with the MOMs is the only error we care about.
          }
        }
      pnode->tmp_lock_node(__func__, NULL, LOGLEVEL);
      if(dup_jobid != NULL)
        {
        free(dup_jobid);
        }
      }
    }
  return requeue_rc;
  }



/*
 * mgr_node_delete - mark a node (or all nodes) in the server's node list
 *                   as being "deleted".  It (they) will no longer get
 *                   assigned to a job, will no longer be pinged, and
 *                   any current job tasks will continue until they end,
 *                   abort, or are killed.
 *
 */

static void mgr_node_delete(

  struct batch_request *preq)

  {
  int             check_all = 0;
  all_nodes_iterator *iter = NULL;

  struct pbsnode *pnode;
  const char     *nodename = NULL;


  svrattrl       *plist;

  char            log_buf[LOCAL_LOG_BUF_SIZE];
  bool            dont_update_nodes = false;

  get_svr_attr_b(SRV_ATR_DontWriteNodesFile, &dont_update_nodes);
  if (dont_update_nodes == true)
    {
    req_reject(PBSE_CANT_EDIT_NODES, 0, preq, NULL, NULL);
    return;
    }

  if ((*preq->rq_ind.rq_manager.rq_objname == '\0') ||
      (*preq->rq_ind.rq_manager.rq_objname == '@'))
    {
    /* In this instance the set node req is to apply to all */
    /* nodes at the local ('\0')  or specified ('@') server */

    if (svr_totnodes)
      {
      nodename = all_nodes_str;
      pnode = (struct pbsnode *)1;
      check_all = 1;
      }
    else
      {
      /* specified server has no nodes in its node table */
      pnode = NULL;
      }
    }
  else
    {
    nodename = preq->rq_ind.rq_manager.rq_objname;

    pnode = find_nodebyname(nodename);
    }

  if (pnode == NULL)
    {
    req_reject(PBSE_UNKNODE, 0, preq, NULL, NULL);

    return;
    }

  //Requeue any jobs running on nodes that are about to be deleted.

  bool requeue_failed = false;
  if (check_all)
    {
    iter = NULL;

    while ((pnode = next_host(&allnodes,&iter,NULL)) != NULL)
      {
      if(!requeue_or_delete_jobs(pnode,preq))
        {
        requeue_failed = true;
        }
      }
    if(iter != NULL)
      {
      delete iter;
      iter = NULL;
      }
    }
  else
    {
    if(!requeue_or_delete_jobs(pnode,preq))
      {
      requeue_failed = true;
      }
    }

  sprintf(log_buf, msg_manager, msg_man_del, preq->rq_user, preq->rq_host);

  log_event(PBSEVENT_ADMIN, PBS_EVENTCLASS_NODE, nodename, log_buf);

  plist = (svrattrl *)GET_NEXT(preq->rq_ind.rq_manager.rq_attr);

  /*if doing many and problem arises with some, record them for report*/
  /*the array of "problem nodes" sees no use now and may never see use*/

  if (check_all)
    {
    /* handle all nodes */
    iter = NULL;

    while ((pnode = next_host(&allnodes,&iter,NULL)) != NULL)
      {
      snprintf(log_buf, sizeof(log_buf), "%s", pnode->get_name());

      effective_node_delete(&pnode);

      mgr_log_attr(msg_man_set, plist, PBS_EVENTCLASS_NODE, log_buf);
      } /* end loop ( all nodes ) */

    if (iter != NULL)
      delete iter;
    }
  else
    {
    /* handle single nodes */
    snprintf(log_buf, sizeof(log_buf), "%s", pnode->get_name());

    effective_node_delete(&pnode);

    mgr_log_attr(msg_man_set, plist, PBS_EVENTCLASS_NODE, log_buf);
    }

  /*set "deleted" bit in node's (nodes, check_all == 1) "inuse" field*/
  /*remove entire prop list, including the node name, from the node */
  /*remove the IP address array hanging from the node               */

  /* update the nodes file and node state since we deleted node(s) */
  write_node_state();
  write_node_power_state();
  update_nodes_file(NULL);

  recompute_ntype_cnts();

  if(requeue_failed)
    {
    //The requeue or delete failed, tell the user.
    req_reject(PBSE_MOM_TIMED_OUT_ON_REQUEUE, 0, preq, NULL, NULL);
    }
  else
    {
    reply_ack(preq);  /*request completely successful*/
    }

  hierarchy_handler.reloadHierarchy();

  return;
  }  /* END mgr_node_delete() */





/*
 * mgr_node_create - process request to create a node
 *
 * Creates pbsnode and calls mgr_set_node_attr to set
 *      any associated node attributes also specified in the
 *      request.
 */

void mgr_node_create(

  struct batch_request *preq)

  {
  int       bad;
  svrattrl *plist;
  int       rc;
  bool      dont_update_nodes = false;

  get_svr_attr_b(SRV_ATR_DontWriteNodesFile, &dont_update_nodes);
  if (dont_update_nodes == true)
    {
    req_reject(PBSE_CANT_EDIT_NODES, 0, preq, NULL, NULL);
    return;
    }

  plist = (svrattrl *)GET_NEXT(preq->rq_ind.rq_manager.rq_attr);

  rc = create_pbs_dynamic_node(
         preq->rq_ind.rq_manager.rq_objname,
         plist,
         preq->rq_perm,
         &bad);

  if (rc != 0)
    {
    if(LOGLEVEL >= 7)
      {
      log_err(rc,__func__,"Error creating a node.");
      }
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

    return;
    }

  if(LOGLEVEL >= 7)
    {
    log_event(PBSEVENT_SYSTEM, PBS_EVENTCLASS_SERVER, __func__, "Created a node.");
    }


  mgr_log_attr(
    msg_man_set,
    plist,
    PBS_EVENTCLASS_NODE,
    preq->rq_ind.rq_manager.rq_objname);

  setup_notification(preq->rq_ind.rq_manager.rq_objname);     /* set mechanism for notifying */
  /* other nodes of new member   */

  if (update_nodes_file(NULL))
    {
    /* if update fails now (odd) */

    svr_chngNodesfile = 1;  /* try it when server shutsdown */
    }
  else
    {
    svr_chngNodesfile = 0;
    }
  hierarchy_handler.reloadHierarchy();
  reply_ack(preq);     /* create request successful */

  return;
  }



/*
 * req_manager - the dispatch routine for a series of functions which
 * implement the Manager (qmgr) Batch Request
 *
 * The privilege of the requester is checked against the type of
 * the object and the operation to be performed on it.  Then the
 * appropriate function is called to perform the operation.
 */

int req_manager(
    struct batch_request *preq)
  {
  int rc = PBSE_NONE;
  char log_buf[LOCAL_LOG_BUF_SIZE+1];

  switch (preq->rq_ind.rq_manager.rq_cmd)
    {

    case MGR_CMD_CREATE:

    case MGR_CMD_DELETE:

      if ((preq->rq_perm & PERM_MANAGER) == 0)
        {
        rc = PBSE_PERM;
        snprintf(log_buf, LOCAL_LOG_BUF_SIZE,
            "error in permissions (PERM_MANAGER)");
        req_reject(rc, 0, preq, NULL, log_buf);
        return rc;
        }

      switch (preq->rq_ind.rq_manager.rq_objtype)
        {

        case MGR_OBJ_QUEUE:

          if (preq->rq_ind.rq_manager.rq_cmd == MGR_CMD_CREATE)
            mgr_queue_create(preq);
          else
            mgr_queue_delete(preq);

          break;

        case MGR_OBJ_NODE:

          if(hierarchy_handler.isMomHierarchyFilePresent())
            {
            rc = PBSE_CREATE_NOT_ALLOWED_WITH_MOM_HIERARCHY;
            snprintf(log_buf, LOCAL_LOG_BUF_SIZE,
                (preq->rq_ind.rq_manager.rq_cmd == MGR_CMD_DELETE)?"Can not delete node.":"Can not create node" );
            req_reject(rc, 0, preq, NULL, log_buf);
            return rc;
            }

          if (preq->rq_ind.rq_manager.rq_cmd == MGR_CMD_CREATE)
            mgr_node_create(preq);
          else
            mgr_node_delete(preq);

          break;

        default:

          rc = PBSE_IVALREQ;
          snprintf(log_buf, LOCAL_LOG_BUF_SIZE, "Invalid object type");
          req_reject(rc, 0, preq, NULL, log_buf);
          return rc;
          break;
        }

      break;

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

        case MGR_OBJ_SERVER:

          mgr_server_set(preq);

          break;

        case MGR_OBJ_QUEUE:

          mgr_queue_set(preq);

          break;

        case MGR_OBJ_NODE:

          mgr_node_set(preq);

          break;

        default:

          req_reject(PBSE_IVALREQ, 0, preq, NULL, NULL);

          break;
        }

      break;

    case MGR_CMD_UNSET:

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

        case MGR_OBJ_SERVER:

          mgr_server_unset(preq);

          break;

        case MGR_OBJ_QUEUE:

          mgr_queue_unset(preq);

          break;

#if 0

          /* "unsetting" nodes isn't currently supported */

        case MGR_OBJ_NODE:

          mgr_node_unset(preq);

          break;

#endif /* 0 */

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
  }  /* END req_manager() */



/*
 * manager_oper_chk - check the @host part of a manager or operator acl
 * entry to insure it is fully qualified.  This is to prevent
 * input errors when setting the list.
 * This is the at_action() routine for the server attributes
 * "managers" and "operators"
 */

int manager_oper_chk(

  pbs_attribute *pattr,
  void          *pobject,
  int            actmode)

  {
  char   *entry;
  int    err = 0;
  int      i;

  struct array_strings *pstr;
  char                  log_buf[LOCAL_LOG_BUF_SIZE];

  if (actmode == ATR_ACTION_FREE)
    {
    return(0); /* no checking on free */
    }

  pstr = pattr->at_val.at_arst;

  if (pstr == NULL)
    {
    return(0);
    }

  for (i = 0;i < pstr->as_usedptr;++i)
    {
    entry = strchr(pstr->as_string[i], (int)'@');

    if (entry == NULL)
      {
      err = PBSE_BADHOST;

      break;
      }

    entry++; /* point after the '@' */

    if (hostname_check(entry) == FAILURE)
      {
      if (actmode == ATR_ACTION_RECOV)
        {
        sprintf(log_buf, "bad entry in acl: %s",
          pstr->as_string[i]);

        log_err(PBSE_BADACLHOST, "manager_oper_chk", log_buf);
        }
      else
        {
        sprintf(log_buf, "bad entry in acl: %s", pstr->as_string[i]);

        log_err(PBSE_BADACLHOST, "manager_oper_chk", log_buf);

        err = PBSE_BADACLHOST;
        }
      }
    else
      {
      err = PBSE_NONE;
      }
    }

  return(err);
  }  /* END manager_oper_chk() */

/*
 * servername_chk - insure it is fully qualified.
 * This is the at_action() routine for the server attribute
 * "server_name"
 */

int servername_chk(

  pbs_attribute *pattr,   /* I */
  void          *pobject, /* I */
  int           actmode) /* I */

  {
  int    err = 0;
  char    hostname[PBS_MAXSERVERNAME + 1];
  char *pstr;

  extern char              server_name[];
  char         log_buf[LOCAL_LOG_BUF_SIZE];

  if (actmode == ATR_ACTION_FREE)
    {
    if ((gethostname(hostname, PBS_MAXHOSTNAME) == -1) ||
        (get_fullhostname(hostname, hostname, PBS_MAXHOSTNAME, NULL) == -1))
      {
      return(PBSE_BADHOST);
      }

    strcpy(server_name, hostname);

    return(0);
    }

  pstr = pattr->at_val.at_str;

  if (pstr == NULL)
    {
    return(PBSE_BADHOST);
    }

  /* must be fully qualified host */

  if (get_fullhostname(pstr, hostname, PBS_MAXHOSTNAME, NULL) ||
      strncmp(pstr, hostname, PBS_MAXHOSTNAME))
    {
    if (actmode == ATR_ACTION_RECOV)
      {
      sprintf(log_buf, "bad servername: %s", pstr);

      log_err(PBSE_BADHOST,"servername_chk",log_buf);
      }
    else
      {
      err = PBSE_BADHOST;
      }
    }
  else
    {
    strcpy(server_name, hostname);
    }

  return(err);
  }  /* END server_name_chk() */


/*
 * extra_resc_chk() called when extra_resc server pbs_attribute is changed
 *     It's purpose is to re-init the resource definitions by calling
 *     init_resc_defs().  Unfortunately, it gets call before the
 *     change is actually committed; so we setup a work task to call
 *     it async asap.
 */
void on_extra_resc(

  struct work_task *ptask)

  {
  init_resc_defs();

  free(ptask->wt_mutex);
  free(ptask);
  }

int extra_resc_chk(

  pbs_attribute *pattr,   /* I */
  void          *pobject, /* I */
  int            actmode) /* I */

  {
  /* Is there anything to validate?  Maybe check for all alphanum? */
  /* the new resource is at pattr->at_val.at_str */
  set_task(WORK_Immed, 0, on_extra_resc, (char *)NULL, FALSE);

  return(TRUE);
  }

/*
 * free_extraresc() makes sure that the init_resc_defs() is called after
 * the list has changed by 'unset'.
 */
void free_extraresc(

  pbs_attribute *attr)

  {
  set_task(WORK_Immed, 0, on_extra_resc, (char *)NULL, FALSE);

  free_arst(attr);
  }


/*
 * disallowed_types_chk -
 *      This is the at_action() routine for the queue disallowed_type pbs_attribute
 */

int disallowed_types_chk(

  pbs_attribute *pattr,
  void          *pobject,
  int            actmode)

  {
  int i;
  int j;
  int found;

  struct array_strings *pstr;
  extern char *array_disallowed_types[];

  if (actmode == ATR_ACTION_FREE)
    {
    return(0);  /* no checking on free or DECR */
    }

  pstr = pattr->at_val.at_arst;

  if (pstr == NULL)
    {
    return(0);
    }

  for (i = 0;i < pstr->as_usedptr;i++)
    {
    /* first check for dupe */
    for (j = 0; j < pstr->as_usedptr;j++)
      {
      if (i == j)
        continue;

      if (strcmp(pstr->as_string[i], pstr->as_string[j]) == 0)
        {
        return PBSE_DUPLIST;
        }
      }

    found = FALSE;

    for (j = 0; (strcmp(array_disallowed_types[j], "_END_") != 0); j++)
      {
      if (strcmp(pstr->as_string[i], array_disallowed_types[j]) == 0)
        {
        found = TRUE;
        break;
        }
      }

    if (found == FALSE)
      {
      return(PBSE_BADDISALLOWTYPE);
      }
    }

  return(0);
  } /* END disallowed_types_chk() */


static int mgr_long_action_helper(

  pbs_attribute *pattr,
  int            actmode,
  long           minimum,
  long           maximum)

  {
  int err = PBSE_NONE;

  if (pattr->at_val.at_long < minimum)
    {
    err = PBSE_BADATVAL;
    }

  if (pattr->at_val.at_long > maximum)
    {
    err = PBSE_EXLIMIT;
    }

  if (err && (actmode == ATR_ACTION_RECOV))
    {
    pattr->at_val.at_long = maximum;
    err = PBSE_NONE;
    }

  return(PBSE_NONE);
  }  /* END mgr_long_action_helper() */





/*
 * schiter_chk - make sure values are sane
 * This is the at_action() routine for the server pbs_attribute
 * "scheduler_iteration"
 */

int schiter_chk(

  pbs_attribute *pattr,
  void          *pobject,
  int            actmode)

  {
  return(mgr_long_action_helper(pattr, actmode, 1, PBS_SCHEDULE_CYCLE));
  }  /* END schiter_chk() */



/* nextjobnum_action - makes sure value is sane (>=0)
   Note that there must be special code in svr_recov
   to prevent the pbs_attribute value from clobbering
   the internal counter server.sv_qs.sv_jobidnumber.
 */
int nextjobnum_chk(

  pbs_attribute *pattr,
  void          *pobject,
  int            actmode)

  {
  if (pattr->at_val.at_long > PBS_SEQNUMTOP)
    {
    return(PBSE_EXLIMIT);
    }
  else if (pattr->at_val.at_long >= 0)
    {
    return(PBSE_NONE);
    }
  else
    {
    return(PBSE_BADATVAL);
    }
  }

int set_nextjobnum(

  pbs_attribute *attr,
  pbs_attribute *new_attr,
  enum batch_op  op)

  {
  char log_buf[LOCAL_LOG_BUF_SIZE + 1];
  /* this is almost identical to set_l, but we need to grab the
     current value of server.sv_qs.sv_jobidnumber before we INCR/DECR the value of
     attr->at_val.at_long.  In fact, it probably should be moved to Libattr/ */

  /* The special handling of INCR is to allow setting of a job number range.
   * If the job numbers are already in the range, it does not alter the number.
   * Otherwise, if the number is in the default range, it sets it to the new range.
   */

  lock_sv_qs_mutex(server.sv_qs_mutex, __func__);
  switch (op)
    {

    case SET:
    
      attr->at_val.at_long = new_attr->at_val.at_long;
    
      break;

      /* Code in Moab depends on this, do not change until the Moab code is fixed. */

    case INCR:


      attr->at_val.at_long = MAX(server.sv_qs.sv_jobidnumber, new_attr->at_val.at_long);
      
      break;

    case DECR:
      
      attr->at_val.at_long -= new_attr->at_val.at_long;
      
      break;

    default:
     
      sprintf(log_buf, "%s:1", __func__);
      unlock_sv_qs_mutex(server.sv_qs_mutex, log_buf);
      return(PBSE_SYSTEM);
    }

  attr->at_flags |= ATR_VFLAG_SET | ATR_VFLAG_MODIFY;

  server.sv_qs.sv_jobidnumber = attr->at_val.at_long;

  sprintf(log_buf, "%s:2", __func__);
  unlock_sv_qs_mutex(server.sv_qs_mutex, log_buf);

  return(PBSE_NONE);
  } /* END set_nextjobnum() */

/* END req_manager.c */

