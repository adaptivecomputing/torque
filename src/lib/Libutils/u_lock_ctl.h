#include <license_pbs.h> /* See here for the software license */
#ifndef _U_LOCK_CTL_H
#define _U_LOCK_CTL_H
#include <pthread.h> /* pthread_mutex_t */

typedef struct lock_ctl
  {
  /* used for startup/sequence control in the system */
  pthread_mutex_t *startup;
  /* connection table mutex. Currently only used during malloc. */
  pthread_mutex_t *conn_table;
  /* tcparray table mutex. Currently only used during malloc */
  pthread_mutex_t *tcp_table;
  } lock_ctl;

int lock_init();

void lock_destroy();

int lock_startup();

int unlock_startup();

int lock_conn_table();

int unlock_conn_table();

int lock_tcp_table();

int unlock_tcp_table();

#endif /* _U_LOCK_CTL_H */
