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
#include "id_map.hpp"

/* Global Data declarations */
extern int LOGLEVEL;


/*
 * add_node_names
 * adds the node names from pjob's exec hosts to ar
 * @param ar - the alps reservation we're populating
 * @param pjob - the job whose reservation we're examining
 * @see track_alps_reservation() - parent
 */

int add_node_names(

  alps_reservation *ar,
  job              *pjob)

  {
  if (!pjob->ji_wattr[JOB_ATR_exec_host].at_val.at_str)
    {
    if (LOGLEVEL >= 7)
      {
      char log_buf[PBS_MAXSVRJOBID + 512];
      snprintf(log_buf, sizeof(log_buf), "Job %s exec list was null", pjob->ji_qs.ji_jobid);
      log_event(PBSEVENT_JOB, PBS_EVENTCLASS_JOB, __func__, log_buf);
      }
    return -1;
    }

  char *exec_str = strdup(pjob->ji_wattr[JOB_ATR_exec_host].at_val.at_str);
  char *host_tok;
  char *str_ptr = exec_str;
  char *slash;
  int   rc = PBSE_NONE;
  char *prev_node = NULL;

  while ((host_tok = threadsafe_tokenizer(&str_ptr, "+")) != NULL)
    {
    if ((slash = strchr(host_tok, '/')) != NULL)
      *slash = '\0';

    if ((prev_node == NULL) ||
        (strcmp(prev_node, host_tok)))
      {
      ar->ar_node_names.push_back(std::string(host_tok));
      rc = PBSE_NONE;
      }

    prev_node = host_tok;
    }

  if (exec_str != NULL)
    free(exec_str);

  return(rc);
  } /* END add_node_names() */


alps_reservation *populate_alps_reservation(

  job *pjob)

  {
  alps_reservation *ar = new alps_reservation(pjob->ji_internal_id,
                                              pjob->ji_wattr[JOB_ATR_reservation_id].at_val.at_str);
  
  if (ar != NULL)
    {
    if ((add_node_names(ar, pjob)) != PBSE_NONE)
      {
      delete ar;
      ar = NULL;
      }
    }

  return(ar);
  } /* END populate_alps_reservation() */




/* 
 * track_alps_reservation
 * creates an alps reservation based 
 */

int track_alps_reservation(
    
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
  } /* track_alps_reservation() */




int insert_alps_reservation(
    
  alps_reservation *ar)

  {
  int rc = PBSE_NONE;

  alps_reservations.lock();
  if (!alps_reservations.insert(ar,ar->rsv_id))
    rc = ENOMEM;
  alps_reservations.unlock();

  return(rc);
  } /* insert_alps_reservation() */



int already_recorded(

  const char *rsv_id)

  {
  int               recorded = FALSE;

  alps_reservations.lock();
  if (alps_reservations.find(rsv_id) != NULL)
    recorded = TRUE;
  alps_reservations.unlock();

  return(recorded);
  } /* already_recorded() */



/*
 * is_orphaned()
 *
 * @param rsv_id - the id of the reservation
 * @param job_id - here we'll print the id of the job this reservation was attached to, 
 * if we find one
 *
 * @return true if the reservation is now an orphan, false otherwise.
 */

bool is_orphaned(

  char *rsv_id,
  char *job_id)

  {
  bool              orphaned = false;
  job              *pjob;
  alps_reservation *ar = NULL;

  alps_reservations.lock();
  ar = alps_reservations.find(rsv_id);
  alps_reservations.unlock();

  if (ar != NULL)
    {
    if (job_id != NULL)
      snprintf(job_id, PBS_MAXSVRJOBID, "%s", job_mapper.get_name(ar->internal_job_id));

    if ((pjob = svr_find_job_by_id(ar->internal_job_id)) != NULL)
      {
      if (pjob->ji_qs.ji_state == JOB_STATE_COMPLETE)
        orphaned = true;

      unlock_ji_mutex(pjob, __func__, "1", LOGLEVEL);
      }
    else
      orphaned = true;
    }
  else
    {
    if (job_id != NULL)
      snprintf(job_id, PBS_MAXSVRJOBID, "unknown");
    orphaned = true;
    }

  return(orphaned);
  } /* END is_orphaned() */


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
  int               rc = PBSE_NONE;
  alps_reservation *ar;

  alps_reservations.lock();
  ar = alps_reservations.find(rsv_id);
  if (!alps_reservations.remove(rsv_id))
    rc = THING_NOT_FOUND;
  alps_reservations.unlock();

  if (ar != NULL)
    delete ar;

  return(rc);
  } /* END remove_alps_reservation() */



/*
 * clear_all_alps_reservations()
 *
 * Clears all of the reservations from the structure and deletes them.
 */

void clear_all_alps_reservations()

  {
  alps_reservation *ar;

  alps_reservations.lock();

  while ((ar = alps_reservations.pop()) != NULL)
    delete ar;

  alps_reservations.unlock();
  } // END clear_all_alps_reservations()


