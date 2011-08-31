#include <license_pbs.h> /* See here for the software license */
#include <pbs_config.h> /* Needed for PTHREAD_MUTEX_ERRORCHECK */
#include "u_lock_ctl.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "../Liblog/pbs_log.h" /* log_err */
#include "pbs_error.h" /* PBSE_NONE */
#include "log.h" /* PBSEVENT_SYSTEM, PBSEVENT_FORCE, PBS_EVENTCLASS_SERVER */

/* This is only used in this file. All access is through the methods */
lock_ctl *locks = NULL;

int lock_init()
  {
  int rc = PBSE_NONE;
  pthread_mutexattr_t t_attr;
  pthread_mutexattr_settype(&t_attr, PTHREAD_MUTEX_ERRORCHECK);
  memset(&t_attr, 0, sizeof(pthread_mutexattr_t));
  if ((locks = (lock_ctl *)calloc(1, sizeof(lock_ctl))) == NULL)
    rc = PBSE_MEM_MALLOC;
  else if ((locks->startup = (pthread_mutex_t *)calloc(1, sizeof(pthread_mutex_t))) == NULL)
    rc = PBSE_MEM_MALLOC;
  else if ((locks->conn_table = (pthread_mutex_t *)calloc(1, sizeof(pthread_mutex_t))) == NULL)
    rc = PBSE_MEM_MALLOC;
  else if ((locks->tcp_table = (pthread_mutex_t *)calloc(1, sizeof(pthread_mutex_t))) == NULL)
    rc = PBSE_MEM_MALLOC;
  else
    {
    memset(locks->startup, 0, sizeof(pthread_mutex_t));
    pthread_mutex_init(locks->startup, &t_attr);

    memset(locks->conn_table, 0, sizeof(pthread_mutex_t));
    pthread_mutex_init(locks->conn_table, &t_attr);

    memset(locks->tcp_table, 0, sizeof(pthread_mutex_t));
    pthread_mutex_init(locks->tcp_table, &t_attr);
    }
  return rc;
  }

void lock_destroy()
  {
  pthread_mutex_destroy(locks->startup);
  free(locks->startup);
  pthread_mutex_destroy(locks->conn_table);
  free(locks->conn_table);
  pthread_mutex_destroy(locks->tcp_table);
  free(locks->tcp_table);
  free(locks);
  locks = NULL;
  }

int lock_startup()
  {
  if (locks == NULL)
    lock_init();
  if (pthread_mutex_lock(locks->startup) != 0)
    {
    log_err(-1,"mutex_lock","ALERT:   cannot lock startup mutex!\n");
    return(PBSE_MUTEX);
    }
  return(PBSE_NONE);
  }

int unlock_startup()
  {
  if (pthread_mutex_unlock(locks->startup) != 0)
    {
    log_err(-1,"mutex_unlock","ALERT:   cannot unlock startup mutex!\n");
    return(PBSE_MUTEX);
    }
  return(PBSE_NONE);
  }

int lock_conn_table()
  {
  if (locks == NULL)
    lock_init();
  if (pthread_mutex_lock(locks->conn_table) != 0)
    {
    log_err(-1,"mutex_lock","ALERT:   cannot lock conn_table mutex!\n");
    return(PBSE_MUTEX);
    }
  return(PBSE_NONE);
  }

int unlock_conn_table()
  {
  if (pthread_mutex_unlock(locks->conn_table) != 0)
    {
    log_err(-1,"mutex_unlock","ALERT:   cannot unlock conn_table mutex!\n");
    return(PBSE_MUTEX);
    }
  return(PBSE_NONE);
  }

int lock_tcp_table()
  {
  if (locks == NULL)
    lock_init();
  if (pthread_mutex_lock(locks->tcp_table) != 0)
    {
    log_err(-1,"mutex_lock","ALERT:   cannot lock tcp_table mutex!\n");
    return(PBSE_MUTEX);
    }
  return(PBSE_NONE);
  }

int unlock_tcp_table()
  {
  if (pthread_mutex_unlock(locks->tcp_table) != 0)
    {
    log_err(-1,"mutex_unlock","ALERT:   cannot unlock tcp_table mutex!\n");
    return(PBSE_MUTEX);
    }
  return(PBSE_NONE);
  }

