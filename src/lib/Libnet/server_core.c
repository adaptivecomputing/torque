#include "license_pbs.h" /* See here for the software license */
#include "lib_net.h"
#include <arpa/inet.h> /* inet_addr */
#include <unistd.h> /* close */
#include <errno.h> /* errno, strerror */
#include <stdio.h> /* printf */
#include <sys/un.h> /*sockaddr_un */
#include <fcntl.h>
#include <sys/stat.h> /* chmod */
#include <signal.h>  /* signal and constants */

#include "pbs_error.h" /* PBSE_NONE */
#include "log.h" /* log_event, PBSEVENT_JOB, PBS_EVENTCLASS_JOB */
#include "../Liblog/log_event.h" /* log_event */
#include "lib_ifl.h" /* process_svr_conn */
#include "../Libnet/lib_net.h"
#include "threadpool.h"
#include "../Liblog/pbs_log.h"

#define NUM_ACCEPT_RETRIES 5

extern int debug_mode;
extern void *(*read_func[])(void *);
extern char *msg_daemonname;

extern bool trqauthd_up;
int         listening_socket; // global record of the current listening socket

/* Note, in extremely high load cases, the alloc value in /proc/net/sockstat can exceed the max value. This will substantially slow down throughput and generate connection failures (accept gets a EMFILE error). As the client is designed to run on each submit host, that issue shouldn't occur. The client must be restarted to clear out this issue. */
int start_listener(
    
  const char   *server_ip,
  int            server_port,
  void          *(*process_meth)(void *))

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
      if ((new_conn_port = (int *)calloc(1, sizeof(int))) == NULL)
        {
        printf("Error allocating new connection handle on accept.\n");
        break;
        }

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

  if (listen_socket != -1)
    close(listen_socket);

  return(rc);
  } /* END start_listener() */


/* start_domainsocket_listener
 * Starts a listen thread on a UNIX domain socket connection
 */
int start_domainsocket_listener(
    
  const char   *socket_name,
  void          *(*process_meth)(void *))

  {
  struct sockaddr_un addr;
  int rc = PBSE_NONE;
  char log_buf[LOCAL_LOG_BUF_SIZE];
  int                *new_conn_port = NULL;
  int                 listen_socket = 0;
  int                 total_cntr = 0;
  pthread_t           tid;
  pthread_attr_t      t_attr;
  int objclass = 0;
  char authd_host_port[1024];

  memset(&addr, 0, sizeof(addr));
  addr.sun_family = AF_UNIX;
  strncpy(addr.sun_path, socket_name, sizeof(addr.sun_path)-1);

  /* socket_name is a file in the file system. It must be gone 
     before we can bind to it. Unlink it */
  unlink(socket_name);

  /* Ignore SIGPIPE so a rougue client won't cause us to abort */
  signal(SIGPIPE, SIG_IGN);

  if ( (listen_socket = socket(AF_UNIX, SOCK_STREAM, 0)) < 0)
    {
    snprintf(log_buf, sizeof(log_buf), "socket failed: %d %s", errno, strerror(errno));
    log_event(PBSEVENT_ADMIN | PBSEVENT_FORCE, PBS_EVENTCLASS_SERVER, __func__, log_buf);
    rc = PBSE_SOCKET_FAULT;
    }
  else if ( bind(listen_socket, (struct sockaddr *)&addr, sizeof(addr)) < 0)
    {
    snprintf(log_buf, sizeof(log_buf), "failed to bind socket %s: %d %s", socket_name, errno, strerror(errno));
    log_event(PBSEVENT_ADMIN | PBSEVENT_FORCE, PBS_EVENTCLASS_SERVER, __func__, log_buf);
    rc = PBSE_SOCKET_FAULT;
    }
  else if (chmod(socket_name,  S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH) < 0)
    {
    snprintf(log_buf, sizeof(log_buf), "failed to change file permissions on  %s: %d %s", socket_name, errno, strerror(errno));
    log_event(PBSEVENT_ADMIN | PBSEVENT_FORCE, PBS_EVENTCLASS_SERVER, __func__, log_buf);
    rc = PBSE_SOCKET_FAULT;
    }
  else if ( listen(listen_socket, 64) < 0)
    {
    snprintf(log_buf, sizeof(log_buf), "listen failed %s: %d %s", socket_name, errno, strerror(errno));
    log_event(PBSEVENT_ADMIN | PBSEVENT_FORCE, PBS_EVENTCLASS_SERVER, __func__, log_buf);
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
      log_get_host_port(authd_host_port, sizeof(authd_host_port));
      if (authd_host_port[0])
        snprintf(log_buf, sizeof(log_buf),
          "TORQUE authd daemon started and listening on %s (unix socket %s)", 
            authd_host_port, socket_name);
      else
        snprintf(log_buf, sizeof(log_buf),
          "TORQUE authd daemon started and listening unix socket %s", socket_name);
      log_event(PBSEVENT_SYSTEM | PBSEVENT_FORCE, PBS_EVENTCLASS_TRQAUTHD,
        msg_daemonname, log_buf);
      }
    while (trqauthd_up == true)
      {
      if((new_conn_port = (int *)calloc(1, sizeof(int))) == NULL)
        {
        printf("Error allocating new connection handle on accept.\n");
        break;
        }
      if ((*new_conn_port = accept(listen_socket, NULL, NULL)) == -1)
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

    /* Do not free the new_conn_port here. It will be freed in process_svr_conn */

    pthread_attr_destroy(&t_attr);
    log_event(PBSEVENT_JOB, PBS_EVENTCLASS_JOB, "net_srvr",
                "Socket close of network listener requested");
    }

  if (listen_socket != -1)
    close(listen_socket);

  unlink(socket_name);
  log_close(1);

  return(rc);
  } /* END start_domainsocket_listener() */




int start_listener_addrinfo(

  char   *host_name,
  int     server_port,
  void *(*process_meth)(void *))

  {
  struct addrinfo    *adr_svr = NULL;
  struct sockaddr     adr_client;
  struct sockaddr_in *in_addr;
  struct sockaddr_in  svr_address;
  socklen_t           len_inet;
  int                 rc = PBSE_NONE;
  int                 sockoptval;
  int                 new_conn_port = -1;
  int                 listen_socket = 0;
  int                 total_cntr = 0;
  unsigned short      port_net_byte_order;
  pthread_attr_t      t_attr;
  char                err_msg[MAXPATHLEN];
  char                log_buf[LOCAL_LOG_BUF_SIZE + 1];
  int                 ret = pbs_getaddrinfo(host_name, NULL, &adr_svr);

  if (ret != 0)
    {
    /* hostname didn't resolve */
    snprintf(err_msg, sizeof(err_msg),
      "Error with getaddrinfo on host name %s. Error code = %d, '%s'.\n",
      host_name, ret, gai_strerror(ret));
    log_event(PBSEVENT_JOB, PBS_EVENTCLASS_JOB, __func__, err_msg);
    rc = PBSE_SOCKET_FAULT;
    return rc;
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
    // record this so it can be closed by children
    listening_socket = listen_socket;

    int exit_loop = FALSE;
    int retry_tolerance = NUM_ACCEPT_RETRIES;

    while (1)
      {
      long *args = NULL;

      /* if successfully allocated args will be freed in process_meth */
      args = (long *)calloc(3, sizeof(long));
      if (args == NULL)
        {
        snprintf(log_buf, sizeof(log_buf), "failed to allocate argument space");
        log_event(PBSEVENT_ERROR, PBS_EVENTCLASS_REQUEST, __func__, log_buf);
        /* Let's try to recover */
        sleep(5);
        continue;
        }

     len_inet = sizeof(struct sockaddr);

      if ((new_conn_port = accept(listen_socket, (struct sockaddr *)&adr_client, (socklen_t *)&len_inet)) == -1)
        {
        switch (errno)
          {
          case EMFILE:
          case ENFILE:
          case EINTR:

            /* transient error, try again */
            if (retry_tolerance-- <= 0)
              {
              exit_loop = TRUE;
              snprintf(err_msg, sizeof(err_msg), "Exiting loop because we passed our retry tolerance: %d", errno);
              }
            else
              sleep(1);

            break;

          default:
        
            snprintf(err_msg, sizeof(err_msg), "error in accept %s - stopping accept loop", strerror(errno));
            exit_loop = TRUE;
            break;
          }

        if (exit_loop == TRUE)
          {
          if (args)
            free(args);
          break;
          }

        errno = 0;
        }
      else
        {
        retry_tolerance = NUM_ACCEPT_RETRIES;
        sockoptval = 1;
        setsockopt(new_conn_port, SOL_SOCKET, SO_REUSEADDR, (void *)&sockoptval, sizeof(sockoptval));

        in_addr = (struct sockaddr_in *)&adr_client;
        args[0] = new_conn_port;
        args[1] = ntohl(in_addr->sin_addr.s_addr);
        args[2] = htons(in_addr->sin_port);
        
        if (debug_mode == TRUE)
          {
          process_meth((void *)args);
          }
        else
          {
          if (new_conn_port == PBS_LOCAL_CONNECTION)
            {
            snprintf(log_buf, LOCAL_LOG_BUF_SIZE, "Ignoring local incoming request %d", new_conn_port);
            log_record(PBSEVENT_SYSTEM, PBS_EVENTCLASS_REQUEST, __func__, log_buf);
            }
          else
            {
            /* add_conn is not protocol independent. We need to 
               do some IPv4 stuff here */
            add_conn(
              new_conn_port,
              FromClientDIS,
              (pbs_net_t)ntohl(in_addr->sin_addr.s_addr),
              (unsigned int)htons(in_addr->sin_port),
              PBS_SOCK_INET,
              NULL);
            
            enqueue_threadpool_request(process_meth, args, request_pool);
            }
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
      } /* END infinite_loop() */

    pthread_attr_destroy(&t_attr);

    /* all conditions for exiting the loop must populate err_msg */
    log_event(PBSEVENT_JOB, PBS_EVENTCLASS_JOB, __func__, err_msg);
    }

  if (listen_socket != -1)
    close(listen_socket);

  return(rc);
  } /* END start_listener_addrinfo() */
