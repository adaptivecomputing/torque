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
/*************************************************************
*      myname           List.h
*      created          7/12/1996
*      updated          8/13/1996
*      works            8/13/1996
*
*      function         Singly Linked List
*
*
*
*************************************************************/
#ifndef _LIST_H
#define _LIST_H
/* Feature test switches */
#ifndef _POSIX_SOURCE
#define _POSIX_SOURCE 1
#endif  /* _POSIX_SOURCE */

#if __STDC__ == 1
/*
 * The following macro definations take affect when compiling under ansi C
 *
 * The A_ macro is provided for function prototype declarations.  It allows
 * ANSI C prototypes to be complied under K&R C
 */

#define A_(x) x

#else
/* The following macro definations take affect when compiling under K&R C */

#define const
#define volatile
#define A_(x) ()

#endif  /* __STDC__ */

#include "Node.h"

typedef Np List;

/*prototypes*/

extern void
  ListPutDF A_((int df));

extern void
  ListCondPrint A_((char *str));

extern int
  ListIsEmpty A_((List L));

extern void
  ListPrint A_((List L));

extern void
  ListErr A_((int e));

extern List
  ListInsertFront A_((List L, Np nxp));

extern List
  ListInsertSortedN A_((List L, Np nxp));

extern List
  ListInsertSortedD A_((List L, char * lexem, int typ, int LineDe, int leve, int funFlag));

extern int
  ListIsMember A_((List L, Np nxp));

extern Np
  ListGetLast A_((List L));

extern Np
  ListGetSucc A_((List L, Np nxp));

extern List
  ListDelete A_((List L));

extern List
  ListDeleteNode A_((List L, Np nxp));

extern Np
  ListFindNodeByLexeme A_((List L, char * lexem));

extern Np
  ListFindNodeByLexemeInLevel A_((List L, char * lexem, int leve));

extern Np
  ListFindNodeByLexemeInLine A_((List L, char * lexem, int line));

extern Np
  ListMatchNodeByLexemeInLine A_((List L, char * lexem, int line));

extern Np
  ListFindNodeBeforeLexemeInLine A_((List L, char * lexem, int line));

extern Np
  ListMatchNodeBeforeLexemeInLine A_((List L, char * lexem, int line));

extern Np
  ListFindNodeByLexemeAndTypeInLevel A_((List L, char * lexem, int leve, int type, int (*compare_func)()));

extern Np
  ListFindAnyNodeInLevelOfType A_((List L, int leve, int type));

extern List
  ListDeleteLevel A_((List L, int leve));

extern void
  ListParamLink A_((Np tailnp, Np newnp));

extern Np
  ListFindFunProtoByLexemeInProg A_((List L, char * lexem));

#endif /* _LIST_H */
