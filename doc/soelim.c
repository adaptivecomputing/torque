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
 * soelim - eliminate .so lines from troff documents
 *
 * This simple utility is provided to eliminate the .so lines
 * from troff documents by merging in the text from the referenced
 * source file.  Some systems already come with soelim, others do not.
 *
 * Usage: soelim [-d include [-d include]...] inputfile
 * The named input file is opened processed.  Any .so line in the
 * input is replace by the lines from the file.
 *
 * Output is written to standard out
 */

#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include <stdlib.h>

char *sodir[] = { 0, 0, 0, 0, 0 };

int
main(int argc, char *argv[])
  {
  int   dofile(FILE *);
  int   dirct = 0;
  FILE *filein;
  int   i;
  extern int   optind;
  extern char *optarg;

  while ((i = getopt(argc, argv, "d:")) != -1)
    switch (i)
      {

      case(int)'d':
              sodir[dirct++] = optarg;
        break;

      default:
        fprintf(stderr, "Usage: %s [-d include_dir] [filename ...]\n", argv[0]);
        return 1;
      }

  if (optind < argc)
  {
    for (i = optind; i < argc; ++i)
      {

      filein = fopen(argv[i], "r");

      if (filein == NULL)
        {
        fprintf(stderr, "soelim: unable to open %s\n", argv[i]);
        return (1);
        }

      if (dofile(filein) != 0)
        return (1);
      }
    }
  else
    {
    return (dofile(stdin));
    }

  return (0);
  }

int dofile(filein)
FILE *filein;
  {
  char buf[256];
  int  dirindex;
  int  found;
  char newfile[1024];
  FILE *newin;
  char *pc;
  char *pdir;
  char *pso;

  while (fgets(buf, 255, filein) != NULL)
    {
    if (strncmp(buf, ".so ", 4) == 0)
      {
      dirindex = 0;
      found = 0;
      pso = buf + 4;

      while (*pso == ' ')
        pso++;

      pc = pso;

      while (isspace((int)*pc) == 0)
        pc++;

      *pc = '\0';

      while ((pdir = sodir[dirindex]) != NULL)
        {
        (void)strcpy(newfile, pdir);
        (void)strcat(newfile, "/");
        (void)strcat(newfile, pso);

        if ((newin = fopen(newfile, "r")) != NULL)
          {
          found = 1;
          dofile(newin);
          break;
          }

        dirindex++;
        }

      if (found == 0)
        {
        /* try local directory */

        (void)strcpy(newfile, pso);

        if ((newin = fopen(newfile, "r")) != NULL)
          {
          dofile(newin);
          }
        else
          {
          fprintf(stderr, "Unable to find include file %s\n", pso);
          exit(1);
          }
        }

      }
    else
      {
      fputs(buf, stdout);
      }
    }

  fclose(filein);

  return (0);
  }
