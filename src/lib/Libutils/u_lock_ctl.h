#include <license_pbs.h> /* See here for the software license */
#ifndef _U_LOCK_CTL_H
#define _U_LOCK_CTL_H
#include <pthread.h> /* pthread_mutex_t */
#include "pbs_nodes.h" /* pbsnode */

typedef struct lock_ctl
  {
  /* used for startup/sequence control in the system */
  pthread_mutex_t *startup;
  /* connection table mutex. Currently only used during malloc. */
  pthread_mutex_t *conn_table;
  /* tcparray table mutex. Currently only used during malloc */
  pthread_mutex_t *tcp_table;
  /* for saving files on startup */
  pthread_mutex_t *setup_save;
  } lock_ctl;

typedef struct lock_cntr
  {
  int the_num;
  pthread_mutex_t *the_lock;
  } lock_cntr;

int lock_init();

void lock_destroy();

int lock_startup();

int unlock_startup();

int lock_conn_table();

int unlock_conn_table();

int lock_tcp_table();

int unlock_tcp_table();

int lock_ss();

int unlock_ss();

int lock_node(struct pbsnode *the_node, const char *method_name, char *msg, int logging);

int unlock_node(struct pbsnode *the_node, const char *method_name, char *msg, int logging);

int lock_cntr_init();

int lock_cntr_incr();

int lock_cntr_decr();

void lock_cntr_display();

void lock_cntr_destroy();
#endif /* _U_LOCK_CTL_H */
