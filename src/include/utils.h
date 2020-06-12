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
#include <signal.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>
#include <time.h>
#include <limits.h>
#include <sys/types.h>
#include <sys/param.h>
#include <sys/select.h>

#include "portability.h"
#include "server_limits.h"
#include "list_link.h"
#include "pbs_nodes.h"
#include "libpbs.h"
#include "pbs_ifl.h"
#include "pbs_error.h"
#include "log.h"
#include "mcom.h"

#define FDMOVE(fd) if (fd < 3) { \
      int hold = fcntl(fd,F_DUPFD,3); \
      close(fd); \
      fd = hold; \
      }


#define MAXLINE 1024
#ifndef NULL
#define NULL 0
#endif
#ifndef TRUE
#define TRUE 1
#define FALSE 0
#endif

#define FATAL_ERROR    -2
#define NONFATAL_ERROR -1

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

const unsigned long NANO_SECONDS = 1000000000;

extern char **ArgV;
extern int ArgC;
extern char *OriginalPath;

/* Function declarations */

/* group functions in u_groups.c */
extern void free_grname(struct group *, char *);
extern void free_pwnam(struct passwd *pwdp, char *buf);
extern bool is_group_member(char *user_name, char *group_name);
extern struct group *getgrnam_ext (char **, char *);
extern struct group *getgrgid_ext (char **, gid_t);

/* user functions in u_users.c */
extern struct passwd *getpwnam_ext (char **, char *);
struct passwd *get_password_entry_by_uid(char ** user_buf, uid_t uid);
int                   setuid_ext(uid_t uid, int set_euid);
int                   initgroups_ext(const char *username, gid_t gr_id);

/* tree functions in u_tree.c */
extern void tinsert (const u_long, struct pbsnode *, tree **);
extern void *tdelete (const u_long, tree **);
extern struct pbsnode *tfind (const u_long, tree **);
extern int tlist (tree *, char *, int);
extern void tfree (tree **);
extern int is_whitespace (char);
void       move_past_whitespace(char **);
extern int write_buffer (char *,int,int);

/* misc functions */
extern void save_args(int, char **);
extern char *find_command(char *, char *);
void         translate_vector_to_range_string(std::string &range_string, const std::vector<int> &indices);
int          translate_range_string_to_vector(const char *range_str, std::vector<int> &indices);
void         capture_until_close_character(char **start, std::string &storage, char end);
bool         task_hosts_match(const char *, const char *);

/* utility functions in u_mu.c */
int           MUSNPrintF (char **, int *, const char *, ...);
int           MUStrNCat (char **, int *, const char *);
int           MUSleep (long);
int           MUReadPipe (char *, char *, int);
int           is_whitespace (char);
char         *trim(char *);
char         *threadsafe_tokenizer(char **str, const char  *delims);
int           safe_strncat(char *, const char *, size_t);
unsigned int  get_random_number();

/* MXML functions from u_MXML.c */
extern int MXMLGetChild (mxml_t *, char *, int *, mxml_t **);
extern int MXMLAddE (mxml_t *, mxml_t *);
extern int MXMLGetAttrF (mxml_t *, char *, int *, void *, enum MDataFormatEnum, int);
extern int MXMLGetAttr (mxml_t *, char *, int *, char *, int);
extern int MXMLToString (mxml_t *, char *, int, char **, mbool_t);
int        MXMLFromString(mxml_t **EP, char *XMLString, char **Tail, char *EMsg, int emsg_size);

/* functions from u_xml.c */
int get_parent_and_child(char *,char **,char **,char **);
int escape_xml(char *,char *,int);
int unescape_xml(char *,char *,int);

/* functions from u_xml.c */
int get_parent_and_child(char *,char **,char **,char **);
int escape_xml(char *,char *,int);
int unescape_xml(char *,char *,int);

/* functions from u_putenv.c */
int put_env_var(const char *, const char *);

// from u_wrapper.c
int rmdir_ext(const char *dir, int retry_limit = 20);
int unlink_ext(const char *filename, int retry_limit = 20);
int mkdir_wrapper(const char *pathname, mode_t mode);

/* from parse_config.c */
int setbool(const char *value);

#endif /* END #ifndef UTILS_H */
 
