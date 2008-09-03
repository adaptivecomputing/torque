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
*      myname           Semantic.h
*      created          9/3/1996
*      updated          9/12/1996
*      works            9/12/1996
*
*  designer  rrs/1996
*************************************************************/

/***********************************************************
*
*                OPERATION OVERVIEW
*
*
************************************************************/
#ifndef _SEMANTIC_H
#define _SEMANTIC_H
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

#include "SymTab.h"

/*prototypes*/
extern void
  SemanticInit A_((void));

extern void
  SemanticPutDF A_((int df));

extern void
  SemanticCondPrint A_((char *str));

extern void
  SemanticPrint A_((void));

extern void
  SemanticErr A_((int e));

extern void
  SemanticPrintToken2 A_((struct MYTOK symbol));

extern void
  SemanticPutToken A_((struct MYTOK symbolp, char *lex, int lin, int len, int typ, int varFlag));

extern void
  SemanticStatNopCk A_((void));

extern void
  SemanticStatPlusExprCk A_((struct MYTOK left_expr, struct MYTOK right_expr));

extern void
  SemanticStatMinusExprCk A_((struct MYTOK left_expr, struct MYTOK right_expr));

extern void
  SemanticStatMultDivExprCk A_((struct MYTOK left_expr, struct MYTOK right_expr));

extern void
  SemanticStatModulusExprCk A_((struct MYTOK left_expr, struct MYTOK right_expr));

extern void
  SemanticStatCompExprCk A_((struct MYTOK left_expr, struct MYTOK right_expr));

extern void
  SemanticStatAndOrExprCk A_((struct MYTOK left_expr, struct MYTOK right_expr));

extern void
  SemanticStatNotExprCk A_((struct MYTOK expr));

extern void
  SemanticStatPostOpExprCk A_((struct MYTOK expr));

extern void
  SemanticStatUnaryExprCk A_((struct MYTOK expr));

extern void
  SemanticStatAssignCk A_((struct MYTOK var, struct MYTOK expr));

extern void
  SemanticStatPrintTailCk A_((struct MYTOK exp));

extern void
  SemanticStatWhileHeadCk A_((struct MYTOK exp));

extern void
  SemanticStatIfHeadCk A_((struct MYTOK exp));

extern void
  SemanticStatReturnTailCk A_((struct MYTOK exp));

extern void
  SemanticStatForHeadCk A_((struct MYTOK exp6, struct MYTOK exp8));

extern void
  SemanticStatForAssignCk A_((struct MYTOK exp1, struct MYTOK exp2));

extern void
  SemanticStatForPostAssignCk A_((struct MYTOK exp));

extern void
  SemanticStatForeachHeadCk A_((struct MYTOK val1, struct MYTOK val2));

extern void
  SemanticVarDefCk A_((struct MYTOK var));

extern int
  SemanticParamVarCk A_((struct MYTOK val));

extern int
  SemanticParamConstsCk A_((struct MYTOK val));

extern void
  SemanticCaseTypeCk A_((struct MYTOK val));

extern void
  SemanticCaseInTypeCk A_((struct MYTOK val));

extern void
  SemanticCaseInVarCk A_((struct MYTOK var));

extern void
  SemanticTimeConstCk A_((struct MYTOK h, struct MYTOK m, struct MYTOK s));

extern void
  SemanticDateConstCk A_((struct MYTOK m, struct MYTOK d, struct MYTOK y));

extern void
  SemanticIntConstRangeCk A_((struct MYTOK lo, struct MYTOK hi));

extern void
  SemanticFloatConstRangeCk A_((struct MYTOK lo, struct MYTOK hi));

extern void
  SemanticDayofweekConstRangeCk A_((struct MYTOK lo, struct MYTOK hi));

extern void
  SemanticDateTimeConstRangeCk A_((struct MYTOK lo, struct MYTOK hi));

extern void
  SemanticSizeConstRangeCk A_((struct MYTOK lo, struct MYTOK hi));

#endif /* _SEMANTIC_H */
