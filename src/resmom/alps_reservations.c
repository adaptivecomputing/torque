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
#include "utils.h"
#include "lib_ifl.h"
#include "mom_config.h"
#include <vector>
#include <string>

typedef std::vector<host_req *> host_req_list;

extern char mom_alias[];

std::string get_frequency_request(struct cpu_frequency_value *pfreq)
  {
  std::string cray_frequency = "";
  char bf[20];
  switch(pfreq->frequency_type)
    {
    case P0:
    case P1:
    case P2:
    case P3:
    case P4:
    case P5:
    case P6:
    case P7:
    case P8:
    case P9:
    case P10:
    case P11:
    case P12:
    case P13:
    case P14:
    case P15:
      sprintf(bf," p-state='%u'",pfreq->frequency_type);
      cray_frequency = bf;
      break;
    case Performance:
      cray_frequency = " p-governor='performance'";
      break;
    case PowerSave:
      cray_frequency = " p-governor='powersave'";
      break;
    case OnDemand:
      cray_frequency = " p-governor='ondemand'";
      break;
    case Conservative:
      cray_frequency = " p-governor='conservative'";
      break;
    case AbsoluteFrequency:
      sprintf(bf," p-state='%lu000'",pfreq->mhz);
      cray_frequency = bf;
      break;
    }
  return cray_frequency;
  }


host_req *get_host_req(

  char         *hostname,
  unsigned int  ppn)

  {
  host_req *hr = (host_req *)calloc(1, sizeof(host_req));

  hr->hostname = strdup(hostname);
  hr->ppn = ppn;

  return(hr);
  } /* END get_host_req() */




void free_host_req(

  host_req *hr)

  {
  free(hr->hostname);
  free(hr);
  } /* END free_host_req() */

/* 
 * Sort the exec_hosts lists to match the nodes in mppnodes. 
 *
 */

host_req_list *sort_exec_hosts(
  host_req_list *exec_hosts,
  const char      *mppnodes)
  {
  if(mppnodes == NULL)
    {
    return exec_hosts;
    }

  char *tmp = strdup(mppnodes);
  char *tmp_str = tmp;
  host_req_list *tmp_host_list = new host_req_list();
  char *tok;
  host_req *pHr;
    
  while((tok = threadsafe_tokenizer(&tmp_str,",")) != NULL)
    {
    for(int i = exec_hosts->size() - 1;i >= 0;i--)
      {
      pHr = exec_hosts->at(i);
      if(strcmp(pHr->hostname,tok) == 0)
        {
        tmp_host_list->push_back(pHr);
        exec_hosts->erase(exec_hosts->begin() + i);
        break;
        }
      }
    }
  while(exec_hosts->size() != 0)
    {
    tmp_host_list->push_back(exec_hosts->back());
    exec_hosts->pop_back();
    }
  delete exec_hosts;
  free(tmp);
  return tmp_host_list;
  }

host_req_list *parse_exec_hosts(

  char *exec_hosts_param,
  const char *mppnodes)

  {
  char            *slash;
  char            *host_tok;
  char            *exec_hosts = strdup(exec_hosts_param);
  char            *str_ptr = exec_hosts;
  const char     *delims = "+";
  host_req        *hr;
  host_req_list   *list = new host_req_list();

  while ((host_tok = threadsafe_tokenizer(&str_ptr, delims)) != NULL)
    {
    if ((slash = strchr(host_tok, '/')) != NULL)
      *slash = '\0';

    /* skip this host - the login shouldn't be part of the alps reservation */
    if ((strcmp(mom_host, host_tok)) &&
        (strcmp(mom_alias, host_tok)))
      {
      std::vector<int> indices;

      if (slash != NULL)
        {
        translate_range_string_to_vector(slash + 1, indices);
        hr = get_host_req(host_tok, indices.size());
        }
      else
        hr = get_host_req(host_tok, 1);

      list->push_back(hr);
      }
    }

  free(exec_hosts);
  return(sort_exec_hosts(list,mppnodes));
  } /* END parse_exec_hosts() */




/*
 * save_current_reserve_param
 *
 * @param nppn - the number of cores to use per node, or -1 if the alps 
 * request shouldn't specify this.
 */

int save_current_reserve_param(

  std::string&     command,
  char            *apbasil_protocol,
  std::string&     node_list,
  unsigned int    width,
  int             nppn,
  int             nppcu,
  int             mppdepth,
  std::string&    cray_frequency)

  {
  char            buf[MAXLINE * 2];
  float           apbasil_protocol_float;

  if (apbasil_protocol != NULL)
    {
    if (sscanf(apbasil_protocol, "%f", &apbasil_protocol_float) != 1)
      apbasil_protocol_float = 1.0;
    }
  else
    {
    if (sscanf(DEFAULT_APBASIL_PROTOCOL, "%f", &apbasil_protocol_float) != 1)
      apbasil_protocol_float = 1.0;
    }

  if(apbasil_protocol_float < APBASIL_PROTOCOL_v14)
    {
    cray_frequency = "";
    }
  /* print out the current reservation param element */
  /* place everything up to the node list */
  if (nppn == -1)
    {
    if (mppdepth == 0)
      {
      if (apbasil_protocol_float >= APBASIL_PROTOCOL_v13)
        snprintf(buf, sizeof(buf), APBASIL_RESERVE_PARAM_BEGIN_SANS_NPPN_13, width, nppcu, cray_frequency.c_str());
      else
        snprintf(buf, sizeof(buf), APBASIL_RESERVE_PARAM_BEGIN_SANS_NPPN, width);
      }
    else
      {
      if (apbasil_protocol_float >= APBASIL_PROTOCOL_v13)
        snprintf(buf, sizeof(buf), APBASIL_RESERVE_PARAM_BEGIN_DEPTH_SANS_NPPN_13, width, nppcu, mppdepth, cray_frequency.c_str());
      else
        snprintf(buf, sizeof(buf), APBASIL_RESERVE_PARAM_BEGIN_DEPTH_SANS_NPPN, width, mppdepth);
      }
    }
  else
    {
    if (mppdepth == 0)
      {
      if (apbasil_protocol_float >= APBASIL_PROTOCOL_v13)
        snprintf(buf, sizeof(buf), APBASIL_RESERVE_PARAM_BEGIN_13, width, nppcu, nppn, cray_frequency.c_str());
      else
        snprintf(buf, sizeof(buf), APBASIL_RESERVE_PARAM_BEGIN, width, nppn);
      }
    else
      {
      if (apbasil_protocol_float >= APBASIL_PROTOCOL_v13)
        snprintf(buf, sizeof(buf), APBASIL_RESERVE_PARAM_BEGIN_DEPTH_13, width, nppcu, nppn, mppdepth, cray_frequency.c_str());
      else
        snprintf(buf, sizeof(buf), APBASIL_RESERVE_PARAM_BEGIN_DEPTH, width, nppn, mppdepth);
      }
    }

  command += buf;
  
  /* add the node list */
  command += node_list.c_str();
  
  /* end the reserve param element */
  command += APBASIL_RESERVE_PARAM_END;

  return(PBSE_NONE);
  } /* END save_current_reserve_param() */



/*
 * adjust_for_depth()
 *
 * pre-cond: nppn and width have been calculated based on the list of hosts
 * assigned to the job. This didn't account for the depth requested - 
 * @param depth means how many execution slots are used per task. 
 * @param nppn is the number of tasks per node.
 * @param width is the number of tasks for the whole job.
 * Dividing each by the depth produces the correct results.
 * @post-cond: nppn and width have been updated to account for the depth.
 */

void adjust_for_depth(

  unsigned int &width,
  unsigned int &nppn,
  int           depth)

  {
  if (depth != 0)
    {
    width /= depth;
    nppn  /= depth;
    }
  } /* END adjust_for_depth() */



int create_reserve_params_from_host_req_list(

  host_req_list    *list, /* I */
  char            *apbasil_protocol, /* I */
  int              use_nppn,      /* I */
  int              nppcu,         /* I */
  int              mppdepth,      /* I */
  std::string&     command,       /* O */
  std::string&     cray_frequency) /* I */

  {
  std::string    node_list = "";
  host_req       *hr;
  unsigned int    nppn = 0;
  unsigned int    width = 0;
  
  for(host_req_list::iterator iter = list->begin();iter != list->end();iter++)
    {
    hr = *iter;
    width += hr->ppn;
    nppn = MAX((unsigned int)nppn, hr->ppn);
    
    if (node_list.length() != 0)
      node_list += ",";
    
    node_list += hr->hostname;
    
    free_host_req(hr);
    }

  if (use_nppn == FALSE)
    nppn = -1;

  adjust_for_depth(width, nppn, mppdepth);
  
  save_current_reserve_param(command, apbasil_protocol, node_list, width, nppn, nppcu, mppdepth, cray_frequency);

  return(PBSE_NONE);
  } /* END create_reserve_params_from_host_req_list() */




int create_reserve_params_from_multi_req_list(

  char           *multi_req_list, /* I */
  char           *apbasil_protocol, /* I */
  int             nppcu,          /* I */
  int             mppdepth,       /* I */
  std::string&    command,        /* O */
  std::string&    cray_frequency) /* I */

  {
  std::string     node_list = "";
  char           *tok;
  char           *str = multi_req_list;
  int             node_count;
  char           *comma;
  unsigned int    width;
  unsigned int    nppn;
  
  while ((tok = threadsafe_tokenizer(&str, "*")) != NULL)
    {
    node_count = 1;

    node_list.clear();
    node_list += tok;
    
    comma = tok;
    while ((comma = strchr(comma+1, ',')) != NULL)
      node_count++;
    
    tok = threadsafe_tokenizer(&str, "|");
    nppn = atoi(tok);
    
    width = nppn * node_count;

    adjust_for_depth(width, nppn, mppdepth);

    save_current_reserve_param(command, apbasil_protocol, node_list, width, nppn, nppcu, mppdepth, cray_frequency);
    }

  return(PBSE_NONE);
  } /* END create_reserve_params_from_multi_req_list() */




void get_reservation_command(

  host_req_list   *list,
  char            *username,
  char            *jobid,
  char            *apbasil_path,
  char            *apbasil_protocol,
  char            *multi_req_list,
  int              use_nppn,
  int              nppcu,
  int              mppdepth,
  std::string&     command,
  std::string&     cray_frequency)

  {
  std::string      node_list = "";
  char            buf[MAXLINE * 2];

  command.clear();
  /* place the top header */
  snprintf(buf, sizeof(buf), APBASIL_RESERVE_REQ, 
    (apbasil_protocol != NULL) ? apbasil_protocol : DEFAULT_APBASIL_PROTOCOL);
  command += buf;

  /* place the reserve header */
  snprintf(buf, sizeof(buf), APBASIL_RESERVE_ARRAY, username, jobid);
  command += buf;

  if (multi_req_list == NULL)
    {
    create_reserve_params_from_host_req_list(list, apbasil_protocol, use_nppn, nppcu, mppdepth, command, cray_frequency);
    }
  else
    {
    /* no need to account for use_nppn here, this path always should */
    create_reserve_params_from_multi_req_list(multi_req_list, apbasil_protocol, nppcu, mppdepth, command, cray_frequency);
    }

  /* pipe the output to apbasil */
  snprintf(buf, sizeof(buf), "</ReserveParamArray></BasilRequest>\" | %s",
    (apbasil_path != NULL) ? apbasil_path : DEFAULT_APBASIL_PATH);
  command += buf;
  } /* END get_reservation_command() */



int find_error_type(

  xmlNode *node)

  {
  char    *attr_val = (char *)xmlGetProp(node, (const xmlChar *)error_class);
  int      rc = apbasil_fail_transient;
  xmlNode *child;
  xmlNode *grandchild;

  /* Make sure attr_val had a property before checking on it */
  if ((attr_val) && !strcmp(attr_val, "PERMANENT"))
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
          if ((errtext) && strstr(errtext, "error") != NULL)
            {
            snprintf(log_buffer, sizeof(log_buffer), "%s alps error: '%s'",
              (attr_val)?attr_val:"error was null", errtext);
            log_err(-1, __func__, log_buffer);
            }
          if (errtext)
            xmlFree((xmlChar *)errtext);
          }
        }
      }
    }

  if (attr_val)
    xmlFree((xmlChar *)attr_val);

  return(rc);
  } /* END find_error_type() */




int parse_reservation_output(

  const char  *output,
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

  const char  *command_str,
  char         **reservation_id)

  {
  int             rc;
  FILE           *alps_pipe;
  int             fd;
  std::string      output = "";
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
  while ((bytes_read = read(fd, tmpBuf, sizeof(tmpBuf) - 1)) > 0)
    {
    total_bytes_read += bytes_read;
    tmpBuf[bytes_read] = '\0';
    output += tmpBuf;
    }

  /* perform post-processing */
  pclose(alps_pipe);

  if ((bytes_read == -1) ||
      (total_bytes_read == 0))
    rc = READING_PIPE_ERROR;
  else
    {
    rc = parse_reservation_output(output.c_str(), reservation_id);
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
  char       *apbasil_protocol,
  char       *command_buf,
  int         command_buf_size)

  {
  int       rc;
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
    command_buf_size);


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




/*
 * create_alps_reservation
 * 
 * @param use_nppn - specifies whether or not to place nppn='X' in the alps request. 
 * If the user requested their resources using -l procs we shouldn't specify nppn because
 * if there are nodes of different core counts this can cause alps failures, because the
 * nppn would be the number of cores of the largest nodes, causing alps to reject the 
 * smaller nodes.
 * specify 
 */

int create_alps_reservation(

  char       *exec_hosts,
  char       *username,
  char       *jobid,
  char       *apbasil_path,
  char       *apbasil_protocol,
  long long   pagg_id_value,
  int         use_nppn,
  int         nppcu,
  int         mppdepth,
  char      **reservation_id,
  const char *mppnodes,
  std::string& cray_frequency)

  {
  host_req_list    *list;
  std::string      command = "";
  int              rc = 1;
  int              retry_count = 0;
  char            *user = strdup(username);
  char            *aroba;

  if ((aroba = strchr(user, '@')) != NULL)
    *aroba = '\0';

  if (strchr(exec_hosts, '|') == NULL)
    {
    list = parse_exec_hosts(exec_hosts,mppnodes);
    
    if (list->size() == 0)
      {
      free(user);
      delete list;
      /* this is a login only job */
      return(PBSE_NONE);
      }
  
    get_reservation_command(list, user, jobid, apbasil_path, apbasil_protocol, NULL, use_nppn, nppcu, mppdepth,command,cray_frequency);
  
    delete list;
    }
  else
    {
    get_reservation_command(NULL, user, jobid, apbasil_path, apbasil_protocol, exec_hosts, use_nppn, nppcu, mppdepth,command,cray_frequency);
    }

  free(user);

  /* retry on failure up to  */
  while ((retry_count++ < APBASIL_RETRIES) &&
         (rc != apbasil_fail_permanent) &&
         (rc != PBSE_NONE))
    {
    rc = execute_reservation(command.c_str(), reservation_id);

    if (rc != PBSE_NONE)
      usleep(100);
    }

  if (rc == PBSE_NONE)
    {
    char confirm_command_buf[MAXLINE * 2];

    if (LOGLEVEL >= 3)
      {
      snprintf(log_buffer, sizeof(log_buffer),
        "Successful reservation command is: %s", command.c_str());
      log_event(PBSEVENT_JOB | PBSEVENT_SYSLOG, PBS_EVENTCLASS_JOB, __func__, log_buffer);
      }

    rc = 1;
    retry_count = 0;
  
    while ((retry_count++ < APBASIL_RETRIES) &&
           (rc != PBSE_NONE))
      {
      rc = confirm_reservation(jobid, *reservation_id, pagg_id_value, apbasil_path, apbasil_protocol,
                               confirm_command_buf, sizeof(confirm_command_buf));

      if (rc != PBSE_NONE)
        usleep(100);
      }

    if (rc != PBSE_NONE)
      {
      snprintf(log_buffer, sizeof(log_buffer),
        "Failed confirmation command is: %s", confirm_command_buf);
      log_err(-1, __func__, confirm_command_buf);
      }
    else if (LOGLEVEL >= 3)
      {
      snprintf(log_buffer, sizeof(log_buffer),
        "Successful confirmation command is: %s", confirm_command_buf);
      log_event(PBSEVENT_JOB | PBSEVENT_SYSLOG, PBS_EVENTCLASS_JOB, __func__, log_buffer);
      }
    }
  else
    {
    snprintf(log_buffer, sizeof(log_buffer),
      "Failed reservation command is: %s", command.c_str());
    log_err(-1, __func__, log_buffer);
    }

  return(rc);
  } /* END create_alps_reservation() */

