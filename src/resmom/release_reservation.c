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



#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <libxml/parser.h>
#include <libxml/tree.h>
#include <unistd.h>

#include "alps_functions.h"
#include "alps_constants.h"
#include "utils.h"





int parse_release_output(

  char *output)

  {
  int        rc = -1;
  xmlDocPtr  doc;
  xmlNode   *node;
  char      *attr_val;

  if ((doc = xmlReadMemory(output, strlen(output), "apbasil", NULL, 0)) == NULL)
    {
    char buf[MAXLINE * 4];
    xmlErrorPtr pErr = xmlGetLastError();
    snprintf(buf, sizeof(buf), "Failed to parse the output of alps - %s", pErr->message);
    log_err(-1, __func__, buf);
    return(ALPS_PARSING_ERROR);
    }

  node = xmlDocGetRootElement(doc);

  while (node != NULL)
    {
    if (!strcmp((const char *)node->name, response_data))
      {
      attr_val = (char *)xmlGetProp(node, (const xmlChar *)status);

      if (strcmp((const char *)attr_val, success))
        {
        rc = find_error_type(node);
        }
      else
        rc = PBSE_NONE;

      free(attr_val);

      break;
      }
    else if (!strcmp((const char *)node->name, text_name))
      node = node->next;
    else
      node = node->children;
    }

  return(rc);
  } /* END parse_release_output() */



int execute_alps_release(
    
  char *command)

  {
  FILE     *alps_pipe;
  char      output[MAXLINE * 4];
  int       fd;
  int       rc;
  char     *ptr;
  int       bytes_read;
  int       total_bytes_read = 0;

  if ((alps_pipe = popen(command, "r")) == NULL)
    {
    snprintf(log_buffer, sizeof(log_buffer),
      "Unable to open command %s for apbasil",
      command);
    log_err(errno, __func__, log_buffer);

    return(WRITING_PIPE_ERROR);
    }

  fd = fileno(alps_pipe);

  /* now read from the pipe */
  ptr = output;
  memset(output, 0, sizeof(output));

  while ((bytes_read = read(fd, ptr, sizeof(output) - total_bytes_read - 1)) > 0)
    {
    total_bytes_read += bytes_read;
    ptr += bytes_read;
    }

  /* perform post-processing */
  pclose(alps_pipe);

  if ((bytes_read == -1) ||
      (total_bytes_read == 0))
    rc = READING_PIPE_ERROR;
  else
    rc = parse_release_output(output);

  return(rc);
  } /* END execute_alps_release() */




int destroy_alps_reservation(

  char *reservation_id,
  char *apbasil_path,
  char *apbasil_protocol)

  {
  int       rc = 1;
  int       retry_count = 0;
  char      command_buf[MAXLINE * 2];

  snprintf(command_buf, sizeof(command_buf), DELETE_BASIL_REQ,
    (apbasil_protocol != NULL) ? apbasil_protocol : DEFAULT_APBASIL_PROTOCOL,
    reservation_id,
    (apbasil_path != NULL) ? apbasil_path : DEFAULT_APBASIL_PATH);

  while ((rc != PBSE_NONE) &&
         (rc != apbasil_fail_permanent) &&
         (retry_count < APBASIL_RETRIES))
    {
    rc = execute_alps_release(command_buf);
    retry_count++;
    }

  return(rc);
  } /* END destroy_alps_reservation() */

