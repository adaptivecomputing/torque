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

#ifndef UTILS_H
#define UTILS_H

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <pwd.h>
#include <signal.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>
#include <fcntl.h>
#include <time.h>
#include <limits.h>
#include <netdb.h>
#include <grp.h>
#include <sys/types.h>
#include <sys/param.h>
#include <sys/times.h>
#include <sys/stat.h>
#include <sys/select.h>

#include "portability.h"
#include "server_limits.h"
#include "list_link.h"
#include "attribute.h"
#include "pbs_nodes.h"
#include "libpbs.h"
#include "pbs_ifl.h"
#include "resource.h"
#include "svrfunc.h"
#include "pbs_error.h"
#include "log.h"
#include "mcom.h"

#ifndef MAXLINE 
#define MAXLINE 1024
#endif
#ifndef NULL
#define NULL 0
#endif

#define BUFFER_OVERFLOW -5
#define LT_ESCAPED       "&lt;"
#define LT_ESCAPED_LEN   4
#define GT_ESCAPED       "&gt;"
#define GT_ESCAPED_LEN   4
#define AMP_ESCAPED      "&amp;"
#define AMP_ESCAPED_LEN  5
#define QUOT_ESCAPED     "&quot;"
#define QUOT_ESCAPED_LEN 6
#define APOS_ESCAPED     "&apos;"
#define APOS_ESCAPED_LEN 6

/* Function declarations */

/* group functions in u_groups.c */
extern struct group *getgrnam_ext (char *);

/* user functions in u_users.c */
extern struct passwd *getpwnam_ext (char *);

/* tree functions in u_tree.c */
extern void tinsert (const u_long, struct pbsnode *, tree **);
extern void *tdelete (const u_long, tree **);
extern struct pbsnode *tfind (const u_long, tree **);
extern int tlist (tree *, char *, int);
extern void tfree (tree **);

/* moab-like utility functions in u_mu.c */
extern int MUSNPrintF (char **, int *, char *, ...);
extern int MUStrNCat (char **, int *, char *);
extern int MUSleep (long);
extern int MUReadPipe (char *, char *, int);
extern int is_whitespace (char);

/* MXML functions from u_MXML.c */
extern int MXMLGetChild (mxml_t *, char *, int *, mxml_t **);
extern int MXMLAddE (mxml_t *, mxml_t *);
extern int MXMLGetAttrF (mxml_t *, char *, int *, void *, enum MDataFormatEnum, int);
extern int MXMLGetAttr (mxml_t *, char *, int *, char *, int);
extern int MXMLToString (mxml_t *, char *, int, char **, mbool_t);
extern int MXMLFromString(mxml_t **EP, char *XMLString, char **Tail, char *EMsg, int emsg_size);

/* functions from u_xml.c */
int get_parent_and_child(char *,char **,char **,char **);
int escape_xml(char *,char *,int);
int unescape_xml(char *,char *,int);

#endif /* END #ifndef UTILS_H */
 
