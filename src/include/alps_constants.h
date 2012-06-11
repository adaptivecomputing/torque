/*
*         OpenPBS (Portable Batch System) v2.3 Software License
*
* Copyright (c) 1999-2010 Veridian Information Solutions, Inc.
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


#define APBASIL_RETRIES             5
#define WRITING_PIPE_ERROR          -1
#define READING_PIPE_ERROR          -2
#define ALPS_PARSING_ERROR          -3
#define ALPS_QUERY_FAILURE          -4
#define DEFAULT_APBASIL_PATH        "/usr/bin/apbasil"
#define DEFAULT_APBASIL_PROTOCOL    "1.0"
#define APBASIL_QUERY               "echo \"<?xml version='1.0'?><BasilRequest protocol='%s' method='QUERY' type='INVENTORY'></BasilRequest>\" | %s"
#define APBASIL_RESERVE_PARAM_BEGIN "<ReserveParam architecture='XT' width='%d' nppn='%d'><NodeParamArray><NodeParam>"
#define APBASIL_RESERVE_PARAM_END   "</NodeParam></NodeParamArray></ReserveParam>"
#define APBASIL_RESERVE_ARRAY       "<ReserveParamArray user_name='%s' batch_id='%s'>"
#define CLOSE_RESERVE_ARRAY         "</ReserveParamArray>"
#define APBASIL_RESERVE_REQ         "echo \"<?xml version='1.0'?><BasilRequest protocol='%s' method='RESERVE'>"
#define CLOSE_BASIL_REQ             "</BasilRequest>"
#define CONFIRM_BASIL_REQ           "echo \"<?xml version='1.0'?><BasilRequest protocol='%s' method='CONFIRM' reservation_id='%s' %s='%lld'/>\" | %s"
#define DELETE_BASIL_REQ            "echo \"<?xml version='1.0'?><BasilRequest protocol='%s' method='RELEASE' reservation_id='%s'/>\" | %s"


extern const int   apbasil_fail_transient;
extern const int   apbasil_fail_permanent;
extern const char *message;
extern const char *response_data;
extern const char *node_array;
extern const char *node_name;
extern const char *reservation_array;
extern const char *name;
extern const char *node_id;
extern const char *architecture;
extern const char *state;
extern const char *success;
extern const char *status;
extern const char *role;
extern const char *interactive_caps;
extern const char *clock_mhz;
extern const char *reservation_id;
extern const char *processor_array;
extern const char *processor;
extern const char *ProcessorAllocation;
extern const char *memory_array;
extern const char *memory_name;
extern const char *label_array;
extern const char *segment_array;
extern const char *segment;
extern const char *accelerator_array;
extern const char *accelerator;
extern const char *ordinal;
extern const char *type;
extern const char *text_name;
extern const char *memory_mb;
extern const char *family;
extern const char *cproc_eq;
extern const int   cproc_eq_len;
extern const char *CRAY_GPU_STATUS_START;
extern const char *CRAY_GPU_STATUS_END;
extern const char *alps_reporter_feature;
extern const char *alps_starter_feature;
extern const char *reserved;
extern const char *pagg_id;
extern const char *admin_cookie;
extern const char *error_class;

typedef struct host_req
  {
  unsigned int     ppn;
  char            *hostname;
  } host_req;


host_req *get_host_req(char *hostname);
void      free_host_req(host_req *hr);
