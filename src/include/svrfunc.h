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

extern int   chk_hold_priv A_((long hold, int priv));
extern void  get_jobowner A_((char *from, char *to));
extern char *parse_servername A_((char *, unsigned int *));
extern void  process_Areply A_((int));
extern void  process_Dreply A_((int));
extern void  process_request A_((int));
extern void  process_dis_request A_((int));
extern int   save_flush();
extern void  save_setup A_((int));
extern int   save_struct A_((char *, unsigned int));
extern void  schedule_job A_((void));
extern char *site_map_user A_((char *, char *));
extern int   socket_to_handle A_((int));
extern void  svr_disconnect A_((int));
extern int   svr_get_privilege A_((char *, char *));
extern int   srv_shutdown A_((int));
extern void  write_node_state A_((void ));
extern int   setup_nodes A_((void));
extern int   node_avail A_((char *spec, int  *navail,
			    int *nalloc, int *nreserved, int *ndown));
extern void  node_unreserve A_((resource_t handle));
extern int   node_reserve A_((char *speclist, resource_t tag));

#ifdef JOB_H
extern int   set_nodes A_((job *,char *,char **));
extern int   is_ts_node A_((char *));
extern char *find_ts_node A_((void));
extern void  free_nodes A_((job *));
#endif	/* JOB_H */

#ifdef ATTRIBUTE_H
extern int   check_que_enable A_((attribute *,void *,int));
extern int   set_queue_type A_((attribute *,void *,int));
#ifdef QUEUE_H
extern int   chk_resc_limits A_((attribute *,pbs_queue *,char *));
#endif	/* QUEUE_H */
#endif	/* ATTRIBUTE_H */

#ifdef PBS_NET_H
extern int   svr_connect A_((pbs_net_t, unsigned int, void (*)(int), enum conn_type));
#endif	/* PBS_NET_H */

#ifdef WORK_TASK_H
extern void  release_req A_((struct work_task *));
#ifdef BATCH_REQUEST_H
extern int   issue_Drequest A_((int, struct batch_request *, void (*)(), struct work_task **));
#endif	/* BATCH_REQUEST_H */
#endif	/* WORK_TASK_H */


/* The following is used in req_stat.c and req_select.c */

#ifdef STAT_CNTL

struct select_list {
	struct select_list *sl_next;	/* ptr to next in list   */
	enum batch_op	    sl_op;	/* comparison operator   */
	attribute_def      *sl_def;	/* ptr to attr definition,for at_comp */
	int		    sl_atindx;	/* index into attribute_def, for type */
	attribute	    sl_attr;	/* the attribute (value) */
};

struct stat_cntl {		/* used in req_stat_job */
	int		      sc_XXXX;
	int		      sc_type;
	int		      sc_XXXY;
	int		      sc_conn;
	pbs_queue	     *sc_pque;
	struct batch_request *sc_origrq;
	struct select_list   *sc_select;
	void		    (*sc_post) A_((struct stat_cntl *));
	char		      sc_jobid[PBS_MAXSVRJOBID+1];
};

extern int stat_to_mom A_((job *, struct stat_cntl *));

/*
 * the following defines are due to the fact that O_SYNC was not
 * POSIX standard (TDISABLEFILESYNC managed w/configure --disable-filesync) 
 */

#endif	/* STAT_CNTL */

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

