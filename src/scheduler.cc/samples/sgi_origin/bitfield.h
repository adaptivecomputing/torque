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

/*
 *
 * Definition of interface for dealing with arbitrarily large numbers of
 * contiguous bits.  Size of the bitfield is declared at compile time with
 * the BITFIELD_SIZE #define (default is 128 bits).
 * 
 * Macros/inlines all take pointers to a Bitfield, and provide :
 * 
 * Macro BITFIELD_CLRALL(Bitfield *p)
 * Macro BITFIELD_SETALL(Bitfield *p)
 * 	Clear or set all bits in a Bitfield.
 * 
 * Macro BITFIELD_SET_LSB(Bitfield *p)
 * Macro BITFIELD_CLR_LSB(Bitfield *p)
 * Macro BITFIELD_SET_MSB(Bitfield *p)
 * Macro BITFIELD_CLR_MSB(Bitfield *p)
 *	Set the least or most significant bit of a Bitfield.
 * 
 * Macro BITFIELD_LSB_ISONE(Bitfield *p)
 * Macro BITFIELD_MSB_ISONE(Bitfield *p)
 * 	Equals non-zero if the least or most significant bit of the Bitfield
 * 	    is set, or zero otherwise.
 *
 * Macro BITFIELD_SETB(Bitfield *p, int bit)
 * Macro BITFIELD_CLRB(Bitfield *p, int bit)
 * Macro BITFIELD_TSTB(Bitfield *p, int bit)
 * 	Set, clear, or test the bit at position 'bit' in the Bitfield '*p'.
 * 	BITFIELD_TSTB() is non-zero if the bit at position 'bit' is set, or
 *	     zero if it is clear.
 * 
 * Inline BITFIELD_IS_ZERO(Bitfield *p)
 * Inline BITFIELD_IS_ONES(Bitfield *p)
 * 	Return non-zero if the bitfield is composed of all zeros or ones,
 *	     or zero if the bitfield is non-homogeneous.
 * 
 * Inline BITFIELD_IS_NONZERO(Bitfield *p)
 *	Returns non-zero if the bitfield contains at least one set bit.
 *
 * Inline BITFIELD_NUM_ONES(Bitfield *p)
 *	Returns number of '1' bits in the bitfield.
 * 
 * Inline BITFIELD_MS_ONE(Bitfield *p)
 * Inline BITFIELD_LS_ONE(Bitfield *p)
 *	Returns bit position number of least or most significant 1-bit in
 *	the Bitfield.
 * 
 * Inline BITFIELD_EQ(Bitfield *p, Bitfield *q)
 * Inline BITFIELD_NOTEQ(Bitfield *p, Bitfield *q)
 *	Return non-zero if Bitfields '*p' and '*q' are (not) equal.
 * 
 * Inline BITFIELD_SETM(Bitfield *p, Bitfield *mask)
 * Inline BITFIELD_CLRM(Bitfield *p, Bitfield *mask)
 * Inline BITFIELD_ANDM(Bitfield *p, Bitfield *mask)
 * Inline BITFIELD_TSTM(Bitfield *p, Bitfield *mask)
 * Inline BITFIELD_TSTALLM(Bitfield *p, Bitfield *mask)
 *	Apply the specified 'mask' to the given bitfield 'p':
 *	SETM() sets bits in 'p' for any bits set in 'mask' ('p |= mask').
 *	CLRM() clears bits in 'p' for any bits set in 'mask' ('p &=~ mask').
 *	ANDM() logical-and's 'mask' into 'p' ('p &= mask');
 *	TSTM() returns non-zero if *any* bits set in 'mask' are set in 'p'.
 *	TSTMALL() returns non-zero if *all* bits set in 'mask' are also set 
 *		in 'p'.
 * 
 * Inline BITFIELD_CPY(Bitfield *p, Bitfield *q)
 * Inline BITFIELD_CPYNOTM(Bitfield *p, Bitfield *q)
 *	Copy the (inverse of) bitfield 'q' into 'p'.
 * 
 * Inline BITFIELD_ORNOTM(Bitfield *p, Bitfield *q)
 * 	Set any bits in 'p' where the corresponding bit in 'q' is clear.
 * 	    (p |= ~q)
 * 
 * Inline BITFIELD_SHIFTL(Bitfield *p)
 * Inline BITFIELD_SHIFTR(Bitfield *p)
 * 	Shift the bits in Bitfield 'p' one bit to the right or left.
 */

#ifndef	BITFIELD_H_
#define	BITFIELD_H_

/* The size of bitfields being used.  Default to 128 bits. */
#ifndef BITFIELD_SIZE
#define	BITFIELD_SIZE		128
#endif	/* !BITFIELD_SIZE */

#include <assert.h>
#define BITFIELD_BPW		((int)(sizeof (unsigned long long) * 8))

#define BITFIELD_SHIFT(bit)	((bit) / BITFIELD_BPW)
#define BITFIELD_OFFSET(bit)	((bit) & (BITFIELD_BPW - 1))
#define	BITFIELD_WORDS		(BITFIELD_SHIFT(BITFIELD_SIZE))

typedef struct bitfield {
	unsigned long long _bits[BITFIELD_WORDS];
} Bitfield;

#define	INLINE	__inline 

/* Operate on least significant bit of a bitfield. */

#define	BITFIELD_LSB_ISONE(p) \
    ((p)->_bits[0] & 1ULL)

#define	BITFIELD_SET_LSB(p) \
    ((p)->_bits[0] |= 1ULL)

#define	BITFIELD_CLR_LSB(p) \
    ((p)->_bits[0] &= ~(1ULL))


/* Operate on most significant bit of a bitfield. */

#define BITFIELD_MSB_ISONE(p) \
    ((p)->_bits[BITFIELD_SHIFT(BITFIELD_SIZE-1)] & (1ULL << (BITFIELD_BPW-1)))

#define BITFIELD_SET_MSB(p) \
    ((p)->_bits[BITFIELD_SHIFT(BITFIELD_SIZE-1)] |= (1ULL << (BITFIELD_BPW-1)))

#define BITFIELD_CLR_MSB(p) \
    ((p)->_bits[BITFIELD_SHIFT(BITFIELD_SIZE-1)] &= ~(1ULL << (BITFIELD_BPW-1)))


/* Operate on arbitrary bits within the bitfield. */

#define BITFIELD_SETB(p,bit)	(((bit) >= 0 && (bit) < BITFIELD_SIZE) ? \
    (p)->_bits[BITFIELD_SHIFT(bit)] |= (1ULL << BITFIELD_OFFSET(bit)) : 0)

#define BITFIELD_CLRB(p,bit)	(((bit) >= 0 && (bit) < BITFIELD_SIZE) ? \
    (p)->_bits[BITFIELD_SHIFT(bit)] &= ~(1ULL << BITFIELD_OFFSET(bit)) : 0)

#define BITFIELD_TSTB(p,bit)	(((bit) >= 0 && (bit) < BITFIELD_SIZE) ? \
    ((p)->_bits[BITFIELD_SHIFT(bit)] & (1ULL << BITFIELD_OFFSET(bit))) : 0)


/* Clear or set all the bits in the bitfield. */

#define BITFIELD_CLRALL(p)	{					\
	int w;								\
	assert(p != NULL);						\
	for (w = 0; w < BITFIELD_WORDS; w++)				\
		(p)->_bits[w] = 0ULL;					\
}

#define BITFIELD_SETALL(p)	{					\
	int w;								\
	assert(p != NULL);						\
	for (w = 0; w < BITFIELD_WORDS; w++)				\
		(p)->_bits[w] = ~(0ULL);				\
}


/* Comparison functions for two bitfield. */

INLINE int BITFIELD_IS_ZERO(Bitfield *p)		{
	int w;
	assert(p != NULL);
	for (w = 0; w < BITFIELD_WORDS; w++)
		if ((p)->_bits[w])
			return 0;
	return 1;
}

INLINE int BITFIELD_IS_ONES(Bitfield *p)		{
	int w;
	assert(p != NULL);
	for (w = 0; w < BITFIELD_WORDS; w++)
		if ((p)->_bits[w] != ~(0ULL))
			return 0;
	return 1;
}

INLINE int BITFIELD_IS_NONZERO(Bitfield *p)		{
	int w;
	assert(p != NULL);
	for (w = 0; w < BITFIELD_WORDS; w++)
		if ((p)->_bits[w])
			return 1;
	return 0;
}

INLINE int BITFIELD_NUM_ONES(Bitfield *p)		{
	int w, cnt;
	unsigned long long n;
	assert(p != NULL);

	cnt = 0;
	for (w = 0; w < BITFIELD_WORDS; w++)
		for (n = (p)->_bits[w]; n != 0ULL; cnt++)
			n &= (n - 1);

	return (cnt);
}

INLINE int BITFIELD_LS_ONE(Bitfield *p)			{
	int w, bit;
	unsigned long long n, x;
	assert(p != NULL);

	bit = 0;
	for (w = 0; w < BITFIELD_WORDS; w++) {
		n = (p)->_bits[w];

		/* Look for the first non-zero word. */
		if (n != 0ULL)
			break;
			
		bit += BITFIELD_BPW;
	}

	/* No non-zero words found in the bitfield. */
	if (w == BITFIELD_WORDS)
		return (-1);

	/* Slide a single bit left, looking for the non-zero bit. */
	for (x = 1ULL; !(n & x); bit ++)
		x <<= 1;

	return (bit);
}

INLINE int BITFIELD_MS_ONE(Bitfield *p)			{
	int w, bit;
	unsigned long long n, x;
	assert(p != NULL);

	bit = BITFIELD_SIZE - 1;
	for (w = BITFIELD_WORDS - 1; w >= 0; w--) {
		n = (p)->_bits[w];

		/* Look for the first non-zero word. */
		if (n != 0ULL)
			break;
			
		bit -= BITFIELD_BPW;
	}

	/* No non-zero words found in the bitfield. */
	if (w < 0)
		return (-1);

	/* Slide a single bit right, looking for the non-zero bit. */
	for (x = 1ULL << BITFIELD_BPW - 1; !(n & x); bit--)
		x >>= 1;

	return (bit);
}

INLINE int BITFIELD_EQ(Bitfield *p, Bitfield *q)	{
	int w;
	assert(p != NULL && q != NULL);
	for (w = 0; w < BITFIELD_WORDS; w++)
		if ((p)->_bits[w] != (q)->_bits[w])
			return 0;
	return 1;
}

INLINE int BITFIELD_NOTEQ(Bitfield *p, Bitfield *q)	{
	int w;
	assert(p != NULL && q != NULL);
	for (w = 0; w < BITFIELD_WORDS; w++)
		if ((p)->_bits[w] != (q)->_bits[w])
			return 1;
	return 0;
}


/* Logical manipulation functions for applying one bitfield to another. */

INLINE int BITFIELD_SETM(Bitfield *p, Bitfield *mask)	{
	int w;
	assert(p != NULL && mask != NULL);
	for (w = 0; w < BITFIELD_WORDS; w++)
		(p)->_bits[w] |= (mask)->_bits[w];
	return 0;
}

INLINE int BITFIELD_CLRM(Bitfield *p, Bitfield *mask)	{
	int w;
	assert(p != NULL && mask != NULL);
	for (w = 0; w < BITFIELD_WORDS; w++)
		(p)->_bits[w] &= ~((mask)->_bits[w]);
	return 0;
}

INLINE int BITFIELD_ANDM(Bitfield *p, Bitfield *mask)	{
	int w;
	assert(p != NULL && mask != NULL);
	for (w = 0; w < BITFIELD_WORDS; w++)
		(p)->_bits[w] &= (mask)->_bits[w];
	return 0;
}

INLINE int BITFIELD_TSTM(Bitfield *p, Bitfield *mask)	{
	int w;
	assert(p != NULL && mask != NULL);
	for (w = 0; w < BITFIELD_WORDS; w++)
		if ((p)->_bits[w] & (mask)->_bits[w])
			return 1;
	return 0;
}

INLINE int BITFIELD_TSTALLM(Bitfield *p, Bitfield *mask) {
	int w;
	assert(p != NULL && mask != NULL);
	for (w = 0; w < BITFIELD_WORDS; w++)
		if (((p)->_bits[w] & (mask)->_bits[w]) != (mask)->_bits[w])
			return 0;
	return 1;
}

INLINE int BITFIELD_CPY(Bitfield *p, Bitfield *q)	{
	int w;
	assert(p != NULL && q != NULL);
	for (w = 0; w < BITFIELD_WORDS; w++)
		(p)->_bits[w] = (q)->_bits[w];
	return 0;
}

INLINE int BITFIELD_CPYNOTM(Bitfield *p, Bitfield *q)	{
	int w;
	assert(p != NULL && q != NULL);
	for (w = 0; w < BITFIELD_WORDS; w++)
		(p)->_bits[w] = ~((q)->_bits[w]);
	return 0;
}

INLINE int BITFIELD_ORNOTM(Bitfield *p, Bitfield *q)	{
	int w;
	assert(p != NULL && q != NULL);
	for (w = 0; w < BITFIELD_WORDS; w++)
		(p)->_bits[w] |= ~((q)->_bits[w]);
	return 0;
}


/* Logical shift left and shift right for bitfield. */

INLINE int BITFIELD_SHIFTL(Bitfield *p)			{
	int w, upper;
	assert(p != NULL);
	
	for (w = 0; w < BITFIELD_WORDS - 1; w++) {
		upper = (p->_bits[w] & (1ULL << (BITFIELD_BPW - 1))) ? 1 : 0;
		p->_bits[w] <<= 1;
		p->_bits[w + 1] <<= 1;
		p->_bits[w + 1] |= upper;
	}
	return 0;
}

INLINE int BITFIELD_SHIFTR(Bitfield *p)			{
	int w, lower;
	assert(p != NULL);
	
	for (w = BITFIELD_WORDS - 1; w > 0; w--) {
		lower = p->_bits[w] & 1ULL;
		p->_bits[w] >>= 1;
		p->_bits[w - 1] >>= 1;
		p->_bits[w - 1] |= (lower ? (1ULL << (BITFIELD_BPW - 1)) : 0);
	}
	return 0;
}

#endif	/* BITFIELD_H_ */
