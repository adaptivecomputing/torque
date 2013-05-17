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

#include <pthread.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <stdio.h>
#include <netdb.h> /* struct addrinfo */

#include "net_cache.h"
#include "hash_table.h"
#include "pbs_error.h"

#include <vector>



class addrcache
  {
  public:

  struct addrinfo *addToCache(struct addrinfo *pAddr,const char *host)
    {
    if(pAddr->ai_family != AF_INET) return NULL;
    struct sockaddr_in *pINetAddr = (struct sockaddr_in *)pAddr->ai_addr;
    char key[65];
    sprintf(key,"%d",pINetAddr->sin_addr.s_addr);
    int i;
    if((i = get_value_hash(addrToName,key)) >= 0)
      {
      freeaddrinfo(pAddr);
      return addrs.at(i);
      }
    pthread_mutex_lock(cacheMutex);
    int index = addrs.size();
    /*
    if(pAddr->ai_next != NULL)
      {
      freeaddrinfo(pAddr->ai_next);
      pAddr->ai_next = NULL;
      }
    */
    addrs.push_back(pAddr);
    hosts.push_back(strdup(host));
    add_hash(addrToName,index,key);
    add_hash(nameToAddr,index,(void *)host);
    pthread_mutex_unlock(cacheMutex);
    return pAddr;
    }

  struct addrinfo * getFromCache(in_addr_t addr)
    {
    struct addrinfo *p = NULL;
    pthread_mutex_lock(cacheMutex);
    char key[65];
    sprintf(key,"%d",addr);
    int index = get_value_hash(addrToName,key);
    if(index >= 0) p = addrs.at(index);
    pthread_mutex_unlock(cacheMutex);
    return p;
    }

  struct addrinfo * getFromCache(const char *hostName)
    {
    struct addrinfo *p = NULL;
    pthread_mutex_lock(cacheMutex);
    int index = get_value_hash(nameToAddr,(void *)hostName);
    if(index >= 0) p = addrs.at(index);
    pthread_mutex_unlock(cacheMutex);
    return p;
    }

  char * getHostName(in_addr_t addr)
    {
    char *p = NULL;
    pthread_mutex_lock(cacheMutex);
    char key[65];
    sprintf(key,"%d",addr);
    int index = get_value_hash(addrToName,key);
    if(index >= 0) p = hosts.at(index);
    pthread_mutex_unlock(cacheMutex);
    return p;
    }

  addrcache()
    {
    cacheMutex = (pthread_mutex_t *)calloc(1,sizeof(pthread_mutex_t));
    pthread_mutex_init(cacheMutex,NULL);
    nameToAddr = create_hash(INITIAL_HASH_SIZE);
    addrToName = create_hash(INITIAL_HASH_SIZE);
    }

  ~addrcache()
    {
    pthread_mutex_destroy(cacheMutex);
    free(cacheMutex);
    for(std::vector<struct addrinfo *>::iterator i = addrs.begin();i != addrs.end();i++)
      {
      freeaddrinfo(*i);
      }
    addrs.clear();
    for(std::vector<char *>::iterator i = hosts.begin();i != hosts.end();i++)
      {
      free(*i);
      }
    hosts.clear();
    free_hash(nameToAddr);
    free_hash(addrToName);
    }

private:

    hash_table_t *nameToAddr;
    hash_table_t *addrToName;
    pthread_mutex_t *cacheMutex;
    std::vector<struct addrinfo *> addrs;
    std::vector<char *> hosts;
  };

addrcache cache;

char *get_cached_nameinfo(const struct sockaddr_in  *sai)
  {
  return cache.getHostName(sai->sin_addr.s_addr);
  } /* END get_cached_nameinfo() */

char *get_cached_fullhostname(const char *hostname,const struct sockaddr_in *sai)
  {
  struct addrinfo *pAddrInfo = NULL;

  if(hostname != NULL) pAddrInfo = cache.getFromCache(hostname);
  if((pAddrInfo == NULL) && sai != NULL) pAddrInfo = cache.getFromCache(sai->sin_addr.s_addr);
  if(pAddrInfo == NULL) return NULL;
  return pAddrInfo->ai_canonname;
  } /* END get_cached_fullhostname() */

struct sockaddr_in *get_cached_addrinfo(const char *hostname)
  {
  struct addrinfo *pAddrInfo = NULL;
  
  if(hostname != NULL) pAddrInfo = cache.getFromCache(hostname);
  if(pAddrInfo == NULL) return NULL;
  return (struct sockaddr_in *)pAddrInfo->ai_addr;
  } /* END get_cached_addrinfo() */

struct addrinfo *get_cached_addrinfo_full(const char *hostname)
  {
  struct addrinfo *pAddrInfo = NULL;

  if(hostname != NULL) pAddrInfo = cache.getFromCache(hostname);
  if(pAddrInfo == NULL) return NULL;
  return pAddrInfo;
  } /* END get_cached_addrinfo() */



struct addrinfo * insert_addr_name_info(struct addrinfo *pAddrInfo,const char *host)
  {
  if(pAddrInfo == NULL)
    {
    struct addrinfo hints;

    memset(&hints,0,sizeof(hints));
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_family = AF_INET;
    hints.ai_flags = AI_CANONNAME;

    if(getaddrinfo(host,NULL,&hints,&pAddrInfo) != 0)
      {
      return NULL;
      }
    }
  return cache.addToCache(pAddrInfo,host);
  } /* END insert_addr_name_info() */
