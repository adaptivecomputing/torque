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
 * @(#) $Id$
 */

/*
 * list_link.h - header file for general linked list routines
 *  see list_link.c
 *
 * A user defined linked list can be managed by these routines if
 * the first element of the user structure is the list_link struct
 * defined below.
 */

#ifndef LIST_LINK_H
#define LIST_LINK_H 1

#include <sys/types.h>

/* list entry list sub-structure */

typedef struct list_link
  {

  struct list_link *ll_prior;

  struct list_link *ll_next;
  void   *ll_struct;
  } list_link;

typedef list_link tlist_head;

/*
** Simple struct to construct linked PID lists
*/

typedef struct pidl
  {
  pid_t        pid;
  struct pidl *next;
  } pidl;


/* macros to clear list head or link */

#define CLEAR_HEAD(e) e.ll_next = &e, e.ll_prior = &e, e.ll_struct = (void *)0
#define CLEAR_LINK(e) e.ll_next = &e, e.ll_prior = &e

#define LINK_INSET_BEFORE 0
#define LINK_INSET_AFTER  1

#ifdef NDEBUG
#define GET_NEXT(pe)  (pe).ll_next->ll_struct
#define GET_PRIOR(pe) (pe).ll_prior->ll_struct
#else
#define GET_NEXT(pe) get_next(pe, (char *)__FILE__, __LINE__)
#define GET_PRIOR(pe) get_prior(pe,(char *) __FILE__, __LINE__)
#endif

/* function prototypes */

extern void insert_link(list_link *old, list_link *new_link, void *pobj, int pos);
extern void append_link(tlist_head *head, list_link *new_link, void *pnewobj);
extern void delete_link(list_link *old);
extern void swap_link(list_link *, list_link *);
extern int  is_linked(list_link *head, list_link *old);
extern int  is_link_initialized(list_link *);
extern void list_move(tlist_head *old, tlist_head *new_link);
extern void free_pidlist(struct pidl *pl);

#ifndef NDEBUG
extern void *get_next(list_link, char *file, int line);
extern void *get_prior(list_link, char *file, int line);
#endif /* NDEBUG */

#endif /* LIST_LINK_H */
