#include "license_pbs.h" /* See here for the software license */
#include "lib_net.h"

#include <stdio.h> /* perror */
#include <unistd.h> /* close, usleep, read, write */

#include <string.h> /* memset */
#include <sys/ioctl.h> /* ioctl, FIONREAD */
#include <signal.h> /* Signals SIGPIPE, etc */
#include <sys/types.h>
#include <sys/socket.h> /* Socket communication */
#include <netdb.h> /* struct addrinfo */
#include <netinet/in.h> /* Internet domain sockets */
#include <arpa/inet.h> /* in_addr_t */
#include <netinet/tcp.h>
#include <errno.h> /* errno */
#include <fcntl.h> /* fcntl, F_GETFL */
#include <sys/time.h> /* gettimeofday */
#include <poll.h> /* poll functionality */
#include "../lib/Liblog/pbs_log.h" /* log_err */
#include "log.h" /* LOCAL_LOG_BUF_SIZE */
#include "net_cache.h"

#include "pbs_error.h" /* torque error codes */

extern time_t pbs_tcp_timeout; /* located in tcp_dis.c. Move here later */

#define RES_PORT_START 144
#define RES_PORT_END (IPPORT_RESERVED - 1)
#define RES_PORT_RANGE (RES_PORT_END - RES_PORT_START + 1)
#define RES_PORT_RETRY 3
#define PBS_NET_RC_RETRY -2
#define TCP_PROTO_NUM 0
#define MAX_NUM_LEN 21

unsigned availBytesOnDescriptor(
    
  int pLocalSocket)

  {
  unsigned availBytes;
  if (ioctl(pLocalSocket, FIONREAD, &availBytes) != -1)
    return availBytes;
  perror("availBytes");
  return(0);
  } /* END availBytesOnDescriptor() */




int socket_avail_bytes_on_descriptor(
    
  int socket)

  {
  unsigned avail_bytes;
  if (ioctl(socket, FIONREAD, &avail_bytes) != -1)
    return avail_bytes;
  return(0);
  } /* END socket_avail_bytes_on_descriptor() */




int socket_get_tcp()

  {
  int local_socket = 0;
  struct linger l_delay;
  int on = 1;
  int ka_val = 1;
  int ka_timeout = 10;

  (void) signal(SIGPIPE, SIG_IGN);
  memset(&l_delay, 0, sizeof(struct linger));
  l_delay.l_onoff = 0;
  if ((local_socket = socket(PF_INET, SOCK_STREAM, TCP_PROTO_NUM)) == -1)
    {
    local_socket = -2;
    }
  else if (setsockopt(local_socket, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on)) == -1)
    {
    close(local_socket);
    local_socket = -3;
    }
  else if (setsockopt(local_socket, SOL_SOCKET, SO_LINGER, &l_delay, sizeof(struct linger)) == -1)
    {
    close(local_socket);
    local_socket = -4;
    }
  else if (setsockopt(local_socket, SOL_SOCKET, SO_KEEPALIVE, &ka_val, sizeof(int)) < 0)
    {
    close(local_socket);
    local_socket = -5;
    }
  else if (setsockopt(local_socket, SOL_TCP, TCP_KEEPIDLE, &ka_timeout, sizeof(int)) < 0)
    {
    close(local_socket);
    local_socket = -6;
    }

  return local_socket;
  } /* END socket_get_tcp() */




int get_listen_socket(
    
  struct addrinfo *addr_info)

  {
  int local_socket = 0;
  struct linger l_delay;
  int on = 1;
  int ka_val = 1;
  int ka_timeout = 10;
  
  (void) signal(SIGPIPE, SIG_IGN);
  memset(&l_delay, 0, sizeof(struct linger));
  l_delay.l_onoff = 0;

  if ((local_socket = socket(addr_info->ai_family, addr_info->ai_socktype, addr_info->ai_protocol)) == -1)
    {
    local_socket = -2;
    }
  else if (setsockopt(local_socket, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on)) == -1)
    {
    close(local_socket);
    local_socket = -3;
    }
  else if (setsockopt(local_socket, SOL_SOCKET, SO_LINGER, &l_delay, sizeof(struct linger)) == -1)
    {
    close(local_socket);
    local_socket = -4;
    }
  else if (setsockopt(local_socket, SOL_SOCKET, SO_KEEPALIVE, &ka_val, sizeof(int)) < 0)
    {
    close(local_socket);
    local_socket = -5;
    }
  else if (setsockopt(local_socket, SOL_TCP, TCP_KEEPIDLE, &ka_timeout, sizeof(int)) < 0)
    {
    close(local_socket);
    local_socket = -6;
    }

  return(local_socket);
  } /* END get_listen_socket() */
   


int get_random_reserved_port()

  {
  int res_port = 0;
  res_port = (rand() % RES_PORT_RANGE) + RES_PORT_START;
  return res_port;
  } /* END get_random_reserved_port() */




int socket_get_tcp_priv()

  {
  int priv_port = 0, local_socket = 0;
  int cntr = 0;
#ifdef HAVE_RRESVPORT
  int on = 1;
  struct linger l_delay;
#endif
  int rc = PBSE_NONE;
  struct sockaddr_in local;
  int flags;
  memset(&local, 0, sizeof(struct sockaddr_in));
  local.sin_family = AF_INET;

  /* If any of the following 2 succeed (negative conditions) jump to else below
   * else run the default */
#ifdef HAVE_RRESVPORT
  memset(&l_delay, 0, sizeof(struct linger));
  l_delay.l_onoff = 0;
  if ((local_socket = rresvport(&priv_port)) != -1)
    {
    if (setsockopt(local_socket, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(int)) == -1)
      {
      rc = PBSE_SOCKET_FAULT;
      }
    else if (setsockopt(local_socket, SOL_SOCKET, SO_LINGER, &l_delay, sizeof(struct linger)) == -1)
      {
      rc = PBSE_SOCKET_FAULT;
      }
    }
  if (rc == PBSE_NONE)
    {
    /* Success case */
    priv_port = local_socket;
    }
  else if ((local_socket = socket_get_tcp()) > 0)
#else
#ifdef HAVE_BINDRESVPORT
    if ((local_socket = socket_get_tcp()) > 0)
      {
      if ((rc = bindresvport(local_socket, &local)) == 0)
        {
        /* Success case */
        priv_port = ntohs(local.sin_port);
        }
      else
        {
        rc = PBSE_SOCKET_FAULT;
        }
      }
    else
      rc = PBSE_SOCKET_FAULT;
  if (rc != PBSE_NONE)
#else /* Default */
    if ((local_socket = socket_get_tcp()) > 0)
#endif
#endif
      {
      /* According to the notes in the previous code:
       * bindresvport seems to cause connect() failures in some odd corner case
       * when talking to a local daemon.  So we'll only try this once and
       * fallback to the slow loop around bind() if connect() fails
       * with EADDRINUSE or EADDRNOTAVAIL.
       * http://www.supercluster.org/pipermail/torqueusers/2006-June/003740.html
       */

      flags = fcntl(local_socket, F_GETFL);
      flags |= O_NONBLOCK;
      fcntl(local_socket, F_SETFL, flags);

      priv_port = get_random_reserved_port();
      while (cntr < RES_PORT_RETRY)
        {
        if (++priv_port >= RES_PORT_END)
          priv_port = RES_PORT_START;
        local.sin_port = htons(priv_port);
        if (((rc = bind(local_socket, (struct sockaddr *)&local, sizeof(struct sockaddr))) < 0) &&
            ((rc == EADDRINUSE) ||
             (errno == EADDRNOTAVAIL) ||
             (errno == EINVAL) ||
             (rc == EINPROGRESS)))
          {
          cntr++;
          }
        else
          {
          rc = PBSE_NONE;
          break;
          }
        }
      if (cntr >= RES_PORT_RETRY)
        {
        close(local_socket);
        rc = PBSE_SOCKET_FAULT;
        errno = PBSE_SOCKET_FAULT;
        local_socket = -1;
        }
      }
    else
      {
      /* If something worked the first time you end up here */
      rc = PBSE_NONE;
      }
  if (rc != PBSE_NONE)
    {
    local_socket = -1;
    }
  return local_socket;
  } /* END socket_get_tcp_priv() */




int socket_connect(

  int   *local_socket,
  char  *dest_addr,
  int    dest_addr_len,
  int    dest_port,
  int    family,
  int    is_privileged,
  char **error_msg)

  {
  struct sockaddr_in remote;
  size_t r_size = sizeof(struct sockaddr_in);

  memset(&remote, 0, r_size);
  remote.sin_family = family;
  memcpy(&remote.sin_addr, dest_addr, dest_addr_len);
  remote.sin_port = htons((unsigned short)dest_port);
  return socket_connect_addr(local_socket, (struct sockaddr *)&remote, r_size, is_privileged, error_msg);
  }

int socket_connect_addr(
    
  int              *local_socket,
  struct sockaddr  *remote,
  size_t            remote_size,
  int               is_privileged,
  char            **error_msg)

  {
  int cntr = 0;
  int rc = PBSE_NONE;
  char tmp_buf[LOCAL_LOG_BUF_SIZE+1];
  const char id[] = "socket_connect_addr";

  while ((rc = connect(*local_socket, remote, remote_size)) != 0)
    {
/*    fprintf(stdout, "rc != 0 (%d)-(%d) (port_number=%d)\n", rc, errno, *local_socket); */
    switch (errno)
      {
      case ECONNREFUSED:    /* Connection refused */
        snprintf(tmp_buf, LOCAL_LOG_BUF_SIZE, "cannot connect to port %d in %s - connection refused", *local_socket, id);
        *error_msg = strdup(tmp_buf);
        rc = PBS_NET_RC_RETRY;
        close(*local_socket);
        *local_socket = -1;
        cntr = 0;
        break;

      case EINPROGRESS:   /* Operation now in progress */
      case EALREADY:    /* Operation already in progress */
      case EISCONN:   /* Transport endpoint is already connected */
      case ETIMEDOUT:   /* Connection timed out */
      case EAGAIN:    /* Operation would block */
      case EINTR:     /* Interrupted system call */
        if ((rc = socket_wait_for_write(*local_socket)) == PBSE_NONE)
          {
          /* no network failures detected, socket available */
          break;
          }
        /* socket not ready for writing after 5 timeout */
      case EINVAL:    /* Invalid argument */
      case EADDRINUSE:    /* Address already in use */
      case EADDRNOTAVAIL:   /* Cannot assign requested address */
        if (is_privileged)
          {
          rc = PBSE_SOCKET_FAULT;
          /* 3 connect attempts are made to each socket */
          /* Fail on RES_PORT_RETRY */
          if (cntr++ < RES_PORT_RETRY)
            {
            close(*local_socket);
            if ((*local_socket = socket_get_tcp_priv()) < 0)
              rc = PBSE_SOCKET_FAULT;
            else
              {
              rc = PBSE_NONE;
              continue;
              }
            }
          else
            {
            close(*local_socket);
            *local_socket = -1;
            /* Hit RES_PORT_RETRY, exit */
            cntr = 0;
            }
          }
        break;

      default:
        snprintf(tmp_buf, LOCAL_LOG_BUF_SIZE, "cannot connect to port %d in %s - errno:%d %s", *local_socket, id, errno, strerror(errno));
        *error_msg = strdup(tmp_buf);
        close(*local_socket);
        rc = PBSE_SOCKET_FAULT;
        cntr = 0;
        *local_socket = -1;
        break;
      }
    if (cntr == 0)
      break;
    }
  return rc;
  } /* END socket_connect() */




int socket_wait_for_write(
    
  int socket)

  {
  int            rc = PBSE_NONE;
  int            write_soc = 0, val;
  socklen_t      len = sizeof(int);
  fd_set         wfd;
  struct timeval timeout;

  timeout.tv_sec = pbs_tcp_timeout;
  timeout.tv_usec = 0;

  FD_ZERO(&wfd);
  FD_SET(socket, &wfd);

  if ((write_soc = select(socket+1, 0, &wfd, 0, &timeout)) != 1)
    {
    rc = PBSE_TIMEOUT;
    }
  else if (((rc = getsockopt(socket, SOL_SOCKET, SO_ERROR, &val, &len)) == 0) && (val == 0))
    {
    rc = PBSE_NONE;
    }
  else
    {
    errno = val;
    rc = PBSE_SOCKET_WRITE;
    }
  return rc;
  } /* END socket_wait_for_write() */




int socket_wait_for_xbytes(
    
  int socket,
  int len)

  {
  int rc = PBSE_NONE;
  int avail_bytes = socket_avail_bytes_on_descriptor(socket);
  while (avail_bytes < len)
    {
    if ((rc = socket_wait_for_read(socket)) == PBSE_NONE)
      {
      avail_bytes = socket_avail_bytes_on_descriptor(socket);
      if (avail_bytes < len)
        usleep(1);
      }
    else
      {
      break;
      }
    }
  return rc;
  } /* END socket_wait_for_xbytes() */




int socket_wait_for_read(
    
  int socket)

  {
  int           rc = PBSE_NONE;
  int           ret;
  struct pollfd pfd;

  pfd.fd = socket;
  pfd.events = POLLIN | POLLHUP; /* | POLLRDNORM; */
  pfd.revents = 0;

  while (pfd.revents == 0)
    {
    ret = poll(&pfd, 1, pbs_tcp_timeout * 1000); /* poll's timeout is in milliseconds */
    if (ret > 0)
      {
      char buf[8];
      if (recv(socket, buf, 7, MSG_PEEK | MSG_DONTWAIT) == 0)
        {
        /* This will only occur when the socket has closed */
        rc = PBSE_SOCKET_CLOSE;
        break;
        }
      else
        break; /* data exists */
      }
    else if (ret == 0)
      {
      /* Server timeout reached */
      rc = PBSE_TIMEOUT;
      break;
      }
    else /* something bad happened to poll */
      rc = PBSE_SOCKET_DATA;
    }

  if (pfd.revents & POLLNVAL)
    {
    rc = PBSE_SOCKET_CLOSE;
    }

  return(rc);
  } /* END socket_wait_for_read() */




void socket_read_flush(
    
  int socket)

  {
  char incoming[256];
  int  avail_bytes = 0;
  int  i;
  
  while ((avail_bytes = socket_avail_bytes_on_descriptor(socket)) > 0)
    {
    if (avail_bytes > 256)
      avail_bytes = 256; 
    i = read(socket, incoming, avail_bytes);
    if (i < 0)
      break;
    }
  } /* END socket_read_flush() */




int socket_write(
    
  int   socket,
  char *data,
  int   data_len)

  {
  int data_len_actual = -1;
  if ((data != NULL) || (data_len > 0))
    {
    data_len_actual = write(socket, (const char *)data, data_len);
    if (data_len_actual == -1)
      printf("Error (%d-%s) writing %d bytes to socket (write_socket) data [%s]\n", errno, strerror(errno), data_len, data);
    else if (data_len_actual != data_len)
      {
      printf("Error (%d-%s)writing data to socket (tried to send %d chars, actual %d)\n", errno, strerror(errno), data_len, data_len_actual);
      data_len_actual = data_len_actual - data_len;
      }
    }
  return data_len_actual;
  } /* END socket_write() */




int socket_read_force(

  int        socket,
  char      *the_str,
  long long  avail_bytes,
  long long *byte_count)

  {
  int rc = PBSE_NONE;
  char *read_loc = the_str;
  long long tmp_len = avail_bytes;
  long long bytes_read = 1;
  long long sock_check = 0;
  char log_buf[LOCAL_LOG_BUF_SIZE+1];
  while (bytes_read != 0)
    {
    bytes_read = read(socket, read_loc, tmp_len);
    if ((bytes_read == -1) && (errno != EINTR))
      {
      if (getenv("PBSDEBUG") != NULL)
        fprintf(stderr, "Error reading data from socket %d - (%s)\n",
            errno, strerror(errno));
      rc = PBSE_SOCKET_READ;
      break;
      }
    else if (bytes_read == 0)
      {
      if (*byte_count == 0)
        rc = PBSE_SOCKET_READ;
      break;
      }
    else if (bytes_read == avail_bytes)
      {
      *byte_count += bytes_read;
      break;
      }
    else
      {
      tmp_len -= bytes_read;
      read_loc += bytes_read;
      *byte_count += bytes_read;
      sock_check = socket_avail_bytes_on_descriptor(socket);
      if (sock_check == 0)
        {
        snprintf(log_buf, LOCAL_LOG_BUF_SIZE, "ioctl hsa been lying, expected avail %lld, actual avail %lld", tmp_len, sock_check);
        log_record(PBSEVENT_SYSTEM, PBS_EVENTCLASS_REQUEST, __func__, log_buf);
        break;
        }
      if (sock_check < tmp_len)
        tmp_len = sock_check;
      }
    }
  return rc;
  } /* END socket_read_force() */




int socket_read(
    
  int         socket,
  char      **the_str,
  long long  *str_len)

  {
  int       rc = PBSE_NONE;
  long long avail_bytes = socket_avail_bytes_on_descriptor(socket);
  long long byte_count = 0;

  if ((the_str == NULL) || (str_len == NULL))
    return PBSE_INTERNAL;

  while (avail_bytes == 0)
    {
    if ((rc = socket_wait_for_read(socket)) != PBSE_NONE)
      break;
    avail_bytes = socket_avail_bytes_on_descriptor(socket);
    if (avail_bytes == 0)
      {
      rc = PBSE_SOCKET_READ;
      break;
      }
    }

  if (rc != PBSE_NONE)
    {
    }
  else if ((*the_str = calloc(1, avail_bytes+1)) == NULL)
    {
    rc = PBSE_MEM_MALLOC;
    }
  else if ((rc = socket_read_force(socket, *the_str, avail_bytes, &byte_count))
      != PBSE_NONE)
    {
    }
  else
    {
    if (getenv("PBSDEBUG") != NULL)
      if (byte_count != avail_bytes)
        fprintf(stderr, "Bytes on socket = %lld, bytes read = %lld\n",
            avail_bytes, byte_count);
    *str_len = byte_count;
    }

  return(rc);
  } /* END socket_read() */




int socket_read_one_byte(
    
  int   socket,
  char *one_char)

  {
  int rc = PBSE_NONE;
  int avail_bytes = socket_avail_bytes_on_descriptor(socket);
  if (avail_bytes <= 0)
    rc = socket_wait_for_read(socket);
  if (rc == PBSE_NONE)
    {
    if (read(socket, one_char, 1) != 1)
      rc = PBSE_SOCKET_READ;
    else
      rc = PBSE_NONE;
    }
  return rc;
  } /* END socket_read_one_byte() */




int socket_read_num(
    
  int        socket,
  long long *the_num)

  {
  int  rc =  PBSE_INTERNAL;
  int  pos = 0;
  char str_ll[MAX_NUM_LEN];
  char tmp_char = '\0';
  int  avail_bytes = socket_avail_bytes_on_descriptor(socket);

  memset(str_ll, 0, MAX_NUM_LEN);

  while (1)
    {
    if (avail_bytes == 0)
      {
      /* Wait until there is activity on the socket .... */
      if ((rc = socket_wait_for_read(socket)) != PBSE_NONE)
        break;
      avail_bytes = socket_avail_bytes_on_descriptor(socket);
      if (avail_bytes == 0)
        {
        rc = PBSE_SOCKET_READ;
        break;
        }
      }
    else if (read(socket, &tmp_char, 1) == -1)
      break;
    else if (pos >= (int)sizeof(str_ll) - 1)
      break;
    else if ((tmp_char >= '0' && tmp_char <= '9') || (tmp_char == '-'))
      {
      str_ll[pos++] = tmp_char;
      avail_bytes--;
      }
    else if (tmp_char == '|')
      {
      if (pos != 0)
        {
        *the_num = atol(str_ll);
        rc = PBSE_NONE;
        break;
        }
      }
    else
      break;
    }

  if (str_ll[0] == '\0')
    rc = PBSE_SOCKET_READ;

  return rc;
  } /* END socket_read_num() */




/* memory for "the_str" is allocated in inside this function.
 * "str_len" is not.
 */
int socket_read_str(
    
  int         socket,
  char      **the_str,
  long long  *str_len)

  {
  int rc =  PBSE_NONE;
  long long tmp_len = 0;
  long long read_len = 0;
  char delin;

  if ((the_str == NULL) || (str_len == NULL))
    return PBSE_INTERNAL;

  if ((rc = socket_read_num(socket, (long long *)&tmp_len)) != PBSE_NONE)
    {
    }
  else if (tmp_len == 0)
    {
    /* Valid case, NULL string */
    *the_str = NULL;
    *str_len = 0;
    rc = PBSE_NONE;
    }
  else if ((*the_str = (char *)calloc(1, (tmp_len)+1)) == NULL)
    {
    rc = PBSE_INTERNAL;
    }
  /* This is where the select would go until the availbytes is > the tmp_len */
  else if ((rc = socket_wait_for_xbytes(socket, tmp_len+1)) != PBSE_NONE)
    {
    }
  else if (socket_read_force(socket, *the_str, tmp_len, &read_len) == -1)
    {
    rc = PBSE_INTERNAL;
    }
  else if (read(socket, &delin, 1) != 1)
    {
    rc = PBSE_INTERNAL;
    }
  else if (delin != '|')
    {
    rc = PBSE_INTERNAL;
    }
  else
    {
    if (getenv("PBSDEBUG") != NULL)
      if (read_len != tmp_len)
        fprintf(stderr, "Bytes on socket = %lld, bytes read = %lld\n",
            tmp_len, read_len);
    *str_len = tmp_len;
    /* SUCCESS */
    }
  return rc;
  } /* socket_read_str() */




int socket_close(
    
  int socket)

  {
  int rc;
  socket_read_flush(socket);
  if ((rc = close(socket)) == 0)
    rc = PBSE_NONE;
  else
    rc = PBSE_SYSTEM;
  return rc;
  } /* END socket_close() */



int get_addr_info(
    
  char               *name,
  struct sockaddr_in *sa_info,
  int                 retry)

  {
  int                 rc = PBSE_NONE;
  int                 cntr = 0;
  struct addrinfo    *addr_info;
  struct addrinfo     hints;
  struct timeval      start_time;
  struct timeval      end_time;
  struct sockaddr_in *cached_sai;

  /* retrieve from cache if possible */
  if ((cached_sai = get_cached_addrinfo(name)) != NULL)
    {
    memcpy(sa_info, cached_sai, sizeof(struct sockaddr_in));
    return(PBSE_NONE);
    }

  memset(&hints, 0, sizeof(struct addrinfo));
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_family = PF_INET;

  while (cntr < retry)
    {
    gettimeofday(&start_time, 0);

    if ((rc = getaddrinfo(name, NULL, &hints, &addr_info)) != 0)
      {
      gettimeofday(&end_time, 0);

      rc = PBSE_BADHOST;
      }
    else
      {
      sa_info->sin_addr = ((struct sockaddr_in *)addr_info->ai_addr)->sin_addr;
      sa_info->sin_family = addr_info->ai_family;
      insert_addr_name_info(name, addr_info->ai_canonname, sa_info);
      freeaddrinfo(addr_info);
      gettimeofday(&end_time, 0);

      rc = PBSE_NONE;

      break;
      }

    cntr++;
    }

  return(rc);
  } /* END get_addr_info() */

