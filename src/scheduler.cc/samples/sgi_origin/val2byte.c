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
/* $Id$ */

/* Convert a an allocation value string to its equivalent value in bytes. */

#include <sys/param.h>
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "toolkit.h"

/* Power-of-two unit multipliers. */
#define KILO ((size_t)1024)
#define MEGA (KILO*1024)
#define GIGA (MEGA*1024)
#define TERA (GIGA*1024)

size_t 
schd_val2byte(char *val)
{
    /* char   *id = "schd_val2byte"; */
    int     b = 0;
    size_t num = 0;
    char   *p;

#if 0
    /* Dead code as of Feb 10, 1998.  Remove no later than Feb 10, 1999. */
    char buffer[1000];
#endif /* 0 */

    if (val == NULL)
	return (0);

    b = 0;
    num = 0;
    num = strtoul(val, &p, 0);

#if 0
    /* Dead code as of Feb 10, 1998.  Remove no later than Feb 10, 1999. */
    (void) sprintf(buffer,"1: val=[%s] num=%lld, p=[%s]",val,num,p);
    log_record(1,1,id,buffer);
#endif /* 0 */

    /* Now 'p' should point to first non-number character. */

    /* If no units given, return the number. */
    if ((p == val) || (*p == '\0'))
	return (num);	/* XXX return num if p == val? */

    /* Parse the given order of magnitude. */
    switch (*p) {
    case 'k':
    case 'K':
	num *= KILO;
	break;

    case 'm':
    case 'M':
	num *= MEGA;
	break;

    case 'g':
    case 'G':
	num *= GIGA;
	break;

    case 't':
    case 'T':
	num *= TERA;
	break;

    case 'b':
    case 'B':
	b++;
	break;

    default:
	return (-1);
    }

    /* XXX if (b && *p != '\0') return error. */

    /* Parse the word-size unit. */
    p++;
    if ((*p != '\0') && !b) {
	switch (*p) {
	case 'b':
	case 'B':
	    break;

	case 'w':
	case 'W':
	    num *= NBPW;	/* Multiply by size of word. */
	    break;

	default:
	    return (-1);
	}
    }

    /* Return the total number of bytes represented by the value. */
    return (num);
}
