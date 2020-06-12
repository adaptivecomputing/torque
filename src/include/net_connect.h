#ifndef _NET_CONNECT_H
#define _NET_CONNECT_H
#include <stdlib.h>
#include <pthread.h> /* pthread_mutex_t */


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
 * Other Include Files Required
 * <sys/types.h>
 */
#define PBS_NET_H
#ifndef PBS_NET_TYPE
typedef unsigned long pbs_net_t;        /* for holding host addresses */
#define PBS_NET_TYPE
#endif

#define PBS_NET_MAXCONNECTIDLE  900
#define PBS_NET_CONN_AUTHENTICATED 1
#define PBS_NET_CONN_FROM_PRIVIL   2
#define PBS_NET_CONN_NOTIMEOUT     4

#define PBS_SOCK_UNIX     1
#define PBS_SOCK_INET     2

/*
** Protocol numbers and versions for PBS communications.
*/

#define RM_PROTOCOL 1 /* resource monitor protocol number */
#define RM_PROTOCOL_VER 2 /* resmon protocol version number */

#define TM_PROTOCOL 5 /* task manager protocol number */
#define TM_PROTOCOL_VER 2 /* task manager protocol version number */

#define IM_PROTOCOL 3 /* inter-manager protocol number */
#define IM_PROTOCOL_VER 2 /* inter-manager protocol version number */

#define IS_PROTOCOL 4 /* inter-server protocol number */
#define IS_PROTOCOL_VER 3 /* inter-server protocol version number */


/*
** Types of Inter Server messages.
*/

/* sync w/PBSServerCmds[] in resmom/mom_main.c and PBSServerCmds2[] in server/node_manager.c */

#define IS_NULL           0
#define IS_CLUSTER_ADDRS  2
#define IS_UPDATE         3
#define IS_STATUS         4
#define IS_GPU_STATUS     5


/* tell pbs_mom the direction of the hello */
#define TO_SERVER         0
#define FROM_SERVER       1


/* return codes for client_to_svr() */

#define PBS_NET_RC_FATAL -1
#define PBS_NET_RC_RETRY -2
#define PBS_NET_RC_UNSET -3

/* trqauthd's unix socket name */
#define TRQAUTHD_SOCK_NAME  "trqauthd-unix"

/* defines for unix sockets and creds */
#ifndef TSOCK_PATH
#define TSOCK_PATH "/tmp/.torque-unix"
#endif

/* End of message delineator for IS protocol */
#define IS_EOL_MESSAGE "END_OF_LINE"

#ifndef SCM_CREDS
#define SCM_CREDS SCM_CREDENTIALS
#endif

#ifndef linux
#  ifndef __NetBSD__
#    define SPC_PEER_UID(c)   ((c)->cr_uid)
#    define SPC_PEER_GID(c)   ((c)->cr_groups[0])
#  else
#    define SPC_PEER_UID(c)   ((c)->sc_uid)
#    define SPC_PEER_GID(c)   ((c)->sc_gid)
#  endif
#else
#  define SPC_PEER_UID(c)   ((c)->uid)
#  define SPC_PEER_GID(c)   ((c)->gid)
#endif

#ifdef __NetBSD__

typedef struct sockcred ucreds;
#else

typedef struct ucred ucreds;
#endif


enum conn_type
  {
  Primary = 0,
  Secondary,
  FromClientASN,
  FromClientDIS,
  ToServerASN,
  ToServerDIS,
  TaskManagerDIS,
  Idle
  };

/* functions available in libnet.a */

#ifdef __cplusplus
extern "C"
{
#endif
int add_conn(int, enum conn_type, pbs_net_t, unsigned int, unsigned int, void *(*func)(void *));
int add_scheduler_conn(int, enum conn_type, pbs_net_t, unsigned int, unsigned int, void *(*func)(void *));
int  client_to_svr(pbs_net_t, unsigned int, int, char *);
void close_conn(int,int);
void clear_conn(int,int);
pbs_net_t get_connectaddr(int,int);
int  get_connecthost(int sock, char *, int);
void set_localhost_name(char *, size_t);
pbs_net_t get_hostaddr(int *, const char *);
int  get_fullhostname(char *, char *, int, char *);
unsigned int  get_svrport(char *, char *, unsigned int);
int  init_network(unsigned int, void *(*readfunc)(void *));
void net_close(int);
int  wait_request(time_t waittime, long *);
void net_add_close_func(int, void(*func)(int));
int get_max_num_descriptors(void);
char * netaddr_pbs_net_t(pbs_net_t);
#ifdef __cplusplus
}
#endif

int socket_to_handle(int sock, int *local_errno);


struct connection
  {
  pbs_net_t cn_addr; /* internet address of client */
  int  cn_handle; /* handle for API, see svr_connect() */
  unsigned int cn_port; /* internet port number of client */
  unsigned short cn_authen; /* authentication flags */
  unsigned short cn_socktype; /* authentication flags */
  enum conn_type cn_active;     /* idle or type if active */
  time_t cn_lasttime;    /* time last active */
  void *(*cn_func)(void *);  /* read function when data rdy */
  void (*cn_oncl)(int);  /* func to call on close */
  pthread_mutex_t *cn_mutex;
  int cn_stay_open; /* Set to TRUE when the connection needs to remain open */
  };

struct netcounter
  {
  time_t  time;
  int     counter;
  };

void netcounter_get(int netrates[]);
#define MAXLISTENERS  3

typedef struct listener_connection
  {
  pbs_net_t address; /* internet address of listener */
  unsigned int port; /* internet port number of listener */
  int sock;           /* socket for this connection */
  int first_time;
  } listener_connection;

#endif /* _NET_CONNECT_H */
