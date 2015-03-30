#include <stdlib.h>
#include <stdio.h>

int global_is_svr_attr_set_rc = 0;

int is_svr_attr_set(int i)
  {
  return(global_is_svr_attr_set_rc);
  }
