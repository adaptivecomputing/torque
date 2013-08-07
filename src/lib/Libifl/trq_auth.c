#include "license_pbs.h" /* See here for the software license */
#include "lib_ifl.h"

#include <limits.h> /* LOGIN_NAME_MAX */
#include <netinet/in.h> /* in_addr_t */
#include <stdio.h> /* sprintf */
#include <unistd.h> /* close */
#include <arpa/inet.h> /* inet_addr */
#include <errno.h>
#include <ctype.h> /* isspace */
#include <sys/types.h>
#include <sys/socket.h>
#include <pwd.h>  /* getpwuid */
#include "csv.h"
#include "pbs_config.h"
#include "../Libnet/lib_net.h" /* get_hostaddr, socket_* */
#include "../../include/log.h" /* log event types */

char         *trq_addr = NULL;
int           trq_addr_len;
char         *trq_server_name = NULL;
int           debug_mode = 0;
static char   active_pbs_server[PBS_MAXSERVERNAME + 1];
extern time_t pbs_tcp_timeout;

#ifdef UNIT_TEST
  int process_svr_conn_rc;
#endif

int set_active_pbs_server(

  const char *new_active_server)

  {
  strncpy(active_pbs_server, new_active_server, PBS_MAXSERVERNAME);
  return(PBSE_NONE);
  }

/* validate_active_pbs_server -- Get the currently
   active pbs_server and put it in active_server.
   this fuction sends a request to trqauthd to ask it
   to validate the currenly active pbs_server. 
   trqauthd will return the currently active server */

int validate_active_pbs_server(

  char **active_server,
  int    port)

  {
  char     *err_msg;
  char      *current_server = NULL;
  char      unix_sockname[MAXPATHLEN + 1];
  char      write_buf[MAX_LINE];
  int       write_buf_len;
  char     *read_buf;
  long long read_buf_len = MAX_LINE;
  int       local_socket;
  int       rc;

  /* the format is TRQ command|destination port */
  sprintf(write_buf, "%d|%d|", TRQ_VALIDATE_ACTIVE_SERVER, port);

  write_buf_len = strlen(write_buf);
  snprintf(unix_sockname, sizeof(unix_sockname), "%s/%s", TRQAUTHD_SOCK_DIR, TRQAUTHD_SOCK_NAME);

  local_socket = socket_get_unix();
  if (local_socket < 0)
    {
    fprintf(stderr, "could not allocate unix domain socket: %d\n", local_socket);
    return(local_socket * -1); /*socket_get_unix returns a negative PBSE error value
                                 Change it back */
    }

  rc = socket_connect_unix(local_socket, unix_sockname, &err_msg);
  if (rc != PBSE_NONE)
    {
    fprintf(stderr, "socket_connect_unix failed: %d\n", rc);
    return(PBSE_SYSTEM);
    }

  rc = socket_write(local_socket, write_buf, write_buf_len);
  if (rc <= 0 )
    {
    fprintf(stderr, "socket_write failed: %d\n", rc);
    return(PBSE_SYSTEM);
    }

  rc = socket_read_str(local_socket, &read_buf, &read_buf_len);
  if (rc != PBSE_NONE) 
    return(rc);

  if (read_buf_len == 0)
    return(PBSE_SOCKET_READ);

  rc = PBSE_NONE;

  current_server = (char *)calloc(1, strlen(read_buf));
  if (current_server == NULL)
    {
    return(PBSE_MEM_MALLOC);
    }

  strcpy(current_server, read_buf);
  
  close(local_socket);

  *active_server = current_server;

  return(rc);
  } /* END validate_active_pbs_server() */

/* 
 * get_active_pbs_server()
 * sends a TRQ_GET_ACTIVE_SERVER request to trqauthd. trqauthd will send a string of the
 * currently active server back
 * @pre-cond:  active_server must be a valid char **
 * @post-cond: active_server will be populated with the hostname of the active server
*/

int get_active_pbs_server(
    
  char **active_server)

  {
  char     *err_msg;
  char      *current_server = NULL;
  char      unix_sockname[MAXPATHLEN + 1];
  char      write_buf[MAX_LINE];
  int       write_buf_len;
  char     *read_buf;
  long long read_buf_len = MAX_LINE;
  int       local_socket;
  int       rc;
  char     *timeout_ptr;
  bool      retry = true;
  int       retries = 0;

  if ((timeout_ptr = getenv("PBSAPITIMEOUT")) != NULL)
    {
    time_t tmp_timeout = strtol(timeout_ptr, NULL, 0);

    if (tmp_timeout > 0)
      {
      pbs_tcp_timeout = tmp_timeout;

      if (tmp_timeout > 2)
        retry = false;
      }

    }

  /* the syntax for this call is a number followed by a | (pipe). The pipe indicates 
     the end of a number */
  sprintf(write_buf, "%d|", TRQ_GET_ACTIVE_SERVER);
  write_buf_len = strlen(write_buf);
  snprintf(unix_sockname, sizeof(unix_sockname), "%s/%s", TRQAUTHD_SOCK_DIR, TRQAUTHD_SOCK_NAME);

  local_socket = socket_get_unix();
  if (local_socket < 0)
    return(local_socket * -1); /* socket_get_unix returns a negative PBSE error on failure. make it positive */

  rc = socket_connect_unix(local_socket, unix_sockname, &err_msg);
  if (rc != PBSE_NONE)
    {
    fprintf(stderr, "socket_connect_unix failed: %d\n", rc);
    return(PBSE_SYSTEM);
    }

  rc = socket_write(local_socket, write_buf, write_buf_len);
  if (rc <= 0 )
    {
    fprintf(stderr, "socket_write failed: %d\n", rc);
    return(PBSE_SYSTEM);
    }

  do
    {
    rc = socket_read_str(local_socket, &read_buf, &read_buf_len);
    if (rc == PBSE_NONE) 
      break;
    } while ((retry == true) && (++retries <= 5));

  if (rc != PBSE_NONE)
    return(rc);

  if (read_buf_len == 0)
    return(PBSE_SOCKET_READ);

  rc = PBSE_NONE;

  current_server = strdup(read_buf);
  
  close(local_socket);

  *active_server = current_server;

  return(rc);
  }

int trq_simple_disconnect(

  int sock_handle)

  {
  int rc;

  rc = close(sock_handle);
  if (rc != 0)
    return(PBSE_SYSTEM);

  return(PBSE_NONE);
  }


/* trq_simple_connect 
 * The original purpose of this function is to connect to pbs_server
 * to make sure it is available. Return PBSE_NONE on success.
 * Anything else is failure.
 */

int trq_simple_connect(
    
  const char *server_name,
  int         batch_port,
  int        *sock_handle)

  {
  struct addrinfo      *results = NULL;
  struct addrinfo      *addr_info;
  struct addrinfo      hints;
  char                 port_string[10]; /* the string of the batch port for pbs_server. Needed for getaddrinfo */
  int                  rc;
  int                  sock;
  int                  optval = 1;

  memset(&hints, 0, sizeof(hints));
  /* set the hints so we get a STREAM socket */
  hints.ai_family = AF_UNSPEC; /* allow for IPv4 or IPv6 */
  hints.ai_socktype = SOCK_STREAM; /* we want a tcp connection */
  hints.ai_flags = AI_PASSIVE;  /* fill in my IP for me */
  snprintf(port_string, 10, "%d", batch_port);
  rc = getaddrinfo(server_name, port_string , &hints, &results);
  if (rc != PBSE_NONE)
    {
    fprintf(stderr, "cannot resolve server name %s\n", server_name);
    return(PBSE_SERVER_NOT_FOUND);
    }

  for (addr_info = results; addr_info != NULL; addr_info = addr_info->ai_next)
    {

    sock = socket(addr_info->ai_family, SOCK_STREAM, addr_info->ai_protocol);
    if (sock < 0)
      {
      fprintf(stderr, "Could not open socket in %s. error %d\n", __func__, errno);
      rc = PBSE_SYSTEM;
      continue;
      }
      
    /* Make sure we don't make the socket wait for the linger timeout before getting freed */
    rc = setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval));
    if (rc != 0)
      {
      fprintf(stderr, "setsockopt failed in %s. error %d\n", __func__, errno);
      rc = PBSE_SYSTEM;
      continue;
      }

    rc = connect(sock, addr_info->ai_addr, addr_info->ai_addrlen);
    if (rc != 0)
      {
      /* This server is not listening */
      close(sock);
      rc = PBSE_SYSTEM;
      continue;
      }
    else
      {
      rc = PBSE_NONE;
      break;
      }
    }

  if (results != NULL)
    freeaddrinfo(results);
  if (rc != PBSE_NONE)
      return(rc);

  *sock_handle = sock;
  
  if (addr_info == NULL)
    return(PBSE_SERVER_NOT_FOUND);
  
  return(rc);
  }

/* validate_server:
 * This function tries to find the currently active
 * pbs_server. If no server can be found the default 
 * server is made the active server */ 
int validate_server(

  char  *active_server_name,
  int    t_server_port,
  char  *ssh_key,
  char **sign_key)

  {
  int rc = PBSE_NONE;
  int sd;
  char server_name_list[PBS_MAXSERVERNAME*3+1];
  char current_name[PBS_MAXSERVERNAME+1];
  char *tp;
  char  log_buf[LOCAL_LOG_BUF_SIZE];

  /* First we try to connect to the pbs_server as 
     indicated in active_server_name. If that fails
     we go through the server list. If that fails
     we stick with the active_server_name. If 
     another server in teh server_name_list responds 
     that will become the active_pbs_server */
  if (active_server_name != NULL)
    rc = trq_simple_connect(active_server_name, t_server_port, &sd);
  else
    rc = PBSE_UNKREQ; /* If we don't have a server name we want to process everyting. */

  if ( rc != PBSE_NONE)
    {
    int list_len;
    int i;

    snprintf(server_name_list, sizeof(server_name_list), "%s", pbs_get_server_list());
    list_len = csv_length(server_name_list);

    for (i = 0; i < list_len; i++)  /* Try all server names in the list. */
      {
      tp = csv_nth(server_name_list, i);

      if (tp && tp[0])
        {
        /* Trim any leading space */
        while(isspace(*tp)) tp++; 
        
        memset(current_name, 0, sizeof(current_name));
        snprintf(current_name, sizeof(current_name), "%s", tp);

        if (getenv("PBSDEBUG"))
          {
          fprintf(stderr, "pbs_connect attempting connection to server \"%s\"\n",
                                                                    current_name);
          }

        rc = trq_simple_connect(current_name, t_server_port, &sd);
        if ( rc == PBSE_NONE)
          {
          trq_simple_disconnect(sd);
          fprintf(stderr, "changing active server to %s\n", current_name);
          strcpy(active_pbs_server, current_name); 
          sprintf(log_buf, "Changing active server to %s\n", current_name);
          log_event(PBSEVENT_CLIENTAUTH | PBSEVENT_FORCE, PBS_EVENTCLASS_TRQAUTHD, __func__, log_buf);
          break;
          }
        }
      }
    }
  else
    trq_simple_disconnect(sd);

  if (rc == PBSE_SERVER_NOT_FOUND) /* This only indicates no server is currently active. Go to default */
    {
    active_pbs_server[0] = '\0';
    rc = PBSE_NONE;
    }

  fprintf(stderr, "Active server name: %s  pbs_server port is: %d\n", active_pbs_server, t_server_port);

  return(rc);
  } /* END validate_server() */


int parse_request_client(

  int    sock,
  char **server_name,
  int   *server_port,
  int   *auth_type,
  char **user,
  int   *user_pid,
  int   *user_sock)

  {
  int rc = PBSE_NONE;
  long long tmp_val = 0, tmp_port = 0, tmp_auth_type = 0, tmp_sock = 0, tmp_pid = 0;
  
  if ((rc = socket_read_str(sock, server_name, &tmp_val)) != PBSE_NONE)
    {
    }
  else if ((rc = socket_read_num(sock, &tmp_port)) != PBSE_NONE)
    {
    }
  else if ((rc = socket_read_num(sock, &tmp_auth_type)) != PBSE_NONE)
    {
    }
  else if ((rc = socket_read_str(sock, user, &tmp_val)) != PBSE_NONE)
    {
    }
  else if ((rc = socket_read_num(sock, &tmp_pid)) != PBSE_NONE)
    {
    }
  else if ((rc = socket_read_num(sock, &tmp_sock)) != PBSE_NONE)
    {
    }
  else
    {
    *server_port = (int)tmp_port;
    *auth_type = (int)tmp_auth_type;
    *user_pid = (int)tmp_pid;
    *user_sock = (int)tmp_sock;
    }
  return rc;
  }

int build_request_svr(
    int auth_type,
    char *user,
    int sock,
    char **send_message)
  {
  /* PBS_BATCH_PROT_TYPE PBS_BATCH_PROT_VER PBS_BATCH_AuthenUser */
  int rc = PBSE_NONE;
  int len = 0;
  int user_ll = 0, user_len = 0, port_len = 0;
  char *resp_msg;
  char tmp_buf[13];
  len += 2 + 2 + 4;
  user_len = strlen(user);
  sprintf(tmp_buf, "%d", user_len);
  user_ll = strlen(tmp_buf);
  len += LOGIN_NAME_MAX + 1;
  len += user_ll;
  len += user_len;
  sprintf(tmp_buf, "%d", sock);
  port_len = strlen(tmp_buf);
  len += 2 + port_len;
  len += 2;
  if (AUTH_TYPE_IFF == auth_type)
    {
    resp_msg = (char *)calloc(1, len);
    sprintf(resp_msg, "+%d+%d%d+%d%d+%d%s%d+%d+0",
        PBS_BATCH_PROT_TYPE,
        PBS_BATCH_PROT_VER,
        2, PBS_BATCH_AuthenUser,
        user_ll, user_len, user,
        port_len, sock);
    *send_message = resp_msg;
    }
  else if (AUTH_TYPE_KEY == auth_type)
    {
    /* Not yet implemented */
    rc = PBSE_NOT_IMPLEMENTED;
    }
  else
    {
    rc = PBSE_AUTH_INVALID;
    }
  return rc;
  }

int build_active_server_response(

  char **send_message)

  {
  int rc = PBSE_NONE;
  int len = 0;
  char *resp_msg;
  char temp_buf[20]; 

  len = strlen(active_pbs_server);

  if (len == 0)
    {
    validate_server(NULL, PBS_BATCH_SERVICE_PORT, NULL, NULL);
    len = strlen(active_pbs_server);
    }

  sprintf(temp_buf, "%d", len);

  resp_msg = (char *)calloc(1, len + strlen(temp_buf) + 2); /* 2 because we need one for the '|' delimeter and one for a null termination */
  if (resp_msg == NULL)
    {
    return(PBSE_MEM_MALLOC);
    }

  sprintf(resp_msg, "%d|%s|", len, active_pbs_server);

  *send_message = resp_msg;

  return(rc);

  }

int validate_user(
    int sock,
    const char *user_name, 
    int user_pid,
    char *msg)
  {
  struct ucred cr;
  socklen_t   cr_size;
  struct passwd *user_pwd;

  if (msg == NULL)
    return(PBSE_BAD_PARAMETER);

  if (user_name == NULL)
    {
    sprintf(msg, "%s: user_name is NULL", __func__);
    return(PBSE_BAD_PARAMETER);
    }

  cr_size = sizeof(struct ucred);
  if (getsockopt(sock, SOL_SOCKET, SO_PEERCRED, (void *)&cr, &cr_size) < 0)
    {
    sprintf(msg, "getsockopt for SO_PEERDRED failed: %d", errno);
    return(PBSE_SOCKET_FAULT);
    }

  user_pwd = getpwuid(cr.uid);
  if (user_pwd == NULL)
    {
    sprintf(msg, "UID %d returned NULL from getpwuid", cr.uid);
    return(PBSE_IFF_NOT_FOUND);
    }

  if (strcmp(user_pwd->pw_name, user_name))
    {
    sprintf(msg, "User names do not match: submitted: %s, expected: %s", user_name, user_pwd->pw_name);
    return(PBSE_IFF_NOT_FOUND);
    }

  if (cr.pid != user_pid)
    {
    sprintf(msg, "invalid pid: submitted: %d, expected: %d", user_pid, cr.pid);
    return(PBSE_IFF_NOT_FOUND);
    }

  return(PBSE_NONE);
  }



int parse_response_svr(
    int sock,
    char **err_msg)
  {
  /*
   * PBS_BATCH_PROT_TYPE
   * PBS_BATCH_PROT_VER
   * reply->brp_code
   * reply->brp_auxcode
   * reply->brp_choice
   * if reply->brp_choice == BATCH_REPLY_CHOICE_Text also read:
   * preq->rq_reply.brp_un.brp_txt.brp_str
   * using
   * preq->rq_reply.brp_un.brp_txt.brp_txtlen
   */
  int rc = PBSE_NONE;
  struct batch_reply *reply = NULL;
  char *tmp_val = NULL;
  struct tcp_chan *chan = NULL;
  if ((chan = DIS_tcp_setup(sock)) == NULL)
    {
    }
  else if ((reply = (struct batch_reply *)calloc(1, sizeof(struct batch_reply))) == NULL)
    {
    }
  else if ((rc = decode_DIS_replyCmd(chan, reply)))
    {
    free(reply);
    if (chan->IsTimeout == TRUE)
      {
      rc = PBSE_TIMEOUT;
      }
    else
      {
      rc = PBSE_PROTOCOL;
      }
    if ((tmp_val = pbs_strerror(rc)) == NULL)
      {
      char err_buf[80];
      snprintf(err_buf, 79, "Error creating error message for code %d", rc);
      *err_msg = strdup(err_buf);
      }
    else
      *err_msg = strdup(tmp_val);
    }
  else
    {
    rc = reply->brp_code;
    if (reply->brp_code != PBSE_NONE)
      {
      *err_msg = strdup(reply->brp_un.brp_txt.brp_str);
      }
    free(reply);
    }
  DIS_tcp_cleanup(chan);
  return rc;
  }

int get_trq_server_addr(
   
  char  *server_name,
  char **server_addr,
  int   *server_addr_len)

  {
  int   rc = PBSE_NONE;
  int   local_errno;
  char *host_addr = NULL;
  unsigned short af_family;

  if ((trq_server_name != NULL) && (strcmp(trq_server_name, server_name) == 0))
    {
    /* server_name matches one item cache */
    host_addr = (char *)calloc(1, trq_addr_len + 1);
    memcpy(host_addr, trq_addr, trq_addr_len);
    *server_addr = host_addr;
    *server_addr_len = trq_addr_len;
    }
  else if (trq_server_name == NULL)
    {
    if ((rc = get_hostaddr_hostent_af(&local_errno, server_name, &af_family, server_addr, server_addr_len)) == PBSE_NONE)
      {
      /* The following is not strictly thread safe.
       * It is set the first call, and if two calls to different systems
       * get here simultaniously...
       */
      trq_addr = (char *)calloc(1, *server_addr_len + 1);
      memcpy(trq_addr, *server_addr, *server_addr_len);
      trq_addr_len = *server_addr_len;
      trq_server_name = strdup(server_name);
      }
    }
  else
    {
    if ((rc = get_hostaddr_hostent_af(&local_errno, server_name, &af_family, server_addr, server_addr_len)) == PBSE_NONE)
      {
      }
    }
  return rc;
  }

void send_svr_disconnect(int sock, const char *user_name)
  {
  /*
   * Disconnect message to svr:
   * +2+22+592+{user_len}{user}
   */
  int rc = PBSE_NONE;
  int len = 0, user_len = 0, user_ll = 0, resp_msg_len = 0;
  char tmp_buf[8];
  char *resp_msg = NULL;

  user_len = strlen(user_name);
  sprintf(tmp_buf, "%d", user_len);
  user_ll = strlen(tmp_buf);
  len += 8;
  len += user_ll;
  len += 1;
  len += user_len;
  len += LOGIN_NAME_MAX + 1;

  resp_msg = (char *)calloc(1, len);
  sprintf(resp_msg, "+2+22+59%d+%d%s", user_ll, user_len, user_name);
  resp_msg_len = strlen(resp_msg);
  if ((rc = socket_write(sock, resp_msg, resp_msg_len)) != resp_msg_len)
    {
    if (debug_mode == TRUE)
      fprintf(stderr, "Can not send close message to pbs_server!! (socket #%d)\n", sock);
    }
  free(resp_msg);
  }

void *process_svr_conn(
    
  void *sock)

  {
  const char *className = "trqauthd";
  int         rc = PBSE_NONE;
  char       *server_name = NULL;
  int         server_port = 0;
  int         auth_type = 0;
  char       *user_name = NULL;
  int         user_pid = 0;
  int         user_sock = 0;
  char       *error_msg = NULL;
  char       *send_message = NULL;
  int         send_len = 0;
  char       *trq_server_addr = NULL;
  int         trq_server_addr_len = 0;
  int         svr_sock = -1;
  int         msg_len = 0;
  int         debug_mark = 0;
  int         local_socket = *(int *)sock;
  char        msg_buf[1024];
  long long   req_type;  /* Type of request coming in */

  rc = socket_read_num(local_socket, &req_type);
  if (rc == PBSE_NONE)
    {
    switch (req_type)
      {
      case TRQ_GET_ACTIVE_SERVER:
        {
        /* rc will get evaluated after the switch statement. */
        rc = build_active_server_response(&send_message);
        break;
        }

      case TRQ_VALIDATE_ACTIVE_SERVER:
        {
        if ((rc = socket_read_num(local_socket, (long long *)&server_port)) != PBSE_NONE)
          {
          break;
          }
        else if ((rc = validate_server(server_name, server_port, NULL, NULL)) != PBSE_NONE)
          {
          break;
          }
        else if ((rc = build_active_server_response(&send_message)) != PBSE_NONE)
          {
          break;
          }

        break;
        }

      case TRQ_AUTH_CONNECTION:
        {

        int         disconnect_svr = TRUE;
        /* incoming message format is:
         * trq_system_len|trq_system|trq_port|Validation_type|user_len|user|pid|psock|
         * message format to pbs_server is:
         * +2+22+492+user+sock+0
         * format from pbs_server is:
         * +2+2+0+0+1
         * outgoing message format is:
         * #|msg_len|message|
         * Send response to client here!!
         * Disconnect message to svr:
         * +2+22+592+{user_len}{user}
         *
         * msg to client in the case of success:
         * 0|0||
         */

        if ((rc = parse_request_client(local_socket, &server_name, &server_port, &auth_type, &user_name, &user_pid, &user_sock)) != PBSE_NONE)
          {
          disconnect_svr = FALSE;
          debug_mark = 1;
          }
        else if ((rc = validate_user(local_socket, user_name, user_pid, msg_buf)) != PBSE_NONE)
          {
          log_record(PBSEVENT_CLIENTAUTH | PBSEVENT_FORCE, PBS_EVENTCLASS_TRQAUTHD, __func__, msg_buf);
          disconnect_svr = FALSE;
          debug_mark = 1;
          }
        else if ((rc = get_trq_server_addr(server_name, &trq_server_addr, &trq_server_addr_len)) != PBSE_NONE)
          {
          disconnect_svr = FALSE;
          debug_mark = 2;
          }
        else if ((svr_sock = socket_get_tcp_priv()) < 0)
          {
          rc = PBSE_SOCKET_FAULT;
          disconnect_svr = FALSE;
          debug_mark = 3;
          }
        else if ((rc = socket_connect(&svr_sock, trq_server_addr, trq_server_addr_len, server_port, AF_INET, 1, &error_msg)) != PBSE_NONE)
          {
          /* for now we only need ssh_key and sign_key as dummys */
          char *ssh_key = NULL;
          char *sign_key = NULL;
          char  log_buf[LOCAL_LOG_BUF_SIZE];

          validate_server(server_name, server_port, ssh_key, &sign_key);
          sprintf(log_buf, "Active server is %s", active_pbs_server);
          log_event(PBSEVENT_CLIENTAUTH, PBS_EVENTCLASS_TRQAUTHD, __func__, log_buf);
          disconnect_svr = FALSE;
          debug_mark = 4;
          socket_close(svr_sock);
          }
        else if ((rc = build_request_svr(auth_type, user_name, user_sock, &send_message)) != PBSE_NONE)
          {
          socket_close(svr_sock);
          disconnect_svr = FALSE;
          debug_mark = 5;
          }
        else if ((send_len = ((send_message == NULL)?0:strlen(send_message)) ) <= 0)
          {
          socket_close(svr_sock);
          disconnect_svr = FALSE;
          rc = PBSE_INTERNAL;
          debug_mark = 6;
          }
        else if ((rc = socket_write(svr_sock, send_message, send_len)) != send_len)
          {
          socket_close(svr_sock);
          disconnect_svr = FALSE;
          rc = PBSE_SOCKET_WRITE;
          debug_mark = 7;
          }
        else if ((rc = parse_response_svr(svr_sock, &error_msg)) != PBSE_NONE)
          {
          socket_close(svr_sock);
          disconnect_svr = FALSE;
          debug_mark = 8;
          }
        else
          {
          /* Success case */
          if (send_message != NULL)
            free(send_message);
          send_message = (char *)calloc(1, 6);
          if(send_message != NULL)
            strcat(send_message, "0|0||");
          if (debug_mode == TRUE)
            {
            fprintf(stderr, "Conn to %s port %d success. Conn %d authorized\n", server_name, server_port, user_sock);
            }

          snprintf(msg_buf, sizeof(msg_buf),
            "User %s at IP:port %s:%d logged in", user_name, server_name, server_port);
          log_record(PBSEVENT_CLIENTAUTH | PBSEVENT_FORCE, PBS_EVENTCLASS_TRQAUTHD,
            className, msg_buf);
          }

        if (TRUE == disconnect_svr)
          {
          send_svr_disconnect(svr_sock, user_name);
          socket_close(svr_sock);
          }
        break;
        }
      default:
        rc = PBSE_IVALREQ;
        break;
      }
    }
  else
    {
    sprintf(msg_buf, "socket_read_num failed: %d", rc);
    log_record(PBSEVENT_CLIENTAUTH, PBS_EVENTCLASS_TRQAUTHD, __func__, msg_buf);
    }

#ifdef UNIT_TEST
  process_svr_conn_rc = rc;
#endif

  if (rc != PBSE_NONE)
    {
    /* Failure case */
    if (send_message != NULL)
      free(send_message);
    
    msg_len = 6 + 1 + 6 + 1 + 1;
    
    if (error_msg == NULL)
      {
      char *tmp_err = pbse_to_txt(rc);
      if (tmp_err != NULL)
        error_msg = strdup(tmp_err);
      else
        error_msg = strdup("");
      }

    msg_len += strlen(error_msg);
    send_message = (char *)calloc(1, msg_len);
    if(send_message != NULL)
      snprintf(send_message, msg_len, "%d|%d|%s|", rc, (int)strlen(error_msg), error_msg);
    if (debug_mode == TRUE)
      {
      fprintf(stderr, "Conn to %s port %d Fail. Conn %d not authorized (dm = %d, Err Num %d)\n", server_name, server_port, user_sock, debug_mark, rc);
      }

    snprintf(msg_buf, sizeof(msg_buf),
      "User %s at IP:port %s:%d login attempt failed --%s", 
        (user_name) ? user_name : "null",
        (server_name) ? server_name : "null", server_port, 
        (error_msg) ? error_msg : "nullL");
    log_record(PBSEVENT_CLIENTAUTH | PBSEVENT_FORCE, PBS_EVENTCLASS_TRQAUTHD,
      className, msg_buf);
    }

  if(send_message != NULL)
    rc = socket_write(local_socket, send_message, strlen(send_message));


  if (trq_server_addr != NULL)
    free(trq_server_addr);

  if (server_name != NULL)
    free(server_name);

  if (user_name != NULL)
    free(user_name);

  if (error_msg != NULL)
    free(error_msg);

  if (send_message != NULL)
    free(send_message);

  socket_close(local_socket);
  free(sock);

  return(NULL);
  } /* END process_svr_conn() */



