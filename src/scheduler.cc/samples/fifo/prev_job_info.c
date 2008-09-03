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
#include "prev_job_info.h"
#include "job_info.h"


/*
 *
 * create_prev_job_info - create the prev_job_info array from an array
 *    of jobs
 *
 *   jinfo_arr - job_info array
 *   size - size of jinfo_arr or UNSPECIFIED if unknown
 *
 * returns new prev_job_array
 *
 * NOTE: jinfo_arr is modified
 *
 */
prev_job_info *create_prev_job_info(job_info **jinfo_arr, int size)
  {
  prev_job_info *new;  /* new prev_job_info array */
  int local_size;  /* the size of the array */
  int i;

  if (size == UNSPECIFIED)
    {
    for (i = 0; jinfo_arr[i] != NULL; i++)
      ;

    local_size = i;
    }
  else
    local_size = size;

  if ((new = (prev_job_info *) malloc(sizeof(prev_job_info)  * local_size)) == NULL)
    {
    fprintf(stderr, "Error allocating memory\n");
    return NULL;
    }

  for (i = 0; jinfo_arr[i] != NULL; i++)
    {
    new[i].name = jinfo_arr[i] -> name;
    new[i].resused = jinfo_arr[i] -> resused;
    new[i].account = jinfo_arr[i] -> account;
    new[i].ginfo = jinfo_arr[i] -> ginfo;

    /* so the memory is not freed at the end of the scheduling cycle */
    jinfo_arr[i] -> name = NULL;
    jinfo_arr[i] -> resused = NULL;
    jinfo_arr[i] -> account = NULL;
    }

  return new;
  }

/*
 *
 * free_prev_job_info - free a prev_job_info struct
 *
 *   pjinfo - prev_job_info to free
 *
 * returns nothing
 *
 */
void free_prev_job_info(prev_job_info *pjinfo)
  {
  if (pjinfo -> name != NULL)
    free(pjinfo -> name);

  if (pjinfo -> account != NULL)
    free(pjinfo -> account);

  free_resource_req_list(pjinfo -> resused);
  }

/*
 *
 * free_pjobs - free a list of prev_job_info structs
 *
 *   pjinfo_arr - array of prev_job_info structs to free
 *
 * returns nothing
 *
 */
void free_pjobs(prev_job_info *pjinfo_arr, int size)
  {
  int i;

  if (pjinfo_arr == NULL)
    return;

  for (i = 0; i < size; i++)
    free_prev_job_info(&pjinfo_arr[i]);

  free(pjinfo_arr);
  }
