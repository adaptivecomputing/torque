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
    {
    int rc;

    /* validate the mutex */
    if (mutex == NULL)
      {
      mutex_valid = false;
      return;
      }

    managed_mutex = mutex;
    mutex_valid = true;
    locked = false;
    if (is_locked == false)
      {
      rc = lock();
      if (rc != PBSE_NONE)
        {
        mutex_valid = false;
        unlock_on_exit = false;
        return;
        }
      }

    unlock_on_exit = true;
    locked = true;
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

  /* allows user to request a mutes not be
   * unlocked by the destructor
   */
  void mutex_mgr::set_lock_on_exit(bool val)
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

