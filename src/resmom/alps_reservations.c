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




int create_reserve_params_from_host_req_list(

  resizable_array *host_req_list, /* I */
  dynamic_string  *command)       /* O */

  {
  dynamic_string *node_list = get_dynamic_string(-1, NULL);
  host_req       *hr;
  unsigned int    nppn = 0;
  unsigned int    width = 0;
  int             iter = -1;
  
  while ((hr = (host_req *)next_thing(host_req_list, &iter)) != NULL)
    {
    width += hr->ppn;
    nppn = MAX(nppn,hr->ppn);
    
    if (node_list->used != 0)
      append_dynamic_string(node_list, ",");
    
    append_dynamic_string(node_list, hr->hostname);
    
    free_host_req(hr);
    }
  
  save_current_reserve_param(command, node_list, width, nppn);

  return(PBSE_NONE);
  } /* END create_reserve_params_from_host_req_list() */



int create_reserve_params_from_multi_req_list(

  char           *multi_req_list, /* I */
  dynamic_string *command)        /* O */

  {
  dynamic_string *node_list = get_dynamic_string(-1, NULL);
  char           *tok;
  char           *str = multi_req_list;
  int             node_count = 1;
  char           *comma;
  unsigned int    width;
  unsigned int    nppn;
  
  while ((tok = threadsafe_tokenizer(&str, "*")) != NULL)
    {
    clear_dynamic_string(node_list);
    append_dynamic_string(node_list, tok);
    
    comma = tok;
    while ((comma = strchr(comma+1, ',')) != NULL)
      node_count++;
    
    tok = threadsafe_tokenizer(&str, "|");
    nppn = atoi(tok);
    
    width = nppn * node_count;
    save_current_reserve_param(command, node_list, width, nppn);
    }

  return(PBSE_NONE);
  } /* END create_reserve_params_from_multi_req_list() */




dynamic_string *get_reservation_command(

  resizable_array *host_req_list,
  char            *username,
  char            *jobid,
  char            *apbasil_path,
  char            *apbasil_protocol,
  char            *multi_req_list)

  {
  dynamic_string *command = get_dynamic_string(-1, NULL);
  dynamic_string *node_list = get_dynamic_string(-1, NULL);
  char            buf[MAXLINE * 2];

  /* place the top header */
  snprintf(buf, sizeof(buf), APBASIL_RESERVE_REQ, 
    (apbasil_protocol != NULL) ? apbasil_protocol : DEFAULT_APBASIL_PROTOCOL);
  append_dynamic_string(command, buf);

  /* place the reserve header */
  snprintf(buf, sizeof(buf), APBASIL_RESERVE_ARRAY, username, jobid);
  append_dynamic_string(command, buf);

  if (multi_req_list == NULL)
    {
    create_reserve_params_from_host_req_list(host_req_list, command);
    }
  else
    {
    create_reserve_params_from_multi_req_list(multi_req_list, command);
    }

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
  xmlNode *grandchild;

  if (!strcmp(attr_val, "PERMANENT"))
    rc = apbasil_fail_permanent;

  for (child = node->children; child != NULL; child = child->next)
    {
    if (!strcmp((const char *)child->name, message))
      {
      for (grandchild = child->children; grandchild != NULL; grandchild = grandchild->next)
        {
        if (!strcmp((const char *)grandchild->name, text_name))
          {
          char *errtext = (char *)xmlNodeGetContent(grandchild);
          if (strstr(errtext, "error") != NULL)
            {
            snprintf(log_buffer, sizeof(log_buffer), "%s alps error: '%s'",
              attr_val, errtext);
            log_err(-1, __func__, log_buffer);
            }
          }
        }
      }
    }

  free(attr_val);

  return(rc);
  } /* END find_error_type() */




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
        free(attr_val);
        return(find_error_type(node));
        }
      else
        rc = PBSE_NONE;
        
      free(attr_val);
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
  int             rc;
  FILE           *alps_pipe;
  int             fd;
  dynamic_string *output = NULL;
  char            tmpBuf[MAXLINE];
  int             bytes_read;
  int             total_bytes_read = 0;

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
  output = get_dynamic_string(-1, NULL);
  while ((bytes_read = read(fd, tmpBuf, sizeof(tmpBuf) - 1)) > 0)
    {
    total_bytes_read += bytes_read;
    tmpBuf[bytes_read] = '\0';
    append_dynamic_string(output, tmpBuf);
    }

  /* perform post-processing */
  pclose(alps_pipe);

  if ((bytes_read == -1) ||
      (total_bytes_read == 0))
    rc = READING_PIPE_ERROR;
  else
    {
    rc = parse_reservation_output(output->str, reservation_id);
    }

  free_dynamic_string(output);

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

      free(attr_val);

      break;
      }
    else if (!strcmp((const char *)node->name, text_name))
      node = node->next;
    else
      node = node->children;
    }

  return(rc);
  } /* END parse_confirmation_output() */



/*
 * creates and returns a dynamic string containing the command that
 * will be executed in order to confirm an apbasil reservation. This will be 
 *
 * echo \"<?xml version='1.0'?><BasilRequest protocol='A' method='CONFIRM' 
 * reservation_id='B' 'C=D' />\" | E
 * A is the protocol version, 1.0 by default
 * B is the reservation id returned from the reservation creation
 * C is either admin_cookie in version 1.0 or pagg_id in later versions
 * D is the value of the admin cookie or the pagg id
 * E is the path to apbasil, most likely /usr/bin/apbasil
 *
 */

int get_confirm_command(

  char      *rsv_id,
  long long  pagg_id_value,
  char      *apbasil_protocol,
  char      *apbasil_path,
  char      *command_buf,
  int        size)

  {
  const char *pagg_str;

  if ((apbasil_protocol == NULL) ||
      (!strcmp(apbasil_protocol, DEFAULT_APBASIL_PROTOCOL)))
    pagg_str = admin_cookie;
  else
    pagg_str = pagg_id;

  snprintf(command_buf, size, CONFIRM_BASIL_REQ,
    (apbasil_protocol != NULL) ? apbasil_protocol : DEFAULT_APBASIL_PROTOCOL,
    rsv_id,
    pagg_str,
    pagg_id_value,
    (apbasil_path != NULL) ? apbasil_path : DEFAULT_APBASIL_PATH);

  return(PBSE_NONE);
  } /* END get_confirm_command() */





int confirm_reservation(

  char       *jobid,
  char       *reservation_id,
  long long   pagg_id_value,
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

  get_confirm_command(reservation_id, 
    pagg_id_value,
    apbasil_protocol,
    apbasil_path,
    command_buf,
    sizeof(command_buf));

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
  long long   pagg_id_value,
  char      **reservation_id)

  {
  resizable_array *host_req_list;
  dynamic_string  *command;
  int              rc = 1;
  int              retry_count = 0;
  char            *user = strdup(username);
  char            *aroba;
  
  if ((aroba = strchr(user, '@')) != NULL)
    *aroba = '\0';

  if (strchr(exec_hosts, '|') == NULL)
    {
    host_req_list = parse_exec_hosts(exec_hosts);
    
    if (host_req_list->num == 0)
      {
      free(user);
      free_resizable_array(host_req_list);
      /* this is a login only job */
      return(PBSE_NONE);
      }
  
    command = get_reservation_command(host_req_list, user, jobid, apbasil_path, apbasil_protocol, NULL);
  
    free_resizable_array(host_req_list);
    }
  else
    {
    command = get_reservation_command(NULL, user, jobid, apbasil_path, apbasil_protocol, exec_hosts);
    }

  free(user);

  /* retry on failure up to  */
  while ((retry_count++ < APBASIL_RETRIES) &&
         (rc != apbasil_fail_permanent) &&
         (rc != PBSE_NONE))
    {
    rc = execute_reservation(command->str, reservation_id);

    if (rc != PBSE_NONE)
      usleep(100);
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
      rc = confirm_reservation(jobid, *reservation_id, pagg_id_value, apbasil_path, apbasil_protocol);

      if (rc != PBSE_NONE)
        usleep(100);
      }
    }

  return(rc);
  } /* END create_alps_reservation() */

