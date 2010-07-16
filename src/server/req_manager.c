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
 * disallowed_types_chk() - at action for queue disallowed_types attribute
 */
#include <pbs_config.h>   /* the master config generated by configure */

#include <sys/types.h>
#include "libpbs.h"
#include <ctype.h>
#include <memory.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include "server_limits.h"
#include "list_link.h"
#include "attribute.h"
#include "resource.h"
#include "server.h"
#include "queue.h"
#include "credential.h"
#include "batch_request.h"
#include "net_connect.h"
#include "pbs_error.h"
#include "log.h"
#include "svrfunc.h"
#include "pbs_job.h"
#include "pbs_nodes.h"
#include "work_task.h"
#include "mcom.h"


#define PERM_MANAGER (ATR_DFLAG_MGWR | ATR_DFLAG_MGRD)
#define PERM_OPorMGR (ATR_DFLAG_MGWR | ATR_DFLAG_MGRD | ATR_DFLAG_OPRD | ATR_DFLAG_OPWR)

/* Global Data Items: */

extern struct server server;
extern tlist_head     svr_queues;
extern attribute_def que_attr_def[];
extern attribute_def svr_attr_def[];
extern attribute_def  node_attr_def[];   /* node attributes defs */
extern int  svr_chngNodesfile;
extern char *msg_attrtype;
extern char *msg_daemonname;
extern char *msg_manager;
extern char *msg_man_cre;
extern char *msg_man_del;
extern char *msg_man_set;
extern char *msg_man_uns;


extern int que_purge(pbs_queue *);
extern void save_characteristic(struct pbsnode *);
extern int chk_characteristic(struct pbsnode *, int *);
extern int hasprop(struct pbsnode *, struct prop *);
extern int PNodeStateToString(int, char *, int);



/* private data */

static char *all_quename = "_All_";
static char *all_nodes = "_All_";

/*
 * check_que_attr - check if attributes in request are consistant with
 * the current queue type.  This is called when creating or setting
 * the attributes of a queue.
 *
 * Returns: NULL if all ok or name of bad attribute if not ok
 */

static char *
check_que_attr(pbs_queue *pque)
  {
  int   i;
  int   type;

  type = pque->qu_qs.qu_type;  /* current type of queue */

  for (i = 0; i < (int)QA_ATR_LAST; ++i)
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
  }

/*
 * check_que_enable - check if attempt to enable incompletely defined queue
 * This is the at_action() routine for QA_ATR_Enabled
 */

int
check_que_enable(
  attribute *pattr,
  void *pque, /* actually a queue* */
  int mode
)
  {
  attribute *datr;

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
      datr = &((pbs_queue *)pque)->qu_attr[(int)QR_ATR_RouteDestin];

      if (!(datr->at_flags & ATR_VFLAG_SET) ||
          (datr->at_val.at_arst->as_usedptr == 0))
        return (PBSE_QUENOEN);
      }
    }

  return (0); /* ok to enable queue */
  }

/*
 * set_queue_type - check the requested value of the queue type attribute
 * and set qu_type accordingly.
 * This is the at_action() routine for QA_ATR_QType
 */

int
set_queue_type(attribute *pattr, void *pque, int mode)
  {
  int   i;
  char *pca;
  char *pcv;
  int   spectype;

  static struct
    {
    int   type;
    char *name;
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

    if (spectype != -1)   /* set up the attribute */
      {
      ((pbs_queue *)pque)->qu_qs.qu_type = spectype;
      (void)free(pattr->at_val.at_str);
      pattr->at_val.at_str = malloc(strlen(qt[i].name) + 1);

      if (pattr->at_val.at_str == (char *)0)
        return (PBSE_SYSTEM);

      (void)strcpy(pattr->at_val.at_str, qt[i].name);

      return (0);
      }
    }

  return (PBSE_BADATVAL);
  }


/*
 * mgr_log_attr - log the change of an attribute
 */

static void
mgr_log_attr(
  char *msg,
  struct svrattrl *plist,
  int logclass, /* see log.h */
  char *objname /* object being modified */
)
  {
  char *pstr;

  while (plist)
    {
    (void)strcpy(log_buffer, msg);
    (void)strcat(log_buffer, plist->al_name);

    if (plist->al_rescln)
      {
      (void)strcat(log_buffer, ".");
      (void)strcat(log_buffer, plist->al_resc);
      }

    if (plist->al_op == INCR)
      pstr = " + ";
    else if (plist->al_op == DECR)
      pstr = " - ";
    else
      pstr = " = ";

    (void)strcat(log_buffer, pstr);

    if (plist->al_valln)
      (void)strncat(log_buffer, plist->al_value,
                    LOG_BUF_SIZE - strlen(log_buffer) - 1);

    log_buffer[LOG_BUF_SIZE-1] = '\0';

    log_event(PBSEVENT_ADMIN, logclass, objname, log_buffer);

    plist = (struct svrattrl *)GET_NEXT(plist->al_link);
    }
  }




/*
 * mgr_set_attr - set attributes for manager function
 *
 * The set operation is performed as an atomic operation: all specified
 * attributes must be successfully set, or none are modified.
 */

static int mgr_set_attr(

  attribute *pattr,  /* current attributes */
  attribute_def *pdef,
  int   limit,
  svrattrl *plist,
  int   privil,
  int  *bad,
  void  *parent,
  int   mode)

  {
  int   index;
  attribute *new;
  attribute *pnew;
  attribute *pold;
  int   rc;

  if (plist == (struct svrattrl *)0)
    {
    return(0);
    }

  new = (attribute *)calloc((unsigned int)limit, sizeof(attribute));

  if (new == (attribute *)0)
    {
    return(PBSE_SYSTEM);
    }

  /*
   * decode the new attribute values which are in the request,
   * copy the corresponding current attribute into a holding array
   * and update it with the newly decoded value
   */

  if ((rc = attr_atomic_set(plist, pattr, new, pdef, limit, -1, privil, bad)) != 0)
    {
    attr_atomic_kill(new, pdef, limit);

    return(rc);
    }

  for (index = 0;index < limit;index++)
    {
    pnew = new + index;

    pold = pattr + index;

    if (pnew->at_flags & ATR_VFLAG_MODIFY)
      {
      /*
       * for each attribute which is to be modified, call the
       * at_action routine for the attribute, if one exists, with the
       *  new value.  If the action fails, undo everything.
       */

      if ((pdef + index)->at_action)
        {
        if ((rc = (pdef + index)->at_action(new + index, parent, mode)))
          {
          /* always allow removing from ACLs */
          if (!((plist->al_op == DECR) && (pdef + index)->at_type == ATR_TYPE_ACL))
            {
            attr_atomic_kill(new, pdef, limit);

            return(rc);
            }
          }
        }

      /* now replace the old values with any modified new values */

      (pdef + index)->at_free(pold);

      if ((pold->at_type == ATR_TYPE_LIST) ||
          (pold->at_type == ATR_TYPE_RESC))
        {
        list_move(&pnew->at_val.at_list, &pold->at_val.at_list);
        }
      else
        {
        *pold = *pnew;
        }

      pold->at_flags = pnew->at_flags; /* includes MODIFY */
      }
    }

  /*
   * we have moved all the "external" values to the old array, thus
   * we just free the new array, NOT call at_free on each.
   */

  free((char *)new);

  return(0);
  }  /* END mgr_set_attr() */





/*
 * mgr_unset_attr - Unset (clear) attributes for manager function
 *
 * The set operation is performed as an atomic operation: all specified
 * attributes must be successfully set, or none are modified.
 */

int mgr_unset_attr(

  attribute *pattr,  /* current attributes */
  attribute_def *pdef,
  int   limit,
  svrattrl *plist,
  int   privil,
  int  *bad)

  {
  int   index;
  int   ord;
  svrattrl *pl;
  resource_def *prsdef;
  resource *presc;

  /* first check the attribute exists and we have privilege to set */

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

    /* have we privilege to unset the attribute ? */

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

    if (((pdef + index)->at_type == ATR_TYPE_RESC) &&
        (plist->al_resc != NULL))
      {
      /* free resource member, not the attribute */

      prsdef = find_resc_def(
                 svr_resc_def,
                 plist->al_resc,
                 svr_resc_size);

      if ((presc = find_resc_entry(pattr + index, prsdef)))
        {
        if ((pdef->at_parent != PARENT_TYPE_SERVER) ||
            (index != (int)SRV_ATR_resource_cost))
          {
          prsdef->rs_free(&presc->rs_value);
          }

        delete_link(&presc->rs_link);
        }

      free(presc);
      }
    else
      {
      /* free the whole attribute */

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

static void update_subnode(

  struct pbsnode *pnode)

  {

  struct  pbssubn *pnxtsn;
  unsigned short   state;

  state = pnode->nd_state & INUSE_COMMON_MASK;

  for (pnxtsn = pnode->nd_psn;pnxtsn;pnxtsn = pnxtsn->next)
    {
    pnxtsn->host  = pnode;

    pnxtsn->inuse = (pnxtsn->inuse & ~INUSE_COMMON_MASK) | state;
    }

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
 *      in constructing this temporary node attribute array. Once this temporary
 *      array is constucted, the "svrattrl" request list is processed against it
 * to produce a new "updated" attribute array.  When all the attribute
 *      processing completes, assuming it's successful, the updated attribute
 *      array is used to modify the node itself-- this can be done via each
 *      attribute's "action" function with the value of mode set to "ALTER".
 * Although it's a little twisted to use the idea of an attribute's
 * "action function" to move between node an attribute,it pushes the
 *      details down a level, below that which is taking place here-- a good
 *      thing.
 */

int mgr_set_node_attr(

  struct pbsnode *pnode, /*do some kind of "set" action on this node */
  attribute_def  *pdef,  /*pbsnode doesn't have attributes*/
  /*but in the future it might  so,*/
  /*make  code similar to existing */

  int   limit,  /*length of the *pdef array      */
  svrattrl *plist,  /*batch request's svrattrl list  */
  /*begins here                    */
  int   privil, /*requester's privilege          */
  int  *bad,  /*if there is a "bad attribute"  */
  /*pass back position via this loc*/
  void  *parent, /*may go unused in this function */
  int   mode)  /*passed to attrib's action func */
/*not used by this func at this time*/
  {
  int   i;
  int   index;
  int   nstatus = 0;
  int   nprops = 0;
  int   rc;
  attribute *new;
  attribute *unused = (attribute *)0;
  attribute *pnew;

  struct pbsnode   tnode;  /*temporary node*/

  struct pbssubn   tsnd;  /*temporary subnode */

  struct pbssubn  *xtmp;

  struct prop     *pdest;

  struct prop    **plink;

  if (plist == NULL)
    {
    return(0);  /* nothing to do, return success */
    }

  /* Get heap space for a temporary node-attribute array and use the
   * various "node-attribute action" functions defined in the file
   * "node_attr_def.c" to set the current values of each entry's
   * structure members
   */

  new = (attribute *)calloc((unsigned int)limit, sizeof(attribute));

  if (new == NULL)
    {
    return(PBSE_SYSTEM);
    }

  for (index = 0; index < limit; index++)
    {
    if ((pdef + index)->at_action)
      {
      if ((rc = (pdef + index)->at_action(new + index, (void *)pnode, ATR_ACTION_NEW)))
        {
        /*call the "at_free" func for each member of*/
        /*new" and then free the entire "new" array */
        /*return code (rc) shapes caller's reply    */

        attr_atomic_kill(new, pdef, limit);

        return(rc);
        }
      }
    }

  /*
   * The function "attr_atomic_node_set" does the following:
   * successively decodes the new attribute modifications carried in the
   * the request list, "plist", into a temporary attribute (on stack).
   * Updates each attribute in the derived node-attribute array using the
   * decoded values.  If a failure of some sort occurs, as evidenced by
   * a non-zero return code (rc), call attr_atomic_kill() to undo
   * everything-- calls upon the "at_free" function for each attribute
   * to free hanging structures, then frees up the "new" array
   * return code (rc) shapes caller's reply
   */

  if ((rc = attr_atomic_node_set(plist, unused, new, pdef, limit, -1, privil, bad)) != 0)
    {
    attr_atomic_kill(new, pdef, limit);

    return(rc);
    }

  /*
   * Use the updated attribute values in attribute array, "new", to update a
   * temporary (on stack) pbsnode, which is a copy of the one pointed to by
   * pnode.  If all goes well, we are home free-- just copy the information
   * in the temporary pbsnode, after it has gotten modified, back to the
   * node pointed to by "pnode" and free the attribute array "new".
   */

  tnode = *pnode;

  tsnd  = *pnode->nd_psn;

  tnode.nd_psn = &tsnd;

  tsnd.host = &tnode;

  for (index = 0;index < limit;index++)
    {
    pnew = new + index;

    if (pnew->at_flags & ATR_VFLAG_MODIFY)
      {
      /*
       * for each attribute which is to cause modification to the node, "tnode",
       * call the specified "at_action" routine for that attribute passing it
       * the address of the temporary attribute and the address of the temporary
       * pbsnode object.  If any of the update actions fail along the way, undo
       * everything.   Any malloc'd substructures should be hanging off the
       * appropriate attribute of the attribute array "new".
       */

      if ((pdef + index)->at_action)
        {
        if ((rc = (pdef + index)->at_action(new + index, (void *)&tnode, ATR_ACTION_ALTER)))
          {
          attr_atomic_kill(new, pdef, limit);

          return(rc);
          }
        }
      }
    }

  /*
   * we have successfully updated tnode; use it to modify pnode
   * if pnode has any malloc-ed storage that is being replaced
   * be sure to free the old.
   */

  if (pnode->nd_prop && (pnode->nd_prop != tnode.nd_prop))
    {
    if (pnode->nd_prop->as_buf)
      free(pnode->nd_prop->as_buf);

    free(pnode->nd_prop);

    pnode->nd_prop = NULL;
    }

  /* NOTE:  nd_status properly freed during attribute alter */

  if ((pnode->nd_state != tnode.nd_state))
    {
    char OrigState[1024];
    char FinalState[1024];

    /* changing node state */

    /* log change */

    PNodeStateToString(pnode->nd_state, OrigState, sizeof(OrigState));
    PNodeStateToString(tnode.nd_state, FinalState, sizeof(FinalState));

    sprintf(log_buffer, "node %s state changed from %s to %s",
            pnode->nd_name,
            OrigState,
            FinalState);

    log_event(
      PBSEVENT_ADMIN,
      PBS_EVENTCLASS_NODE,
      pnode->nd_name,
      log_buffer);
    }

  /* NOTE:  nd_status properly freed during attribute alter */

  /*
    if ((pnode->nd_status != NULL) && (pnode->nd_status != tnode.nd_status))
      {
      if (pnode->nd_status->as_buf != NULL)
        free(pnode->nd_status->as_buf);

      free(pnode->nd_status);

      pnode->nd_status = NULL;
      }
  */

  xtmp                = pnode->nd_psn;

  *pnode              = tnode;        /* updates all data including linking in props */

  pnode->nd_psn       = xtmp;

  *pnode->nd_psn      = tsnd;

  pnode->nd_psn->host = pnode;

  free(new);  /*any new  prop list has been put on pnode*/

  /*dispense with the attribute array itself*/

  /* update prop list based on new prop array */

  free_prop_list(pnode->nd_first);

  plink = &pnode->nd_first;

  if (pnode->nd_prop)
    {
    nprops = pnode->nd_prop->as_usedptr;

    for (i = 0;i < nprops;++i)
      {
      pdest = init_prop(pnode->nd_prop->as_string[i]);

      *plink = pdest;
      plink  = &pdest->next;
      }
    }

  /* now add in name as last prop */

  pdest  = init_prop(pnode->nd_name);

  *plink = pdest;

  pnode->nd_last = pdest;

  pnode->nd_nprops = nprops + 1;

  /* update status list based on new status array */

  free_prop_list(pnode->nd_f_st);

  plink = &pnode->nd_f_st;

  if (pnode->nd_status != NULL)
    {
    nstatus = pnode->nd_status->as_usedptr;

    for (i = 0;i < nstatus;++i)
      {
      pdest = init_prop(pnode->nd_status->as_string[i]);

      *plink = pdest;
      plink  = &pdest->next;
      }
    }

  /* now add in name as last status */

  pdest  = init_prop(pnode->nd_name);

  *plink = pdest;

  pnode->nd_l_st = pdest;

  pnode->nd_nstatus = nstatus + 1;

  /* now update subnodes */

  update_subnode(pnode);

  return(0);
  }  /* END mgr_set_node_attr() */




/*
 * mgr_queue_create - process request to create a queue
 *
 * Creates queue and calls mgr_set_attr to set queue attributes.
 */

void mgr_queue_create(

  struct batch_request *preq)

  {
  int   bad;
  char  *badattr;
  svrattrl *plist;
  pbs_queue *pque;
  int   rc;

  rc = strlen(preq->rq_ind.rq_manager.rq_objname);

  if ((rc > PBS_MAXQUEUENAME) || (rc == 0))
    {
    req_reject(PBSE_QUENBIG, 0, preq, NULL, NULL);

    return;
    }

  if (find_queuebyname(preq->rq_ind.rq_manager.rq_objname))
    {
    req_reject(PBSE_QUEEXIST, 0, preq, NULL, NULL);

    return;
    }

  pque = que_alloc(preq->rq_ind.rq_manager.rq_objname);

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

    que_free(pque);
    }
  else
    {
    que_save(pque);
    svr_save(&server, SVR_SAVE_QUICK);

    sprintf(log_buffer, msg_manager,
            msg_man_cre,
            preq->rq_user,
            preq->rq_host);

    log_event(
      PBSEVENT_ADMIN,
      PBS_EVENTCLASS_QUEUE,
      pque->qu_qs.qu_name,
      log_buffer);

    mgr_log_attr(
      msg_man_set,
      plist,
      PBS_EVENTCLASS_QUEUE,
      preq->rq_ind.rq_manager.rq_objname);

    /* check the appropriateness of the attributes vs. queue type */

    if ((badattr = check_que_attr(pque)) != NULL)
      {
      /* mismatch, issue warning */

      sprintf(log_buffer, msg_attrtype,
              pque->qu_qs.qu_name,
              badattr);

      reply_text(preq, PBSE_ATTRTYPE, log_buffer);
      }
    else
      {
      reply_ack(preq);
      }
    }

  return;
  }






/*
 * mgr_queue_delete - delete a queue
 *
 * The queue must be empty of jobs
 */

void mgr_queue_delete(

  struct batch_request *preq)

  {
  pbs_queue *pque;
  int    rc;

  pque = find_queuebyname(preq->rq_ind.rq_manager.rq_objname);

  if (pque == NULL)
    {
    /* FAILURE */

    req_reject(PBSE_UNKQUE, 0, preq, NULL, NULL);

    return;
    }

  if ((rc = que_purge(pque)) != 0)
    {
    /* FAILURE */

    req_reject(rc, 0, preq, NULL, NULL);

    return;
    }

  svr_save(&server, SVR_SAVE_QUICK);

  sprintf(log_buffer, msg_manager,
          msg_man_del,
          preq->rq_user,
          preq->rq_host);

  log_event(
    PBSEVENT_ADMIN,
    PBS_EVENTCLASS_QUEUE,
    preq->rq_ind.rq_manager.rq_objname,
    log_buffer);

  reply_ack(preq);

  /* SUCCESS */

  return;
  }  /* END mgr_queue_delete() */




int hostname_check(

  char *hostname)

  {
  char  myhost[PBS_MAXHOSTNAME];
  char  extension[PBS_MAXHOSTNAME];
  char  ret_hostname[PBS_MAXHOSTNAME];
  char *open_bracket;

  if (hostname == NULL)
    return(SUCCESS);
  else if(strchr(hostname,'*') != NULL)
    return(SUCCESS);

  strcpy(myhost,hostname);
  open_bracket = strchr(myhost,'[');

  ret_hostname[0] = '\0';

  if (open_bracket == NULL)
    {
    /* handle normally */
    if ((get_fullhostname(hostname,ret_hostname,PBS_MAXHOSTNAME,NULL)) ||
        strncmp(hostname, ret_hostname, PBS_MAXHOSTNAME))
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
      strcpy(extension,close_bracket+1);
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
  int     rc;


  plist = (svrattrl *)GET_NEXT(preq->rq_ind.rq_manager.rq_attr);

  rc = mgr_set_attr(
         server.sv_attr,
         svr_attr_def,
         SRV_ATR_LAST,
         plist,
         preq->rq_perm,
         &bad_attr,
         (void *) & server,
         ATR_ACTION_ALTER);

  /* PBSE_BADACLHOST - lets show the user the first bad host in the ACL  */

  if (rc == PBSE_BADACLHOST)
    {
    char     *bad_host;
    char     *host_entry;
    attribute temp;
    int       index;
    int       i;

    struct    array_strings *pstr;
    int       bhstrlen;

    bhstrlen = PBS_MAXHOSTNAME + 17;
    bad_host = malloc(sizeof(char) * (bhstrlen + 1));

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

      index = find_attr(svr_attr_def, plist->al_name, SRV_ATR_LAST);

      clear_attr(&temp, &svr_attr_def[index]);
      svr_attr_def[index].at_decode(&temp, plist->al_name, plist->al_resc, plist->al_value);

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

      pstr = server.sv_attr[(int)index].at_val.at_arst;

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

    return;
    } /* END if (rc == PBSE_BADACLHOST) */
  else if (rc != 0)
    {
    reply_badattr(rc, bad_attr, plist, preq);

    return;
    }

  svr_save(&server, SVR_SAVE_FULL);

  sprintf(log_buffer, msg_manager, msg_man_set,
          preq->rq_user,
          preq->rq_host);

  log_event(
    PBSEVENT_ADMIN,
    PBS_EVENTCLASS_SERVER,
    msg_daemonname,
    log_buffer);

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
  int   bad_attr = 0;
  svrattrl *plist;
  int   rc;

  plist = (svrattrl *)GET_NEXT(preq->rq_ind.rq_manager.rq_attr);

  rc = mgr_unset_attr(
         server.sv_attr,
         svr_attr_def,
         SRV_ATR_LAST,
         plist,
         preq->rq_perm,
         &bad_attr);

  if (rc != 0)
    {
    reply_badattr(rc, bad_attr, plist, preq);
    }
  else
    {
    svr_save(&server, SVR_SAVE_FULL);

    sprintf(log_buffer, msg_manager,
            msg_man_uns,
            preq->rq_user,
            preq->rq_host);

    log_event(
      PBSEVENT_ADMIN,
      PBS_EVENTCLASS_SERVER,
      msg_daemonname,
      log_buffer);

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
  int    allques;
  int    bad = 0;
  char   *badattr;
  svrattrl  *plist;
  pbs_queue *pque;
  char      *qname;
  int    rc;

  if ((*preq->rq_ind.rq_manager.rq_objname == '\0') ||
      (*preq->rq_ind.rq_manager.rq_objname == '@'))
    {
    qname   = all_quename;
    allques = TRUE;

    pque = (pbs_queue *)GET_NEXT(svr_queues);
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

    return;
    }

  /* set the attributes */

  sprintf(log_buffer, msg_manager,
          msg_man_set,
          preq->rq_user,
          preq->rq_host);

  log_event(PBSEVENT_ADMIN, PBS_EVENTCLASS_QUEUE, qname, log_buffer);

  if (allques == TRUE)
    pque = (pbs_queue *)GET_NEXT(svr_queues);

  plist = (svrattrl *)GET_NEXT(preq->rq_ind.rq_manager.rq_attr);

  while (pque != NULL)
    {
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
      reply_badattr(rc, bad, plist, preq);

      return;
      }

    que_save(pque);

    mgr_log_attr(msg_man_set, plist, PBS_EVENTCLASS_QUEUE, pque->qu_qs.qu_name);

    if (allques == FALSE)
      break;

    pque = (pbs_queue *)GET_NEXT(pque->qu_link);
    }  /* END while (pque != NULL) */

  /* check the appropriateness of the attributes based on queue type */

  if (allques == TRUE)
    pque = (pbs_queue *)GET_NEXT(svr_queues);

  while (pque != NULL)
    {
    if ((badattr = check_que_attr(pque)) != NULL)
      {
      sprintf(log_buffer, msg_attrtype,
              pque->qu_qs.qu_name,
              badattr);

      reply_text(preq, PBSE_ATTRTYPE, log_buffer);

      return;
      }

    if (allques == FALSE)
      break;

    pque = (pbs_queue *)GET_NEXT(pque->qu_link);
    }  /* END while (pque != NULL) */

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
  char      *qname;
  int    rc;

  if ((*preq->rq_ind.rq_manager.rq_objname == '\0') ||
      (*preq->rq_ind.rq_manager.rq_objname == '@'))
    {
    qname   = all_quename;
    allques = TRUE;

    pque = (pbs_queue *)GET_NEXT(svr_queues);
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

    return;
    }

  sprintf(log_buffer, msg_manager,

          msg_man_uns,
          preq->rq_user,
          preq->rq_host);

  log_event(
    PBSEVENT_ADMIN,
    PBS_EVENTCLASS_QUEUE,
    qname,
    log_buffer);

  plist = (svrattrl *)GET_NEXT(preq->rq_ind.rq_manager.rq_attr);

  while (pque != NULL)
    {
    rc = mgr_unset_attr(
           pque->qu_attr,
           que_attr_def,
           QA_ATR_LAST,
           plist,
           preq->rq_perm,
           &bad_attr);

    if (rc != 0)
      {
      reply_badattr(rc, bad_attr, plist, preq);

      return;
      }

    que_save(pque);

    mgr_log_attr(msg_man_uns, plist, PBS_EVENTCLASS_QUEUE, pque->qu_qs.qu_name);

    if ((pque->qu_attr[(int)QA_ATR_QType].at_flags & ATR_VFLAG_SET) == 0)
      pque->qu_qs.qu_type = QTYPE_Unset;

    if (allques == FALSE)
      break;

    pque = GET_NEXT(pque->qu_link);
    }

  reply_ack(preq);

  return;
  }





/*
 * mgr_node_set - Set node "state" or "properties"
 *
 * Finds the node, Sets the requested state flags, properties, or type and returns
 * a reply to the sender of the batch_request
 */

void mgr_node_set(

  struct batch_request *preq)  /* I */

  {
  static int need_todo = 0;

  int  allnodes = 0;
  int  propnodes = 0;
  int  bad = 0;
  svrattrl *plist;

  struct pbsnode  *pnode;
  char  *nodename = NULL;
  int  rc;

  int  i, len;
  int  problem_cnt = 0;
  char  *problem_names;

  struct pbsnode  **problem_nodes = NULL;

  struct prop props;

  if ((*preq->rq_ind.rq_manager.rq_objname == '\0') ||
      (*preq->rq_ind.rq_manager.rq_objname == '@') ||
      ((*preq->rq_ind.rq_manager.rq_objname == ':') &&
       (*(preq->rq_ind.rq_manager.rq_objname + 1) != '\0')))
    {
    /*In this instance the set node req is to apply to all */
    /*nodes at the local ('\0')  or specified ('@') server */

    if ((pbsndlist != NULL) && svr_totnodes)
      {
      pnode = *pbsndlist;

      if ((*preq->rq_ind.rq_manager.rq_objname == ':') &&
          (strcmp(preq->rq_ind.rq_manager.rq_objname + 1, "ALL") != 0))
        {
        propnodes = 1;
        nodename = preq->rq_ind.rq_manager.rq_objname;
        props.name = nodename + 1;
        props.mark = 1;
        props.next = NULL;
        }
      else
        {
        allnodes = 1;
        nodename = all_nodes;
        }
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

  /*set "state", "properties", or type of node (nodes if allnodes == 1)*/

  sprintf(log_buffer, msg_manager,
          msg_man_set,
          preq->rq_user,
          preq->rq_host);

  log_event(PBSEVENT_ADMIN, PBS_EVENTCLASS_NODE, nodename, log_buffer);

  if (allnodes || propnodes)
    {
    pnode = pbsndlist[0];

    problem_nodes = (struct pbsnode **)malloc(svr_totnodes * sizeof(struct pbsnode *));

    problem_cnt = 0;
    }

  plist = (svrattrl *)GET_NEXT(preq->rq_ind.rq_manager.rq_attr);

  for (i = 0;i < svr_totnodes;i++, pnode = pbsndlist[i])
    {
    if (propnodes && !hasprop(pnode, &props))
      continue;

    save_characteristic(pnode);

    rc = mgr_set_node_attr(
           pnode,
           node_attr_def,
           ND_ATR_LAST,
           plist,
           preq->rq_perm,
           &bad,
           (void *)pnode,
           ATR_ACTION_ALTER);

    if (rc != 0)
      {
      if (allnodes || propnodes)
        {
        if (problem_nodes)     /*we have an array in which to save*/
          problem_nodes[ problem_cnt ] = pnode;

        ++problem_cnt;
        }
      else
        {
        /*In the specific node case, reply w/ error and return*/

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
        }  /* END else */
      }
    else
      {
      /* modifications succeeded for this node */

      chk_characteristic(pnode, &need_todo);

      mgr_log_attr(
        msg_man_set,
        plist,
        PBS_EVENTCLASS_NODE,
        pnode->nd_name);
      }

    if (!allnodes && !propnodes)
      break;
    }  /* END for (i) */

  if (need_todo & WRITENODE_STATE)
    {
    /*some nodes set to "offline"*/

    write_node_state();

    need_todo &= ~(WRITENODE_STATE);
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

    if (!update_nodes_file())

      need_todo &= ~(WRITE_NEW_NODESFILE);  /*successful on update*/
    }

  if (allnodes || propnodes)
    {
    /* modification was for all nodes */

    if (problem_cnt)
      {
      /* one or more problems encountered */

      for (len = 0, i = 0;i < problem_cnt;i++)
        len += strlen(problem_nodes[i]->nd_name) + 3;

      len += strlen(pbse_to_txt(PBSE_GMODERR));

      if ((problem_names = malloc(len)))
        {
        strcpy(problem_names, pbse_to_txt(PBSE_GMODERR));

        for (i = 0;i < problem_cnt;i++)
          {
          if (i)
            strcat(problem_names, ", ");

          strcat(problem_names, problem_nodes[i]->nd_name);
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
    }    /* END if (allnodes || propnodes) */

  recompute_ntype_cnts();

  reply_ack(preq);  /*request completely successful*/

  return;
  }  /* END void mgr_node_set() */





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
  static int need_todo = 0;

  int  allnodes = 0;

  struct pbsnode *pnode;
  char  *nodename = NULL;
  int  rc;

  int  i, len;
  int  problem_cnt = 0;
  char  *problem_names;

  struct pbsnode  **problem_nodes = NULL;
  svrattrl *plist;

  if ((*preq->rq_ind.rq_manager.rq_objname == '\0') ||
      (*preq->rq_ind.rq_manager.rq_objname == '@'))
    {
    /* In this instance the set node req is to apply to all */
    /* nodes at the local ('\0')  or specified ('@') server */

    if ((pbsndlist != NULL) && svr_totnodes)
      {
      nodename = all_nodes;
      pnode = *pbsndlist;
      allnodes = 1;
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

  sprintf(log_buffer, msg_manager,

          msg_man_del,
          preq->rq_user,
          preq->rq_host);

  log_event(PBSEVENT_ADMIN, PBS_EVENTCLASS_NODE, nodename, log_buffer);

  /*if doing many and problem arises with some, record them for report*/
  /*the array of "problem nodes" sees no use now and may never see use*/

  if (allnodes)
    {
    pnode = pbsndlist[0];

    problem_nodes = (struct pbsnode **)malloc(
                      svr_totnodes * sizeof(struct pbsnode *));

    problem_cnt = 0;
    }

  /*set "deleted" bit in node's (nodes, allnodes == 1) "inuse" field*/
  /*remove entire prop list, including the node name, from the node */
  /*remove the IP address array hanging from the node               */

  plist = (svrattrl *)GET_NEXT(preq->rq_ind.rq_manager.rq_attr);

  for (i = 0;i < svr_totnodes;i++, pnode = pbsndlist[i])
    {
    save_characteristic(pnode);

    nodename = strdup(pnode->nd_name);

    effective_node_delete(pnode);

    rc = 0;   /*currently, failure not possible so set rc=0   */

    if (rc != 0)
      {
      if (allnodes)
        {
        if (problem_nodes)     /*we have an array in which to save*/
          problem_nodes[problem_cnt] = pnode;

        ++problem_cnt;
        }
      else
        {
        /*In the specific node case, reply w/ error and return*/

        switch (rc)
          {

          default:

            req_reject(rc, 0, preq, NULL, NULL);

            break;
          }

        return;
        }
      }
    else
      {
      /*modifications succeed for this node*/

      chk_characteristic(pnode, &need_todo);

      if (nodename)
        {
        mgr_log_attr(msg_man_set, plist, PBS_EVENTCLASS_NODE, nodename);

        free(nodename);
        }
      }

    if (!allnodes)
      break;
    } /* bottom of the for() */

  if (need_todo & WRITENODE_STATE)
    {
    /*some nodes set to "offline"*/

    write_node_state();

    need_todo &= ~(WRITENODE_STATE);
    }

  if (need_todo & WRITE_NEW_NODESFILE)
    {
    /* create/delete/prop/ntype change */

    if (!update_nodes_file())
      need_todo &= ~(WRITE_NEW_NODESFILE);  /*successful on update*/
    }

  if (allnodes)
    {
    /*modification was for all nodes  */

    if (problem_cnt)      /*one or more problems encountered*/
      {

      for (len = 0, i = 0; i < problem_cnt; i++)
        len += strlen(problem_nodes[i]->nd_name) + 3;

      len += strlen(pbse_to_txt(PBSE_GMODERR));

      if ((problem_names = malloc(len)))
        {

        strcpy(problem_names, pbse_to_txt(PBSE_GMODERR));

        for (i = 0; i < problem_cnt; i++)
          {
          if (i)
            strcat(problem_names, ", ");

          strcat(problem_names, problem_nodes[i]->nd_name);
          }

        reply_text(preq, PBSE_GMODERR, problem_names);

        free(problem_names);
        }
      else
        {
        reply_text(preq, PBSE_GMODERR, pbse_to_txt(PBSE_GMODERR));
        }
      }

    if (problem_nodes)  /*maybe problem  malloc failed */
      free(problem_nodes);

    if (problem_cnt)    /*reply has already been sent  */
      {
      recompute_ntype_cnts();
      return ;
      }
    }

  recompute_ntype_cnts();

  reply_ack(preq);  /*request completely successful*/

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
  int   bad;
  svrattrl *plist;
  int   rc;

  plist = (svrattrl *)GET_NEXT(preq->rq_ind.rq_manager.rq_attr);

  rc = create_pbs_node(
         preq->rq_ind.rq_manager.rq_objname,
         plist,
         preq->rq_perm,
         &bad);

  if (rc != 0)
    {
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

  mgr_log_attr(

    msg_man_set,
    plist,
    PBS_EVENTCLASS_NODE,
    preq->rq_ind.rq_manager.rq_objname);

  setup_notification(preq->rq_ind.rq_manager.rq_objname);     /* set mechanism for notifying */
  /* other nodes of new member   */

  if (update_nodes_file())
    {
    /* if update fails now (odd) */

    svr_chngNodesfile = 1;  /* try it when server shutsdown */
    }
  else
    {
    svr_chngNodesfile = 0;
    }

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

void req_manager(

  struct batch_request *preq)  /* I */

  {
  switch (preq->rq_ind.rq_manager.rq_cmd)
    {

    case MGR_CMD_CREATE:

    case MGR_CMD_DELETE:

      if ((preq->rq_perm & PERM_MANAGER) == 0)
        {
        req_reject(PBSE_PERM, 0, preq, NULL, NULL);

        return;
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

          if (preq->rq_ind.rq_manager.rq_cmd == MGR_CMD_CREATE)
            mgr_node_create(preq);
          else
            mgr_node_delete(preq);

          break;

        default:

          req_reject(PBSE_IVALREQ, 0, preq, NULL, NULL);

          return;

          break;
        }

      break;

    case MGR_CMD_SET:

      if ((preq->rq_perm & PERM_OPorMGR) == 0)
        {
        req_reject(PBSE_PERM, 0, preq, NULL, NULL);

        return;
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
        req_reject(PBSE_PERM, 0, preq, NULL, NULL);

        return;
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

  return;
  }  /* END req_manager() */



/*
 * manager_oper_chk - check the @host part of a manager or operator acl
 * entry to insure it is fully qualified.  This is to prevent
 * input errors when setting the list.
 * This is the at_action() routine for the server attributes
 * "managers" and "operators"
 */

int manager_oper_chk(

  attribute *pattr,
  void      *pobject,
  int      actmode)

  {
  char   *entry;
  int    err = 0;
  int      i;

  struct array_strings *pstr;

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
        sprintf(log_buffer, "bad entry in acl: %s",
          pstr->as_string[i]);

        log_err(PBSE_BADACLHOST,
          "manager_oper_chk",
          log_buffer);
        }
      else
        {
        sprintf(log_buffer, "bad entry in acl: %s",
          pstr->as_string[i]);

        log_err(PBSE_BADACLHOST,
          "manager_oper_chk",
          log_buffer);

        err = PBSE_BADACLHOST;
        }
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

  attribute *pattr,   /* I */
  void      *pobject, /* I */
  int      actmode) /* I */

  {
  int    err = 0;
  char    hostname[PBS_MAXHOSTNAME + 1];
  char *pstr;

  extern char              server_name[];

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
      sprintf(log_buffer, "bad servername: %s",
              pstr);

      log_err(PBSE_BADHOST,
              "servername_chk",
              log_buffer);
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
 * extra_resc_chk() called when extra_resc server attribute is changed
 *     It's purpose is to re-init the resource definitions by calling
 *     init_resc_defs().  Unfortunately, it gets call before the
 *     change is actually committed; so we setup a work task to call
 *     it async asap.
 */
void on_extra_resc(

  struct work_task *ptask)
  {
  init_resc_defs();
  }

int extra_resc_chk(

  attribute *pattr,   /* I */
  void      *pobject, /* I */
  int      actmode) /* I */

  {

  struct work_task *ptask;

  /* Is there anything to validate?  Maybe check for all alphanum? */
  /* the new resource is at pattr->at_val.at_str */
  ptask = set_task(WORK_Immed, 0, on_extra_resc, NULL);

  return (ptask == NULL);
  }

/*
 * free_extraresc() makes sure that the init_resc_defs() is called after
 * the list has changed by 'unset'.
 */
void free_extraresc(

  struct attribute *attr)

  {
  set_task(WORK_Immed, 0, on_extra_resc, NULL);

  free_arst(attr);
  }


/*
 * disallowed_types_chk -
 *      This is the at_action() routine for the queue disallowed_type attribute
 */

int disallowed_types_chk(

  attribute *pattr,
  void      *pobject,
  int        actmode)

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

  attribute *pattr,
  int      actmode,
  long       minimum,
  long       maximum)

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
 * This is the at_action() routine for the server attribute
 * "scheduler_iteration"
 */

int schiter_chk(

  attribute *pattr,
  void      *pobject,
  int      actmode)

  {
  return(mgr_long_action_helper(pattr, actmode, 1, PBS_SCHEDULE_CYCLE));
  }  /* END schiter_chk() */

/* nextjobnum_action - makes sure value is sane (>=0)
   Note that there must be special code in svr_recov
   to prevent the attribute value from clobbering
   the internal counter server.sv_qs.sv_jobidnumber.
 */
int nextjobnum_chk(

  attribute *pattr,
  void      *pobject,
  int        actmode)
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

  struct attribute *attr,
  struct attribute *new,
  enum batch_op op)

  {
  /* this is almost identical to set_l, but we need to grab the
     current value of server.sv_qs.sv_jobidnumber before we INCR/DECR the value of
     attr->at_val.at_long.  In fact, it probably should be moved to Libattr/ */

  /* The special handling of INCR is to allow setting of a job number range.
   * If the job numbers are already in the range, it does not alter the number.
   * Otherwise, if the number is in the default range, it sets it to the new range.
   */

  switch (op)
    {

    case SET:
      attr->at_val.at_long = new->at_val.at_long;
      break;

      /* Code in Moab depends on this, do not change until the Moab code is fixed. */

    case INCR:
      attr->at_val.at_long = MAX(server.sv_qs.sv_jobidnumber, new->at_val.at_long);
      /*case INCR:  attr->at_val.at_long += new->at_val.at_long;*/
      break;

    case DECR:
      attr->at_val.at_long -= new->at_val.at_long;
      break;

    default:
      return(PBSE_SYSTEM);
    }

  attr->at_flags |= ATR_VFLAG_SET | ATR_VFLAG_MODIFY;

  server.sv_qs.sv_jobidnumber = attr->at_val.at_long;
  svr_save(&server, SVR_SAVE_QUICK);

  return 0;
  }

/* END req_manager.c */

