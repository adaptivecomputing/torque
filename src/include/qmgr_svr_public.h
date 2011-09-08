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
 * This is a list of public server attributes
 *
 * FORMAT:
 *  attr1,
 *   attr2, <--- important the last has a comma after it
 *
 *  This file will be used for the initialization of an array
 *
 */

/* sync w/SRV_ATR_* in server.h, server/svr_attr_def.c, and ATTR_* in pbs_ifl.h  */

ATTR_aclhten,
ATTR_aclhost,
ATTR_acluren,
ATTR_acluser,
ATTR_aclroot,
ATTR_comment,
ATTR_defnode,
ATTR_dfltque,
ATTR_locsvrs,
ATTR_logevents,
ATTR_loglevel,
ATTR_managers,
ATTR_mailfrom,
ATTR_maxrun,
ATTR_maxuserrun,
ATTR_maxgrprun,
ATTR_nodepack,
ATTR_nodesuffix,
ATTR_operators,
ATTR_queryother,
ATTR_rescavail,
ATTR_resccost,
ATTR_rescdflt,
ATTR_rescmax,
ATTR_schedit,
ATTR_scheduling,
ATTR_syscost,
ATTR_pingrate,
ATTR_ndchkrate,
ATTR_tcptimeout,
ATTR_jobstatrate,
ATTR_polljobs,
ATTR_downonerror,
ATTR_disableserveridcheck,
ATTR_jobnanny,
ATTR_ownerpurge,
ATTR_qcqlimits,
ATTR_momjobsync,
ATTR_maildomain,
ATTR_killdelay,
ATTR_acllogic,
ATTR_aclgrpslpy,
ATTR_keepcompleted,
ATTR_submithosts,
ATTR_allownodesubmit,
ATTR_allowproxyuser,
ATTR_servername,
ATTR_autonodenp,
ATTR_logfilemaxsize,
ATTR_logfilerolldepth,
ATTR_logkeepdays,
ATTR_nextjobnum,
ATTR_tokens,
ATTR_extraresc,
ATTR_schedversion,
ATTR_acctkeepdays,
ATTR_lockfile,
ATTR_LockfileUpdateTime,
ATTR_LockfileCheckTime,
ATTR_credentiallifetime,
ATTR_jobmustreport,
ATTR_checkpoint_dir,
ATTR_dispsvrsuffix,
ATTR_jobsuffixalias,
ATTR_mailsubjectfmt,
ATTR_mailbodyfmt,
ATTR_npdefault,
ATTR_clonebatchsize,
ATTR_clonebatchdelay,
ATTR_jobstarttimeout,
ATTR_jobforcecanceltime,
ATTR_maxarraysize,
ATTR_maxslotlimit,
ATTR_recordjobinfo,
ATTR_recordjobscript,
ATTR_joblogfilemaxsize,
ATTR_joblogfilerolldepth,
ATTR_joblogkeepdays,
#ifdef MUNGE_AUTH
ATTR_authusers,
#endif
ATTR_moabarraycompatible,
ATTR_nomailforce,
