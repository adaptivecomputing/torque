#include "license_pbs.h" /* See here for the software license */
#include <stdlib.h>
#include <stdio.h>
#include "libpbs.h" /* connect_handle */

struct connect_handle connection[10];

int unlock_conn_table()
  {
  fprintf(stderr, "The call to unlock_conn_table needs to be mocked!!\n");
  exit(1);
  }

int lock_conn_table()
  {
  fprintf(stderr, "The call to lock_conn_table needs to be mocked!!\n");
  exit(1);
  }
