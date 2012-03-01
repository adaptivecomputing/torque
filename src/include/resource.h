#ifndef RESOURCE_H
#define RESOURCE_H

#include "attribute.h" /* pbs_attribute */

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

/*
 * This header file contains the definitions for resources.
 *
 * Other required header files:
 * "portability.h"
 * "attribute.h"
 * "list_link.h"
 *
 * Resources are "a special case" of attributes.  Resources use similiar
 * structures as attributes.  Certain types, type related functions,
 * and flags may differ between the two.
 *
 * Within the resource structure, the value is contained in an pbs_attribute
 * substructure, this is done so the various pbs_attribute decode and encode
 * routines can be "reused".
 *
 * For any server, queue or job pbs_attribute which is a set of resources,
 * the pbs_attribute points to an list  of "resource" structures.
 * The value of the resource is contained in these structures.
 *
 * Unlike "attributes" which are typically identical between servers
 * within an administrative domain,  resources vary between systems.
 * Hence, the resource instance has a pointer to the resource definition
 * rather than depending on a predefined index.
 */

typedef struct resource
  {
  list_link            rs_link; /* link to other resources in list */

  struct resource_def *rs_defin; /* pointer to definition entry */
  pbs_attribute        rs_value; /* pbs_attribute struct holding value */
  } resource;

typedef struct resource_def
  {
  char   *rs_name;
  int     (*rs_decode)(pbs_attribute *, char *, char *, char *, int);
  int     (*rs_encode)(pbs_attribute *, tlist_head *, char *, char *, int, int);
  int     (*rs_set)(pbs_attribute *, pbs_attribute *, enum batch_op);
  int     (*rs_comp)(pbs_attribute *, pbs_attribute *);
  void    (*rs_free)(pbs_attribute *);
  int     (*rs_action)(resource *, pbs_attribute *, int);

unsigned int rs_flags:
  ATRFLAG; /* flags: R/O, ..., see attribute.h */

unsigned int rs_type:
  ATRTYPE; /* type of resource,see attribute.h */
  } resource_def;

/* the resource definition array, only the fixed resources */
extern resource_def svr_resc_def_const[];

/* svr_resc_def_const + resources in "extra_resc" server attr */
extern resource_def *svr_resc_def;

/* size (num elements) in above  */
extern int         svr_resc_size;

extern resource     *add_resource_entry(pbs_attribute *, resource_def *);
extern resource_def *find_resc_def(resource_def *, char *, int);
extern resource     *find_resc_entry(pbs_attribute *, resource_def *);

/* END resource.h */
#endif

