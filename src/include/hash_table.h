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

#ifndef HASH_TABLE_H
#define HASH_TABLE_H

#include "endian.h" /* LITTLE_ENDIAN */

#ifdef __BYTE_ORDER
# if __BYTE_ORDER == __LITTLE_ENDIAN
#  define LITTLEENDIAN
# else
#  if __BYTE_ORDER == __BIG_ENDIAN
#   define BIGENDIAN
#  else
Error: unknown byte order!
#  endif
# endif
#endif /* __BYTE_ORDER */

#ifdef BYTE_ORDER
# if BYTE_ORDER == LITTLE_ENDIAN
#  define LITTLEENDIAN
# else
#  if BYTE_ORDER == BIG_ENDIAN
#   define BIGENDIAN
#  else
Error: unknown byte order!
#  endif
# endif
#endif /* BYTE_ORDER */

#if defined(__AIX43) || defined(__AIX51) || defined(__AIX52) || defined(__AIX53) || defined(__AIX54) || defined(__HPUX) || defined(__IRIX) || defined(__SOLARIS) || defined(__UNICOS) || defined(MBIGENDIAN)

/*
 * BIG_ENDIAN is used for runtime checking of architecture
 * BIGENDIAN  is used for compile-time checking of architecture
 */

/* big endian arch */
# define HASH_BIG_ENDIAN    1
# define HASH_LITTLE_ENDIAN 0

#else

/*
 * LITTLE_ENDIAN is used for runtime checking of architecture
 * LITTLEENDIAN  is used for compile-time checking of architecture
 */


/* little endian arch */
# define HASH_BIG_ENDIAN    0
# define HASH_LITTLE_ENDIAN 1

#endif /* defined(__AIX43) || ... */

#if !defined(BIGENDIAN) && !defined(LITTLE_ENDIAN)
Error: unknown byte order!
#endif

#define KEY_NOT_FOUND     -1
#define INITIAL_HASH_SIZE 8096

/* PLEASE NOTE:
 *
 * This hash table isn't designed to be an industrial-strength,
 * all-purpose hash table. Right now it is intended only to be used 
 * to hold indexes for resizable arrays 
 */

/* struct definitions */

struct bucket
  {
  struct bucket *next;
  int            value;
  char          *key;
  };

typedef struct bucket bucket;

struct hash_table_t
  {
  int      size;
  int      num;
  bucket **buckets;
  };

typedef struct hash_table_t hash_table_t;

int           get_value_hash(hash_table_t *, void *);
int           get_hash(hash_table_t *, void *);
int           add_hash(hash_table_t *, int, void *);
int           remove_hash(hash_table_t *, char *);
void          change_value_hash(hash_table_t *, char *, int);
hash_table_t *create_hash(int);
void          free_hash(hash_table_t *);
void          free_all_keys(hash_table_t *);

#endif /* HASH_TABLE_H */
