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

#include <pthread.h>
#include "mutex_mgr.hpp"
#include "pbs_error.h"

using namespace std;

  mutex_mgr& mutex_mgr::operator= (const mutex_mgr &newMutexMgr)
    {
    if (this == &newMutexMgr)
      return *this;

    unlock_on_exit = newMutexMgr.unlock_on_exit;
    locked = newMutexMgr.locked;
    mutex_valid = newMutexMgr.mutex_valid;
    managed_mutex = newMutexMgr.managed_mutex;
    return *this;
    }

  /* copy constructure for mutex_mgr */
  mutex_mgr::mutex_mgr(const mutex_mgr& newMutexMgr)
    {
    unlock_on_exit = newMutexMgr.unlock_on_exit;
    locked = newMutexMgr.locked;
    mutex_valid = newMutexMgr.mutex_valid;
    managed_mutex = newMutexMgr.managed_mutex;
    }

  /* This constructor saves the given mutex and 
   * locks it  based on the value of is_locked
   */
  mutex_mgr::mutex_mgr(pthread_mutex_t *mutex, bool is_locked) 
  : unlock_on_exit(true), locked(is_locked), mutex_valid(true), managed_mutex(mutex)
    {
    int rc;

    /* validate the mutex */
    if (mutex == NULL)
      {
      mutex_valid = false;
      return;
      }

    if (is_locked == false)
      {
      rc = lock();
      if ((rc != PBSE_NONE) && (rc != PBSE_MUTEX_ALREADY_LOCKED))
        {
        mutex_valid = false;
        unlock_on_exit = false;
        return;
        }
      }
    }

  /* The destructor checks for a valid mutex 
   * It then checks to see if we should unlock
   * on exit and if we are still locked. If so 
   * the mutex is unlocked and the object is freed
   */
 
  mutex_mgr::~mutex_mgr()
    {
    if (mutex_valid == false)
      return;

    if ((unlock_on_exit == true) && (locked == true))
      pthread_mutex_unlock(managed_mutex);
    }

  /* unlock the managed mutex */
  int mutex_mgr::unlock()
    {
    int rc;

    if (mutex_valid == false)
      return(PBSE_INVALID_MUTEX);

    if (locked == false)
      return(PBSE_MUTEX_ALREADY_UNLOCKED);
    
    rc = pthread_mutex_unlock(managed_mutex);
    if (rc != 0)
      return(PBSE_SYSTEM);
    else
      {
      locked = false;
      return(PBSE_NONE);
      }
    }

  /* locked the managed mutex */
  int mutex_mgr::lock()
    {
    int rc;

    if (mutex_valid == false)
      return(PBSE_INVALID_MUTEX);

    if (locked == true)
      return(PBSE_MUTEX_ALREADY_LOCKED);

    rc = pthread_mutex_lock(managed_mutex);
    if (rc != 0)
      return(PBSE_SYSTEM);
    else
      {
      locked = true;
      return(PBSE_NONE);
      }
    }


  void  mutex_mgr::set_lock_state(bool val)
    {
    locked = val;
    }

  /* allows user to request a mutex not be
   * unlocked by the destructor
   */
  void mutex_mgr::set_unlock_on_exit(bool val)
    {
    unlock_on_exit = val;
    }

  /* this method allows the user to check if the mutex 
     is valid */
  bool mutex_mgr::is_valid()
   {
   if (mutex_valid == true)
     return(true);
   else
     return(false);
   }

  void mutex_mgr::mark_as_locked()
    {
    locked = true;
    }

