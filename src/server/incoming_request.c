/*
*         OpenPBS (Portable Batch System) v2.3 Software License
*
* Copyright (c) 1999-2000 Veridian Information Solutions, Inc.
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
/*
 * The entry point function for pbs_daemon.
 *
 * Included public functions re:
 *
 * main initialization and main loop of pbs_daemon
 */

#include <stdio.h>
#include "server_comm.h"
#include "dis.h"
#include "threadpool.h"
#include "pbs_error.h"
#include "log.h"
#include "libpbs.h"
#include "net_connect.h"
#include "batch_request.h"

const int SHORT_TIMEOUT = 5;

char *netaddr(struct sockaddr_in *ap);
void netcounter_incr();

// We have now separated into one timeout for incoming connections and a separate one for 
// outgoing connections.
time_t pbs_incoming_tcp_timeout = PBS_TCPINTIMEOUT;

int get_protocol_type(

  struct tcp_chan *chan,
  int             &rc)

  {
  unsigned int timeout = SHORT_TIMEOUT;
  int          protocol_type;

  // we don't want to get all threads stuck polling for input, so make sure that
  // we have a short timeout to start.
  if (timeout > pbs_incoming_tcp_timeout)
    timeout = pbs_incoming_tcp_timeout;

  protocol_type = disrui_peek(chan, &rc, timeout);

  if (chan->IsTimeout)
    {
    // If we aren't too busy try again. If we are too busy just move on so we
    // don't completely jam ourselves.
    if ((timeout < pbs_incoming_tcp_timeout) &&
        (threadpool_is_too_busy(request_pool, ATR_DFLAG_MGRD) == false))
      {
      chan->IsTimeout = 0;
      protocol_type = disrui_peek(chan, &rc, pbs_incoming_tcp_timeout - SHORT_TIMEOUT);
      }
    }

  return(protocol_type);
  } /* END get_protocol_type() */



int process_pbs_server_port(
     
  int   sock,
  int   is_scheduler_port,
  long *args)
 
  {
  int              protocol_type;
  int              rc = PBSE_NONE;
  char             log_buf[LOCAL_LOG_BUF_SIZE];
  struct tcp_chan *chan = NULL;
   
  if ((chan = DIS_tcp_setup(sock)) == NULL)
    {
    return(PBSE_MEM_MALLOC);
    }

  protocol_type = get_protocol_type(chan, rc);
  
  switch (protocol_type)
    {
    case PBS_BATCH_PROT_TYPE:
      
      rc = process_request(chan);
      
      break;
      
    case IS_PROTOCOL:

      {
      // always close the socket for is requests 
      rc = PBSE_SOCKET_CLOSE;

      is_request_info isr;

      isr.chan = chan;
      isr.args = args;
  
      if (threadpool_is_too_busy(request_pool, ATR_DFLAG_MGRD) == false)
        svr_is_request(&isr);
      else
        {
        write_tcp_reply(chan, IS_PROTOCOL, IS_PROTOCOL_VER, IS_STATUS, PBSE_SERVER_BUSY);
        DIS_tcp_cleanup(chan);
        }

      // don't let this get cleaned up below
      chan = NULL;
      
      break;
      }

    default:
      {
      struct sockaddr     s_addr;
      struct sockaddr_in *addr;
      socklen_t           len = sizeof(s_addr);

      if (getpeername(sock, &s_addr, &len) == 0)
        {
        addr = (struct sockaddr_in *)&s_addr;
        
        if (protocol_type == 0)
          {
          /* 
           * Don't log error if close is on scheduler port.  Scheduler is
           * responsible for closing the connection
           */
          if (!is_scheduler_port)
            {
            if (LOGLEVEL >= 8)
              {
              snprintf(log_buf, sizeof(log_buf),
                "protocol_type: %d: Socket (%d) close detected from %s", protocol_type, sock, netaddr(addr));
              log_event(PBSEVENT_SYSTEM, PBS_EVENTCLASS_REQUEST, __func__, log_buf);
              }
            }

          if (chan->IsTimeout)
            {
            chan->IsTimeout = 0;
            rc = PBSE_TIMEOUT;
            }
          else
            rc = PBSE_SOCKET_CLOSE;
          }
        else
          {
          snprintf(log_buf,sizeof(log_buf),
              "Socket (%d) Unknown protocol %d from %s", sock, protocol_type, netaddr(addr));
          log_err(-1, __func__, log_buf);
          rc = PBSE_SOCKET_DATA;
          }
        }
      else
        rc = PBSE_SOCKET_CLOSE;

      break;
      }
    }

  if (chan != NULL)
    DIS_tcp_cleanup(chan);

  return(rc);
  }  /* END process_pbs_server_port() */




void *start_process_pbs_server_port(
    
  void *new_sock)

  {
  long *args = (long *)new_sock;
  int sock;
  int rc = PBSE_NONE;
 
  sock = (int)args[0];

  while ((rc != PBSE_SOCKET_DATA) &&
         (rc != PBSE_SOCKET_INFORMATION) &&
         (rc != PBSE_INTERNAL) &&
         (rc != PBSE_SYSTEM) &&
         (rc != PBSE_MEM_MALLOC) &&
         (rc != PBSE_SOCKET_CLOSE) &&
         (rc != PBSE_TIMEOUT))
    {
    netcounter_incr();

    rc = process_pbs_server_port(sock, FALSE, args);
    }

  free(new_sock);
  close_conn(sock, FALSE);

  /* Thread exit */
  return(NULL);
  }
