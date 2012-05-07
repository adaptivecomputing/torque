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
 * encode_DIS_svrattrl() - encode a list of server "svrattrl" structures
 *
 * The first item encoded is a unsigned integer, a count of the
 * number of svrattrl entries in the linked list.  This is encoded
 * even when there are no svrattrl entries in the list.
 *
 * Each individual entry is then encoded as:
 *  u int size of the three strings (name, resource, value)
 *   including the terminating nulls
 *  string attribute name
 *  u int 1 or 0 if resource name does or does not follow
 *  string resource name (if one)
 *  string  value of attribute/resource
 *  u int "op" of attrlop
 *
 * Note, the encoding of a svrattrl is the same as the encoding of
 * the pbs_ifl.h structures "attrl" and "attrlop".  Any one of the
 * three forms can be decoded into any of the three with the possible
 * loss of the "flags" field (which is the "op" of the attrlop).
 */

#include <pbs_config.h>   /* the master config generated by configure */

#include "libpbs.h"
#include "list_link.h"
#include "attribute.h"
#include "dis.h"


int encode_DIS_svrattrl(
    
  struct tcp_chan *chan,
  svrattrl *psattl)

  {
  unsigned int ct = 0;
  unsigned int name_len;
  svrattrl *ps;
  int rc;

  /* count how many */

  for (ps = psattl; ps; ps = (svrattrl *)GET_NEXT(ps->al_link))
    {
    ++ct;
    }

  if ((rc = diswui(chan, ct)))
    return rc;

  for (ps = psattl; ps; ps = (svrattrl *)GET_NEXT(ps->al_link))
    {
    /* length of three strings */
    name_len = (int)strlen(ps->al_atopl.name) +
               (int)strlen(ps->al_atopl.value) + 2;

    if (ps->al_atopl.resource)
      name_len += strlen(ps->al_atopl.resource) + 1;

    if ((rc = diswui(chan, name_len)))
      break;

    if ((rc = diswst(chan, ps->al_atopl.name)))
      break;

    if (ps->al_rescln)   /* has a resource name */
      {
      if ((rc = diswui(chan, 1)))
        break;

      if ((rc = diswst(chan, ps->al_atopl.resource)))
        break;
      }
    else
      {
      if ((rc = diswui(chan, 0))) /* no resource name */
        break;
      }

    if ((rc = diswst(chan, ps->al_atopl.value)) ||
        (rc = diswui(chan, (unsigned int)ps->al_op)))
      break;
    }

  return rc;
  }
