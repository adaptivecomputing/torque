#ifndef MOM_HIERARCHY_H
#define MOM_HIERARCHY_H

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



#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include "tcp.h" /* tcp_chan */

#include "resizable_array.h"
#include "dynamic_string.h"


#define INITIAL_SIZE_NETWORK    1
#define NODE_COMM_MAX_ATTEMPTS  10
#define NODE_COMM_RETRY_TIME    90
#define UNREAD_STATUS           -505

#define IS_VALID_STREAM(x) (x >= 0)


/* the basic struct to hold the node communication information */
typedef struct node_comm_t
  {
  time_t              mtime;        /* the last time that this connection was used/updated */
  int                 stream;       /* the stream to communicate over */
  short               bad;          /* indicates an error occurred with this communication */ 
  struct sockaddr_in  sock_addr;    /* information for the socket connection */
  char               *name;         /* the node's hostname */
  } node_comm_t;



/* mom_hierarchy_t holder */
typedef struct mom_hierarchy
  {
  int              current_path;  /* index of the active path */
  int              current_level; /* index of the active level of the active path */
  int              current_node;  /* index of the active node from that path */
  resizable_array *paths;         /* the paths we currently have, a 3D array of resizable arrays */
  } mom_hierarchy_t;



/* struct to hold information on each node that passed a status string */
typedef struct reported_node
  {
  char            hostname[100];
  dynamic_string *status;
  } reported_node_t;



int add_network_entry(mom_hierarchy_t *,char *,struct addrinfo *,unsigned short,int,int);
mom_hierarchy_t *initialize_mom_hierarchy();
node_comm_t *force_path_update(mom_hierarchy_t *);
node_comm_t *update_current_path(mom_hierarchy_t *);
int tcp_connect_sockaddr(struct sockaddr *,size_t);
int write_tcp_reply(struct tcp_chan *chan,int,int,int,int);
int read_tcp_reply(struct tcp_chan *chan,int,int,int,int *);
void free_mom_hierarchy(mom_hierarchy_t *);

#endif /* ndef MOM_HIERARCHY_H */
