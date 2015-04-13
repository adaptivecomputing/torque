#include <stdlib.h>
#include <stdio.h>

bool get_svr_attr_l_valset = true;
long get_svr_attr_l_val = 1;

int get_svr_attr_l(

  int i,
  long *l)

  {
  if (get_svr_attr_l_valset)
     *l = get_svr_attr_l_val;

  return(0);
  }
