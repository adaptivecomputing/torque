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
 * get_server
 *
 * ------------------------------------
 * As specified in section 5 of the ERS:
 *
 *  5.1.2.  Directing Requests to Correct Server
 *
 *  A  command  shall  perform  its  function  by  sending   the
 *  corresponding  request  for  service  to the a batch server.
 *  The choice of batch servers to which to send the request  is
 *  governed by the following ordered set of rules:
 *
 *  1. For those commands which require or accept a job identif-
 *     ier  operand, if the server is specified in the job iden-
 *     tifier operand as @server, then the batch  requests  will
 *     be sent to the server named by server.
 *
 *  2. For those commands which require or accept a job identif-
 *     ier  operand  and  the @server is not specified, then the
 *     command will attempt to determine the current location of
 *     the  job  by  sending  a  Locate Job batch request to the
 *     server which created the job.
 *
 *  3. If a server component of a destination  is  supplied  via
 *     the  -q  option,  such  as  on  qsub and qselect, but not
 *     qalter, then the server request is sent to that server.
 *
 *  4. The server request is sent to the  server  identified  as
 *     the default server, see section 2.6.3.
 *     [pbs_connect() implements this]
 *
 *  2.6.3.  Default Server
 *
 *  When a server is not specified to a client, the client  will
 *  send  batch requests to the server identified as the default
 *  server.  A client identifies the default server by  (a)  the
 *  setting  of  the environment variable PBS_DEFAULT which con-
 *  tains a destination, or (b) the  destination  in  the  batch
 *  administrator established file {PBS_DIR}/server_name.
 * ------------------------------------
 *
 * Takes a job_id_in string as input, calls parse_jobid to separate
 * the pieces, then applies the above rules in order
 * If things go OK, the function value is set to 0,
 * if errors, it is set to 1.
 *
 * Full legal syntax is:
 *  seq_number[.parent_server[:port]][@current_server[:port]]
 *
 */

#include <pbs_config.h>   /* the master config generated by configure */

#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netdb.h>
#include "pbs_ifl.h"
#include "net_connect.h"

#define notNULL(x) (((x)!=NULL) && (strlen(x)>(size_t)0))

extern int parse_jobid(char *, char **, char **, char **);



int
TShowAbout(void)

  {
  char *dserver;
  char *servervar;

  char  tmpLine[1024];

  dserver = pbs_default();

  servervar = getenv("PBS_DEFAULT");

  strcpy(tmpLine, PBS_DEFAULT_FILE);

  tmpLine[strlen(tmpLine) - strlen("/pbs_server") - 1] = '\0';

  fprintf(stderr, "HomeDir:   %s  InstallDir: %s  Server: %s%s\n",
          tmpLine,
          PBS_INSTALL_DIR,
          dserver,
          (servervar != NULL) ? " (PBS_DEFAULT is set)" : "");

  fprintf(stderr, "BuildDir:  %s\n",
          PBS_SOURCE_DIR);

  fprintf(stderr, "BuildUser: %s\n",
          PBS_BUILD_USER);

  fprintf(stderr, "BuildHost: %s\n",
          PBS_BUILD_HOST);

  fprintf(stderr, "BuildDate: %s\n",
          PBS_BUILD_DATE);

  fprintf(stderr, "Version:   %s\n",
          PACKAGE_VERSION);

  return(0);
  }  /* END TShowAbout() */


/**
 * Take a job ID string and parse it into
 * job ID and server name parts.
 *
 * @param job_id_in Input string of the form <job number>[.<parent server name>][@<host server url>]
 * @param job_id_out Output string containing only the job ID part of the input specification.
 * @param server_out Output string containing nothing or a host url if specified.
 */
int get_server(
  char *job_id_in,    /* read only */
  char *job_id_out,   /* write only */
  char *server_out)   /* write only */

  {
  char *seq_number;
  char *parent_server;
  char *current_server;
  char def_server[PBS_MAXSERVERNAME + 1];
  char host_server[PBS_MAXSERVERNAME + 1];
  char *c;

  /* parse the job_id_in into components */

  if (parse_jobid(job_id_in, &seq_number, &parent_server, &current_server))
    {
    return(1);
    }

  /* Apply the above rules, in order, except for the locate job request.
     That request is only sent if the job is not found on the local server.
  */

  if (notNULL(current_server))
    {
    /* @server found */

    strcpy(server_out, current_server);
    }
  else if (notNULL(parent_server))
    {
    /* .server found */

    strcpy(server_out, parent_server);
    }
  else
    {
    /* can't locate a server, so return a NULL
        to tell pbs_connect to use default */

    server_out[0] = '\0';
    }

  /* Make a fully qualified name of the job id. */

  strcpy(job_id_out, seq_number);

  strcat(job_id_out, ".");

  if (notNULL(parent_server))
    {
    if (notNULL(current_server))
      {
      /* parent_server might not be resolvable if current_server specified */
      strcat(job_id_out, parent_server);
      }
    else
      {
      if (get_fullhostname(parent_server, host_server, PBS_MAXSERVERNAME, NULL) != 0)
        {
        /* FAILURE */

        return(1);
        }

      strcat(job_id_out, host_server);
      }

    if ((c = strchr(parent_server, ':')) != 0)
      {
      if (*(c - 1) == '\\')
        c--;

      strcat(job_id_out, c);
      }
    }
  else
    {
    parent_server = pbs_default();

    if ((parent_server == (char *)NULL) || (*parent_server == '\0'))
      {
      return(1);
      }

    strncpy(def_server, parent_server, PBS_MAXSERVERNAME);

    c = def_server;

    while ((*c != '\n') && (*c != '\0'))
      c++;

    *c = '\0';

    if (get_fullhostname(def_server, host_server, PBS_MAXSERVERNAME, NULL) != 0)
      {
      /* FAILURE */

      return(1);
      }

    strcat(job_id_out, host_server);

    if ((c = strchr(def_server, ':')) != 0)
      {
      if (*(c - 1) == '\\')
        c--;

      strcat(job_id_out, c);
      }
    }    /* END else */

  return(0);
  }  /* END get_server() */

/* END get_server.c */

