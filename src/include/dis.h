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


unsigned long disrul (int stream, int *retval);

/*#if UINT_MAX == ULONG_MAX*/
#if SIZEOF_UNSIGNED == SIZEOF_LONG
#define disrui(stream, retval) (unsigned)disrul(stream, (retval))
#else
unsigned disrui (int stream, int *retval);
#endif

/*#if USHRT_MAX == UINT_MAX*/
#if SIZEOF_UNSIGNED_SHORT == SIZEOF_UNSIGNED_INT
#define disrus(strea, retval) (unsigned short)disrui(stream, (retval))
#else
unsigned short disrus (int stream, int *retval);
#endif

/*#if UCHAR_MAX == USHRT_MAX*/
#if SIZEOF_UNSIGNED_CHAR == SIZEOF_UNSIGNED_SHORT
#define disruc(stream, retval) (unsigned char)disrus(stream, (retval))
#else
unsigned char disruc (int stream, int *retval);
#endif

long disrsl (int stream, int *retval);
/*#if INT_MIN == LONG_MIN && INT_MAX == LONG_MAX*/
#if SIZEOF_INT == SIZEOF_LONG
#define disrsi(stream, retval) (int)disrsl(stream, (retval))
#else
int disrsi (int stream, int *retval);
#endif

/*#if SHRT_MIN == INT_MIN && SHRT_MAX == INT_MAX*/
#if SIZEOF_SHORT == SIZEOF_INT
#define disrss(stream, retval) (short)disrsi(stream, (retval))
#else
short disrss (int stream, int *retval);
#endif

/*#if CHAR_MIN == SHRT_MIN && CHAR_MAX == SHRT_MAX*/
#if SIZEOF_SIGNED_CHAR == SIZEOF_SHORT
#define disrsc(stream, retval) (signed char)disrss(stream, (retval))
#else
signed char disrsc (int stream, int *retval);
#endif

/*#if CHAR_MIN, i.e. if chars are signed*/
/* also, flip the order of statements */
#ifdef __CHAR_UNSIGNED__
#define disrc(retval, stream) (char)disruc(stream, (retval))
#else
#define disrc(stream, retval) (char)disrsc(stream, (retval))
#endif

char *disrcs(int stream, size_t *nchars, int *retval);
int disrfcs(int stream, size_t *nchars, size_t achars, char *value);
char *disrst(int stream, int *retval);
int disrfst(int stream, size_t achars, char *value);

/*
 * some compilers do not like long doubles, if long double is the same
 * as a double, just use a double.
 */
#if SIZEOF_DOUBLE == SIZEOF_LONG_DOUBLE
typedef double dis_long_double_t;
#else
typedef long double dis_long_double_t;
#endif

dis_long_double_t disrl (int stream, int *retval);
/*#if DBL_MANT_DIG == LDBL_MANT_DIG && DBL_MAX_EXP == LDBL_MAX_EXP*/
#if SIZEOF_DOUBLE == SIZEOF_LONG_DOUBLE
#define disrd(stream, retval) (double)disrl(stream, (retval))
#else
double disrd (int stream, int *retval);
#endif

/*#if FLT_MANT_DIG == DBL_MANT_DIG && FLT_MAX_EXP == DBL_MAX_EXP*/
#if SIZEOF_FLOAT == SIZEOF_DOUBLE
#define disrf(stream, retval) (float)disrd(stream, (retval))
#else
float disrf (int stream, int *retval);
#endif

int diswul (int stream, unsigned long value);
/*#if UINT_MAX == ULONG_MAX*/
#if SIZEOF_UNSIGNED_INT == SIZEOF_UNSIGNED_LONG
#define diswui(stream, value) diswul(stream, (unsigned long)(value))
#else
int diswui (int stream, unsigned value);
#endif
#define diswus(stream, value) diswui(stream, (unsigned)(value))
#define diswuc(stream, value) diswui(stream, (unsigned)(value))

int diswsl (int stream, long value);
/*#if INT_MIN == LONG_MIN && INT_MAX == LONG_MAX*/
#if SIZEOF_INT == SIZEOF_LONG
#define diswsi(stream, value) diswsl(stream, (long)(value))
#else
int diswsi (int stream, int value);
#endif
#define diswss(stream, value) diswsi(stream, (int)(value))
#define diswsc(stream, value) diswsi(stream, (int)(value))

/*#if CHAR_MIN*/
#ifdef __CHAR_UNSIGNED__
#define diswc(stream, value) diswui(stream, (unsigned)(value))
#else
#define diswc(stream, value) diswsi(stream, (int)(value))
#endif

int diswcs (int stream, const char *value, size_t nchars);
#define diswst(stream, value) diswcs(stream, value, strlen(value))

int diswl_ (int stream, dis_long_double_t value, unsigned int ndigs);
#define diswl(stream, value) diswl_(stream, (value), LDBL_DIG)
#define diswd(stream, value) diswl_(stream, (dis_long_double_t)(value), DBL_DIG)
/*#if FLT_MANT_DIG == DBL_MANT_DIG || DBL_MANT_DIG == LDBL_MANT_DIG*/
#if SIZEOF_FLOAT == SIZEOF_DOUBLE
#define diswf(stream,value) diswl_(stream,(dis_long_double_t)(value),FLT_DIG)
#else
int diswf (int stream, double value);
#endif


extern const char *dis_emsg[];

/* the following routines set/control DIS over tcp */

void DIS_tcp_reset (int fd, int rw);
void DIS_tcp_setup (int fd);
int  DIS_tcp_wflush (int fd);
void DIS_tcp_settimeout (long timeout);
int  DIS_tcp_istimeout (int fd);
void DIS_tcp_close (int fd);



extern int  PConnTimeout(int);

/* NOTE:  increase THE_BUF_SIZE to 131072 for systems > 5k nodes */

/* NOTE: THE_BUF_SZIE will be similar in size to PBS_ACCT_MAX_RCD in acct.h */

#define THE_BUF_SIZE 262144 /* max size of tcp send buffer (must be big enough to contain all job attributes) */


struct tcpdisbuf
  {
  unsigned long tdis_bufsize;
  char *tdis_leadp;
  char *tdis_trailp;
  char *tdis_eod;
  char  *tdis_thebuf;
  };

struct tcp_chan
  {

  struct tcpdisbuf readbuf;

  struct tcpdisbuf writebuf;

  int              IsTimeout;  /* (boolean)  1 - true */
  int              ReadErrno;
  int              SelectErrno;
  };

#endif /* DATA_IS_STRINGS_ */
