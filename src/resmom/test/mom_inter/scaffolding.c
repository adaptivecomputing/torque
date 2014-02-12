#include "license_pbs.h" /* See here for the software license */
#include <stdlib.h>
#include <stdio.h>
#include "port_forwarding.h" /* pfwdsock */

int mom_reader_go;
int DEBUGMODE;
char *xauth_path;

ssize_t read_ac_socket(int fd, void *buf, ssize_t count)
  {
  fprintf(stderr, "The call to read_nonblocking_socket needs to be mocked!!\n");
  exit(1);
  }

ssize_t read_blocking_socket(int fd, void *buf, ssize_t count)
  {
  return(0);
  }

void port_forwarder( struct pfwdsock *socks, int (*connfunc)(char *, long, char *), char *phost, int pport, char *EMsg)
  {
  fprintf(stderr, "The call to port_forwarder needs to be mocked!!\n");
  exit(1);
  }

ssize_t write_ac_socket(int fd, const void *buf, ssize_t count)
  {
  fprintf(stderr, "The call to write_nonblocking_socket needs to be mocked!!\n");
  exit(1);
  }

int conn_qsub(char *hostname, long port, char *EMsg)
  {
  fprintf(stderr, "The call to conn_qsub needs to be mocked!!\n");
  exit(1);
  }

int put_env_var(const char *name, const char *value)
  {
  return(0);
  }
