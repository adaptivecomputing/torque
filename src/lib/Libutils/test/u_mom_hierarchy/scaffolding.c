#include "license_pbs.h" /* See here for the software license */
#include <stdlib.h>
#include <stdio.h>
#include <sys/socket.h> /* struct sockaddr */
#include <netinet/in.h> /* in_addr_t */
#include <string>
#include <errno.h>

#include "tcp.h"
#include "log.h" /* LOG_BUF_SIZE */
#include "mom_hierarchy.h"

mom_hierarchy_t *mh;
int pbs_errno;
time_t time_now;
int   LOGLEVEL=0;
char log_buffer[LOG_BUF_SIZE];
int  mom_hierarchy_retry_time;

const char *dis_emsg[] =
  {
  "No error",
  "Input value too large to convert to this type",
  "Tried to write floating point infinity",
  "Negative sign on an unsigned datum",
  "Input count or value has leading zero",
  "Non-digit found where a digit was expected",
  "Input string has an embedded ASCII NUL",
  "Premature end of message",
  "Unable to calloc enough space for string",
  "Supporting protocol failure",
  "Protocol failure in commit",
  "End of File"
  };

void DIS_tcp_reset(int fd, int i)
  { 
  fprintf(stderr, "The call to DIS_tcp_reset needs to be mocked!!\n");
  exit(1);
  }

int diswsl(tcp_chan *chan, long value)
  { 
  fprintf(stderr, "The call to diswsl needs to be mocked!!\n");
  exit(1);
  }

int DIS_tcp_wflush(tcp_chan *chan)
  { 
  fprintf(stderr, "The call to DIS_tcp_wflush needs to be mocked!!\n");
  exit(1);
  }

long disrsl(tcp_chan *chan, int *retval)
  { 
  fprintf(stderr, "The call to disrsl needs to be mocked!!\n");
  exit(1);
  }

void log_err(int errnum, const char *routine, const char *text)
  {
  fprintf(stderr,"ERROR: Function: %s Message: %s\n",routine,text);
  }

extern int dummySocket;
extern int dummySocketAfterRetries;
extern int connectAddrRetries;

int socket_connect_addr(int &local_socket, struct sockaddr *remote, size_t remote_size, int is_privileged, std::string &err_msg)
  {
  if(connectAddrRetries-- <= 0)
    {
    return dummySocketAfterRetries;
    }
  errno = dummySocket;
  return dummySocket;
  }

extern int tcpSocket;

int socket_get_tcp_priv()
  {
  return tcpSocket;
  }

void log_event(int eventtype, int objclass, const char *objname, const char *text)
  {
  }

char *trim(char *str)
  {
  return(str);
  }

int pbs_getaddrinfo(
    
  const char       *pNode,
  struct addrinfo  *pHints,
  struct addrinfo **ppAddrInfoOut)

  {
  return(0);
  }

char *threadsafe_tokenizer(

  char **str,    /* M */
  const char  *delims) /* I */

  {
  return(NULL);
  }

ssize_t read_ac_socket(

  int     fd,
  void   *buf,
  ssize_t count)

  {
  return(0);
  }

int get_parent_and_child(

  char  *start,       /* I */
  char **parent,      /* O */
  char **child,       /* O */
  char **end)         /* O */

  {
  return(0);
  }
