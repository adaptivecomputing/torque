/*
 * node_power_state.c
 *
 *  Created on: Apr 4, 2014
 *      Author: Brad Daw
 *
 *      Functions for setting the power state of a MOM node.
 */
#include "license_pbs.h" /* See here for the software license */
#include <stdlib.h>
#include <stdio.h>
#include <arpa/inet.h>
#include "batch_request.h"
#include "pbs_nodes.h"
#include "svrfunc.h"
#include "../lib/Libutils/u_lock_ctl.h" /* unlock_node */
#include "log.h"
#include "threadpool.h"
#include "net_cache.h"

extern char server_name[];


void *send_power_state_to_mom(
    
  void *arg)

  {
  struct batch_request  *pRequest = (struct batch_request *)arg;
  struct pbsnode        *pNode = find_nodebyname(pRequest->rq_host);

  if (pNode == NULL)
    {
    free_br(pRequest);
    return NULL;
    }

  int handle = 0;
  int local_errno = 0;

  handle = svr_connect(pNode->nd_addrs[0],pNode->nd_mom_port,&local_errno,pNode,NULL);
  if (handle < 0)
    {
    unlock_node(pNode, __func__, "Error connecting", LOGLEVEL);
    return NULL;
    }

  unlock_node(pNode, __func__, "Done connecting", LOGLEVEL);
  issue_Drequest(handle, pRequest, true);

  return NULL;
  }

bool getMacAddr(
    
  std::string   &interface,
  unsigned char *mac_addr)

  {
  char buff[1024];

  struct addrinfo *pAddr = get_cached_addrinfo_full(server_name);

  FILE *pPipe = popen("/sbin/ip addr","r");
  if (pPipe == NULL)
    return false;

  char *iface = NULL;
  char *macAddr = NULL;

  while (fgets(buff,sizeof(buff),pPipe) != NULL)
    {
    char *tok = strtok(buff," ");
    if (buff[0] != ' ')
      {
      tok = strtok(NULL," :");
      if (strlen(tok) != 0)
        {
        if (iface != NULL) free(iface);
        iface = strdup(tok);
        }
      }
    else if (!strcmp(tok,"link/ether"))
      {
      tok = strtok(NULL," ");
      if (strlen(tok) != 0)
        {
        if (macAddr != NULL)
          free(macAddr);

        macAddr = strdup(tok);
        }
      }
    else if (!strcmp(tok,"inet"))
      {
      tok = strtok(NULL," ");
      char *iaddr = strdup(tok);
      for (char *ind = iaddr;*ind;ind++)
        {
        if (*ind == '/')
          {
          *ind = '\0';
          break;
          }
        }

      in_addr_t in_addr = inet_addr(iaddr);
      free(iaddr);
      struct addrinfo *pAddrInd = pAddr;
      while ((pAddrInd != NULL)&&(macAddr != NULL)&&(iface != NULL))
        {
        struct in_addr   saddr;
        saddr = ((struct sockaddr_in *)pAddrInd->ai_addr)->sin_addr;
        if (in_addr == saddr.s_addr)
          {
          unsigned int sa[6];
          int cnt = sscanf(macAddr,"%2x:%2x:%2x:%2x:%2x:%2x",
                    &sa[0], &sa[1], &sa[2], &sa[3], &sa[4], &sa[5]);
          free(macAddr);
          macAddr = NULL;

          if (cnt != 6)
            {
            free(iface);
            iface = NULL;
            break;
            }
          for (int i = 0;i < 6;i++)
            {
            *mac_addr++ = (unsigned char)sa[i];
            }

          interface = iface;
          free(iface);
          iface = NULL;
          break;
          }
        pAddrInd = pAddrInd->ai_next;
        }
      }
    else
      {
      if (iface != NULL)
        {
        free(iface);
        iface = NULL;
        }

      if (macAddr != NULL)
        {
        free(macAddr);
        macAddr = NULL;
        }
      }
    }
    
  if (macAddr != NULL)
    {
    free(macAddr);
    macAddr = NULL;
    }

  if (iface != NULL)
    {
    free(iface);
    iface = NULL;
    }

  pclose(pPipe);
  return (interface.length() != 0);
  }



int set_node_power_state(
    
  struct pbsnode **ppNode,
  unsigned short   newState)

  {
  struct pbsnode *pNode = *ppNode;
  if (pNode->nd_addrs == NULL)
    {
    return PBSE_BAD_PARAMETER;
    }

  if (newState == POWER_STATE_RUNNING)
    {
    static std::string interface;
    static unsigned char mac_addr[6];
    if (interface.length() == 0)
      {
      if (!getMacAddr(interface,mac_addr))
        {
        return PBSE_SYSTEM;
        }
      }

    int sock;
    if ((sock = socket(AF_INET,SOCK_PACKET,SOCK_PACKET)) < 0)
      {
      return PBSE_SYSTEM;
      }

    unsigned char outpack[1000];

    memcpy(outpack+6,mac_addr,6);
    memcpy(outpack,pNode->nd_mac_addr,6);
    outpack[12] = 0x08;
    outpack[13] = 0x42;
    int offset = 14;
    memset(outpack + offset,0xff,6);
    offset += 6;

    for (int i = 0;i < 16;i++)
      {
      memcpy(outpack + offset,pNode->nd_mac_addr,6);
      offset += 6;
      }

    int one = 1;
    if (setsockopt(sock, SOL_SOCKET, SO_BROADCAST, (char *)&one, sizeof(one)) < 0)
      {
      close(sock);
      return PBSE_SYSTEM;
      }

    struct sockaddr whereto;
    whereto.sa_family = 0;
    snprintf(whereto.sa_data, sizeof(whereto.sa_data), "%s", interface.c_str());

    if (sendto(sock, outpack, offset, 0, &whereto, sizeof(whereto)) < 0)
      {
      close(sock);
      return PBSE_SYSTEM;
      }

    close(sock);
    return PBSE_NONE;
    }

  if (pNode->nd_job_usages.size() != 0)
    {
    //Can't change the power state on a node with running jobs.
    return PBSE_CANT_CHANGE_POWER_STATE_WITH_JOBS_RUNNING;
    }
  struct batch_request *request = alloc_br(PBS_BATCH_ChangePowerState);
  if (request == NULL)
    {
    return PBSE_SYSTEM;
    }

  request->rq_ind.rq_powerstate = newState;
  pNode->nd_power_state_change_time = time(NULL);

  snprintf(request->rq_host, sizeof(request->rq_host), "%s", pNode->nd_name);
  std::string hostname(request->rq_host);
  int rc = PBSE_NONE;

  {
    int handle = 0;
    int local_errno = 0;
    handle = svr_connect(pNode->nd_addrs[0],pNode->nd_mom_port,&local_errno,pNode,NULL);
    if(handle < 0)
      {
      unlock_node(pNode, __func__, "Error connecting", LOGLEVEL);
      *ppNode = NULL;
      return local_errno;
      }
    unlock_node(pNode, __func__, "Done connecting", LOGLEVEL);
    *ppNode = NULL;
    rc = issue_Drequest(handle, request,true);
    if(rc == PBSE_NONE)
      {
      rc = request->rq_reply.brp_code;
      if(rc < 0) rc = -rc;
      }
  }
  pNode = find_nodebyname(hostname.c_str());
  *ppNode = pNode;
  if ((rc == PBSE_NONE)&&(pNode != NULL))
    {
    pNode->nd_power_state = newState;
    }

  return(rc);
  }





