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




#include <stdio.h>
#include <errno.h>
#include <vector>
#include <string>

#include "pbs_config.h"
#include "pbs_nodes.h"
#include "log.h"
#include "dis.h"
#include "alps_functions.h"
#include "attribute.h"
#include "mom_update.h"
#include "u_tree.h"
#include "net_cache.h"
#include "threadpool.h"
#include "../lib/Libnet/lib_net.h"
#include "../lib/Libutils/u_lock_ctl.h"
#include "mutex_mgr.hpp"
#include "server_comm.h"
#include "mom_hierarchy_handler.h"


extern int              allow_any_mom;
extern AvlTree          ipaddrs;


job *get_job_from_job_usage_info(job_usage_info *jui, struct pbsnode *pnode);
int  unlock_ji_mutex(job *, const char *, const char *, int);


/* 
 * reads all of the status information from stream
 * and stores it in a dynamic string
 */

void get_status_info(

  struct tcp_chan          *chan,
  std::vector<std::string> &status)

  {
  char           *ret_info;
  int             rc;

  while (((ret_info = disrst(chan, &rc)) != NULL) && 
         (rc == DIS_SUCCESS))
    {
    if (!strcmp(ret_info, IS_EOL_MESSAGE))
      {
      free(ret_info);
      ret_info = NULL;
      break;
      }

    status.push_back(ret_info);
    free(ret_info);
    ret_info = NULL;
    }

  if (ret_info != NULL)
    free(ret_info);
  } /* END get_status_info() */




int is_reporter_node(

  const char *node_id)

  {
  struct pbsnode *pnode = find_nodebyname(node_id);
  int             rc = FALSE;

  if (pnode != NULL)
    {
    rc = pnode->nd_is_alps_reporter;
    unlock_node(pnode, __func__, NULL, LOGLEVEL);
    }

  return(rc);
  } /* END is_reporter_node() */




int is_stat_get(

  const char      *node_name,
  struct tcp_chan *chan)

  {
  int                      rc;
  char                     log_buf[LOCAL_LOG_BUF_SIZE];
  std::vector<std::string> status_info;

  if (LOGLEVEL >= 3)
    {
    sprintf(log_buf, "received status from node %s", node_name);
    log_record(PBSEVENT_SCHED, PBS_EVENTCLASS_REQUEST, __func__, log_buf);
    }

  get_status_info(chan, status_info);
 
  if (is_reporter_node(node_name))
    rc = process_alps_status(node_name, status_info);
  else
    rc = process_status_info(node_name, status_info);

  return(rc);
  }  /* END is_stat_get() */



/*
 * Function to check if there is a job assigned to this gpu
 */

int gpu_has_job(

  struct pbsnode *pnode,
  int  gpuid)

  {
  job            *pjob;
  char           *gpu_str;
  char           *found_str;
  /* increased so that really high gpu indexes don't bother us */
  char            tmp_str[PBS_MAXHOSTNAME + 10];

  /* check each subnode for a job using a gpuid */
  for (unsigned int i = 0; i < pnode->nd_job_usages.size(); i++)
    {
    // make a copy because we're going to lose the lock below
    job_usage_info jui = pnode->nd_job_usages[i];
    
    if ((pjob = get_job_from_job_usage_info(&jui, pnode)) != NULL)
      {
      mutex_mgr job_mutex(pjob->ji_mutex, true);

      /* Does this job have this gpuid assigned? */
      if ((pjob->ji_qs.ji_state == JOB_STATE_RUNNING) &&
          (pjob->ji_wattr[JOB_ATR_exec_gpus].at_flags & ATR_VFLAG_SET) != 0)
        {
        gpu_str = pjob->ji_wattr[JOB_ATR_exec_gpus].at_val.at_str;
        
        if (gpu_str != NULL)
          {
          snprintf(tmp_str, sizeof(tmp_str), "%s-gpu/%d",
            pnode->nd_name, gpuid);
          
          /* look thru the string and see if it has this host and gpuid.
           * exec_gpus string should be in format of 
           * <hostname>-gpu/<index>[+<hostname>-gpu/<index>...]
           */
          
          found_str = strstr (gpu_str, tmp_str);
          if (found_str != NULL)
            {
            return(TRUE);
            }
          }
        }
      }
    } /* END for each job on node */

  return(FALSE);
  }





/* sync w/#define IS_XXX */

const char *PBSServerCmds2[] =
  {
  "NULL",
  "HELLO",
  "CLUSTER_ADDRS",
  "UPDATE",
  "STATUS",
  "GPU_STATUS",
  NULL
  };



/*************************************************
 * svr_is_request
 *
 * Return: svr_is_request always returns a non-zero value
 *         and it must call close_conn to close the connection
 *         before returning. PBSE_SOCKET_CLOSE is the code
 *         for a successful return. But which ever retun 
 *         code is iused it must terminate the while loop
 *         in start_process_pbs_server_port.
 *************************************************/
void *svr_is_request(
  
  void *v)  

  {
  int                 command = 0;
  int                 ret = DIS_SUCCESS;
  int                 i;
  int                 err;
  char                nodename[PBS_MAXHOSTNAME];
  int                 perm = ATR_DFLAG_MGRD | ATR_DFLAG_MGWR;

  unsigned long       ipaddr;
  unsigned short      mom_port;
  unsigned short      rm_port;
  unsigned long       tmpaddr;
  struct sockaddr_in  addr;
  struct pbsnode     *node = NULL;
  char                log_buf[LOCAL_LOG_BUF_SIZE+1];
  char                msg_buf[80];
  char                tmp[80];
  int                 version;
  struct tcp_chan    *chan;
  long               *args;
  is_request_info    *isr = (is_request_info *)v;

  if (isr == NULL)
    return(NULL);

  chan = isr->chan;
  args = isr->args;
      
  version = disrsi(chan, &ret);

  if (ret != DIS_SUCCESS)
    {
    log_err(-1,  __func__, "Cannot read version - skipping this request.\n");
    close_conn(chan->sock, FALSE);
    DIS_tcp_cleanup(chan);
    return(NULL);
    }

  command = disrsi(chan, &ret);

  if (ret != DIS_SUCCESS)
    {
    snprintf(log_buf, sizeof(log_buf), "could not read command: %d", ret);
    log_err(-1, __func__, log_buf);
    close_conn(chan->sock, FALSE);
    DIS_tcp_cleanup(chan);
    return(NULL);
    }

  if (LOGLEVEL >= 4)
    {
    snprintf(log_buf, LOCAL_LOG_BUF_SIZE,
        "message received from sock %d (version %d)",
        chan->sock,
        version);

    log_event(PBSEVENT_ADMIN, PBS_EVENTCLASS_SERVER, __func__, log_buf);
    }

  /* Just a note to let us know we only do IPv4 for now */
  addr.sin_family = AF_INET;
  memcpy(&addr.sin_addr, (void *)&args[1], sizeof(struct in_addr));
  addr.sin_port = args[2];

  if (version != IS_PROTOCOL_VER)
    {
    netaddr_long(args[1], tmp);
    sprintf(msg_buf, "%s:%ld", tmp, args[2]);
    
    snprintf(log_buf, LOCAL_LOG_BUF_SIZE, "protocol version %d unknown from %s",
      version,
      msg_buf);

    log_err(-1, __func__, log_buf);
    close_conn(chan->sock, FALSE);
    DIS_tcp_cleanup(chan);
    return(NULL);
    }

  /* check that machine is known */
  mom_port = disrsi(chan, &ret);
  rm_port = disrsi(chan, &ret);

  if (LOGLEVEL >= 3)
    {
    netaddr_long(args[1], tmp);
    sprintf(msg_buf, "%s:%ld", tmp, args[2]);
    snprintf(log_buf, LOCAL_LOG_BUF_SIZE,
      "message received from addr %s: mom_port %d  - rm_port %d",
      msg_buf,
      mom_port,
      rm_port);

    log_event(PBSEVENT_ADMIN,PBS_EVENTCLASS_SERVER,__func__,log_buf);
    }

  ipaddr = args[1];
  
  if ((node = AVL_find(ipaddr, mom_port, ipaddrs)) != NULL)
    {
    lock_node(node, __func__, "AVL_find", LOGLEVEL);
    } /* END if AVL_find != NULL) */
  else if (allow_any_mom)
    {
    const char *name = get_cached_nameinfo(&addr);

    if (name != NULL)
      snprintf(nodename, sizeof(nodename), "%s", name);
    else if (getnameinfo((struct sockaddr *)&addr, sizeof(addr), nodename, sizeof(nodename)-1, NULL, 0, 0) != 0)
      {
      tmpaddr = ntohl(addr.sin_addr.s_addr);
      sprintf(nodename, "0x%lX", tmpaddr);
      }
    else
      insert_addr_name_info(NULL, nodename);

    err = create_partial_pbs_node(nodename, ipaddr, perm);

    if (err == PBSE_NONE)
      {
      node = find_nodebyname(nodename);
      }                                                         
    }
    
  if (node == NULL)
    {
    /* node not listed in trusted ipaddrs list */
    netaddr_long(args[1], tmp);
    sprintf(msg_buf, "%s:%ld", tmp, args[2]);
    
    snprintf(log_buf, LOCAL_LOG_BUF_SIZE,
      "bad attempt to connect from %s (address not trusted - check entry in server_priv/nodes)",
      msg_buf);
    
    if (LOGLEVEL >= 2)
      {
      log_record(PBSEVENT_SCHED, PBS_EVENTCLASS_REQUEST, __func__, log_buf);
      }
    else
      {
      log_err(-1, __func__, log_buf);
      }
    
    close_conn(chan->sock, FALSE);
    DIS_tcp_cleanup(chan);
    return(NULL);
    }

  if (LOGLEVEL >= 3)
    {
    netaddr_long(args[1], tmp);
    sprintf(msg_buf, "%s:%ld", tmp, args[2]);

    snprintf(log_buf, LOCAL_LOG_BUF_SIZE,
     "message %s (%d) received from mom on host %s (%s) (sock %d)",
     PBSServerCmds2[command],
     command,
     node->nd_name,
     msg_buf,
     chan->sock);

    log_event(PBSEVENT_ADMIN,PBS_EVENTCLASS_SERVER,__func__,log_buf);
    }

  mutex_mgr node_mutex(node->nd_mutex, true);

  switch (command)
    {
    case IS_NULL:  /* a ping from server */

      DBPRT(("%s: IS_NULL\n", __func__))

      break;

    case IS_UPDATE:

      DBPRT(("%s: IS_UPDATE\n", __func__))

      i = disrui(chan, &ret);

      if (ret != DIS_SUCCESS)
        {
        if (LOGLEVEL >= 1)
          {
          snprintf(log_buf, LOCAL_LOG_BUF_SIZE,
              "IS_UPDATE error %d on node %s\n", ret, node->nd_name);

          log_err(ret, __func__, log_buf);
          }

        goto err;
        }

      DBPRT(("%s: IS_UPDATE %s 0x%x\n", __func__, node->nd_name, i))

      update_node_state(node, i);

      if ((node->nd_state & INUSE_DOWN) != 0)
        {
        node->nd_mom_reported_down = TRUE;
        }

      break;

    case IS_STATUS:

      {
      std::string node_name = node->nd_name;
     
      if (LOGLEVEL >= 2)
        {
        snprintf(log_buf, LOCAL_LOG_BUF_SIZE,
            "IS_STATUS received from %s", node->nd_name);

        log_event(PBSEVENT_ADMIN, PBS_EVENTCLASS_SERVER, __func__, log_buf);
        }

      node_mutex.unlock();

      ret = is_stat_get(node_name.c_str(), chan);

      node = find_nodebyname(node_name.c_str());

      if (node != NULL)
        {
        node->nd_stream = -1;
        node_mutex.mark_as_locked();

        if (ret == SEND_HELLO)
          {
          //struct hello_info *hi = new hello_info(node->nd_id);
          write_tcp_reply(chan, IS_PROTOCOL, IS_PROTOCOL_VER, IS_STATUS, DIS_SUCCESS);

          hierarchy_handler.sendHierarchyToANode(node);
          ret = DIS_SUCCESS;
          }
        else
          write_tcp_reply(chan,IS_PROTOCOL,IS_PROTOCOL_VER,IS_STATUS,ret);
        }

      if (ret != DIS_SUCCESS)
        {
        if (LOGLEVEL >= 1)
          {
          snprintf(log_buf, LOCAL_LOG_BUF_SIZE,
              "IS_STATUS error %d on node %s", ret, node_name.c_str());

          log_err(ret, __func__, log_buf);
          }

        goto err;
        }

      break;
      }

    default:

      snprintf(log_buf, LOCAL_LOG_BUF_SIZE,
          "unknown command %d sent from %s",
        command,
        node->nd_name);

      log_err(-1, __func__, log_buf);

      goto err;

      break;
    }  /* END switch (command) */

  /* must be closed because mom opens and closes this connection each time */
  close_conn(chan->sock, FALSE);
  DIS_tcp_cleanup(chan);
  
  return(NULL);

err:

  /* a DIS write error has occurred */

  if (node != NULL)
    {
    if (LOGLEVEL >= 1)
      {
      DBPRT(("%s: error processing node %s\n",
            __func__,
            node->nd_name))
      }

    netaddr_long(args[1], tmp);
    sprintf(msg_buf, "%s:%ld", tmp, args[2]);

    sprintf(log_buf, "%s from %s(%s)",
      dis_emsg[ret],
      node->nd_name,
      msg_buf);
    }
  else
    {
    sprintf(log_buf,"%s occurred when trying to read sock %d",
      dis_emsg[ret],
      chan->sock);
    }
    
  log_err(-1, __func__, log_buf);
    
  close_conn(chan->sock, FALSE);
  DIS_tcp_cleanup(chan);

  return(NULL);
  } /* END svr_is_request */
