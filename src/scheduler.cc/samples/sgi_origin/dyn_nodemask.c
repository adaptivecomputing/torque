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

#ifdef NODEMASK

#include <sys/types.h>
#include <sys/sysmp.h>
#include <errno.h>
#include <invent.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdlib.h>
#include <string.h>

#include "pbs_ifl.h"
#include "log.h"
#include "toolkit.h"
#include "gblxvars.h"

#include <ctype.h>

/*
 * Uncomment the #define below to enable the "naive" node allocation
 * algorithm.
 */
/* #define SIMPLE */

#ifndef SIMPLE
#define CONTIG
#endif /* ! SIMPLE */

#ifdef DEBUG
extern int schd_FAKE_MACH_MULT;
#endif /* DEBUG */

/*
 * mask_mask is the mask of bits that are in this particular part of the
 * nodemask.  ndmask is the actual nodemask in question.
 *
 * For instance, a mask_mask of: 00000000111111111111111100000000
 * and a nodemask of :   00000000000000111111111100000000
 * generate the output:   ........------XXXXXXXXXX........
 *
 * Note that the storage is static - copy the return before the next call!
 */
char *
schd_format_nodemask(Bitfield *mask_mask, Bitfield *ndmask)
  {
  static char prt_bits[BITFIELD_SIZE + 2];
  Bitfield bit;
  int    idx;

  /* Find the most significant bit in the nodemask. */
  BITFIELD_CLRALL(&bit);
  BITFIELD_SETB(&bit, (BITFIELD_SIZE - 1));

  /* Walk the bits in the nodemask, from MSB to LSB. */

  for (idx = 0; idx < BITFIELD_SIZE; idx ++)
    {
    if (BITFIELD_TSTM(&bit, mask_mask))
      prt_bits[idx] = (BITFIELD_TSTM(ndmask, &bit)) ? 'X' : '-';
    else
      prt_bits[idx] = '.';

    BITFIELD_SHIFTR(&bit);
    }

  /* Null terminate the resulting string. */
  prt_bits[idx] = '\0';

  return (prt_bits);
  }

int
schd_bits2mask(char *string, Bitfield *mask)
  {
  int    i, bit, len;
  Bitfield new_bit;

  BITFIELD_CLRALL(&new_bit);

  len = (int)strlen(string);

  /* Start with the low bit. */
  bit = 0;

  for (i = 0; i < len; i++)
    {
    if (string[len - i - 1] == '1')
      BITFIELD_SETB(&new_bit, bit);
    else if (string[len - i - 1] != '0')
      return (-1);

    ++ bit;
    }

  /* Done.  Copy the new nodemask and return success. */

  BITFIELD_CPY(mask, &new_bit);

  return (0);
  }

#ifdef SIMPLE
int schd_alloc_nodes(int nnodes, Queue *queue, Bitfield *job_mask)
  {
  Bitfield *queue_mask;
  Bitfield *q_avail_mask;
  Bitfield new_bit;
  int candidate;
  int n_found;

  n_found = 0;
  BITFIELD_CLRALL(&new_bit);

  queue_mask   = &(queue->queuemask);
  q_avail_mask = &(queue->availmask);

  candidate = (BITFIELD_SIZE - 1);

  while ((n_found < nnodes) && (candidate >= 0))
    {
    if (BITFIELD_TSTB(queue_mask, candidate))
      {
      if (BITFIELD_TSTB(q_avail_mask, candidate))
        {
        /* available */
        BITFIELD_SETB(&new_bit, candidate);
        ++n_found;
        }
      }

    -- candidate;
    }

  /*
   * If enough nodes were allocated, copy the resultant new nodemask
   * into the supplied area.
   */
  if (nnodes == n_found)
    {
    BITFIELD_CPY(job_mask, &new_bit);
    return nnodes;
    }

  /* Didn't get enough nodes.  Return 0 as an error. */
  return 0;
  }

#endif /* SIMPLE */

#ifdef CONTIG
int schd_alloc_nodes(int request, Queue *queue, Bitfield *maskp)
  {
  char   *id = "schd_alloc_nodes";
  Bitfield avail;
  Bitfield mask;
  Bitfield contig;
  int remain;
  int qmsb;
  int qlsb;
  int i, n;
  int count;
  int found;

  /* Make certain the nodecount request can be fulfilled. */

  if (request <= 0 || request > BITFIELD_NUM_ONES(&(queue->availmask)))
    return 0;

  /*
   * Make a copy of the queue's available bit mask to play with, and clear
   * the allocated nodes mask.
   */
  BITFIELD_CPY(&avail, &(queue->availmask));

  BITFIELD_CLRALL(&mask);

  /* How many have been found, and how many remain. */
  found  = 0;

  remain = request;

  while (remain > 0)
    {
    /*
     * Find first and last available bit positions in the
     * queue's available node mask.
     */
    qmsb = BITFIELD_MS_ONE(&avail);
    qlsb = BITFIELD_LS_ONE(&avail);

    /*
     * Starting with the size of the remaining nodes needed to satisfy
     * this request, look for a set of 'n' contiguous bits in the
     * available node mask.  If that is not found, try the next smallest
     * contiguous vector, etc.
     */

    for (n = remain; n > 0; n--)
      {
      /*
       * Create a contiguous bitmask of 'n' bits, starting at the
       * position of the highest bit in the avail mask.
       */
      BITFIELD_CLRALL(&contig);

      for (i = 0; i < n; i++)
        BITFIELD_SETB(&contig, qmsb - i);

      /*
       * Calculate how many times this contiguous bitmask needs to be
       * shifted to the right to cover every set of 'n' bits between
       * the qmsb and qlsb, inclusive.  Count the initial configuration
       * as well (the trailing '+ 1').
       */
      count = (qmsb + 1 - qlsb) - n + 1;

      /*
       * Shift the contiguous mask right one bit at a time, checking
       * if all the bits in the mask are set in the available mask.
       */
      for (i = 0; i < count; i++)
        {

        /* Are all bits in contig also set in the avail mask? */
        if (BITFIELD_TSTALLM(&avail, &contig))
          {
          break;
          }

        BITFIELD_SHIFTR(&contig);
        }

      /*
       * If the contiguous bits are available, add them to the new job
       * nodemask, and remove them from the avail mask.  Adjust the
       * remaining node count, and start the next hunt for the remaining
       * nodes.
       */
      if (i < count)
        {
        BITFIELD_SETM(&mask,  &contig);
        BITFIELD_CLRM(&avail, &contig);

        found  += n;
        remain -= n;

        break; /* for(n) loop */
        }
      }

    /* Check for something going wrong. */
    if (n == 0)
      {
      DBPRT(("%s: couldn't find any contiguous bits (even one!)\n", id));
      break; /* while(remain) loop */
      }
    }

  /*
   * If no bits remain to be allocated, copy the new mask into the provided
   * space, and return the number of bits requested.
   */
  if (!remain && (found == request))
    {
    BITFIELD_CPY(maskp, &mask);
    DBPRT(("%s: mask %s\n", id,
           schd_format_nodemask(&queue->queuemask, maskp)));
    return found;
    }

  return 0;
  }

#endif /* CONTIG */

int
schd_str2mask(char *maskstr, Bitfield *maskp)
  {
  char *id = "str2mask";
  Bitfield nodemask;
  char hex;
  int ndbit = 0, len, dec, hxbit;

  unsigned long long compat;
  char *ptr;
  char     buf[32];

  if (maskstr == NULL)
    return (-1);

  /* Nodemask string must be in the format '0x...'. */
  if ((maskstr[0] != '0') || (tolower(maskstr[1]) != 'x'))
    {

    /* XXX
     * For backwards compatability, if the string will convert
     * into an unsigned long long, then assume this is an old-
     * style decimal nodemask (on 8-128P machines, nodemask is
     * an unsigned long long, and was treated as such).
     *
     * Note that this assumes that sizeof(unsigned long long)
     * is the same as sizeof(unsigned long), which is only the
     * case with the 64-bit SGI compiler ABI.
     */
    compat = strtoul(maskstr, &ptr, 10);

    if (*ptr != '\0')
      return (-1);

    /*
     * String converted to an unsigned long long.  Print it as
     * a hex back into a buffer, and point at that buffer instead.
     * This will cause the newly created hex string to be used.
     */
    (void)sprintf(buf, "0x%llx", compat);

    maskstr = buf;

    (void)sprintf(log_buffer,
                  "converted old-style nodemask %lu to %s", compat, maskstr);

    log_record(PBSEVENT_SYSTEM, PBS_EVENTCLASS_SERVER, id,
               log_buffer);
    }

  /* Zero out the new nodemask struct, and fill it from the maskstr. */
  BITFIELD_CLRALL(&nodemask);

  /* Walk backwards through the hex digits in the string 0x... */
  for (len = (int)strlen(maskstr); len > 2 /* 0x... */; len --)
    {
    hex = maskstr[len - 1];
    /* Make sure it's a hex digit. */

    if (!isxdigit((int)hex))
      return (-1);

    /* If this digit is a '0', it contributes nothing. */
    if (hex != '0')
      {
      /* Convert the hex digit to a decimal int. */
      dec = isdigit((int)hex) ? (hex - '0') :
            (tolower((int)hex) - 'a' + 10);

      for (hxbit = 0; hxbit < 4; hxbit ++)
        {
        if (dec & (1 << hxbit))
          BITFIELD_SETB(&nodemask, ndbit);

        ndbit ++;
        }
      }
    else
      ndbit += 4;

    /* Make sure we haven't walked past the end of the nodemask. */
    if (ndbit > (sizeof(Bitfield) * 8))
      return (-1);
    }

  /* Copy the newly created nodemask into the handed-in one. */
  BITFIELD_CPY(maskp, &nodemask);

  return (0);
  }

int
schd_mask2str(char *maskstr, Bitfield *maskp)
  {
  /* char *id = "mask2str"; */
  Bitfield nodemask;
  int bit, x = 0, i, m;

  if (maskstr == NULL || maskp == NULL)
    return (-1);

  nodemask = *maskp;

  i = 0; /* 'i' is the current index into maskstr[]. */

  /* Preface the string with '0x' ... */
  maskstr[i++] = '0';

  maskstr[i++] = 'x';

  m = 3; /* The bit position in this nibble (bit % 4). */

  for (bit = (sizeof(Bitfield) * 8) - 1; bit >= 0; bit --)
    {
    if (BITFIELD_TSTB(&nodemask, bit))
      x |= (1 << m);

    if (m-- == 0)
      {
      maskstr[i++] = (x < 10) ? (x + '0') : (x + 'a' - 10);
      x = 0;
      m = 3;
      }
    }

  maskstr[i] = '\0';

  return (0);
  }

#endif /* NODEMASK */
