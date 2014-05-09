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

#include "delete_all_tracker.hpp"
#include "attribute.h"

const char *mgr = "manager";

delete_all_tracker::delete_all_tracker()

  {
  pthread_mutex_init(&this->lock, NULL);
  std::string manager(mgr);
  std::pair<std::string, bool> mgr_pair(manager, false);
  this->qdel_map.insert(mgr_pair);
  }



delete_all_tracker::delete_all_tracker(
    
  const delete_all_tracker &other) : qdel_map(other.qdel_map)

  {
  pthread_mutex_init(&this->lock, NULL);
  }



delete_all_tracker::~delete_all_tracker()

  {
  }



/*
 * is_manager()
 *
 * @param perm - the permissions of the user
 * @return true - if the user has manager permissions
 */

bool is_manager(

  int perm)

  {
  return((perm & ATR_DFLAG_MGWR) != 0);
  }



/*
 * currently_deleting_all()
 */

bool delete_all_tracker::currently_deleting_all(
    
  const char *user,
  int         perm)

  {
  std::string user_name;
  bool        deleting = false;

  if (is_manager(perm))
    user_name = mgr;
  else
    user_name = user;

  pthread_mutex_lock(&this->lock);

  try
    {
    deleting = this->qdel_map[user_name];
    }
  catch (...)
    {
    deleting = false;
    }
  
  pthread_mutex_unlock(&this->lock);

  return(deleting);
  }



bool delete_all_tracker::start_deleting_all_if_possible(
    
  const char *user,
  int         perm)

  {
  std::string user_name;
  bool        can_start = false;
  bool        deleting;

  if (is_manager(perm))
    user_name = mgr;
  else
    user_name = user;
  
  pthread_mutex_lock(&this->lock);

  try
    {
    deleting = this->qdel_map[user_name];
    }
  catch (...)
    {
    deleting = false;
    }
  
  if (deleting == false)
    {
    this->qdel_map[user_name] = true;
    can_start = true;
    }
  
  pthread_mutex_unlock(&this->lock);

  return(can_start);
  }



void delete_all_tracker::done_deleting_all(
    
  const char *user,
  int         perm)

  {
  std::string user_name;

  if (is_manager(perm))
    user_name = mgr;
  else
    user_name = user;

  pthread_mutex_lock(&this->lock);

  try
    {
    this->qdel_map[user_name] = false;
    }
  catch (...) {}
  
  pthread_mutex_unlock(&this->lock);
  }
