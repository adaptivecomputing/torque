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
#include "../Libnet/lib_net.h"
#include "threadpool.h"
#include "../Liblog/pbs_log.h"

extern int debug_mode;
extern void *(*read_func[])(void *);
extern char *msg_daemonname;

/* Note, in extremely high load cases, the alloc value in /proc/net/sockstat can exceed the max value. This will substantially slow down throughput and generate connection failures (accept gets a EMFILE error). As the client is designed to run on each submit host, that issue shouldn't occur. The client must be restarted to clear out this issue. */
int start_listener(
    
  char   *server_ip,
  int     server_port,
  void *(*process_meth)(void *))

  {
  struct sockaddr_in  adr_svr;
  struct sockaddr_in  adr_client;
  int                 rc = PBSE_NONE;
  int                 sockoptval = 1;
  int                 len_inet = sizeof(struct sockaddr_in);
  int                *new_conn_port = NULL;
  int                 listen_socket = 0;
  int                 total_cntr = 0;
  pthread_t           tid;
  pthread_attr_t      t_attr;
  int objclass = 0;
  char msg_started[1024];

  memset(&adr_svr, 0, sizeof(adr_svr));
  adr_svr.sin_family = AF_INET;

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
  else if (setsockopt(listen_socket, SOL_SOCKET, SO_REUSEADDR, (void *)&sockoptval, sizeof(sockoptval)) == -1)
    {
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
    log_get_set_eventclass(&objclass, GETV);
    if (objclass == PBS_EVENTCLASS_TRQAUTHD)
      {
      snprintf(msg_started, sizeof(msg_started),
        "TORQUE authd daemon started and listening on IP:port %s:%d", server_ip, server_port);
      log_event(PBSEVENT_SYSTEM | PBSEVENT_FORCE, PBS_EVENTCLASS_TRQAUTHD,
        msg_daemonname, msg_started);
      }
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
        if (debug_mode == TRUE)
          {
          process_meth((void *)new_conn_port);
          }
        else
          {
          pthread_create(&tid, &t_attr, process_meth, (void *)new_conn_port);
          }
        }
      if (debug_mode == TRUE)
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

  return(rc);
  } /* END start_listener() */


int start_listener_addrinfo(

  char   *host_name,
  int     server_port,
  void *(*process_meth)(void *))

  {
  struct addrinfo    *adr_svr;
  struct sockaddr     adr_client;
  struct sockaddr_in *in_addr;
  struct sockaddr_in  svr_address;
  socklen_t           len_inet;
  int                 rc = PBSE_NONE;
  int                 sockoptval;
  int                *new_conn_port = NULL;
  int                 listen_socket = 0;
  int                 total_cntr = 0;
  unsigned short      port_net_byte_order;
  pthread_attr_t      t_attr;

  if (!(getaddrinfo(host_name, NULL, NULL, &adr_svr) == 0))
    {
    rc = PBSE_SOCKET_FAULT;
    }

  port_net_byte_order = htons(server_port);
  memcpy(&adr_svr->ai_addr->sa_data, &port_net_byte_order, sizeof(unsigned short));

  memset(&svr_address, 0, sizeof(svr_address));
  svr_address.sin_family      = adr_svr->ai_family;
  svr_address.sin_port        = htons(server_port);
  svr_address.sin_addr.s_addr = htonl(INADDR_ANY);
    
  if ((listen_socket = get_listen_socket(adr_svr)) < 0)
    {
    /* Can not get socket for listening */
    rc = PBSE_SOCKET_FAULT;
    }
  else if ((bind(listen_socket, (struct sockaddr *)&svr_address, sizeof(svr_address))) == -1)
    {
    /* Can not bind local socket */
    rc = PBSE_SOCKET_FAULT;
    }
  else if (listen(listen_socket, 256) == -1)
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
    freeaddrinfo(adr_svr);
    while (1)
      {
      len_inet = sizeof(struct sockaddr);
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
        if (debug_mode == TRUE)
          {
          process_meth((void *)new_conn_port);
          }
        else
          {
          /* add_conn is not protocol independent. We need to 
             do some IPv4 stuff here */
          in_addr = (struct sockaddr_in *)&adr_client;
          add_conn(
            *new_conn_port,
            FromClientDIS,
            (pbs_net_t)ntohl(in_addr->sin_addr.s_addr),
            (unsigned int)htons(in_addr->sin_port),
            PBS_SOCK_INET,
            NULL);
          enqueue_threadpool_request(process_meth, new_conn_port);
          /*pthread_create(&tid, &t_attr, process_meth, (void *)new_conn_port);*/
          }
        }
      if (debug_mode == TRUE)
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

  return(rc);
  } /* END start_listener_addrinfo() */
