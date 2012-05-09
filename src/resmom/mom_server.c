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
#include <netdb.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/param.h>
#include <sys/times.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/time.h>
#if defined(NTOHL_NEEDS_ARPA_INET_H) && defined(HAVE_ARPA_INET_H)
#include <arpa/inet.h>
#endif

#if defined(NVIDIA_GPUS) && defined(NVML_API)
#include "nvml.h"
#endif  /* NVIDIA_GPUS and NVML_API */


#include "pbs_ifl.h"
#include "pbs_error.h"
#include "log.h"
#include "net_connect.h"
#include "rpp.h"
#include "dis.h"
#include "dis_init.h"
#include "list_link.h"
#include "attribute.h"
#include "pbs_nodes.h"
#include "resmon.h"
#include "server_limits.h"
#include "pbs_job.h"
#include "utils.h"

#include        "mcom.h"

/* Global Data Items */
#include "Long.h"

#define MAX_RETRY_TIME_IN_SECS  (5 * 60)
#define STARTING_RETRY_INTERVAL_IN_SECS  2
#define MAX_GPUS  32



typedef struct mom_server
  {
  int             SStream;                  /* streams to pbs_server daemons */
  char            pbs_servername[PBS_MAXSERVERNAME + 1];
  time_t          next_connect_time;
  int             connect_failure_count;
  int             sent_hello_count;
  char            ReportMomState;
  time_t          MOMLastSendToServerTime;
  time_t          MOMLastRecvFromServerTime;
  char            MOMLastRecvFromServerCmd[MMAX_LINE];
  int             received_hello_count;
  int             received_cluster_address_count;
  char            MOMSendStatFailure[MMAX_LINE];
  } mom_server;

mom_server     mom_servers[PBS_MAXSERVER];
int            mom_server_count = 0;
pbs_net_t      down_svraddrs[PBS_MAXSERVER];


extern unsigned int default_server_port;
extern char  mom_host[];
extern char  *path_jobs;
extern char  *path_home;
extern  char            *path_spool;
extern int  pbs_errno;
extern unsigned int pbs_mom_port;
extern unsigned int pbs_rm_port;
extern unsigned int pbs_tm_port;
extern int             internal_state;
extern int             LOGLEVEL;
extern char            PBSNodeCheckPath[1024];
extern int             PBSNodeCheckInterval;
extern char            PBSNodeMsgBuf[1024];
extern int             received_hello_count[];
extern char            TMOMRejectConn[];
extern time_t          LastServerUpdateTime;
extern int             ServerStatUpdateInterval;
extern long            system_ncpus;
extern int             alarm_time; /* time before alarm */
extern int             rm_errno;
extern time_t          time_now;
extern int             verbositylevel;
extern tree           *okclients;  /* accept connections from */

extern struct config *rm_search(struct config *where, char *what);

extern struct rm_attribute *momgetattr(char *str);
extern char *conf_res(char *resline, struct rm_attribute *attr);
extern char *dependent(char *res, struct rm_attribute *attr);
extern char *reqgres(struct rm_attribute *);

#ifdef NVIDIA_GPUS
extern int find_file(char *, char *);
extern char  mom_host[];
extern int             MOMNvidiaDriverVersion;
extern int  use_nvidia_gpu;
#endif  /* NVIDIA_GPUS */

char TORQUE_JData[MMAX_LINE];

void state_to_server(int, int);
#ifdef NVIDIA_GPUS
void mom_server_all_update_gpustat(void);
#endif  /* NVIDIA_GPUS */
extern void DIS_rpp_reset(void);

#ifdef NVIDIA_GPUS
int    nvidia_gpu_modes[50];
#endif  /* NVIDIA_GPUS */
/**
 * mom_server_init
 *
 * Does the memory intialization for an instance of a mom server
 * structure.
 *
 * @param pms pointer to mom_server instance
 * @see mom_server_all_init
 */

void mom_server_init(

  mom_server *pms)

  {
  pms->SStream = -1;
  pms->MOMLastRecvFromServerTime = 0;
  pms->ReportMomState = 1;

  return;
  }




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
  int sindex;

  for (sindex = 0;sindex < PBS_MAXSERVER;sindex++)
    {
    mom_server_init(&mom_servers[sindex]);
    }

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

  char *name)

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
 * mom_server_find_by_stream
 *--------------------------------------------------------------------
 * Find an instance of a mom server structure given a stream number.
 *
 * @param stream number to find
 * @see mom_server_find_by_ip
 */

mom_server *mom_server_find_by_stream(

  int stream)

  {
  mom_server *pms;
  int sindex;

  for (sindex = 0;sindex < PBS_MAXSERVER;sindex++)
    {
    pms = &mom_servers[sindex];

    if (pms->SStream == stream)
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
 * @see mom_server_find_by_stream
 */

mom_server *mom_server_find_by_ip(

  u_long search_ipaddr)

  {
  mom_server         *pms;
  int                 sindex;

  struct sockaddr_in *addr;
  u_long              ipaddr;

  for (sindex = 0;sindex < PBS_MAXSERVER;sindex++)
    {
    pms = &mom_servers[sindex];

    if (pms->SStream != -1)
      {
      addr = rpp_getaddr(pms->SStream);

      if (addr == NULL)
        {
        /* FAILURE */

        return(NULL);
        }

      ipaddr = ntohl(addr->sin_addr.s_addr);

      if (ipaddr == search_ipaddr)
        {
        return(pms);
        }
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

  char *value)

  {
  static char *id = "mom_server_add";
  mom_server *pms;


  if ((pms = mom_server_find_by_name(value)))
    {
    /* This server name has already been added. */

    sprintf(log_buffer, "server host %s already added",
            value);

    log_record(PBSEVENT_SYSTEM, PBS_EVENTCLASS_SERVER, id, log_buffer);
    }
  else if ((pms = mom_server_find_empty_slot()) != NULL)
    {
    /* Fill in the new server instance */

    strncpy(pms->pbs_servername, value, PBS_MAXSERVERNAME);
    mom_server_count++;

    sprintf(log_buffer, "server %s added", pms->pbs_servername);
    log_record(PBSEVENT_SYSTEM, PBS_EVENTCLASS_SERVER, id, log_buffer);
    }
  else
    {
    sprintf(log_buffer, "server table overflow (max=%d) - server host %s not added",
            PBS_MAXSERVER,
            value);

    log_err(-1, id, log_buffer);

    return(0); /* FAILURE */
    }

  /* Leaving this out breaks things but seems bad because if gethostbyname fails,
   * there is no retry except for the old way reinserting the name over and over again.
   * And what happens if the server connect via a different interface than the
   * one that gethostbyname returns?  It really seems better to not deal with
   * the IP address here but rather do what needs to be done when a connection
   * is established.  Anyway, this should fix things for now.
   */

    {

    struct hostent *host;

    struct in_addr  saddr;
    u_long          ipaddr;

    /* FIXME: must be able to retry failed lookups later */

    if ((host = gethostbyname(pms->pbs_servername)) == NULL)
      {
      sprintf(log_buffer, "host %s not found",
              pms->pbs_servername);

      log_err(-1, id, log_buffer);

      }
    else
      {
      memcpy(&saddr, host->h_addr, host->h_length);

      ipaddr = ntohl(saddr.s_addr);

      if (ipaddr != 0)
        tinsert(ipaddr, NULL, &okclients);
      }
    }    /* END BLOCK */

  return(1);      /* SUCCESS */
  }  /* END mom_server_add() */






/**
 * mom_server_open_stream
 *
 * Open a connection to a pbs_server.
 *
 * @param pms pointer to mom_server instance
 *
 * @see mom_server_check_connection() - parent
 */

int mom_server_open_stream(

  mom_server *pms)

  {
  static char id[] = "mom_server_open_stream";

  static int PassCount = 0;
  char server_name[PBS_MAXSERVERNAME + 1];
  int port = default_server_port;
  char *portstr;

  /* Make a copy of the server name because it might have a port number. */

  strcpy(server_name, pms->pbs_servername);

  if ((portstr = strchr(server_name, ':')) != NULL)
    {
    *(portstr) = '\0';

    if (*(portstr + 1) != '\0')
      port = atoi(portstr + 1);
    }

  if (LOGLEVEL >= 5)
    {
    sprintf(log_buffer, "%s: trying to open RPP conn to %s port %d",
      id,
      server_name,
      port);

    log_record(PBSEVENT_SYSTEM,0,id,log_buffer);
    }

  if ((pms->SStream = rpp_open(
                        server_name,
                        port,
                        pms->MOMSendStatFailure)) < 0)
    {
    /* FAILURE */

    if ((PassCount == 0) || (LOGLEVEL >= 6))
      {
      if (errno == ENOENT)
        {
        sprintf(log_buffer, "%s: cannot open rpp connection to %s, errno=%d, %s (check /etc/hosts file?)",
          id,
          server_name,
          errno,
          pms->MOMSendStatFailure);
        }
      else
        {
        sprintf(log_buffer, "%s: cannot open rpp connection to %s, errno=%d, %s",
          id,
          server_name,
          errno,
          pms->MOMSendStatFailure);
        }

      log_record(PBSEVENT_SYSTEM, 0, id, log_buffer);
      }

    PassCount = 1;

    pms->SStream = -1;

    return(DIS_EOF);
    }  /* END if ((pms->SStream = rpp_open()) < 0) */

  if (LOGLEVEL >= 3)
    {
    sprintf(log_buffer, "%s: added connection to %s port %d",
      id,
      server_name,
      port);

    log_record(PBSEVENT_SYSTEM, 0, id, log_buffer);
    }

  return(DIS_SUCCESS);
  }  /* END mom_server_open_stream() */





void mom_server_close_stream(

  int stream)

  {
  mom_server *pms;

  if ((pms = mom_server_find_by_stream(stream)) != NULL)
    {
    pms->SStream = -1;  /* Force new connection to server next time mom_server_check_connections is called. */
    }

  return;
  }





void mom_server_stream_error(

  mom_server *pms,
  char       *id,
  char       *message)
  {
  sprintf(log_buffer, "error %s to server %s",
    message,
    pms->pbs_servername);

  log_record(PBSEVENT_SYSTEM, 0, id, log_buffer);

  rpp_close(pms->SStream);

  pms->SStream = -1;  /* Force new connection to server next time mom_server_check_connections is called. */

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

  mom_server *pms,
  char       *id,
  char       *message)

  {
  if (rpp_flush(pms->SStream) == -1)
    {
    log_err(errno, id, message);

    rpp_close(pms->SStream);

    pms->SStream = -1;  /* Force new connection to server next time mom_server_check_connections is called. */

    return(DIS_PROTO);
    }

  return(DIS_SUCCESS);
  }





/**
 * Create an inter-server message to send to pbs_server (i.e., 'send status update')
 *
 * @see state_to_server() - parent - create state IS_UPDATE message
 * @see is_update_stat() - parent - create full IS_UPDATE message
 * @see is_request() - peer - process hello/cluster_addrs requests from pbs_server
 */

int is_compose(

  mom_server *pms,
  int         command)

  {
  int ret;

  if (pms->SStream < 0)
    {
    return(DIS_EOF);
    }

  DIS_rpp_reset();

  if ((ret = diswsi(pms->SStream, IS_PROTOCOL)) != DIS_SUCCESS)
    {
    mom_server_stream_error(pms, "is_compose", "writing protocol");

    return(ret);
    }
  else if ((ret = diswsi(pms->SStream, IS_PROTOCOL_VER)) != DIS_SUCCESS)
    {
    mom_server_stream_error(pms, "is_compose", "writing protocol version");

    return(ret);
    }
  else if ((ret = diswsi(pms->SStream, command)) != DIS_SUCCESS)
    {
    mom_server_stream_error(pms, "is_compose", "writing protocol version");

    return(ret);
    }

  return(DIS_SUCCESS);
  }  /* END is_compose() */






/**
 *  generate_server_status
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





extern struct config *config_array;

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

  char  *name,
  char **BPtr,
  int   *BSpace)

  {
  struct config  *ap;

  struct rm_attribute *attr;
  char *value;

  ap = rm_search(config_array, name);

  if (ap)
    {
    attr = momgetattr(ap->c_u.c_value);

    if (attr)
      {
      value = dependent(name, attr);

      if (value && *value)
        {
        MUSNPrintF(BPtr, BSpace, "%s=%s",
          name,
          value);

        (*BPtr)++; /* Need to start the next string after the null */
        (*BSpace)--;
        }
      }
    }

  return;
  }





void gen_arch(

  char  *name,
  char **BPtr,
  int   *BSpace)

  {
  struct config  *ap;

  ap = rm_search(config_array,name);

  if (ap != NULL)
    {
    MUSNPrintF(BPtr,BSpace,"%s=%s",
      name,
      ap->c_u.c_value);

    (*BPtr)++; /* Need to start the next string after the null */
    (*BSpace)--;
    }

  return;
  }





void gen_opsys(

  char  *name,
  char **BPtr,
  int   *BSpace)

  {
  struct config  *ap;

  ap = rm_search(config_array,name);

  if (ap != NULL)
    {
    MUSNPrintF(BPtr,BSpace,"%s=%s",
      name,
      ap->c_u.c_value);

    (*BPtr)++; /* Need to start the next string after the null */
    (*BSpace)--;
    }

  return;
  }





void gen_jdata(

  char  *name,
  char **BPtr,
  int   *BSpace)

  {
  if (TORQUE_JData[0] != '\0')
    {
    MUSNPrintF(BPtr,BSpace,"%s=%s",
      name,
      TORQUE_JData);

    (*BPtr)++; /* Need to start the next string after the null */
    (*BSpace)--;
    }
  return;
  }





void gen_gres(

  char  *name,
  char **BPtr,
  int   *BSpace)

  {

  char  *value;

  value = reqgres(NULL);

  if (value != NULL)
    {
    MUSNPrintF(BPtr, BSpace, "%s=%s",
                name,
                value);
    (*BPtr)++; /* Need to start the next string after the null */
    (*BSpace)--;
    }

  return;
  }    /* END gen_gres() */






void gen_gen(

  char  *name,
  char **BPtr,
  int   *BSpace)

  {
  struct config  *ap;
  char  *value;
  char  *ptr;

  ap = rm_search(config_array,name);

  if (ap != NULL)
    {
    ptr = conf_res(ap->c_u.c_value, NULL);

    if (ptr && *ptr)
      {
      MUSNPrintF(BPtr,BSpace,"%s=%s",
        name,
        ptr);

      (*BPtr)++; /* Need to start the next string after the null */
      (*BSpace)--;
      }
    }
  else
    {
    value = dependent(name, NULL);

    if (value == NULL)
      {
      /* value not set (attribute required) */

      MUSNPrintF(BPtr,BSpace,"%s=? %d",
        name,
        rm_errno);

      (*BPtr)++; /* Need to start the next string after the null */
      (*BSpace)--;
      }
    else if (value[0] == '\0')
      {
      /* value not set (attribute optional) */
      }
    else
      {
      MUSNPrintF(BPtr, BSpace, "%s=%s",
                 name,
                 value);
      (*BPtr)++; /* Need to start the next string after the null */
      (*BSpace)--;
      }
    } /* else if (ap) */

  return;
  }   /* END gen_gen() */

typedef void (*gen_func_ptr)(char *, char **, int *);

typedef struct stat_record
  {
  char *name;
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
  {NULL,          NULL}
  };


/*
 * Function to initialize the Nvidia nvml api
 */
#if defined(NVIDIA_GPUS) && defined(NVML_API)
void log_nvml_error(
  nvmlReturn_t  rc,
  char*         gpuid,
  char*         id)
  {

  switch (rc)
    {
    case NVML_SUCCESS:
      if (LOGLEVEL >= 3)
        {
        log_err(
          PBSE_RMSYSTEM,
          id,
          "Successful");
        }
      break;
    case NVML_ERROR_ALREADY_INITIALIZED:
      if (LOGLEVEL >= 3)
        {
        log_err(
          PBSE_RMSYSTEM,
          id,
          "Already initialized");
        }
      break;
    case NVML_ERROR_NO_PERMISSION:
      if (LOGLEVEL >= 1)
        {
        log_err(
          PBSE_RMSYSTEM,
          id,
          "No permission");
        }
      break;
    case NVML_ERROR_INVALID_ARGUMENT:
      if (LOGLEVEL >= 1)
        {
        log_err(
          PBSE_RMSYSTEM,
          id,
          "NVML invalid argument");
        }
      break;
    case NVML_ERROR_NOT_FOUND:
      if (LOGLEVEL >= 1)
        {
        sprintf(log_buffer, "NVML device %s not found",
          (gpuid != NULL) ? gpuid : "NULL");
        log_err(
          PBSE_RMSYSTEM,
          id,
          log_buffer);
        }
      break;
    case NVML_ERROR_NOT_SUPPORTED:
      if (LOGLEVEL >= 1)
        {
        sprintf(log_buffer, "NVML device %s not supported",
          (gpuid != NULL) ? gpuid : "NULL");
        log_err(
          PBSE_RMSYSTEM,
          id,
          log_buffer);
        }
      break;
    case NVML_ERROR_UNKNOWN:
      if (LOGLEVEL >= 1)
        {
        log_err(
          PBSE_RMSYSTEM,
          id,
          "Unknown error");
        }
      break;
    default:
      if (LOGLEVEL >= 1)
        {
        sprintf(log_buffer, "Unexpected error code %d",
          rc);
        log_err(
          PBSE_RMSYSTEM,
          id,
          log_buffer);
        }
      break;
    }
  }
#endif  /* NVIDIA_GPUS and NVML_API */



/*
 * Function to initialize the Nvidia nvml api
 */
#if defined(NVIDIA_GPUS) && defined(NVML_API)
int init_nvidia_nvml()
  {
  static char id[] = "init_nvidia_nvml";

  nvmlReturn_t  rc;
  unsigned int      device_count;

  rc = nvmlInit();

  if (rc == NVML_SUCCESS)
    {
    rc = nvmlDeviceGetCount(&device_count);
    if (rc == NVML_SUCCESS)
      {
      if ((int)device_count > 0)
        return (TRUE);

      sprintf(log_buffer,"No Nvidia gpus detected\n");
      log_ext(-1, id, log_buffer, LOG_DEBUG);

      /* since we detected no gpus, shut down nvml */

      shut_nvidia_nvml();

      return (FALSE);
      }
    }

  log_nvml_error (rc, NULL, id);

  return (FALSE);
  }
#endif  /* NVIDIA_GPUS and NVML_API */



/*
 * Function to shutdown the Nvidia nvml api
 */
#if defined(NVIDIA_GPUS) && defined(NVML_API)
int shut_nvidia_nvml()
  {
  static char id[] = "shut_nvidia_nvml";

  nvmlReturn_t  rc;

  if (!use_nvidia_gpu)
    return (TRUE);

  rc = nvmlShutdown();

  if (rc == NVML_SUCCESS)
    return (TRUE);

  log_nvml_error (rc, NULL, id);

  return (FALSE);
  }
#endif  /* NVIDIA_GPUS and NVML_API */



/*
 * Function to get the NVML device handle
 */

#if defined(NVIDIA_GPUS) && defined(NVML_API)
nvmlDevice_t get_nvml_device_handle(
  char *gpuid)
  {
  static char id[] = "get_nvml_device_handle";

  nvmlReturn_t      rc;
  nvmlDevice_t      device_hndl;
  char             *ptr;
  unsigned int      index;

  /* if gpuid contains a : then try to get the device handle by pci bus id */

  ptr = strchr(gpuid, ':');
  if (ptr != NULL)
    {
    rc = nvmlDeviceGetHandleByPciBusId(gpuid, &device_hndl);
    }
  else
    {
    /* try to get the device handle by index */

    index = atoi(gpuid);
    rc = nvmlDeviceGetHandleByIndex(index, &device_hndl);
    }

  if (rc == NVML_SUCCESS)
    return (device_hndl);

  log_nvml_error (rc, gpuid, id);

  return (NULL);

  }
#endif  /* NVIDIA_GPUS and NVML_API */


#ifdef NVIDIA_GPUS
/*
 * Function to determine if the nvidia kernel module is loaded
 */
static int check_nvidia_module_loaded()
  {
  static char id[] = "check_nvidia_module_loaded";
  char line[4096];
  FILE *file;

  file = fopen("/proc/modules", "r");
  if (!file)
    {
    if (LOGLEVEL >= 3)
      {
      log_err(
        errno,
        id,
        "Failed to read /proc/modules");
      }
    return(FALSE);
    }

  while (fgets(line, sizeof(line), file))
    {
    char *tok = strtok(line, " \t");

    if (tok)
      {
      if (strcmp(tok, "nvidia") == 0)
        {
        fclose(file);
        return(TRUE);
        }
      }
    }

  if (LOGLEVEL >= 3)
    {
    log_err(
      PBSE_RMSYSTEM,
      id,
      "No Nvidia driver loaded");
    }

  fclose(file);
  return(FALSE);
  }

#endif  /* NVIDIA_GPUS */



#ifdef NVIDIA_GPUS
/*
 * Function to get the nvidia driver version
 */
static int check_nvidia_version_file()
  {
  static char id[] = "check_nvidia_version_file";
  char line[4096];
  FILE *file;

  /* if file does not exist then version is too old */
  file = fopen("/proc/driver/nvidia/version", "r");
  if (!file)
    {
    if (LOGLEVEL >= 3)
      {
      log_err(
        PBSE_RMSYSTEM,
        id,
        "No Nvidia driver info available. Driver not supported?");
      }
    return(FALSE);
    }

  while (fgets(line, sizeof(line), file))
    {
    char *tok;

    if (strncmp(line, "NVRM", 4) == 0)
      {
      if (LOGLEVEL >= 3)
        {
        sprintf(log_buffer,"Nvidia driver info: %s\n", line);
        log_ext(-1, id, log_buffer, LOG_DEBUG);
        }
      tok = strstr(line, "Kernel Module");
      if (tok)
        {
        tok += 13;
        MOMNvidiaDriverVersion = atoi(tok);
        if (MOMNvidiaDriverVersion >= 260)
          {
          fclose(file);
          return(TRUE);
          }
        break;
        }
      }
    }

  fclose(file);
  return(FALSE);
  }

#endif  /* NVIDIA_GPUS */



/*
 * Function to determine if nvidia-smi is setup correctly
 */
#ifdef NVIDIA_GPUS
int check_nvidia_setup()
  {
  static char id[] = "check_nvidia_setup";

  int  rc;
  static int check_setup = TRUE;
  static int nvidia_setup_is_ok = FALSE;

  /* Check the setup for the nvidia gpus */

  if (check_setup)
    {
    char *pathEnv;

    /* only check the setup once */
    check_setup = FALSE;

    /* check if the nvidia module is loaded in */

    if (!check_nvidia_module_loaded())
      {
      return (FALSE);
      }

    /* see if we can get the nvidia driver version */

    if (!check_nvidia_version_file())
      {
      return (FALSE);
      }

#ifdef NVML_API
    nvidia_setup_is_ok = TRUE;
#else
    /* Get the PATH environment variable so we can see
     * if the nvidia-smi executable is in the execution path
     */

    pathEnv = getenv("PATH");

    if (pathEnv == NULL)
      {
      if (LOGLEVEL >= 3)
        {
        log_err(
          PBSE_RMSYSTEM,
          id,
          "cannot get PATH");
        }
      return(FALSE);
      }

    /* We have the PATH, now find the nvidia-smi executable */
    rc = find_file(pathEnv, "nvidia-smi");
    if (rc == FALSE)
      {
      if (LOGLEVEL >= 3)
        {
        log_err(
          PBSE_RMSYSTEM,
          id,
          "cannot find nvidia-smi in PATH");
        }
      return(FALSE);
      }
    nvidia_setup_is_ok = TRUE;
#endif  /* NVML_API */
    }
  return (nvidia_setup_is_ok);
  }
#endif  /* NVIDIA_GPUS */


/*
 * Function to collect nvidia-smi data
 */

#ifdef NVIDIA_GPUS
static char *gpus(
  char *buffer,
  int  buffer_size)
  {
  static char id[] = "gpus";

  FILE *fd;
  char *ptr; /* pointer to the current place to copy data into munge_buf */
  int  bytes_read;
  int  total_bytes_read = 0;
  char buf[RETURN_STRING_SIZE];
  char cmdbuf[101];

  if (!check_nvidia_setup())
    {
    return (FALSE);
    }

  if (MOMNvidiaDriverVersion >= 270)
    {
    sprintf(cmdbuf, "nvidia-smi -q -x 2>&1");
    }
  else /* 260 driver */
    {
    sprintf(cmdbuf, "nvidia-smi -a -x 2>&1");
    }

  if (LOGLEVEL >= 7)
    {
    sprintf(log_buffer,"%s: GPU cmd issued: %s\n", id, cmdbuf);
    log_ext(-1, id, log_buffer, LOG_DEBUG);
    }

	if ((fd = popen(cmdbuf, "r")) != NULL)
		{
    memset(buffer, 0, buffer_size);
    ptr = buffer;
    do
      {
      bytes_read = fread(buf, sizeof(char), MUNGE_SIZE, fd);
      if(bytes_read > 0)
        {
        total_bytes_read += bytes_read;
        memcpy(ptr, buf, bytes_read);
        ptr += bytes_read;
        }
      }while(bytes_read > 0);

      pclose(fd);

      if(bytes_read == -1)
        {
        /* read failed */
        if (LOGLEVEL >= 0)
          {
          sprintf(log_buffer, "error reading popen pipe");

          log_err(
            PBSE_RMSYSTEM,
            id,
            log_buffer);
          }
        return(NULL);
        }
		}
  else
    {
    if (LOGLEVEL >= 0)
      {
      sprintf(log_buffer, "error %d (%s) on popen", errno, strerror(errno));

      log_err(
        PBSE_RMSYSTEM,
        id,
        log_buffer);
      }
    return(NULL);
    }

  return(buffer);
  }
#endif  /* NVIDIA_GPUS */


/*
 * Function to collect gpu modes
 */

#ifdef NVIDIA_GPUS
static int gpumodes(
  int  buffer[],
  int  buffer_size)
  {
  static char id[] = "gpumodes";

  FILE *fd;
  char *ptr; /* pointer to the current place to copy data into buf */
  char buf[201];
  int  idx;
  int  gpuid;
  int  gpumode;

  if (!check_nvidia_setup())
    {
    return (FALSE);
    }

  for (idx=0; idx<buffer_size; idx++)
    {
    buffer[idx] = -1;
    }

  /* this only works for Nvidia driver version 260 */

  if (LOGLEVEL >= 7)
    {
    sprintf(log_buffer,"%s: GPU cmd issued: %s\n", id, "nvidia-smi -s 2>&1");
    log_ext(-1, id, log_buffer, LOG_DEBUG);
    }

	if ((fd = popen("nvidia-smi -s 2>&1", "r")) != NULL)
		{
    while (!feof(fd))
      {
      if (fgets(buf, 200, fd))
        {
        ptr = buf;
        ptr = strstr(ptr, "GPU");
        if (ptr)
          {
          ptr += 4;
          gpuid = atoi(ptr);

          ptr = strchr(ptr, ':');
          if (ptr)
            {
            ptr++;
            gpumode = atoi(ptr);
            }
          buffer[gpuid] = gpumode;
          }
        }
      }
    pclose(fd);
		}
  else
    {
    if (LOGLEVEL >= 0)
      {
      sprintf(log_buffer, "error %d (%s) on popen", errno, strerror(errno));

      log_err(
        PBSE_RMSYSTEM,
        id,
        log_buffer);
      }
    return(FALSE);
    }
  return(TRUE);
  }
#endif  /* NVIDIA_GPUS */


/*
 * Function to set gpu mode
 */

#ifdef NVIDIA_GPUS
int setgpumode(
  char *gpuid,
  int   gpumode)
  {
  static char id[] = "setgpumode";

#ifdef NVML_API
  nvmlReturn_t      rc;
  nvmlComputeMode_t compute_mode;
  nvmlDevice_t      device_hndl;

  if (!check_nvidia_setup())
    {
    return (FALSE);
    }

  switch (gpumode)
    {
    case gpu_normal:
      compute_mode = NVML_COMPUTEMODE_DEFAULT;
      break;
    case gpu_exclusive_thread:
      compute_mode = NVML_COMPUTEMODE_EXCLUSIVE_THREAD;
      break;
    case gpu_prohibited:
      compute_mode = NVML_COMPUTEMODE_PROHIBITED;
      break;
    case gpu_exclusive_process:
      compute_mode = NVML_COMPUTEMODE_EXCLUSIVE_PROCESS;
      break;
    default:
      if (LOGLEVEL >= 1)
        {
        sprintf(log_buffer, "Unexpected compute mode %d",
          rc);
        log_err(
          PBSE_RMSYSTEM,
          id,
          log_buffer);
        }
      return (FALSE);
    }

  /* get the device handle */

  device_hndl = get_nvml_device_handle(gpuid);

  if (device_hndl != NULL)
    {
	  if (LOGLEVEL >= 7)
	    {
      sprintf(log_buffer, "changing to mode %d for gpu %s",
			        gpumode,
			        gpuid);

      log_ext(-1, id, log_buffer, LOG_DEBUG);
	    }

    rc = nvmlDeviceSetComputeMode(device_hndl, compute_mode);

    if (rc == NVML_SUCCESS)
      return (TRUE);

    log_nvml_error (rc, gpuid, id);
    }

  return(FALSE);

#else
  FILE *fd;
  char buf[301];

  if (!check_nvidia_setup())
    {
    return (FALSE);
    }

  /* build command to be issued */

  if (MOMNvidiaDriverVersion == 260)
    {
    sprintf(buf, "nvidia-smi -g %s -c %d 2>&1",
      gpuid,
      gpumode);
    }
  else /* 270 or greater driver */
    {
    sprintf(buf, "nvidia-smi -i %s -c %d 2>&1",
      gpuid,
      gpumode);
    }

  if (LOGLEVEL >= 7)
    {
    sprintf(log_buffer,"%s: GPU cmd issued: %s\n", id, buf);
    log_ext(-1, id, log_buffer, LOG_DEBUG);
    }

	if ((fd = popen(buf, "r")) != NULL)
		{
    while (!feof(fd))
      {
      if (fgets(buf, 300, fd))
        {
        int len = strlen(buf);
        /* bypass blank lines */
        if ((len == 1 ) && (buf[0] == '\n'))
          {
          continue;
          }
        /* for 270 and above we need to check the return string to see if it went okay */
        /* 260 driver does not return anything on success */

        if ((MOMNvidiaDriverVersion >= 270) &&
            ((memcmp(buf, "Set compute mode to", 19) == 0) ||
            (memcmp(buf, "Compute mode is already set to", 30) == 0)))
          {
          break;
          }
        if (LOGLEVEL >= 7)
          {
          sprintf(
            log_buffer,
            "nvidia-smi gpu change mode returned: %s",
            buf);
          log_ext(-1, id, log_buffer, LOG_INFO);
          }
        pclose(fd);
        return(FALSE);
        }
      }
    pclose(fd);
		}
  else
    {
    if (LOGLEVEL >= 0)
      {
      sprintf(log_buffer, "error %d (%s) on popen", errno, strerror(errno));

      log_err(
        PBSE_RMSYSTEM,
        id,
        log_buffer);
      }
    return(FALSE);
    }

  return(TRUE);
#endif  /* NVML_API */
  }
#endif  /* NVIDIA_GPUS */



/*
 * Function to reset gpu ecc count
 */

#ifdef NVIDIA_GPUS
int resetgpuecc(
  char *gpuid,
  int   reset_perm,
  int   reset_vol)
  {
  static char id[] = "resetgpuecc";

#ifdef NVML_API
  nvmlReturn_t      rc;
  nvmlEccBitType_t  counter_type;
  nvmlDevice_t      device_hndl;

  if (!check_nvidia_setup())
    {
    return (FALSE);
    }

  if (reset_perm == 1)
    {
    /* reset ecc counts */
    counter_type = NVML_AGGREGATE_ECC;
    }
  else if (reset_vol == 1)
    {
    /* reset volatile ecc counts */
    counter_type = NVML_AGGREGATE_ECC;
    }

  /* get the device handle */

  device_hndl = get_nvml_device_handle(gpuid);

  if (device_hndl != NULL)
    {
	  if (LOGLEVEL >= 7)
	    {
		  sprintf(log_buffer, "reseting error count %d-%d for gpu %s",
						  reset_perm,
						  reset_vol,
						  gpuid);

		  log_ext(-1, id, log_buffer, LOG_DEBUG);
	    }

    rc = nvmlDeviceClearEccErrorCounts(device_hndl, counter_type);

    if (rc == NVML_SUCCESS)
      return (TRUE);

    log_nvml_error (rc, gpuid, id);
    }

  return(FALSE);

#else
  FILE *fd;
  char buf[301];

  if (!check_nvidia_setup())
    {
    return (FALSE);
    }

  /* build command to be issued */

  if (MOMNvidiaDriverVersion == 260)
    {
    sprintf(buf, "nvidia-smi -g %s",
      gpuid);

    if (reset_perm == 1)
      {
      /* reset permanent ecc counts */
      strcat (buf, " -p");
      }

    if (reset_vol == 1)
      {
      /* reset volatile ecc counts */
      strcat (buf, " -v");
      }
    }
  else /* 270 or greater driver */
    {
    sprintf(buf, "nvidia-smi -i %s",
      gpuid);

    /* 270 can currently reset only 1 at a time */

    if (reset_perm == 1)
      {
      /* reset ecc counts */
      strcat (buf, " -p 1");
      }
    else if (reset_vol == 1)
      {
      /* reset volatile ecc counts */
      strcat (buf, " -p 0");
      }
    }

  strcat(buf, " 2>&1");

  if (LOGLEVEL >= 7)
    {
    sprintf(log_buffer,"%s: GPU cmd issued: %s\n", id, buf);
    log_ext(-1, id, log_buffer, LOG_DEBUG);
    }

	if ((fd = popen(buf, "r")) != NULL)
		{
    while (!feof(fd))
      {
      if (fgets(buf, 300, fd))
        {
        int len = strlen(buf);
        /* bypass blank lines */
        if ((len == 1 ) && (buf[0] == '\n'))
          {
          continue;
          }
        /* for 270 we need to check the return string to see if it went okay */
        /* 260 driver does not return anything on success */

        if ((MOMNvidiaDriverVersion >= 270) &&
            ((memcmp(buf, "Reset volatile ECC errors to zero", 33) == 0) ||
            (memcmp(buf, "Reset aggregate ECC errors to zero", 34) == 0)))
          {
          break;
          }
        if (LOGLEVEL >= 7)
          {
          sprintf(
            log_buffer,
            "nvidia-smi gpu reset ecc returned: %s",
            buf);
          log_ext(-1, id, log_buffer, LOG_INFO);
          }
        pclose(fd);
        return(FALSE);
        }
      }
    pclose(fd);
		}
  else
    {
    if (LOGLEVEL >= 0)
      {
      sprintf(log_buffer, "error %d (%s) on popen", errno, strerror(errno));

      log_err(
        PBSE_RMSYSTEM,
        id,
        log_buffer);
      }
    return(FALSE);
    }
  return(TRUE);
#endif  /* NVML_API */
  }
#endif  /* NVIDIA_GPUS */





#ifdef NVIDIA_GPUS
/*
 * uses the gpu_flags to determine what to set up for job
 *
 * @param pjob - the job to set up gpus for
 * @return PBSE_NONE if success, error code otherwise
 */
int setup_gpus_for_job(

  job  *pjob) /* I */

  {
  static char *id = "setup_gpus_for_job";

  char *gpu_str;
  char *ptr;
  char  tmp_str[PBS_MAXHOSTNAME + 10];
  int   gpu_flags = 0;
  char  gpu_id[30];
  int   gpu_mode = -1;

  /* if node does not have Nvidia recognized driver version then forget it */

  if (MOMNvidiaDriverVersion < 260)
    return(PBSE_NONE);

  /* if there are no gpus, do nothing */
  if ((pjob->ji_wattr[JOB_ATR_exec_gpus].at_flags & ATR_VFLAG_SET) == 0)
    return(PBSE_NONE);

  /* if there are no gpu flags, do nothing */
  if ((pjob->ji_wattr[JOB_ATR_gpu_flags].at_flags & ATR_VFLAG_SET) == 0)
    return(PBSE_NONE);

  gpu_str = pjob->ji_wattr[JOB_ATR_exec_gpus].at_val.at_str;

  if (gpu_str == NULL)
    return(PBSE_NONE);

  gpu_flags = pjob->ji_wattr[JOB_ATR_gpu_flags].at_val.at_long;

  if (LOGLEVEL >= 7)
    {
		sprintf(log_buffer, "job %s has exec_gpus %s gpu_flags %d",
						pjob->ji_qs.ji_jobid,
						gpu_str,
						gpu_flags);

	  log_ext(-1, id, log_buffer, LOG_DEBUG);
    }

  /* traverse the gpu_str to see what gpus we have assigned */

  strcpy(tmp_str, mom_host);
  strcat(tmp_str, "-gpu/");

  ptr = strstr(gpu_str, tmp_str);
  
  if (ptr == NULL)
    {
    /* might be fully qualified host name */
    strcpy(tmp_str, mom_host);
    ptr = strchr(tmp_str, '.');
    if (ptr != NULL)
      ptr[0] = '\0';

    strcat(tmp_str, "-gpu/");
    ptr = strstr(gpu_str, tmp_str);
    }

  while(ptr != NULL)
    {
    ptr = strchr(ptr, '/');
    if (ptr != NULL)
      {
      ptr++;
      sprintf(gpu_id,"%d",atoi(ptr));

      /* do we need to reset volatile error counts on gpu */
      if (gpu_flags >= 1000)
        {
        if (LOGLEVEL >= 7)
          {
    		  sprintf(log_buffer, "job %s reseting gpuid %s volatile error counts",
						  pjob->ji_qs.ji_jobid,
						  gpu_id);

	        log_ext(-1, id, log_buffer, LOG_DEBUG);
          }

        resetgpuecc(gpu_id, 0, 1);
        }

      gpu_mode = gpu_flags;
      if (gpu_mode  >= 1000)
        {
        gpu_mode -= 1000;
        }

      /* do we need to change modes on gpu */
      if (nvidia_gpu_modes[atoi(ptr)] != gpu_mode)
        {
        if (LOGLEVEL >= 7)
          {
    		  sprintf(log_buffer, "job %s change to mode %d for gpuid %s",
					    pjob->ji_qs.ji_jobid,
					    gpu_mode,
					    gpu_id);

          log_ext(-1, id, log_buffer, LOG_DEBUG);
          }

        setgpumode(gpu_id, gpu_mode);
        }

      ptr = strstr(ptr, tmp_str);
      }
    }

  /* do we need to change mode on gpu */


  return(PBSE_NONE);
  } /* END setup_gpus_for_job() */
#endif  /* NVIDIA_GPUS */







/**
 * generate_server_status
 *
 */

void generate_server_status(

  char *buffer,
  int   buffer_size)

  {
  int   i;
  char *BPtr = buffer;
  int   BSpace = buffer_size;

  for (i = 0;stats[i].name != NULL;i++)
    {
    alarm(alarm_time);

    if (stats[i].func)
      {
      (stats[i].func)(stats[i].name, &BPtr, &BSpace);
      }

    alarm(0);
    }  /* END for (i) */

  TORQUE_JData[0] = '\0';
  return;
  }  /* END generate_server_status */



/*
 * Function to collect gpu statuses to be sent to server. (Currently Nvidia only)
 */
#ifdef NVML_API

void generate_server_gpustatus_nvml(
  char *buffer,
  int  buffer_size)
  {
  static char id[] = "generate_server_gpustatus_nvml";

  nvmlReturn_t      rc;
  unsigned int      device_count;
  unsigned int      tmpint;
  int               idx;
  nvmlDevice_t      device_hndl;
  nvmlPciInfo_t     pci_info;
  nvmlMemory_t      mem_info;
  nvmlComputeMode_t comp_mode;
  nvmlEnableState_t ecc_mode, ecc_pend_mode, display_mode;
  nvmlUtilization_t util_info;
  unsigned long long ecc_counts;
  char              tmpbuf[1024+1];
  char              *outptr;
  int               len;

  if(buffer == NULL)
    {
    return;
    }

  if (!check_nvidia_setup())
    {
    return;
    }

  memset(buffer, 0, buffer_size);

   /* get timestamp to report */

  len = snprintf(tmpbuf, 100, "timestamp=%s",
    ctime(&time_now));
  strncpy(buffer, tmpbuf, len - 1);
  outptr = buffer;
  outptr += strlen(buffer) + 1;

  /* get the driver version to report */

  rc = nvmlSystemGetDriverVersion(tmpbuf, 1024);
  if (rc == NVML_SUCCESS)
    {
    strcat(outptr, "driver_ver=");
    strcat(outptr, tmpbuf);
    outptr += strlen(outptr) + 1;
    }
  else
    {
    log_nvml_error (rc, NULL, id);
    }

  /* get the device count */

  rc = nvmlDeviceGetCount(&device_count);
  if (rc != NVML_SUCCESS)
    {
    log_nvml_error (rc, NULL, id);
    return;
    }

  /* get the device handle for each gpu and report the data */

  for (idx = 0; idx < device_count; idx++)
    {
    rc = nvmlDeviceGetHandleByIndex(idx, &device_hndl);

    if (rc != NVML_SUCCESS)
      {
      log_nvml_error (rc, NULL, id);
      continue;
      }

    /* get the PCI info */

    rc = nvmlDeviceGetPciInfo(device_hndl, &pci_info);

    if (rc == NVML_SUCCESS)
      {

      strcat(outptr, "gpuid=");
      strcat(outptr, pci_info.busId);
      outptr += strlen(outptr) + 1;

      strcat(outptr, "gpu_pci_device_id=");
      snprintf(tmpbuf, 100, "%d", pci_info.pciDeviceId);
      strcat(outptr, tmpbuf);
      outptr += strlen(outptr) + 1;

      strcat(outptr, "gpu_pci_location_id=");
      strcat(outptr, pci_info.busId);
      outptr += strlen(outptr) + 1;
      }
    else
      {
      log_nvml_error (rc, NULL, id);
      }

    /* get the product name */

    rc = nvmlDeviceGetName(device_hndl, tmpbuf, 1024);

    if (rc == NVML_SUCCESS)
      {
      strcat(outptr, "gpu_product_name=");
      strcat(outptr, tmpbuf);
      outptr += strlen(outptr) + 1;
      }
    else
      {
      log_nvml_error (rc, NULL, id);
      }

    /* get the display mode */

    rc = nvmlDeviceGetDisplayMode(device_hndl, &display_mode);

    if (rc == NVML_SUCCESS)
      {
      strcat(outptr, "gpu_display=Enabled");
      outptr += strlen(outptr) + 1;
      }
    else if (rc == NVML_ERROR_INVALID_ARGUMENT)
      {
      strcat(outptr, "gpu_display=Disabled");
      outptr += strlen(outptr) + 1;
      }
    else
      {
      log_nvml_error (rc, NULL, id);
      }

    /* get the fan speed */

    rc = nvmlDeviceGetFanSpeed(device_hndl, &tmpint);

    if (rc == NVML_SUCCESS)
      {
      snprintf(tmpbuf, 20, "gpu_fan_speed=%d%%", tmpint);
      strcat(outptr, tmpbuf);
      outptr += strlen(outptr) + 1;
      }
    else
      {
      log_nvml_error (rc, NULL, id);
      }

    /* get the memory information */

    rc = nvmlDeviceGetMemoryInfo(device_hndl, &mem_info);

    if (rc == NVML_SUCCESS)
      {
      snprintf(tmpbuf, 50, "gpu_memory_total=%lld MB", (mem_info.total/(1024*1024)));
      strcat(outptr, tmpbuf);
      outptr += strlen(outptr) + 1;

      snprintf(tmpbuf, 50, "gpu_memory_used=%lld MB", (mem_info.used/(1024*1024)));
      strcat(outptr, tmpbuf);
      outptr += strlen(outptr) + 1;
      }
    else
      {
      log_nvml_error (rc, NULL, id);
      }

    /* get the compute mode */

    rc = nvmlDeviceGetComputeMode(device_hndl, &comp_mode);

    if (rc == NVML_SUCCESS)
      {
      strcat(outptr, "gpu_mode=");
      switch (comp_mode)
        {
          case NVML_COMPUTEMODE_DEFAULT:
            strcat(outptr, "Default");
            nvidia_gpu_modes[idx] = gpu_normal;
            break;
          case NVML_COMPUTEMODE_EXCLUSIVE_THREAD:
            strcat(outptr, "Exclusive_Thread");
            nvidia_gpu_modes[idx] = gpu_exclusive_thread;
            break;
          case NVML_COMPUTEMODE_PROHIBITED:
            strcat(outptr, "Prohibited");
            nvidia_gpu_modes[idx] = gpu_prohibited;
            break;
          case NVML_COMPUTEMODE_EXCLUSIVE_PROCESS:
            strcat(outptr, "Exclusive_Process");
            nvidia_gpu_modes[idx] = gpu_exclusive_process;
            break;
          default:
            strcat(outptr, "Unknown");
            nvidia_gpu_modes[idx] = -1;
            break;
        }
      outptr += strlen(outptr) + 1;
      }
    else
      {
      log_nvml_error (rc, NULL, id);
      }

    /* get the utilization rates */

    rc = nvmlDeviceGetUtilizationRates(device_hndl, &util_info);

    if (rc == NVML_SUCCESS)
      {
      len = snprintf(tmpbuf, 100, "gpu_utilization=%d%%", util_info.gpu);
      strcat(outptr, tmpbuf);
      outptr += strlen(outptr) + 1;

      len = snprintf(tmpbuf, 100, "gpu_memory_utilization=%d%%", util_info.memory);
      strcat(outptr, tmpbuf);
      outptr += strlen(outptr) + 1;
      }
    else
      {
      log_nvml_error (rc, NULL, id);
      }

    /* get the ECC mode */

    rc = nvmlDeviceGetEccMode(device_hndl, &ecc_mode, &ecc_pend_mode);

    if (rc == NVML_SUCCESS)
      {
      snprintf(tmpbuf, 50, "gpu_ecc_mode=%s",
        (ecc_mode == NVML_FEATURE_ENABLED) ? "Enabled" : "Disabled");
      strcat(outptr, tmpbuf);
      outptr += strlen(outptr) + 1;
      }
    else
      {
      log_nvml_error (rc, NULL, id);
      }

    /* get the single bit ECC errors */

    rc = nvmlDeviceGetTotalEccErrors(device_hndl, NVML_SINGLE_BIT_ECC,
        NVML_AGGREGATE_ECC, &ecc_counts);

    if (rc == NVML_SUCCESS)
      {
      snprintf(tmpbuf, 100, "gpu_single_bit_ecc_errors=%lld", ecc_counts);
      strcat(outptr, tmpbuf);
      outptr += strlen(outptr) + 1;
      }
    else
      {
      log_nvml_error (rc, NULL, id);
      }

    /* get the double bit ECC errors */

    rc = nvmlDeviceGetTotalEccErrors(device_hndl, NVML_DOUBLE_BIT_ECC,
        NVML_AGGREGATE_ECC, &ecc_counts);

    if (rc == NVML_SUCCESS)
      {
      snprintf(tmpbuf, 100, "gpu_double_bit_ecc_errors=%lld", ecc_counts);
      strcat(outptr, tmpbuf);
      outptr += strlen(outptr) + 1;
      }
    else
      {
      log_nvml_error (rc, NULL, id);
      }

    /* get the temperature */

    rc = nvmlDeviceGetTemperature(device_hndl, NVML_TEMPERATURE_GPU, &tmpint);

    if (rc == NVML_SUCCESS)
      {
      snprintf(tmpbuf, 25, "gpu_temperature=%d C", tmpint);
      strcat(outptr, tmpbuf);
      outptr += strlen(outptr) + 1;
      }
    else
      {
      log_nvml_error (rc, NULL, id);
      }

    }

  return;

  }
#endif  /* NVML_API */



/*
 * Function to collect gpu statuses to be sent to server. (Currently Nvidia only)
 */
#ifdef NVIDIA_GPUS

void generate_server_gpustatus_smi(
  char *buffer,
  int  buffer_size)
  {
  static char id[] = "generate_server_gpustatus_smi";

  char *dataptr, *outptr, *tmpptr1, *tmpptr2, *savptr;
  /* 
   * we hope we don't get more than 32 gpus on a node so we guess at how much
   * data might get returned from nvidia-smi. xml inflates return data.
   */
  char gpu_string[MAX_GPUS * 3000];
  int  gpu_modes[MAX_GPUS];
  int  have_modes = FALSE;
  int  gpuid = -1;
  mxml_t *EP;
  char *Tail;
  char Emsg[MAXLINE];

  if(buffer == NULL)
    {
    return;
    }

  memset(buffer, 0, buffer_size);

  dataptr = gpus(gpu_string, sizeof(gpu_string));

  if(dataptr == NULL)
    {
    return;
    }

  /* move past the php code*/
  dataptr = strstr(gpu_string, "<timestamp>");
  if(dataptr)
    {
    MXMLFromString(&EP, dataptr, &Tail, Emsg, sizeof(Emsg));
    strcpy(buffer, "timestamp=");
    strcat(buffer, EP->Val);
    outptr = buffer;
    outptr += strlen(buffer) + 1;
    MXMLDestroyE(&EP);
    }
  else
    {
      buffer[0] = 0;
      return;
    }

  dataptr = strstr(gpu_string, "<driver_version>");
  if(dataptr)
    {
    MXMLFromString(&EP, dataptr, &Tail, Emsg, sizeof(Emsg));
    strcat(outptr, "driver_ver=");
    strcat(outptr, EP->Val);
    outptr += strlen(outptr) + 1;
    MXMLDestroyE(&EP);
    }
  else
    {
    /* cannot determine driver version */
    strcat(outptr, "driver_ver=UNKNOWN");
    outptr += strlen(outptr) + 1;
    return;
    }

  while ((dataptr = strstr(dataptr, "<gpu id=")) != NULL)
    {
      if(dataptr)
        {
        MXMLFromString(&EP, dataptr, &Tail, Emsg, sizeof(Emsg));
        strcat(outptr, "gpuid=");
        strcat(outptr, EP->AVal[0]);
        outptr += strlen(outptr) + 1;
        if (MOMNvidiaDriverVersion == 260)
          {
          gpuid = atoi(EP->AVal[0]);
          }
        else
          {
          gpuid++;
          }
        MXMLDestroyE(&EP);

        if (MOMNvidiaDriverVersion == 260)
          {
          /* Get and add mode rules information for driver 260 */

          if (!have_modes)
            {
            have_modes = gpumodes(gpu_modes, 32);
            }

          strcat(outptr, "gpu_mode=");
          switch (gpu_modes[gpuid])
            {
              case 0:
                strcat(outptr, "Normal");
                nvidia_gpu_modes[gpuid] = gpu_normal;
                break;
              case 1:
                strcat(outptr, "Exclusive");
                nvidia_gpu_modes[gpuid] = gpu_exclusive_thread;
                break;
              case 2:
                strcat(outptr, "Prohibited");
                nvidia_gpu_modes[gpuid] = gpu_prohibited;
                break;
              default:
                strcat(outptr, "None");
                nvidia_gpu_modes[gpuid] = -1;
                break;
            }
            outptr += strlen(outptr) + 1;

          savptr = dataptr;
          dataptr = strstr(dataptr, "<prod_name>");
          if(dataptr)
            {
            MXMLFromString(&EP, dataptr, &Tail, Emsg, sizeof(Emsg));
            strcat(outptr, "gpu_product_name=");
            strcat(outptr, EP->Val);
            outptr += strlen(outptr) + 1;
            MXMLDestroyE(&EP);
            }
          else
            {
            dataptr = savptr;
            }

          savptr = dataptr;
          dataptr = strstr(dataptr, "<pci_device_id>");
          if(dataptr)
            {
            MXMLFromString(&EP, dataptr, &Tail, Emsg, sizeof(Emsg));
            strcat(outptr, "gpu_pci_device_id=");
            strcat(outptr, EP->Val);
            outptr += strlen(outptr) + 1;
            MXMLDestroyE(&EP);
            }
          else
            {
            dataptr = savptr;
            }

          savptr = dataptr;
          dataptr = strstr(dataptr, "<pci_location_id>");
          if(dataptr)
            {
            MXMLFromString(&EP, dataptr, &Tail, Emsg, sizeof(Emsg));
            strcat(outptr, "gpu_pci_location_id=");
            strcat(outptr, EP->Val);
            outptr += strlen(outptr) + 1;
            MXMLDestroyE(&EP);
            }
          else
            {
            dataptr = savptr;
            }

          savptr = dataptr;
          dataptr = strstr(dataptr, "<display>");
          if(dataptr)
            {
            MXMLFromString(&EP, dataptr, &Tail, Emsg, sizeof(Emsg));
            strcat(outptr, "gpu_display=");
            strcat(outptr, EP->Val);
            outptr += strlen(outptr) + 1;
            MXMLDestroyE(&EP);
            }
          else
            {
            dataptr = savptr;
            }

          savptr = dataptr;
          dataptr = strstr(dataptr, "<temp>");
          if(dataptr)
            {
            MXMLFromString(&EP, dataptr, &Tail, Emsg, sizeof(Emsg));
            strcat(outptr, "gpu_temperature=");
            strcat(outptr, EP->Val);
            outptr += strlen(outptr) + 1;
            MXMLDestroyE(&EP);
            }
          else
            {
            dataptr = savptr;
            }

          savptr = dataptr;
          dataptr = strstr(dataptr, "<fan_speed>");
          if(dataptr)
            {
            MXMLFromString(&EP, dataptr, &Tail, Emsg, sizeof(Emsg));
            strcat(outptr, "gpu_fan_speed=");
            strcat(outptr, EP->Val);
            outptr += strlen(outptr) + 1;
            MXMLDestroyE(&EP);
            }
          else
            {
            dataptr = savptr;
            }

          savptr = dataptr;
          dataptr = strstr(dataptr, "<gpu_util>");
          if(dataptr)
            {
            MXMLFromString(&EP, dataptr, &Tail, Emsg, sizeof(Emsg));
            strcat(outptr, "gpu_utilization=");
            strcat(outptr, EP->Val);
            outptr += strlen(outptr) + 1;
            MXMLDestroyE(&EP);
            }
          else
            {
            dataptr = savptr;
            }

          dataptr = strstr(dataptr, "<memory_util>");
          if(dataptr)
            {
            MXMLFromString(&EP, dataptr, &Tail, Emsg, sizeof(Emsg));
            strcat(outptr, "gpu_memory_utilization=");
            strcat(outptr, EP->Val);
            outptr += strlen(outptr) + 1;
            MXMLDestroyE(&EP);
            }
          else
            {
            dataptr = savptr;
            }

          dataptr = strstr(dataptr, "<aggregate_ecc_errors>");
          if(dataptr)
            {
            tmpptr1 = strstr(dataptr, "<single_bit>");
            if (tmpptr1)
              {
              tmpptr1 = strstr(tmpptr1, "<total>");
              MXMLFromString(&EP, tmpptr1, &Tail, Emsg, sizeof(Emsg));
              strcat(outptr, "gpu_single_bit_ecc_errors=");
              strcat(outptr, EP->Val);
              outptr += strlen(outptr) + 1;
              MXMLDestroyE(&EP);
              }

            tmpptr1 = strstr(dataptr, "<double_bit>");
            if (tmpptr1)
              {
              tmpptr1 = strstr(tmpptr1, "<total>");
              MXMLFromString(&EP, tmpptr1, &Tail, Emsg, sizeof(Emsg));
              strcat(outptr, "gpu_double_bit_ecc_errors=");
              strcat(outptr, EP->Val);
              outptr += strlen(outptr) + 1;
              MXMLDestroyE(&EP);
              }
            }
          else
            {
            dataptr = savptr;
            }

          } /* end (MOMNvidiaDriverVersion == 260) */

        else if (MOMNvidiaDriverVersion >= 270)
          {
          savptr = dataptr;
          dataptr = strstr(dataptr, "<product_name>");
          if(dataptr)
            {
            MXMLFromString(&EP, dataptr, &Tail, Emsg, sizeof(Emsg));
            strcat(outptr, "gpu_product_name=");
            strcat(outptr, EP->Val);
            outptr += strlen(outptr) + 1;
            MXMLDestroyE(&EP);
            }
          else
            {
            dataptr = savptr;
            }

          savptr = dataptr;
          dataptr = strstr(dataptr, "<display_mode>");
          if(dataptr)
            {
            MXMLFromString(&EP, dataptr, &Tail, Emsg, sizeof(Emsg));
            strcat(outptr, "gpu_display=");
            strcat(outptr, EP->Val);
            outptr += strlen(outptr) + 1;
            MXMLDestroyE(&EP);
            }
          else
            {
            dataptr = savptr;
            }

          savptr = dataptr;
          dataptr = strstr(dataptr, "<pci_device_id>");
          if(dataptr)
            {
            MXMLFromString(&EP, dataptr, &Tail, Emsg, sizeof(Emsg));
            strcat(outptr, "gpu_pci_device_id=");
            strcat(outptr, EP->Val);
            outptr += strlen(outptr) + 1;
            MXMLDestroyE(&EP);
            }
          else
            {
            dataptr = savptr;
            }

          savptr = dataptr;
          dataptr = strstr(dataptr, "<pci_bus_id>");
          if(dataptr)
            {
            MXMLFromString(&EP, dataptr, &Tail, Emsg, sizeof(Emsg));
            strcat(outptr, "gpu_pci_location_id=");
            strcat(outptr, EP->Val);
            outptr += strlen(outptr) + 1;
            MXMLDestroyE(&EP);
            }
          else
            {
            dataptr = savptr;
            }

          savptr = dataptr;
          dataptr = strstr(dataptr, "<fan_speed>");
          if(dataptr)
            {
            MXMLFromString(&EP, dataptr, &Tail, Emsg, sizeof(Emsg));
            strcat(outptr, "gpu_fan_speed=");
            strcat(outptr, EP->Val);
            outptr += strlen(outptr) + 1;
            MXMLDestroyE(&EP);
            }
          else
            {
            dataptr = savptr;
            }

          dataptr = strstr(dataptr, "<memory_usage>");
          if(dataptr)
            {
            tmpptr1 = strstr(dataptr, "<total>");
            if (tmpptr1)
              {
              MXMLFromString(&EP, tmpptr1, &Tail, Emsg, sizeof(Emsg));
              strcat(outptr, "gpu_memory_total=");
              strcat(outptr, EP->Val);
              outptr += strlen(outptr) + 1;
              MXMLDestroyE(&EP);
              }

            tmpptr1 = strstr(dataptr, "<used>");
            if (tmpptr1)
              {
              MXMLFromString(&EP, tmpptr1, &Tail, Emsg, sizeof(Emsg));
              strcat(outptr, "gpu_memory_used=");
              strcat(outptr, EP->Val);
              outptr += strlen(outptr) + 1;
              MXMLDestroyE(&EP);
              }
            }
          else
            {
            dataptr = savptr;
            }

          savptr = dataptr;
          dataptr = strstr(dataptr, "<compute_mode>");
          if(dataptr)
            {
            MXMLFromString(&EP, dataptr, &Tail, Emsg, sizeof(Emsg));
            strcat(outptr, "gpu_mode=");
            strcat(outptr, EP->Val);
            outptr += strlen(outptr) + 1;
            if (EP->Val[0] == 'D') /* Default */
              {
              nvidia_gpu_modes[gpuid] = gpu_normal;
              }
            else if (EP->Val[0] == 'P') /* Prohibited */
              {
              nvidia_gpu_modes[gpuid] = gpu_prohibited;
              }
            else if (EP->Val[10] == 'T') /* Exclusive_Thread */
              {
              nvidia_gpu_modes[gpuid] = gpu_exclusive_thread;
              }
            else if (EP->Val[10] == 'P') /* Exclusive_Process */
              {
              nvidia_gpu_modes[gpuid] = gpu_exclusive_process;
              }
            else /* unknown */
              {
              nvidia_gpu_modes[gpuid] = -1;
              }
            MXMLDestroyE(&EP);
            }
          else
            {
            dataptr = savptr;
            }

          savptr = dataptr;
          dataptr = strstr(dataptr, "<utilization>");
          if(dataptr)
            {
            tmpptr1 = strstr(dataptr, "<gpu_util>");
            if (tmpptr1)
              {
              MXMLFromString(&EP, tmpptr1, &Tail, Emsg, sizeof(Emsg));
              strcat(outptr, "gpu_utilization=");
              strcat(outptr, EP->Val);
              outptr += strlen(outptr) + 1;
              MXMLDestroyE(&EP);
              }

            tmpptr1 = strstr(dataptr, "<memory_util>");
            if(tmpptr1)
              {
              MXMLFromString(&EP, tmpptr1, &Tail, Emsg, sizeof(Emsg));
              strcat(outptr, "gpu_memory_utilization=");
              strcat(outptr, EP->Val);
              outptr += strlen(outptr) + 1;
              MXMLDestroyE(&EP);
              }
            }
          else
            {
            dataptr = savptr;
            }

          dataptr = strstr(dataptr, "<ecc_mode>");
          if(dataptr)
            {
            tmpptr1 = strstr(dataptr, "<current_ecc>");
            if (tmpptr1)
              {
              MXMLFromString(&EP, tmpptr1, &Tail, Emsg, sizeof(Emsg));
              strcat(outptr, "gpu_ecc_mode=");
              strcat(outptr, EP->Val);
              outptr += strlen(outptr) + 1;
              MXMLDestroyE(&EP);
              }
            }
          else
            {
            dataptr = savptr;
            }

          dataptr = strstr(dataptr, "<ecc_errors>");
          if(dataptr)
            {
            tmpptr1 = strstr(dataptr, "<aggregate>");
            if (tmpptr1)
              {
              tmpptr2 = strstr(tmpptr1, "<single_bit>");
              if (tmpptr2)
                {
                tmpptr2 = strstr(tmpptr1, "<total>");
                MXMLFromString(&EP, tmpptr2, &Tail, Emsg, sizeof(Emsg));
                strcat(outptr, "gpu_single_bit_ecc_errors=");
                strcat(outptr, EP->Val);
                outptr += strlen(outptr) + 1;
                MXMLDestroyE(&EP);
                }

              tmpptr2 = strstr(tmpptr1, "<double_bit>");
              if (tmpptr2)
                {
                tmpptr2 = strstr(tmpptr1, "<total>");
                MXMLFromString(&EP, tmpptr2, &Tail, Emsg, sizeof(Emsg));
                strcat(outptr, "gpu_double_bit_ecc_errors=");
                strcat(outptr, EP->Val);
                outptr += strlen(outptr) + 1;
                MXMLDestroyE(&EP);
                }
              }
            }
          else
            {
            dataptr = savptr;
            }

          savptr = dataptr;
          dataptr = strstr(dataptr, "<temperature>");
          if(dataptr)
            {
            tmpptr1 = strstr(dataptr, "<gpu_temp>");
            if (tmpptr1)
              {
              MXMLFromString(&EP, tmpptr1, &Tail, Emsg, sizeof(Emsg));
              strcat(outptr, "gpu_temperature=");
              strcat(outptr, EP->Val);
              outptr += strlen(outptr) + 1;
              MXMLDestroyE(&EP);
              }
            }
          else
            {
            dataptr = savptr;
            }

          } /* end (MOMNvidiaDriverVersion >= 270) */

        else
          {
          /* unknown driver version */
          if (LOGLEVEL >= 3)
            {
            log_err(
              PBSE_RMSYSTEM,
              id,
              "Unknown Nvidia driver version");
            }

          /* need to advance dataptr so we don't recycle through same gpu */

          dataptr++;
          }
        }

    }

  return;
  }
#endif  /* NVIDIA_GPUS */


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

void mom_server_update_stat(

  mom_server *pms,
  char       *status_strings)

  {
  static char *id = "mom_server_update_stat";
  char *cp;

  if (pms->pbs_servername[0] == 0)
    {
    /* No server is defined for this slot */

    return;
    }

  if (pms->SStream == -1)
    {
    sprintf(log_buffer, "server \"%s\" has no active stream",
      pms->pbs_servername);

    log_record(PBSEVENT_SYSTEM, 0, id, log_buffer);

    return;
    }

  pms->MOMLastSendToServerTime = time(0);

  /* Generate the message header. */

  if (is_compose(pms,IS_STATUS) != DIS_SUCCESS)
    {
    return;
    }

  /* For each string, put it into the message. */

  for (cp = status_strings;cp && *cp;cp += strlen(cp) + 1)
    {
    if (LOGLEVEL >= 7)
      {
      sprintf(log_buffer,"%s: sending to server \"%s\"",
        id,
        cp);

      log_record(PBSEVENT_SYSTEM,0,id,log_buffer);
      }

    if (diswst(pms->SStream,cp) != DIS_SUCCESS)
      {
      mom_server_stream_error(pms, id, "writing status string");

      /* FAILURE */

      return;
      }
    }

  /* Launch the message */

  if (mom_server_flush_io(pms, id, "flush") != DIS_SUCCESS)
    {
    /* FAILURE */

    return;
    }

  if (LOGLEVEL >= 3)
    {
    sprintf(log_buffer, "status update successfully sent to %s",
      pms->pbs_servername);

    log_record(PBSEVENT_SYSTEM, 0, id, log_buffer);
    }

  /* It would be redundant to send state since it is already in status */

  pms->ReportMomState = 0;

  return;
  }  /* END mom_server_update_stat() */



/**
 * mom_server_update_gpustat
 *
 * Send a gpu status update message to a server.
 *
 * NOTE:  if interface is bad, try to recover is ???
 *        Status is only sent if gpus exist (Currently Nvidia only)
 *
 * @param mom_server_all_update_stat() - parent
 * @param pms pointer to mom_server instance
 */

#ifdef NVIDIA_GPUS
void mom_server_update_gpustat(

  mom_server *pms,
  char       *status_strings)

  {
  static char *id = "mom_server_update_gpustat";
  char *cp;

  if (pms->pbs_servername[0] == 0)
    {
    /* No server is defined for this slot */

    return;
    }

  if (pms->SStream == -1)
    {
    sprintf(log_buffer, "server \"%s\" has no active stream",
      pms->pbs_servername);

    log_record(PBSEVENT_SYSTEM, 0, id, log_buffer);

    return;
    }

  pms->MOMLastSendToServerTime = time(0);

  /* Generate the message header. */

  if (is_compose(pms,IS_GPU_STATUS) != DIS_SUCCESS)
    {
    return;
    }

  /* For each string, put it into the message. */

  for (cp = status_strings;cp && *cp;cp += strlen(cp) + 1)
    {
    if (LOGLEVEL >= 7)
      {
      sprintf(log_buffer,"%s: sending to server \"%s\"",
        id,
        cp);

      log_record(PBSEVENT_SYSTEM,0,id,log_buffer);
      }

    if (diswst(pms->SStream,cp) != DIS_SUCCESS)
      {
      mom_server_stream_error(pms, id, "writing status string");

      /* FAILURE */

      return;
      }
    }

  /* Launch the message */

  if (mom_server_flush_io(pms, id, "flush") != DIS_SUCCESS)
    {
    /* FAILURE */

    return;
    }

  if (LOGLEVEL >= 3)
    {
    sprintf(log_buffer, "status update successfully sent to %s",
      pms->pbs_servername);

    log_record(PBSEVENT_SYSTEM, 0, id, log_buffer);
    }

  /* It would be redundant to send state since it is already in status */

  pms->ReportMomState = 0;

  return;
  }  /* END mom_server_update_gpustat() */
#endif  /* NVIDIA_GPUS */




/**
 * mom_server_all_update_stat
 *
 * This is the former is_update_stat.  It has been reworked to
 * first generate the strings and then walk the server list sending
 * the strings to each server.
 */

void mom_server_all_update_stat(void)

  {
  static char *id = "mom_server_all_update_stat";

  static char status_strings[16 * 1024];  /* Big but smaller than before in is_update_stat */
  int sindex;

  /* We generate the status once, because this might be costly.
   * The buffer status_strings will contain NULL terminated strings.
   * The end of the buffer is marked with an empty string i.e. NULL NULL.
   */

  if (LOGLEVEL >= 6)
    {
    log_record(PBSEVENT_SYSTEM, 0, id, "composing status update for server");
    }

  memset(status_strings, 0, sizeof(status_strings));

  generate_server_status(status_strings, sizeof(status_strings));

  for (sindex = 0;sindex < PBS_MAXSERVER;sindex++)
    {
    mom_server_update_stat(&mom_servers[sindex],status_strings);
    }

  return;
  }  /* END mom_server_all_update_stat() */





/**
 * mom_server_all_update_gpustat
 *
 * This is the former is_update_stat.  It has been reworked to
 * first generate the strings and then walk the server list sending
 * the strings to each server.
 */

#ifdef NVIDIA_GPUS
void mom_server_all_update_gpustat(void)

  {
  static char *id = "mom_server_all_update_gpustat";

  static char gpu_status_strings[MAX_GPUS * 512];
  int sindex;

  /* We generate the status once, because this might be costly.
   * The buffer status_strings will contain NULL terminated strings.
   * The end of the buffer is marked with an empty string i.e. NULL NULL.
   */

  if ((LOGLEVEL >= 6) && (MOMNvidiaDriverVersion >= 260))
    {
    log_record(PBSEVENT_SYSTEM, 0, id, "composing gpu status update for server");
    }

  memset(gpu_status_strings, 0, sizeof(gpu_status_strings));

#ifdef NVIDIA_GPUS
#ifdef NVML_API

  generate_server_gpustatus_nvml(gpu_status_strings, sizeof(gpu_status_strings));
#else

  generate_server_gpustatus_smi(gpu_status_strings, sizeof(gpu_status_strings));
#endif /* NVML_API */
#endif /* NVIDIA_GPUS */

  for (sindex = 0;sindex < PBS_MAXSERVER;sindex++)
    {
    if(gpu_status_strings[0] != 0)
      {
      mom_server_update_gpustat(&mom_servers[sindex], gpu_status_strings);
      }
    }

  return;
  }  /* END mom_server_all_update_gpustat() */
#endif  /* NVIDIA_GPUS */





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
 * mom_server_send_hello
 *
 * This sends a hello message to server.
 *
 * @param pms pointer to mom_server instance
 * @return count 0 or -1
 *
 */

int mom_server_send_hello(

  mom_server *pms)

  {
  static char id[] = "mom_server_send_hello";

  if (is_compose(pms, IS_HELLO) == -1)
    {
    return(-1);
    }

  if (mom_server_flush_io(pms, id, "flush") != DIS_SUCCESS)
    {
    return(-1);
    }

  pms->sent_hello_count++;

  return(0);
  }  /* END mom_server_send_hello() */




/**
 * mom_server_check_connection
 *
 * This checks the status of one server.
 *
 * @param pms pointer to mom_server instance
 * @return count 0 or 1
 *
 * @see mom_server_all_check_connection() - parent
 * @see mom_server_open_stream() - child
 * @see calculate_retry_seconds() - child
 *
 * NOTE:  time_now updated in main_loop()
 */

int mom_server_check_connection(

  mom_server *pms)

  {
  static char id[] = "mom_server_check_connection";

  if (pms->pbs_servername[0] == '\0')
    {
    return(0);
    }

  if ((pms->SStream != -1) &&
      (time_now >= (pms->MOMLastSendToServerTime + (ServerStatUpdateInterval*2))))
    {
    sprintf(log_buffer,"connection to server %s timeout",
      pms->pbs_servername);

    log_record(PBSEVENT_SYSTEM,0,id,log_buffer);

    rpp_close(pms->SStream);

    pms->SStream = -1;
    }

  if (pms->SStream == -1)
    {
    /* No connection to server */

    if ((pms->next_connect_time == 0) || (pms->next_connect_time <= time_now))
      {
      if (mom_server_open_stream(pms) == DIS_SUCCESS)
        {
        pms->connect_failure_count = 0;
        pms->received_cluster_address_count = 0;
        }
      else
        {
        pms->connect_failure_count++;

        pms->next_connect_time = time_now + calculate_retry_seconds(pms->connect_failure_count);

        sprintf(log_buffer, "unable to establish/restore connection to server %s (failcount=%d, retry in %ld seconds)",
                pms->pbs_servername,
                pms->connect_failure_count,
                pms->next_connect_time - (long)time_now);

        log_record(PBSEVENT_SYSTEM, 0, id, log_buffer);

        return(0);        /* attempt to restore connection to pbs_server failed */
        }

      sprintf(log_buffer, "sending hello to server %s",
              pms->pbs_servername);

      log_record(PBSEVENT_SYSTEM, 0, id, log_buffer);

      pms->MOMLastSendToServerTime = time_now;

      if (mom_server_send_hello(pms) == -1)
        {
        return(0);
        }

      }
    }    /* END if (pms->SStream == -1) */

  return(pms->received_cluster_address_count);
  }  /* END mom_server_check_connection() */





/**
 * mom_server_all_check_connection
 *
 * This routine checks all of the servers to see
 * if they are active and up.
 *
 * @return count of active servers
 *
 * @see mom_server_check_connection() - child
 */

int
mom_server_all_check_connection(void)

  {
  int sindex;  /* server index */
  int TotalClusterAddrsCount;

  TotalClusterAddrsCount = 0;

  for (sindex = 0;sindex < PBS_MAXSERVER;sindex++)
    {
    if (mom_servers[sindex].pbs_servername[0] == '\0')
      continue;

    TotalClusterAddrsCount += mom_server_check_connection(&mom_servers[sindex]);
    }  /* END for (sindex) */

  return(TotalClusterAddrsCount);
  }  /* END mom_server_all_check_connection() */





/**
 * mom_server_diag
 *
 * This routine generates the diagnostic information for a single server
 * instance.  Note that the strings generated are newline separated.
 *
 * @param pms pointer to mom_server instance
 * @param sindex the mom_server index (used to display a server ID name)
 * @param BPtr pointer to space for writing the string
 * @param BSpace amount of space remaining
 * @see mom_server_all_diag
 */

void mom_server_diag(

  mom_server   *pms,
  int           sindex,
  char        **BPtr,
  int          *BSpace)

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
          netaddr(rpp_getaddr(pms->SStream)));

  MUStrNCat(BPtr, BSpace, tmpLine);

  if ((pms->received_hello_count > 0) ||
      (pms->received_cluster_address_count > 0))
    {
    if (verbositylevel >= 1)
      {
      sprintf(tmpLine, "  Init Msgs Received:     %d hellos/%d cluster-addrs\n",
              pms->received_hello_count,
              pms->received_cluster_address_count);

      MUStrNCat(BPtr, BSpace, tmpLine);

      sprintf(tmpLine, "  Init Msgs Sent:         %d hellos\n",
              pms->sent_hello_count);

      MUStrNCat(BPtr, BSpace, tmpLine);
      }
    }
  else
    {
    sprintf(tmpLine, "  WARNING:  no hello/cluster-addrs messages received from server\n");

    MUStrNCat(BPtr, BSpace, tmpLine);

    sprintf(tmpLine, "  Init Msgs Sent:         %d hellos\n",
            pms->sent_hello_count);

    MUStrNCat(BPtr, BSpace, tmpLine);
    }

  if (pms->MOMSendStatFailure[0] != '\0')
    {
    sprintf(tmpLine, "  WARNING:  could not open connection to server, %s%s\n",
            pms->MOMSendStatFailure,
            (strstr(pms->MOMSendStatFailure, "cname") != NULL) ?
            " (check name resolution - /etc/hosts?)" :
            "");

    MUStrNCat(BPtr, BSpace, tmpLine);
    }

  if (TMOMRejectConn[0] != '\0')
    {
    MUSNPrintF(BPtr, BSpace, "  WARNING:  invalid attempt to connect from server %s\n",
               TMOMRejectConn);
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

  MUStrNCat(BPtr, BSpace, tmpLine);

  if (pms->MOMLastSendToServerTime > 0)
    {
    sprintf(tmpLine, "  Last Msg To Server:     %ld seconds\n",
            (long)Now - pms->MOMLastSendToServerTime);
    }
  else
    {
    sprintf(tmpLine, "  WARNING:  no messages sent to server\n");
    }

  MUStrNCat(BPtr, BSpace, tmpLine);

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

  char **BPtr,
  int *BSpace)

  {
  int sindex;

  if (mom_servers[0].pbs_servername[0] == '\0')
    {
    MUStrNCat(BPtr, BSpace, "WARNING:  server not specified (set $pbsserver)\n");
    }
  else
    {
    for (sindex = 0;sindex < PBS_MAXSERVER;sindex++)
      {
      mom_server_diag(&mom_servers[sindex], sindex, BPtr, BSpace);
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
void
mom_server_update_receive_time(int stream, char *command_name)
  {
  mom_server *pms;

  if ((pms = mom_server_find_by_stream(stream)) != NULL)
    {
    pms->MOMLastRecvFromServerTime = time(0);
    strcpy(pms->MOMLastRecvFromServerCmd, command_name);
    }
  }





/**
 * mom_server_update_receive_time_by_ip
 *
 * This is a little weird as this is called from code
 * in the server directory, process_request.  There is
 * no stream number there but instead there is an IP
 * address and so we use that to look up the server
 * and update the info.
 *
 * @param ipaddr The IP address from which the command was received.
 * @param command_name The name of the command that was received.
 */

void mom_server_update_receive_time_by_ip(

  u_long  ipaddr,
  char   *command_name)

  {
  mom_server *pms;

  if ((pms = mom_server_find_by_ip(ipaddr)) != NULL)
    {
    pms->MOMLastRecvFromServerTime = time(0);

    strcpy(pms->MOMLastRecvFromServerCmd, command_name);
    }

  return;
  }




/**
** Modified by Tom Proett <proett@nas.nasa.gov> for PBS.
*/

tree *okclients = NULL; /* tree of ip addrs */



/**
 * mom_server_valid_message_source
 *
 * This routine is called from is_request to validate
 * that the request is coming from a know server.
 * If the server is good, a pointer to the server
 * instance is returned.  Otherwise NULL indicates error.
 *
 * @param stream The stream number in question
 * @return pms A pointer to the server instance.
 * @see is_request
 */

mom_server *mom_server_valid_message_source(

  int stream)

  {
  static char *id = "mom_server_valid_message_source";

  struct sockaddr_in *addr = NULL;
  u_long ipaddr;
  mom_server *pms;

  /* Check for the normal case, where some server has an open,
   * establish stream connection to the place where this
   * message came from.
   */

  if ((pms = mom_server_find_by_stream(stream)))
    {
    return(pms);
    }

  addr = rpp_getaddr(stream);  /* Get pointer to sockaddr_in for message source address */

  ipaddr = ntohl(addr->sin_addr.s_addr);  /* Extract IP address of source of the message. */

  /* So the stream number did not match any server but maybe
   * the server has another stream connection open to the IP address.
   */

  if ((pms = mom_server_find_by_ip(ipaddr)))
    {
    /* This case can happen when both the pbs_mom and the pbs_server initiate
     * a communication session with the HELLO protocol on startup.
     * We then have a stream open from both sides.  In this case, the pbs_mom
     * defers and closes the existing stream, replacing it with the new one
     * from the server.
     */
    if (pms->SStream != -1)
      {
      sprintf(log_buffer, "duplicate connection from %s - closing original connection",
              netaddr(addr));

      log_record(
        PBSEVENT_ERROR,
        PBS_EVENTCLASS_SERVER,
        id,
        log_buffer);

      rpp_close(pms->SStream);
      }

    /* Now this is the current stream number for this server */

    pms->SStream = stream;

    mom_server_send_hello(pms);

    return(pms);
    }
  else
    {
    /* There is no existing stream connection to the server. */

    /* Maybe the right thing to do now is to iterate over all defined
     * servers. If there are servers defined with no open stream
     * and a gethostbyname result matches the message source IP address,
     * then accept the stream and put the stream number into the
     * server struct.  Then in the future, the normal case above
     * will match.  This approach doesn't have the mom's madly
     * attempting to clobber the network with gethostbyname if
     * the DNS server is dead.  We only do gethostbyname if we
     * get a message from the pbs_server.
     */
#if 1
      {
      int sindex;

      for (sindex = 0;sindex < PBS_MAXSERVER;sindex++)
        {
        pms = &mom_servers[sindex];

        if (pms->pbs_servername[0] &&
            pms->SStream != -1)
          {

          struct hostent *host;

          struct in_addr  saddr;
          u_long          server_ip;

          if ((host = gethostbyname(pms->pbs_servername)) != NULL)
            {
            memcpy(&saddr, host->h_addr, host->h_length);

            server_ip = ntohl(saddr.s_addr);

            if (ipaddr == server_ip)
              {
              tinsert(ipaddr, NULL, &okclients);
              pms->SStream = stream;
              return(pms);
              }
            }
          }
        }
      }  /* END BLOCK */

#endif

    sprintf(log_buffer, "bad connect from %s - unauthorized server",
            netaddr(addr));

    sprintf(TMOMRejectConn, "%s  %s",
            netaddr(addr),
            "(server not authorized)");

    log_ext(-1,id,log_buffer,LOG_ALERT);

    rpp_close(stream);
    }

  return(NULL);
  }  /* END mom_server_valid_message_source() */





/**
 * Request is coming from another server (i.e., pbs_server) over a DIS rpp
 * stream (process 'hello' and 'cluster_addrs' request).
 *
 * @see is_compose() - peer - generate message to send to pbs_server.
 * @see process_request() - peer - handle jobstart, jobcancel, etc messages.
 *
 * Read the stream to get a Inter-Server request.
 */

void is_request(

  int  stream,   /* I */
  int  version,  /* I */
  int *cmdp)     /* O (optional) */

  {
  static char id[] = "is_request";

  int  command = 0;
  int  ret = DIS_SUCCESS;
  mom_server *pms;

  struct sockaddr_in *addr = NULL;
  u_long ipaddr;
  extern char *PBSServerCmds[];

  if (cmdp != NULL)
    *cmdp = 0;

  if (LOGLEVEL >= 4)
    {
    sprintf(log_buffer, "stream %d version %d",
      stream,
      version);

    log_record(
      PBSEVENT_ERROR,
      PBS_EVENTCLASS_JOB,
      id,
      log_buffer);
    }

  if (version != IS_PROTOCOL_VER)
    {
    sprintf(log_buffer, "protocol version %d unknown",
            version);

    log_ext(-1,id,log_buffer,LOG_ALERT);

    rpp_close(stream);

    return;
    }

  /* check that machine is okay to be a server */
  /* If the stream is the SStream we already opened, then it's fine  */

  if ((pms = mom_server_valid_message_source(stream)) == NULL)
    return;

  command = disrsi(stream, &ret);

  if (ret != DIS_SUCCESS)
    goto err;

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
      id,
      log_buffer);
    }

  mom_server_update_receive_time(stream, PBSServerCmds[command]);

  switch (command)

    {
    case IS_NULL: /* a ping from the server */

      /* nothing seems to ever generate an IS_NULL message */

      if (internal_state & INUSE_DOWN)
        {
        int sindex;

        for (sindex = 0;sindex < PBS_MAXSERVER;sindex++)
          {
          if (mom_servers[sindex].SStream != -1)
            state_to_server(sindex, 1);
          }
        }

      break;

    case IS_HELLO:  /* server wants a return ping */

      if (is_compose(pms, IS_HELLO) != DIS_SUCCESS)
        {
        break;
        }

      if (mom_server_flush_io(pms, id, "flush") != DIS_SUCCESS)
        break;

      pms->received_hello_count++;

      /* FORCE immediate server update */

      LastServerUpdateTime = 0;

      break;

    case IS_CLUSTER_ADDRS:
      for (;;)
        {
        ipaddr = disrul(stream, &ret);

        if (ret != DIS_SUCCESS)
          break;

        tinsert(ipaddr, NULL, &okclients);

        if (LOGLEVEL >= 4)
          {
          char tmpLine[1024];

          sprintf(tmpLine, "%s:\t%s added to okclients",
                  id,
                  netaddr_pbs_net_t(ipaddr));

          log_record(
            PBSEVENT_ERROR,
            PBS_EVENTCLASS_JOB,
            id,
            tmpLine);
          }
        }  /* END for (;;) */

      if (ret != DIS_EOD)
        goto err;

      pms->received_cluster_address_count++;

      /* FORCE immediate update server */

      LastServerUpdateTime = 0;

      break;

    default:

      sprintf(log_buffer, "unknown command %d sent",
              command);

      log_ext(-1,id,log_buffer,LOG_ALERT);

      goto err;
    }  /* END switch(command) */

  rpp_eom(stream);

  return;

err:

  /* We come here if we got a DIS read error or a protocol
  ** element is missing.  */

  sprintf(log_buffer, "%s from %s",
          dis_emsg[ret],
          (addr != NULL) ? netaddr(addr) : "???");

  log_ext(-1,id,log_buffer,LOG_ALERT);

  rpp_close(stream);

  if (pms)
    pms->SStream = -1;  /* Force new connection to server next time mom_server_check_connections is called. */

  }  /* END is_request() */





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
  static char id[] = "check_busy";

  int sindex;
  int numvnodes = 0;
  job *pjob;
  float myideal_load;
  float mymax_load;

  extern int   internal_state;
  extern float ideal_load_val;
  extern float max_load_val;
  extern char *auto_ideal_load;
  extern char *auto_max_load;
  extern tlist_head  svr_alljobs;

  if ((auto_max_load != NULL) || (auto_ideal_load != NULL))
    {
    if ((pjob = (job *)GET_NEXT(svr_alljobs)) != NULL)
      {
      for (;pjob != NULL;pjob = (job *)GET_NEXT(pjob->ji_alljobs))
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

      log_record(
        PBSEVENT_ERROR,
        PBS_EVENTCLASS_JOB,
        id,
        log_buffer);
      }

    internal_state |= INUSE_BUSY;

    for (sindex = 0;sindex < PBS_MAXSERVER;sindex++)
      {
      if (mom_servers[sindex].SStream != -1)
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

      log_record(
        PBSEVENT_ERROR,
        PBS_EVENTCLASS_JOB,
        id,
        log_buffer);
      }

    internal_state = (internal_state & ~INUSE_BUSY);

    for (sindex = 0;sindex < PBS_MAXSERVER;sindex++)
      {
      if (mom_servers[sindex].SStream != -1)
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

  static char tmpPBSNodeMsgBuf[1024];

  char *id = "check_state";

  if (Force)
    {
    ICount = 0;
    }

  /* conditions:  external state should be down if
     - inadequate file handles available (for period X)
     - external health check fails
  */

  /* verify adequate space in spool directory */

#define TMINSPOOLBLOCKS 100  /* blocks available in spool directory required for proper operation */


#if MOMCHECKLOCALSPOOL
    {
    char *sizestr;
    u_Long freespace;
    extern char *size_fs(char *);  /* FIXME: put this in a header file */

    /* size_fs() is arch-specific method in mom_mach.c */
    sizestr = size_fs(path_spool);  /* returns "free:total" */

    freespace = strTouL(sizestr, NULL, 10);

    if (freespace < TMINSPOOLBLOCKS)
      {
      /* inadequate disk space in spool directory */

      strcpy(PBSNodeMsgBuf, "ERROR: torque spool filesystem full");

      /* NOTE:  adjusting internal state may not be proper behavior, see note below */

      internal_state |= INUSE_DOWN;
      }
    }    /* END BLOCK */

#endif /* MOMCHECKLOCALSPOOL */

  if (PBSNodeCheckPath[0] != '\0')
    {
    int IsError = 0;

    if (ICount == 0)
      {
      /* only clear when we're checking again */
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
          IsError = 1;
          }
        else if (!strncasecmp(tmpPBSNodeMsgBuf, "EVENT:", strlen("EVENT:")))
          {
          /* pass event directly to scheduler for processing */

          /* NO-OP */
          }
        else
          {
          /* ignore non-error messages */

          tmpPBSNodeMsgBuf[0] = '\0';
          }
        }
      }    /* END if (ICount == 0) */

    if (tmpPBSNodeMsgBuf[0] != '\0')
      {
      /* update node msg buffer */

      strncpy(
        PBSNodeMsgBuf,
        tmpPBSNodeMsgBuf,
        sizeof(PBSNodeMsgBuf));

      PBSNodeMsgBuf[sizeof(PBSNodeMsgBuf) - 1] = '\0';

      /* NOTE:  not certain this is the correct behavior, scheduler should
         probably make this decision as proper action may be context sensitive */

      if (IsError == 1)
        {
        internal_state |= INUSE_DOWN;

        snprintf(log_buffer,sizeof(log_buffer),
          "Setting node to down. The node health script output the following message:\n%s\n",
          tmpPBSNodeMsgBuf);
        log_event(PBSEVENT_SYSTEM,PBS_EVENTCLASS_NODE,id,log_buffer);
        }
      else
        {
        snprintf(log_buffer,sizeof(log_buffer),
          "Node health script ran and says the node is healthy with this message:\n%s\n",
          tmpPBSNodeMsgBuf);
        log_event(PBSEVENT_SYSTEM,PBS_EVENTCLASS_NODE,id,log_buffer);
        }
      }
    }      /* END if (PBSNodeCheckPath[0] != '\0') */

  ICount++;

  ICount %= MAX(1, PBSNodeCheckInterval);

  return;
  }  /* END check_state() */





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
  static char id[] = "state_to_server";
  mom_server *pms = &mom_servers[ServerIndex];

  if ((force == 0) && (pms->ReportMomState == 0))
    {
    return;    /* Do nothing, just return */
    }

  if (is_compose(pms, IS_UPDATE) != DIS_SUCCESS)
    {
    return;
    }

  if (diswui(pms->SStream, internal_state) != DIS_SUCCESS)
    {
    mom_server_stream_error(pms, id, "writing internal state");

    return;
    }

  if (mom_server_flush_io(pms, id, "flush") == DIS_SUCCESS)
    {
    /* send successful, unset ReportMomState */

    pms->ReportMomState = 0;

    if (LOGLEVEL >= 4)
      {
      sprintf(log_buffer, "sent updated state 0x%x to server %s",
              internal_state,
              pms->pbs_servername);

      log_record(
        PBSEVENT_ERROR,
        PBS_EVENTCLASS_JOB,
        id,
        log_buffer);
      }
    }

  return;
  }  /* END state_to_server() */





void
mom_server_all_send_state(void)

  {
  int sindex;
  mom_server *pms;

  for (sindex = 0;sindex < PBS_MAXSERVER;sindex++)
    {
    pms = &mom_servers[sindex];

    if (pms->ReportMomState != 0)
      state_to_server(sindex, 0);
    }

  return;
  }





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

  job  *pjob,
  char *caller_id,
  void (*message_handler)(int))

  {
  char *svrport = NULL;
  char *serverAddr = NULL;
  char error_buffer[1024];
  int sock;
  int sock3;
  int port;
  int sindex;
  mom_server *pms;

  /* See if the server address string has a ':' implying a port number. */

  serverAddr = pjob->ji_wattr[(int)JOB_ATR_at_server].at_val.at_str;
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

      struct sockaddr_in *addr;
      u_long              ipaddr;
      u_short             ipport;

      pms = &mom_servers[sindex];

      if (pms->SStream != -1)
        {
        addr = rpp_getaddr(pms->SStream);
        if (addr == NULL)
        {
          continue;
        }
        ipaddr = ntohl(addr->sin_addr.s_addr);
        ipport = ntohs(addr->sin_port);

        if (ipaddr != pjob->ji_qs.ji_un.ji_momt.ji_svraddr)
          {
          sock = client_to_svr(
                   ipaddr,
                   ipport,
                   1,  /* use local socket */
                   error_buffer);  /* O */

          if (sock >= 0)
            break;
          }
        }
      }
    }

  /* The epilogue code needs the socket number at 3 or above. */

  if ((sock >= 0) && (sock < 3))
    {
    sock3 = fcntl(sock, F_DUPFD, 3);
    close(sock);
    sock = sock3;
    }

  /*
   * ji_momhandle is used to match reply messages to their job.
   * Why not use the job number to find the job when we receive a reply message?
   */

  pjob->ji_momhandle = sock;

  /* Associate a message handler with the connection */

  if ((sock >= 0) && (message_handler != NULL))
    {
    add_conn(
      sock,
      ToServerDIS,
      pjob->ji_qs.ji_un.ji_momt.ji_svraddr,
      port,
      PBS_SOCK_INET,
      message_handler);
    }

  return(sock);
  }  /* END mom_open_socket_to_jobs_server() */





/**
 * clear_down_mom_servers
 *
 * Clears the mom_server down address list.
 * Called from the catch_child code.
 *
 * @see scan_for_exiting
 */

void
clear_down_mom_servers(void)
  {
  int sindex;

  for (sindex = 0;sindex < PBS_MAXSERVER;sindex++)
    {
    down_svraddrs[sindex] = 0;
    }

  return;
  }

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





/**
 * no_mom_servers_down
 *
 * Checks to see if the server address down list is empty.
 * Called from the catch_child code.
 * @see scan_for_exiting
 */

int
no_mom_servers_down(void)

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

