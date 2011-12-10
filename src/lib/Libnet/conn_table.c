#include <license_pbs.h> /* See here for the software license */
#include <pbs_config.h> /* needed for PTHREAD_MUTEX_ERRORCHECK */

#include <pthread.h> /* pthread_mutex_* */
#include "lib_net.h"
#include "../Libutils/u_lock_ctl.h" /* lock_conn_table, unlock_conn_table */
#include "pbs_error.h" /* PBSE_*, FALSE */
#include "server_limits.h" /* PBS_NET_MAX_CONNECTIONS */
#include "libpbs.h" /* connect_handle */


extern struct connect_handle connection[];
/*
 * finds and locks an entry in the connection table or returns an error
 */ 
int get_connection_entry(
    
  int *conn_pos)

  {
  int                  rc = PBSE_NONE;
  int                  pos = 0;
  pthread_mutex_t     *tmp_mut = NULL;
  pthread_mutexattr_t  t_attr;

  pthread_mutexattr_init(&t_attr);
  pthread_mutexattr_settype(&t_attr, PTHREAD_MUTEX_ERRORCHECK);

  for (pos = 0; pos < PBS_NET_MAX_CONNECTIONS; pos++)
    { 
    lock_conn_table();
    if (connection[pos].ch_mutex == NULL)
      { 
      if ((tmp_mut = calloc(1, sizeof(pthread_mutex_t))) == NULL)
        rc = PBSE_MEM_MALLOC;
      else 
        { 
        connection[pos].ch_mutex = tmp_mut;
        pthread_mutex_init(connection[pos].ch_mutex, &t_attr);
        }
      }
    unlock_conn_table();
    
    if (pthread_mutex_trylock(connection[pos].ch_mutex) != 0)
      {
      /* if we can't lock the mutex, it is busy*/
      continue; 
      }

    /* Why are we checking for this? Isn't it a bug if it's not locked while
     * in use? */
    if (connection[pos].ch_inuse == FALSE)
      {
      *conn_pos = pos;
      break;
      }
    else
      pthread_mutex_unlock(connection[pos].ch_mutex);
    }

  if (*conn_pos == -1)
    {
    if (rc == PBSE_NONE)
      rc = PBSE_NOCONNECTS;
    }

  return(rc);
  } /* END get_connection_entry() */


