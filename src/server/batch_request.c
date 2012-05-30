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

#include <stdio.h>
#include <string.h>

#include "batch_request.h"
#include "utils.h"

int get_batch_request_id(

  batch_request *preq)

  {
  int  id;
  char buf[MAXLINE];

  pthread_mutex_lock(brh.brh_mutex);
  id = brh.brh_index++;
  pthread_mutex_unlock(brh.brh_mutex);

  snprintf(buf, sizeof(buf), "%d", id);

  if ((preq->rq_id = strdup(buf)) == NULL)
    return(ENOMEM);

  insert_batch_request(preq);

  return(PBSE_NONE);
  } /* END get_batch_request_id() */




void initialize_batch_request_holder()

  {
  brh.brh_ra = initialize_resizable_array(INITIAL_REQUEST_HOLDER_SIZE);

  brh.brh_mutex = calloc(1, sizeof(pthread_mutex_t));
  pthread_mutex_init(brh.brh_mutex, NULL);

  brh.brh_ht = create_hash(INITIAL_HASH_SIZE);
  } /* initialize_batch_request_holder() */




int insert_batch_request(

  batch_request *preq)

  {
  int rc;

  pthread_mutex_lock(brh.brh_mutex);

  if ((rc = insert_thing(brh.brh_ra, preq)) < 0)
    {
    rc = ENOMEM;
    log_err(rc, __func__, "No memory to resize the array...SYSTEM FAILURE\n");
    }
  else
    {
    add_hash(brh.brh_ht, rc, preq->rq_id);

    rc = PBSE_NONE;
    }

  pthread_mutex_unlock(brh.brh_mutex);

  return(rc);
  } /* END insert_batch_request() */




batch_request *get_batch_request(

  char *br_id)

  {
  batch_request *preq = NULL;
  int            i;

  pthread_mutex_lock(brh.brh_mutex);
  
  i = get_value_hash(brh.brh_ht, br_id);
  
  if (i >= 0)
    preq = (batch_request *)brh.brh_ra->slots[i].item;
  
  pthread_mutex_unlock(brh.brh_mutex);
  
  return(preq);
  } /* END get_batch_request() */




batch_request *get_remove_batch_request(

  char *br_id)

  {
  batch_request *preq = NULL;
  int            i;

  pthread_mutex_lock(brh.brh_mutex);
  
  i = get_value_hash(brh.brh_ht, br_id);
  
  if (i >= 0)
    {
    preq = (batch_request *)brh.brh_ra->slots[i].item;
    remove_thing_from_index(brh.brh_ra, i);
    remove_hash(brh.brh_ht, br_id);
    }
  
  pthread_mutex_unlock(brh.brh_mutex);
  
  return(preq);
  } /* END get_remove_batch_request() */




int remove_batch_request(

  char *br_id)

  {
  int i;

  pthread_mutex_lock(brh.brh_mutex);

  i = get_value_hash(brh.brh_ht, br_id);

  if (i >= 0)
    {
    remove_thing_from_index(brh.brh_ra, i);
    remove_hash(brh.brh_ht, br_id);
    }
  pthread_mutex_unlock(brh.brh_mutex);

  if (i < 0)
    return(THING_NOT_FOUND);
  else
    return(PBSE_NONE);
  } /* END remove_batch_request() */




