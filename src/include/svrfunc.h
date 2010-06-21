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
 * misc server function prototypes
 */

extern int   chk_hold_priv(long hold, int priv);
extern void  get_jobowner(char *from, char *to);
extern char *parse_servername(char *, unsigned int *);
extern void  process_Areply(int);
extern void  process_Dreply(int);
extern void  process_request(int);
extern void  process_dis_request(int);
extern int   save_flush();
extern void  save_setup(int);
extern int   save_struct(char *, unsigned int);
extern void  schedule_job(void);
extern char *site_map_user(char *, char *);
extern int   socket_to_handle(int);
extern void  svr_disconnect(int);
extern int   svr_get_privilege(char *, char *);
extern int   srv_shutdown(int);
extern void  write_node_state(void);
extern int  write_node_note(void);
extern int   setup_nodes(void);
extern int   node_avail(char *spec, int  *navail,
                              int *nalloc, int *nreserved, int *ndown);
extern void  node_unreserve(resource_t handle);
extern int   node_reserve(char *speclist, resource_t tag);
extern int   init_resc_defs(void);

#ifdef PBS_JOB_H
extern int   set_nodes(job *, char *, int, char **, char *, char *);
extern int   is_ts_node(char *);
extern char *find_ts_node(void);
extern void  free_nodes(job *);
#endif /* PBS_JOB_H */

#ifdef ATTRIBUTE_H
extern int   check_que_enable(attribute *, void *, int);
extern int   set_queue_type(attribute *, void *, int);
#ifdef QUEUE_H
extern int   chk_resc_limits(attribute *, pbs_queue *, char *);
#endif /* QUEUE_H */
#endif /* ATTRIBUTE_H */

#ifdef PBS_NET_H
extern int   svr_connect(pbs_net_t, unsigned int, void (*)(int), enum conn_type);
#endif /* PBS_NET_H */

#ifdef WORK_TASK_H
extern void  release_req(struct work_task *);
#ifdef BATCH_REQUEST_H
extern int   issue_Drequest(int, struct batch_request *, void (*)(), struct work_task **);
#endif /* BATCH_REQUEST_H */
#endif /* WORK_TASK_H */


/* The following is used in req_stat.c and req_select.c */

#ifdef STAT_CNTL

struct select_list
  {

  struct select_list *sl_next; /* ptr to next in list   */
  enum batch_op     sl_op; /* comparison operator   */
  attribute_def      *sl_def; /* ptr to attr definition,for at_comp */
  int      sl_atindx; /* index into attribute_def, for type */
  attribute     sl_attr; /* the attribute (value) */
  };

struct stat_cntl    /* used in req_stat_job */
  {
  int        sc_XXXX;
  int        sc_type;
  int        sc_XXXY;
  int        sc_conn;
  pbs_queue      *sc_pque;

  struct batch_request *sc_origrq;

  struct select_list   *sc_select;
  void (*sc_post)(struct stat_cntl *);
  char        sc_jobid[PBS_MAXSVRJOBID+1];
  };

extern int stat_to_mom(job *, struct stat_cntl *);

#endif /* STAT_CNTL */

/*
 * the following defines are due to the fact that O_SYNC was not
 * POSIX standard (TDISABLEFILESYNC managed w/configure --enable-filesync)
 */

#if TDISABLEFILESYNC
#define O_Sync 0
#else
#ifdef O_SYNC
#define O_Sync O_SYNC
#elif _FSYNC
#define O_Sync _FSYNC
#else
#define O_Sync 0
#endif /* O_SYNC */
#endif /* TDISABLEFILESYNC */

