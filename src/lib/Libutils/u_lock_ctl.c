#include <license_pbs.h> /* See here for the software license */
#include <pbs_config.h> /* Needed for PTHREAD_MUTEX_NORMAL */
#include "u_lock_ctl.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "../Liblog/pbs_log.h" /* log_err */
#include "pbs_error.h" /* PBSE_NONE */
#include "log.h" /* PBSEVENT_SYSTEM, PBSEVENT_FORCE, PBS_EVENTCLASS_SERVER */

/* This is only used in this file. All access is through the methods */
lock_ctl *locks = NULL;
lock_cntr *cntr = NULL;
#define MSG_LEN_SHORT 60
#define MSG_LEN_LONG 160

/* additional lock, unlock method exist in queue_func.*, node_func.* */
int lock_init()

  {
  int rc = PBSE_NONE;
  pthread_mutexattr_t startup_attr;
  pthread_mutexattr_t conn_attr;
  pthread_mutexattr_t tcp_attr;
  pthread_mutexattr_t ss_attr;
  pthread_mutexattr_init(&startup_attr);
  pthread_mutexattr_settype(&startup_attr, PTHREAD_MUTEX_NORMAL);
  pthread_mutexattr_init(&conn_attr);
  pthread_mutexattr_settype(&conn_attr, PTHREAD_MUTEX_NORMAL);
  pthread_mutexattr_init(&tcp_attr);
  pthread_mutexattr_settype(&tcp_attr, PTHREAD_MUTEX_NORMAL);
  pthread_mutexattr_init(&ss_attr);
  pthread_mutexattr_settype(&ss_attr, PTHREAD_MUTEX_NORMAL);

  if ((locks = (lock_ctl *)calloc(1, sizeof(lock_ctl))) == NULL)
    rc = PBSE_MEM_MALLOC;
  else if ((locks->startup = (pthread_mutex_t *)calloc(1, sizeof(pthread_mutex_t))) == NULL)
    rc = PBSE_MEM_MALLOC;
  else if ((locks->conn_table = (pthread_mutex_t *)calloc(1, sizeof(pthread_mutex_t))) == NULL)
    rc = PBSE_MEM_MALLOC;
  else if ((locks->setup_save = (pthread_mutex_t *)calloc(1, sizeof(pthread_mutex_t))) == NULL)
    rc = PBSE_MEM_MALLOC;
  else
    {
    pthread_mutex_init(locks->startup, &startup_attr);
    pthread_mutex_init(locks->conn_table, &conn_attr);
    pthread_mutex_init(locks->setup_save, &ss_attr);
    }

  return(rc);
  } /* END lock_init() */




void lock_destroy()

  {
  pthread_mutex_destroy(locks->startup);
  free(locks->startup);

  pthread_mutex_destroy(locks->conn_table);
  free(locks->conn_table);

  pthread_mutex_destroy(locks->setup_save);
  free(locks->setup_save);

  free(locks);
  locks = NULL;
  } /* END lock_destroy() */




int lock_startup()

  {
  int rc = PBSE_NONE;

  if (locks == NULL)
    {
    if ((rc = lock_init()) == PBSE_NONE)
      {
      if (pthread_mutex_lock(locks->startup) != 0)
        {
        log_err(-1,"mutex_lock","ALERT:   cannot lock startup mutex!\n");
        return(PBSE_MUTEX);
        }
      }
    }

  return(rc);
  } /* END lock_startup() */




int unlock_startup()

  {
  if (pthread_mutex_unlock(locks->startup) != 0)
    {
    log_err(-1,"mutex_unlock","ALERT:   cannot unlock startup mutex!\n");
    return(PBSE_MUTEX);
    }

  return(PBSE_NONE);
  } /* END unlock_startup() */




int lock_conn_table()

  {
  int rc = PBSE_NONE;

  if (locks == NULL)
    {
    if ((rc = lock_init()) == PBSE_NONE)
      {
      lock_init();
      
      if (pthread_mutex_lock(locks->conn_table) != 0)
        {
        log_err(-1,"mutex_lock","ALERT:   cannot lock conn_table mutex!\n");
        return(PBSE_MUTEX);
        }
      }
    }

  return(rc);
  } /* END lock_conn_table() */




int unlock_conn_table()
  {
  if (pthread_mutex_unlock(locks->conn_table) != 0)
    {
    log_err(-1, "mutex_unlock", "ALERT:   cannot unlock conn_table mutex!\n");
    return(PBSE_MUTEX);
    }

  return(PBSE_NONE);
  } /* END unlock_conn_table() */




int lock_ss()

  {
  int rc = PBSE_NONE;

  if (locks == NULL)
    {
    if ((rc = lock_init()) == PBSE_NONE)
      {
    
      if (pthread_mutex_lock(locks->setup_save) != 0)
        {
        log_err(-1,"mutex_lock","ALERT:   cannot lock setup_save mutex!\n");
        return(PBSE_MUTEX);
        }
      }
    }

  return(rc);
  } /* END lock_ss() */




int unlock_ss()
  {
  if (pthread_mutex_unlock(locks->setup_save) != 0)
    {
    log_err(-1,"mutex_unlock","ALERT:   cannot unlock setup_save mutex!\n");
    return(PBSE_MUTEX);
    }

  return(PBSE_NONE);
  } /* END unlock_ss() */




int lock_node(
    
  struct pbsnode *the_node,
  const char     *id,
  char           *msg,
  int             logging)

  {
  int   rc = PBSE_NONE;
  char  err_msg[MSG_LEN_LONG + 1];
  char  stub_msg[] = "no pos";
  
  if (logging >= 10)
    {
    if (msg == NULL)
      msg = stub_msg;
    snprintf(err_msg, MSG_LEN_LONG, "locking start %s in method %s-%s", the_node->nd_name, id, msg);
    log_record(PBSEVENT_DEBUG, PBS_EVENTCLASS_NODE, __func__, err_msg);
    }

  
  if (pthread_mutex_lock(the_node->nd_mutex) != 0)
    {
    if (logging >= 10)
      {
      snprintf(err_msg, MSG_LEN_LONG, "ALERT: cannot lock node %s mutex in method %s",
          the_node->nd_name, id);
      log_record(PBSEVENT_DEBUG, PBS_EVENTCLASS_NODE, __func__, err_msg);
      }
    rc = PBSE_MUTEX;
    }
  
  if (logging >= 7)
    {
    snprintf(err_msg, MSG_LEN_LONG, "locking complete %s in method %s", the_node->nd_name, id);
    log_record(PBSEVENT_DEBUG, PBS_EVENTCLASS_NODE, __func__, err_msg);
    }

  return rc;
  } /* END lock_node() */

int unlock_node(
    
  struct pbsnode *the_node,
  const char     *id,
  char           *msg,
  int             logging)

  {
  int   rc = PBSE_NONE;
  char  err_msg[MSG_LEN_LONG + 1];
  char  stub_msg[] = "no pos";

  if (logging >= 10)
    {
    if (msg == NULL)
      msg = stub_msg;
    snprintf(err_msg, MSG_LEN_LONG, "unlocking %s in method %s-%s", the_node->nd_name, id, msg);
    log_record(PBSEVENT_DEBUG, PBS_EVENTCLASS_NODE, __func__, err_msg);
    }

  if (pthread_mutex_unlock(the_node->nd_mutex) != 0)
    {
    if (logging >= 10)
      {
      snprintf(err_msg, MSG_LEN_LONG, "ALERT: cannot unlock node %s mutex in method %s",
          the_node->nd_name, id);
      log_record(PBSEVENT_DEBUG, PBS_EVENTCLASS_NODE, __func__, err_msg);
      }
    rc = PBSE_MUTEX;
    }

  return rc;
  } /* END unlock_node() */

int lock_cntr_init()
  {
  int rc = PBSE_NONE;
  pthread_mutexattr_t cntr_attr;
  pthread_mutexattr_init(&cntr_attr);
  pthread_mutexattr_settype(&cntr_attr, PTHREAD_MUTEX_NORMAL);
  if ((cntr = (lock_cntr *)calloc(1, sizeof(lock_cntr))) == NULL)
    rc = PBSE_MEM_MALLOC;
  else if ((cntr->the_lock = (pthread_mutex_t *)calloc(1, sizeof(pthread_mutex_t))) == NULL)
    rc = PBSE_MEM_MALLOC;
  else
    {
    memset(cntr->the_lock, 0, sizeof(pthread_mutex_t));
    pthread_mutex_init(cntr->the_lock, &cntr_attr);
    }
  return rc;
  }

int lock_cntr_incr()
  {
  int rc = PBSE_NONE;
  char err_msg[MSG_LEN_SHORT];
  if (pthread_mutex_lock(cntr->the_lock) != 0)
    {
    snprintf(err_msg, MSG_LEN_SHORT, "ALERT: cannot lock mutex for lock_cntr_incr");
    log_record(PBSEVENT_DEBUG, PBS_EVENTCLASS_NODE, "lock_cntr_incr", err_msg);
    rc = PBSE_MUTEX;
    }
  else
    {
    cntr->the_num++;
    }
  if (rc != PBSE_NONE)
    {
    }
  else if (pthread_mutex_unlock(cntr->the_lock) != 0)
    {
    snprintf(err_msg, MSG_LEN_SHORT, "ALERT: cannot unlock mutex for cntr_llock_cntr_incr");
    log_record(PBSEVENT_DEBUG, PBS_EVENTCLASS_NODE, "lock_cntr_incr", err_msg);
    rc = PBSE_MUTEX;
    }
  return rc;
  }

int lock_cntr_decr()
  {
  int rc = PBSE_NONE;
  char err_msg[MSG_LEN_SHORT];
  if (pthread_mutex_lock(cntr->the_lock) != 0)
    {
    snprintf(err_msg, MSG_LEN_SHORT, "ALERT: cannot lock mutex for lock_cntr_decr");
    log_record(PBSEVENT_DEBUG, PBS_EVENTCLASS_NODE, "lock_cntr_decr", err_msg);
    rc = PBSE_MUTEX;
    }
  else
    {
    cntr->the_num--;
    }
  if (rc != PBSE_NONE)
    {
    }
  else if (pthread_mutex_unlock(cntr->the_lock) != 0)
    {
    snprintf(err_msg, MSG_LEN_SHORT, "ALERT: cannot unlock mutex for lock_cntr_decr");
    log_record(PBSEVENT_DEBUG, PBS_EVENTCLASS_NODE, "lock_cntr_decr", err_msg);
    rc = PBSE_MUTEX;
    }
  return rc;
  }

void lock_cntr_display()
  {
  char err_msg[MSG_LEN_SHORT];
  snprintf(err_msg, MSG_LEN_SHORT, "Total cntr value:[%d]", cntr->the_num);
  log_record(PBSEVENT_DEBUG, PBS_EVENTCLASS_NODE, "lock_cntr_display", err_msg);
  }

void lock_cntr_destroy()
  {
  pthread_mutex_destroy(cntr->the_lock);
  free(cntr->the_lock);
  free(cntr);
  }
