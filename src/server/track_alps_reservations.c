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

#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <errno.h>

#include "track_alps_reservations.h"
#include "utils.h"
#include "batch_request.h"
#include "ji_mutex.h"

/* Global Data declarations */
extern int LOGLEVEL;


/*
 * add_node_names
 * adds the node names from pjob's exec hosts to ar
 * @param ar - the alps reservation we're populating
 * @param pjob - the job whose reservation we're examining
 * @see create_alps_reservation() - parent
 */

int add_node_names(

  alps_reservation *ar,
  job              *pjob)

  {
  char *exec_str = strdup(pjob->ji_wattr[JOB_ATR_exec_host].at_val.at_str);
  char *host_tok;
  char *str_ptr = exec_str;
  char *slash;
  int   rc = PBSE_NONE;
  char *prev_node = NULL;

  ar->ar_node_names = initialize_resizable_array(INITIAL_NODE_LIST_SIZE);

  while ((host_tok = threadsafe_tokenizer(&str_ptr, "+")) != NULL)
    {
    if ((slash = strchr(host_tok, '/')) != NULL)
      *slash = '\0';

    if ((prev_node == NULL) ||
        (strcmp(prev_node, host_tok)))
      {
      if ((rc = insert_thing(ar->ar_node_names, host_tok)) < 0)
        break;
      else
        rc = PBSE_NONE;
      }

    prev_node = host_tok;
    }

  return(rc);
  } /* END add_node_names() */



alps_reservation *populate_alps_reservation(

  job *pjob)

  {
  alps_reservation *ar = NULL;
  
  if ((ar = calloc(1, sizeof(alps_reservation))) != NULL)
    {
    ar->job_id = strdup(pjob->ji_qs.ji_jobid);
    ar->rsv_id = strdup(pjob->ji_wattr[JOB_ATR_reservation_id].at_val.at_str);
    if ((add_node_names(ar, pjob)) != PBSE_NONE)
      {
      free(ar->job_id);
      free(ar->rsv_id);
      free(ar);
      ar = NULL;
      }
    }

  return(ar);
  } /* END populate_alps_reservation() */




/* 
 * create_alps_reservation
 * creates an alps reservation based 
 */

int create_alps_reservation(
    
  job *pjob)

  {
  int               rc = PBSE_NONE;
  alps_reservation *ar;

  /* job has no alps reservation, nothing to record */
  if (pjob->ji_wattr[JOB_ATR_reservation_id].at_val.at_str == NULL)
    return(PBSE_NONE);

  if ((ar = populate_alps_reservation(pjob)) != NULL)
    insert_alps_reservation(ar);
  else
    rc = ENOMEM;

  return(rc);
  } /* create_alps_reservation() */




int insert_alps_reservation(
    
  alps_reservation *ar)

  {
  int index;
  int rc;

  pthread_mutex_lock(alps_reservations.rh_mutex);
  if ((index = insert_thing(alps_reservations.rh_alps_rsvs, ar)) >= 0)
    rc = add_hash(alps_reservations.rh_ht, index, ar->rsv_id);
  else
    rc = ENOMEM;
  pthread_mutex_unlock(alps_reservations.rh_mutex);

  return(rc);
  } /* insert_alps_reservation() */



int already_recorded(

  char *rsv_id)

  {
  int               index;
  int               recorded = FALSE;

  pthread_mutex_lock(alps_reservations.rh_mutex);
  if ((index = get_value_hash(alps_reservations.rh_ht, rsv_id)) >= 0)
    {
    if (alps_reservations.rh_alps_rsvs->slots[index].item != NULL)
      recorded = TRUE;
    }
  pthread_mutex_unlock(alps_reservations.rh_mutex);

  return(recorded);
  } /* already_recorded() */



int is_orphaned(

  char *rsv_id)

  {
  int               index;
  int               orphaned = FALSE;
  job              *pjob;
  alps_reservation *ar = NULL;

  pthread_mutex_lock(alps_reservations.rh_mutex);
  index = get_value_hash(alps_reservations.rh_ht, rsv_id);
  if (index != -1)
    ar = alps_reservations.rh_alps_rsvs->slots[index].item;
  pthread_mutex_unlock(alps_reservations.rh_mutex);

  if (ar != NULL)
    {
    if ((pjob = svr_find_job(ar->job_id, TRUE)) != NULL)
      {
      if (pjob->ji_qs.ji_state == JOB_STATE_COMPLETE)
        orphaned = TRUE;

      unlock_ji_mutex(pjob, __func__, "1", LOGLEVEL);
      }
    else
      orphaned = TRUE;
    }
  else
    orphaned = TRUE;

  return(orphaned);
  } /* END is_orphaned() */



void free_alps_reservation(

  alps_reservation *ar)

  {
  free_resizable_array(ar->ar_node_names);
  free(ar->rsv_id);
  free(ar->job_id);
  free(ar);
  } /* END free_alps_reservation() */



/*
 * remove_alps_reservation
 * remove the reservation with rsv_id from the alps_reservations struct
 *
 * @param rsv_id - the id of the reservation to remove
 * @return PBSE_NONE if removed, THING_NOT_FOUND if it wasn't present
 */

int remove_alps_reservation(
    
  char *rsv_id)

  {
  int               index;
  int               rc = PBSE_NONE;
  alps_reservation *ar = NULL;

  pthread_mutex_lock(alps_reservations.rh_mutex);
  if ((index = get_value_hash(alps_reservations.rh_ht, rsv_id)) < 0)
    rc = THING_NOT_FOUND;
  else
    {
    ar = alps_reservations.rh_alps_rsvs->slots[index].item;
    remove_thing_from_index(alps_reservations.rh_alps_rsvs, index);
    remove_hash(alps_reservations.rh_ht, rsv_id);
    }
  pthread_mutex_unlock(alps_reservations.rh_mutex);

  if (ar != NULL)
    free_alps_reservation(ar);

  return(rc);
  } /* END remove_alps_reservation() */




void initialize_alps_reservations()

  {
  alps_reservations.rh_mutex = calloc(1, sizeof(pthread_mutex_t));
  pthread_mutex_init(alps_reservations.rh_mutex, NULL);

  alps_reservations.rh_alps_rsvs = initialize_resizable_array(INITIAL_RESERVATION_HOLDER_SIZE);
  alps_reservations.rh_ht = create_hash(INITIAL_RESERVATION_HOLDER_SIZE + 1);
  } /* END initialize_alps_reservations() */





