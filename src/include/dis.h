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


#ifndef DATA_IS_STRINGS_
#define DATA_IS_STRINGS_

#include <string.h>
#include <limits.h>
#include <float.h>
#include <pthread.h>
#include "tcp.h" /* tcp_chan */

#ifndef TRUE
#define TRUE 1
#define FALSE 0
#endif

/*
 * Integer function return values from Data-is-Strings reading calls
 */

/* sync w/dis_emsg[] (lib/Libdis/dis.c) */

#define DIS_SUCCESS 0 /* No error */
#define DIS_OVERFLOW 1 /* Value too large to convert */
#define DIS_HUGEVAL 2 /* Tried to write floating point infinity */
#define DIS_BADSIGN 3 /* Negative sign on an unsigned datum */
#define DIS_LEADZRO 4 /* Input count or value has leading zero */
#define DIS_NONDIGIT 5 /* Non-digit found where a digit was expected */
#define DIS_NULLSTR 6 /* String read has an embedded ASCII NUL */
#define DIS_EOD  7 /* Premature end of message */
#define DIS_NOMALLOC 8 /* Unable to malloc space for string */
#define DIS_PROTO       9 /* Supporting protocol failure */
#define DIS_NOCOMMIT 10 /* Protocol failure in commit */
#define DIS_EOF  11 /* End of File */
#define DIS_INVALID 12 /* Invalid condition in code*/


unsigned long disrul (struct tcp_chan *chan, int *retval);

/*#if UINT_MAX == ULONG_MAX*/
#if SIZEOF_UNSIGNED == SIZEOF_LONG
#define disrui(chan, retval) (unsigned)disrul(chan, (retval))
#else
unsigned disrui (struct tcp_chan *chan,  int *retval);
#endif

unsigned disrui_peek (struct tcp_chan *chan, int *retval);


/*#if USHRT_MAX == UINT_MAX*/
#if SIZEOF_UNSIGNED_SHORT == SIZEOF_UNSIGNED_INT
#define disrus(chan, retval) (unsigned short)disrui(chan, (retval))
#else
unsigned short disrus (struct tcp_chan *chan, int *retval);
#endif

/*#if UCHAR_MAX == USHRT_MAX*/
#if SIZEOF_UNSIGNED_CHAR == SIZEOF_UNSIGNED_SHORT
#define disruc(chan, retval) (unsigned char)disrus(chan, (retval))
#else
unsigned char disruc (struct tcp_chan *chan, int *retval);
#endif

long disrsl (struct tcp_chan *chan, int *retval);
/*#if INT_MIN == LONG_MIN && INT_MAX == LONG_MAX*/
#if SIZEOF_INT == SIZEOF_LONG
#define disrsi(chan, retval) (int)disrsl(chan, (retval))
#else
int disrsi (struct tcp_chan *chan, int *retval);
#endif

/* MUTSU -- disrss does not appear to be in use anywhere in the code base!! */
/*#if SHRT_MIN == INT_MIN && SHRT_MAX == INT_MAX*/
#if SIZEOF_SHORT == SIZEOF_INT
#define disrss(chan, retval) (short)disrsi(chan, (retval))
#else
short disrss (struct tcp_chan *chan, int *retval);
#endif

/* MUTSU -- disrsc does not appear to be in use anywhere in the code base!! */
/*#if CHAR_MIN == SHRT_MIN && CHAR_MAX == SHRT_MAX*/
#if SIZEOF_SIGNED_CHAR == SIZEOF_SHORT
#define disrsc(chan, retval) (signed char)disrss(chan, (retval))
#else
signed char disrsc (struct tcp_chan *chan, int *retval);
#endif

/* MUTSU -- disrc does not appear to be in use anywhere in the code base!! */
/*#if CHAR_MIN, i.e. if chars are signed*/
/* also, flip the order of statements */
#ifdef __CHAR_UNSIGNED__
#define disrc(retval, chan) (char)disruc(chan, (retval))
#else
#define disrc(chan, retval) (char)disrsc(chan, (retval))
#endif

char *disrcs(struct tcp_chan *chan, size_t *nchars, int *retval);
int disrfcs(struct tcp_chan *chan, size_t *nchars, size_t achars, char *value);
char *disrst(struct tcp_chan *chan, int *retval);
int disrfst(struct tcp_chan *chan, size_t achars, char *value);

/*
 * some compilers do not like long doubles, if long double is the same
 * as a double, just use a double.
 */
#if SIZEOF_DOUBLE == SIZEOF_LONG_DOUBLE
typedef double dis_long_double_t;
#else
typedef long double dis_long_double_t;
#endif

/* MUTSU -- disrl does not appear to be in use anywhere in the code base!! */
dis_long_double_t disrl (struct tcp_chan *chan, int *retval);
/*#if DBL_MANT_DIG == LDBL_MANT_DIG && DBL_MAX_EXP == LDBL_MAX_EXP*/

/* MUTSU -- disrd does not appear to be in use anywhere in the code base!! */
#if SIZEOF_DOUBLE == SIZEOF_LONG_DOUBLE
#define disrd(chan, retval) (double)disrl(chan, (retval))
#else
double disrd (struct tcp_chan *chan, int *retval);
#endif

/* MUTSU -- disrf does not appear to be in use anywhere in the code base!! */
/*#if FLT_MANT_DIG == DBL_MANT_DIG && FLT_MAX_EXP == DBL_MAX_EXP*/
#if SIZEOF_FLOAT == SIZEOF_DOUBLE
#define disrf(chan, retval) (float)disrd(chan, (retval))
#else
float disrf (struct tcp_chan *chan, int *retval);
#endif

int diswul (struct tcp_chan *chan, unsigned long value);
/*#if UINT_MAX == ULONG_MAX*/
#if SIZEOF_UNSIGNED_INT == SIZEOF_UNSIGNED_LONG
#define diswui(chan, value) diswul(chan, (unsigned long)(value))
#else
int diswui (struct tcp_chan *chan, unsigned value);
#endif
#define diswus(chan, value) diswui(chan, (unsigned)(value))
#define diswuc(chan, value) diswui(chan, (unsigned)(value))

int diswsl (struct tcp_chan *chan, long value);
/*#if INT_MIN == LONG_MIN && INT_MAX == LONG_MAX*/
#if SIZEOF_INT == SIZEOF_LONG
#define diswsi(chan, value) diswsl(chan, (long)(value))
#else
int diswsi (struct tcp_chan *chan, int value);
#endif
/* MUTSU -- diswss does not appear to be in use anywhere in the code base!! */
#define diswss(chan, value) diswsi(chan, (int)(value))
/* MUTSU -- diswsc does not appear to be in use anywhere in the code base!! */
#define diswsc(chan, value) diswsi(chan, (int)(value))

/* MUTSU -- diswc does not appear to be in use anywhere in the code base!! */
/*#if CHAR_MIN*/
#ifdef __CHAR_UNSIGNED__
#define diswc(chan, value) diswui(chan, (unsigned)(value))
#else
#define diswc(chan, value) diswsi(chan, (int)(value))
#endif

int diswcs (struct tcp_chan *chan, const char *value, size_t nchars);
#define diswst(chan, value) diswcs(chan, value, strlen(value))

int diswl_ (struct tcp_chan *chan, dis_long_double_t value, unsigned int ndigs);
/* MUTSU -- diswl does not appear to be in use anywhere in the code base!! */
#define diswl(chan, value) diswl_(chan, (value), LDBL_DIG)
/* MUTSU -- diswd does not appear to be in use anywhere in the code base!! */
#define diswd(chan, value) diswl_(chan, (dis_long_double_t)(value), DBL_DIG)
/*#if FLT_MANT_DIG == DBL_MANT_DIG || DBL_MANT_DIG == LDBL_MANT_DIG*/
#if SIZEOF_FLOAT == SIZEOF_DOUBLE
#define diswf(chan,value) diswl_(chan,(dis_long_double_t)(value),FLT_DIG)
#else
int diswf (struct tcp_chan *chan, double value);
#endif

void disiui_();

extern const char *dis_emsg[];

/* the following routines set/control DIS over tcp */

extern struct tcp_chan * DIS_tcp_setup (int fd);
extern int  DIS_tcp_wflush (struct tcp_chan *chan);
extern void DIS_tcp_settimeout (long timeout);
extern void DIS_tcp_cleanup(struct tcp_chan *chan);
extern void DIS_tcp_close(struct tcp_chan *chan);


/* NOTE:  increase THE_BUF_SIZE to 131072 for systems > 5k nodes */

/* NOTE: THE_BUF_SZIE will be similar in size to PBS_ACCT_MAX_RCD in acct.h */

#define THE_BUF_SIZE 262144 /* max size of tcp send buffer (must be big enough to contain all job attributes) */
#define THE_LIST_SIZE 16384 /* Initial size of sister host_exec string buffer */


/* used to tell dis functions whether or not to use tcp or rpp */
#define RPP_FUNC 1
#define TCP_FUNC 0

#endif /* DATA_IS_STRINGS_ */
