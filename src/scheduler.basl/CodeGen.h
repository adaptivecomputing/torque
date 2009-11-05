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
*      myname           CodeGen.h
*      created          12/18/1996
*      updated          12/18/1996
*      works            xx/1996
*
*  designer  rrs/1996
*************************************************************/

/***********************************************************
*
*                OPERATION OVERVIEW
*
*
************************************************************/
#ifndef _CODEGEN_H
#define _CODEGEN_H
/* Feature test switches */
#ifndef _POSIX_SOURCE
#define _POSIX_SOURCE 1
#endif  /* _POSIX_SOURCE */

#include "List.h"

struct Stack
  {
  Np     np;

  struct Stack *rptr;
  };

typedef struct Stack *St;

/*prototypes*/

extern void
  CodeGenInit(void);

extern void
  CodeGenPutDF(int df);

extern void
  CodeGenCondPrint(char *str);

extern void
  CodeGenPrint(void);

extern void
  CodeGenErr(int e);

extern void
  CodeGenBuffClear(void);

extern void
  CodeGenBuffPrint(void);

extern void
  CodeGenBuffEmit(void);

extern void
  CodeGenBuffSwitchEmit(void);

extern void
  CodeGenBuffSave(char *str);

extern void
  CodeGenBuffSaveBefore(char *str, char *lexeme, int level);

extern void
  CodeGenBuffSaveAfter(char *str, char *lexeme, int level);

extern void
  CodeGenBuffSaveFirst(char *str);

extern void
  CodeGenBuffSaveFun(char *str);

extern void
  CodeGenBuffSaveFunBefore(char *str, char *lexeme, int level);

extern void
  CodeGenBuffSaveFunAfter(char *str, char *lexeme, int level);

extern void
  CodeGenBuffSaveFunFirst(char *str);

extern void
  CodeGenStatPrint(void);

extern void
  CodeGenStatPrintTail(struct MYTOK data);

extern void
  CodeGenDefSimple(struct MYTOK data);

extern void
  CodeGenProgHead(void);

extern int
  CodeGenLastDef(char *lexeme);

extern void
  CodeGenStackClear(void);

extern St
  CodeGenStackNew(Np np);

extern void
  CodeGenStackPush(Np np);

extern Np
  CodeGenStackPop(void);

extern void
  CodeGenStackPrint(void);

extern void
  CodeGenBuffSaveSpecOper(char *operstr);

extern Np
  CodeGenBuffGetLast(void);

extern void
  CodeGenBuffDelete(char *lexeme, int inst);

extern void
  CodeGenBuffSaveStrAssign(void);

extern void
  CodeGenBuffSaveForeach(struct MYTOK var, struct MYTOK svar);

extern void
  CodeGenBuffSaveSwitch(struct MYTOK switchVar);

extern void
  CodeGenBuffSaveSwitchIn(struct MYTOK switchVar, struct MYTOK caseVal);

extern void
  CodeGenBuffSaveQueJobFind(void);

extern void
  CodeGenBuffSaveQueFilter(void);

extern Np
  CodeGenBuffGetNp(char *lexeme, int lineDef);
#endif /* _CODEGEN_H */
