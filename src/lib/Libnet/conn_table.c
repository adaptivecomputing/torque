#include <license_pbs.h> /* See here for the software license */
#include <pbs_config.h> /* needed for PTHREAD_MUTEX_ERRORCHECK */

#include <pthread.h> /* pthread_mutex_* */
#include "../Libnet/lib_net.h"
#include "../Libutils/u_lock_ctl.h" /* lock_conn_table, unlock_conn_table */
#include "pbs_error.h" /* PBSE_*, FALSE */
#include "server_limits.h" /* PBS_NET_MAX_CONNECTIONS */
#include "libpbs.h" /* connect_handle */


extern struct connect_handle connection[];
extern struct connection     svr_conn[];

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

  *conn_pos = -1;
  pthread_mutexattr_init(&t_attr);
  pthread_mutexattr_settype(&t_attr, PTHREAD_MUTEX_ERRORCHECK);

  for (pos = 0; pos < PBS_NET_MAX_CONNECTIONS; pos++)
    { 
    lock_conn_table();
    if (connection[pos].ch_mutex == NULL)
      { 
      if ((tmp_mut = (pthread_mutex_t *)calloc(1, sizeof(pthread_mutex_t))) == NULL)
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



/*
 * socket_to_handle() - turn a socket into a connection handle
 * as used by the libpbs.a routines.
 *
 * Returns: >=0 connection handle if successful, or
 *    -1 if error, error number set in local_errno.
 */

int socket_to_handle(

  int  sock,        /* opened socket */
  int *local_errno) /* O */

  {
  int   conn_pos = 0;
  int   rc = PBSE_NONE;

  if ((rc = get_connection_entry(&conn_pos)) != PBSE_NONE)
    {
    *local_errno = PBSE_NOCONNECTS;
    }
  else
    {
    /* NOTE: get_connection_entry() locks connection[conn_pos] */
    connection[conn_pos].ch_stream = 0;
    connection[conn_pos].ch_inuse  = TRUE;
    connection[conn_pos].ch_errno  = 0;
    connection[conn_pos].ch_socket = sock;
    connection[conn_pos].ch_errtxt = 0;

    /* SUCCESS - save handle for later close */
    pthread_mutex_unlock(connection[conn_pos].ch_mutex);
      
    pthread_mutex_lock(svr_conn[sock].cn_mutex);
    svr_conn[sock].cn_handle = conn_pos;
    pthread_mutex_unlock(svr_conn[sock].cn_mutex);

    rc = conn_pos;
    }

  return(rc);
  }  /* END socket_to_handle() */

