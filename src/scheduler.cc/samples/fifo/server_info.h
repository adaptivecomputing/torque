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

#ifndef SERVER_INFO_H
#define SERVER_INFO_H

#include "pbs_ifl.h"
#include "state_count.h"
#include "data_types.h"

/*
 *      query_server - creates a structure of arrays consisting of a server
 *                      and all the queues and jobs that reside in that server
 */
server_info *query_server(int pbs_sd);

/*
 * query_server_info - collect information out of a statserver call
 *       into a server_info structure
 */
server_info *query_server_info(struct batch_status *server);

/*
 * find_alloc_resource - try and find a resource, and if it is not there
 *         alocate space for it and add it to the resource
 *         list
 */

resource *find_alloc_resource(resource *resplist, char *name);

/*
 * find_resource - finds a resource in a resource list
 */

resource *find_resource(resource *reslist, const char *name);

/*
 * free_server_info - free the space used by a server_info structure
 */
void free_server_info(server_info *sinfo);

/*
 *      free_resource - free a resource struct
 */
void free_resource(resource *res);

/*
 *      free_resource_list - free a resource list
 */
void free_resource_list(resource *res_list);

/*
 *      new_server_info - allocate and initalize a new server_info struct
 */
server_info *new_server_info();

/*
 *      new_resource - allocate and initialize new resoruce struct
 */
resource *new_resource();

/*
 *      print_server_info - print server_info structure
 */
void print_server_info(server_info *sinfo, char brief);


/*
 * free_server - free a list of server_info structs
 */
void free_server(server_info *sinfo, int free_queues_too);

/*
 *      update_server_on_run - update server_info strucutre when a job is run
 */
void update_server_on_run(server_info *sinfo, queue_info *qinfo, job_info *jinfo);

/*
 *      set_jobs - set the server_info jobs array from the queue job arrays
 */
void set_jobs(server_info *sinfo);

/*
 *      check_run_job - function used by job_filter to filter out
 *                      non-running jobs.
 */
int check_run_job(job_info *job, void *arg);

/*
 *      free_token_list - free a list of tokens
 */
void free_token_list(token **token_list);

/*
 *      count_tokens - count the number of comma separated tokens
 */
int count_tokens(const char* tokenlist);

/*
 *      get_token - get token number out of a comma separated list
 */
token** get_token_array(char* tokenlist);

token* get_token(char* tokenstring);

void free_token(token* token_ptr);




#endif
