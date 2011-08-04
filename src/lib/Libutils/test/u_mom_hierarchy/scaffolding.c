#include "license_pbs.h" /* See here for the software license */
#include <stdlib.h>
#include <stdio.h>

#include "log.h" /* LOG_BUF_SIZE */
#include "resizable_array.h" /* resizable_array */
int pbs_errno;
time_t time_now;
char log_buffer[LOG_BUF_SIZE];

int insert_thing(resizable_array *ra, void *thing)
  { 
  fprintf(stderr, "The call to insert_thing needs to be mocked!!\n");
  exit(1);
  }

void DIS_tcp_reset(int fd, int i)
  { 
  fprintf(stderr, "The call to DIS_tcp_reset needs to be mocked!!\n");
  exit(1);
  }

int diswsl(int stream, long value)
  { 
  fprintf(stderr, "The call to diswsl needs to be mocked!!\n");
  exit(1);
  }

int DIS_tcp_wflush(int fd)
  { 
  fprintf(stderr, "The call to DIS_tcp_wflush needs to be mocked!!\n");
  exit(1);
  }

resizable_array *initialize_resizable_array(int size)
  { 
  fprintf(stderr, "The call to initialize_resizable_array needs to be mocked!!\n");
  exit(1);
  }

long disrsl(int stream, int *retval)
  { 
  fprintf(stderr, "The call to disrsl needs to be mocked!!\n");
  exit(1);
  }

void log_err(int errnum, char *routine, char *text)
  { 
  fprintf(stderr, "The call to log_err needs to be mocked!!\n");
  exit(1);
  }

