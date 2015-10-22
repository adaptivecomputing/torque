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

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <libxml/parser.h>
#include <libxml/tree.h>
#include "utils.h"
#include "alps_constants.h"
#include <string>
#include <vector>
#include <set>
#include <time.h>

#include "../lib/Libifl/lib_ifl.h"

std::set<std::string> down_nodes;


/*
 * for now, do nothing with reservations
 */
int process_reservations(

  std::vector<std::string> &status,
  xmlNode                  *node)

  {
  return(PBSE_NONE);
  } /* END process_reservations() */



xmlNode *find_processor_allocation(

  xmlNode *child)

  {
  xmlNode *processor_allocation;

  for (processor_allocation = child->children; processor_allocation != NULL; processor_allocation = processor_allocation->next)
    {
    if (!strcmp((char *)processor_allocation->name, ProcessorAllocation))
      break;
    }

  return(processor_allocation);
  } /* find_processor_allocation() */



int process_processor_array(

  xmlNode  *node,
  int      *total_procs,
  int      *total_avail_procs,
  char    **rsv_id)

  {
  xmlNode  *child;
  xmlNode  *processor_allocation;
  int       num_procs = 0;
  int       avail_procs = 0;
  
  for (child = node->children; child != NULL; child = child->next)
    {
    if (!strcmp((const char *)child->name, processor))
      {
      num_procs++;
      
      /* children would be Processorallocation. If this doesn't exist, then count as
       * available */
      if ((processor_allocation = find_processor_allocation(child)) == NULL)
        avail_procs++;
      else if (*rsv_id == NULL)
        *rsv_id = (char *)xmlGetProp(processor_allocation, (const xmlChar *)reservation_id);
      }
    }

  *total_procs       += num_procs;
  *total_avail_procs += avail_procs;

  return(PBSE_NONE);
  } /* END process_processor_array() */




int process_memory_array(

  xmlNode       *node,
  unsigned long *memory)

  {
  int            page_size;
  unsigned long  page_count;
  char          *attr_value;
  xmlNode       *child;

  /* look at each memory child */
  for (child = node->children; child != NULL; child = child->next)
    {
    if (!strcmp((const char *)child->name, memory_name))
      {
      attr_value = (char *)xmlGetProp(child, (const xmlChar *)"page_size_kb");
      page_size  = atoi(attr_value);
      free(attr_value);
      
      attr_value = (char *)xmlGetProp(child, (const xmlChar *)"page_count");
      page_count = strtol(attr_value, NULL, 10);
      free(attr_value);
      
      /* note: CMEMORY is in megabytes */
      *memory += page_size * (page_count / 1024);
      }
    }

  return(PBSE_NONE);
  } /* END process_memory_array() */




int process_label_array(

  std::string&   feature_list,
  xmlNode        *node)

  {
  char     *attr_value;
  xmlNode  *child;

  for (child = node->children; child != NULL; child = child->next)
    {
    if ((attr_value = (char *)xmlGetProp(child, (const xmlChar *)"name")) != NULL)
      {
      if (!strncmp(attr_value, "MOAB:FEATURE=", strlen("MOAB:FEATURE=")))
        {
        char *feature_name = attr_value + strlen("MOAB:FEATURE=");
        if (feature_list.length() == 0)
          {
          feature_list += feature_name;
          }
        else
          {
          /* this can be called multiple times per node. Make sure the same feature
           * isn't inserted twice */
          const char *str = strstr(feature_list.c_str(), feature_name);
          bool        found = false;

          if (str != NULL)
            {
            if ((str == feature_list.c_str()) ||
                (*(str - 1) == ','))
              {
              int feature_name_len = strlen(feature_name);

              if ((str + feature_name_len > feature_list.c_str() + feature_list.length()) ||
                  (*(str + feature_name_len) == ',') ||
                  (*(str + feature_name_len) == '\0'))
                found = true;
              }
            }

          if (found == false)
            {
            feature_list += ",";
            feature_list += feature_name;
            }
          }
        }

      free(attr_value);
      }
    }

  return(PBSE_NONE);
  } /* END process_label_array() */




int process_accelerator_array(

  std::vector<std::string> &status,
  xmlNode                  *node)

  {
  xmlNode       *child;
  char          *attr_value;
  char          *attr_value2;
  char           buf[MAXLINE];

  status.push_back(CRAY_GPU_STATUS_START);

  for (child = node->children; child != NULL; child = child->next)
    {
    if (!strcmp((const char *)child->name, accelerator))
      {
      std::string str;
      /* write the gpu id */
      attr_value = (char *)xmlGetProp(child, (const xmlChar *)type);
      attr_value2 = (char *)xmlGetProp(child, (const xmlChar *)ordinal);
      snprintf(buf, sizeof(buf), "%s-%d", attr_value, atoi(attr_value2));
      str = "gpu_id=";
      str += buf;
      status.push_back(str);
      free(attr_value);
      free(attr_value2);

      attr_value = (char *)xmlGetProp(child, (const xmlChar *)state);
      str = "state=";
      str += attr_value;
      status.push_back(str);
      free(attr_value);

      attr_value = (char *)xmlGetProp(child, (const xmlChar *)family);
      str = "family=";
      str += attr_value;
      status.push_back(str);
      free(attr_value);

      attr_value = (char *)xmlGetProp(child, (const xmlChar *)memory_mb);
      str = "memory=";
      str += attr_value;
      str += "mb";
      status.push_back(str);
      free(attr_value);

      attr_value = (char *)xmlGetProp(child, (const xmlChar *)clock_mhz);
      str = "clock_mhz=";
      str += attr_value;
      status.push_back(str);
      free(attr_value);
      }
    }
  
  status.push_back(CRAY_GPU_STATUS_END);

  return(PBSE_NONE);
  } /* END process_accelerator_array() */




int process_node(

  std::vector<std::string> &status,
  xmlNode                  *node)

  {
  char               *attr_value;
  char               *role_value;
  xmlNode            *child;
  xmlNode            *segments;
  xmlNode            *segment_child;
  xmlNode            *sockets;
  xmlNode            *socket_child;
  xmlNode            *compute_units;
  xmlNode            *compute_unit_child;
  std::string        features = "";
  std::string        node_state = "";
  std::string        node_name = "";
  std::string        node_id_string  = "";
  char                buf[MAXLINE];
  int                 num_procs         = 0;
  int                 avail_procs       = 0;
  int                 num_compute_units = 0;
  unsigned long       memory            = 0;
  unsigned long long  mem_kb;
  char               *rsv_id        = NULL;
  std::set<std::string>::iterator iter;

    {
    std::string str("node=");
    attr_value = (char *)xmlGetProp(node, (const xmlChar *)node_id);
    node_id_string = attr_value;
    str += attr_value;
    status.push_back(str);
    free(attr_value);
    }

  /* check to see if the role is interactive - report these as down */
  role_value = (char *)xmlGetProp(node, (const xmlChar *)role);

    {
    std::string str("ARCH=");
    attr_value = (char *)xmlGetProp(node, (const xmlChar *)architecture);
    str += attr_value;
    status.push_back(str);
    free(attr_value);
    }

    {
    std::string str("name=");
    attr_value = (char *)xmlGetProp(node, (const xmlChar *)name);
    node_name = attr_value;
    str += attr_value;
    status.push_back(str);
    free(attr_value);
    }

  /* process the children */
  for (child = node->children; child != NULL; child = child->next)
    {

    /* 1.3 socket (does not exist in previous protocol versions) */
    if (!strcmp((const char *)child->name, socket_array))
      {
      for (sockets = child->children; sockets != NULL; sockets = sockets->next)
        {
        for (socket_child = sockets->children; socket_child != NULL; socket_child = socket_child->next)
          {
          if (!strcmp((const char *)socket_child->name, segment_array))
            {
            for (segments = socket_child->children; segments != NULL; segments = segments->next)
              {
              for (segment_child = segments->children; segment_child != NULL; segment_child = segment_child->next)
                {
                if (!strcmp((const char *)segment_child->name, compute_unit_array))
                  {
                  for (compute_units = segment_child->children; compute_units != NULL; compute_units = compute_units->next)
                    {
                    /* tally compute units */
                    if (!strcmp((const char *)compute_units->name, compute_unit))
                      num_compute_units++;

                    for (compute_unit_child = compute_units->children; compute_unit_child != NULL; compute_unit_child = compute_unit_child->next)
                      {
                      if (!strcmp((const char *)compute_unit_child->name, processor_array))
                        process_processor_array(compute_unit_child, &num_procs, &avail_procs, &rsv_id);
                      }
                    }
                  }
                else if (!strcmp((const char *)segment_child->name, memory_array))
                  process_memory_array(segment_child, &memory);
                else if (!strcmp((const char *)segment_child->name, label_array))
                  process_label_array(features, segment_child);
                }
              }
            }
          }
        }
      }

    /* 1.1/1.2 segment */
    else if (!strcmp((const char *)child->name, segment_array))
      {
      for (segments = child->children; segments != NULL; segments = segments->next)
        {
        for (segment_child = segments->children; segment_child != NULL; segment_child = segment_child->next)
          {
          if (!strcmp((const char *)segment_child->name, processor_array))
            process_processor_array(segment_child, &num_procs, &avail_procs, &rsv_id);
          else if (!strcmp((const char *)segment_child->name, memory_array))
            process_memory_array(segment_child, &memory);
          else if (!strcmp((const char *)segment_child->name, label_array))
            process_label_array(features, segment_child);
          }
        }
      }

    /* 1.0 tags (no SegmentArray tag) */
    else if (!strcmp((const char *)child->name, processor_array))
      {
      process_processor_array(child, &num_procs, &avail_procs, &rsv_id);
      }
    else if (!strcmp((const char *)child->name, memory_array))
      {
      process_memory_array(child, &memory);
      }
    else if (!strcmp((const char *)child->name, label_array))
      {
      process_label_array(features, child);
      }

    /* 1.0-1.3 */
    else if (!strcmp((const char *)child->name, accelerator_array))
      {
      process_accelerator_array(status, child);
      }
    } /* END the loop for processing the children */

  /* once done, add the procs, available procs, compute unit count, memory info, reservation, and features */

  /* note that CCU should come before CPROC */
  snprintf(buf, sizeof(buf), "CCU=%d", num_compute_units);
  status.push_back(buf);

  snprintf(buf, sizeof(buf), "CPROC=%d", num_procs);
  status.push_back(buf);

  snprintf(buf, sizeof(buf), "APROC=%d", avail_procs);
  status.push_back(buf);

  snprintf(buf, sizeof(buf), "CMEMORY=%lu", memory);
  status.push_back(buf);

  mem_kb = memory * 1024;

  snprintf(buf, sizeof(buf), "totmem=%llukb", mem_kb);
  status.push_back(buf);

  snprintf(buf, sizeof(buf), "physmem=%llukb", mem_kb);
  status.push_back(buf);

  if (rsv_id != NULL)
    {
    /* only write the reservation id if we're in batch mode (or role was not supplied) */
    if ((role_value == NULL) ||
        (!strcmp(role_value, batch_caps)))
      {
      std::string str("reservation_id=");
      str += rsv_id;
      status.push_back(str);
      }

    free(rsv_id);

    /* if there's a reservation on this node, the state is busy */
    node_state = "BUSY";
    status.push_back("state=BUSY");
    
    snprintf(buf, sizeof(buf), "availmem=0kb");
    status.push_back("availmem=0kb");
    }
  else
    {
    /* no reservation, evaluate the state normally */
    std::string str("state=");
    attr_value = (char *)xmlGetProp(node, (const xmlChar *)state);
    

    /* state is down if we're not in batch mode */
    if ((role_value != NULL) &&
        (strcmp(role_value, batch_caps)))
      {
      node_state = "DOWN";
      str += "DOWN";
      status.push_back(str);
      


      snprintf(buf, sizeof(buf), "availmem=0kb");
      status.push_back(buf);
      }
    else
      {
      str += attr_value;
      node_state = attr_value;
      status.push_back(str);
     
      snprintf(buf, sizeof(buf), "availmem=%llukb", mem_kb);
      status.push_back(buf);
      }

    free(attr_value);
    }

  /* Record whenever an ALPS node is reported as entering a DOWN state */
  if (LOGLEVEL >= 3)
    {
    iter = down_nodes.find(node_id_string);

    /* Check to see if a node's state has changed. All nodes in a DOWN state are
     * recorded in the down_nodes structure. We can see if the node has changed status
     * based on whether its ID can be found in the down_nodes structure.
     */
    if (node_state == "DOWN")
      {
      if (iter == down_nodes.end())
        {
        down_nodes.insert(node_id_string);

        snprintf(buf, sizeof(buf), "ALPS node %s (%s) is reported as being DOWN", node_name.c_str(), node_id_string.c_str());
        log_record(PBSEVENT_SYSTEM, PBS_EVENTCLASS_NODE, __func__, buf);
        }
      }
    else
      {
      if (iter != down_nodes.end())
        {
        snprintf(buf, sizeof(buf), "ALPS node %s (%s) is online again", node_name.c_str(), node_id_string.c_str());
        log_record(PBSEVENT_SYSTEM, PBS_EVENTCLASS_NODE, __func__, buf);
        down_nodes.erase(iter);
        }
      }

    }

  /* Verbose debug output, log every single ALPS node's state */
  if (LOGLEVEL >= 7)
    {
    snprintf(buf, sizeof(buf), "ALPS node %s (%s) is reported as being %s", node_name.c_str(), node_id_string.c_str(), node_state.c_str());
    log_record(PBSEVENT_SYSTEM, 0, __func__, buf);
    }

  if (role_value != NULL)
    free(role_value);

  if (features.length() > 0)
    {
    std::string str("feature_list=");
    str += features.c_str();
    status.push_back(str);
    }

  return(PBSE_NONE);
  } /* END process_node() */





int process_nodes(

  std::vector<std::string> &status,
  xmlNode                  *node)

  {
  xmlNode *current;

  for (current = node; current != NULL; current = current->next)
    {
    if (!strcmp((const char *)current->name, node_name))
      process_node(status, current);
    }

  return(PBSE_NONE);
  } /* END process_nodes() */





int process_element(

  std::vector<std::string> &status,
  xmlNode                  *node)

  {
  char *str;
  int   rc = PBSE_NONE;

  if (node == NULL)
    return(PBSE_NONE);

  if (!strcmp((const char *)node->name, response_data))
    {
    str = (char *)xmlGetProp(node, (const xmlChar *)"status");
    if (strcmp(str, success))
      {
      free(str);
      return(ALPS_QUERY_FAILURE);
      }
    else
      rc = process_element(status, node->children);

    free(str);
    }
  else if (!strcmp((const char *)node->name, node_array))
    {
    rc = process_nodes(status, node->children);
    rc = process_element(status, node->next);
    }
  else if (!strcmp((const char *)node->name, reservation_array))
    {
    rc = process_reservations(status, node->children);
    }
  else if (!strcmp((const char *)node->name, text_name))
    {
    rc = process_element(status, node->next);
    }
  else
    {
    /* move on to the next child */
    rc = process_element(status, node->children);
    }

  return(rc);
  } /* END process_element() */




int parse_alps_output(

  std::string              &alps_output,
  std::vector<std::string> &status)

  {
  xmlDocPtr  doc;

  if ((doc = xmlReadMemory(alps_output.c_str(), alps_output.length(), "apbasil", NULL, 0)) == NULL)
    {
    char buf[MAXLINE * 4];
    xmlErrorPtr pErr = xmlGetLastError();
    snprintf(buf, sizeof(buf), "Failed to parse the output of alps - %s", pErr->message);
    log_err(-1, __func__, buf);
    return(ALPS_PARSING_ERROR);
    }

  if (process_element(status, xmlDocGetRootElement(doc)) == ALPS_QUERY_FAILURE)
    {
  	snprintf(log_buffer, sizeof(log_buffer), "Failed to query alps");
	  log_err(-1, __func__, log_buffer);
  	return(ALPS_QUERY_FAILURE);
    }


  xmlFreeDoc(doc);
  xmlMemoryDump();

  return(PBSE_NONE);
  } /* END parse_alps_output() */




int generate_alps_status(

  std::vector<std::string> &status,
  const char               *apbasil_path,
  const char               *apbasil_protocol)

  {
  FILE           *alps_pipe;
  int             fd;
  int             rc;
  int             bytes_read;
  int             total_bytes_read = 0;
  char            inventory_command[MAXLINE * 2];
  char            input_buffer[MAXLINE];
  char           *ptr;
  std::string     alps_output = "";

  status.clear();

  snprintf(inventory_command, sizeof(inventory_command), APBASIL_QUERY,
    (apbasil_protocol != NULL) ? apbasil_protocol : DEFAULT_APBASIL_PROTOCOL,
    (apbasil_path != NULL) ? apbasil_path : DEFAULT_APBASIL_PATH);

  time_t time_to_respond = time(0);

  if ((alps_pipe = popen(inventory_command, "r")) == NULL)
    {
    snprintf(log_buffer, sizeof(log_buffer),
      "Unable to open command %s for apbasil",
      inventory_command);
    log_err(errno, __func__, log_buffer);

    return(WRITING_PIPE_ERROR);
    }

  fd = fileno(alps_pipe);

  /* now read from the pipe */
  ptr = input_buffer;
  memset(input_buffer, 0, sizeof(input_buffer));

  while ((bytes_read = read(fd, ptr, sizeof(input_buffer) - 1)) > 0)
    {
    alps_output += ptr;
    memset(input_buffer, 0, sizeof(input_buffer));
    total_bytes_read += bytes_read;
    }

  /* perform post-processing */
  time_to_respond = time(0) - time_to_respond;

  pclose(alps_pipe);

  if (LOGLEVEL >= 6)
    {
    snprintf(log_buffer, sizeof(log_buffer), "Query took %lu seconds to respond", time_to_respond);
    log_event(PBSEVENT_SYSTEM | PBSEVENT_SYSLOG, PBS_EVENTCLASS_SERVER, __func__, log_buffer);
    }

  if ((bytes_read == -1) ||
      (total_bytes_read == 0))
    rc = READING_PIPE_ERROR;
  else
    {
    int index = alps_output.length() - 1;
    while (alps_output.c_str()[index] != '>')
      {
      alps_output.resize(index);
      index--;
      }

    rc = parse_alps_output(alps_output, status);

    if (rc == PBSE_NONE)
      {
      if (LOGLEVEL >= 7)
        {
        /* log the command if output successfully parsed */
        snprintf(log_buffer, sizeof(log_buffer),
          "Successful inventory command is: %s", inventory_command);
        log_event(PBSEVENT_JOB | PBSEVENT_SYSLOG, PBS_EVENTCLASS_JOB, __func__, log_buffer);
        }
      }
    }

  return(rc);
  } /* END generate_alps_status() */

