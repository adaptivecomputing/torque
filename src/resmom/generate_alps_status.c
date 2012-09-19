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
#include "dynamic_string.h"
#include "utils.h"
#include "alps_constants.h"
#include "alps_functions.h"



/*
 * for now, do nothing with reservations
 */
int process_reservations(

  dynamic_string *status,
  xmlNode        *node)

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

  dynamic_string *feature_list,
  xmlNode        *node)

  {
  char     *attr_value;
  xmlNode  *child;
  int       first = 0;

  for (child = node->children; child != NULL; child = child->next)
    {
    if ((attr_value = (char *)xmlGetProp(child, (const xmlChar *)"name")) != NULL)
      {
      if (!strncmp(attr_value, "MOAB:FEATURE=", strlen("MOAB:FEATURE=")))
        {
        char *feature_name = attr_value + strlen("MOAB:FEATURE=");
        if (first == 0)
          {
          append_dynamic_string(feature_list, feature_name);
          first++;
          }
        else
          {
          append_dynamic_string(feature_list, ",");
          append_dynamic_string(feature_list, feature_name);
          }
        }

      free(attr_value);
      }
    }

  return(PBSE_NONE);
  } /* END process_label_array() */




int process_accelerator_array(

  dynamic_string *status,
  xmlNode        *node)

  {
  xmlNode       *child;
  char          *attr_value;
  char          *attr_value2;
  char           buf[MAXLINE];

  copy_to_end_of_dynamic_string(status, CRAY_GPU_STATUS_START);

  for (child = node->children; child != NULL; child = child->next)
    {
    if (!strcmp((const char *)child->name, accelerator))
      {
      /* write the gpu id */
      attr_value = (char *)xmlGetProp(child, (const xmlChar *)type);
      attr_value2 = (char *)xmlGetProp(child, (const xmlChar *)ordinal);
      snprintf(buf, sizeof(buf), "%s-%d", attr_value, atoi(attr_value2));
      copy_to_end_of_dynamic_string(status, "gpu_id=");
      append_dynamic_string(status, buf);
      free(attr_value);
      free(attr_value2);

      attr_value = (char *)xmlGetProp(child, (const xmlChar *)state);
      copy_to_end_of_dynamic_string(status, "state=");
      append_dynamic_string(status, attr_value);
      free(attr_value);

      attr_value = (char *)xmlGetProp(child, (const xmlChar *)family);
      copy_to_end_of_dynamic_string(status, "family=");
      append_dynamic_string(status, attr_value);
      free(attr_value);

      attr_value = (char *)xmlGetProp(child, (const xmlChar *)memory_mb);
      copy_to_end_of_dynamic_string(status, "memory=");
      append_dynamic_string(status, attr_value);
      append_dynamic_string(status, "mb");
      free(attr_value);

      attr_value = (char *)xmlGetProp(child, (const xmlChar *)clock_mhz);
      copy_to_end_of_dynamic_string(status, "clock_mhz=");
      append_dynamic_string(status, attr_value);
      free(attr_value);
      }
    }
  
  copy_to_end_of_dynamic_string(status, CRAY_GPU_STATUS_END);

  return(PBSE_NONE);
  } /* END process_accelerator_array() */




int process_node(

  dynamic_string *status,
  xmlNode        *node)

  {
  char               *attr_value;
  char               *role_value;
  xmlNode            *child;
  xmlNode            *segments;
  xmlNode            *segment_child;
  dynamic_string     *features;
  char                buf[MAXLINE];
  int                 num_procs     = 0;
  int                 avail_procs   = 0;
  unsigned long       memory        = 0;
  unsigned long long  mem_kb;
  char               *rsv_id        = NULL;

  if ((features = get_dynamic_string(-1, NULL)) == NULL)
    return(ENOMEM);

  copy_to_end_of_dynamic_string(status, "node=");
  attr_value = (char *)xmlGetProp(node, (const xmlChar *)node_id);
  append_dynamic_string(status, attr_value);
  free(attr_value);

  /* check to see if the role is interactive - report these as down */
  role_value = (char *)xmlGetProp(node, (const xmlChar *)role);

  copy_to_end_of_dynamic_string(status, "ARCH=");
  attr_value = (char *)xmlGetProp(node, (const xmlChar *)architecture);
  append_dynamic_string(status, attr_value);
  free(attr_value);

  copy_to_end_of_dynamic_string(status, "name=");
  attr_value = (char *)xmlGetProp(node, (const xmlChar *)name);
  append_dynamic_string(status, attr_value);
  free(attr_value);

  /* process the children */
  for (child = node->children; child != NULL; child = child->next)
    {
    if (!strcmp((const char *)child->name, segment_array))
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
    else if (!strcmp((const char *)child->name, accelerator_array))
      {
      process_accelerator_array(status, child);
      }
    } /* END the loop for processing the children */

  /* once done, add the procs, available procs, memory info, reservation, and features */
  snprintf(buf, sizeof(buf), "CPROC=%d", num_procs);
  copy_to_end_of_dynamic_string(status, buf);

  snprintf(buf, sizeof(buf), "APROC=%d", avail_procs);
  copy_to_end_of_dynamic_string(status, buf);

  snprintf(buf, sizeof(buf), "CMEMORY=%lu", memory);
  copy_to_end_of_dynamic_string(status, buf);

  mem_kb = memory * 1024;

  snprintf(buf, sizeof(buf), "totmem=%llukb", mem_kb);
  copy_to_end_of_dynamic_string(status, buf);

  if (rsv_id != NULL)
    {
    /* don't write the reservation id if we're in interactive mode */
    if ((role_value == NULL) ||
        (strcmp(role_value, interactive_caps)))
      {
      copy_to_end_of_dynamic_string(status, "reservation_id=");
      append_dynamic_string(status, rsv_id);
      }

    free(rsv_id);

    /* if there's a reservation on this node, the state is busy */
    copy_to_end_of_dynamic_string(status, "state=BUSY");
    }
  else
    {
    /* no reservation, evaluate the state normally */
    copy_to_end_of_dynamic_string(status, "state=");
    attr_value = (char *)xmlGetProp(node, (const xmlChar *)state);
    
    if ((role_value != NULL) &&
        (!strcmp(role_value, interactive_caps)))
      append_dynamic_string(status, "DOWN");
    else
      append_dynamic_string(status, attr_value);

    free(attr_value);
    }

  if (role_value != NULL)
    free(role_value);

  if (features->used > 0)
    {
    copy_to_end_of_dynamic_string(status, "feature_list=");
    append_dynamic_string(status, features->str);
    }

  free_dynamic_string(features);

  return(PBSE_NONE);
  } /* END process_node() */





int process_nodes(

  dynamic_string *status,
  xmlNode        *node)

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

  dynamic_string *status,
  xmlNode        *node)

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

      find_error_type(node);

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

  dynamic_string *alps_output,
  dynamic_string *status)

  {
  xmlDocPtr  doc;

  if ((doc = xmlReadMemory(alps_output->str, strlen(alps_output->str), "apbasil", NULL, 0)) == NULL)
    {
    char buf[MAXLINE * 4];
    xmlErrorPtr pErr = xmlGetLastError();
    snprintf(buf, sizeof(buf), "Failed to parse the output of alps - %s", pErr->message);
    log_err(-1, __func__, buf);
    return(ALPS_PARSING_ERROR);
    }

  process_element(status, xmlDocGetRootElement(doc));

  xmlFreeDoc(doc);
  xmlMemoryDump();

  return(PBSE_NONE);
  } /* END parse_alps_output() */




int generate_alps_status(

  dynamic_string *status,
  const char     *apbasil_path,
  const char     *apbasil_protocol)

  {
  FILE           *alps_pipe;
  int             fd;
  int             rc;
  int             bytes_read;
  int             total_bytes_read = 0;
  char            inventory_command[MAXLINE * 2];
  char            input_buffer[MAXLINE];
  char           *ptr;
  dynamic_string *alps_output;

  if ((alps_output = get_dynamic_string(-1, NULL)) == NULL)
    return(ENOMEM);

  snprintf(inventory_command, sizeof(inventory_command), APBASIL_QUERY,
    (apbasil_protocol != NULL) ? apbasil_protocol : DEFAULT_APBASIL_PROTOCOL,
    (apbasil_path != NULL) ? apbasil_path : DEFAULT_APBASIL_PATH);

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
    append_dynamic_string(alps_output, ptr);
    memset(input_buffer, 0, sizeof(input_buffer));
    total_bytes_read += bytes_read;
    }

  /* perform post-processing */
  pclose(alps_pipe);

  if ((bytes_read == -1) ||
      (total_bytes_read == 0))
    rc = READING_PIPE_ERROR;
  else
    {
    int index = alps_output->used - 1;
    while (alps_output->str[index] != '>')
      {
      alps_output->str[index] = '\0';
      index--;
      }

    alps_output->used -= 1;
    rc = parse_alps_output(alps_output, status);
    }

  free_dynamic_string(alps_output);

  if (rc != PBSE_NONE)
    {
    log_err(-1, __func__, "Couldn't correctly get the alps inventory! System is down");
    }

  return(rc);
  } /* END generate_alps_status() */

