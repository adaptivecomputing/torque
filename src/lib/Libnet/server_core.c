#include "license_pbs.h" /* See here for the software license */
#include "lib_net.h"
#include <arpa/inet.h> /* inet_addr */
#include <unistd.h> /* close */
#include <errno.h> /* errno, strerror */
#include <stdio.h> /* printf */

#include "pbs_error.h" /* PBSE_NONE */
#include "log.h" /* log_event, PBSEVENT_JOB, PBS_EVENTCLASS_JOB */
#include "../Liblog/log_event.h" /* log_event */
#include "../Libifl/lib_ifl.h" /* process_svr_conn */


/* Note, in extremely high load cases, the alloc value in /proc/net/sockstat can exceed the max value. This will substantially slow down throughput and generate connection failures (accept gets a EMFILE error). As the client is designed to run on each submit host, that issue shouldn't occur. The client must be restarted to clear out this issue. */
int start_listener(
    char *server_ip,
    int server_port,
    void *(*process_meth)(void *))
  {
  struct sockaddr_in adr_svr, adr_client;
  int rc = PBSE_NONE;
  int sockoptval;
  int len_inet = sizeof(struct sockaddr_in);
  char *ptr = NULL;
  int debug = 0;
  int *new_conn_port = NULL;
  int listen_socket = 0;
  int total_cntr = 0;
  pthread_t tid;
  pthread_attr_t t_attr;
  adr_svr.sin_family = AF_INET;

  if ((ptr = getenv("PBSDEBUG")) != NULL)
    debug = 1;
  if (!(adr_svr.sin_port = htons(server_port)))
    {
    }
  else if ((adr_svr.sin_addr.s_addr = inet_addr(server_ip)) == INADDR_NONE)
    {
    rc = PBSE_SOCKET_FAULT;
    }
  else if ((listen_socket = socket_get_tcp()) < 0)
    {
    /* Can not get socket for listening */
    rc = PBSE_SOCKET_FAULT;
    }
  else if (bind(listen_socket, (struct sockaddr *)&adr_svr, sizeof(struct sockaddr_in)) == -1)
    {
    /* Can not bind local socket */
    rc = PBSE_SOCKET_FAULT;
    }
  else if (listen(listen_socket, 128) == -1)
    {
    /* Can not listener on local socket */
    rc = PBSE_SOCKET_LISTEN;
    }
  else if ((rc = pthread_attr_init(&t_attr)) != 0)
    {
    /* Can not init thread attribute structure */
    rc = PBSE_THREADATTR;
    }
  else if ((rc = pthread_attr_setdetachstate(&t_attr, PTHREAD_CREATE_DETACHED)) != 0)
    {
    /* Can not set thread initial state as detached */
    pthread_attr_destroy(&t_attr);
    }
  else
    {
    while (1)
      {
      new_conn_port = (int *)calloc(1, sizeof(int));
      if ((*new_conn_port = accept(listen_socket, (struct sockaddr *)&adr_client, (socklen_t *)&len_inet)) == -1)
        {
        if (errno == EMFILE)
          {
          sleep(1);
          printf("Temporary pause\n");
          }
        else
          {
          printf("error in accept %s\n", strerror(errno));
          break;
          }
        errno = 0;
        close(*new_conn_port);
        free(new_conn_port);
        new_conn_port = NULL;
        }
      else
        {
        sockoptval = 1;
        setsockopt(*new_conn_port, SOL_SOCKET, SO_REUSEADDR, (void *)&sockoptval, sizeof(sockoptval));
        if (debug == 1)
          {
          process_meth((void *)new_conn_port);
          }
        else
          {
          pthread_create(&tid, &t_attr, process_meth, (void *)new_conn_port);
          }
        }
      if (debug == 1)
        {
        if (total_cntr % 1000 == 0)
          {
          printf("Total requests: %d\n", total_cntr);
          }
        total_cntr++;
        }
      }
    if (new_conn_port != NULL)
      {
      free(new_conn_port);
      }
    pthread_attr_destroy(&t_attr);
    log_event(PBSEVENT_JOB, PBS_EVENTCLASS_JOB, "net_srvr",
                "Socket close of network listener requested");
    }
  close(listen_socket);
  return rc;
  }
