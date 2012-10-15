#include "license_pbs.h" /* See here for the software license */
#include "lib_ifl.h"

#include <limits.h> /* LOGIN_NAME_MAX */
#include <netinet/in.h> /* in_addr_t */
#include <stdio.h> /* sprintf */
#include <arpa/inet.h> /* inet_addr */
#include "../Libnet/lib_net.h" /* get_hostaddr, socket_* */
#include "../../include/log.h" /* log event types */


char *trq_addr = NULL;
int trq_addr_len;
char *trq_server_name = NULL;
int debug_mode = 0;

int parse_request_client(
    int sock,
    char **server_name,
    int *server_port,
    int *auth_type,
    char **user,
    int *user_sock)
  {
  int rc = PBSE_NONE;
  long long tmp_val = 0, tmp_port = 0, tmp_auth_type = 0, tmp_sock = 0;
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
  else if ((rc = socket_read_num(sock, &tmp_sock)) != PBSE_NONE)
    {
    }
  else
    {
    *server_port = (int)tmp_port;
    *auth_type = (int)tmp_auth_type;
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

void send_svr_disconnect(int sock, char *user_name)
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
  char *className = "trqauthd";
  int   rc = PBSE_NONE;
  char *server_name = NULL;
  int   server_port = 0;
  int   auth_type = 0;
  char *user_name = NULL;
  int   user_sock = 0;
  char *error_msg = NULL;
  char *send_message = NULL;
  int   send_len = 0;
  char *trq_server_addr = NULL;
  int   trq_server_addr_len = 0;
  int   disconnect_svr = TRUE;
  int   svr_sock = 0;
  int   msg_len = 0;
  int   debug_mark = 0;
  int   local_socket = *(int *)sock;
  char  msg_buf[1024];

  /* incoming message format is:
   * trq_system_len|trq_system|trq_port|Validation_type|user_len|user|psock|
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

  if ((rc = parse_request_client(local_socket, &server_name, &server_port, &auth_type, &user_name, &user_sock)) != PBSE_NONE)
    {
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
    disconnect_svr = FALSE;
    debug_mark = 4;
    socket_close(svr_sock);
    }
  else if ((rc = build_request_svr(auth_type, user_name, user_sock, &send_message)) != PBSE_NONE)
    {
    socket_close(svr_sock);
    debug_mark = 5;
    }
  else if ((send_len = strlen(send_message)) <= 0)
    {
    socket_close(svr_sock);
    rc = PBSE_INTERNAL;
    debug_mark = 6;
    }
  else if ((rc = socket_write(svr_sock, send_message, send_len)) != send_len)
    {
    socket_close(svr_sock);
    rc = PBSE_SOCKET_WRITE;
    debug_mark = 7;
    }
  else if ((rc = parse_response_svr(svr_sock, &error_msg)) != PBSE_NONE)
    {
    socket_close(svr_sock);
    debug_mark = 8;
    }
  else
    {
    /* Success case */
    if (send_message != NULL)
      free(send_message);
    send_message = (char *)calloc(1, 6);
    strcat(send_message, "0|0||");
    if (debug_mode == TRUE)
      {
      fprintf(stderr, "Conn to %s port %d success. Conn %d authorized\n", server_name, server_port, user_sock);
      }

    snprintf(msg_buf, sizeof(msg_buf),
      "User %s at IP:port %s:%d logged in", user_name, server_name, server_port);
    log_record(PBSEVENT_CLIENTAUTH, PBS_EVENTCLASS_TRQAUTHD,
      className, msg_buf);
    }

  if (rc != PBSE_NONE)
    {
    /* Failure case */
    if (send_message != NULL)
      free(send_message);
    msg_len = 6 + 1 + 6 + 1 + 1;
    if (error_msg == NULL)
      error_msg = strdup(pbse_to_txt(rc));
    msg_len += strlen(error_msg);
    send_message = (char *)calloc(1, msg_len);
    snprintf(send_message, msg_len, "%d|%d|%s|", rc, (int)strlen(error_msg), error_msg);
    if (debug_mode == TRUE)
      {
      fprintf(stderr, "Conn to %s port %d Fail. Conn %d not authorized (dm = %d, Err Num %d)\n", server_name, server_port, user_sock, debug_mark, rc);
      }

    snprintf(msg_buf, sizeof(msg_buf),
      "User %s at IP:port %s:%d login attempt failed --%s", user_name, server_name, server_port, error_msg);
    log_record(PBSEVENT_CLIENTAUTH, PBS_EVENTCLASS_TRQAUTHD,
      className, msg_buf);
    }

  rc = socket_write(local_socket, send_message, strlen(send_message));

  if (TRUE == disconnect_svr)
    {
    send_svr_disconnect(svr_sock, user_name);
    socket_close(svr_sock);
    }

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



