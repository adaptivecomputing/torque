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

#include "resizable_array.h"
#include "pbs_nodes.h"
#include "login_nodes.h"
#include "../lib/Libutils/u_lock_ctl.h"


login_holder logins;




void initialize_login_holder()
  {
  logins.ra = initialize_resizable_array(LOGIN_INITIAL_SIZE);
  logins.next_node = 0;
  logins.iterate_backwards = 0;
  logins.ln_mutex = calloc(1, sizeof(pthread_mutex_t));
  pthread_mutex_init(logins.ln_mutex, NULL);
  }



int add_to_login_holder(

  struct pbsnode *pnode)

  {
  login_node *ln = calloc(1, sizeof(login_node));
  int         rc;

  ln->pnode = pnode;

  pthread_mutex_lock(logins.ln_mutex);
  if ((rc = insert_thing(logins.ra, ln)) >= 0)
    {
    logins.next_node = rc;
    rc = PBSE_NONE;
    }
  pthread_mutex_unlock(logins.ln_mutex);

  return(rc);
  } /* END add_to_login_holder() */




struct pbsnode *check_node(

  login_node  *ln,
  struct prop *needed)

  {
  struct pbsnode *pnode = ln->pnode;

  lock_node(pnode, __func__, NULL, 20);

  if ((hasprop(pnode, needed) == TRUE) &&
      (pnode->nd_nsn - pnode->nd_np_to_be_used >= 1))
    return(pnode);
  else
    {
    unlock_node(pnode, __func__, NULL, 20);
    return(NULL);
    }
  } /* END check_node() */




struct pbsnode *find_fitting_node(

  struct prop *needed)

  {
  struct pbsnode *pnode = NULL;
  login_node     *ln;
  int             iter = -1;

  if (logins.iterate_backwards == TRUE)
    {
    logins.iterate_backwards = FALSE;

    while ((ln = next_thing_from_back(logins.ra, &iter)) != NULL)
      {
      if ((pnode = check_node(ln, needed)) != NULL)
        {
        ln->times_used++;
        return(pnode);
        }
      }
    }
  else
    {
    logins.iterate_backwards = TRUE;

    while ((ln = next_thing(logins.ra, &iter)) != NULL)
      {
      if ((pnode = check_node(ln, needed)) != NULL)
        {
        ln->times_used++;
        return(pnode);
        }
      }
    }

  return(NULL);
  } /* END find_fitting_node() */




void update_next_node_index(

  unsigned int to_beat)

  {
  int         iter = -1;
  int         prev_index = logins.ra->slots[ALWAYS_EMPTY_INDEX].next;
  login_node *ln;

  while ((ln = next_thing(logins.ra, &iter)) != NULL)
    {
    if (ln->times_used < to_beat)
      {
      /* don't break - there may be one lower */
      to_beat = ln->times_used;
      logins.next_node = prev_index;
      }

    prev_index = iter;
    }

  } /* END update_next_node_index() */





struct pbsnode *get_next_login_node(

  struct prop *needed)

  {
  struct pbsnode *pnode = NULL;
  login_node     *ln;
  int             node_fits = TRUE;

  pthread_mutex_lock(logins.ln_mutex);
  ln = (login_node *)logins.ra->slots[logins.next_node].item;

  if (ln != NULL)
    {
    pnode = ln->pnode;
    lock_node(pnode, __func__, NULL, 0);
    
    if (needed != NULL)
      {
      if (hasprop(pnode, needed) == FALSE)
        {
        node_fits = FALSE;
        }
      }
    
    /* must have at least one execution slot available */
    if ((pnode->nd_nsn - pnode->nd_np_to_be_used < 1) ||
        ((pnode->nd_state & INUSE_DOWN) != 0) ||
        ((pnode->nd_state & INUSE_OFFLINE) != 0) ||
        ((pnode->nd_state & INUSE_DELETED) != 0))
      {
      node_fits = FALSE;
      }
    
    if (node_fits == FALSE)
      {
      unlock_node(pnode, __func__, NULL, 0);
      pnode = find_fitting_node(needed);
      }
    else
      {
      ln->times_used++;
      update_next_node_index(ln->times_used);
      }
    }

  pthread_mutex_unlock(logins.ln_mutex);

  return(pnode);
  } /* END get_next_login_node() */


int login_node_count()

  {
  int count = 0;

  pthread_mutex_lock(logins.ln_mutex);
  count = logins.ra->num;
  pthread_mutex_unlock(logins.ln_mutex);

  return(count);
  } /* END login_node_count() */

