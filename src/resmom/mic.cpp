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


#include <source/COIPipeline_source.h>
#include <source/COIEngine_source.h>
#include <string.h>
#include <stdio.h>

#include "pbs_error.h"
#include "log.h"
#include "pbs_nodes.h"
#include <vector>
#include <string>

#define  MAX_ENGINES 32

#ifdef NUMA_SUPPORT
extern int       numa_index;
extern nodeboard node_boards[];
#endif


int add_isa(

  std::vector<std::string> &status,
  struct COI_ENGINE_INFO   *mic_stat)

  {
  char     status_buf[MAXLINE];
  int      rc = PBSE_NONE;
  
  switch (mic_stat->ISA)
    {
    case COI_ISA_x86_64:

      snprintf(status_buf, sizeof(status_buf), "isa=COI_ISA_x86_64");
      break;

    case COI_ISA_MIC:

      snprintf(status_buf, sizeof(status_buf), "isa=COI_ISA_MIC");
      break;

    case COI_ISA_KNF:

      snprintf(status_buf, sizeof(status_buf), "isa=COI_ISA_KNF");
      break;

    case COI_ISA_KNC:

      snprintf(status_buf, sizeof(status_buf), "isa=COI_ISA_KNC");
      break;

    default:

      rc = PBSE_SYSTEM;

    }

  if (rc == PBSE_NONE)
    status.push_back(status_buf);

  return(rc);
  } /* END add_isa() */



int calculate_and_add_load(

  std::vector<std::string> &status,
  struct COI_ENGINE_INFO   *mic_stat)

  {
  double   load_normalized = 0.0;
  uint32_t i;
  double   load = 0;
  char     status_buf[MAXLINE * 2];
  
  for (i = 0; i < mic_stat->NumCores; i++)
    {
    load += mic_stat->Load[i];
    }

  load = load / 100.0;
  load_normalized = load / mic_stat->NumCores;

  snprintf(status_buf, sizeof(status_buf), "load=%f", (float)load);
  status.push_back(status_buf);

  snprintf(status_buf, sizeof(status_buf), "normalized_load=%f", (float)load_normalized);
  status.push_back(status_buf);

  return(PBSE_NONE);
  } /* END calculate_and_add_load() */




int add_single_mic_info(

  std::vector<std::string> &status,
  struct COI_ENGINE_INFO   *mic_stat)

  {
  char     status_buf[MAXLINE * 2];

  snprintf(status_buf, sizeof(status_buf), "mic_id=%u", mic_stat->DeviceId);
  status.push_back(status_buf);

  snprintf(status_buf, sizeof(status_buf), "num_cores=%u", mic_stat->NumCores);
  status.push_back(status_buf);

  snprintf(status_buf, sizeof(status_buf), "num_threads=%u", mic_stat->NumThreads);
  status.push_back(status_buf);

  snprintf(status_buf, sizeof(status_buf), "physmem=%lu", mic_stat->PhysicalMemory);
  status.push_back(status_buf);

  snprintf(status_buf, sizeof(status_buf), "free_physmem=%lu", mic_stat->PhysicalMemoryFree);
  status.push_back(status_buf);

  snprintf(status_buf, sizeof(status_buf), "swap=%lu", mic_stat->SwapMemory);
  status.push_back(status_buf);

  snprintf(status_buf, sizeof(status_buf), "free_swap=%lu", mic_stat->SwapMemoryFree);
  status.push_back(status_buf);

  snprintf(status_buf, sizeof(status_buf), "max_frequency=%u", mic_stat->CoreMaxFrequency);
  status.push_back(status_buf);

  add_isa(status, mic_stat);

  calculate_and_add_load(status, mic_stat);

  return(PBSE_NONE);
  } /* END add_single_mic_info() */





int add_mic_status(

  std::vector<std::string> &status)

  {
  COIENGINE                engine[MAX_ENGINES];
  uint32_t                 num_engines = 0;
  uint32_t                 i;

  struct COI_ENGINE_INFO   mic_stat[MAX_ENGINES];

#ifdef NUMA_SUPPORT
  /* does this node board have mics configured? */
  if (node_boards[numa_index].mic_end_index < 0)
    return(PBSE_NONE);
#endif

  if (COIEngineGetCount(COI_ISA_MIC, &num_engines) != COI_SUCCESS)
    {
    log_err(-1, __func__, "Mics are present but apparently not configured correctly - can't get count");
    return(PBSE_SYSTEM);
    }

  status.push_back(START_MIC_STATUS);

#ifdef NUMA_SUPPORT
  if (num_engines < node_boards[numa_index].mic_end_index)
    {
    snprintf(log_buffer, sizeof(log_buffer),
      "node board %d is supposed to have mic range %d-%d but there are only %d mics",
      numa_index, node_boards[numa_index].mic_start_index,
      node_boards[numa_index].mic_end_index, num_engines);
    log_err(-1, __func__, log_buffer);
    return(PBSE_SYSTEM);
    }

  for (i = node_boards[numa_index].mic_start_index; i <= node_boards[numa_index].mic_end_index; i++)
#else
  for (i = 0; i < num_engines; i++)
#endif
    {
    if (COIEngineGetHandle(COI_ISA_MIC, i, &engine[i]) != COI_SUCCESS)
      {
      snprintf(log_buffer, sizeof(log_buffer), "Can't get handle for mic index %d", (int)i);
      log_err(-1, __func__, log_buffer);

      continue;
      }

    if (COIEngineGetInfo(engine[i], sizeof(struct COI_ENGINE_INFO), &mic_stat[i]) != COI_SUCCESS)
      {
      snprintf(log_buffer, sizeof(log_buffer), "Can't get information for mic index %d", (int)i);
      log_err(-1, __func__, log_buffer);

      continue;
      }

    add_single_mic_info(status, &mic_stat[i]);
    }

  status.push_back(END_MIC_STATUS);

  return(PBSE_NONE);
  } /* END add_mic_status() */




