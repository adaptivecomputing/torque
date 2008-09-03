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
#ifndef HPM_H_
#define HPM_H_

/*
 * HPM stands for Harware Performance Monitor.  This is an attribute that
 * allows users to take advantage of such software as "perfex". SGI
 * Origin2000's only allow one global counter at a time, so when the system
 * is using it users are unable to and vice versa.
 *
 * This functionality is implemented by having the PBS Resource Monitor
 * execute the script with the given argument.  Consult the script for
 * further information.
 *
 * HPM_ATTRIBUTE is the name of the PBS resource attribute that jobs use to
 * specify that they want to use the HPM counters (i.e. to run `perfex(1)')
 *
 * HPM_CTL_FORMAT_STR is a 'sprintf(3)' template for the addreq() resmom
 * call.  The '%s' should be replaced with one of the following strings.
 * The call wi
 *
 * HPM_CTL_USER_STR is the string used by the script to indicate that the
 * system is in user mode.  The command 'hpm_ctl $HPM_CTL_USERMODE', if run
 * by a privileged user, should set up the system for user mode.
 *
 * HPM_CTL_GLOBAL_STR is the string used by the script to indicate that the
 * system is in user mode.  The command 'hpm_ctl $HPM_CTL_GLOBALMODE', if
 * run by a privileged user, should set up the system for system mode.
 *
 * HPM_CTL_QUERY_STR is the string to pass to the script to get the current
 * state of the execution host.
 *
 * HPM_CTL_OKAY_STR is returned by hpm_ctl if it successfully set the system
 * to the requested mode.
 *
 * HPM_CTL_ERROR_STR is the string prefixing an error message from hpm_ctl.
 */

#define HPM_ATTRIBUTE  "hpm"

#define HPM_CTL_FORMAT_STR "hpm_ctl[mode=%s]"
#define HPM_CTL_QUERY_STR "query"
#define HPM_CTL_USERMODE_STR "user"
#define HPM_CTL_GLOBALMODE_STR "global"
#define HPM_CTL_REVOKE_STR "revoke"

#define HPM_CTL_OKAY_STR "OKAY"
#define HPM_CTL_ERROR_STR "ERROR"

/* Legal modes to pass into schd_hpm_setup()  */
#define HPM_SETUP_USERMODE 1
#define HPM_SETUP_GLOBALMODE 2
#define HPM_SETUP_REVOKE 3

#endif /* HPM_H_ */
