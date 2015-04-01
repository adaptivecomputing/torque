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
 * decode_DIS_svrattrl() - decode into a list of server "svrattrl" structures
 *
 * The space for the svrattrl structures is allocated as needed.
 *
 * The first item is a unsigned integer, a count of the
 * number of svrattrl entries in the linked list.  This is encoded
 * even when there are no entries in the list.
 *
 * Each individual entry is encoded as:
 *  u int size of the three strings (name, resource, value)
 *   including the terminating nulls
 *  string attribute name
 *  u int 1 or 0 if resource name does or does not follow
 *  string resource name (if one)
 *  string  value of attribute/resource
 *  u int "op" of attrlop
 *
 * Note, the encoding of a svrattrl is the same as the encoding of
 * the pbs_ifl.h structures "attrl" and "attropl".  Any one of
 * the three forms can be decoded into any of the three with the
 * possible loss of the "flags" field (which is the "op" of the attrlop).
 */

#include <pbs_config.h>   /* the master config generated by configure */

#include <sys/types.h>
#include <stdlib.h>
#include <string.h>
#include "libpbs.h"
#include "list_link.h"
#include "attribute.h"
#include "dis.h"

int decode_DIS_svrattrl(
    
  struct tcp_chan *chan,
  tlist_head      *phead)

  {
  unsigned int  i;
  unsigned int  hasresc;
  size_t        ls;
  unsigned int  data_len;
  unsigned int  numattr;
  svrattrl     *psvrat = NULL;
  int           rc;
  size_t        tsize;


  numattr = disrui(chan, &rc); /* number of attributes in set */

  if (rc) return rc;

  for (i = 0; i < numattr; ++i)
    {
    data_len = disrui(chan, &rc); /* here it is used */

    if (data_len == 0)
      data_len = sizeof(char);

    if (rc)
      return(rc);

    tsize = sizeof(svrattrl) + data_len + 2; /*add 2 for nulls name & resc*/

    if ((psvrat = (svrattrl *)calloc(1, tsize)) == 0)
      return DIS_NOMALLOC;

    CLEAR_LINK(psvrat->al_link);

    psvrat->al_atopl.next = 0;

    psvrat->al_tsize = tsize;

    psvrat->al_name  = (char *)psvrat + sizeof(svrattrl);

    psvrat->al_resc  = 0;

    psvrat->al_value = 0;

    psvrat->al_nameln = 0;

    psvrat->al_rescln = 0;

    psvrat->al_valln  = 0;

    psvrat->al_flags  = 0;

    if ((rc = disrfcs(chan, &ls, data_len, psvrat->al_name)))
      break;

    *(psvrat->al_name + ls++) = '\0';

    psvrat->al_nameln = (int)ls;

    data_len -= ls;

    hasresc = disrui(chan, &rc);

    if (rc) 
	  break;

    if (hasresc)
      {
      psvrat->al_resc = psvrat->al_name + ls;

      if ((rc = disrfcs(chan, &ls, data_len, psvrat->al_resc)))
        break;

      *(psvrat->al_resc + ls++) = '\0';

      psvrat->al_rescln = (int)ls;

      data_len -= ls;
      }

    psvrat->al_value  = psvrat->al_name + psvrat->al_nameln +

                        psvrat->al_rescln;

    if ((rc = disrfcs(chan, &ls, data_len, psvrat->al_value)))
      break;

    *(psvrat->al_value + ls++) = '\0';

    psvrat->al_valln = (int)ls;

    psvrat->al_op = (enum batch_op)disrui(chan, &rc);

    if (rc) break;

    append_link(phead, &psvrat->al_link, psvrat);
    }

  if (rc)
    {
    (void)free(psvrat);
    }

  return (rc);
  }



