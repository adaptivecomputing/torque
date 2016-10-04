#include <stdlib.h>
#include <stdio.h>

bool get_svr_attr_l_valset = true;
bool get_svr_attr_l_val = true;
int    LOGLEVEL = 10;

int get_svr_attr_l(

  int i,
  long *l)

  {
  return(0);
  }

int get_svr_attr_b(int index, bool *b)
  {
  if (get_svr_attr_l_valset == true)
    *b = get_svr_attr_l_val;

  return(0);
  }
