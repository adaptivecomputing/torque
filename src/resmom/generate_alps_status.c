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
#include "alps_functions.h"
#include <string>
#include <vector>
#include <set>
#include <time.h>

#include "lib_ifl.h"

std::set<std::string> down_nodes;
std::map<int, alps_node_info> alps_nodes;


/*
 * for now, do nothing with reservations
 */
int process_reservations(

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
  unsigned long &memory)

  {
  int            page_size = 0;
  unsigned long  page_cnt = 0;
  char          *attr_value;
  xmlNode       *child;

  /* look at each memory child */
  for (child = node->children; child != NULL; child = child->next)
    {
    if (!strcmp((const char *)child->name, memory_name))
      {
      attr_value = (char *)xmlGetProp(child, (const xmlChar *)page_size_kb);
      page_size  = atoi(attr_value);
      free(attr_value);
      
      attr_value = (char *)xmlGetProp(child, (const xmlChar *)page_count);
      if (attr_value != NULL)
        {
        page_cnt = strtol(attr_value, NULL, 10);
        free(attr_value);
        }

      /* note: CMEMORY is in megabytes */
      memory += page_size * (page_cnt / 1024);
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

  alps_node_info &ani,
  xmlNode        *node)

  {
  xmlNode       *child;
  char          *attr_value;
  char          *attr_value2;
  char           buf[MAXLINE];

  for (child = node->children; child != NULL; child = child->next)
    {
    if (!strcmp((const char *)child->name, accelerator))
      {
      alps_accelerator_info aai;

      std::string str;
      /* write the gpu id */
      attr_value = (char *)xmlGetProp(child, (const xmlChar *)type);
      attr_value2 = (char *)xmlGetProp(child, (const xmlChar *)ordinal);
      snprintf(buf, sizeof(buf), "%s-%d", attr_value, atoi(attr_value2));
      aai.accel_id = "gpu_id=";
      aai.accel_id += buf;
      free(attr_value);
      free(attr_value2);

      attr_value = (char *)xmlGetProp(child, (const xmlChar *)state);
      aai.accel_state = "state=";
      aai.accel_state += attr_value;
      free(attr_value);

      attr_value = (char *)xmlGetProp(child, (const xmlChar *)family);
      aai.family = "family=";
      aai.family += attr_value;
      free(attr_value);

      attr_value = (char *)xmlGetProp(child, (const xmlChar *)memory_mb);
      aai.memory = "memory=";
      aai.memory += attr_value;
      aai.memory += "mb";
      free(attr_value);

      attr_value = (char *)xmlGetProp(child, (const xmlChar *)clock_mhz);
      aai.clock_mhz = "clock_mhz=";
      aai.clock_mhz += attr_value;
      free(attr_value);

      ani.accelerators.push_back(aai);
      }
    }

  return(PBSE_NONE);
  } /* END process_accelerator_array() */



int process_node(

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
  std::string         features = "";
  std::string         node_state = "";
  std::string         node_name;
  std::string         node_id_string;
  int                 nid;
  std::string         numa_cfg_val;
  std::string         mcdram_cfg_val;
  char                buf[MAXLINE];
  int                 num_procs         = 0;
  int                 avail_procs       = 0;
  int                 num_compute_units = 0;
  int                 socket_count      = 0;
  unsigned long       memory            = 0;
  unsigned long long  mem_kb;
  char               *rsv_id        = NULL;
  alps_node_info      ani;
  std::set<std::string>::iterator iter;
    
  ani.node_header = "node=";
  attr_value = (char *)xmlGetProp(node, (const xmlChar *)node_id);
  if (attr_value != NULL)
    {
    node_id_string = attr_value;
    ani.node_header += node_id_string;
    nid = strtol(attr_value, NULL, 10);
    free(attr_value);
    }

  /* check to see if the role is interactive - report these as down */
  role_value = (char *)xmlGetProp(node, (const xmlChar *)role);

  ani.arch = "ARCH=";
  attr_value = (char *)xmlGetProp(node, (const xmlChar *)architecture);
  if (attr_value != NULL)
    {
    ani.arch += attr_value;
    free(attr_value);
    }

  ani.name = "name=";
  attr_value = (char *)xmlGetProp(node, (const xmlChar *)name);
  if (attr_value != NULL)
    {
    node_name = attr_value;
    ani.name += node_name;
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
        if (!strcmp((const char *)sockets->name, socket_name))
          {
          socket_count++;

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
                    process_memory_array(segment_child, memory);
                  else if (!strcmp((const char *)segment_child->name, label_array))
                    process_label_array(features, segment_child);
                  }
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
            process_memory_array(segment_child, memory);
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
      process_memory_array(child, memory);
      }
    else if (!strcmp((const char *)child->name, label_array))
      {
      process_label_array(features, child);
      }

    /* 1.0-1.3 */
    else if (!strcmp((const char *)child->name, accelerator_array))
      {
      process_accelerator_array(ani, child);
      }
    } /* END the loop for processing the children */

  if (socket_count == 0)
    socket_count = 1;

  /* once done, add the procs, available procs, compute unit count, memory info, reservation, and features */

  /* note that CCU should come before CPROC */
  snprintf(buf, sizeof(buf), "CCU=%d", num_compute_units);
  ani.ccu = buf;

  snprintf(buf, sizeof(buf), "CPROC=%d", num_procs);
  ani.cproc = buf;

  snprintf(buf, sizeof(buf), "APROC=%d", avail_procs);
  ani.aproc = buf;

  snprintf(buf, sizeof(buf), "CMEMORY=%lu", memory);
  ani.cmem = buf;

  snprintf(buf, sizeof(buf), "socket=%d", socket_count);
  ani.socket = buf;

  mem_kb = memory * 1024;

  snprintf(buf, sizeof(buf), "totmem=%llukb", mem_kb);
  ani.totmem = buf;

  snprintf(buf, sizeof(buf), "physmem=%llukb", mem_kb);
  ani.physmem = buf;

  if (rsv_id != NULL)
    {
    /* only write the reservation id if we're in batch mode (or role was not supplied) */
    if ((role_value == NULL) ||
        (!strcmp(role_value, batch_caps)))
      {
      ani.rsv = "reservation_id=";
      ani.rsv += rsv_id;
      }

    free(rsv_id);

    // if there's a reservation on this node, the state is busy and no memory is available
    node_state = "BUSY";
    ani.state = "state=BUSY";
    ani.availmem = "availmem=0kb";
    }
  else
    {
    /* no reservation, evaluate the state normally */
    ani.state = "state=";
    attr_value = (char *)xmlGetProp(node, (const xmlChar *)state);

    /* state is down if we're not in batch mode */
    if ((role_value != NULL) &&
        (strcmp(role_value, batch_caps)))
      {
      node_state = "DOWN";
      ani.state += "DOWN";
    
      ani.availmem = "availmem=0kb";
      }
    else
      {
      ani.state += attr_value;
      node_state = attr_value;
     
      snprintf(buf, sizeof(buf), "availmem=%llukb", mem_kb);
      ani.availmem = buf;
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
    ani.features = "feature_list=";
    ani.features += features.c_str();
    }

  char node_index_buf[MAXLINE];
  snprintf(node_index_buf, sizeof(node_index_buf), "node_index=%lu", alps_nodes.size());
  ani.node_index = node_index_buf;

  alps_nodes[nid] = ani;

  return(PBSE_NONE);
  } /* END process_node() */



int process_nodes(

  xmlNode                  *node)

  {
  xmlNode *current;

  for (current = node; current != NULL; current = current->next)
    {
    if (!strcmp((const char *)current->name, node_name))
      process_node(current);
    }

  return(PBSE_NONE);
  } /* END process_nodes() */



void get_os_value(

  std::string       &os_value,
  const std::string &numa_cfg_val,
  std::string       &mcdram_cfg_val)

  {

  if ((numa_cfg_val.size() != 0) &&
      (mcdram_cfg_val.size() != 0))
    {
    os_value = "opsys=CLE_";

    if (mcdram_cfg_val == "0")
      mcdram_cfg_val = "flat";
    else if (mcdram_cfg_val == "25")
      mcdram_cfg_val = "split";
    else if (mcdram_cfg_val == "50")
      mcdram_cfg_val = "equal";
    else if (mcdram_cfg_val == "100")
      mcdram_cfg_val = "cache";

    os_value += numa_cfg_val + "_";
    os_value += mcdram_cfg_val;
    }
  else
    os_value = "opsys=";

  } // END get_os_value()



int get_node_info(

  xmlNode        *node,
  alps_node_info &ani)

  {
  char           *attr_value;
  std::string    numa_cfg_val;
  std::string    mcdram_cfg_val;
  unsigned long  hbm_size_in_mb = 0;
  char           buf[MAXLINE];
  
  attr_value = (char *)xmlGetProp(node, (const xmlChar *)numa_config);
  if (attr_value != NULL)
    {
    numa_cfg_val = attr_value;
    free(attr_value);
    }
          
  attr_value = (char *)xmlGetProp(node, (const xmlChar *)hbm_cache_pct);
  if (attr_value != NULL)
    {
    mcdram_cfg_val = attr_value;
    free(attr_value);
    }
  
  attr_value = (char *)xmlGetProp(node, (const xmlChar *)hbm_size);
  if (attr_value != NULL)
    {
    hbm_size_in_mb = strtol(attr_value, NULL, 10);
    free(attr_value);
    }

  attr_value = (char *)xmlGetProp(node, (const xmlChar *)numas);
  if (attr_value != NULL)
    {
    ani.numa_nodes = numas;
    ani.numa_nodes += "=";
    ani.numa_nodes += attr_value;
    free(attr_value);
    }

  get_os_value(ani.os, numa_cfg_val, mcdram_cfg_val);

  snprintf(buf, sizeof(buf), "hbmem=%lukb", hbm_size_in_mb * 1024);
  ani.hbm = buf;

  return(PBSE_NONE);
  } // END get_node_info()



int process_system(

  xmlNode *node)

  {
  std::string cpus_per_cu;
  xmlNode        *child;
  alps_node_info  ani;
	  
  for (child = node->children; child != NULL; child = child->next)
    {
    if (!strcmp((const char *)child->name, nodes_element))
      {
      // Get information for this batch of nodes
      get_node_info(child, ani);

      // Get the range of nodes this information applies to
      char *content = (char *)xmlNodeGetContent(child);
      if (content != NULL)
        {
        std::vector<int> node_ids;
        translate_range_string_to_vector(content, node_ids);

        for (unsigned int i = 0; i < node_ids.size(); i++)
          {
          if (alps_nodes.find(node_ids[i]) != alps_nodes.end())
            {
            alps_nodes[node_ids[i]].os = ani.os;
            alps_nodes[node_ids[i]].hbm = ani.hbm;
            alps_nodes[node_ids[i]].numa_nodes = ani.numa_nodes;
            }
          else
            alps_nodes[node_ids[i]] = ani;
          }

        free(content);
        }
      }
    }
  
  return(PBSE_NONE);  
  } // END process_system()



int process_element(

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
      rc = process_element(node->children);

    free(str);
    }
  else if (!strcmp((const char *)node->name, node_array))
    {
    rc = process_nodes(node->children);
    rc = process_element(node->next);
    }
  else if (!strcmp((const char *)node->name, reservation_array))
    {
    rc = process_reservations(node->children);
    }
  else if (!strcmp((const char *)node->name, text_name))
    {
    rc = process_element(node->next);
    }
  else if (!strcmp((const char *)node->name, system_tag))
    {
    rc = process_system(node);
    }
  else
    {
    /* move on to the next child */
    rc = process_element(node->children);
    }

  return(rc);
  } /* END process_element() */



int parse_alps_output(

  std::string              &alps_output)

  {
  xmlDocPtr  doc;
  xmlNode   *child;

  if ((doc = xmlReadMemory(alps_output.c_str(), alps_output.length(), "apbasil", NULL, 0)) == NULL)
    {
    char buf[MAXLINE * 4];
    xmlErrorPtr pErr = xmlGetLastError();
    snprintf(buf, sizeof(buf), "Failed to parse the output of alps - %s", pErr->message);
    log_err(-1, __func__, buf);
    return(ALPS_PARSING_ERROR);
    }

  if (process_element(xmlDocGetRootElement(doc)) == ALPS_QUERY_FAILURE)
    {
    xmlNode   *root = xmlDocGetRootElement(doc);
	  // Verbose debug output for ALPS_QUERY_FAILURE node error message
	  for (child = root->children; child != NULL; child = child->next)
	    {
	    if (!strcmp((const char *)child->name, response_data))
	      {
        for (xmlNode *gchild = child->children; gchild != NULL; gchild = gchild->next)
          {
          if (!strcmp((const char *)gchild->name, "Message"))
            {
            snprintf(log_buffer, sizeof(log_buffer),
              "Failed to query ALPS: %s", (const char *)xmlNodeGetContent(gchild));
            log_record(PBSEVENT_SYSTEM, 0, __func__, log_buffer);
            }
          }
	      }
	    }

  	return(ALPS_QUERY_FAILURE);
    }


  xmlFreeDoc(doc);
  xmlMemoryDump();

  return(PBSE_NONE);
  } /* END parse_alps_output() */



int get_command_output(

  const char  *command,
  std::string &output)

  {
  FILE           *alps_pipe;
  int             fd;
  int             rc = PBSE_NONE;
  char            input_buffer[MAXLINE];
  time_t          time_to_respond = time(0);
  char           *ptr;
  int             bytes_read;
  int             total_bytes_read = 0;

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
  ptr = input_buffer;
  memset(input_buffer, 0, sizeof(input_buffer));

  while ((bytes_read = read(fd, ptr, sizeof(input_buffer) - 1)) > 0)
    {
    output += ptr;
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
    int index = output.length() - 1;
    while (output.c_str()[index] != '>')
      {
      output.resize(index);
      index--;
      }
    }

  return(rc);
  } // END get_command_output()



void alps_accelerator_info::add_to_status(

  std::vector<std::string> &status)

  {
  status.push_back(this->accel_id);
  status.push_back(this->accel_state);
  status.push_back(this->family);
  status.push_back(this->memory);
  status.push_back(this->clock_mhz);
  } // END add_to_status()



void alps_node_info::add_to_status(

  std::vector<std::string> &status)

  {
  status.push_back(this->node_header);
  status.push_back(this->node_index);
  status.push_back(this->availmem);
  status.push_back(this->state);
  
  if (this->os.size() != 0)
    status.push_back(this->os);

  status.push_back(this->physmem);
  status.push_back(this->totmem);
  status.push_back(this->socket);

  if (this->numa_nodes.size() != 0)
    status.push_back(this->numa_nodes);

  status.push_back(this->cmem);
  // ccu should be added before cproc
  status.push_back(this->ccu);
  status.push_back(this->aproc);
  status.push_back(this->cproc);
  status.push_back(this->name);
  status.push_back(this->arch);

  // The following attributes only exist conditionally
  if (this->hbm.size() > 0)
    status.push_back(this->hbm);

  if (this->rsv.size() > 0)
    status.push_back(this->rsv);

  if (this->features.size() > 0)
    status.push_back(this->features);

  if (this->accelerators.size() > 0)
    {
    status.push_back(CRAY_GPU_STATUS_START);

    for (unsigned int i = 0; i < this->accelerators.size(); i++)
      this->accelerators[i].add_to_status(status);

    status.push_back(CRAY_GPU_STATUS_END);
    }

  } // END add_to_status()



void update_status(

  std::vector<std::string> &status)

  {
  for (std::map<int, alps_node_info>::iterator it = alps_nodes.begin(); it != alps_nodes.end(); it++)
    {
    it->second.add_to_status(status);
    }
  } // END update_status



/*
 * get_knl_information()
 *
 */

int get_knl_information(

  const char *apbasil_path)

  {
  int         rc;
  std::string knl_output;
  char        system_command[MAXLINE * 2];

  snprintf(system_command, sizeof(system_command), APBASIL_SYSTEM,
    "1.7",
    (apbasil_path != NULL) ? apbasil_path : DEFAULT_APBASIL_PATH);

  rc = get_command_output(system_command, knl_output);

  if (rc == PBSE_NONE)
    rc = parse_alps_output(knl_output);

  return(rc);
  } // END get_knl_information()



int generate_alps_status(

  std::vector<std::string> &status,
  const char               *apbasil_path,
  const char               *apbasil_protocol)

  {
  int             rc;
  char            inventory_command[MAXLINE * 2];
  std::string     alps_output;

  status.clear();
  alps_nodes.clear();

  snprintf(inventory_command, sizeof(inventory_command), APBASIL_QUERY,
    (apbasil_protocol != NULL) ? apbasil_protocol : DEFAULT_APBASIL_PROTOCOL,
    (apbasil_path != NULL) ? apbasil_path : DEFAULT_APBASIL_PATH);

  rc = get_command_output(inventory_command, alps_output);

  if (rc == PBSE_NONE)
    {
    rc = parse_alps_output(alps_output);

    if (rc == PBSE_NONE)
      {
      if (LOGLEVEL >= 7)
        {
        /* log the command if output successfully parsed */
        snprintf(log_buffer, sizeof(log_buffer),
          "Successful inventory command is: %s", inventory_command);
        log_event(PBSEVENT_JOB | PBSEVENT_SYSLOG, PBS_EVENTCLASS_JOB, __func__, log_buffer);
        }

      if ((apbasil_protocol != NULL) && !strcmp(apbasil_protocol, "1.7"))
        rc = get_knl_information(apbasil_path);
        
      if (rc == PBSE_NONE)
        update_status(status);
      }
    }

  return(rc);
  } /* END generate_alps_status() */

