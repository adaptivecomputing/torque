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

#ifndef lONG_H
#define lONG_H

#include <limits.h>

#ifndef A_
#if __STDC__ == 1
#define A_(x) x
#else	/* STDC */
#define const
#define volatile
#define A_(x) ()
#endif	/* STDC */
#endif	/* A_   */

/*
 * Define Long and u_Long to be the largest integer types supported by the
 * native compiler.  They need not be supported by printf or scanf or their
 * ilk.  Ltostr, uLtostr, strtoL, strtouL, and atoL provide conversion to and
 * from character string form.
 *
 * The following sections are listed in decreasing order of brain damage.
 */ 

/****************************************************************************/
#if defined(__hpux)						   /* HP-UX */

/* Under HP-UX, the compiler supports 64-bit signed integers as long longs */
/* but it is only capable of supporting 63-bit unsigned integers as */
/* unsigned long longs.  As a consequence, the most negative Long is one */
/* closer to zero than it would otherwise be.  HP-UX also seems to provide */
/* neither defined constant support nor library support. */

typedef long long		Long;
typedef unsigned long long	u_Long;

#define lONG_MIN		-0x7FFFFFFFFFFFFFFFLL
#define lONG_MAX		 0x7FFFFFFFFFFFFFFFLL
#define UlONG_MAX		 0x7FFFFFFFFFFFFFFFULL

Long strToL A_((const char *nptr, char **endptr, int base));
u_Long strTouL A_((const char *nptr, char **endptr, int base));
#define atoL(nptr)		strToL((nptr), (char **)NULL, 10)

/****************************************************************************/
#elif defined(__GNUC__) ||		    /* SunOS, FreeBSD, NetBSD, BSDI */\
      defined(_AIX) && defined(__EXTENDED__)			     /* AIX */

/* On these systems, the compiler supports 64-bit integers as long longs but */
/* there seems to be neither defined constant support nor library support. */

typedef long long		Long;
typedef unsigned long long	u_Long;

#define lONG_MIN		(-0x7FFFFFFFFFFFFFFFLL-1)
#define lONG_MAX		  0x7FFFFFFFFFFFFFFFLL
#define UlONG_MAX		  0xFFFFFFFFFFFFFFFFULL

Long strToL A_((const char *nptr, char **endptr, int base));
u_Long strTouL A_((const char *nptr, char **endptr, int base));
#define atoL(nptr)		strToL((nptr), (char **)NULL, 10)

/****************************************************************************/
#elif defined(__sgi) && defined(_LONGLONG) && _MIPS_SZLONG == 32    /* Irix */\

/* On Irix, the compiler supports 64-bit integers as long longs with defined */
/* constant support and with some library support.  The library has nothing */
/* like LTostr or uLTostr. */

typedef long long		Long;
typedef unsigned long long	u_Long;

#define lONG_MIN		LONGLONG_MIN
#define lONG_MAX		LONGLONG_MAX
#define UlONG_MAX		ULONGLONG_MAX

#define strToL(n, e, b)		strtoll(n, e, (b))
#define strTouL(n, e, b)	strtoull(n, e, (b))
#define atoL(nptr)		atoll((nptr))

/****************************************************************************/
#elif defined(sun) && defined(sparc) && defined(LLONG_MAX)	 /* Solaris */

/* Under Solaris, the compiler supports 64-bit integers as long longs with */
/* defined constant support and library support, but with some strange */
/* aspects to its library support.  This package ignores the Solaris */
/* definitions of lltostr and ulltostr.  They aren't anything close to being */
/* the functionally symetric equivalents of strtoll and strtoull.  The */
/* LTostr and uLTostr functions in this package are much closer. */

typedef long long		Long;
typedef unsigned long long	u_Long;

#define lONG_MIN		LLONG_MIN
#define lONG_MAX		LLONG_MAX
#define UlONG_MAX		ULLONG_MAX

#define strToL(n, e, b)		strtoll(n, e, (b))
#define strTouL(n, e, b)	strtoull(n, e, (b))
#define atoL(nptr)		atoll((nptr))

/****************************************************************************/
#else							/* Unicos, SunOS cc */

/* On this machine, longs are as long as it gets.  With luck, at least 64 */
/* bits (SunOS cc users have no luck).  Naturally, defined constant support */
/* and some library support are both present.  The library has nothing like */
/* LTostr or uLTostr. */

/* What an amazing concept, long as the longest supported integer data type! */
/* Congratulations Cray, for being the only ones so far to get it right! */

typedef long			Long;
typedef unsigned long		u_Long;

#define lONG_MIN		LONG_MIN
#define lONG_MAX		LONG_MAX
#define UlONG_MAX		(u_Long)ULONG_MAX	/* Cast for SunOS cc */

#define strToL(n, e, b)		strtol(n, e, (b))
#define strTouL(n, e, b)	strtoul(n, e, (b))
#define atoL(nptr)		atol((nptr))

/****************************************************************************/
#endif

const char *LTostr A_((Long value, int base));
const char *uLTostr A_((u_Long value, int base));

#endif /* lONG_H */
