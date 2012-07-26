#ifndef _QSUB_FUNCTIONS_H
#define _QSUB_FUNCTIONS_H


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




#include <pbs_config.h>
#include <stdio.h>
#include <termios.h>
#include <sys/types.h>
#include "u_hash_map_structs.h"
#include "u_memmgr.h"
#ifdef HAVE_SYS_IOCTL_H
#include <sys/ioctl.h>
#endif /* HAVE_SYS_IOCTL_H */

#ifdef TEST_FUNCTION
#define PBS_SERVER_HOME "./pbshome"
#define SUBMIT_FILTER_PATH "./submitFilter"
#endif /* TEST_FUNCTION */

typedef struct job_info
  {
  memmgr *mm;
  job_data *job_attr;
  job_data *res_attr;
  job_data *user_attr;
  job_data *client_attr;
  } job_info;

char *x11_get_proto(
    char *xauth_path, /* I */
    int debug);       /* I */

char *smart_strtok(
    char  *line,          /* I */
    char  *delims,        /* I */
    char **ptrPtr,        /* O */
    int    ign_backslash);/* I */

int get_name_value(
    char  *start,
    char **name,
    char **value);

/*char *set_dir_prefix(
    char *prefix,
    int   diropt);
    */

int isexecutable(
    char *s);             /* I */

char *ispbsdir(
    char *s,
    char *prefix);

int istext(
    FILE   *fd,           /* I */
    int    *IsText);      /* O (optional) */

int validate_submit_filter(memmgr **mm, job_data **a_hash);
void validate_pbs_o_workdir(memmgr **mm, job_data **job_attr);
void validate_qsub_host_pbs_o_server(memmgr **mm, job_data **job_attr);
void post_check_attributes(job_info *ji);

/* return 3, 4, 5, 6, -1 on FAILURE, 0 on success */
int get_script(
    int    ArgC,            /* I */
    char **ArgV,            /* I */
    FILE  *file,            /* I */
    char  *script,          /* O (minsize=X) */
    job_info *ji);          /* M */

void make_argv(
    int  *argc,
    char *argv[],
    char *line);

void do_dir(
    char *opts,
    job_info *ji,
    int data_type);

int set_job_env(
    char **envp);         /* I */

char *interactive_port(
    int *sock);

void settermraw(
    struct termios *ptio);

void stopme(
    pid_t p);             /* pid of 0 (process group) or just myself (writer) */

int reader(
    int s,                /* I - reading socket */
    int d);               /* I - writing socket */

void writer(
    int s,                /* writing socket */
    int d);               /* reader socket */

int getwinsize(
    struct winsize *wsz);

void send_winsize(
    int sock,
    struct winsize *wsz);

void send_term(
    int sock);

void catchchild(
    int sig);

void no_suspend(
    int sig);

void bailout(void);

void toolong(
    int sig);

void catchint(
    int sig);

void x11handler(
    memmgr **mm,
    int inter_sock);

void interactive(memmgr **mm, job_data *client_attr);

int validate_group_list(
    char *glist);

void process_opts(
    int    argc,                  /* I */
    char **argv,                  /* I */
    job_info *ji,                 /* I */
    int data_type);               /* O */

void set_job_defaults(job_info *ji);

int load_config(
  char *config_buf,               /* O */
  int   BufSize);                 /* I */

char *get_param(
    char *param,                  /* I */
    char *config_buf);            /* I */

void set_client_attr_defaults(
    memmgr **mm,                  /* M */
    job_data **client_entry);     /* M */

void update_job_env_names(job_info *ji);

void process_config_file(
    job_info *ji);                /* M */

void print_qsub_usage_exit(
    char *error_msg);             /* I */

void add_submit_args_to_job(memmgr **mm, job_data **job_attr, int argc, char **argv);

void main_func(
    int    argc,                  /* I */
    char **argv,                  /* I */
    char **envp);                 /* I */

#endif /* _QSUB_FUNCTIONS_H */
