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

#include <fcntl.h>
#include <stdio.h>
#include <sstream>
#include "mom_hierarchy_handler.h"
#include "pbs_nodes.h"
#include "log.h"
#include "attribute.h"
#include "dis.h"
#include "../lib/Libutils/u_lock_ctl.h"
#include "../lib/Libnet/lib_net.h"
#include "node_manager.h"
#include "threadpool.h"
#include "id_map.hpp"

mom_hierarchy_handler hierarchy_handler; //The global declaration.

mom_hierarchy_t  *mh = NULL; //This evil global is here because the library shared between
                      //server and mom assumes a global with this name.
                      //Good candidate for a refactor.

extern char *path_mom_hierarchy;

class sendNodeHolder
  {
public:
  int                id;
  unsigned short int level;
  unsigned short     port;
  bool               first_send;
  sendNodeHolder(struct pbsnode *node, bool first) : id(node->nd_id), level(node->nd_hierarchy_level),
                                                     port(node->nd_mom_rm_port), first_send(first)
    {
    }
  };



//This method keeps the allnodes array locked until all nodes have been traversed.
// DO NOT use this if you are doing anything with the node that will take an undetermined
// amount of time.
pbsnode *mom_hierarchy_handler::nextNode(all_nodes_iterator **iter)
  {
  if(iter == NULL) return NULL;
  if(*iter == NULL)
    {
    allnodes.lock();
    *iter = allnodes.get_iterator();
    }
  pbsnode *pNode = (*iter)->get_next_item();
  if(pNode == NULL)
    {
    delete *iter;
    *iter = NULL;
    allnodes.unlock();
    }
  else
    lock_node(pNode, __func__, NULL, LOGLEVEL);

  return pNode;
  }



void mom_hierarchy_handler::make_default_hierarchy()

  {
  pbsnode             *pnode;
  std::string         level_ds = "";
  all_nodes_iterator  *iter = NULL;
  char                buf[MAXLINE];
  char                log_buf[LOCAL_LOG_BUF_SIZE];


  hierarchy_xml.clear();

  hierarchy_xml.push_back("<sp>");
  hierarchy_xml.push_back("<sl>");

  if(LOGLEVEL >= 7)
    {
    log_event(PBSEVENT_SYSTEM, PBS_EVENTCLASS_SERVER, __func__, "Creating default hierarchy.");
    }

  while ((pnode = nextNode(&iter)) != NULL)
    {
    if (level_ds.length() > 0)
      level_ds += ",";

    level_ds += pnode->nd_name;

    if (PBS_MANAGER_SERVICE_PORT != pnode->nd_mom_rm_port)
      {
      snprintf(buf, sizeof(buf), ":%d", (int)pnode->nd_mom_rm_port);
      level_ds += buf;
      }

    pnode->nd_hierarchy_level = 0;

    unlock_node(pnode, __func__, NULL, LOGLEVEL);
    }

  if (LOGLEVEL >= 5)
    {
    snprintf(log_buf, sizeof(log_buf),
      "Built default hierarchy %s", level_ds.c_str());
    log_event(PBSEVENT_SYSTEM, PBS_EVENTCLASS_SERVER, __func__, log_buf);
    }


  hierarchy_xml.push_back(level_ds);
  hierarchy_xml.push_back("</sl>");
  hierarchy_xml.push_back("</sp>");
  } /* END make_default_hierarchy() */


/*
 * check_if_in_nodes_file()
 * When parsing the mom_hierarchy file, make sure that the nodes found there
 * are also present in the nodes file, and create the nodes if they don't exist already.
 * Also, mark nodes as having been found in the hierarchy file so that they the hierarchy
 * can be checked for completeness later.
 *
 * @pre-cond: hostname must be a valid char pointer
 * @pre-cond: the nodes file must be parsed before the mom hierarchy
 * @post-cond: any nodes in the hierarchy file that aren't in the nodes file are created
 * @post-cond: rm_port has the mom's rm port stored in it
 */

void mom_hierarchy_handler::check_if_in_nodes_file(
    
  const char     *hostname,
  int             level_index,
  unsigned short &rm_port)

  {
  char                log_buf[LOCAL_LOG_BUF_SIZE];
  struct pbsnode     *pnode;
  char               *colon;
  struct addrinfo    *addr_info;
  struct sockaddr_in *sai;
  unsigned long       ipaddr;
  char               *work_str = strdup(hostname);

  if ((colon = strchr(work_str, ':')) != NULL)
    *colon = '\0';

  if ((pnode = find_nodebyname(work_str)) == NULL)
    {
    snprintf(log_buf, sizeof(log_buf),
      "Node %s found in mom_hierarchy but not found in nodes file. Adding",
      work_str);
    log_err(-1, __func__, log_buf);

    if (pbs_getaddrinfo(work_str, NULL, &addr_info) == 0)
      {
      sai = (struct sockaddr_in *)addr_info->ai_addr;
      ipaddr = ntohl(sai->sin_addr.s_addr);
      }
    else
      {
      log_err(errno, __func__, "getaddrinfo failed");
      free(work_str);
      return;
      }

    create_partial_pbs_node(work_str, ipaddr, ATR_DFLAG_MGRD | ATR_DFLAG_MGWR);
    pnode = find_nodebyname(work_str);
    if (pnode == NULL)
      {
      snprintf(log_buf, sizeof(log_buf),
        "Failed to add node %s to nodes file.",
        work_str);
      log_err(-1, __func__, log_buf);
      free(work_str);
      return;
      }
    }

  free(work_str);

  rm_port = pnode->nd_mom_rm_port;

  pnode->nd_in_hierarchy = TRUE;

  if (pnode->nd_hierarchy_level > level_index)
    pnode->nd_hierarchy_level = level_index;

  unlock_node(pnode, __func__, NULL, LOGLEVEL);

  if (colon != NULL)
    *colon = ':';
  } /* END check_if_in_nodes_file() */


/*
 * convert_level_to_send_format()
 *
 * @pre-cond: nodes must be a valid std::vector of node_comm_t
 * @post-cond: all nodes at this level are added to send format in the format for sending
 */
void mom_hierarchy_handler::convert_level_to_send_format(mom_nodes    &nodes,
                                                                int   level_index)
  {
  node_comm_t       nc;
  std::stringstream level_string;

  hierarchy_xml.push_back("<sl>");

  for (mom_nodes::iterator nodes_iter = nodes.begin(); nodes_iter != nodes.end(); nodes_iter++)
    {
    nc = *nodes_iter;
    unsigned short rm_port = 0;

    if (level_string.str().size() != 0)
      level_string << ",";

    check_if_in_nodes_file(nc.name.c_str(), level_index, rm_port);
    level_string << nc.name;

    if (rm_port != PBS_MANAGER_SERVICE_PORT)
      {
      level_string << ":";
      level_string << rm_port;
      }
    }

  hierarchy_xml.push_back(level_string.str());
  hierarchy_xml.push_back("</sl>");
  } /* END convert_level_to_send_format() */

/*
 * convert_path_to_send_format()
 * iterates over each level in the path and adds in to send format appropriately.
 *
 * @pre-cond: levels must be a valid std::vector of std::vectors.
 * @post-cond: this path is added to send_format in the correct format.
 */

/*
 * convert_path_to_send_format()
 * iterates over each level in the path and adds in to send format appropriately.
 *
 * @pre-cond: levels must be a valid std::vector of std::vectors.
 * @post-cond: this path is added to send_format in the correct format.
 */
void mom_hierarchy_handler::convert_path_to_send_format(mom_levels &levels)
  {
  int level_index = 0;
  hierarchy_xml.push_back("<sp>");

  for (unsigned int i = 0; i < levels.size(); i++)
    convert_level_to_send_format(levels[i], level_index++);

  hierarchy_xml.push_back("</sp>");
  } /* END convert_path_to_send_format() */


/*
 * add_missing_nodes()
 *
 * @pre-cond: nodes have been marked if they are in the hierarchy or not
 * (i.e.: check_if_in_nodes_file() has been called for all nodes in the hierarchy)
 * @post-cond: any nodes not in the hierarchy are added in a new path, all at level 1
 */
void mom_hierarchy_handler::add_missing_nodes(void)

  {
  struct pbsnode *pnode;
  bool            found_missing_node = false;
  all_nodes_iterator *iter = NULL;
  char            log_buf[LOCAL_LOG_BUF_SIZE];
  std::string     level_string = "";

  /* check if there are nodes that weren't in the hierarchy file that are in the nodes file */
  while ((pnode = nextNode(&iter)) != NULL)
    {
    if (pnode->nd_in_hierarchy == FALSE)
      {
      if (found_missing_node == false)
        {
        hierarchy_xml.push_back("<sp>");
        hierarchy_xml.push_back("<sl>");
        found_missing_node = true;
        hierarchy_xml.push_back(pnode->nd_name);
        }
      else
        {
        hierarchy_xml.push_back(",");
        hierarchy_xml.push_back(pnode->nd_name);
        }

      snprintf(log_buf, sizeof(log_buf),
        "Node %s found in the nodes file but not in the mom_hierarchy file. Making it a level 1 node",
        pnode->nd_name);

      pnode->nd_hierarchy_level = 0;
      log_err( -1, __func__, log_buf);
      }
    unlock_node(pnode, __func__, NULL, LOGLEVEL);
    }

  if (found_missing_node == true)
    {
    hierarchy_xml.push_back("</sl>");
    hierarchy_xml.push_back("</sp>");
    }
  }



/*
 * convert_mom_hierarchy_to_send_format()
 * iterates over the mom_hierarchy struct and adds each node to send_format
 * in the format for sending.
 *
 */

void mom_hierarchy_handler::convert_mom_hierarchy_to_send_format(void)
  {

  for (unsigned int i = 0; i < mh->paths.size(); i++)
    convert_path_to_send_format(mh->paths[i]);

  if (hierarchy_xml.size() == 0)
    {
    /* if there's an error, make a default hierarchy */
    make_default_hierarchy();
    }
  else
    {
    add_missing_nodes();
    }
  }


/*
 * loadHierarchy()
 * opens the mom hierarchy file, creates a mom hierarchy, and places it into a format
 * to be sent to the mom nodes.
 * if no hierarchy file exists or if it cannot be parsed, all of the nodes are placed
 * into a default hierarchy with all nodes at level 1.
 *
 * @pre-cond: nodes file has been parsed.
 * @post-cond: send_format is populated so that the hierarchy can be sent.
 */

void mom_hierarchy_handler::loadHierarchy(void)
  {
  char            log_buf[LOCAL_LOG_BUF_SIZE];
  int             fds;

  if(mh != NULL)
    {
    free_mom_hierarchy(mh);
    }
  mh = initialize_mom_hierarchy();

  if(LOGLEVEL >= 7)
    {
    log_event(PBSEVENT_SYSTEM, PBS_EVENTCLASS_SERVER, __func__, "Loading hierarchy.");
    }

  if ((fds = open(path_mom_hierarchy, O_RDONLY, 0)) < 0)
    {
    if (errno == ENOENT)
      {
      /* Each node is a top level node */
      make_default_hierarchy();
      lastReloadTime = time(NULL) + RELOAD_TIME_PADDING; //Just in case the last hierarchy was sent in the same second.
      return;
      }

    snprintf(log_buf, sizeof(log_buf),
      "Unable to open %s", path_mom_hierarchy);
    log_err(errno, __func__, log_buf);
    }
  else
    {
    filePresent = true;
    parse_mom_hierarchy(fds);

    convert_mom_hierarchy_to_send_format();
    }

  if (fds >= 0)
    close(fds);
  lastReloadTime = time(NULL) + RELOAD_TIME_PADDING; //Just in case the last hierarchy was sent in the same second.
  } /* END loadHierarchy() */

//This is called any time a node is dynamically added or deleted.
void mom_hierarchy_handler::reloadHierarchy(void)
  {
  pthread_mutex_lock(&hierarchy_mutex);
  sendOnDemand = false; //Reloading the hierarchy means every MOM needs a new list.
  loadHierarchy();
  if((nextSendTime - time(NULL)) > SEND_DELAY_AFTER_RELOAD)
    {
    nextSendTime = time(NULL) + SEND_DELAY_AFTER_RELOAD;
    }
  pthread_mutex_unlock(&hierarchy_mutex);
  }

//This is called at startup after all the nodes have been read from the nodes file.
void mom_hierarchy_handler::initialLoadHierarchy(void)
  {
  pthread_mutex_lock(&hierarchy_mutex);
  loadHierarchy();

  all_nodes_iterator *iter = NULL;
  struct pbsnode    *pnode;
  while ((pnode = nextNode(&iter)) != NULL)
    {
    if(sendOnDemand)
      {
      pnode->nd_lastHierarchySent = time(NULL); //Pretend the hierarchy was already sent.
      pnode->nd_state &= ~INUSE_NOHIERARCHY;
      }
    else
      {
      pnode->nd_lastHierarchySent = 0;
      pnode->nd_state &= ~INUSE_NOHIERARCHY;
      }
    unlock_node(pnode, __func__, NULL, LOGLEVEL);
    }
  pthread_mutex_unlock(&hierarchy_mutex);
  }

void *sendHierarchyThreadTaskWrapper(void *vp)
  {
  return hierarchy_handler.sendHierarchyThreadTask(vp);
  }



void *mom_hierarchy_handler::sendHierarchyThreadTask(void *vp)
  {
  sendNodeHolder *pNodeHolder = (sendNodeHolder *)vp;
  struct pbsnode *pnode = NULL;
  char            log_buf[LOCAL_LOG_BUF_SIZE+1];
  unsigned short  port;
  const char     *name;

  if (pNodeHolder == NULL)
    {
    log_err(PBSE_BAD_PARAMETER, __func__, "NULL input pointer");
    return(NULL);
    }

  name = node_mapper.get_name(pNodeHolder->id);
  port = pNodeHolder->port;
  time_t timeSent = time(NULL); //Record the time now to avoid a race condition.
    
  if (sendHierarchyToNode(name, port, pNodeHolder->first_send) == PBSE_NONE)
    {
    pnode = find_nodebyname(name);
    if (pnode != NULL)
      {
      pnode->nd_lastHierarchySent = timeSent;
      unlock_node(pnode, __func__, NULL, LOGLEVEL);
      }
      
    if (LOGLEVEL >= 3)
      {
      snprintf(log_buf, sizeof(log_buf),
        "Successfully sent hierarchy to %s", name);
      log_event(PBSEVENT_SYSTEM, PBS_EVENTCLASS_SERVER, __func__, log_buf);
      }
    }

  delete pNodeHolder;

  // Update book-keeping
  pthread_mutex_lock(&hierarchy_mutex);
  if (sendingThreadCount > 0)
    {
    sendingThreadCount--;
    }
  pthread_mutex_unlock(&hierarchy_mutex);

  return(NULL);
  } /* END send_hierarchy_threadtask() */



int mom_hierarchy_handler::sendHierarchyToNode(

  const char     *name,
  unsigned short  port,
  bool            first_time)

  {
  char                log_buf[LOCAL_LOG_BUF_SIZE];
  const char        *string;
  int                 ret = PBSE_NONE;
  int                 sock;
  struct addrinfo    *pAddrInfo = NULL;
  struct sockaddr_in  sa;
  struct tcp_chan    *chan = NULL;

  if (name == NULL)
    return(-1);

  if ((ret = pbs_getaddrinfo(name,NULL,&pAddrInfo)) != PBSE_NONE)
    {
    return ret;
    }
  memcpy(&sa,pAddrInfo->ai_addr,sizeof(sa));

  sa.sin_port = htons(port);

  /* for now we'll only try once as this is going to be tried once each time in the loop */
  sock = tcp_connect_sockaddr((struct sockaddr *)&sa, sizeof(sa));

  if (sock < 0)
    {
    /* could not connect */
    /* - quiting after 5 retries",*/
    if (first_time)
      {
      snprintf(log_buf, LOCAL_LOG_BUF_SIZE,
        "Could not send mom hierarchy to host %s:%d",
        name, port);
      log_err(-1, __func__, log_buf);
      }

    return(-1);
    }

  add_conn(sock, ToServerDIS, ntohl(sa.sin_addr.s_addr), sa.sin_port, PBS_SOCK_INET, NULL);

  if ((chan = DIS_tcp_setup(sock)) == NULL)
    {
    ret = PBSE_MEM_MALLOC;
    }
  /* write the protocol, version and command */
  else if ((ret = is_compose(chan, IS_CLUSTER_ADDRS)) == DIS_SUCCESS)
    {
    std::vector<std::string> tmpHolder;

    pthread_mutex_lock(&hierarchy_mutex);
    tmpHolder = hierarchy_xml;
    pthread_mutex_unlock(&hierarchy_mutex);

    for (unsigned int i = 0; i < tmpHolder.size(); i++)
      {
      string = tmpHolder[i].c_str();
      if ((ret = diswst(chan, string)) != DIS_SUCCESS)
        {
        if (first_time)
          {
          if (ret > 0)
            {
            snprintf(log_buf, LOCAL_LOG_BUF_SIZE,
              "Could not send mom hierarchy to host %s - %s",
              name, dis_emsg[ret]);
            }
          else
            {
            snprintf(log_buf, LOCAL_LOG_BUF_SIZE,
              "Unknown error when sending mom hierarchy to host %s",
              name);
            }

          log_err(-1, __func__, log_buf);
          }

        break;
        }
      }

    ret = diswst(chan, IS_EOL_MESSAGE);

    DIS_tcp_wflush(chan);
    }

  close_conn(sock, FALSE);
  if (chan != NULL)
    {
    DIS_tcp_cleanup(chan);
    }

  return(ret);
  } /* END sendHierarchyToNode() */



void mom_hierarchy_handler::sendHierarchyToANode(
    
  struct pbsnode *node)

  {
  node->nd_lastHierarchySent = 0; //Make sure we send this out.
  sendNodeHolder *pNodeHolder = new sendNodeHolder(node, false);

  pthread_mutex_lock(&hierarchy_mutex);
  sendingThreadCount++;
  pthread_mutex_unlock(&hierarchy_mutex);

  if (enqueue_threadpool_request(sendHierarchyThreadTaskWrapper, (void *)pNodeHolder, task_pool))
    {
    pthread_mutex_lock(&hierarchy_mutex);
    sendingThreadCount--;
    nextSendTime = time(NULL); //We failed somewhere to get ready to send.
    pthread_mutex_unlock(&hierarchy_mutex);
    return; //We failed to create the thread but we have marked the node to send the update out to.
    }
  } // END sendHierarchyToANode()



bool sortFunc(
    
  sendNodeHolder *a,
  sendNodeHolder *b)

  {
  return a->level < b->level;
  }



void mom_hierarchy_handler::checkAndSendHierarchy(
    
  bool first_time)

  {
  time_t now = time(NULL);

  if ((sendOnDemand) || // Don't send if we are only sending on demand.
      ((first_time == false) &&
       ((nextSendTime > now) || // Don't send if its not time to send.
        (sendingThreadCount != 0)))) // Don't send if we are still sending from last time.
    return; 

  std::vector<sendNodeHolder *>  nodesToSend;
  all_nodes_iterator          *iter = NULL;
  struct pbsnode              *pnode;
  bool                         hierarchyNotSentFlagSet = false;

  while ((pnode = nextNode(&iter)) != NULL)
    {
    // On follow up passes, don't try to send the hierarchy to nodes that
    // are down or offlined. Wait for them to report in or be marked online by
    // the admin to send it again.
    if ((pnode->nd_lastHierarchySent < lastReloadTime) &&
        ((first_time) ||
         (((pnode->nd_state & INUSE_DOWN) == 0) &&
          ((pnode->nd_state & INUSE_OFFLINE) == 0))))
      {
      // This new'd object is deleted in sendHierarchyThreadTask
      nodesToSend.push_back(new sendNodeHolder(pnode, first_time));
      }
    else if (pnode->nd_state & INUSE_NOHIERARCHY)
      {
      hierarchyNotSentFlagSet = true;
      }
    unlock_node(pnode, __func__, NULL, LOGLEVEL);
    }

  if (nodesToSend.size() != 0)
    {
    std::sort(nodesToSend.begin(),nodesToSend.end(),sortFunc);
    for (std::vector<sendNodeHolder *>::iterator i = nodesToSend.begin();i != nodesToSend.end();i++)
      {
      pthread_mutex_lock(&hierarchy_mutex);
      sendingThreadCount++;
      pthread_mutex_unlock(&hierarchy_mutex);
      if (enqueue_threadpool_request(sendHierarchyThreadTaskWrapper, (void *)*i, task_pool))
        {
        pthread_mutex_lock(&hierarchy_mutex);
        sendingThreadCount--;
        pthread_mutex_unlock(&hierarchy_mutex);
        }
      }
    }
  else
    {
    if (hierarchyNotSentFlagSet)
      {
      //If we are here then all nodes have an up to date and correct hierarchy so clear all flags.
      while ((pnode = nextNode(&iter)) != NULL)
        {
        if (pnode->nd_state & INUSE_NOHIERARCHY)
          {
          // This was created as a dynamic node and it now has a good ok host list.
          pnode->nd_state &= ~(INUSE_NOHIERARCHY|INUSE_OFFLINE);
          }
        unlock_node(pnode, __func__, NULL, LOGLEVEL);
        }
      }

    pthread_mutex_lock(&hierarchy_mutex);
    nextSendTime = time(NULL) + MOM_CHECK_SEND_INTERVAL;
    pthread_mutex_unlock(&hierarchy_mutex);
    }
  } // END checkAndSendHierarchy()
