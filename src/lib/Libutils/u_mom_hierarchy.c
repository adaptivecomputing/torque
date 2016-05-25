
/*
*         OpenPBS (Portable Batch System) v2.3 Software License
*
* Copyright (c) 1999-2010 Veridian Information Solutions, Inc.
* All rights reserved.
*
* ---------------------------------------------------------------------------
* For a license to use or redistribute the OpenPBS software under conditions
* other than those described below, or to purchase support for this software,
* please contact Veridian Systems, PBS Products Department ("Licensor") at:
*
*    www.OpenPBS.org  +1 650 967-4675                  sales@OpenPBS.org
*                        877 902-4PBS (US toll-free)
* ---------------------------------------------------------------------------
*
* This license covers use of the OpenPBS v2.3 software (the "Software") at
* your site or location, and, for certain users, redistribution of the
* Software to other sites and locations.  Use and redistribution of
* OpenPBS v2.3 in source and binary forms, with or without modification,
* are permitted provided that all of the following conditions are met.
* After December 31, 2001, only conditions 3-6 must be met:
*
* 1. Commercial and/or non-commercial use of the Software is permitted
*    provided a current software registration is on file at www.OpenPBS.org.
*    If use of this software contributes to a publication, product, or
*    service, proper attribution must be given; see www.OpenPBS.org/credit.html
*
* 2. Redistribution in any form is only permitted for non-commercial,
*    non-profit purposes.  There can be no charge for the Software or any
*    software incorporating the Software.  Further, there can be no
*    expectation of revenue generated as a consequence of redistributing
*    the Software.
*
* 3. Any Redistribution of source code must retain the above copyright notice
*    and the acknowledgment contained in paragraph 6, this list of conditions
*    and the disclaimer contained in paragraph 7.
*
* 4. Any Redistribution in binary form must reproduce the above copyright
*    notice and the acknowledgment contained in paragraph 6, this list of
*    conditions and the disclaimer contained in paragraph 7 in the
*    documentation and/or other materials provided with the distribution.
*
* 5. Redistributions in any form must be accompanied by information on how to
*    obtain complete source code for the OpenPBS software and any
*    modifications and/or additions to the OpenPBS software.  The source code
*    must either be included in the distribution or be available for no more
*    than the cost of distribution plus a nominal fee, and all modifications
*    and additions to the Software must be freely redistributable by any party
*    (including Licensor) without restriction.
*
* 6. All advertising materials mentioning features or use of the Software must
*    display the following acknowledgment:
*
*     "This product includes software developed by NASA Ames Research Center,
*     Lawrence Livermore National Laboratory, and Veridian Information
*     Solutions, Inc.
*     Visit www.OpenPBS.org for OpenPBS software support,
*     products, and information."
*
* 7. DISCLAIMER OF WARRANTY
*
* THIS SOFTWARE IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND. ANY EXPRESS
* OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
* OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, AND NON-INFRINGEMENT
* ARE EXPRESSLY DISCLAIMED.
*
* IN NO EVENT SHALL VERIDIAN CORPORATION, ITS AFFILIATED COMPANIES, OR THE
* U.S. GOVERNMENT OR ANY OF ITS AGENCIES BE LIABLE FOR ANY DIRECT OR INDIRECT,
* INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
* LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,
* OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
* LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
* NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
* EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*
* This license will be governed by the laws of the Commonwealth of Virginia,
* without reference to its choice of law rules.
*/
#include "mom_hierarchy.h"

#include <string.h>
#include <time.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h> /* sockaddr_in */
#include <arpa/inet.h> /* inet_ntoa */
#include <unistd.h>

#include "net_cache.h"
#include "pbs_ifl.h"
#include "dis.h"
#include "pbs_error.h"
#include "utils.h"
#include "../Liblog/pbs_log.h"
#include "../Libnet/lib_net.h" /* socket_get_tcp_priv, socket_connect_addr */
#include "lib_ifl.h"
#define LOCAL_LOG_BUF 1024


extern int   mom_hierarchy_retry_time;


extern mom_hierarchy_t *mh;
extern int              LOGLEVEL;


void free_mom_hierarchy(

  mom_hierarchy_t *param_mh)

  {
  free(param_mh);
  } /* END free_mom_hierarchy() */




mom_hierarchy_t *initialize_mom_hierarchy()

  {
  mom_hierarchy_t *nt = (mom_hierarchy_t *)calloc(1, sizeof(mom_hierarchy_t));

  nt->current_path  = -1;
  nt->current_level = -1;
  nt->current_node  = -1;

  return(nt);
  } /* END initialize_mom_hierarchy() */




int add_network_entry(

  mom_hierarchy_t    *nt,
  const char         *name,
  struct addrinfo    *addr_info,
  unsigned short      rm_port,
  int                 path,
  int                 level)

  {
  node_comm_t      nc;

  /* check if the path is already in the array */
  while (nt->paths.size() <= (size_t)path)
    {
    /* insert a new path */
    mom_levels nl;

    nt->paths.push_back(nl);
    }

  mom_levels &levels = nt->paths.at(path);

  /* check if the level is in the array already */
  while (levels.size() <= (size_t)level)
    {
    /* insert a new level */
    mom_nodes nce;

    levels.push_back(nce);
    }
  
  mom_nodes &node_comm_entries = levels.at(level);

  /* finally, insert the entry into the node_comm_entries */
  /* initialize the node comm entry */
  nc.sock_addr.sin_addr = ((struct sockaddr_in *)addr_info->ai_addr)->sin_addr;
  nc.sock_addr.sin_family = AF_INET;
  nc.sock_addr.sin_port = htons(rm_port);
  nc.stream = -1;
  nc.name = name;

  node_comm_entries.push_back(nc);
  
  return(PBSE_NONE);
  } /* END add_network_entry() */




/* 
 * rm_establish_connection
 * establishes a tcp connection between moms to the rm manager port
 * 
 * @param nc - the node we're communicating with
 * @return PBSE_NONE on success, else -1
 */

int rm_establish_connection(

  node_comm_t &nc)
    
  {
  nc.stream = tcp_connect_sockaddr((struct sockaddr *)&nc.sock_addr,sizeof(nc.sock_addr), true);

  if (nc.stream < 0)
    {
    nc.mtime = time(NULL);
    nc.bad = TRUE;
    return(-1);
    }
  else
    return(PBSE_NONE);

  } /* END rm_establish_connection() */





/* 
 * tcp_connect_sockaddr
 *
 * opens a socket to the node with sockaddr
 *
 * returns permanent or transient on FAILURE, stream descriptor otherwise
 */

int tcp_connect_sockaddr(

  struct sockaddr *sa,      /* I */
  size_t           sa_size, /* I */
  bool             use_log) /* I */

  {
  int          rc = PBSE_NONE;
  int          stream = TRANSIENT_SOCKET_FAIL;
  std::string  err_msg;
  char         local_err_buf[LOCAL_LOG_BUF];
  char        *tmp_ip = NULL;
  int          retryCount = 5;

  errno = 0;

  while (retryCount-- >= 0)
    {
    if ((stream = socket_get_tcp_priv()) < 0)
      {
      /* FAILED */
      if (use_log == true)
        log_err(errno,__func__,"Failed when trying to get privileged port - socket_get_tcp_priv() failed");
      }
    else if ((rc = socket_connect_addr(stream, sa, sa_size, 1, err_msg)) != PBSE_NONE)
      {
      /* FAILED */
      if (errno != EINTR) //Interrupted system call is a retryable error so try it again.
        {
        retryCount = -1;
        }
      else
        {
        usleep(10000); //Catch a breath on a retryable error.
        }
      
      if (use_log == true)
        {
        tmp_ip = inet_ntoa(((struct sockaddr_in *)sa)->sin_addr);
        snprintf(local_err_buf, LOCAL_LOG_BUF, "Failed when trying to open tcp connection - connect() failed [rc = %d] [addr = %s:%d]", rc, tmp_ip, htons(((struct sockaddr_in *)sa)->sin_port));
        log_err(-1,__func__,local_err_buf);
        }

      if (err_msg.size() != 0)
        {
        if (use_log == true)
          log_err(-1, __func__, err_msg.c_str());

        err_msg.clear();
        }
      }
    else
      {
      /* SUCCESS */
      return(stream);
      }

    /* FAILURE */
    if (IS_VALID_STREAM(stream))
      {
      close(stream);
      stream = TRANSIENT_SOCKET_FAIL;
      }
    }

  return(stream);
  } /* END tcp_connect_sockaddr() */




/* 
 * updates which path we're looking at 
 */
node_comm_t *force_path_update(

  mom_hierarchy_t *nt)

  {
  int    path;
  int    level;
  int    node;
  int    attempts = 0;
  int    updated  = FALSE;
  time_t time_now = time(NULL);
 
  if (nt->paths.size() > 1)
    {
    path = nt->paths.size() - 1;

    while ((path >= 0) &&
           (updated == FALSE))
      {
      mom_levels &levels = nt->paths.at(nt->paths.size() - 1);
     
      for (level = levels.size() - 1; level >= 0 && updated == FALSE; level--)
        {
        mom_nodes &node_comm_entries = levels.at(level);
        
        for (node = 0; node < (int)node_comm_entries.size(); node++)
          {
          node_comm_t &nc = node_comm_entries.at(node);

          if ((nc.bad == TRUE) &&
              (time_now - nc.mtime <= mom_hierarchy_retry_time))
            continue;

          if (rm_establish_connection(nc) == PBSE_NONE)
            {
            /* SUCCESS, return the new node */
            return(&nc);
            }
          else
            attempts++;
          } /* END for each node comm point */

        /* exit to examine alternate paths if we're above NODE_COMM_MAX_ATTEMPTS */
        if (attempts > NODE_COMM_MAX_ATTEMPTS)
          break;
        } /* END for each level */

      path--;
      } /* END for each path */
    }
  else
    {
    mom_levels &levels = nt->paths.at(nt->paths.size()-1);

    for (level = levels.size() - 1; level >= 0 && updated == FALSE; level--)
      {
      mom_nodes &node_comm_entries = levels.at(level);

      for (node = 0; node < (int)node_comm_entries.size(); node++)
        {
        node_comm_t &nc = node_comm_entries.at(node);

        if ((nc.bad == TRUE) &&
            (time_now - nc.mtime <= mom_hierarchy_retry_time))
          continue;

        if (rm_establish_connection(nc) == PBSE_NONE)
          {
          /* SUCCESS, return the new node */
          return(&nc);
          }
        } /* END for each node comm point */
      } /* END for each level */
    }

  /* if this ever happens the mom will go to the server */
  return(NULL);
  } /* END force_path_update() */




/* 
 * @return a node, or NULL. if NULL, this mom should talk to pbs_server
 */
node_comm_t *update_current_path(

  mom_hierarchy_t *nt)

  {
  /* check to make sure we're initialized */
  if (nt->paths.size() == 0)
    return(NULL);

  /* if we haven't picked a path, update */
  if ((nt->current_level < 0) ||
      (nt->current_path  < 0) ||
      (nt->current_node  < 0))
    return(force_path_update(nt));

  mom_levels  &levels = nt->paths.at(nt->current_path);
  mom_nodes   &node_comm_entries = levels.at(nt->current_level);
  node_comm_t &nc = node_comm_entries.at(nt->current_node);

  if (nc.stream < 0)
    return(force_path_update(nt));
  else if ((nt->current_path  != 0) ||
           (nt->current_level != 0) ||
           (nt->current_node  != 0))
    {
    if ((time(NULL) - nc.mtime) > mom_hierarchy_retry_time)
      {
      close(nc.stream);
      return(force_path_update(nt));
      }
    }

  return(&nc);
  } /* END update_current_path() */




/* 
 * write a reply to a socket.
 * reply consists of 4 integers:
 * protocol, version, command, and then exit code 
 *
 * @return PBSE_NONE if successfully written, else error code 
 */

int write_tcp_reply(

  struct tcp_chan *chan,
  int protocol,
  int version,
  int command,
  int exit_code)

  {
  int ret;

  if ((ret = diswsi(chan,protocol)) == DIS_SUCCESS)
    {
    if ((ret = diswsi(chan,version)) == DIS_SUCCESS)
      {
      if ((ret = diswsi(chan,command)) == DIS_SUCCESS)
        {
        if ((ret = diswsi(chan,exit_code)) == DIS_SUCCESS)
          ret = DIS_tcp_wflush(chan);

        return(ret);
        }
      }
    }

  /* ERROR */
  return(ret);
  } /* END write_tcp_reply() */




/* 
 * read reply and check for success
 */
int read_tcp_reply(

  struct tcp_chan *chan,
  int              protocol,
  int              version,
  int              command,
  int             *exit_status)

  {
  char log_buf[LOCAL_LOG_BUF_SIZE];
  int  ret;
  int  value; /* value read from sock */

  *exit_status = UNREAD_STATUS;

  if (LOGLEVEL >= 6)
    {
    sprintf(log_buf, "protocol: %d  version: %d  command:%d  sock:%d", protocol, version, command, chan->sock);
    log_event(PBSEVENT_JOB, PBS_EVENTCLASS_JOB,__func__, log_buf);
    }

  if ((value = disrsi(chan,&ret)) != protocol)
    {
    snprintf(log_buf,sizeof(log_buf),
      "Mismatching protocols. Expected protocol %d but read reply for %d\n",
      protocol,
      value);
    log_err(-1, __func__, log_buf);
    }
  else if (ret != DIS_SUCCESS)
    {
    }
  else if ((value = disrsi(chan,&ret)) != version)
    {
    snprintf(log_buf, sizeof(log_buf),
      "Mismatching versions. Expected version %d for protocol %d but read version %d\n",
      version,
      protocol,
      value);
    log_err(-1, __func__, log_buf);
    }
  else if (ret != DIS_SUCCESS)
    {
    }
  else if ((value = disrsi(chan,&ret)) != command)
    {
    snprintf(log_buf, sizeof(log_buf),
      "Mismatching commands. Expected command %d for protocol %d but read command %d\n",
      command,
      protocol,
      value);
    log_err(-1, __func__, log_buf);
    }
  else if (ret != DIS_SUCCESS)
    {
    }
  else
    {
    /* read the exit code */
    *exit_status = disrsi(chan,&ret);
    }

  if (ret != DIS_SUCCESS)
    {
    if (ret >= 0)
      {
      snprintf(log_buf, sizeof(log_buf),
        "Could not read reply for protocol %d command %d: %s",
        protocol, command, dis_emsg[ret]);
      }
    else
      {
      snprintf(log_buf, sizeof(log_buf),
        "Could not read reply for protocol %d command %d",
        protocol, command);
      }
    log_err(-1, __func__, log_buf);
    }

  return(ret);
  } /* END read_tcp_reply() */



int handle_level(
    
  char           *level_iter,
  int             path_index,
  int            &level_index)

  {
  const char     *delims = ",";
  char           *host_tok;

  level_index++;

  /* find each hostname */
  host_tok = threadsafe_tokenizer(&level_iter, delims);

  while (host_tok != NULL)
    {
    struct addrinfo *addr_info = NULL;
    char            *colon;
    unsigned short   rm_port;

    host_tok = trim(host_tok);

    if ((colon = strchr(host_tok, ':')) != NULL)
      {
      *colon = '\0';
      rm_port = strtol(colon+1, NULL, 10);
      }
    else
      rm_port = PBS_MANAGER_SERVICE_PORT;

    if (pbs_getaddrinfo(host_tok, NULL, &addr_info) == 0)
      add_network_entry(mh, host_tok, addr_info, rm_port, path_index, level_index);

    host_tok = threadsafe_tokenizer(&level_iter, delims);
    }

  return(PBSE_NONE);
  } /* END handle_level() */



int handle_path(

  char           *path_iter,
  int            &path_index)

  {
  char  log_buf[LOCAL_LOG_BUF_SIZE];
  char *level_parent;
  char *level_child;

  int   level_index = -1;

  path_index++;
  
  /* iterate over each level in the path */
  while (get_parent_and_child(path_iter,&level_parent,&level_child,&path_iter) == PBSE_NONE)
    {
    if (!strncmp(level_parent,"level",strlen("level")))
      {
      handle_level(level_child, path_index, level_index);
      }
    else
      {
      /* non-fatal error */
      snprintf(log_buf, sizeof(log_buf),
        "Found noise in the mom hierarchy file. Ignoring <%s>%s</%s>",
        level_parent, level_child, level_parent);
      log_err(-1, __func__, log_buf);
      }
    }
  
  return(PBSE_NONE);
  } /* END handle_path() */



void parse_mom_hierarchy(
    
  int fds)

  {
  int             bytes_read;
  char            buffer[MAXLINE<<10];
  char           *current;
  char           *parent;
  char           *child;
  char            log_buf[LOCAL_LOG_BUF_SIZE];
  int             path_index = -1;

  memset(&buffer, 0, sizeof(buffer));

  if ((bytes_read = read_ac_socket(fds, buffer, sizeof(buffer) - 1)) < 0)
    {
    snprintf(log_buf, sizeof(log_buf),
      "Unable to read from mom hierarchy file");
    log_err(errno, __func__, log_buf);

    return;
    }
  
  current = buffer;

  while (get_parent_and_child(current, &parent, &child, &current) == PBSE_NONE)

    {
    if (!strncmp(parent,"path",strlen("path")))
      handle_path(child, path_index);
    else
      {
      /* non-fatal error */
      snprintf(log_buf, sizeof(log_buf),
        "Found noise in the mom hierarchy file. Ignoring <%s>%s</%s>",
        parent, child, parent);
      log_err(-1, __func__, log_buf);
      }
    }
  } /* END parse_mom_hierarchy() */



/* END u_mom_hierarchy.c */

