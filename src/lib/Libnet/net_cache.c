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

#include "net_cache.h"
#include "resizable_array.h"
#include "hash_table.h"
#include "pbs_error.h"


typedef struct network_cache
  {
  resizable_array *nc_ra;
  hash_table_t    *nc_namekey;
  hash_table_t    *nc_saikey;
  pthread_mutex_t *nc_mutex;
  } network_cache;



network_cache cache;


void initialize_network_info()
  {
  cache.nc_ra = initialize_resizable_array(INITIAL_HASH_SIZE);

  cache.nc_namekey = create_hash(INITIAL_HASH_SIZE);
  cache.nc_saikey = create_hash(INITIAL_HASH_SIZE);

  cache.nc_mutex = calloc(1, sizeof(pthread_mutex_t));
  pthread_mutex_init(cache.nc_mutex, NULL);
  } /* END initialize_network_info() */




char *get_cached_nameinfo(
    
  struct sockaddr_in  *sai)

  {
  network_info *ni;
  char         *hostname = NULL;
  int           index;
  char          s_addr_key[65];

  pthread_mutex_lock(cache.nc_mutex);

  sprintf (s_addr_key, "%d", sai->sin_addr.s_addr);

  if ((index = get_value_hash(cache.nc_saikey, s_addr_key)) >= 0)
    {
    ni = (network_info *)cache.nc_ra->slots[index].item;

    if (ni != NULL)
      hostname = ni->hostname;
    }


  pthread_mutex_unlock(cache.nc_mutex);

  return(hostname);
  } /* END get_cached_nameinfo() */




char *get_cached_fullhostname(

  char               *hostname,
  struct sockaddr_in *sai)

  {
  network_info *ni;
  int           index = -1;
  char         *fullname = NULL;
  char          s_addr_key[65];

  if (cache.nc_mutex == NULL)
    return(NULL);

  pthread_mutex_lock(cache.nc_mutex);

  if (hostname != NULL)
    index = get_value_hash(cache.nc_namekey, hostname);

  if ((index == -1) &&
      (sai != NULL))
    {
    sprintf (s_addr_key, "%d", sai->sin_addr.s_addr);
    index = get_value_hash(cache.nc_saikey, s_addr_key);
    }

  if (index >= 0)
    {
    ni = (network_info *)cache.nc_ra->slots[index].item;
    fullname = ni->full_hostname;
    }

  pthread_mutex_unlock(cache.nc_mutex);

  return(fullname);
  } /* END get_cached_fullhostname() */



struct sockaddr_in *get_cached_addrinfo(
    
  char               *hostname)
  
  {
  network_info       *ni;
  int                 index;
  struct sockaddr_in *sai = NULL;

  if (cache.nc_mutex == NULL)
    return(NULL);

  pthread_mutex_lock(cache.nc_mutex);

  if ((index = get_value_hash(cache.nc_namekey, hostname)) >= 0)
    {
    ni = (network_info *)cache.nc_ra->slots[index].item;

    if (ni != NULL)
      {
      sai = &ni->sai;
      }
    }

  pthread_mutex_unlock(cache.nc_mutex);

  return(sai);
  } /* END get_cached_addrinfo() */




network_info *get_network_info_holder(

  char               *hostname,
  char               *full_hostname,
  struct sockaddr_in *sai)

  {
  /* initialize the network info holder */
  network_info *ni = calloc(1, sizeof(network_info));

  ni->hostname = strdup(hostname);

  if (full_hostname != NULL)
    ni->full_hostname = strdup(full_hostname);

  memcpy(&ni->sai, sai, sizeof(struct sockaddr_in));

  return(ni);
  } /* END get_network_info_holder() */




int insert_addr_name_info(
    
  char               *hostname,
  char               *full_hostname,
  struct sockaddr_in *sai)

  {
  int           rc = PBSE_NONE;
  int           index;
  network_info *ni;
  char          s_addr_key[65];

  if (cache.nc_mutex == NULL)
    return(-1);

  pthread_mutex_lock(cache.nc_mutex);

  /* only insert if it isn't already there */
  if (get_value_hash(cache.nc_namekey, hostname) < 0)
    {
    ni = get_network_info_holder(hostname, full_hostname, sai);

    if ((index = insert_thing(cache.nc_ra, ni)) < 0)
      rc = ENOMEM;
    else
      {
      /* store the key in both hash tables so we can look things up either way */
      add_hash(cache.nc_namekey, index, ni->hostname);
      sprintf (s_addr_key, "%d", sai->sin_addr.s_addr);
      add_hash(cache.nc_saikey, index, strdup(s_addr_key));
      }
    }

  pthread_mutex_unlock(cache.nc_mutex);

  return(rc);
  } /* END insert_addr_name_info() */


