
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
#include <unistd.h>

#include "pbs_ifl.h"
#include "dis.h"
#include "pbs_error.h"
#include "resizable_array.h"
#include "utils.h"




/* external global vars */
extern time_t time_now;



mom_hierarchy_t *initialize_mom_hierarchy(void)

  {
  /* malloc can be used here as ALL the variables inside the struct are
   * manually initialized
   */
  mom_hierarchy_t *nt = malloc(sizeof(mom_hierarchy_t));
  nt->paths = initialize_resizable_array(INITIAL_SIZE_NETWORK);

  if (nt->paths == NULL)
    {
    free(nt);
    nt = NULL;
    }

  nt->current_path  = -1;
  nt->current_level = -1;
  nt->current_node  = -1;

  return(nt);
  } /* END initialize_mom_hierarchy() */




int add_network_entry(

  mom_hierarchy_t *nt,
  char               *name,
  struct hostent     *hp,
  unsigned short      rm_port,
  unsigned short      service_port,
  int                 path,
  int                 level)

  {
  int              rc;
  node_comm_t     *nc = malloc(sizeof(node_comm_t));
  resizable_array *levels;
  resizable_array *node_comm_entries;

  memset(nc,0,sizeof(node_comm_t));

  /* check if the path is already in the array */
  if (nt->paths->num > path)
    levels = (resizable_array *)nt->paths->slots[path+1].item;
  else
    {
    /* insert a new path */
    levels = initialize_resizable_array(INITIAL_SIZE_NETWORK);

    if (levels == NULL)
      return(-1);

    if ((rc = insert_thing(nt->paths,levels)) < 0)
      return(rc);
    }

  /* check if the level is in the array already */
  if (levels->num > level)
    node_comm_entries = (resizable_array *)levels->slots[level+1].item;
  else
    {
    /* insert a new level */
    node_comm_entries = initialize_resizable_array(INITIAL_SIZE_NETWORK);

    if (node_comm_entries == NULL)
      return(-1);

    if ((rc = insert_thing(levels,node_comm_entries)) < 0)
      return(rc);
    }

  nc->service_port = service_port;

  /* finally, insert the entry into the node_comm_entries */
  /* initialize the node comm entry */
  nc->sock_addr.sin_family = AF_INET;
  memcpy(&(nc->sock_addr.sin_addr), hp->h_addr_list[0], hp->h_length);
  nc->sock_addr.sin_port = htons(rm_port);


  nc->stream = -1;
  nc->name   = malloc(strlen(name) + 1);
  if (nc->name == NULL)
    return(ENOMEM);
  else
    strcpy(nc->name,name);

  if ((rc = insert_thing(node_comm_entries,nc)) < 0)
    return(rc);
  else
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

  node_comm_t *nc)
    
  {
  nc->stream = tcp_connect_sockaddr((struct sockaddr *)&nc->sock_addr,sizeof(nc->sock_addr));

  if (nc->stream < 0)
    {
    nc->mtime = time_now;
    nc->bad = TRUE;
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
 * returns -1 on FAILURE, stream descriptor otherwise
 */

int tcp_connect_sockaddr(

  struct sockaddr *sa,      /* I */
  size_t           sa_size) /* I */

  {
  char *id = "tcp_connect_sockaddr";
  int   i = 1;
  int   stream = socket(AF_INET,SOCK_STREAM,0);
  
  if (stream < 0)
    {
    /* FAILED */
    log_err(errno,id,"Failed when trying to open tcp connection - socket() failed");
    }
  else if (setsockopt(stream,SOL_SOCKET,SO_REUSEADDR,(char *)&i,sizeof(i)) < 0)
    {
    /* FAILED */
    log_err(errno,id,"Failed when trying to open tcp connection - setsockopt() failed");
    }
  else if (bindresvport(stream,NULL) < 0)
    {
    /* FAILED */
    log_err(errno,id,"Failed when trying to open tcp connection - bindresvport() failed");
    }
  else if (connect(stream,sa,sa_size) < 0)
    {
    /* FAILED */
    log_err(errno,id,"Failed when trying to open tcp connection - connect() failed");
    }
  else
    {
    /* SUCCESS */
    return(stream);
    }

  /* FAILURE */
  if (IS_VALID_STREAM(stream))
    close(stream);

  return(-1);
  } /* END tcp_connect_sockaddr() */




/* 
 * updates which path we're looking at 
 */
node_comm_t *force_path_update(

  mom_hierarchy_t *nt)

  {
  int path;
  int level;
  int node;
  int attempts = 0;
  int updated  = FALSE;
 
  resizable_array    *levels;
  resizable_array    *node_comm_entries;
  node_comm_t        *nc;


  if (nt->paths->num > 1)
    {
    path = nt->paths->num-1;

    while ((path >= 0) &&
           (updated == FALSE))
      {
      levels = (resizable_array *)nt->paths->slots[nt->paths->num].item;
      
      for (level = 0; level < levels->num && updated == FALSE; level++)
        {
        node_comm_entries = (resizable_array *)levels->slots[level+1].item;
        
        for (node = 0; node < node_comm_entries->num; node++)
          {
          nc = (node_comm_t *)node_comm_entries->slots[node+1].item;

          if ((nc->bad == TRUE) &&
              (time_now - nc->mtime <= NODE_COMM_RETRY_TIME))
            continue;

          if (rm_establish_connection(nc) == PBSE_NONE)
            {
            /* SUCCESS, return the new node */
            return(nc);
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
    levels = (resizable_array *)nt->paths->slots[nt->paths->num].item;

    for (level = 0; level < levels->num && updated == FALSE; level++)
      {
      node_comm_entries = (resizable_array *)levels->slots[level+1].item;

      for (node = 0; node < node_comm_entries->num; node++)
        {
        nc = (node_comm_t *)node_comm_entries->slots[node+1].item;

        if ((nc->bad == TRUE) &&
            (time_now - nc->mtime <= NODE_COMM_RETRY_TIME))
          continue;

        if (rm_establish_connection(nc) == PBSE_NONE)
          {
          /* SUCCESS, return the new node */
          return(nc);
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
  resizable_array *levels;
  resizable_array *node_comm_entries;
  node_comm_t     *nc;

  /* check to make sure we're initialized */
  if (nt->paths->num == 0)
    return(NULL);

  /* if we haven't picked a path, update */
  if ((nt->current_level < 0) ||
      (nt->current_path  < 0) ||
      (nt->current_node  < 0))
    return(force_path_update(nt));

  levels = (resizable_array *)nt->paths->slots[nt->current_path+1].item;
  node_comm_entries = (resizable_array *)levels->slots[nt->current_level+1].item;
  nc = (node_comm_t *)node_comm_entries->slots[nt->current_node+1].item;

  if (nc->stream < 0)
    return(force_path_update(nt));
  else if ((nt->current_path  != 0) ||
           (nt->current_level != 0) ||
           (nt->current_node  != 0))
    {
    if ((time_now - nc->mtime) > NODE_COMM_RETRY_TIME)
      {
      close(nc->stream);
      return(force_path_update(nt));
      }
    }

  return(nc);
  } /* END update_current_path() */




/* 
 * write a reply to a socket.
 * reply consists of 4 integers:
 * protocol, version, command, and then exit code 
 *
 * @return PBSE_NONE if successfully written, else error code 
 */

int write_tcp_reply(

  int sock,
  int protocol,
  int version,
  int command,
  int exit_code)

  {
  int ret;

  if ((ret = diswsi(sock,protocol)) == DIS_SUCCESS)
    {
    if ((ret = diswsi(sock,version)) == DIS_SUCCESS)
      {
      if ((ret = diswsi(sock,command)) == DIS_SUCCESS)
        {
        ret = diswsi(sock,exit_code);

        DIS_tcp_wflush(sock);

        return(ret);
        }
      }
    }

  /* ERROR */
  return(pbs_errno);
  } /* END write_tcp_reply() */




/* 
 * read reply and check for success
 */
int read_tcp_reply(

  int  sock,
  int  protocol,
  int  version,
  int  command,
  int *exit_status)

  {
  static char *id = "read_tcp_reply";

  int ret;
  int value; /* value read from sock */

  *exit_status = UNREAD_STATUS;

  /*DIS_tcp_setup(sock);*/

  if ((value = disrsi(sock,&ret)) == protocol)
    {
    if ((value = disrsi(sock,&ret)) == version)
      {
      if ((value = disrsi(sock,&ret)) == command)
        {
        /* read the exit code */
        *exit_status = disrsi(sock,&ret);

        DIS_tcp_reset(sock,0);
        }
      else
        {
        snprintf(log_buffer,sizeof(log_buffer),
          "Mismatching commands. Expected command %d for protocol %d but read command %d\n",
          command,
          protocol,
          value);
        log_err(-1,id,log_buffer);
        }
      }
    else
      {
      snprintf(log_buffer,sizeof(log_buffer),
        "Mismatching versions. Expected version %d for protocol %d but read version %d\n",
        version,
        protocol,
        value);
      log_err(-1,id,log_buffer);
      }
    }
  else
    {
    snprintf(log_buffer,sizeof(log_buffer),
      "Mismatching protocols. Expected protocol %d but read reply for %d\n",
      protocol,
      value);
    log_err(-1,id,log_buffer);
    }

  return(ret);
  } /* END read_tcp_reply() */




/* END u_mom_hierarchy.c */

