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
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*
 * Used by PBS on IBM SP systems to set environment for tasks spawned by poe as:
 *
 * pbspd job_key window_id_file cmd args
 *
 * This results in the execution of "cmd" with the arguments "args"
 * The environ variable MP_PARTITION is set to "job_key"
 * The environ variable MP_CHILD is used to index into the text file
 * window_id_file to obtain the window id which becomes the value of the
 * environ variable MP_MPI_NETWORK.
 */

#define SWLINELEN 5

char
get_winid(char *winidfile, char *mpchild)
  {
  FILE *wf;
  int   tid;
  char  buf[SWLINELEN+1];
  char  swid = '\0';


  if ((wf = fopen(winidfile, "r")) == NULL)
    return '\0';

  tid = atoi(mpchild) + 1;

  while (tid--)
    {
    if (fgets(buf, SWLINELEN, wf) == NULL)
      {
      fclose(wf);
      return '\0';
      }
    }

  fclose(wf);

  swid = buf[0];
  return (swid);
  }


main(int argc, char **argv)
  {
  int      len;
  char      *jk;
  char     *mpchild;
  char     winid;

  static char *job_key = "MP_PARTITION=";
  static char job_win[15+SWLINELEN] = "MP_MPI_NETWORK=";

  /* Check that calling args are correct */

  if (argc < 4)
    {
    fprintf(stderr, "%s: insufficent arguments\n", argv[0]);
    return 1;
    }

  if ((mpchild = getenv("MP_CHILD")) == NULL)
    {
    fprintf(stderr, "%s: MP_CHILD not in environment\n", argv[0]);
    return 1;
    }

  /*
   * Build up job key and window variables and put into environment.
   * The job key is the first argument.
   * The switch window_id_file is the second argument.
   */

  len = strlen(argv[1]) + strlen(job_key) + 1;

  if ((jk  = (char *)malloc(len)) == 0)
    {
    fprintf(stderr, "%s: cannot allocate memory\n", argv[0]);
    return 1;
    }

  (void)strcpy(jk, job_key);
  (void)strcat(jk, argv[1]);
  putenv(jk);


  winid = get_winid(argv[2], mpchild);

  if (winid == '\0')
    {
    fprintf(stderr, "%s: unable to obtain switch window id\n",
            argv[0]);
    return 1;
    }

  len = strlen(job_win);

  job_win[len] = winid;
  job_win[len+1] = '\0';
  putenv(job_win);

  /* now exec the real program with its args */

  if (execvp(argv[3], &argv[3]) < 0)
    {
    fprintf(stderr, "%s: unable to exec %s\n", argv[0], argv[3]);
    perror("error:");
    return 1;
    }

  return 0;
  }
