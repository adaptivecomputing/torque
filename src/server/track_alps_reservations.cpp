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

#include "track_alps_reservations.hpp"
#include "utils.h"
#include "batch_request.h"
#include "ji_mutex.h"
#include "id_map.hpp"

/* Global Data declarations */
extern int LOGLEVEL;



reservation_holder::reservation_holder() : reservations(), orphaned_reservations()
  {
  pthread_mutex_init(&this->rh_mutex, NULL);
  }



/* 
 * track_alps_reservation
 * creates an alps reservation based 
 */

int reservation_holder::track_alps_reservation(
    
  job *pjob)

  {
  /* job has no alps reservation, nothing to record */
  if (pjob->ji_wattr[JOB_ATR_reservation_id].at_val.at_str == NULL)
    return(PBSE_NONE);
  
  int              rc = PBSE_NONE;
  alps_reservation ar(pjob);

  pthread_mutex_lock(&this->rh_mutex);
  this->reservations[ar.rsv_id] = ar;
  pthread_mutex_unlock(&this->rh_mutex);

  return(rc);
  } /* track_alps_reservation() */



/*
 * insert_alps_reservation()
 */

int reservation_holder::insert_alps_reservation(
    
  const alps_reservation &ar)

  {
  int rc = PBSE_NONE;

  pthread_mutex_lock(&this->rh_mutex);
  this->reservations[ar.rsv_id] = ar;
  pthread_mutex_unlock(&this->rh_mutex);

  return(rc);
  } /* insert_alps_reservation() */



/*
 * already_recorded()
 */

bool reservation_holder::already_recorded(

  const char *rsv_id)

  {
  bool recorded;

  pthread_mutex_lock(&this->rh_mutex);

  recorded = this->reservations.find(rsv_id) != this->reservations.end();
  
  pthread_mutex_unlock(&this->rh_mutex);

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

bool reservation_holder::is_orphaned(

  const char  *rsv_id,
  std::string &job_id)

  {
  bool              orphaned = false;
  job              *pjob;
  std::map<std::string, alps_reservation>::iterator it;

  pthread_mutex_lock(&this->rh_mutex);
  it = this->reservations.find(rsv_id);

  if (it != this->reservations.end())
    {
    job_id = job_mapper.get_name(it->second.internal_job_id);

    if ((pjob = svr_find_job_by_id(it->second.internal_job_id)) != NULL)
      {
      if (pjob->ji_qs.ji_state == JOB_STATE_COMPLETE)
        orphaned = true;

      unlock_ji_mutex(pjob, __func__, "1", LOGLEVEL);
      }
    else
      {
      orphaned = true;
      }
    }
  else
    {
    job_id = "unknown";
    orphaned = true;
    }

  if (orphaned == true)
    {
    if (this->orphaned_reservations.find(rsv_id) != this->orphaned_reservations.end())
      {
      // Only send one release reservation request at a time.
      orphaned = false;
      }
    else
      {
      // Record this so we only send one at a time
      this->orphaned_reservations.insert(rsv_id);
      }
    }

  pthread_mutex_unlock(&this->rh_mutex);

  return(orphaned);
  } /* END is_orphaned() */



/*
 * remove_alps_reservation
 * remove the reservation with rsv_id from the alps_reservations struct
 *
 * @param rsv_id - the id of the reservation to remove
 * @return PBSE_NONE if removed, THING_NOT_FOUND if it wasn't present
 */

int reservation_holder::remove_alps_reservation(
    
  const char *rsv_id)

  {
  int                                               rc = PBSE_NONE;
  std::map<std::string, alps_reservation>::iterator it;

  pthread_mutex_lock(&this->rh_mutex);
  it = this->reservations.find(rsv_id);
  if (it == this->reservations.end())
    rc = THING_NOT_FOUND;
  else
    this->reservations.erase(it);

  pthread_mutex_unlock(&this->rh_mutex);

  return(rc);
  } /* END remove_alps_reservation() */



void reservation_holder::remove_from_orphaned_list(
    
  const char *rsv_id)

  {
  pthread_mutex_lock(&this->rh_mutex);

  this->orphaned_reservations.erase(rsv_id);

  pthread_mutex_unlock(&this->rh_mutex);
  }


void reservation_holder::clear()

  {
  pthread_mutex_lock(&this->rh_mutex);

  this->reservations.clear();
  this->orphaned_reservations.clear();

  pthread_mutex_unlock(&this->rh_mutex);
  }



int reservation_holder::count()

  {
  int size;
  pthread_mutex_lock(&this->rh_mutex);

  size = this->reservations.size();

  pthread_mutex_unlock(&this->rh_mutex);

  return(size);
  }



