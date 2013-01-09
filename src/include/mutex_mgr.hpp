#include <pthread.h>

/* mutex_mgr 
 * This class is used to manage pthread mutexes.
 * The private variable unlock_on_exit tells
 * the destructor whether or not to unlock the managed
 * mutex. 
 * here are three constructors all of which initialize 
 * unlock_on_exit to true. unlock_on_exit can be set
 * to true or false with the method set_lock_on_exit.
 * The method detach() unlocks the managed mutex.
 * the method attach() locks the managed mutex.
 */
class mutex_mgr
  {
  bool unlock_on_exit;
  bool locked;
  bool mutex_valid;
  pthread_mutex_t *managed_mutex;

  public:
    mutex_mgr& operator= (const mutex_mgr &newMutexMgr);
    mutex_mgr(const mutex_mgr& newMutexMgr);
    mutex_mgr(pthread_mutex_t *mutex, bool is_locked = false);
    ~mutex_mgr();
    int unlock();
    int lock();
    void set_lock_on_exit(bool val);
    bool is_valid();
  };

