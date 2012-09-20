/*
*         OpenPBS (Portable Batch System) v2.3 Software License
*
* Copyright (c) 1999-2010 Veridian Information Solutions, Inc.
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


#include <errno.h>
#include <signal.h>
#include <stdlib.h>
#include <stdint.h>
#include <time.h>
#include "pbs_error.h"
#include "utils.h"
#include "hash_table.h"


/* PLEASE NOTE:
 *
 * This hash table isn't designed to be an industrial-strength,
 * all-purpose hash table. Right now it is intended only to be used 
 * to hold indexes for resizable arrays 
 */

/*
 * -------------------------------------------------------------------------------
 *  Hashing functions obtained from http://burtleburtle.net/bob/c/lookup3.c
 *  lookup3.c, by Bob Jenkins, May 2006, Public Domain.
 *
 *  These are functions for producing 32-bit hashes for hash table lookup.
 *  hashword(), hashlittle(), hashlittle2(), hashbig(), mix(), and final() 
 *  are externally useful functions.  Routines to test the hash are included 
 *  if SELF_TEST is defined.  You can use this free for any purpose.  It's in
 *  the public domain.  It has no warranty.
 *
 *  You probably want to use hashlittle().  hashlittle() and hashbig()
 *  hash byte arrays.  hashlittle() is is faster than hashbig() on
 *  little-endian machines.  Intel and AMD are little-endian machines.
 *  On second thought, you probably want hashlittle2(), which is identical to
 *  hashlittle() except it returns two 32-bit hashes for the price of one.  
 *  You could implement hashbig2() if you wanted but I haven't bothered here.
 *
 *  If you want to find a hash of, say, exactly 7 integers, do
 *  a = i1;  b = i2;  c = i3;
 *  mix(a,b,c);
 *  a += i4; b += i5; c += i6;
 *  mix(a,b,c);
 *  a += i7;
 *  final(a,b,c);
 *  then use c as the hash value.  If you have a variable length array of
 *  4-byte integers to hash, use hashword().  If you have a byte array (like
 *  a character string), use hashlittle().  If you have several byte arrays, or
 *  a mix of things, see the comments above hashlittle().  
 *
 *  Why is this so big?  I read 12 bytes at a time into 3 4-byte integers, 
 *  then mix those integers.  This is fast (you can do a lot more thorough
 *  mixing with 12*3 instructions on 3 integers than you can with 3 instructions
 *  on 1 byte), but shoehorning those bytes into integers efficiently is messy.
 *  -------------------------------------------------------------------------------
 */




/*
 * My best guess at if you are big-endian or little-endian.  This may
 * need adjustment.
 */


#define hashsize(n) ((uint32_t)1<<(n))
#define hashmask(n) (hashsize(n)-1)
#define rot(x,k) (((x)<<(k)) | ((x)>>(32-(k))))

/*
 * -------------------------------------------------------------------------------
 *  mix -- mix 3 32-bit values reversibly.
 *
 *  This is reversible, so any information in (a,b,c) before mix() is
 *  still in (a,b,c) after mix().
 *
 *  If four pairs of (a,b,c) inputs are run through mix(), or through
 *  mix() in reverse, there are at least 32 bits of the output that
 *  are sometimes the same for one pair and different for another pair.
 *  This was tested for:
 *  * pairs that differed by one bit, by two bits, in any combination
 *    of top bits of (a,b,c), or in any combination of bottom bits of
 *    (a,b,c).
 *  * "differ" is defined as +, -, ^, or ~^.  For + and -, I transformed
 *    the output delta to a Gray code (a^(a>>1)) so a string of 1's (as
 *    is commonly produced by subtraction) look like a single 1-bit
 *    difference.
 *  * the base values were pseudorandom, all zero but one bit set, or 
 *    all zero plus a counter that starts at zero.
 *
 *  Some k values for my "a-=c; a^=rot(c,k); c+=b;" arrangement that
 *  satisfy this are
 *  4  6  8 16 19  4
 *  9 15  3 18 27 15
 *  14  9  3  7 17  3
 *  Well, "9 15 3 18 27 15" didn't quite get 32 bits diffing
 *  for "differ" defined as + with a one-bit base and a two-bit delta.  I
 *  used http://burtleburtle.net/bob/hash/avalanche.html to choose 
 *  the operations, constants, and arrangements of the variables.
 *
 *  This does not achieve avalanche.  There are input bits of (a,b,c)
 *  that fail to affect some output bits of (a,b,c), especially of a.  The
 *  most thoroughly mixed value is c, but it doesn't really even achieve
 *  avalanche in c.
 *
 *  This allows some parallelism.  Read-after-writes are good at doubling
 *  the number of bits affected, so the goal of mixing pulls in the opposite
 *  direction as the goal of parallelism.  I did what I could.  Rotates
 *  seem to cost as much as shifts on every machine I could lay my hands
 *  on, and rotates are much kinder to the top and bottom bits, so I used
 *  rotates.
 *  -------------------------------------------------------------------------------
 */


#define mix(a,b,c) \
  { \
    a -= c;  a ^= rot(c, 4);  c += b; \
    b -= a;  b ^= rot(a, 6);  a += c; \
    c -= b;  c ^= rot(b, 8);  b += a; \
    a -= c;  a ^= rot(c,16);  c += b; \
    b -= a;  b ^= rot(a,19);  a += c; \
    c -= b;  c ^= rot(b, 4);  b += a; \
  }



/*
 * -------------------------------------------------------------------------------
 *  final -- final mixing of 3 32-bit values (a,b,c) into c
 *
 *  Pairs of (a,b,c) values differing in only a few bits will usually
 *  produce values of c that look totally different.  This was tested for
 *  * pairs that differed by one bit, by two bits, in any combination
 *    of top bits of (a,b,c), or in any combination of bottom bits of
 *    (a,b,c).
 *  * "differ" is defined as +, -, ^, or ~^.  For + and -, I transformed
 *     the output delta to a Gray code (a^(a>>1)) so a string of 1's (as
 *     is commonly produced by subtraction) look like a single 1-bit
 *     difference.
 *  * the base values were pseudorandom, all zero but one bit set, or 
 *    all zero plus a counter that starts at zero.
 *
 *  These constants passed:
 *  14 11 25 16 4 14 24
 *  12 14 25 16 4 14 24
 *  and these came close:
 *  4  8 15 26 3 22 24
 *  10  8 15 26 3 22 24
 *  11  8 15 26 3 22 24
 *  -------------------------------------------------------------------------------
 */




#define final(a,b,c) \
  { \
    c ^= b; c -= rot(b,14); \
    a ^= c; a -= rot(c,11); \
    b ^= a; b -= rot(a,25); \
    c ^= b; c -= rot(b,16); \
    a ^= c; a -= rot(c,4);  \
    b ^= a; b -= rot(a,14); \
    c ^= b; c -= rot(b,24); \
  }




/*
 * -------------------------------------------------------------------------------
 *  hashlittle() -- hash a variable-length key into a 32-bit value
 *  k       : the key (the unaligned variable-length array of bytes)
 *  length  : the length of the key, counting by bytes
 *  initval : can be any 4-byte value dbeer - change to seeded random value
 *  Returns a 32-bit value.  Every bit of the key affects every bit of
 *  the return value.  Two keys differing by one or two bits will have
 *  totally different hash values.
 *
 *  The best hash table sizes are powers of 2.  There is no need to do
 *  mod a prime (mod is sooo slow!).  If you need less than 32 bits,
 *  use a bitmask.  For example, if you need only 10 bits, do
 *  h = (h & hashmask(10));
 *  In which case, the hash table should have hashsize(10) elements.
 *
 *  If you are hashing n strings (uint8_t **)k, do it like this:
 *  for (i=0, h=0; i<n; ++i) h = hashlittle( k[i], len[i], h);
 *
 *  By Bob Jenkins, 2006.  bob_jenkins@burtleburtle.net.  You may use this
 *  code any way you wish, private, educational, or commercial.  It's free.
 *
 *  Use for hash table lookup, or anything where one collision in 2^^32 is
 *  acceptable.  Do NOT use for cryptographic purposes.
 *  -------------------------------------------------------------------------------
 *
 *  dbeer: function has been slightly reformatted for 
 */

uint32_t hashlittle( const void *key, size_t length, uint32_t initval)
  {
  uint32_t a,b,c;                                          /* internal state */
  union { const void *ptr; size_t i; } u;     /* needed for Mac Powerbook G4 */
  
  /* Set up the internal state */
  a = b = c = 0xdeadbeef + ((uint32_t)length) + initval;
  
  u.ptr = key;
  if (HASH_LITTLE_ENDIAN && ((u.i & 0x3) == 0)) 
    {
    const uint32_t *k = (const uint32_t *)key;         /* read 32-bit chunks */
    const uint8_t  *k8;
    
    /*------ all but last block: aligned reads and affect 32 bits of (a,b,c) */
    while (length > 12)
      {
      a += k[0];
      b += k[1];
      c += k[2];
      mix(a,b,c);
      length -= 12;
      k += 3;
      }
    
    /*----------------------------- handle the last (probably partial) block */
    /* 
     * "k[2]&0xffffff" actually reads beyond the end of the string, but
     * then masks off the part it's not allowed to read.  Because the
     * string is aligned, the masked-off tail is in the same word as the
     * rest of the string.  Every machine with memory protection I've seen
     * does it on word boundaries, so is OK with this.  But VALGRIND will
     * still catch it and complain.  The masking trick does make the hash
     * noticably faster for short strings (like English words).
     */

    /* dbeer: slightly modified - I'm using only the valgrind friendly code */
    
    k8 = (const uint8_t *)k;
    switch(length)
      {
      case 12: c+=k[2]; b+=k[1]; a+=k[0]; break;
      case 11: c+=((uint32_t)k8[10])<<16;  /* fall through */
      case 10: c+=((uint32_t)k8[9])<<8;    /* fall through */
      case 9 : c+=k8[8];                   /* fall through */
      case 8 : b+=k[1]; a+=k[0]; break;
      case 7 : b+=((uint32_t)k8[6])<<16;   /* fall through */
      case 6 : b+=((uint32_t)k8[5])<<8;    /* fall through */
      case 5 : b+=k8[4];                   /* fall through */
      case 4 : a+=k[0]; break;
      case 3 : a+=((uint32_t)k8[2])<<16;   /* fall through */
      case 2 : a+=((uint32_t)k8[1])<<8;    /* fall through */
      case 1 : a+=k8[0]; break;
      case 0 : return c;
      }
    
    } 
  else if (HASH_LITTLE_ENDIAN && ((u.i & 0x1) == 0)) 
    {
    const uint16_t *k = (const uint16_t *)key;         /* read 16-bit chunks */
    const uint8_t  *k8;
    
    /*--------------- all but last block: aligned reads and different mixing */
    while (length > 12)
      {
      a += k[0] + (((uint32_t)k[1])<<16);
      b += k[2] + (((uint32_t)k[3])<<16);
      c += k[4] + (((uint32_t)k[5])<<16);
      mix(a,b,c);
      length -= 12;
      k += 6;
      }
    
    /*----------------------------- handle the last (probably partial) block */
    k8 = (const uint8_t *)k;
    switch(length)
      {
      case 12: 
        c+=k[4]+(((uint32_t)k[5])<<16);
        b+=k[2]+(((uint32_t)k[3])<<16);
        a+=k[0]+(((uint32_t)k[1])<<16);
        break;
      case 11: 
        c+=((uint32_t)k8[10])<<16;     /* fall through */  
      case 10: 
        c+=k[4]; 
        b+=k[2]+(((uint32_t)k[3])<<16);   
        a+=k[0]+(((uint32_t)k[1])<<16);
        break;
      case 9: 
        c+=k8[8];                      /* fall through */
      case 8: 
        b+=k[2]+(((uint32_t)k[3])<<16);
        a+=k[0]+(((uint32_t)k[1])<<16);
        break;
      case 7: 
        b+=((uint32_t)k8[6])<<16;      /* fall through */
      case 6: 
        b+=k[2];
        a+=k[0]+(((uint32_t)k[1])<<16);
        break;
      case 5:
        b+=k8[4];                      /* fall through */
      case 4:
        a+=k[0]+(((uint32_t)k[1])<<16);
        break;
      case 3:
        a+=((uint32_t)k8[2])<<16;      /* fall through */
      case 2:
        a+=k[0];
        break;
      case 1:
        a+=k8[0];
        break;
      case 0:
        return c;                     /* zero length requires no mixing */
      }
    } 
  else 
    {                        /* need to read the key one byte at a time */
    const uint8_t *k = (const uint8_t *)key;
    /*--------------- all but the last block: affect some 32 bits of (a,b,c) */
    while (length > 12)
      {
      a += k[0];
      a += ((uint32_t)k[1])<<8;
      a += ((uint32_t)k[2])<<16;
      a += ((uint32_t)k[3])<<24;
      b += k[4];
      b += ((uint32_t)k[5])<<8;
      b += ((uint32_t)k[6])<<16;
      b += ((uint32_t)k[7])<<24;
      c += k[8];
      c += ((uint32_t)k[9])<<8;
      c += ((uint32_t)k[10])<<16;
      c += ((uint32_t)k[11])<<24;
      mix(a,b,c);
      length -= 12;
      k += 12;
      }

    /*-------------------------------- last block: affect all 32 bits of (c) */
    switch(length)                   /* all the case statements fall through */
      {
      case 12:
        c+=((uint32_t)k[11])<<24;
      case 11: 
        c+=((uint32_t)k[10])<<16;
      case 10:
        c+=((uint32_t)k[9])<<8;
      case 9:
        c+=k[8];
      case 8:
        b+=((uint32_t)k[7])<<24;
      case 7:
        b+=((uint32_t)k[6])<<16;
      case 6:
        b+=((uint32_t)k[5])<<8;
      case 5:
        b+=k[4];
      case 4:
        a+=((uint32_t)k[3])<<24;
      case 3:
        a+=((uint32_t)k[2])<<16;
      case 2:
        a+=((uint32_t)k[1])<<8;
      case 1:
        a+=k[0];
        break;
      case 0:
        return c;
      }
    }
  
  final(a,b,c);
  return c;
  } /* END hashlittle() */




/*
 * hashbig():
 * This is the same as hashword() on big-endian machines.  It is different
 * from hashlittle() on all machines.  hashbig() takes advantage of
 * big-endian byte ordering. 
 */
uint32_t hashbig( const void *key, size_t length, uint32_t initval)
  {
  uint32_t a,b,c;
  union { const void *ptr; size_t i; } u; /* to cast key to (size_t) happily */
  
  /* Set up the internal state */
  a = b = c = 0xdeadbeef + ((uint32_t)length) + initval;
  
  u.ptr = key;
  if (HASH_BIG_ENDIAN && ((u.i & 0x3) == 0)) 
    {
    const uint32_t *k = (const uint32_t *)key;         /* read 32-bit chunks */
    const uint8_t  *k8;
    
    /*------ all but last block: aligned reads and affect 32 bits of (a,b,c) */
    while (length > 12)
      {
      a += k[0];
      b += k[1];
      c += k[2];
      mix(a,b,c);
      length -= 12;
      k += 3;
      }
    
    /*----------------------------- handle the last (probably partial) block */
    /* 
     * "k[2]<<8" actually reads beyond the end of the string, but
     * then shifts out the part it's not allowed to read.  Because the
     * string is aligned, the illegal read is in the same word as the
     * rest of the string.  Every machine with memory protection I've seen
     * does it on word boundaries, so is OK with this.  But VALGRIND will
     * still catch it and complain.  The masking trick does make the hash
     * noticably faster for short strings (like English words).
     *
     * dbeer- also using only the valgrind friendly code 
     */

    k8 = (const uint8_t *)k;
    switch(length)                   /* all the case statements fall through */
      {
      case 12: 
        c+=k[2];
        b+=k[1]; 
        a+=k[0];
        break;
      case 11:
        c+=((uint32_t)k8[10])<<8;  /* fall through */
      case 10:
        c+=((uint32_t)k8[9])<<16;  /* fall through */
      case 9:
        c+=((uint32_t)k8[8])<<24;  /* fall through */
      case 8:
        b+=k[1]; a+=k[0];
        break;
      case 7:
        b+=((uint32_t)k8[6])<<8;   /* fall through */
      case 6:
        b+=((uint32_t)k8[5])<<16;  /* fall through */
      case 5:
        b+=((uint32_t)k8[4])<<24;  /* fall through */
      case 4:
        a+=k[0]; break;
      case 3:
        a+=((uint32_t)k8[2])<<8;   /* fall through */
      case 2:
        a+=((uint32_t)k8[1])<<16;  /* fall through */
      case 1:
        a+=((uint32_t)k8[0])<<24; break;
      case 0:
        return c;
      } 
    } 
  else 
    {                        /* need to read the key one byte at a time */
    const uint8_t *k = (const uint8_t *)key;
    
    /*--------------- all but the last block: affect some 32 bits of (a,b,c) */
    while (length > 12)
      {
      a += ((uint32_t)k[0])<<24;
      a += ((uint32_t)k[1])<<16;
      a += ((uint32_t)k[2])<<8;
      a += ((uint32_t)k[3]);
      b += ((uint32_t)k[4])<<24;
      b += ((uint32_t)k[5])<<16;
      b += ((uint32_t)k[6])<<8;
      b += ((uint32_t)k[7]);
      c += ((uint32_t)k[8])<<24;
      c += ((uint32_t)k[9])<<16;
      c += ((uint32_t)k[10])<<8;
      c += ((uint32_t)k[11]);
      mix(a,b,c);
      length -= 12;
      k += 12;
      }
    
    /*-------------------------------- last block: affect all 32 bits of (c) */
    switch(length)                   /* all the case statements fall through */
      {
      case 12:
        c+=k[11];
      case 11:
        c+=((uint32_t)k[10])<<8;
      case 10:
        c+=((uint32_t)k[9])<<16;
      case 9:
        c+=((uint32_t)k[8])<<24;
      case 8:
        b+=k[7];
      case 7:
        b+=((uint32_t)k[6])<<8;
      case 6:
        b+=((uint32_t)k[5])<<16;
      case 5: 
        b+=((uint32_t)k[4])<<24;
      case 4:
        a+=k[3];
      case 3:
        a+=((uint32_t)k[2])<<8;
      case 2:
        a+=((uint32_t)k[1])<<16;
      case 1:
        a+=((uint32_t)k[0])<<24;
        break;
      case 0:
        return c;
      }
    }
  
  final(a,b,c);
  return c;
  } /* END hashbig() */




hash_table_t *create_hash(

  int size)

  {
  hash_table_t *ht = calloc(1, sizeof(hash_table_t));
  size_t        amount = sizeof(bucket *) * size;

  ht->size = size;
  ht->num  = 0;
  ht->buckets = calloc(1, amount);

  return(ht);
  } /* END create_hash() */





void free_buckets(

  bucket **buckets,
  int      size)

  {
  int     i;
  bucket *b;
  bucket *tmp;

  for (i = 0; i < size; i++)
    {
    b = buckets[i];

    while (b != NULL)
      {
      tmp = b;
      b = b->next;

      free(tmp);
      }
    }
  } /* END free_buckets() */




/* 
 * NOTE: this should only be called right before a hash table is freed
 * and *ONLY* if the keys are all OWNED by the hash table and nobody else
 */

void free_all_keys(
    
  hash_table_t *ht) 

  {
  bucket *b;
  int     i;

  for (i = 0; i < ht->size; i++)
    {
    b = ht->buckets[i];

    while (b != NULL)
      {
      free(b->key);
      b = b->next;
      }
    }

  } /* END free_all_keys() */




void free_hash(

  hash_table_t *ht)

  {
  free_buckets(ht->buckets, ht->size);
  free(ht->buckets);
  free(ht);
  } /* END free_hash() */




int get_hash(
    
  hash_table_t *ht,
  void         *key)

  {
  size_t   len;
  uint32_t hash;
  uint32_t mask = 0;
  uint32_t counter;

  if (ht == NULL)
    return(-1);
  if (key == NULL)
    return(-1);

  len = strlen(key);
  counter = ht->size >> 1;

#ifdef HASH_LITTLE_ENDIAN
  hash = hashlittle(key,len,0);
#else
  hash = hashbig(key,len,0);
#endif

  for (mask = 0; counter != 0; counter = counter >> 1)
    mask |= counter;
  
  return(hash & mask);
  } /* END get_hash() */






void add_to_bucket(

  bucket **buckets,
  int      index,
  void    *key,
  int      value)

  {
  if (buckets[index] == NULL)
    {
    /* empty bucket, just add it here */
    buckets[index] = calloc(1, sizeof(bucket));
    buckets[index]->value = value;
    buckets[index]->key   = key;
    buckets[index]->next  = NULL;
    }
  else
    {
    /* make this the last bucket at the end */
    bucket *b = buckets[index];

    while (b->next != NULL)
      b = b->next;

    b->next = calloc(1, sizeof(bucket));
    b->next->value = value;
    b->next->key   = key;
    b->next->next  = NULL;
    }
  } /* END add_to_bucket() */





int add_hash(
    
  hash_table_t *ht,
  int           value,
  void         *key)

  {
  int index;

  /* check if we need to rehash */
  if (ht->size == ht->num)
    {
    int      old_bucket_size = ht->size;
    int      new_bucket_size = ht->size * 2;
    size_t   amount = new_bucket_size * sizeof(bucket *);
    bucket **tmp = calloc(1, amount);
    int      i;
    int      new_index;
      
    ht->size = new_bucket_size;

    /* rehash all of the hold values */
    for (i = 0; i < old_bucket_size; i++)
      {
      bucket *b = ht->buckets[i];

      while (b != NULL)
        {
        new_index = get_hash(ht, b->key);

        add_to_bucket(tmp,new_index,b->key,b->value);

        b = b->next;
        }
      } /* END foreach (old buckets) */

    free_buckets(ht->buckets, old_bucket_size);

    free(ht->buckets);

    ht->buckets = tmp;
    } /* END if (need to rehash) */

  index = get_hash(ht, key);

  add_to_bucket(ht->buckets,index,key,value);

  ht->num++;

  return(PBSE_NONE);
  } /* END add_hash() */





int remove_hash(
    
  hash_table_t *ht,
  char         *key)

  {
  int     rc = PBSE_NONE;
  int     index = get_hash(ht, key);
  bucket *b = ht->buckets[index];
  bucket *prev = NULL;

  while (b != NULL)
    {
    if (!strcmp(key,b->key))
      break;

    prev = b;
    b = b->next;
    }

  if (b == NULL)
    rc = KEY_NOT_FOUND;
  else
    {
    if (prev == NULL)
      {
      if (b->next == NULL)
        {
        free(b);
        ht->buckets[index] = NULL;
        }
      else
        {
        /* copy the next bucket here and release it */
        ht->buckets[index] = b->next;

        free(b);
        }
      }
    else
      {
      prev->next = b->next;
      free(b);
      }

    ht->num--;
    } /* END found */

  return(rc);
  } /* END remove_hash() */




/* 
 * find the bucket with key in it and returns the value
 * this is used as a quick lookup to find a job instead of a linear search.
 */
int get_value_hash(
    
  hash_table_t *ht,
  void         *key)

  {
  int     value = -1;
  int     hash_index;
  bucket *b;

  if (ht == NULL)
    return(-1);
  if (key == NULL)
    return(-1);

  hash_index = get_hash(ht, key);
  if (hash_index < 0)
    return(-1);

  b = ht->buckets[hash_index];
  while (b != NULL)
    {
    if (!strcmp(b->key, key))
      {
      value = b->value;
      break;
      }

    b = b->next;
    }

  return(value);
  } /* END get_value_from_hash() */






void change_value_hash(

  hash_table_t *ht,
  char         *key,
  int           new_value)

  {
  int index = get_hash(ht, key);
  bucket *b = ht->buckets[index];

  while (b != NULL)
    {
    if (!strcmp(b->key,key))
      {
      b->value = new_value;
      break;
      }

    b = b->next;
    }
  } /* END change_value_hash() */




