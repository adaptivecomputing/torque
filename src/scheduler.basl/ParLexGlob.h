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
*      myname           ParLexGlob.h
*      created          7/15/1996
*      updated          8/6/1996
*      works            7/29/1996
*
*      designed   rrs/1996
*************************************************************/
#ifndef _PARLEXGLOB_H
#define _PARLEXGLOB_H
/* Feature test switches */
#ifndef _POSIX_SOURCE
#define _POSIX_SOURCE 1
#endif  /* _POSIX_SOURCE */

#define YES_INT 2 /* funFlag: means func is internally def */
#define YES 1
#define NO 0
#define OTHER 0
#define CONST 1
#define VAR 2

#define LEXEMSZ 80

struct MYTOK
  {
  char lexeme[LEXEMSZ];
  int line;
  int len;
  int type;
  int varFlag; /* VAR, CONST, OTHER */
  };

typedef struct MYTOK STRMYTOK;

/* The following defines must coincide */
/* with SymTabGlob.h entries. */
#define UNKNOWN 0
#define INTTYPE 1
#define FLOATTYPE 2
#define STRINGTYPE 3
#define STATUSTYPE 4
#define DAYOFWEEKTYPE 5
#define SERVERTYPE 6
#define QUETYPE 7
#define JOBTYPE 8
#define SIZETYPE 9
#define INTRANGETYPE 10
#define FLOATRANGETYPE 11
#define DAYOFWEEKRANGETYPE 12
#define SERVERSETTYPE 13
#define QUESETTYPE 14
#define JOBSETTYPE 15
#define SIZESETTYPE 16
#define PARAMTYPE 17
#define FUNTYPE 18
#define SERVERSTATETYPE 19
#define QUESTATETYPE 20
#define JOBSTATETYPE 21
#define SIZESTATETYPE 22
#define DATETIMETYPE 23
#define CNODETYPE 24
#define VOIDTYPE 25
#define DATETIMERANGETYPE 26
#define CNODESETTYPE 27
#define SIZERANGETYPE 28
#define GENERICTYPE 29
#define KEYWORDTYPE 30

#endif /* _PARLEXGLOB_H */
