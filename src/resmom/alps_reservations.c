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




#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <libxml/parser.h>
#include <libxml/tree.h>
#include <unistd.h>

#include "alps_constants.h"
#include "resizable_array.h"
#include "dynamic_string.h"
#include "utils.h"


extern char mom_alias[];
extern char mom_host[];


host_req *get_host_req(

  char *hostname)

  {
  host_req *hr = calloc(1, sizeof(host_req));

  hr->hostname = strdup(hostname);
  hr->ppn = 1;

  return(hr);
  } /* END get_host_req() */




void free_host_req(

  host_req *hr)

  {
  free(hr->hostname);
  free(hr);
  } /* END free_host_req() */



resizable_array *parse_exec_hosts(

  char *exec_hosts_param)

  {
  char            *slash;
  char            *host_tok;
  char            *exec_hosts = strdup(exec_hosts_param);
  char            *str_ptr = exec_hosts;
  char            *delims = "+";
  char            *prev_host_tok = NULL;
  host_req        *hr;
  resizable_array *host_req_list = initialize_resizable_array(100);

  while ((host_tok = threadsafe_tokenizer(&str_ptr, delims)) != NULL)
    {
    if ((slash = strchr(host_tok, '/')) != NULL)
      *slash = '\0';

    /* skip this host - the login shouldn't be part of the alps reservation */
    if ((strcmp(mom_host, host_tok)) &&
        (strcmp(mom_alias, host_tok)))
      {
      if ((prev_host_tok != NULL) &&
          (!strcmp(prev_host_tok, host_tok)))
        {
        hr = (host_req *)host_req_list->slots[host_req_list->last].item;
        hr->ppn += 1;
        }
      else
        {
        prev_host_tok = host_tok;
        hr = get_host_req(host_tok);
        insert_thing(host_req_list, hr);
        }
      }
    }

  free(exec_hosts);

  return(host_req_list);
  } /* END parse_exec_hosts() */



int save_current_reserve_param(

  dynamic_string *command,
  dynamic_string *node_list,
  unsigned int    width,
  unsigned int    nppn)

  {
  char            buf[MAXLINE * 2];
  int             rc; 

  /* print out the current reservation param element */
  /* place everything up to the node list */
  snprintf(buf, sizeof(buf), APBASIL_RESERVE_PARAM_BEGIN, width, nppn);
  rc = append_dynamic_string(command, buf);
  
  /* add the node list */
  if (rc == PBSE_NONE)
    append_dynamic_string(command, node_list->str);
  
  /* end the reserve param element */
  if (rc == PBSE_NONE)
    append_dynamic_string(command, APBASIL_RESERVE_PARAM_END);

  return(rc);
  } /* END save_current_reserve_param() */




dynamic_string *get_reservation_command(

  resizable_array *host_req_list,
  char            *username,
  char            *jobid,
  char            *apbasil_path,
  char            *apbasil_protocol)

  {
  dynamic_string *command = get_dynamic_string(-1, NULL);
  dynamic_string *node_list = get_dynamic_string(-1, NULL);
  char            buf[MAXLINE * 2];
  unsigned int    width = 0;
  unsigned int    nppn = 0;
  int             iter = -1;
  host_req       *hr;

  /* place the top header */
  snprintf(buf, sizeof(buf), APBASIL_RESERVE_REQ, 
    (apbasil_protocol != NULL) ? apbasil_protocol : "1.0");
  append_dynamic_string(command, buf);

  /* place the reserve header */
  snprintf(buf, sizeof(buf), APBASIL_RESERVE_ARRAY, username, jobid);
  append_dynamic_string(command, buf);

  while ((hr = (host_req *)next_thing(host_req_list, &iter)) != NULL)
    {
    /* find the node list for nodes of equal ppn */
    if ((nppn != 0) &&
        (nppn != hr->ppn))
      {
      save_current_reserve_param(command, node_list, width, nppn);

      /* clear values */
      width = 0;
      clear_dynamic_string(node_list);
      }
    
    width += hr->ppn;
    nppn = hr->ppn;
    
    if (node_list->used != 0)
      append_dynamic_string(node_list, ",");
    
    append_dynamic_string(node_list, hr->hostname);

    free_host_req(hr);
    }
      
  save_current_reserve_param(command, node_list, width, nppn);

  free_dynamic_string(node_list);

  /* pipe the output to apbasil */
  snprintf(buf, sizeof(buf), "</ReserveParamArray></BasilRequest>\" | %s",
    (apbasil_path != NULL) ? apbasil_path : DEFAULT_APBASIL_PATH);
  append_dynamic_string(command, buf);

  return(command);
  } /* END get_reservation_command() */



int find_error_type(

  xmlNode *node)

  {
  char    *attr_val = (char *)xmlGetProp(node, (const xmlChar *)error_class);
  int      rc = apbasil_fail_transient;
  xmlNode *child;

  if (!strcmp(attr_val, "PERMANENT"))
    rc = apbasil_fail_permanent;

  for (child = node->children; child != NULL; child = child->next)
    {
    if (!strcmp((const char *)child->name, text_name))
      {
      char *errtext = (char *)xmlNodeGetContent(child);
      if (strstr(errtext, "error") != NULL)
        {
        snprintf(log_buffer, sizeof(log_buffer), "%s alps error: '%s'",
          attr_val, errtext);
        log_err(-1, __func__, log_buffer);
        }
      }
    }

  return(rc);
  } /* END find_and_log_error() */




int parse_reservation_output(

  char  *output,
  char **rsv_id)

  {
  xmlDocPtr  doc;
  xmlNode   *node;
  char      *attr_val;
  int        rc = apbasil_fail_transient;

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
    if (!strcmp((const char *)node->name, text_name))
      {
      node = node->next;
      continue;
      }
    
    if (!strcmp((const char *)node->name, response_data))
      {
      attr_val = (char *)xmlGetProp(node, (const xmlChar *)status);
      if (strcmp(attr_val, success))
        {
        /* FAILURE - permanent or transient? */
        return(find_error_type(node));
        }
      else
        rc = PBSE_NONE;
      }
    else if (!strcmp((const char *)node->name, reserved))
      {
      *rsv_id = (char *)xmlGetProp(node, (const xmlChar *)reservation_id);
      }

    node = node->children;
    }

  return(rc);
  } /* END parse_reservation_output() */




int execute_reservation(

  char  *command_str,
  char **reservation_id)

  {
  int       rc;
  FILE     *alps_pipe;
  int       fd;
  char      output[MAXLINE * 4];
  char     *ptr;
  int       bytes_read;
  int       total_bytes_read = 0;

  if ((alps_pipe = popen(command_str, "r")) == NULL)
    {
    snprintf(log_buffer, sizeof(log_buffer),
      "Unable to open command %s for apbasil",
      command_str);
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
    {
    rc = parse_reservation_output(output, reservation_id);
    }

  return(rc);
  } /* END execute_reservation() */




int parse_confirmation_output(

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

      if (strcmp(attr_val, success))
        {
        rc = find_error_type(node);
        }
      else
        rc = PBSE_NONE;

      break;
      }
    else if (!strcmp((const char *)node->name, text_name))
      node = node->next;
    else
      node = node->children;
    }

  return(rc);
  } /* END parse_confirmation_output() */




int confirm_reservation(

  char       *jobid,
  char      **reservation_id,
  long long   pagg_id,
  char       *apbasil_path,
  char       *apbasil_protocol)

  {
  int       rc;
  char      command_buf[MAXLINE * 2];
  FILE     *alps_pipe;
  int       fd;
  char      output[MAXLINE * 4];
  char     *ptr;
  int       bytes_read;
  int       total_bytes_read = 0;

  snprintf(command_buf, sizeof(command_buf), CONFIRM_BASIL_REQ,
    apbasil_protocol, *reservation_id, pagg_id,
    (apbasil_path != NULL) ? apbasil_path : DEFAULT_APBASIL_PATH);

  if ((alps_pipe = popen(command_buf, "r")) == NULL)
    {
    snprintf(log_buffer, sizeof(log_buffer),
      "Unable to open command %s for apbasil",
      command_buf);
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
    rc = parse_confirmation_output(output);

  return(rc);
  } /* END confirm_reservation() */




int create_alps_reservation(

  char       *exec_hosts,
  char       *username,
  char       *jobid,
  char       *apbasil_path,
  char       *apbasil_protocol,
  long long   pagg_id,
  char      **reservation_id)

  {
  resizable_array *host_req_list;
  dynamic_string  *command;
  int              rc = 1;
  int              retry_count = 0;
  char            *user = strdup(username);
  char            *aroba;

  host_req_list = parse_exec_hosts(exec_hosts);

  if ((aroba = strchr(user, '@')) != NULL)
    *aroba = '\0';
  
  command = get_reservation_command(host_req_list, user, jobid, apbasil_path, apbasil_protocol);

  free(user);

  /* retry on failure up to  */
  while ((retry_count++ < APBASIL_RETRIES) &&
         (rc != apbasil_fail_permanent) &&
         (rc != PBSE_NONE))
    {
    rc = execute_reservation(command->str, reservation_id);
    }

  free_dynamic_string(command);

  if (rc == PBSE_NONE)
    {
    rc = 1;
    retry_count = 0;
  
    while ((retry_count++ < APBASIL_RETRIES) &&
           (rc != apbasil_fail_permanent) &&
           (rc != PBSE_NONE))
      {
      rc = confirm_reservation(jobid, reservation_id, pagg_id, apbasil_path, apbasil_protocol);
      }
    }

  return(rc);
  } /* END create_alps_reservation() */

