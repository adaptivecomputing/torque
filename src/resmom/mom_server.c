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


/*============================================================================
 * mom_server.c
 *============================================================================
 *
 * This file contains code related to mom's servers.  A pbs_mom process
 * receives messages from a pbs_server process and replies back to these
 * messages.  A pbs_mom will ONLY talk to pre-defined instances of pbs_server.
 * These are defined by config files and the in-memory instances of these
 * server definitions are maintained in mom_server structures.
 *
 * In this file, the naming convention for routines will be of the form
 * mom_server_xxx where xxx is the method name and mom_server corresponds
 * to the class name so this is equivalent to mom_server::xxx if this were
 * a C++ program.  This describes functions which operate on a instance
 * of a mom_server.  There will also be aggregate functions that operate
 * on the set of all mom_servers.  These names will be of the form
 * mom_server_all_xxx.
 *
 * Note that there are two senarios being encompassed in this code.  One
 * is the idea of independent pbs_servers controlling and receiving status
 * updates from a compute node.  The other idea is the newer one of having
 * the two or more pbs_servers operating as redundant sets.
 *
 * The practical implication of this becomes apparent when a job finishes
 * and an OBIT message must be sent up to the job's controlling pbs_server.
 * If the servers are independent, then the OBIT must go only to the server
 * that sent the queuejob request.  However, if the servers are a redundant
 * set, then the code sending the OBIT must iterate through the set attempting
 * to sent the OBIT until it succeeds.
 *
 * The mom_server structure contains a reference to a connection.  Connections
 * are basically a socket and event handler pair.  Connections are also used
 * in communication with a scheduler and with sister moms.
 *
 * Here are the senarios pertaining to this module.
 *
 * HELLO sent by pbs_server first
 * ------------------------------
 *
 *   pbs_server             pbs_mom
 *      |                      |
 *      +--- HELLO ----------->|
 *      |                      |
 *      |<----------- HELLO ---+
 *      |                      |
 *      +--- CLUSTER_ADDRS --->|
 *      |                      |
 *
 *
 * HELLO sent by pbs_mom first
 * ---------------------------
 *
 *   pbs_server             pbs_mom
 *      |                      |
 *      |<----------- HELLO ---+
 *      |                      |
 *      +--- CLUSTER_ADDRS --->|
 *      |                      |
 *
 *
 * HELLO sent by both
 * ------------------
 * Both HELLO's are launched at the same time.
 * This is my best guess at how this works.
 * In mom_server_valid_message_source it tries to match the
 * stream number on which the message arrived with a server.
 * If the stream is found, the message is valid.  Otherwise,
 * an attempt is made to match the IP address from the message
 * source with the IP address of some existing server stream.
 * If a match is found, a message is logged about a duplicate
 * stream, the previous stream associated with the server is
 * closed and the new stream replaces it.
 *
 *   pbs_server             pbs_mom
 *      |                      |
 *      +--- HELLO ----------->|
 *      |                      |
 *      |<----------- HELLO ---+
 *      |                      |
 *      +--- CLUSTER_ADDRS --->|
 *      |                      |
 *
 *
 * STATUS sent by pbs_mom
 * ----------------------
 * The pbs_mom has timer and when it fires, a IS_STATUS message
 * is sent to all servers.  There is no response from the server.
 *
 *   pbs_server             pbs_mom
 *      |                      |
 *      |<---------- STATUS ---+
 *      |                      |
 *
 *
 * UPDATE sent by pbs_mom
 * ----------------------
 * Each server has a flag, ReportMomState, that is set whenever
 * the state changes.  Every time the main loop cycles, all servers
 * are checked and if the flag is set, an IS_UPDATE message is
 * sent to the server. There is no response from the server.
 * The state is also sent periodically in the STATUS message
 * and so when this message is sent, the ReportMomState flag is
 * cleared.
 *
 *   pbs_server             pbs_mom
 *      |                      |
 *      |<---------- UPDATE ---+
 *      |                      |
 *
 *
 *----------------------------------------------------------------------------
 */



#include <pbs_config.h>   /* the master config generated by configure */

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>
#include <time.h>
#include <limits.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/param.h>
#include <netinet/in.h>
#include <sys/time.h>
#include <sstream>
#if defined(NTOHL_NEEDS_ARPA_INET_H) && defined(HAVE_ARPA_INET_H)
#include <arpa/inet.h>
#endif

#include "pbs_ifl.h"
#include "pbs_error.h"
#include "log.h"
#include "../lib/Liblog/pbs_log.h"
#include "../lib/Liblog/log_event.h"
#include "net_connect.h"
#include "dis.h"
#include "dis_init.h"
#include "list_link.h"
#include "attribute.h"
#include "pbs_nodes.h"
#include "resmon.h"
#include "server_limits.h"
#include "pbs_job.h"
#include "utils.h"
#include "u_tree.h"
#include "mom_hierarchy.h"
#include "mom_server.h"
#include "mom_comm.h"
#include "mcom.h"
#include "pbs_constants.h" /* Long */
#include "mom_server_lib.h"
#include "lib_ifl.h" /* pbs_disconnect_socket */
#include "alps_functions.h"
#include "../lib/Libnet/lib_net.h" /* netaddr */
#include "net_cache.h"
#include "mom_config.h"
#include "mom_func.h"
#include <string>
#include <vector>
#include "container.hpp"
#include <arpa/inet.h>
#include <boost/tokenizer.hpp>
#ifdef PENABLE_LINUX_CGROUPS
#include "machine.hpp"
#endif
#ifdef USE_RESOURCE_PLUGIN
#include "json/json.h"
#include "trq_plugin_api.h"
#include "plugin_internal.h"
#endif

#define MAX_RETRY_TIME_IN_SECS           (5 * 60)
#define STARTING_RETRY_INTERVAL_IN_SECS   2
#define UPDATE_TO_SERVER                  0
#define UPDATE_TO_MOM                     1
#define MIN_SERVER_UDPATE_SPACING         3
#define MAX_SERVER_UPDATE_SPACING         40
#define NO_SERVER_CONFIGURED             -1
#define COULD_NOT_CONTACT_SERVER         -2

#ifdef NUMA_SUPPORT
extern int numa_index;
extern int num_node_boards;
extern void collect_cpuact(void);
#endif /* NUMA_SUPPORT */

mom_server     mom_servers[PBS_MAXSERVER];
int            mom_server_count = 0;
pbs_net_t      down_svraddrs[PBS_MAXSERVER];

#ifdef PENABLE_LINUX_CGROUPS
extern Machine this_node;
#endif

extern uint32_t            global_mic_count;
extern unsigned int        default_server_port;
extern char               *path_jobs;
extern char               *path_home;
extern  char              *path_spool;
extern unsigned int        pbs_mom_port;
extern unsigned int        pbs_rm_port;
extern unsigned int        pbs_tm_port;
extern int                 internal_state;
char                       TORQUE_JData[MMAX_LINE];
extern int                 received_hello_count[];
extern char                TMOMRejectConn[];
extern time_t              LastServerUpdateTime;
extern bool                ForceServerUpdate;
extern long                system_ncpus;
extern int                 alarm_time; /* time before alarm */
extern time_t              time_now;
extern int                 verbositylevel;
extern AvlTree             okclients;
extern tlist_head          mom_polljobs;
extern char                mom_alias[];
extern int                 updates_waiting_to_send;
time_t                     LastUpdateAttempt;
extern bool                received_cluster_addrs;
extern time_t              requested_cluster_addrs;
extern time_t              first_update_time;
extern int                 UpdateFailCount;
extern mom_hierarchy_t    *mh;
extern char               *stat_string_aggregate;
extern unsigned int        ssa_index;
extern u_long              localaddr;
extern container::item_container<received_node *> received_statuses;
std::vector<std::string>   global_gpu_status;
std::vector<std::string>   mom_status;


extern struct config *rm_search(struct config *where, const char *what);

extern struct rm_attribute *momgetattr(char *str);
extern char *conf_res(char *resline, struct rm_attribute *attr);
extern void send_update_soon();

#ifdef NVIDIA_GPUS
extern int  use_nvidia_gpu;
#endif

#ifdef MIC
int check_for_mics(uint32_t& num_engines);
#endif

int num_stat_update_failures = 0;

void check_state(int);
void state_to_server(int, int);
void node_comm_error(node_comm_t *, const char *);
int  add_mic_status(std::vector<std::string>& status);
int  add_gpu_status(std::vector<std::string>& status);

/* clear servers */
void clear_servers()
  {
  mom_server *pms;
  int         sindex;

  for (sindex = 0; sindex < PBS_MAXSERVER; sindex++)
    {
    pms = &mom_servers[sindex];
    /* the name is what we check in order to know if the server is there */
    memset(pms->pbs_servername, 0, sizeof(pms->pbs_servername));
    }

  mom_server_count = 0;

  } /* END clear_servers() */





/**
 * mom_server_all_init
 *
 * Does the memory intialization for all instances of the mom server
 * structures.  Called from the pbs_mom startup code.
 * @see setup_program_envrionment
 */

void mom_server_all_init(void)

  {
  mom_server_count = 0;
  return;
  }  /* END mom_server_all_init() */






/*--------------------------------------------------------------------
 * mom_server_find_by_name
 *--------------------------------------------------------------------
 * Find an instance of a mom server structure given a server name.
 *
 * @param stream number to find
 * @see mom_server_find_by_ip
 */

mom_server *mom_server_find_by_name(

  const char *name)

  {
  mom_server *pms;
  int sindex;

  for (sindex = 0;sindex < PBS_MAXSERVER;sindex++)
    {
    pms = &mom_servers[sindex];

    if (!strcmp(pms->pbs_servername, name))
      {
      return(pms);
      }
    }

  return(NULL);
  }






/*--------------------------------------------------------------------
 * mom_server_find_by_ip
 *--------------------------------------------------------------------
 * Find an instance of a mom server structure given an ip address.
 *
 * @param ipaddr IP address to find
 */

mom_server *mom_server_find_by_ip(

  u_long search_ipaddr)

  {
  mom_server         *pms;
  int                 sindex;

  u_long              ipaddr;

  for (sindex = 0;sindex < PBS_MAXSERVER;sindex++)
    {
    pms = &mom_servers[sindex];

    ipaddr = ntohl(pms->sock_addr.sin_addr.s_addr);
    
    if (ipaddr == search_ipaddr)
      {
      return(pms);
      }
    }  /* END for (sindex) */

  /* FAILURE */

  return(NULL);
  }  /* END mom_server_find_by_ip() */






/*--------------------------------------------------------------------
 * mom_server_find_empty_slot
 *--------------------------------------------------------------------
 * Find a free instance of a mom server structure.
 *
 */

mom_server *mom_server_find_empty_slot(void)

  {
  mom_server *pms;
  int sindex;

  for (sindex = 0;sindex < PBS_MAXSERVER;sindex++)
    {
    pms = &mom_servers[sindex];

    if (pms->pbs_servername[0] == 0)
      {
      return(pms);
      }
    }

  return(NULL);
  }





/**
 * mom_server_add
 *
 * Allocate a mom server structure from the global array.
 *
 * @param value name of the new server to be added
 * @see setpbsservername
 */

int mom_server_add(

  const char *value)

  {
  mom_server      *pms;
  struct addrinfo *addr_info;
  char             tmp_server_name[PBS_MAXSERVERNAME];
  unsigned short   port;
  char            *colon;

  if ((pms = mom_server_find_by_name(value)))
    {
    /* This server name has already been added. */
    sprintf(log_buffer, "server host %s already added", value);

    log_record(PBSEVENT_SYSTEM, PBS_EVENTCLASS_SERVER, __func__, log_buffer);
    }
  else if ((pms = mom_server_find_empty_slot()) != NULL)
    {
    /* Fill in the new server instance */
    snprintf(tmp_server_name, sizeof(tmp_server_name), "%s", value);

    colon = strchr(tmp_server_name,':');
    if (colon != NULL)
      {
      *colon = '\0';
      port = (short)atoi(colon+1);
      }
    else
      port = default_server_port;

    if (pbs_getaddrinfo(tmp_server_name, NULL,&addr_info) != 0)
      {
      sprintf(log_buffer, "Cannot resolve host %s for pbs_server", tmp_server_name);
      log_err(PBSE_BADHOST, __func__, log_buffer);

      return(0); /* FAILURE */
      }

    /* copy the server name and set up the sock address */
    snprintf(pms->pbs_servername, sizeof(pms->pbs_servername), "%s", tmp_server_name);

    pms->sock_addr.sin_addr = ((struct sockaddr_in *)addr_info->ai_addr)->sin_addr;
    pms->sock_addr.sin_family = AF_INET;
    pms->sock_addr.sin_port = htons(port);

    mom_server_count++;

    sprintf(log_buffer, "server %s added", pms->pbs_servername);
    log_record(PBSEVENT_SYSTEM, PBS_EVENTCLASS_SERVER, __func__, log_buffer);
    }
  else
    {
    sprintf(log_buffer, "server table overflow (max=%d) - server host %s not added",
      PBS_MAXSERVER,
      value);

    log_err(-1, __func__, log_buffer);

    return(0); /* FAILURE */
    }

  /* Leaving this out breaks things but seems bad because if getaddrinfo fails,
   * there is no retry except for the old way reinserting the name over and over again.
   * And what happens if the server connect via a different interface than the
   * one that getaddrinfo returns?  It really seems better to not deal with
   * the IP address here but rather do what needs to be done when a connection
   * is established.  Anyway, this should fix things for now.
   */

    {
    struct in_addr  saddr;
    u_long          ipaddr;

    /* FIXME: must be able to retry failed lookups later */

    if (!overwrite_cache(pms->pbs_servername, &addr_info))
      {
      sprintf(log_buffer, "host %s not found", pms->pbs_servername);

      log_err(-1, __func__, log_buffer);
      }
    else
      {
      saddr = ((struct sockaddr_in *)addr_info->ai_addr)->sin_addr;

      ipaddr = ntohl(saddr.s_addr);

      if (ipaddr != 0)
        {
        okclients = AVL_insert(ipaddr, 0, NULL, okclients);
        }

      }
    }    /* END BLOCK */

  return(1);      /* SUCCESS */
  }  /* END mom_server_add() */





void mom_server_stream_error(

  int         stream,
  const char *name,
  const char *id,
  const char *message)

  {
  sprintf(log_buffer, "error %s to server %s", message, name);

  log_record(PBSEVENT_SYSTEM, 0, id, log_buffer);

  return;
  }  /* END mom_server_stream_error() */






/**
 * mom_server_flush_io
 *
 * A wrapper function for the library that adds logging and error
 * handling specific to the mom_server module.
 *
 * @param pms pointer to mom_server instance
 */

int mom_server_flush_io(

  struct tcp_chan *chan,
  const char      *id,
  const char      *message)

  {
  if (DIS_tcp_wflush(chan) == -1)
    {
    log_err(errno, id, message);

    close(chan->sock);

    return(DIS_PROTO);
    }

  return(DIS_SUCCESS);
  } /* END mom_server_flush_io() */





/**
 * Create an inter-server message to send to pbs_server (i.e., 'send status update')
 *
 * @see state_to_server() - parent - create state IS_UPDATE message
 * @see is_update_stat() - parent - create full IS_UPDATE message
 * @see mom_is_request() - peer - process hello/cluster_addrs requests from pbs_server
 */

int is_compose(

  struct tcp_chan *chan,
  const char      *server_name,
  int              command)

  {
  int ret;

  if (chan->sock < 0)
    {
    return(DIS_EOF);
    }

  if ((ret = diswsi(chan, IS_PROTOCOL)) != DIS_SUCCESS)
    {
    mom_server_stream_error(chan->sock, server_name, __func__, "writing protocol");

    return(ret);
    }
  else if ((ret = diswsi(chan, IS_PROTOCOL_VER)) != DIS_SUCCESS)
    {
    mom_server_stream_error(chan->sock, server_name, __func__, "writing protocol version");

    return(ret);
    }
  else if ((ret = diswsi(chan, command)) != DIS_SUCCESS)
    {
    mom_server_stream_error(chan->sock, server_name, __func__, "writing command");

    return(ret);
    }

  return(DIS_SUCCESS);
  }  /* END is_compose() */



#ifdef PENABLE_LINUX_CGROUPS
void gen_layout(

  const char               *name,
  std::vector<std::string> &status)

  {
  std::stringstream layout;
  layout << name << "=";
  this_node.displayAsJson(layout, false);
  status.push_back(layout.str());
  } // END gen_layout()
#endif



/**
 * gen_size
 *
 * For the size attribute to be returned, it must be
 * defined in the pbs_mom config file.  The syntax
 * is unique in that you must ask for the size of
 * either a file or a file system.
 *
 * For example:
 * size[fs=/]
 * size[file=/home/user/test.txt]
 */

void gen_size(

  const char               *name,
  std::vector<std::string> &status)

  {
  struct config       *ap;

  struct rm_attribute *attr;
  const char          *value;

  ap = rm_search(config_array, name);

  if (ap)
    {
    attr = momgetattr(ap->c_u.c_value);

    if (attr)
      {
      value = dependent(name, attr);

      if (value && *value)
        {
        std::string s(name);
        s += "=";
        s += value;
        status.push_back(s);
        }
      }
    }

  return;
  }



void gen_arch(

  const char               *name,
  std::vector<std::string> &status)

  {
  struct config  *ap;

  ap = rm_search(config_array,name);

  if (ap != NULL)
    {
    std::string s(name);
    s += "=";
    s += ap->c_u.c_value;
    status.push_back(s);
    }

  return;
  }



void gen_opsys(

  const char               *name,
  std::vector<std::string> &status)

  {
  struct config  *ap;

  ap = rm_search(config_array,name);

  if (ap != NULL)
    {
    std::string s(name);
    s += "=";
    s += ap->c_u.c_value;
    status.push_back(s);
    }

  return;
  }



void gen_jdata(

  const char               *name,
  std::vector<std::string> &status)

  {
  if (TORQUE_JData[0] != '\0')
    {
    std::string s(name);
    s += "=";
    s += TORQUE_JData;
    status.push_back(s);
    }

  }

void gen_gres(

  const char  *name,
  std::vector<std::string> &status)

  {
  const char *value;

  value = reqgres(NULL);

  if (value != NULL)
    {
    std::string s(name);
    s += "=";
    s += value;
    status.push_back(s);
    }

  return;
  }    /* END gen_gres() */

void gen_gen(

  const char  *name,
  std::vector<std::string> &status)

  {
  struct config *ap;
  const char    *value;
  char          *ptr;

  ap = rm_search(config_array,name);

  if (ap != NULL)
    {
    ptr = conf_res(ap->c_u.c_value, NULL);

    if (ptr && *ptr)
      {
      std::string s(name);
      s += "=";
      s += ptr;
      status.push_back(s);
      }
    }
  else
    {
    value = dependent(name, NULL);

    if (value == NULL)
      {
      /* value not set (attribute required) */
      std::string s(name);
      s += "=";
      s += rm_errno;
      status.push_back(s);
      }
    else if (value[0] == '\0')
      {
      /* value not set (attribute optional) */
      }
    else
      {
      std::string s(name);
      s += "=";
      s += value;
      status.push_back(s);
      }
    } /* else if (ap) */

  return;
  }   /* END gen_gen() */



void gen_macaddr(

  const char               *name,
  std::vector<std::string> &status)

  {
  static std::string mac_addr;

  if (mac_addr.length() == 0)
    {
    char             buff[500];
    struct addrinfo *pAddr = NULL;
    struct addrinfo  hints;

    memset(&hints, 0, sizeof(hints));
    // IPv4
    hints.ai_family = AF_INET;
    if (getaddrinfo(mom_host, NULL, &hints, &pAddr) != 0)
      {
      return;
      }

    FILE *pPipe = popen("/sbin/ip addr","r");
    if (pPipe == NULL)
      {
      freeaddrinfo(pAddr);
      return;
      }

    char *macAddr = NULL;
    while (fgets(buff,sizeof(buff),pPipe) != NULL)
      {
      char *tok = strtok(buff," ");
      if (!strcmp(tok,"link/ether"))
        {
        tok = strtok(NULL," ");
        if (strlen(tok) != 0)
          {
          if (macAddr != NULL) free(macAddr);
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
        while ((pAddrInd != NULL)&&(macAddr != NULL))
          {
          struct in_addr   saddr;
          saddr = ((struct sockaddr_in *)pAddrInd->ai_addr)->sin_addr;
          if(in_addr == saddr.s_addr)
            {
            mac_addr = macAddr;
            free(macAddr);
            macAddr = NULL;
            break;
            }
          pAddrInd = pAddrInd->ai_next;
          }
        }
      else
        {
        if(macAddr != NULL)
          {
          free(macAddr);
          macAddr = NULL;
          }
        }
      }
    pclose(pPipe);
    if(macAddr != NULL)
      {
      free(macAddr);
      }
    freeaddrinfo(pAddr);
    }

  if(mac_addr.length()  == 0)
    {
    return;
    }

  std::string s(name);
  s += "=";
  s += mac_addr;
  status.push_back(s);
  } // END gen_macaddr()



typedef void (*gen_func_ptr)(const char *, std::vector<std::string> &);

typedef struct stat_record
  {
  const char *name;
  gen_func_ptr func;
  } stat_record;

stat_record stats[] = {
  {"arch",        gen_arch},
  {"opsys",       gen_gen},
  {"uname",       gen_gen},
  {"sessions",    gen_gen},
  {"nsessions",   gen_gen},
  {"nusers",      gen_gen},
  {"idletime",    gen_gen},
  {"totmem",      gen_gen},
  {"availmem",    gen_gen},
  {"physmem",     gen_gen},
  {"ncpus",       gen_gen},
  {"loadave",     gen_gen},
  {"message",     gen_gen},
  {"gres",        gen_gres},
  {"netload",     gen_gen},
  {"size",        gen_size},
  {"state",       gen_gen},
  {"jobs",        gen_gen},
  {"jobdata",     gen_jdata},
  {"varattr",     gen_gen},
  {"cpuclock",    gen_gen},
  {"macaddr",     gen_macaddr},
#ifdef PENABLE_LINUX_CGROUPS
  {"layout",      gen_layout},
#endif
  {NULL,          NULL}
  };



/*
 * add_custom_node_resources()
 *
 * Adds the custom things people want to report. This is the interaction point for the
 * resource plugin for node resource piece
 */

void add_custom_node_resources()

  {
#ifdef USE_RESOURCE_PLUGIN
  static const int                           node_resource_alarm_seconds = 5;
  static std::map<std::string, std::string>  varattrs;
  static std::map<std::string, unsigned int> greses;
  static std::map<std::string, double>       gmetrics;
  static std::set<std::string>               features;

  if (LOGLEVEL >= 3)
    log_event(PBSEVENT_SYSTEM, PBS_EVENTCLASS_MOM, __func__, "Starting the node resource plugin");

  // Set an alarm so the plug-in can't bring the mom to its knees
  alarm(node_resource_alarm_seconds);
  report_node_generic_resources(greses);
  report_node_generic_metrics(gmetrics);
  report_node_varattrs(varattrs);
  report_node_features(features);
  alarm(0);

  if ((varattrs.size() > 0) ||
      (greses.size() > 0) ||
      (gmetrics.size() > 0) ||
      (features.size() > 0))
    {
    Json::Value plugin_info;
    std::string status_entry(PLUGIN_EQUALS);

    if (greses.size() > 0)
      {
      Json::Value gres_values;

      for (std::map<std::string, unsigned int>::iterator it = greses.begin();
           it != greses.end();
           it++)
        gres_values[it->first] = it->second;

      plugin_info[GRES] = gres_values;
      }

    if (varattrs.size() > 0)
      {
      Json::Value varattr_info;

      for (std::map<std::string, std::string>::iterator it = varattrs.begin();
           it != varattrs.end();
           it++)
        varattr_info[it->first] = it->second;

      plugin_info[VARATTRS] = varattr_info;
      }

    if (gmetrics.size() > 0)
      {
      Json::Value gmetric_info;

      for (std::map<std::string, double>::iterator it = gmetrics.begin();
           it != gmetrics.end();
           it++)
        gmetric_info[it->first] = it->second;

      plugin_info[GMETRICS] = gmetric_info;
      }

    if (features.size() > 0)
      {
      std::string feature_list;

      for (std::set<std::string>::iterator it = features.begin(); it != features.end(); it++)
        {
        if (feature_list.size() > 0)
          feature_list += ",";

        feature_list += *it;
        }

      plugin_info[FEATURES] = feature_list;
      }

    status_entry += plugin_info.toStyledString();
    mom_status.push_back(status_entry);
    }

  if (LOGLEVEL >= 3)
    log_event(PBSEVENT_SYSTEM, PBS_EVENTCLASS_MOM, __func__, "Finished the node resource plugin");
#endif

  } // END add_custom_node_resources()



/**
 * generate_server_status
 *
 *  This should update the PBS server with the status information
 *  that the resource manager should need.  This should allow for
 *  less trouble on the part of the resource manager.  It can get
 *  this information from the server rather than going to each mom.
 *
 *  This was originally part of is_update_stat, a very complicated
 * routine.  I have broken this into pieces so that the special cases
 * a each in a specific routine.  The routine gen_gen is the one
 * for the general case.
 *
 *  The old is_update_stat used to write directly to a DIS stream.
 * Now we generate the strings in to a buffer, each string terminated
 * with a NULL and a double NULL at the end.
 *
 * Warning: Because of the complexity of the old is_update_stat, it
 * was very hard to break out the special cases.  I actually had to
 * go back and look at older code before there were multiple server
 * arrays to try and figure out what should be happening.
 *
 * If there is some trouble with some status getting back to the
 * pbs_server, this is the place to look.
 */

void generate_server_status(
    
  std::vector<std::string> &status)

  {
  int   i;
  std::stringstream ss;

#ifdef NUMA_SUPPORT
  /* identify which vnode this is */
  ss << NUMA_KEYWORD;
  ss << numa_index;
  status.push_back(ss.str());
  ss.str("");
#endif /* NUMA_SUPPORT */

  for (i = 0;stats[i].name != NULL;i++)
    {
    alarm(alarm_time);

    if (stats[i].func)
      {
      (stats[i].func)(stats[i].name, status);
      }

    alarm(0);
    }  /* END for (i) */

  ss << "version=" << PACKAGE_VERSION;
  status.push_back(ss.str());

  add_custom_node_resources();

  TORQUE_JData[0] = '\0';
  }  /* END generate_server_status */



int should_request_cluster_addrs()

  {
  int should = FALSE;
  time_now = time(NULL);

  if (received_cluster_addrs == false)
    {
    if (time_now - requested_cluster_addrs > DEFAULT_SERVER_STAT_UPDATES)
      {
      should = TRUE;
      }
    }

  return(should);
  } /* END should_request_cluster_addrs() */



/* 
 * writes the header for a server status update
 *
 *  Header format
 *
 *   Protocol | Version | Command (IS_STATUS) | mom service port | mom manager port 
 *   The following two lines are added to the header if cgroups are enabled.
 *   | available sockets | available numa_nodes | available cores | available threads
 *   | total sockets     | total numa_nodes     | total cores     | total threads
 */
int write_update_header(
    
  struct tcp_chan *chan,
  const char *id,
  const char *name)

  {
  int  ret;
  char buf[MAXLINE];
  
  if ((ret = is_compose(chan, name, IS_STATUS)) == DIS_SUCCESS)
    {
    if ((ret = diswus(chan, pbs_mom_port)) == DIS_SUCCESS)
      {
      if ((ret = diswus(chan, pbs_rm_port)) == DIS_SUCCESS)
        {
        if (is_reporter_mom == FALSE)
          {
          /* write this node's name first - alps handles this separately */
          snprintf(buf,sizeof(buf),"node=%s",mom_alias);
          
          if ((ret = diswst(chan, buf)) != DIS_SUCCESS)
            mom_server_stream_error(chan->sock, name, id, "writing status string");
          else if (should_request_cluster_addrs() == TRUE)
            {
            if ((ret = diswst(chan, "first_update=true")) != DIS_SUCCESS)
              mom_server_stream_error(chan->sock, name, id, "writing status string");
            else
              requested_cluster_addrs = time_now;
            }
          }
        }
      }
    }

  return(ret);
  } /* END write_update_header() */



int write_my_server_status(
 
  struct tcp_chan          *chan,
  const char               *id,
  std::vector<std::string> &strings,
  void                     *dest,
  int                       mode)
 
  {
  int          ret = DIS_SUCCESS;

  mom_server  *pms;
  node_comm_t *nc;
 
  /* put each string into the message. */
  for (unsigned int i = 0; i < strings.size(); i++)
    {
    const char *str_to_write = strings[i].c_str();

    if (LOGLEVEL >= 7)
      {
      sprintf(log_buffer,"%s: sending to server \"%s\", i = %u size = %d",
        id,
        str_to_write, i, (int)strings.size());
      
      log_record(PBSEVENT_SYSTEM,0,id,log_buffer);
      }
    
    if ((ret = diswst(chan, str_to_write)) != DIS_SUCCESS)
      {
      switch (mode)
        {
        case UPDATE_TO_SERVER:
          
          pms = (mom_server *)dest;
          
          mom_server_stream_error(chan->sock, pms->pbs_servername, id, "writing status string");
          
          break;
          
        case UPDATE_TO_MOM:
          
          nc = (node_comm_t *)dest;
          nc->stream = chan->sock;
          
          node_comm_error(nc, "Error writing strings to");
          
          break;
        } /* END switch (mode) */
      
      break;
      }
    }

  return(ret);
  } /* END write_my_server_status() */





int write_cached_statuses(
 
  struct tcp_chan *chan,
  const char      *id,
  void            *dest,
  int              mode)
 
  {
  int            ret = DIS_SUCCESS;
  received_statuses.lock();
  container::item_container<received_node *>::item_iterator *iter = received_statuses.get_iterator();
  const char   *cp;
  received_node *rn;
  mom_server    *pms;
  node_comm_t   *nc;
  bool           error = false;
  
  /* traverse the received_nodes array and send/clear the updates */
  while (((rn = iter->get_next_item()) != NULL) &&
         (error == false))
    {
    for (unsigned int i = 0; i < rn->statuses.size(); i++)
      {
      cp = rn->statuses[i].c_str();
      if (LOGLEVEL >= 7)
        {
        sprintf(log_buffer,"%s: sending to server \"%s\"",
          id,
          cp);
        
        log_record(PBSEVENT_SYSTEM,0,id,log_buffer);
        }
      
      if ((ret = diswst(chan,cp)) != DIS_SUCCESS)
        {
        error = true;

        /* FAILURE */
        switch (mode)
          {
          case UPDATE_TO_SERVER:
            
            pms = (mom_server *)dest;
            
            mom_server_stream_error(chan->sock, pms->pbs_servername, id, "writing status string");
            
            break;
            
          case UPDATE_TO_MOM:
            
            nc = (node_comm_t *)dest;
            nc->stream = chan->sock;
            
            node_comm_error(nc,"Error writing strings to");
            
            break;
          } /* END switch (mode) */
        
        break;
        }
      
      } /* END write each string */
    
    rn->statuses.clear();
    } /* END iterate over received statuses */

  delete iter;

  received_statuses.unlock();
  return(ret);
  } /* END write_cached_statuses() */





/**
 * mom_server_update_stat
 *
 * Send a status update message to a server.
 *
 * NOTE:  if interface is bad, try to recover is ???
 *
 * @param mom_server_all_update_stat() - parent
 * @param pms pointer to mom_server instance
 */
 
int mom_server_update_stat(
 
  mom_server               *pms,
  std::vector<std::string> &strings)
 
  {
  int              stream;
  int              ret = -1;
  int              rc  = COULD_NOT_CONTACT_SERVER;
  struct tcp_chan *chan = NULL;

  if ((pms->pbs_servername[0] == '\0') ||
      (time_now < (pms->MOMLastSendToServerTime + get_stat_update_interval())))
    {
    /* No server is defined for this slot */
    
    return(NO_SERVER_CONFIGURED);
    }

  stream = tcp_connect_sockaddr((struct sockaddr *)&pms->sock_addr, sizeof(pms->sock_addr), false);
 
  if (IS_VALID_STREAM(stream))
    {
    if ((chan = DIS_tcp_setup(stream)) == NULL)
      {
      }
    else if ((ret = write_update_header(chan, __func__, pms->pbs_servername)) != DIS_SUCCESS)
      {
      }
    else if ((ret = write_my_server_status(chan, __func__, strings, pms, UPDATE_TO_SERVER)) != DIS_SUCCESS)
      {
      }
    else if ((ret = write_cached_statuses(chan, __func__, pms, UPDATE_TO_SERVER)) != DIS_SUCCESS)
      {
      }
    else if ((ret = diswst(chan, IS_EOL_MESSAGE)) != DIS_SUCCESS)
      {
      }
    else if ((ret = DIS_tcp_wflush(chan)) != DIS_SUCCESS)
      {
      }
    else
      {
      read_tcp_reply(chan, IS_PROTOCOL, IS_PROTOCOL_VER, IS_STATUS, &ret);
      }

    if (chan != NULL)
      DIS_tcp_cleanup(chan);
      
    close(stream);
  
    if (ret != DIS_SUCCESS)
      {

      /* FAILURE */
      if (ret == UNREAD_STATUS)
        {
        snprintf(log_buffer,sizeof(log_buffer),"Couldn't read a reply from the server");
        }
      else
        {
        if (ret >= 0)
          {
          snprintf(log_buffer,sizeof(log_buffer),
            "Couldn't send update to server: %s",
            dis_emsg[ret]);
          }
        else
          {
          snprintf(log_buffer,sizeof(log_buffer), "Couldn't send update to server");
          }
        }
      
      log_err(-1,__func__,log_buffer);
      
      /* force another update to the server so we get this out there */
      UpdateFailCount++;
      }
    else
      {
      /* SUCCESS */
      if (LOGLEVEL >= 3)
        {
        sprintf(log_buffer, "status update successfully sent to %s", pms->pbs_servername);
        
        log_record(PBSEVENT_SYSTEM, 0, __func__, log_buffer);

        }
        
      rc = PBSE_NONE;
      
      /* It would be redundant to send state since it is already in status */  
      pms->ReportMomState = 0;

#ifndef NUMA_SUPPORT      
      pms->MOMLastSendToServerTime = time_now;
#else
      if (numa_index + 1 >= num_node_boards)
        pms->MOMLastSendToServerTime = time_now;
#endif
      ForceServerUpdate = false;
      LastServerUpdateTime = time_now;
      
      UpdateFailCount = 0;
      }
    } /* END if valid stream */
  else
    {
    UpdateFailCount++;
    }
  
  return(rc);
  }  /* END mom_server_update_stat() */





void node_comm_error(
 
  node_comm_t *nc,
  const char *message)
 
  {
  snprintf(log_buffer,sizeof(log_buffer), "%s %s", message, nc->name.c_str());
  log_err(-1, "Node communication process",log_buffer);
  
  close(nc->stream);
  nc->stream = -1;
  nc->bad = TRUE;
  } /* END node_comm_error() */





int write_status_strings(
 
  std::vector<std::string> &strings,
  node_comm_t              *nc)
 
  {
  int            fds = nc->stream;
  int            rc = DIS_SUCCESS;
  struct tcp_chan *chan = NULL;

  if (LOGLEVEL >= 9)
    {
    snprintf(log_buffer, sizeof(log_buffer),
      "Attempting to send status update to mom %s", nc->name.c_str());
    log_record(PBSEVENT_SYSTEM, PBS_EVENTCLASS_SERVER, __func__, log_buffer);
    }
 
  if ((chan = DIS_tcp_setup(fds)) == NULL)
    {
    }
  /* write protocol */
  else if ((rc = write_update_header(chan,__func__,nc->name.c_str())) != DIS_SUCCESS)
    {
    }
  else if ((rc = write_my_server_status(chan,__func__, strings, nc, UPDATE_TO_SERVER)) != DIS_SUCCESS)
    {
    }
  else if ((rc = write_cached_statuses(chan,__func__,nc,UPDATE_TO_SERVER)) != DIS_SUCCESS)
    {
    }
  /* write message that we're done */
  else if ((rc = diswst(chan, IS_EOL_MESSAGE)) != DIS_SUCCESS)
    {
    }
  else if ((rc = DIS_tcp_wflush(chan)) == DIS_SUCCESS)
    {
    if (LOGLEVEL >= 7)
      {
      snprintf(log_buffer, sizeof(log_buffer),
        "Successfully sent status update to mom %s", nc->name.c_str());
      log_record(PBSEVENT_SYSTEM, PBS_EVENTCLASS_SERVER,__func__,log_buffer);
      }
    }

  if (chan != NULL)
    DIS_tcp_cleanup(chan);
  pbs_disconnect_socket(fds);
  nc->stream = -1;
 
  return(rc);
  } /* END write_status_strings() */





/* send_update() 
 *
 * decides whether or not a status update should be sent, including using a 
 * back-off algorithm for continuously failures
 */

int send_update()
  
  {
  int mod_value;

  if (first_update_time > time_now)
    return(FALSE);
  
  if (time_now < (LastServerUpdateTime + get_stat_update_interval()))
    return(FALSE);
  
  long attempt_diff;

  if (UpdateFailCount == 0)
    return(TRUE);

  /* the following conditions are to continually back off if we're experiencing
   * several failures in a row */
  attempt_diff = time_now - LastUpdateAttempt;

  /* never send updates in a rapid-fire fashion */
  if (attempt_diff > MIN_SERVER_UDPATE_SPACING)
    {
    /* cap the longest time between updates */
    if ((LastServerUpdateTime == 0) ||
      (attempt_diff > MAX_SERVER_UPDATE_SPACING))
      return(TRUE);

    /* make sending more likely the longer we've waited */
    mod_value = MAX(2, ServerStatUpdateInterval - attempt_diff);

    if (rand() % mod_value == 0)
      return(TRUE);
    }
  return(FALSE);
  } /* END send_update() */



/*
 * places the gpu status on the end of the other status information
 */

int append_gpu_status(
    
  std::vector<std::string> &source,
  std::vector<std::string> &destination)

  {
  destination.insert(destination.end(), source.begin(), source.end());

  return(PBSE_NONE);
  }



int send_update_to_a_server()

  {
  int rc = NO_SERVER_CONFIGURED;
  char    log_buf[LOCAL_LOG_BUF_SIZE];

  /* now, once we contact one server we stop attempting to report in */
  for (int sindex = 0; sindex < PBS_MAXSERVER && rc != PBSE_NONE; sindex++)
    {
    int tmp_rc = mom_server_update_stat(&mom_servers[sindex], mom_status);

    if (tmp_rc != NO_SERVER_CONFIGURED)
      rc = tmp_rc;
    }

  if (rc == COULD_NOT_CONTACT_SERVER)
    {
    num_stat_update_failures++;
    log_err(-1, __func__, "Could not contact any of the servers to send an update");
    sprintf(log_buf, "Status not successfully updated for %d MOM status update intervals", num_stat_update_failures);
    log_err(-1, __func__, log_buf);
    }
  else if (num_stat_update_failures != 0)
    {
    sprintf(log_buf, "Status update successfully sent after %d MOM status update intervals", num_stat_update_failures);
    log_err(-1, __func__, log_buf);
    num_stat_update_failures = 0;
    }

  return(rc);
  } /* END send_update_to_a_server() */



void update_mom_status()

  {
  mom_status.clear();

  generate_server_status(mom_status);
#ifdef NVIDIA_GPUS
  append_gpu_status(global_gpu_status, mom_status);
#endif /* NVIDIA_GPU */

#ifdef MIC
  add_mic_status(mom_status);
#endif /* MIC */
  } /* update_mom_status() */


int send_status_through_hierarchy()

  {
  node_comm_t *nc = NULL;
  int          rc = -1;

  if ((nc = update_current_path(mh)) != NULL)
    {
    /* write to the socket */
    while (nc != NULL)
      {
      if (write_status_strings(mom_status, nc) < 0)
        {
        nc->bad = TRUE;
        nc->mtime = time_now;
        nc = force_path_update(mh);
        }
      else 
        {
        rc = PBSE_NONE;
  
        close(nc->stream);
        break;
        }
      }
    }

  return(rc);
  } /* END send_status_through_hierarchy() */



/**
 * mom_server_all_update_stat
 *
 * This is the former is_update_stat.  It has been reworked to
 * first generate the strings and then walk the server list sending
 * the strings to each server.
 */

void mom_server_all_update_stat(void)
 
  {
  pid_t        pid;
  int          fd_pipe[2];
  int          rc;
  char         buf[LOCAL_LOG_BUF_SIZE];
  size_t       len;

  time_now = time(NULL);

  memset(buf, 0, LOCAL_LOG_BUF_SIZE);

  if (send_update() == FALSE)
    {
    /* no update is needed */
    return;
    }
 
  if (PBSNodeCheckInterval > 0)
    check_state((LastServerUpdateTime == 0));
 
  LastUpdateAttempt = time_now;
 
  /* We generate the status once, because this might be costly.
   * The dynamic string mom_status will contain NULL terminated strings.
   * The end of the buffer is marked with an empty string i.e. NULL NULL.
   */
 
  if (LOGLEVEL >= 6)
    {
    log_record(PBSEVENT_SYSTEM, 0, __func__, "composing status update for server");
    }

  if (is_reporter_mom == TRUE)
    {
    generate_alps_status(mom_status, apbasil_path, apbasil_protocol);

    if (send_update_to_a_server() == PBSE_NONE)
      {
      ForceServerUpdate = false;
      LastServerUpdateTime = time_now;
      }
    }
  else
    {
    /* The NVIDIA NVML library has a problem when we use it after the first fork. Let's get the gpu status first
       and then fork */
#ifdef NVIDIA_GPUS
    global_gpu_status.clear();
    add_gpu_status(global_gpu_status);
#endif

#ifdef MIC
    check_for_mics(global_mic_count);
#endif 

    /* It is possible that pbs_server may get busy and start queing incoming requests and not be able 
       to process them right away. If pbs_mom is waiting for a reply to a statuys update that has 
       been queued and at the same time the server makes a request to the mom we can get stuck
       in a pseudo live-lock state. That is the server is waiting for a response from the mom and
       the mom is waiting for a response from the server. neither of which will come until a request times out.
       If we fork the status updates this alleviates the problem by making one less request from the
       mom single threaded */
    rc = pipe(fd_pipe);
    if (rc != 0)
      {
      sprintf(buf, "pipe creation failed: %d", errno);
      log_err(-1, __func__, buf);
      }

    pid = fork();

    if (pid < 0)
      {
      log_record(PBSEVENT_SYSTEM, 0, __func__, "Failed to fork stat update process");
      return;
      }

    if (pid > 0)
      {
      // PARENT 
      close(fd_pipe[1]);
      ForceServerUpdate = false;
      LastServerUpdateTime = time_now;
      UpdateFailCount = 0;
      updates_waiting_to_send = 0;
    
      received_node                                             *rn;
      received_statuses.lock();
      container::item_container<received_node *>::item_iterator *iter = received_statuses.get_iterator();
      
      // clear cached statuses from hierarchy
      while ((rn = iter->get_next_item()) != NULL)
        rn->statuses.clear();

      delete iter;
      received_statuses.unlock();

      len = read(fd_pipe[0], buf, LOCAL_LOG_BUF_SIZE);

      close(fd_pipe[0]);

      if (len <= 0)
        {
        log_err(-1, __func__, "read of pipe failed for status update");
        return;
        }

      if (buf[0] != '0')
        num_stat_update_failures++;
      else
        {
        num_stat_update_failures = 0;
        for (int sindex = 0; sindex < PBS_MAXSERVER; sindex++)
          {
          if (mom_servers[sindex].pbs_servername[0] == '\0')
            continue;
          mom_servers[sindex].MOMLastSendToServerTime = time_now;
          }
        }

      return;
      }

    // CHILD
    close(fd_pipe[0]);

#ifdef NUMA_SUPPORT
    for (numa_index = 0; numa_index < num_node_boards; numa_index++)
#endif /* NUMA_SUPPORT */
      {
      update_mom_status();
  
      if (send_status_through_hierarchy() != PBSE_NONE)
        rc = send_update_to_a_server();
      }

    sprintf(buf, "%d", rc);
    len = strlen(buf);
    write(fd_pipe[1], buf, len);

    exit_called = true;
  
    exit(0);
    }
 
  }  /* END mom_server_all_update_stat() */




/**
 * power
 */

long power(

  register int x,
  register int n)

  {
  register long p;

  for (p = 1;n > 0;--n)
    {
    p *= x;
    }

  return(p);
  }





int calculate_retry_seconds(

  int count)

  {
  int retry_seconds = 0;

  /* Why are we using this hand-crafted power function instead of pow()? */
  /* pow() is probably using a more efficient processor instruction on most
   * architectures */

  retry_seconds = power(STARTING_RETRY_INTERVAL_IN_SECS, count);

  /* the (retry_seconds <= 0) condition helps avoid overflow! */

  if ((retry_seconds > MAX_RETRY_TIME_IN_SECS) ||
      (retry_seconds <= 0))
    {
    retry_seconds = MAX_RETRY_TIME_IN_SECS;
    }

  return(retry_seconds);
  }




/**
 * mom_server_diag
 *
 * This routine generates the diagnostic information for a single server
 * instance.  Note that the strings generated are newline separated.
 *
 * @param pms pointer to mom_server instance
 * @param sindex the mom_server index (used to display a server ID name)
 * @param output the string stream that all of this output should be written to
 * @see mom_server_all_diag
 */

void mom_server_diag(

  mom_server        *pms,
  int                sindex,
  std::stringstream &output)

  {
  char tmpLine[1024];
  time_t Now;

  if (pms->pbs_servername[0] == '\0')
    {
    return;
    }

  time(&Now);

  sprintf(tmpLine, "Server[%d]: %s (%s)\n",
          sindex,
          pms->pbs_servername,
          netaddr(&(pms->sock_addr)));

  output << tmpLine;

  if (pms->MOMSendStatFailure[0] != '\0')
    {
    sprintf(tmpLine, "  WARNING:  could not open connection to server, %s%s\n",
            pms->MOMSendStatFailure,
            (strstr(pms->MOMSendStatFailure, "cname") != NULL) ?
            " (check name resolution - /etc/hosts?)" :
            "");

    output << tmpLine;
    }

  if (TMOMRejectConn[0] != '\0')
    {
    output << "  WARNING:  invalid attempt to connect from server " << TMOMRejectConn << "\n";
    }

  if (pms->MOMLastRecvFromServerTime > 0)
    {
    sprintf(tmpLine, "  Last Msg From Server:   %ld seconds (%s)\n",
            (long)Now - pms->MOMLastRecvFromServerTime,
            (pms->MOMLastRecvFromServerCmd[0] != '\0') ?
            pms->MOMLastRecvFromServerCmd : "N/A");
    }
  else
    {
    sprintf(tmpLine, "  WARNING:  no messages received from server\n");
    }

  output << tmpLine;

  if (pms->MOMLastSendToServerTime > 0)
    {
    sprintf(tmpLine, "  Last Msg To Server:     %ld seconds\n",
            (long)Now - pms->MOMLastSendToServerTime);
    }
  else
    {
    sprintf(tmpLine, "  WARNING:  no messages sent to server\n");
    }

  output << tmpLine;

  return;
  }  /* END mom_server_diag() */





/**
 * mom_server_all_diag
 *
 * This routine is called as a result of a momctl -d3 command.
 * It generates strings that are sent back to the momctl
 * program to display diagnostic information about the servers.
 * This is called from mom_main where the message is recognized
 * and the other parts of the diagnostics are generated.
 */

void mom_server_all_diag(

  std::stringstream &output)

  {
  int sindex;

  if (mom_servers[0].pbs_servername[0] == '\0')
    {
    output << "WARNING:  server not specified (set $pbsserver)\n";
    }
  else
    {
    for (sindex = 0;sindex < PBS_MAXSERVER;sindex++)
      {
      mom_server_diag(&mom_servers[sindex], sindex, output);
      }
    }

  return;
  }




/**
 * mom_server_update_receive_time
 *
 * This is called from is_request whenever a message is received
 * from a pbs_server.  The information saved here is used to
 * display diagnostics and to decide if the server is up and active.
 *
 * @param stream The stream number to update.
 * @param command_name The name of the command that was received.
 */
void mom_server_update_receive_time(
    
  int stream,
  char *command_name,
  struct sockaddr_in *pAddr)

  {
  mom_server      *pms;
  unsigned long    ipaddr;

  time_now = time(NULL);

  ipaddr = ntohl(pAddr->sin_addr.s_addr);

  if ((pms = mom_server_find_by_ip(ipaddr)) != NULL)
    {
    pms->MOMLastRecvFromServerTime = time_now;
    snprintf(pms->MOMLastRecvFromServerCmd, sizeof(pms->MOMLastRecvFromServerCmd), "%s", command_name);
    }
  } /* END mom_server_update_receive_time() */





/**
 * mom_server_update_receive_time_by_ip
 *
 * This is a little weird as this is called from code
 * in the server directory, mom_process_request.  There is no 
 * stream number there but instead there is an IP address and so 
 * we use that to look up the server and update the info. 
 *
 * @param ipaddr The IP address from which the command was received.
 * @param command_name The name of the command that was received.
 */

void mom_server_update_receive_time_by_ip(

  u_long  ipaddr,
  const char   *command_name)

  {
  mom_server *pms;
  time_now = time(NULL);

  if ((pms = mom_server_find_by_ip(ipaddr)) != NULL)
    {
    pms->MOMLastRecvFromServerTime = time_now;

    snprintf(pms->MOMLastRecvFromServerCmd, sizeof(pms->MOMLastRecvFromServerCmd), "%s", command_name);
    }

  return;
  }




/**
** Modified by Tom Proett <proett@nas.nasa.gov> for PBS.
*/

/*tree *okclients = NULL;*/ /* tree of ip addrs */
AvlTree okclients = NULL;



/**
 * mom_server_valid_message_source
 *
 * This routine is called from mom_is_request to validate
 * that the request is coming from a know server.
 * If the server is good, a pointer to the server
 * instance is returned.  Otherwise NULL indicates error.
 *
 * @param stream The stream number in question
 * @return pms A pointer to the server instance.
 * @see mom_is_request
 */

mom_server *mom_server_valid_message_source(

  struct tcp_chan  *chan,
  char            **err_msg,
  struct sockaddr_in *pAddr)

  {
  u_long           ipaddr;
  mom_server      *pms;


  /* Check for the normal case, where some server has an open,
   * establish stream connection to the place where this
   * message came from.
   */

  ipaddr = ntohl(pAddr->sin_addr.s_addr);  /* Extract IP address of source of the message. */

  /* So the stream number did not match any server but maybe
   * the server has another stream connection open to the IP address.
   */

  if ((pms = mom_server_find_by_ip(ipaddr)))
    {
    /* Now this is the current stream number for this server */
    mom_server_all_update_stat();

    return(pms);
    }
  else
    {
    /* There is no existing stream connection to the server. */

    /* Maybe the right thing to do now is to iterate over all defined
     * servers. If there are servers defined with no open stream
     * and a getaddrinfo result matches the message source IP address,
     * then accept the stream and put the stream number into the
     * server struct.  Then in the future, the normal case above
     * will match.  This approach doesn't have the mom's madly
     * attempting to clobber the network with getaddrinfo if
     * the DNS server is dead.  We only do getaddrinfo if we
     * get a message from the pbs_server.
     */
#if 1
      {
      int sindex;

      for (sindex = 0;sindex < PBS_MAXSERVER;sindex++)
        {
        pms = &mom_servers[sindex];

        if (pms->pbs_servername[0])
          {
          struct addrinfo *addr_info;

          struct in_addr   saddr;
          u_long           server_ip;

          if (overwrite_cache(pms->pbs_servername, &addr_info))
            {
            saddr = ((struct sockaddr_in *)addr_info->ai_addr)->sin_addr;

            server_ip = ntohl(saddr.s_addr);

            if (ipaddr == server_ip)
              {
              okclients = AVL_insert(ipaddr, 0, NULL, okclients);

              return(pms);
              }
            }
          }
        }
      }  /* END BLOCK */

#endif

    if (LOGLEVEL >= 3)
      {
      *err_msg = (char *)calloc(1,240);
      snprintf(*err_msg, 240, "bad connect from %s - unauthorized server. Will check if its a valid mom",
        netaddr(pAddr));
      }
    }

  return(NULL);
  }  /* END mom_server_valid_message_source() */




int process_host_name(

  char *hostname,
  int   path,
  int   level,
  int   path_complete,
  int  *something_added)

  {
  char               *colon;
  unsigned short      rm_port;
  unsigned long       ipaddr;
  struct addrinfo    *addr_info;
  struct sockaddr_in  sa;
      
  rm_port      = PBS_MANAGER_SERVICE_PORT;
  
  if ((colon = strchr(hostname, ':')) != NULL)
    {
    *colon = '\0';
    rm_port = (unsigned short)atoi(colon+1);
    }
  
  if (overwrite_cache(hostname, &addr_info))
    {
    sa.sin_addr = ((struct sockaddr_in *)addr_info->ai_addr)->sin_addr;
    ipaddr      = ntohl(sa.sin_addr.s_addr);
    
    if (path_complete == FALSE)
      {
      add_network_entry(mh, hostname, addr_info, rm_port, path, level);
      *something_added = TRUE;
      }

    /* add to acceptable host tree */
    okclients = AVL_insert(ipaddr, rm_port, NULL, okclients);
    }
  else
    {
    snprintf(log_buffer, sizeof(log_buffer),
      "Bad entry in mom_hierarchy file, could not resolve host %s",
      hostname);

    log_err(PBSE_BADHOST, __func__, log_buffer);
    }

  return(PBSE_NONE);
  } /* END process_host_name() */




int am_i_on_this_level(

  char *level_str)

  {
  char *myself = level_str;

  while ((myself = strstr(myself, mom_alias)) != NULL)
    {
    if ((myself - 1 < level_str) ||
        (*(myself - 1) == ','))
      {
      myself += strlen(mom_alias);
      
      if ((*myself == '\0') ||
          (*myself == ',')  ||
          (*myself == ':'))
        {
        /* we only need to store the path up to the level that includes ourselves */
        return(TRUE);
        break;
        }
      }
    else
      {
      myself++;
      }
    }

  return(FALSE);
  } /* END am_i_on_this_level() */




int process_level_string(

  char *str,
  int   path,
  int   level,
  int  *path_complete,
  int  *something_added)

  {
  const char *delims = ",";
  char *host_tok;
  int   rc = PBSE_NONE;
  int   temp_rc;

  if (path < 0)
    return(path);

  if (am_i_on_this_level(str) == TRUE)
    *path_complete = TRUE;

  host_tok = strtok(str, delims);

  while (host_tok != NULL)
    {
    temp_rc = process_host_name(host_tok, path, level, *path_complete, something_added);

    if (rc == PBSE_NONE)
      rc = temp_rc;

    host_tok = strtok(NULL, delims);
    }

  return(rc);
  } /* END process_level_string() */



/* 
 * re-order the paths, the shallowest being first, because
 * we want to try the shallowest path
 **/
void sort_paths()

  {
  for (unsigned int i = 0;i < mh->paths.size();i++)
    {
    for (int j = (int)mh->paths.size() -1; (j >= 0) && (j != (int)i); j--)
      {
      if (mh->paths.at(j).size() < mh->paths.at(i).size())
        {
        /* swap positions */
        mom_levels tmp = mh->paths.at(i);
        mh->paths.at(i) = mh->paths.at(j);
        mh->paths.at(j) = tmp;
        }
      }
    }
  } /* END sort_paths() */




void reset_okclients()

  {
  okclients = AVL_clear_tree(okclients);

  // re-add each server
  for (int sindex = 0;sindex < PBS_MAXSERVER;sindex++)
    {
    mom_server *pms = &mom_servers[sindex];

    if (pms->pbs_servername[0] != '\0')
      {
      struct in_addr   saddr;
      u_long           ipaddr;
      struct addrinfo *addr_info;

      if (!overwrite_cache(pms->pbs_servername, &addr_info))
        {
        sprintf(log_buffer, "host %s not found", pms->pbs_servername);

        log_err(-1, __func__, log_buffer);
        }
      else
        {
        saddr = ((struct sockaddr_in *)addr_info->ai_addr)->sin_addr;

        ipaddr = ntohl(saddr.s_addr);

        if (ipaddr != 0)
          {
          okclients = AVL_insert(ipaddr, 0, NULL, okclients);
          }
        }
      }
    }

  // add localhost
  okclients = AVL_insert(localaddr, 0, NULL, okclients);

  // BMD: add the node's ip address

  }



int read_cluster_addresses(

  struct tcp_chan *chan,
  int              version)

  {
  int             rc;
  int             level = -1;
  int             path_index  = -1;
  int             path_complete = FALSE;
  int             something_added;
  char           *str;
  char           *okclients_list;
  std::string      hierarchy_file = "/n";
  long            list_size;
  long            list_len = 0;
  if (mh != NULL)
    free_mom_hierarchy(mh);

  mh = initialize_mom_hierarchy();
  reset_okclients();

  while (((str = disrst(chan, &rc)) != NULL) &&
         (rc == DIS_SUCCESS))
    {
    if (!strcmp(str, "<sp>"))
      {
      path_index++;
      path_complete = FALSE;
      something_added = FALSE;
      level = -1;

      hierarchy_file += "<path>\n";
      }
    else if (!strcmp(str, "<sl>"))
      {
      hierarchy_file += "  <level>";
      level++;
      }
    else if (!strcmp(str, "</sp>"))
      {
      if (path_complete == FALSE)
        {
        /* we were not in the last path, so delete it */
        if (mh->paths.size() > 0)
          {
          mh->paths.pop_back();
          path_index--;
          }
        hierarchy_file.clear();
        }
      else if (something_added == FALSE)
        {
        /* if we're on the first level of the path, we didn't record anything 
         * and we need to decrement the path_index */
        path_index--;
        hierarchy_file.clear();
        }
      else
        hierarchy_file += "</path>\n";
      }
    else if (!strcmp(str, "</sl>"))
      {
      hierarchy_file += "  </level>\n";
      /* NO-OP */
      }
    else if (!strcmp(str, IS_EOL_MESSAGE))
      {
      /* done */
      free(str);
      str = NULL;
      break;
      }
    else
      {
      /* receiving a level */
      hierarchy_file += str;
      process_level_string(str, path_index, level, &path_complete, &something_added);
      }

    free(str);
    } /* END reading input from chan */

  if (str != NULL)
    {
    free(str);
    str = NULL;
    }

  if (rc != DIS_SUCCESS)
    {
    /* transmission failure */
    if (mh != NULL)
      free_mom_hierarchy(mh);

    mh = initialize_mom_hierarchy();

    /* request new cluster addresses immediately */
    requested_cluster_addrs = 0;
    }
  else
    {
    received_cluster_addrs = true;
    send_update_soon();
    
    sort_paths();

    /* log the hierrarchy */
    list_size = MAXLINE * 2;
    if ((okclients_list = (char *)calloc(1, list_size)) != NULL)
      {
      AVL_list(okclients, &okclients_list, &list_len, &list_size);
      snprintf(log_buffer, sizeof(log_buffer),
        "Successfully received the mom hierarchy file. My okclients list is '%s', and the hierarchy file is '%s'",
        okclients_list, hierarchy_file.c_str());
      log_event(PBSEVENT_SYSTEM, PBS_EVENTCLASS_NODE, __func__, log_buffer);

      free(okclients_list);
      }
 
    /* tell the mom to go ahead and send an update to pbs_server */
    first_update_time = 0;
    }

  return(PBSE_NONE);
  } /* END read_cluster_addresses() */





/**
 * Request is coming from another server (i.e., pbs_server) over a DIS 
 * stream (process 'hello' and 'cluster_addrs' request).
 *
 * @see is_compose() - peer - generate message to send to pbs_server.
 * @see mom_process_request() - peer - handle jobstart, 
 *      jobcancel, etc messages.
 *
 * Read the stream to get a Inter-Server request.
 */

void mom_is_request(

  struct tcp_chan *chan,
  int              version,  /* I */
  int             *cmdp,     /* O (optional) */
  struct sockaddr_in *pAddr) /* Filled in internet address for this socket */

  {
  int                 command = 0;
  int                 ret = DIS_SUCCESS;
 
  char               *err_msg = NULL;
  u_long              ipaddr;
  extern char        *PBSServerCmds[];

  if (cmdp != NULL)
    *cmdp = 0;

  if (LOGLEVEL >= 7)
    {
    sprintf(log_buffer, "stream %d version %d",
      chan->sock,
      version);

    log_record(PBSEVENT_ERROR, PBS_EVENTCLASS_JOB, __func__, log_buffer);
    }

  if (version != IS_PROTOCOL_VER)
    {
    sprintf(log_buffer, "protocol version %d unknown",
            version);

    log_ext(-1,__func__,log_buffer,LOG_ALERT);

    close_conn(chan->sock, FALSE);
    chan->sock = -1;

    return;
    }

  /* check that machine is okay to be a server */
  if (mom_server_valid_message_source(chan, &err_msg,pAddr) == NULL)
    {
    ipaddr = ntohl(pAddr->sin_addr.s_addr);

    if (AVL_is_in_tree_no_port_compare(ipaddr, 0, okclients) == 0)
      {
      if (err_msg)
        {
        log_ext(-1,"mom_server_valid_message_source", err_msg, LOG_ALERT);
        free(err_msg);
        }
      else
        log_ext(-1, __func__, "Invalid source for IS_REQUEST", LOG_ALERT);

      sprintf(TMOMRejectConn, "%s  %s", netaddr(pAddr), "(server not authorized)");

      close_conn(chan->sock, FALSE);
      chan->sock = -1;
      return;
      }
    }

  if (err_msg)
    free(err_msg);
 
  command = disrsi(chan, &ret);

  if (ret != DIS_SUCCESS)
    command = -1;
  else
    {
    if (cmdp != NULL)
      *cmdp = command;
    
    if (LOGLEVEL >= 3)
      {
      sprintf(log_buffer, "command %d, \"%s\", received",
        command,
        PBSServerCmds[command]);
      
      log_record(
        PBSEVENT_ERROR,
        PBS_EVENTCLASS_JOB,
        __func__,
        log_buffer);
      }
    
    mom_server_update_receive_time(chan->sock, PBSServerCmds[command],pAddr);
    }

  switch (command)

    {
    case IS_NULL: /* a ping from the server */

      /* nothing seems to ever generate an IS_NULL message */

      if (internal_state & INUSE_DOWN)
        {
        int sindex;

        for (sindex = 0;sindex < PBS_MAXSERVER;sindex++)
          {
          if (mom_servers[sindex].pbs_servername[0] != '\0')
            state_to_server(sindex, 1);
          }
        }

      break;

    case IS_CLUSTER_ADDRS:

      /* server is sending the mom hierarchy to me */
      ret = read_cluster_addresses(chan, version);

      break;

    case IS_STATUS:

      if (read_status_strings(chan,version) < 0)
        ret = -1;
  
      break;

    default:

      sprintf(log_buffer, "unknown command %d sent",
        command);

      log_ext(-1,__func__,log_buffer,LOG_ALERT);

      ret = -1;
    }  /* END switch(command) */

  if (ret != DIS_SUCCESS)
    {
    /* We come here if we got a DIS read error or a protocol element is missing.  */
 
    if (ret > 0)
      {
      sprintf(log_buffer, "%s from %s", dis_emsg[ret], netaddr(pAddr));
      log_ext(-1,__func__,log_buffer,LOG_ALERT);
      }
    }

  close_conn(chan->sock, FALSE);
  chan->sock = -1;
  }  /* END mom_is_request() */





float compute_load_threshold(

  char  *config,
  int    numvnodes,
  float  threshold)

  {
  float  retval = -1;
  float  tmpval;
  char  *op;

  if (numvnodes <= 0)
    {
    return(threshold);
    }

  if ((config == NULL) || (*config == '0'))
    {
    return(threshold);
    }

  switch (*config)
    {

    case 'c':

      retval = system_ncpus;

      break;

    case 't':

      retval = numvnodes;

      break;

    default:

      return(threshold);

      /*NOTREACHED*/

      break;
    }

  config++;

  switch (*config)
    {

    case '+':

    case '-':

    case '*':

    case '/':

      op = config;

      break;

    default:

      return(retval);

      /*NOTREACHED*/

      break;
    }

  config++;

  tmpval = atof(config);

  if (!tmpval)
    {
    return(retval);
    }

  switch (*op)
    {
    case '+':

      retval = retval + tmpval;

      break;

    case '-':

      retval = retval - tmpval;

      break;

    case '*':

      retval = retval * tmpval;

      break;

    case '/':

      retval = retval / tmpval;

      break;
    }

  return(retval);
  }  /* END compute_load_threshold() */





/*
 * check_busy() -
 * If current load average ge max_load_val and busy not already set
 *  set it
 * If current load average lt ideal_load_val and busy currently set
 *  unset it
 */

void check_busy(

  double mla) /* I */

  {
  int sindex;
  int numvnodes = 0;
  job *pjob;
  float myideal_load;
  float mymax_load;

  extern int   internal_state;

  if ((auto_max_load != NULL) || (auto_ideal_load != NULL))
    {
    std::list<job *>::iterator iter;

    for (iter = alljobs_list.begin(); iter != alljobs_list.end(); iter++)
      {
      pjob = *iter;

      numvnodes += pjob->ji_numvnod;
      }

    mymax_load = compute_load_threshold(auto_max_load, numvnodes, max_load_val);

    myideal_load = compute_load_threshold(auto_ideal_load, numvnodes, ideal_load_val);
    }
  else
    {
    mymax_load = max_load_val;
    myideal_load = ideal_load_val;
    }

  if ((mla >= mymax_load) &&
      ((internal_state & INUSE_BUSY) == 0))
    {
    /* node transitioned from free to busy, report state */

    if (LOGLEVEL >= 2)
      {
      sprintf(log_buffer, "state changed from idle to busy (load max=%f  detected=%f)\n",
              mymax_load,
              mla);

      log_record(PBSEVENT_ERROR, PBS_EVENTCLASS_JOB, __func__, log_buffer);
      }

    internal_state |= INUSE_BUSY;

    for (sindex = 0;sindex < PBS_MAXSERVER;sindex++)
      {
      if (mom_servers[sindex].pbs_servername[0] != '\0')
        mom_servers[sindex].ReportMomState = 1;
      }
    }
  else if ((mla < myideal_load) &&
           ((internal_state & INUSE_BUSY) != 0))
    {
    /* node transitioned from busy to free, report state */

    if (LOGLEVEL >= 4)
      {
      sprintf(log_buffer, "state changed from busy to idle (load max=%f  detected=%f)\n",
              mymax_load,
              mla);

      log_record(PBSEVENT_ERROR, PBS_EVENTCLASS_JOB, __func__, log_buffer);
      }

    internal_state = (internal_state & ~INUSE_BUSY);

    for (sindex = 0;sindex < PBS_MAXSERVER;sindex++)
      {
      if (mom_servers[sindex].pbs_servername[0] != '\0')
        mom_servers[sindex].ReportMomState = 1;
      }
    }

  return;
  }  /* END check_busy() */




/*
 * check_state() -
 *   if down criteria satisfied and node is up, mark node down
 *   if down criteria is not set and node is down, mark it up
 */

void check_state(

  int Force)  /* I */

  {
  static int ICount = 0;

  char tmpPBSNodeMsgBuf[MAXLINE];

  if (Force)
    {
    ICount = 0;
    }

  memset(tmpPBSNodeMsgBuf, 0, MAXLINE);

  /* conditions:  external state should be down if
     - inadequate file handles available (for period X)
     - external health check fails
  */

  /* verify adequate space in spool directory */

#define TMINSPOOLBLOCKS 100  /* blocks available in spool directory required for proper operation */


#if MOMCHECKLOCALSPOOL
    {
    char *sizestr;
    u_Long freespace = 0;
    extern char *size_fs(char *);  /* FIXME: put this in a header file */

    /* size_fs() is arch-specific method in mom_mach.c */
    sizestr = size_fs(path_spool);  /* returns "free:total" */

    if (sizestr != NULL)
      freespace = atoL(sizestr);

    if (freespace < TMINSPOOLBLOCKS)
      {
      /* inadequate disk space in spool directory */

      strcpy(PBSNodeMsgBuf, "ERROR: torque spool filesystem full");

      /* NOTE:  adjusting internal state may not be proper behavior, see note below */

      internal_state |= INUSE_DOWN;
      ICount++;

      ICount %= MAX(1, PBSNodeCheckInterval);

      return;
      }
    }    /* END BLOCK */

#endif /* MOMCHECKLOCALSPOOL */

  if (PBSNodeCheckPath[0] != '\0')
    {
    if (ICount == 0)
      {
      /* only do this when running the check script, otherwise down nodes are 
       * marked as up */
      /* clear node state and node messages */

      internal_state &= ~INUSE_DOWN;

      PBSNodeMsgBuf[0] = '\0';

      if (MUReadPipe(
            PBSNodeCheckPath,
            tmpPBSNodeMsgBuf,
            sizeof(tmpPBSNodeMsgBuf)) == 0)
        {
        if (!strncasecmp(tmpPBSNodeMsgBuf, "ERROR", strlen("ERROR")))
          {
          internal_state |= INUSE_DOWN;

          if (LOGLEVEL >= 1)
            {
            snprintf(log_buffer,sizeof(log_buffer),
            "Setting node to down. The node health script output the following message: %s",
            tmpPBSNodeMsgBuf);
            log_event(PBSEVENT_SYSTEM,PBS_EVENTCLASS_NODE,__func__,log_buffer);
            }
          }
        else if (!strncasecmp(tmpPBSNodeMsgBuf, "EVENT:", strlen("EVENT:")))
          {
          /* pass event directly to scheduler for processing */
          /* EVENT: is a keyword for Moab */
          if (LOGLEVEL >= 3)
            {
            snprintf(log_buffer,sizeof(log_buffer),
              "Node health script ran and says the node is healthy with this message: %s",
              tmpPBSNodeMsgBuf);
            log_event(PBSEVENT_SYSTEM,PBS_EVENTCLASS_NODE,__func__,log_buffer);
            }
          }
        else
          {
          /* We are not going to post this message */
          tmpPBSNodeMsgBuf[0] = '\0';

          if (LOGLEVEL >= 6)
            {
            snprintf(log_buffer,sizeof(log_buffer),
              "Node health script ran and says the node is healthy");
            log_event(PBSEVENT_SYSTEM,PBS_EVENTCLASS_NODE,__func__,log_buffer);
            }
          }
        }
      }    /* END if (ICount == 0) */

    if (tmpPBSNodeMsgBuf[0] != '\0')
      {
      /* update node msg buffer */
      snprintf(PBSNodeMsgBuf, sizeof(PBSNodeMsgBuf), "%s", tmpPBSNodeMsgBuf);

      PBSNodeMsgBuf[sizeof(PBSNodeMsgBuf) - 1] = '\0';
      }
    }      /* END if (PBSNodeCheckPath[0] != '\0') */

  ICount++;

  ICount %= MAX(1, PBSNodeCheckInterval);

  return;
  }  /* END check_state() */




/**
 * shutdown_to_server() - if ReportMomState is set, send state message to
 * the server.
 *
 * @see is_compose() - child
 */

void shutdown_to_server(

  int ServerIndex)  /* I */

  {
  int              sock;
  u_long           ipaddr;
  mom_server      *pms = &mom_servers[ServerIndex];
  int              ret;
  char             error_buf[MAXLINE];
  struct tcp_chan *chan = NULL;

  /* We high jacked this function from state_to_server. We are modifying it 
     so we make our own connection to the server */

  ipaddr = htonl(pms->sock_addr.sin_addr.s_addr);
  if ((sock = client_to_svr(ipaddr, default_server_port, 0, error_buf)) < 0)
    return;

  if ((chan = DIS_tcp_setup(sock)) == NULL)
    goto shutdown_to_server_done;

  if (is_compose(chan, pms->pbs_servername, IS_UPDATE) != DIS_SUCCESS)
    {
    goto shutdown_to_server_done;
    }

  if ((ret = diswus(chan, pbs_mom_port)) != DIS_SUCCESS)
    {
    goto shutdown_to_server_done;
    }
  
  if ((ret = diswus(chan, pbs_rm_port)) != DIS_SUCCESS)
    {
    goto shutdown_to_server_done;
    }

  if (diswui(chan, internal_state) != DIS_SUCCESS)
    {
    mom_server_stream_error(chan->sock, pms->pbs_servername, __func__, "writing internal state");

    goto shutdown_to_server_done;
    }

  if (mom_server_flush_io(chan, __func__, "flush") == DIS_SUCCESS)
    {
    /* send successful, unset ReportMomState */

    pms->ReportMomState = 0;

    if (LOGLEVEL >= 4)
      {
      sprintf(log_buffer, "sent updated state 0x%x to server %s",
              internal_state,
              pms->pbs_servername);

      log_record(PBSEVENT_ERROR, PBS_EVENTCLASS_JOB, __func__, log_buffer);
      }
    }

shutdown_to_server_done:

  if (chan != NULL)
    DIS_tcp_cleanup(chan);

  close(sock);

  return;
  }  /* END shutdown_to_server() */




/**
 * state_to_server() - if ReportMomState is set, send state message to
 * the server.
 *
 * @see is_compose() - child
 */

void state_to_server(

  int ServerIndex,  /* I */
  int force)        /* I (boolean) */

  {
  mom_server       *pms = &mom_servers[ServerIndex];
  int              ret;
  int              stream;
  struct tcp_chan *chan;

  if (((force == 0) &&
       (pms->ReportMomState == 0)) ||
      (pms->pbs_servername[0] == '\0'))
    {
    return;    /* Do nothing, just return */
    }

  stream = tcp_connect_sockaddr((struct sockaddr *)&pms->sock_addr, sizeof(pms->sock_addr), true);

  if (IS_VALID_STREAM(stream))
    {
    if ((chan = DIS_tcp_setup(stream)) == NULL)
      {
      }
    else if (is_compose(chan, pms->pbs_servername, IS_UPDATE) != DIS_SUCCESS)
      {
      }
    else if ((ret = diswus(chan, pbs_mom_port)) != DIS_SUCCESS)
      {
      }
    else if ((ret = diswus(chan, pbs_rm_port)) != DIS_SUCCESS)
      {
      }
    else if (diswui(chan, internal_state) != DIS_SUCCESS)
      {
      mom_server_stream_error(chan->sock, pms->pbs_servername, __func__, "writing internal state");
      }
    else if (mom_server_flush_io(chan, __func__, "flush") == DIS_SUCCESS)
      {
      /* send successful, unset ReportMomState */
      pms->ReportMomState = 0;
      
      if (LOGLEVEL >= 4)
        {
        sprintf(log_buffer, "sent updated state 0x%x to server %s",
          internal_state,
          pms->pbs_servername);
        
        log_record(PBSEVENT_ERROR, PBS_EVENTCLASS_JOB, __func__, log_buffer);
        }
      }

    if (chan != NULL)
      DIS_tcp_cleanup(chan);

    close(stream);
    }
  else
    {
    mom_server_stream_error(-1, pms->pbs_servername, __func__, "Couldn't open stream to server");
    }

  return;
  }  /* END state_to_server() */





void mom_server_all_send_state(void)

  {
  int         sindex;
  mom_server *pms;

  for (sindex = 0;sindex < PBS_MAXSERVER;sindex++)
    {
    pms = &mom_servers[sindex];

    if (pms->ReportMomState != 0)
      state_to_server(sindex, 0);
    }

  return;
  } /* END mom_server_all_send_state() */





/*
 * The job needs to originate a message to the server.
 * So we are opening a communication socket to the
 * server associated with the job.
 *
 * Most messages from the mom to the server are replies
 * to a message originated from the server and use
 * the socket stream established by the server.
 * So this routine is rarely called.  It is mostly
 * used for job obits.
 *
 * The server address is saved two ways, as an
 * string attribute in the job and also as a long.
 * The long value must somehow get set by the
 * server as it does not appear to be set anywhere here.
 * The string address is an IP address or host name with an
 * optional port number, i.e. format "xx.xx.xx.xx[:xxx]".
 * All we do here is extract the port number from the
 * string if it is present.
 *
 * What are the implications of the job server address
 * being hard-coded for a job with regards to high
 * availability?  If the original server fails, will
 * the job be able to send the obit's to the alternate
 * server?
 *
 * @see add_conn() - child
 */

int mom_open_socket_to_jobs_server(

  job        *pjob,
  const char *caller_id,
  void       *(*message_handler)(void *))

  {
  char       *svrport = NULL;
  char       *serverAddr = NULL;
  char        error_buffer[1024];
  int         sock;
  int         sock3;
  int         port;
  int         sindex;
  mom_server *pms;

  /* See if the server address string has a ':' implying a port number. */

  serverAddr = pjob->ji_wattr[JOB_ATR_at_server].at_val.at_str;
  if (serverAddr != NULL)
    {
    svrport = strchr(serverAddr, (int)':');
    }

  if (svrport)
    port = atoi(svrport + 1);    /* Yes, use the specified server port number. */
  else
    port = default_server_port;  /* No, use the global default server port. */

  sock = client_to_svr(
           pjob->ji_qs.ji_un.ji_momt.ji_svraddr, /* This is set in req_queuejob. */
           port,
           1,  /* use local socket */
           error_buffer);  /* O */

  if (sock < 0)
    {
    /* error_buffer is filled in by the library with a message describing the failure */

    log_err(errno, caller_id, error_buffer);

    /* Try to make HA work.
     * Perhaps we should keep a list identifying who has been tried?
     * But for now, just see if there is another server
     * not at the IP address of where the job came from.
     */

    for (sindex = 0;sindex < PBS_MAXSERVER;sindex++)
      {
      u_long              ipaddr;

      pms = &mom_servers[sindex];

      if (pms->pbs_servername[0] != '\0')
        {
        ipaddr = ntohl(pms->sock_addr.sin_addr.s_addr);

        if (ipaddr != pjob->ji_qs.ji_un.ji_momt.ji_svraddr)
          {
          sock = client_to_svr(
                   ipaddr,
                   ntohs(pms->sock_addr.sin_port),
                   1,  /* use local socket */
                   error_buffer);  /* O */

          if (sock >= 0)
            break;
          }
        }
      }
    }

  /* The epilogue code needs the socket number at 3 or above. */
  if (sock >= 0)
    {
    if (sock < 3)
      {
      sock3 = fcntl(sock, F_DUPFD, 3);
      close(sock);
      sock = sock3;
      }
   
    /* Associate a message handler with the connection */
    if ((message_handler != NULL)&&(sock >= 0))
      {
      add_conn(
        sock,
        ToServerDIS,
        pjob->ji_qs.ji_un.ji_momt.ji_svraddr,
        port,
        PBS_SOCK_INET,
        message_handler);
      }
    }

  /*
   * ji_momhandle is used to match reply messages to their job.
   * Why not use the job number to find the job when we receive a reply message?
   */
  pjob->ji_momhandle = sock;

  return(sock);
  }  /* END mom_open_socket_to_jobs_server() */


int mom_open_socket_to_jobs_server_with_retries(

  job        *pjob,
  const char *caller_id,
  void       *(*message_handler)(void *),
  int         retry_limit)

  {
  int retries = -1;
  int sock = -1;

  while ((sock < 0) &&
         (retries < retry_limit))
    {
    sock = mom_open_socket_to_jobs_server(pjob, __func__, message_handler);

    switch (errno)
      {
      case EINTR:
      case ETIMEDOUT:
      case EINPROGRESS:

        retries++;

        break;

      default:

        retries = retry_limit;

        break;
      }
    }

  return(sock);
  } // END mom_open_socket_to_jobs_server_with_retries()



/**
 * clear_down_mom_servers
 *
 * Clears the mom_server down address list.
 * Called from the catch_child code.
 *
 * @see scan_for_exiting
 */

void clear_down_mom_servers(void)
  {
  int sindex;

  for (sindex = 0;sindex < PBS_MAXSERVER;sindex++)
    {
    down_svraddrs[sindex] = 0;
    }

  return;
  } /* END clear_down_mom_servers() */

/**
 * is_mom_server_down
 *
 * Checks to see if the server address is in the down list.
 * Called from the catch_child code.
 * @see scan_for_exiting
 */

int is_mom_server_down(

  pbs_net_t server_address)

  {
  int sindex;

  for (sindex = 0; sindex < PBS_MAXSERVER && down_svraddrs[sindex] != 0; sindex++)
    {
    if (down_svraddrs[sindex] == server_address)
      {
      return(1);
      }
    }

  return(0);
  }


#define THIS_HOST_LEN 256
/*
 * is_for_this_host
 *
 * Parses the device_string and compares the host for
 * the device in the string to see if it is the same
 * as this host. If yes return true, else return false
 *
 * @param device_spec - string with the job specification 
 *                   indicating the node and device index to be run 
 *                   in the job.
 *
 */

bool is_for_this_host(
    
  std::string device_spec, 
  const char *suffix)

  {
  char  *ptr;
  char  temp_char_string[THIS_HOST_LEN];

  snprintf(temp_char_string, sizeof(temp_char_string), "%s", device_spec.c_str());

  /* peel off the -device part of the device_spec */
  ptr = strstr(temp_char_string, suffix);
  if (ptr != NULL)
    *ptr = '\0';
  else
    return(false);

  if (!strcmp(mom_alias, temp_char_string))
    return(true);

  return(false);
  }

void get_device_indices(
  
  const char *device_str, 
  std::vector<unsigned int> &device_indices, 
  const char *suffix)

  {
  std::string device_string = device_str;
  std::vector<std::string> device_tokens;
  boost::char_separator<char> sep("+");
  boost::tokenizer< boost::char_separator<char> > tokens(device_string, sep);

  /* reset device_indices so it is empty */
  /* The string comes in with the format of
     <hostname>-gpu/<index1>+<hostname>-gpu/<index2>+...
   */
  device_indices.clear();

  /* pull out each element of the device string */
  //BOOST_FOREACH (const std::string& t, tokens)
  //  {
  //  device_tokens.push_back(t);
  //  }

  for (boost::tokenizer< boost::char_separator<char> >::iterator t=tokens.begin(); t != tokens.end(); ++t)
    {
    device_tokens.push_back(*t);
    }

  /* We now have each device request in the form of <host>-device/x where 
     x is the indices of the device to allocate. See the spec is for this host
     and add the index to device_indices if it is. */
  for (std::vector<std::string>::iterator device_spec = device_tokens.begin(); device_spec != device_tokens.end(); ++device_spec)
    {
    std::string host_name_part;
    std::string device_index_part;
    std::vector<std::string> parts;
    boost::char_separator<char> device_sep("/");
    boost::tokenizer< boost::char_separator<char> > device_spec_parts(*device_spec, device_sep);

/*    BOOST_FOREACH (const std::string& tok, device_spec_parts)
      {
      parts.push_back(tok);
      }*/

    for (boost::tokenizer< boost::char_separator<char> >::iterator tok=device_spec_parts.begin(); tok != device_spec_parts.end(); tok++)
      {
      parts.push_back(*tok);
      }

    /* parts should have two entries. */
    /* The first part will be the host name */
    host_name_part = parts[0].c_str();
    
    /* The second part will be the index */
    device_index_part = parts[1].c_str();

    if (is_for_this_host(host_name_part, suffix) == true)
      {
      unsigned int device_index = atoi(device_index_part.c_str());

      device_indices.push_back(device_index);
      }

    }
  }



/**
 * no_mom_servers_down
 *
 * Checks to see if the server address down list is empty.
 * Called from the catch_child code.
 * @see scan_for_exiting
 */

int no_mom_servers_down(void)

  {
  if (down_svraddrs[0] == 0)
    {
    return(1);
    }

  return(0);
  }






/**
 * set_mom_server_down
 *
 * Add this server address to the down list.
 * Called from the catch_child code.
 * @see scan_for_exiting
 */

void set_mom_server_down(

  pbs_net_t server_address)

  {
  int sindex;

  for (sindex = 0; sindex < PBS_MAXSERVER; sindex++)
    {
    if (down_svraddrs[sindex] == 0)
      {
      down_svraddrs[sindex] = server_address;
      break;
      }
    }

  return;
  }



/* END mom_server.c */

