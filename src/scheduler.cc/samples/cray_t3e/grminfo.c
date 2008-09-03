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
 * This file contains the routines necessary to query information from
 * the T3e's Global Resource Monitor (GRM). The GRM has the final decison
 * on the placement of jobs on the torus of the T3e, thus its a good idea
 * for the scheduler to know some of the contraints of the GRM, so as to
 * avoid scheduling a job that the GRM will not be able to run immediately.
 * For example, the GRM enforces a requirement that all jobs must be run
 * on contiguous PEs. So knowing the total number of free PE's is not
 * sufficient for scheduling.
 *
 * Secondly, the T3e permits a site to assign arbitrary "labels" to PEs.
 * These must conform to certain restrictions (see also setlabel(1) and
 * grmview(1) ).  For example, the label must be 2-8 characters in length
 * and start with either an 'H' or 'S' indicating whether it is a hard
 * (e.g. required) or soft (e.g.  requested, not required) label.
 *
 * The routines in this file make sactl(2) calls to query this information
 * from the GRM, and creates a global PE_MAP array for use by the queue
 * packing algorithms, etc.
 */

#include <stdio.h>
#include <sys/types.h>
#include <sys/param.h>
typedef unsigned int    uint; /* why isn't this defined in sys/??? */
#include <sys/sactl.h>
#include <sys/grminfo.h>
#include <string.h>
#include <malloc.h>

#include "toolkit.h"
#include "gblxvars.h"

#define OPERATIONAL (GRMINFO_PE_AVAIL_REG | GRMINFO_PE_AVAIL_NET)

/*
 * Information is gathered from three locations and stored in the global
 * schd_PEMAP array. The array is populated in two steps:
 *
 *    1. PE specific information (current and max application counts,
 *       status, label)
 *    2. Running Application information (each running job reported by
 *       GRM is mapped back (via the sessionID/JID GRM field) to a PBS
 *       JOBID (from the Running Jobs List), if its available).
 */
int load_pe_map(Job *jobs)
  {
  int             i, j, k, ret, pe, found, pe_count = 0;
  int             name[SA_MAX_NAMES];
  char            buf[256];
  sa_info_t       sa_info;
  Job            *this;

  struct pe_attr *attr;

  struct pe_app  *app;
  static char    *func_id = "load_pe_map";
  union
    {
    char slab[sizeof(int) * 2];
    int  ilab[2];
    } label;

  for (i = 0; i < MAX_PE_MAP_SIZE; i++)
    {
    schd_PEMAP[i].job       = NULL;
    schd_PEMAP[i].label[0]  = '\0';
    schd_PEMAP[i].status    = STAT_DOWN;  /* default: down */
    schd_PEMAP[i].apps_max  = -1;         /* unlimited apps */
    schd_PEMAP[i].apps_num  = 0;          /* # of apps assigned */
    }

  /* load PE resource information */
  /* Get the PEs MAP */

  name[0] = SA_GRM;

  name[1] = GRM_CTL_PE_MAP;

  ret = sactl(name, 2, (char *) & sa_info, sizeof(sa_info), SA_INFO, 0, 0, 0);

  if (ret < 0)
    {
    return -1;
    }
  else if (sa_info.numrec == 0)
    {
    return -1; /* no PEs defined */
    }

  if ((attr = (struct pe_attr *)malloc(sa_info.numrec * sa_info.recsize))
      != (struct pe_attr *)0)
    {
    ret = sactl(name, 2, (char *) attr,
                sa_info.numrec * sa_info.recsize, SA_READ, 0, 0, 0);

    if (ret < 0)
      {
      free((char *)attr);
      return -1;
      }

    for (i = 0, pe = 0; i < sa_info.numrec; i++)
      {

      /* we only care about Application PEs... */
      if (attr[i].type != GRMINFO_PE_TYPE_APP)
        {
        continue;
        }

      pe = attr[i].logical_pe;

      /* check if this PE is okay; otherwise, skip it */

      if ((attr[pe].available & OPERATIONAL) != OPERATIONAL)
        {
        continue;
        }

      pe_count++;

      schd_PEMAP[pe].status = STAT_AVAIL;
      schd_PEMAP[pe].apps_num = attr[pe].aps_ents;
      schd_PEMAP[pe].apps_max = attr[pe].aps_max;

      if (attr[pe].label)
        {

        /* label value stored internally to GRM as an int;
         *  convert it back to char so we can use it.
         */
        label.ilab[0] = attr[i].label;
        label.ilab[1] = 0;
        strncpy(schd_PEMAP[pe].label, label.slab, MAX_LABEL_SZ);
        schd_PEMAP[pe].label[MAX_LABEL_SZ] = '\0';
        }

      free((char *)attr);
      }
    }

  schd_MAX_NCPUS = pe_count;

  /* now get the application specific PE information */

  name[0] = SA_GRM;
  name[1] = GRM_CTL_PE_APS;
  ret = sactl(name, 2, (char *) & sa_info, sizeof(sa_info), SA_INFO, 0, 0, 0);

  if (ret < 0 || sa_info.numrec == 0)
    {
    return -1; /* no PEs defined */
    }

  if ((app = (struct pe_app *)malloc(sa_info.numrec * sa_info.recsize))
      != (struct pe_app *)0)
    {
    ret = sactl(name, 2, (char *) app,
                sa_info.numrec * sa_info.recsize, SA_READ, 0, 0, 0);

    if (ret < 0)
      {
      free((char *)app);
      return -1;
      }

    /* loop through the GRM's list of applications */
    for (i = 0, pe = 0; i < sa_info.numrec; i++)
      {
      found = 0;

      if (app[i].base_pe < 0) /* -1 == Queued */
        continue;

      /* Got a running job, now need to match it to
       * something in the PBS RunningJobs list...
       */
      for (this = jobs; this != NULL; this = this->next)
        {
        if (this->state == 'R')
          {
          if (app[i].jid == this->session)
            {
            pe = app[i].base_pe + app[i].num_pes;

            for (k = app[i].base_pe; k < pe; k++)
              {
              schd_PEMAP[k].job = this;
              schd_PEMAP[k].status = STAT_JOB_PBS;
              }

            found = 1;

            break;
            }
          }
        }

      if (!found)
        {
        /* must be a foreign job */
        pe = app[i].base_pe + app[i].num_pes;

        for (k = app[i].base_pe; k < pe; k++)
          {
          schd_PEMAP[k].status = STAT_JOB_CMD;
          }
        }
      }
    }


#ifdef DEBUG_LOTS
  /* now a sanity check... */

  for (i = 0; i < MAX_PE_MAP_SIZE; i++)
    {

    printf("[%3d] stat=%d ", i, schd_PEMAP[i].status);

    if (schd_PEMAP[i].status == STAT_DOWN)
      printf(" DOWN\n");

    else if (schd_PEMAP[i].status == STAT_JOB_PBS)
      printf("num=%d max=%d label=[%s] job=%s\n",
             schd_PEMAP[i].apps_num, schd_PEMAP[i].apps_max,
             (schd_PEMAP[i].label != NULL) ? schd_PEMAP[i].label : "None",
             schd_PEMAP[i].job->jobid);

    else if (schd_PEMAP[i].status == STAT_JOB_CMD)
      printf("num=%d max=%d label=[%s] job=FOREIGN\n",
             schd_PEMAP[i].apps_num, schd_PEMAP[i].apps_max,
             (schd_PEMAP[i].label != NULL) ? schd_PEMAP[i].label : "None");

    else
      printf("num=%d max=%d label=[%s] AVAILABLE\n",
             schd_PEMAP[i].apps_num, schd_PEMAP[i].apps_max,
             (schd_PEMAP[i].label != NULL) ? schd_PEMAP[i].label : "None");
    }

#endif /* DEBUG_LOTS */
  return 0;
  }
