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

#include <assert.h>
#include <limits.h>
#include <stdio.h>
#include <unistd.h>
#include <dirent.h>
#include <fcntl.h>
#include <errno.h>
#include <strings.h>
#include <pwd.h>
#include <mntent.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/procfs.h>
#include <sys/param.h>
#include <sys/stat.h>
#include <sys/quota.h>
#include <sys/statfs.h>
#include <sys/sysmacros.h>
#include <sys/sysmp.h>
#include <sys/resource.h>
#if MOM_CHECKPOINT == 1
#include <ckpt.h>
#endif	/* MOM_CHECKPOINT */
#if	NODEMASK != 0
#include <sys/pmo.h>
#include <sys/syssgi.h>
#include <sys/nodemask.h>
#endif	/* NODEMASK */
#include <arraysvcs.h>

#include "pbs_error.h"
#include "portability.h"
#include "list_link.h"
#include "server_limits.h"
#include "attribute.h"
#include "resource.h"
#include "job.h"
#include "log.h"
#include "mom_mach.h"
#include "resmon.h"
#include "../rm_dep.h"

/*
**	System dependent code to gather information for the resource
**	monitor for a Silicon Graphics (SGI) machine.
**
**	Resources known by this code:
**		cput		cpu time for a pid or session
**		mem		memory size for a pid or session in KB
**		sessions	list of sessions in the system
**		pids		list of pids in a session
**		nsessions	number of sessions in the system
**		nusers		number of users in the system
**		totmem		total memory size in KB
**		availmem	available memory size in KB
**		ncpus		number of cpus
**		physmem		physical memory size in KB
**		size		size of a file or filesystem in KB
**		idletime	seconds of idle time
**		loadave		current load average
**		quota		quota information (sizes in KB)
*/

static char ident[] = "@(#) sgi/$RCSfile$ $Revision$";

#define SGI_ZOMBIE_WRONG 1
#define COMPLEX_MEM_CALC 0

#ifndef TRUE
#define FALSE	0
#define TRUE	1
#endif	/* TRUE */

#if COMPLEX_MEM_CALC==1
static  char            procfs[] = "/proc";
static  char            procfmts[] = "/proc/%s";
#else
static  char            procfs[] = "/proc/pinfo";
static  char            procfmts[] = "/proc/pinfo/%s";
#endif /* COMPLEX_MEM_CALC */
static	DIR		*pdir = NULL;
static	int		pagesize;
static	int		kfd = -1;
static  time_t		sampletime;

#define	TBL_INC		100		/* initial proc table */
#define MAPNUM		512		/* max number of mem segs */

static	int		nproc = 0;
static	int		max_proc = 0;
struct	proc_info {
	prpsinfo_t	info;
#if COMPLEX_MEM_CALC==1
	int		map_num;
	prmap_sgi_t	*map;
#endif /* COMPLEX_MEM_CALC */
	ash_t		procash;
};
static	struct	proc_info	*proc_array = NULL;

extern	char	*ret_string;
time_t		wait_time = 10;
extern	char	extra_parm[];
extern	char	no_parm[];

extern	struct	pbs_err_to_txt	pbs_err_to_txt[];
extern	time_t			time_now;
extern	time_t			last_scan;

/*
** external functions and data
*/
extern	struct	config		*search A_((struct config *, char *));
extern	struct	rm_attribute	*momgetattr A_((char *));
extern	int			rm_errno;
extern	unsigned	int	reqnum;
extern	double	cputfactor;
extern	double	wallfactor;
extern  long    system_ncpus;
extern char	*loadave	A_((struct rm_attribute *attrib));
extern  int     ignwalltime;

/*
** local functions and data
*/
static char	*totmem		A_((struct rm_attribute *attrib));
static char	*availmem	A_((struct rm_attribute *attrib));
static char	*physmem	A_((struct rm_attribute *attrib));
static char	*ncpus		A_((struct rm_attribute *attrib));
static char	*quota		A_((struct rm_attribute *attrib));
#if	NODEMASK != 0
static char	*availmask	A_((struct rm_attribute *attrib));
#endif	/* NODEMASK */
extern char	*nullproc	A_((struct rm_attribute *attrib));

/*
** local resource array
*/
struct	config	dependent_config[] = {
	{ "totmem",	{totmem} },
	{ "availmem",	{availmem} },
	{ "physmem",	{physmem} },
	{ "ncpus",	{ncpus} },
	{ "loadave",	{loadave} },
	{ "quota",	{quota} },
#if	NODEMASK != 0
	{ "availmask",	{availmask} },
#endif	/* NODEMASK */
	{ NULL,		{nullproc} },
};
	
off_t	kern_addr[] = {
	-1,			/* KSYM_PHYS */
	-1,			/* KSYM_LOAD */
};

#define	KSYM_PHYS	0
#define	KSYM_LOAD	1

void
dep_initialize()
{
	int	 i;
	char	*id = "dep_initialize";
	static	char	mem[] = "/dev/kmem";

	pagesize = getpagesize();

	if ((pdir = opendir(procfs)) == NULL) {
		log_err(errno, id, "opendir");
		return;
	}

	kern_addr[KSYM_PHYS] = SEEKLIMIT & sysmp(MP_KERNADDR, MPKA_PHYSMEM);
	kern_addr[KSYM_LOAD] = SEEKLIMIT & sysmp(MP_KERNADDR, MPKA_AVENRUN);

	if ((kfd = open(mem, O_RDONLY)) == -1) {
		log_err(errno, id, mem);
		return;
	}

	/* insure /dev/kmem closed on exec */

	if ((i = fcntl(kfd,  F_GETFD)) == -1) {
		log_err(errno, id, "F_GETFD");
	}
	i |= FD_CLOEXEC;
	if (fcntl(kfd, F_SETFD, i) == -1) {
		log_err(errno, id, "F_SETFD");
	}

	return;
}

void
dep_cleanup()
{
	char	*id = "dep_cleanup";

	log_record(PBSEVENT_SYSTEM, 0, id, "dependent cleanup");
	if (pdir) {
		closedir(pdir);
		pdir = NULL;
	}
	if (kfd != -1)
		close(kfd);
}

/*
**	Don't need any periodic procsessing.
*/
void
end_proc()
{
	return;
}

/*
 * Internal size decoding routine.
 *
 *	Accepts a resource pointer and a pointer to the unsigned 64 bit integer
 *	to receive the decoded value.  It returns a PBS error code, and the
 *	decoded value in the unsigned long integer.
 *
 *		sizeof(word) = sizeof(int)
 */

static int getsize(pres, ret)
    resource		*pres;
    rlim64_t		*ret;
{
	rlim64_t	value;

	if (pres->rs_value.at_type != ATR_TYPE_SIZE)
		return (PBSE_ATTRTYPE);
	value = pres->rs_value.at_val.at_size.atsv_num;
	if (pres->rs_value.at_val.at_size.atsv_units ==
	    ATR_SV_WORDSZ) {
		value *= sizeof(int);
	}
	*ret = value << pres->rs_value.at_val.at_size.atsv_shift;

	return (PBSE_NONE);
}

/*
 * Internal time decoding routine.
 *
 *	Accepts a resource pointer and a pointer to the unsigned long integer
 *	to receive the decoded value.  It returns a PBS error code, and the
 *	decoded value of time in seconds in the unsigned long integer.
 */

static int getlong(pres, ret)
    resource		*pres;
    unsigned long	*ret;
{

	if (pres->rs_value.at_type != ATR_TYPE_LONG)
		return (PBSE_ATTRTYPE);
	if (pres->rs_value.at_val.at_long < 0)
	        return (PBSE_BADATVAL);
	*ret = pres->rs_value.at_val.at_long;

	return (PBSE_NONE);
}

/*
 * Time decoding macro.  Accepts a timestruc_t pointer.  Returns unsigned long
 * time in seconds, rounded.
 */

#define tv(val) (ulong)((val).tv_sec + ((val).tv_nsec + 500000000)/1000000000)

/*
**      Scan a list of tasks and return true if one of them matches
**      the ash.
*/
static
int
injob(pjob, pash)
    job		*pjob;
    ash_t	*pash;
{
	ash_t	ash;

	sscanf(pjob->ji_globid, "%llx", &ash);
	return (ash == *pash);
}

/*
 * Internal session cpu time decoding routine.
 *
 *	Accepts a head of a task list.  Returns the sum of all cpu time
 *	consumed for all tasks in the list, in seconds, adjusted by cputfactor.
 */
static unsigned long cput_sum(pjob)
    job		   *pjob;
{
	char			*id = "cput_ses";
	int			i;
	int			inproc = 0;
	ulong			cputime;
	ulong			deltat;
	int			nps = 0;
	ulong			proctime;
	ulong			perc;
	prpsinfo_t		*pi;

	cputime = 0;
	if (pjob->ji_globid == NULL) {
		pjob->ji_flags |= MOM_NO_PROC;
		return (cputime);
	}

	for (i=0; i<nproc; i++) {
		pi = &proc_array[i].info;

		if (!injob(pjob, &proc_array[i].procash))
		   if (!inproc)
			continue;
		   else
			break;

		inproc = 1;

		nps++;
		proctime = tv(pi->pr_time) + tv(pi->pr_ctime);
		cputime += proctime;
		DBPRT(("%s: ses %d pid %d pcput %lu cputime %lu\n", id, pi->pr_sid, pi->pr_pid, proctime, cputime))
	}

	if (nps == 0)
		pjob->ji_flags |= MOM_NO_PROC;

	return ((unsigned long)((double)cputime * cputfactor));
}

/*
 * Return TRUE if any process in the job is over limit for cputime usage.
 */
static int overcpu_proc(pjob, limit)
    job			*pjob;
    unsigned long	limit;
{
	ulong		cputime;
	int		i;
	int		inproc = 0;
	prpsinfo_t	*pi;

	if (pjob->ji_globid == NULL)
		return FALSE;

	for (i=0; i<nproc; i++) {
		pi = &proc_array[i].info;

		if (!injob(pjob, &proc_array[i].procash))
		    if (!inproc)
			continue;
		    else
			break;

		inproc = 1;
		cputime = tv(pi->pr_time) + tv(pi->pr_ctime);

		if (cputime > limit)
			return (TRUE);
        }

	return (FALSE);
}

/*
 * Internal session memory usage function.
 *
 *	Returns the total number of bytes of address
 *      space consumed by all current tasks within the list of tasks.
 */
static rlim64_t mem_sum(pjob)
    job		*pjob;
{
	static	char		id[] = "mem_sum";
	int			i;
	int			inproc = 0;
	rlim64_t		segadd;
	prpsinfo_t		*pi;

	DBPRT(("%s: entered pagesize %d\n", id, pagesize))
	segadd = 0;
	if (pjob->ji_globid == NULL)
		return (segadd);

	for (i=0; i<nproc; i++) {
		pi = &proc_array[i].info;

		if (!injob(pjob, &proc_array[i].procash))
		    if (!inproc)
			continue;
		    else
			break;

		DBPRT(("%s: %s(%d:%d) mem %llu\n",
			id, pi->pr_fname, pi->pr_sid, pi->pr_pid,
			(rlim64_t)((rlim64_t)pi->pr_size * (rlim64_t)pagesize)))

		segadd += (rlim64_t)((rlim64_t)pi->pr_size*(rlim64_t)pagesize);
	}
	DBPRT(("%s: total mem %llu\n\n", id, segadd))
	return (segadd);
}

#if COMPLEX_MEM_CALC==1
/*
 * Internal session resident memory size function.  COMPLEX VERSION
 *
 *	Returns in a 64 bit intege the number of bytes used by session
 */
static rlim64_t resi_sum(pjob)
    job		*pjob;
{
	static	char		id[] = "resi_sum";
	rlim64_t		resisize, resisub;
	int			num, i, j;
	int			inproc = 0;
	prpsinfo_t		*pi;
	prmap_sgi_t		*mp;
	u_long			lastseg, nbps;

	DBPRT(("%s: entered pagesize %d\n", id, pagesize))

	resisize = 0;
	if (pjob->ji_globid == NULL)
		return (resisize);
	lastseg = 99999;
	nbps = (pagesize / sizeof(uint_t)) * pagesize;
				/* sysmacros.h says "4Meg" ...hmmm */

	for (i=0; i<nproc; i++) {
		pi = &proc_array[i].info;

		if (!injob(pjob, &proc_array[i].procash))
		    if (!inproc)
                        continue;
		    else
			break;

		DBPRT(("%s: %s(%d:%d) rss %llu (%lu pages)\n",
			id, pi->pr_fname, pi->pr_sid, pi->pr_pid,
			(rlim64_t)((rlim64_t)pi->pr_rssize*(rlim64_t)pagesize),
			pi->pr_rssize))

		resisub = 0;
		num = proc_array[i].map_num;
		mp = proc_array[i].map;
		for (j=0; j<num; j++, mp++) {
			u_long	cnt = mp->pr_mflags >> MA_REFCNT_SHIFT;
			u_long	end = (u_long)mp->pr_vaddr + mp->pr_size - 1;
			u_long	seg1 = (u_long)mp->pr_vaddr / nbps;
			u_long	seg2 = end / nbps;
			rlim64_t	numseg = seg2 - seg1;

			if (lastseg != seg2)
				numseg++;
			lastseg = seg2;
			numseg = numseg*pagesize/cnt;
			numseg += mp->pr_wsize*pagesize/MA_WSIZE_FRAC/cnt;
			resisub += numseg;
			DBPRT(("%s: %d\t%lluk\t%lluk\n", id, j,
				numseg/1024, resisub/1024))
		}
		resisize += resisub;
		DBPRT(("%s: %s subtotal rss %llu\n", id,
			pi->pr_fname, resisub))
        }
	DBPRT(("%s: total rss %llu\n\n", id, resisize))
	return (resisize);
}
#else /* COMPLEX_MEM_CALC==0 */
/*
 * Internal session mem (workingset) size function.  SIMPLE CALC VERSION
 *
 *	Returns in a 64 bit intege the number of bytes used by session
 */
static rlim64_t resi_sum(pjob)
    job			*pjob;
{
	static	char		id[] = "resi_sum";
	int			i;
	int			inproc = 0;
	rlim64_t		resisize, resisub;
	prpsinfo_t		*pi;

	DBPRT(("%s: entered pagesize %d\n", id, pagesize))

	resisize = 0;
	for (i=0; i<nproc; i++) {
		pi = &proc_array[i].info;

		if (!injob(pjob, &proc_array[i].procash))
		    if (!inproc)
                        continue;
		    else
			break;

		DBPRT(("%s: %s(%d:%d) rss %llu (%lu pages)\n",
			id, pi->pr_fname, pi->pr_sid, pi->pr_pid,
			(rlim64_t)((rlim64_t)pi->pr_rssize*(rlim64_t)pagesize),
			pi->pr_rssize))
		resisize += (rlim64_t)((rlim64_t)pagesize * pi->pr_rssize);
	}
	DBPRT(("%s: total rss %llu\n\n", id, resisize))
	return (resisize);
}
#endif /* COMPLEX_MEM_CALC */


/*
 * Return TRUE if any process in the session is over limit for memory usage.
 */
static int overmem_proc(pjob, limit)
    job			*pjob;
    rlim64_t		limit;
{
	int		i;
	int		inproc = 0;
	prpsinfo_t	*pi;

	if (pjob->ji_globid == NULL)
		return (FALSE);

	for (i=0; i<nproc; i++) {
		pi = &proc_array[i].info;

		if (!injob(pjob, &proc_array[i].procash))
		    if (!inproc)
			continue;
		    else
			break;
                if ((rlim64_t)((rlim64_t)pagesize * pi->pr_size) > limit)
			return (TRUE);
        }

	return (FALSE);
}

extern char *msg_momsetlim;

/*
 * Internal error routine
 */
int error(string, value)
    char	*string;
    int		value;
{
	int		i = 0;
	char		*message;

	assert(string != NULL);
	assert(*string != '\0');
	assert(value > PBSE_);			/* minimum PBS error number */
	assert(value <= PBSE_NOSYNCMSTR);	/* maximum PBS error number */
	assert(pbs_err_to_txt[i].err_no != 0);

	do {
		if (pbs_err_to_txt[i].err_no == value)
			break;
	} while (pbs_err_to_txt[++i].err_no != 0);

	assert(pbs_err_to_txt[i].err_txt != NULL);
	message = *pbs_err_to_txt[i].err_txt;
	assert(message != NULL);
	assert(*message != '\0');
	(void)fprintf(stderr, msg_momsetlim, string, message);
	(void)fflush(stderr);

	return (value);
}

/*
 * Establish system-enforced limits for the job.
 *
 *	Run through the resource list, checking the values for all items
 *	we recognize.
 *
 *	If set_mode is SET_LIMIT_SET, then also set hard limits for the
 *	system enforced limits (not-polled).
 *	If anything goes wrong with the process, return a PBS error code
 *	and print a message on standard error.  A zero-length resource list
 *	is not an error.
 *
 *	If set_mode is SET_LIMIT_SET the entry conditions are:
 *	    1.	MOM has already forked, and we are called from the child.
 *	    2.	The child is still running as root.
 *	    3.  Standard error is open to the user's file.
 *
 *	If set_mode is SET_LIMIT_ALTER, we are beening called to modify
 *	existing limits.  Cannot alter those set by setrlimit (kernel)
 *	because we are the wrong process.  
 */
int mom_set_limits(pjob, set_mode)
    job			*pjob;
    int			 set_mode;	/* SET_LIMIT_SET or SET_LIMIT_ALTER */
{
	char		*id = "mom_set_limits";
	char		*pname;
	int		retval;
	rlim64_t	sizeval; /* place to build 64 bit value */
	unsigned long	value;	/* place in which to build resource value */
	resource	*pres;
       	struct rlimit64	res64lim;
	rlim64_t	mem_limit  = 0;
#if	NODEMASK != 0
	__uint64_t	rvalue;
	__uint64_t	nodemask;
#endif	/* NODEMASK */

        log_buffer[0] = '\0';

	DBPRT(("%s: entered\n", id))
	assert(pjob != NULL);
	assert(pjob->ji_wattr[(int)JOB_ATR_resource].at_type == ATR_TYPE_RESC);
	pres = (resource *)
	    GET_NEXT(pjob->ji_wattr[(int)JOB_ATR_resource].at_val.at_list);

/*
 * Cycle through all the resource specifications,
 * setting limits appropriately.
 */

	while (pres != NULL) {
		assert(pres->rs_defin != NULL);
		pname = pres->rs_defin->rs_name;
		assert(pname != NULL);
		assert(*pname != '\0');

		if (strcmp(pname, "ncpus") == 0) {
			char	hold[16];
			extern	struct	var_table	vtable;

			retval = getlong(pres, &value);
			if (retval != PBSE_NONE)
			        return (error(pname, retval));
			sprintf(hold, "%d", (int)pres->rs_value.at_val.at_long);
			bld_env_variables(&vtable, "NCPUS", hold);
		} else if (strcmp(pname, "cput") == 0) {	/* check */
			/* cpu time - check, if less than pcput use it */
			retval = getlong(pres, &value);
			if (retval != PBSE_NONE)
			        return (error(pname, retval));
		} else if (strcmp(pname, "pcput") == 0) {
			/* process cpu time - set */
			retval = getlong(pres, &value);
			if (retval != PBSE_NONE)
			        return (error(pname, retval));
			res64lim.rlim_cur = res64lim.rlim_max =
				(unsigned long)((double)value / cputfactor);
			if (setrlimit64(RLIMIT_CPU, &res64lim) < 0)
	        		return (error("RLIMIT_CPU", PBSE_SYSTEM));
		} else if (strcmp(pname, "file") == 0) {	/* set */
			if (set_mode == SET_LIMIT_SET)  {
			    retval = getsize(pres, &sizeval);
			    if (retval != PBSE_NONE)
			        return (error(pname, retval));
			    res64lim.rlim_cur = res64lim.rlim_max = sizeval;
			    if (setrlimit64(RLIMIT_FSIZE, &res64lim) < 0)
			        return (error(pname, PBSE_SYSTEM));
			}
		} else if (strcmp(pname, "vmem") == 0) {	/* check */
			retval = getsize(pres, &sizeval);
			if (retval != PBSE_NONE)
			        return (error(pname, retval));
			if ((mem_limit == 0) || (sizeval < mem_limit))
				mem_limit = sizeval;
		} else if (strcmp(pname, "pvmem") == 0) {	/* set */
			retval = getsize(pres, &sizeval);
			if (retval != PBSE_NONE)
			        return (error(pname, retval));
			if ((mem_limit == 0) || (sizeval < mem_limit))
				mem_limit = sizeval;
		} else if (strcmp(pname, "mem") == 0) {		/* ignore */
		} else if (strcmp(pname, "pmem") == 0) {	/* set */
			if (set_mode == SET_LIMIT_SET)  {
			    retval = getsize(pres, &sizeval);
			    if (retval != PBSE_NONE)
			        return (error(pname, retval));
			    res64lim.rlim_cur = res64lim.rlim_max = sizeval;
			    if (setrlimit64(RLIMIT_RSS, &res64lim) < 0)
	        		return (error("RLIMIT_RSS", PBSE_SYSTEM));
			}
		} else if (strcmp(pname, "walltime") == 0) {	/* Check */
			retval = getlong(pres, &value);
			if (retval != PBSE_NONE)
			        return (error(pname, retval));
		} else if (strcmp(pname, "nice") == 0) {	/* set nice */
			if (set_mode == SET_LIMIT_SET)  {
			    errno = 0;
			    if ((nice((int)pres->rs_value.at_val.at_long) == -1)
				&& (errno != 0))
				return (error(pname, PBSE_BADATVAL));
			}
#if	NODEMASK != 0
		} else if (strcmp(pname, "nodemask") == 0) {  /* set nodemask */
			/* call special node mask function */
			nodemask = pres->rs_value.at_val.at_ll;
			rvalue = (__uint64_t)pmoctl(61, nodemask,0);
			if (rvalue != nodemask) {
				(void)sprintf(log_buffer, "Tried to set node mask to 0x%0llx, was set to 0x%0llx", nodemask, rvalue);
				LOG_EVENT(PBSEVENT_ERROR, PBS_EVENTCLASS_JOB,
					  pjob->ji_qs.ji_jobid, log_buffer);
			}
#endif	/* NODEMASK */
		} else if ((pres->rs_defin->rs_flags & ATR_DFLAG_RMOMIG) == 0)
			/* don't recognize and not marked as ignore by mom */
			return (error(pname, PBSE_UNKRESC));
		pres = (resource *)GET_NEXT(pres->rs_link);
	}
	if (set_mode == SET_LIMIT_SET)  {
	    /* if either of vmem or pvmem was given, set sys limit to lesser */
	    if (mem_limit != 0) {
		res64lim.rlim_cur = res64lim.rlim_max= mem_limit;
		if (setrlimit64(RLIMIT_VMEM, &res64lim) < 0)
			return (error("RLIMIT_VMEM", PBSE_SYSTEM));
	    }
	}
	return (PBSE_NONE);
}
/*
 * State whether MOM main loop has to poll this job to determine if some
 * limits are being exceeded.
 *
 *	Sets flag TRUE if polling is necessary, FALSE otherwise.  Actual
 *	polling is done using the mom_over_limit machine-dependent function.
 */

int mom_do_poll(pjob)
    job			*pjob;
{
	char		*id = "mom_do_poll";
	char		*pname;
	resource	*pres;

	DBPRT(("%s: entered\n", id))
	assert(pjob != NULL);
	assert(pjob->ji_wattr[(int)JOB_ATR_resource].at_type == ATR_TYPE_RESC);
	pres = (resource *)
	    GET_NEXT(pjob->ji_wattr[(int)JOB_ATR_resource].at_val.at_list);

	while (pres != NULL) {
		assert(pres->rs_defin != NULL);
		pname = pres->rs_defin->rs_name;
		assert(pname != NULL);
		assert(*pname != '\0');

		if (strcmp(pname, "walltime") == 0 ||
		    strcmp(pname, "mem") == 0 ||
		    strcmp(pname, "ncpus") == 0 ||
		    strcmp(pname, "cput") == 0 ||
		    strcmp(pname, "pcput") == 0 ||
		    strcmp(pname, "pvmem") == 0 ||
		    strcmp(pname, "vmem") == 0)
			return (TRUE);
		pres = (resource *)GET_NEXT(pres->rs_link);
	}

	return (FALSE);
}

/*
 * Setup for polling.
 *
 *	Open kernel device and get namelist info.
 *	Also open sgi project files.
 */
int mom_open_poll()
{
	char		*id = "mom_open_poll";
	extern int	 open_sgi_proj();

	DBPRT(("%s: entered\n", id))
	pagesize = getpagesize();

	proc_array = (struct proc_info *)calloc(TBL_INC,
				sizeof(struct proc_info));
	if (proc_array == NULL) {
		log_err(errno, id, "malloc");
		return (PBSE_SYSTEM);
	}
	max_proc = TBL_INC;

	return (open_sgi_proj());
}

/*
 * Declare start of polling loop.
 *
 *	for each job, obtain ASH from task tables
 *	then obtain list of pids in each ASH in turn
 *	open and process /proc/(pid)
 */

int mom_get_sample()
{
	static	char		id[] = "mom_get_sample";
	int			fd;
	struct dirent		*dent;
	char			procname[100];
	int			np;
	int			num;
	int			mapsize;
	time_t			currtime;
	prmap_sgi_arg_t		maparg;
	struct	proc_info	*pi;
	prmap_sgi_t		map[MAPNUM];
	job			*pjob;
	aspidlist_t		*taskpids = 0;
	ash_t			ash;
	extern aserror_t 	aserrorcode;
	extern list_head	svr_alljobs;
	

	DBPRT(("%s: entered pagesize %d\n", id, pagesize))
	rewinddir(pdir);
	nproc = 0;
	pi = proc_array;

	mapsize = sizeof(prmap_sgi_t) * MAPNUM;
	maparg.pr_size = mapsize;

	currtime = time(0);
	for (pjob = (job *)GET_NEXT(svr_alljobs);
				pjob;
				pjob = (job *)GET_NEXT(pjob->ji_alljobs)) {
		if (pjob->ji_qs.ji_substate != JOB_SUBSTATE_RUNNING)
			continue;

		if (pjob->ji_globid == NULL)
			continue;

		sscanf(pjob->ji_globid, "%llx", &ash);
		DBPRT(("%s: looking at job %s ASH %llx\n",
			id, pjob->ji_qs.ji_jobid, ash))

		taskpids = aspidsinash_local(ash);
		if (taskpids == NULL) {
		    sprintf(log_buffer, "no pids in ash %lld for job %s", 
			ash, pjob->ji_qs.ji_jobid);
		    log_err(aserrorcode, id, log_buffer);
		    continue;
		}

		for (np=0; np < taskpids->numpids; ++np, (void)close(fd)) {
	    

		    DBPRT(("%s:\t\t pid %d\n", id, taskpids->pids[np]))
		    sprintf(procname, "%s/%d", procfs, taskpids->pids[np]);
		    if ((fd = open(procname, O_RDONLY)) == -1)
			continue;

		    if (ioctl(fd, PIOCPSINFO, &pi->info) == -1) {
			if (errno != ENOENT) {
				sprintf(log_buffer,
					"%s: ioctl(PIOCPSINFO)", procname);
				log_err(errno, id, log_buffer);
			}
			continue;
		    }

#if COMPLEX_MEM_CALC==1
		    if (pi->map) {
			free(pi->map);		/* free any old space */
			pi->map = NULL;
		    }
		    pi->map_num = 0;
		    maparg.pr_vaddr = (caddr_t)map;

		    if ((num = ioctl(fd, PIOCMAP_SGI, &maparg)) == -1) {
			if (errno != ENOENT)
				log_err(errno, id, "ioctl(PIOCMAP_SGI)");
			free(map);
			continue;
		    }
		    if (num > 0) {
			size_t	nb = sizeof(prmap_sgi_t) * num;

			assert(num < MAPNUM);
			pi->map = (prmap_sgi_t *) malloc(nb);
			memcpy(pi->map, map, nb);
			pi->map_num = num;
		    }
#endif /* COMPLEX_MEM_CALC */

		    /* save the ASH to which the proc belongs */
		    pi->procash = ash; 

		    if (++nproc == max_proc) {
			struct	proc_info	*hold;

			DBPRT(("%s: alloc more table space %d\n", id, nproc))
			max_proc *= 2;

			hold = (struct proc_info *)realloc(proc_array,
					max_proc*sizeof(struct proc_info));
			assert(hold != NULL);
			proc_array = hold;
			memset(&proc_array[nproc], '\0',
				sizeof(struct proc_info) * (max_proc >> 1));
		    }
		    pi = &proc_array[nproc];
		}
		if (taskpids != NULL)
		    asfreepidlist(taskpids, 0);
	}
	sampletime = time(0);
	if ((sampletime - currtime) > 5) {
		sprintf(log_buffer, "time lag %d secs", sampletime-currtime);
		log_err(-1, id, log_buffer);
		return PBSE_SYSTEM;
	}
	return (PBSE_NONE);
}

/*
 * Measure job resource usage and compare with its limits.
 *
 *	If it has exceeded any well-formed polled limit return TRUE.
 *	Otherwise, return FALSE.
 */

int mom_over_limit(pjob)
    job			*pjob;
{
	char		*pname;
	int		retval;
	rlim64_t	sizeval;
	unsigned long	value, num;
	rlim64_t	num64;
	resource	*pres;

	assert(pjob != NULL);
	assert(pjob->ji_wattr[(int)JOB_ATR_resource].at_type == ATR_TYPE_RESC);
	pres = (resource *)
	    GET_NEXT(pjob->ji_wattr[(int)JOB_ATR_resource].at_val.at_list);

	for ( ; pres != NULL; pres = (resource *)GET_NEXT(pres->rs_link)) {
		assert(pres->rs_defin != NULL);
		pname = pres->rs_defin->rs_name;
		assert(pname != NULL);
		assert(*pname != '\0');

		if (strcmp(pname, "ncpus") == 0) {
			attribute		*at;
			resource_def		*rd;
			resource		*prescpup;

			retval = getlong(pres, &value);
			if (retval != PBSE_NONE)
				continue;

			at = &pjob->ji_wattr[(int)JOB_ATR_resc_used];
			assert(at->at_type == ATR_TYPE_RESC);
			rd = find_resc_def(svr_resc_def, "cpupercent",
				svr_resc_size);
			assert(rd != NULL);
			prescpup = find_resc_entry(at, rd);
			assert(prescpup != NULL);

			num = prescpup->rs_value.at_val.at_long;
			if (num > (value*100+10)) {
				sprintf(log_buffer,
					"ncpus %.2f exceeded limit %lu",
					(float)num/100.0, value);
#if !defined(SGI_ZOMBIE_WRONG)
				return (TRUE);
#else
				LOG_EVENT(PBSEVENT_JOB, PBS_EVENTCLASS_JOB,
					  pjob->ji_qs.ji_jobid, log_buffer);
#endif /* SGI_ZOMBIE_WRONG */
			}
		} else if (strcmp(pname, "cput") == 0) {
			retval = getlong(pres, &value);
			if (retval != PBSE_NONE)
				continue;
			if ((num = cput_sum(pjob)) > value) {
				sprintf(log_buffer,
					"cput %lu exceeded limit %lu",
					num, value);
				return (TRUE);
			}
		} else if (strcmp(pname, "pcput") == 0) {
			retval = getlong(pres, &value);
			if (retval != PBSE_NONE)
				continue;
			if (overcpu_proc(pjob, value)) {
				sprintf(log_buffer,
					"pcput exceeded limit %lu",
					value);
				return (TRUE);
			}
		} else if (strcmp(pname, "vmem") == 0) {
			retval = getsize(pres, &sizeval);
			if (retval != PBSE_NONE)
				continue;
			if ((num64 = mem_sum(pjob)) > sizeval) {
				sprintf(log_buffer,
					"vmem %llu exceeded limit %llu",
					num64, sizeval);
				return (TRUE);
			}
		} else if (strcmp(pname, "pvmem") == 0) {
			retval = getsize(pres, &sizeval);
			if (retval != PBSE_NONE)
				continue;
			if (overmem_proc(pjob, sizeval)) {
				sprintf(log_buffer,
					"pvmem exceeded limit %llukb", sizeval);
				return (TRUE);
			}
		} else if (strcmp(pname, "mem") == 0) {
			retval = getsize(pres, &sizeval);
			if (retval != PBSE_NONE)
				continue;
			if ((num64 = resi_sum(pjob)) > sizeval) {
				sprintf(log_buffer,
					"mem %llu exceeded limit %llu",
					num64, sizeval);
				return (TRUE);
			}
		} else if (strcmp(pname, "walltime") == 0) {
			if ((pjob->ji_qs.ji_svrflags & JOB_SVFLG_HERE) == 0)
				continue;
			retval = getlong(pres, &value);
			if (retval != PBSE_NONE)
				continue;
			num = (unsigned long)((double)(time_now - pjob->ji_qs.ji_stime) * wallfactor);
			if (num > value) {
				sprintf(log_buffer,
					"walltime %lu exceeded limit %lu",
					num, value);
				if (ignwalltime == 0)
					return (TRUE);
			}
		}
	}

	return (FALSE);
}

/*
 * Update the job attribute for resources used.
 *
 *	The first time this is called for a job, set up resource entries for
 *	each resource that can be reported for this machine.  Fill in the
 *	correct values.  Return an error code.
 */
int mom_set_use(pjob)
    job			*pjob;
{
	resource		*pres;
	attribute		*at;
	resource_def		*rd;
	unsigned long		*lp, lnum, newcpu, oldcpu;
	long			 dur;
	unsigned long		 percent;

	assert(pjob != NULL);
	at = &pjob->ji_wattr[(int)JOB_ATR_resc_used];
	assert(at->at_type == ATR_TYPE_RESC);

	at->at_flags |= ATR_VFLAG_MODIFY;
	if ((at->at_flags & ATR_VFLAG_SET) == 0) {
		at->at_flags |= ATR_VFLAG_SET;

		rd = find_resc_def(svr_resc_def, "cput", svr_resc_size);
		assert(rd != NULL);
		pres = add_resource_entry(at, rd);
		pres->rs_value.at_flags |= ATR_VFLAG_SET;
		pres->rs_value.at_type = ATR_TYPE_LONG;
		pres->rs_value.at_val.at_long = 0;

		rd = find_resc_def(svr_resc_def, "cpupercent", svr_resc_size);
		assert(rd != NULL);
		pres = add_resource_entry(at, rd);
		pres->rs_value.at_flags |= ATR_VFLAG_SET;
		pres->rs_value.at_type = ATR_TYPE_LONG;
		pres->rs_value.at_val.at_long = 0;

		rd = find_resc_def(svr_resc_def, "vmem", svr_resc_size);
		assert(rd != NULL);
		pres = add_resource_entry(at, rd);
		pres->rs_value.at_flags |= ATR_VFLAG_SET;
		pres->rs_value.at_type = ATR_TYPE_SIZE;
		pres->rs_value.at_val.at_size.atsv_shift = 10; /* in KB */
		pres->rs_value.at_val.at_size.atsv_units = ATR_SV_BYTESZ;

		rd = find_resc_def(svr_resc_def, "walltime", svr_resc_size);
		assert(rd != NULL);
		pres = add_resource_entry(at, rd);
		pres->rs_value.at_flags |= ATR_VFLAG_SET;
		pres->rs_value.at_type = ATR_TYPE_LONG;

		rd = find_resc_def(svr_resc_def, "mem", svr_resc_size);
		assert(rd != NULL);
		pres = add_resource_entry(at, rd);
		pres->rs_value.at_flags |= ATR_VFLAG_SET;
		pres->rs_value.at_type = ATR_TYPE_SIZE;
		pres->rs_value.at_val.at_size.atsv_shift = 10; /* in KB */
		pres->rs_value.at_val.at_size.atsv_units = ATR_SV_BYTESZ;
	}

	rd = find_resc_def(svr_resc_def, "cput", svr_resc_size);
	assert(rd != NULL);
	pres = find_resc_entry(at, rd);
	assert(pres != NULL);
	lp = (unsigned long *)&pres->rs_value.at_val.at_long;
	oldcpu = *lp;
	lnum = cput_sum(pjob);
	if (lnum > *lp) {
		*lp = lnum;

		if ( (dur = sampletime - pjob->ji_sampletim) > 10) {
			newcpu = *lp;	/* save new cput */
			rd = find_resc_def(svr_resc_def, "cpupercent", svr_resc_size);
			assert(rd != NULL);
			pres = find_resc_entry(at, rd);
			assert(pres != NULL);
			lp = (unsigned long *)&pres->rs_value.at_val.at_long;
			percent = (newcpu - oldcpu)*100 / dur;
			*lp = MAX(*lp, percent);
			DBPRT(("cpu %%  : ses %ld (new %lu - old %lu)/delta %ld = %lu%%\n", pjob->ji_wattr[(int)JOB_ATR_session_id].at_val.at_long, newcpu, oldcpu, dur, percent))
		}
		pjob->ji_sampletim = sampletime;
	}

	rd = find_resc_def(svr_resc_def, "vmem", svr_resc_size);
	assert(rd != NULL);
	pres = find_resc_entry(at, rd);
	assert(pres != NULL);
	lp = &pres->rs_value.at_val.at_size.atsv_num;
	lnum = (mem_sum(pjob) + 1023) >> 10;	/* as KB */
	*lp = MAX(*lp, lnum);

	rd = find_resc_def(svr_resc_def, "walltime", svr_resc_size);
	assert(rd != NULL);
	pres = find_resc_entry(at, rd);
	assert(pres != NULL);
	pres->rs_value.at_val.at_long = (long)((double)(time_now - pjob->ji_qs.ji_stime) * wallfactor);

	rd = find_resc_def(svr_resc_def, "mem", svr_resc_size);
	assert(rd != NULL);
	pres = find_resc_entry(at, rd);
	assert(pres != NULL);
	lp = &pres->rs_value.at_val.at_size.atsv_num;
	lnum = (resi_sum(pjob) + 1023) >> 10;	/* in KB */
	*lp = MAX(*lp, lnum);

	return (PBSE_NONE);
}

/*
 *	Kill a task session.
 *	Call with the task pointer and a signal number.
 */
int kill_task(ptask, sig)
    task	*ptask;
    int		sig;
{
	char		*id = "kill_task";
	ash_t		ash;
	int		ct = 0;
	int		np;
	struct startjob_rtn sgid;
	aspidlist_t	*taskpids = 0;
	extern aserror_t aserrorcode;

	if (ptask->ti_job->ji_globid != NULL) {
		sscanf(ptask->ti_job->ji_globid, "%llx", &ash);
	} else {
		ash = asashofpid(ptask->ti_qs.ti_sid);
		sgid.sj_ash = ash;
		set_globid(ptask->ti_job, &sgid);
	}
		
	if ((ash != 0LL) && (ash != -1LL)) {
		taskpids = aspidsinash_local(ash);
		if (taskpids) {
			for (np=0; np<taskpids->numpids; ++np) {
				(void)kill(taskpids->pids[np], sig);
				++ct;
			}
		} else {
			sprintf(log_buffer, "no pids in ash %lld in %s",ash,id);
			log_err(aserrorcode, id, log_buffer);
		}
	}
	return ct;
}

/*
 * Clean up everything related to polling.
 */
int mom_close_poll()
{
	char	*id = "mom_close_poll";
	int	i;

	DBPRT(("%s: entered\n", id))
	if (proc_array) {
#if COMPLEX_MEM_CALC==1
		for(i=0; i<max_proc; i++) {
			struct	proc_info	*pi = &proc_array[i];

			if (pi->map)
				free(pi->map);
		}
#endif /* COMPLEX_MEM_CALC */
		free(proc_array);
	}
	if (pdir) {
		if (closedir(pdir) != 0) {
			log_err(errno, id, "closedir");
			return (PBSE_SYSTEM);
		}
	}

	return (PBSE_NONE);
}

/*
 * mom_does_chkpnt - return 1 if mom supports checkpoint
 *			    0 if not
 */

int mom_does_chkpnt()
{
#if MOM_CHECKPOINT == 1
	return (1);
#else /* MOM_CHECKPOINT */
	return (0);
#endif	/* MOM_CHECKPOINT */
}

/*
 * Checkpoint the task.
 *
 *	If abort is true, kill it too.
 */

int mach_checkpoint(ptask, file, abort)
    task	*ptask;
    char	*file;
    int		abort;
{
#if MOM_CHECKPOINT == 1
	ash_t	ash;

	sscanf(ptask->ti_job->ji_globid, "%llx", &ash);

	/* ckpt_setup(0, 0);  Does nothing so why have it */

	if (abort) 
		cpr_flags = CKPT_CHECKPOINT_KILL | CKPT_NQE;
	else
		cpr_flags = CKPT_CHECKPOINT_CONT | CKPT_NQE;
	return ( ckpt_create(file, ash, P_ASH, 0, 0) );
     /*	return ( ckpt_create(file, ptask->ti_qs.ti_sid, P_SID, 0, 0) ); */
#else /* MOM_CHECKPOINT */
	return (-1);
#endif /* MOM_CHECKPOINT */
}

/*
 * Restart the task from the checkpoint file.
 *
 *	Return -1 on error or sid if okay.
 */

long mach_restart(ptask, file)
    task	*ptask;
    char	*file;
{
#if MOM_CHECKPOINT == 1
	ckpt_id_t rc;
	ash_t	 momash;
	ash_t	 oldash = 0;
	char	 cvtbuf[20];
	cpr_flags = CKPT_NQE;

	/* KLUDGE to work-around SGI problem, for some reason ckpt_restart() */
	/* passes open file descriptor to /proc to restarted process	     */
	if (pdir)
		closedir(pdir);

	/* To restart the job with its old ASH, Mom must be in that ASH	    */
	/* When she does the restart.   However, before changing to that    */
	/* ASH, Mom must put herself in a new ASH all by herself, otherwise */
	/* she will take other system daemons with her into the job's ASH   */

	momash = getash();
	newarraysess();		/* isolate Mom in a ASH by herself  */
	if (ptask->ti_job->ji_globid != NULL) {
					/* now get job's old ASH and set it */
		sscanf(ptask->ti_job->ji_globid, "%llx", &oldash);
		if (setash(oldash) == -1) {
			DBPRT(("setash failed before restart, errno = %d", errno))
		}
	}
	rc =  ckpt_restart(file, (struct ckpt_args **)0, 0);
	if ((ptask->ti_job->ji_globid == NULL) && (rc > 0)) {
		(void)sprintf(cvtbuf, "%llx", rc);
		ptask->ti_job->ji_globid = strdup(cvtbuf);
	}

	newarraysess();		/* again, isolate Mom into ASH by herself */
	if (setash(momash) == -1) {	/* put Mom back to her old ASH */
		DBPRT(("setash failed after restart, errno = %d", errno))
	}
	
	/* KLUDGE TO work-around SGI problem, ckpt_restart sets the uid of */
	/* the calling process (me) to that of the restarted process       */
	(void)setuid(0);
	if ((pdir = opendir(procfs)) == NULL) {
		log_err(errno, "mach_restart", "opendir");
	}

	return ((int)rc);
#else	/* MOM_CHECKPOINT */
	return (-1);
#endif	/* MOM_CHECKPOINT */
}

/*
**	Return 1 if proc table can be read, 0 otherwise.
*/
int
getprocs()
{
	static	unsigned	int	lastproc = 0;

	if (lastproc == reqnum)         /* don't need new proc table */
		return 1;

	if (mom_get_sample() != PBSE_NONE)
		return 0;

	lastproc = reqnum;
	return 1;
}

char	*
cput(attrib)
struct	rm_attribute	*attrib;
{
	rm_errno = RM_ERR_UNKNOWN;
	return NULL;
}

char	*
mem(attrib)
struct	rm_attribute	*attrib;
{
	rm_errno = RM_ERR_UNKNOWN;
	return NULL;
}

char	*
sessions(attrib)
struct	rm_attribute	*attrib;
{
	rm_errno = RM_ERR_UNKNOWN;
	return NULL;
}

char	*
pids(attrib)
struct	rm_attribute	*attrib;
{
	rm_errno = RM_ERR_UNKNOWN;
	return NULL;
}

char	*
nsessions(attrib)
struct	rm_attribute	*attrib;
{
	rm_errno = RM_ERR_UNKNOWN;
	return NULL;
}

char	*
nusers(attrib)
struct	rm_attribute	*attrib;
{
	rm_errno = RM_ERR_UNKNOWN;
	return NULL;
}

static char	*
totmem(attrib)
struct	rm_attribute	*attrib;
{
	static	char	id[] = "totmem";
	struct	statfs	fsbuf;

	if (attrib) {
		log_err(-1, id, extra_parm);
		rm_errno = RM_ERR_BADPARAM;
		return NULL;
	}

	if (statfs(procfs, &fsbuf, sizeof(struct statfs), 0) == -1) {
		log_err(errno, id, "statfs");
		rm_errno = RM_ERR_SYSTEM;
		return NULL;
	}
	DBPRT(("%s: bsize=%ld blocks=%lld\n", id,
		fsbuf.f_bsize, fsbuf.f_blocks))
	sprintf(ret_string, "%llukb", 
		((rlim64_t)fsbuf.f_bsize * (rlim64_t)fsbuf.f_blocks) >> 10);
	return ret_string;
}

/*
 * availmem() - return amount of available memory in system in KB as string
 */

static char	*
availmem(attrib)
struct	rm_attribute	*attrib;
{
	char	*id = "availmem";
	struct	statfs	fsbuf;

	if (attrib) {
		log_err(-1, id, extra_parm);
		rm_errno = RM_ERR_BADPARAM;
		return NULL;
	}

	if (statfs(procfs, &fsbuf, sizeof(struct statfs), 0) == -1) {
		log_err(errno, id, "statfs");
		rm_errno = RM_ERR_SYSTEM;
		return NULL;
	}
	DBPRT(("%s: bsize=%ld bfree=%lld\n", id,
		fsbuf.f_bsize, fsbuf.f_bfree))
	sprintf(ret_string, "%llukb", 
		((rlim64_t)fsbuf.f_bsize * (rlim64_t)fsbuf.f_bfree) >> 10);
	return ret_string;
}


static char	*
ncpus(attrib)
struct	rm_attribute	*attrib;
{
	char		*id = "ncpus";

	if (attrib) {
		log_err(-1, id, extra_parm);
		rm_errno = RM_ERR_BADPARAM;
		return NULL;
	}
	sprintf(ret_string, "%ld", sysmp(MP_NAPROCS));
	system_ncpus=sysmp(MP_NAPROCS);
	return ret_string;
}

/*
 * physmem() - return amount of physical memory in system in KB as string
 */

static char	*
physmem(attrib)
struct	rm_attribute	*attrib;
{
	char		*id = "physmem";
	unsigned int	pmem;

	if (attrib) {
		log_err(-1, id, extra_parm);
		rm_errno = RM_ERR_BADPARAM;
		return NULL;
	}
	if (lseek(kfd, (off_t)kern_addr[KSYM_PHYS], SEEK_SET) == -1) {
		sprintf(log_buffer, "lseek to 0x%llx", kern_addr[KSYM_PHYS]);
		log_err(errno, id, log_buffer);
		rm_errno = RM_ERR_SYSTEM;
		return NULL;
	}
	if (read(kfd, (char *)&pmem, sizeof(pmem)) != sizeof(pmem)) {
		log_err(errno, id, "read");
		rm_errno = RM_ERR_SYSTEM;
		return NULL;
	}

	sprintf(ret_string, "%llukb",((rlim64_t)pmem*(rlim64_t)pagesize) >> 10);
	return ret_string;
}

/*
 * size_fs() - return file system size in Kb as string 
 */

char	*
size_fs(param)
char	*param;
{
	char		*id = "size_fs";
	struct	statfs	fsbuf;

	if (param[0] != '/') {
		sprintf(log_buffer, "%s: not full path filesystem name: %s\n",
			id, param);
		log_err(-1, id, log_buffer);
		rm_errno = RM_ERR_BADPARAM;
		return NULL;
	}
	if (statfs(param, &fsbuf, sizeof(struct statfs), 0) == -1) {
		log_err(errno, id, "statfs");
		rm_errno = RM_ERR_BADPARAM;
		return NULL;
	}
	sprintf(ret_string, "%llukb", 
		((rlim64_t)fsbuf.f_bsize * (rlim64_t)fsbuf.f_bfree) >> 10);
	return ret_string;
}

/*
 * size_file() - return file size in Kb as string 
 */

char	*
size_file(param)
char	*param;
{
	char		*id = "size_file";
	struct	stat64   sbuf;

	if (param[0] != '/') {
		sprintf(log_buffer, "%s: not full path filesystem name: %s\n",
			id, param);
		log_err(-1, id, log_buffer);
		rm_errno = RM_ERR_BADPARAM;
		return NULL;
	}

	if (stat64(param, &sbuf) == -1) {
		log_err(errno, id, "stat");
		rm_errno = RM_ERR_BADPARAM;
		return NULL;
	}

	sprintf(ret_string, "%llukb", (sbuf.st_size+512)>>10);
	return ret_string;
}

char	*
size(attrib)
struct	rm_attribute	*attrib;
{
	char	*id = "size";
	char	*param;

	if (attrib == NULL) {
		log_err(-1, id, no_parm);
		rm_errno = RM_ERR_NOPARAM;
		return NULL;
	}
	if (momgetattr(NULL)) {
		log_err(-1, id, extra_parm);
		rm_errno = RM_ERR_BADPARAM;
		return NULL;
	}

	param = attrib->a_value;
	if (strcmp(attrib->a_qualifier, "file") == 0)
		return (size_file(param));
	else if (strcmp(attrib->a_qualifier, "fs") == 0)
		return (size_fs(param));
	else {
		rm_errno = RM_ERR_BADPARAM;
		return NULL;
	}
}

time_t	maxtm;

static void
setmax(dev)
char	*dev;
{
	struct	stat	sb;

	if (stat(dev, &sb) == -1)
		return;
	if (maxtm < sb.st_atime)
		maxtm = sb.st_atime;

	return;
}

char	*
idletime(attrib)
struct	rm_attribute	*attrib;
{
	char	*id = "idletime";
	DIR	*dp;
	struct	dirent	*de;
	char	ttyname[50];
	time_t	curtm;

	if (attrib) {
		log_err(-1, id, extra_parm);
		rm_errno = RM_ERR_BADPARAM;
		return NULL;
	}

	if ((dp=opendir("/dev")) == NULL) {
		log_err(errno, id, "opendir /dev");
		rm_errno = RM_ERR_SYSTEM;
		return NULL;
	}

	maxtm = 0;
	curtm = time(NULL);

	setmax("/dev/mouse");
	while ((de=readdir(dp)) != NULL) {
		if (maxtm >= curtm)
			break;
		if (strncmp(de->d_name, "tty", 3))
			continue;
		sprintf(ttyname, "/dev/%s", de->d_name);
		setmax(ttyname);
	}
	closedir(dp);

	sprintf(ret_string, "%d", MAX(0, curtm - maxtm));
	return ret_string;
}


int
get_la(rv)
	double	*rv;
{
	char	*id = "get_la";
	int 	load;

	if (lseek(kfd, (off_t)kern_addr[KSYM_LOAD], SEEK_SET) == -1) {
		sprintf(log_buffer, "lseek to 0x%llx", kern_addr[KSYM_LOAD]);
		log_err(errno, id, log_buffer);
		return (rm_errno = RM_ERR_SYSTEM);
	}
	if (read(kfd, (char *)&load, sizeof(load)) != sizeof(load)) {
		log_err(errno, id, "read");
		return (rm_errno = RM_ERR_SYSTEM);
	}

	/*
	** SGI does not have FSCALE like the SUN so the 1024
	** divisor was found by experment compairing the result
	** of this routine to uptime.
	*/
	*rv = (double)load/1024.0;
	return 0;
}

u_long
gracetime(secs)
u_long	secs;
{
	time_t	now = time((time_t *)NULL);

	if (secs > now)		/* time is in the future */
		return (secs - now);
	else
		return 0;
}

static char	*
quota(attrib)
struct	rm_attribute	*attrib;
{
	char	*id = "quota";
	int			type;
	dev_t			dirdev;
	uid_t			uid;
	struct	stat		sb;
	struct	mntent		*me;
	struct	dqblk		qi;
	FILE			*m;
	struct	passwd		*pw;
	static	char		*type_array[] = {
		"harddata",
		"softdata",
		"currdata",
		"hardfile",
		"softfile",
		"currfile",
		"timedata",
		"timefile",
	};
	enum	type_name {
		harddata,
		softdata,
		currdata,
		hardfile,
		softfile,
		currfile,
		timedata,
		timefile,
		type_end
	};

	if (attrib == NULL) {
		log_err(-1, id, no_parm);
		rm_errno = RM_ERR_NOPARAM;
		return NULL;
	}
	if (strcmp(attrib->a_qualifier, "type")) {
		sprintf(log_buffer, "unknown qualifier %s",
			attrib->a_qualifier);
		log_err(-1, id, log_buffer);
		rm_errno = RM_ERR_BADPARAM;
		return NULL;
	}		

	for (type=0; type<type_end; type++) {
		if (strcmp(attrib->a_value, type_array[type]) == 0)
			break;
	}
	if (type == type_end) {		/* check to see if command is legal */
		sprintf(log_buffer, "bad param: %s=%s",
			attrib->a_qualifier, attrib->a_value);
		log_err(-1, id, log_buffer);
		rm_errno = RM_ERR_BADPARAM;
		return NULL;
	}

	if ((attrib = momgetattr(NULL)) == NULL) {
		log_err(-1, id, no_parm);
		rm_errno = RM_ERR_NOPARAM;
		return NULL;
	}
	if (strcmp(attrib->a_qualifier, "dir") != 0) {
		sprintf(log_buffer, "bad param: %s=%s",
			attrib->a_qualifier, attrib->a_value);
		log_err(-1, id, log_buffer);
		rm_errno = RM_ERR_BADPARAM;
		return NULL;
	}
	if (attrib->a_value[0] != '/') {	/* must be absolute path */
		sprintf(log_buffer,
			"not an absolute path: %s", attrib->a_value);
		log_err(-1, id, log_buffer);
		rm_errno = RM_ERR_BADPARAM;
		return NULL;
	}
	if (stat(attrib->a_value, &sb) == -1) {
		sprintf(log_buffer, "stat: %s", attrib->a_value);
		log_err(errno, id, log_buffer);
		rm_errno = RM_ERR_EXIST;
		return NULL;
	}
	dirdev = sb.st_dev;
	DBPRT(("dir has devnum %d\n", dirdev))

	if ((m = setmntent(MOUNTED, "r")) == NULL) {
		log_err(errno, id, "setmntent");
		rm_errno = RM_ERR_SYSTEM;
                return NULL;
        }
	while ((me = getmntent(m)) != NULL) {
		if (strcmp(me->mnt_type, MNTTYPE_IGNORE) == 0)
			continue;
		if (stat(me->mnt_dir, &sb) == -1) {
			sprintf(log_buffer, "stat: %s", me->mnt_dir);
			log_err(errno, id, log_buffer);
			continue;
		}
		DBPRT(("%s\t%s\t%d\n", me->mnt_fsname, me->mnt_dir, sb.st_dev))
		if (sb.st_dev == dirdev)
			break;
	}
	endmntent(m);
	if (me == NULL)	{
		sprintf(log_buffer,
			"filesystem %s not found", attrib->a_value);
		log_err(-1, id, log_buffer);
		rm_errno = RM_ERR_EXIST;
		return NULL;
	}
	if (hasmntopt(me, MNTOPT_QUOTA) == NULL) {
		sprintf(log_buffer,
			"no quotas on filesystem %s", me->mnt_dir);
		log_err(-1, id, log_buffer);
		rm_errno = RM_ERR_EXIST;
		return NULL;
	}

	if ((attrib = momgetattr(NULL)) == NULL) {
		log_err(-1, id, no_parm);
		rm_errno = RM_ERR_NOPARAM;
		return NULL;
	}
	if (strcmp(attrib->a_qualifier, "user") != 0) {
		sprintf(log_buffer, "bad param: %s=%s",
			attrib->a_qualifier, attrib->a_value);
		log_err(-1, id, log_buffer);
		rm_errno = RM_ERR_BADPARAM;
		return NULL;
	}
	if ((uid = (uid_t)atoi(attrib->a_value)) == 0) {
		if ((pw = getpwnam(attrib->a_value)) == NULL) {
			sprintf(log_buffer,
				"user not found: %s", attrib->a_value);
			log_err(-1, id, log_buffer);
			rm_errno = RM_ERR_EXIST;
			return NULL;
		}
		uid = pw->pw_uid;
	}

	if (quotactl(Q_GETQUOTA, me->mnt_fsname, uid, (caddr_t)&qi) == -1) {
		log_err(errno, id, "quotactl");
		rm_errno = RM_ERR_SYSTEM;
		return NULL;
	}

	/* all size values in KB */
	switch (type) {
	case harddata:
		sprintf(ret_string, "%ukb", BBTOB(qi.dqb_bhardlimit) >> 10);
		break;
	case softdata:
		sprintf(ret_string, "%ukb", BBTOB(qi.dqb_bsoftlimit) >> 10);
		break;
	case currdata:
		sprintf(ret_string, "%ukb", BBTOB(qi.dqb_curblocks) >> 10);
		break;
	case hardfile:
		sprintf(ret_string, "%u", qi.dqb_fhardlimit);
		break;
	case softfile:
		sprintf(ret_string, "%u", qi.dqb_fsoftlimit);
		break;
	case currfile:
		sprintf(ret_string, "%u", qi.dqb_curfiles);
		break;
	case timedata:
		sprintf(ret_string, "%lu",gracetime((u_long)qi.dqb_btimelimit));
		break;
	case timefile:
		sprintf(ret_string, "%lu",gracetime((u_long)qi.dqb_ftimelimit));
		break;
	}

	return ret_string;
}

#if	NODEMASK != 0

#define MAXCNODES 64	/* Maximum number of nodes available in a nodemask. */

/*
** Return a MAXCNODES-bit string with a '1' in each position where there
** are two CPUs available for a node.
*/
static char    *
availmask(attrib)
    struct	rm_attribute	*attrib;
{
	char		*id = "availmask";
	int		nprocs, i;
	cnodeid_t	cpumap[MAXCNODES * 2];
	char		nodect[MAXCNODES];

	if (attrib) {
		log_err(-1, id, extra_parm);
		rm_errno = RM_ERR_BADPARAM;
		return NULL;
	}

	/* Initialize cpu/node count and cpumap. */
	for (i = 0; i < MAXCNODES; i++) {
		nodect[i] = 0;
		cpumap[i * 2] = cpumap[i * 2 + 1] = -1;
	}

	nprocs = sysmp(MP_NPROCS);
	if (nprocs < 1) {
		log_err(errno, id, "sysmp(MP_NPROCS");
		rm_errno = RM_ERR_SYSTEM;
		return (NULL);
	}

	if (sysmp(MP_NUMA_GETCPUNODEMAP, (void *)cpumap, 
	    sizeof(cnodeid_t) * nprocs) != 0) 
	{
		log_err(errno, id, "sysmp(MP_NUMA_GETCPUNODEMAP");
		rm_errno = RM_ERR_SYSTEM;
		return (NULL);
	}

	/* 
	 * cpumap[] now contains either -1, or the number of the node that
	 * corresponds to that CPU.  Each node needs to have 2 CPUs - count 
	 * up the references to each node, and generate the return string 
	 * based on whether exactly 2 CPUs map to the node at the n'th 
	 * position (from the right) or not.
	 */
	for (i = 0; i < MAXCNODES * 2; i++)
		if (cpumap[i] != (cnodeid_t)(-1))
			nodect[cpumap[i]] ++;

	/* Create the null-terminated string for the nodect ref'd in map[]. */
	for (i = 0; i < MAXCNODES; i++)
		ret_string[MAXCNODES - (i + 1)] = (nodect[i] == 2 ? '1' : '0');
	ret_string[i] = '\0';
	
	return ret_string;
}
#endif /* NODEMASK */



void scan_non_child_tasks(void)

  {
  /* NYI */

  return;
  }  /* END scan_non_child_tasks() */


