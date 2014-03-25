#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "user_info.h"

bool exit_called = false;
int LOGLEVEL = 10;

void log_event(

    int eventtype,
    int objclass,
    const char *objname,
    const char *text)

  {
  return;
  }

int get_svr_attr_l(int index, long *l)
  {
  *l = 1;
  return(0);
  }

int num_array_jobs(const char *array_req)
  {
  return(11);
  }


void log_err(int error, const char *func_id, const char *msg) {}
