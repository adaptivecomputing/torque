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

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <ctype.h>
#include "data_types.h"
#include "parse.h"
#include "constant.h"
#include "config.h"
#include "misc.h"
#include "globals.h"
#include "fairshare.h"
#include "prime.h"
#include "node_info.h"


/*
 *
 * parse_config - parse the config file and set a struct config
 *
 *   fname - file name of the config file
 *   GLOBAL: conf  - config structure
 *
 * returns success/failure
 *
 * FILE FORMAT:
 * config_name [white space] : [white space] config_value [prime_value]
 *
 * EX: sort_by: shortest_job_first prime
 */
int parse_config(char *fname)
  {
  FILE *fp;   /* file pointer to config file */
  char buf[256];  /* used to read in lines from the file */
  char *config_name;  /* parse first word of line */
  char *config_value;  /* parsed second word - right after colen (:) */
  char *prime_value;  /* optional third word */
  int num = -1;   /* used to convert string -> integer */
  char *endp;   /* used for strtol() */
  char error = 0;  /* boolean: is there an error? */
  enum prime_time prime; /* used to convert string -> prime value */
  int linenum = 0;  /* the current line number in the file */
  int pkey_num = 1;  /* number of prime time keys for multisort */
  int npkey_num = 1;  /* number of nonprime time keys for multisort */
  int i;

  if ((fp = fopen(fname, "r")) == NULL)
    {
    sprintf(buf, "Can not open file: %s", fname);
    perror(buf);
    return 0;
    }

  while (fgets(buf, 256, fp) != NULL)
    {
    linenum++;
    error = 0;
    prime = ALL;
    num = -1;
    /* skip blank lines and comments */

    if (!skip_line(buf))
      {
      if (buf[strlen(buf)-1] == '\n')
        buf[strlen(buf)-1] = '\0';

      config_name = strtok(buf, ": \t");

      config_value = strtok(NULL, " \t");

      prime_value = strtok(NULL, " \t");

      if (config_name != NULL && config_value != NULL)
        {
        /* value is true */
        if (!strcmp(config_value, "true") || !strcmp(config_value, "True")
            || !strncmp(config_value, "TRUE", 4))
          {
          num = 1;
          }

        /* value is false */
        else if (!strcmp(config_value, "false") ||
                 !strcmp(config_value, "False") || !strcmp(config_value, "FALSE"))
          {
          num = 0;
          }
        else if (isdigit((int) config_value[0]))    /* value is number */
          {
          num = strtol(config_value, &endp, 10);
          }

        /* else ;  value is a string */

        if (prime_value != NULL)
          {
          if (!strcmp(prime_value, "prime") || !strcmp(prime_value, "PRIME"))
            prime = PRIME;
          else if (!strcmp(prime_value, "non_prime") ||
                   !strcmp(prime_value, "NON_PRIME"))
            prime = NON_PRIME;
          else if (!strcmp(prime_value, "all") || !strcmp(prime_value, "ALL"))
            prime = ALL;
          else if (!strcmp(prime_value, "none") || !strcmp(prime_value, "NONE"))
            prime = NONE;
          else
            error = 1;
          }

        if (!strcmp(config_name, PARSE_ROUND_ROBIN))
          {
          if (prime == PRIME || prime == ALL)
            conf.prime_rr = num ? 1 : 0;

          if (prime == NON_PRIME || prime == ALL)
            conf.non_prime_rr = num ? 1 : 0;
          }
        else if (!strcmp(config_name, PARSE_BY_QUEUE))
          {
          if (prime == PRIME || prime == ALL)
            conf.prime_bq = num ? 1 : 0;

          if (prime == NON_PRIME || prime == ALL)
            conf.non_prime_bq = num ? 1 : 0;
          }
        else if (!strcmp(config_name, PARSE_STRICT_FIFO))
          {
          if (prime == PRIME || prime == ALL)
            conf.prime_sf = num ? 1 : 0;

          if (prime == NON_PRIME || prime == ALL)
            conf.non_prime_sf = num ? 1 : 0;
          }
        else if (!strcmp(config_name, PARSE_FAIR_SHARE))
          {
          if (prime == PRIME || prime == ALL)
            conf.prime_fs = num ? 1 : 0;

          if (prime == NON_PRIME || prime == ALL)
            conf.non_prime_fs = num ? 1 : 0;
          }
        else if (!strcmp(config_name, PARSE_LOAD_BALENCING))
          {
          if (prime == PRIME || prime == ALL)
            conf.prime_lb = num ? 1 : 0;

          if (prime == NON_PRIME || prime == ALL)
            conf.non_prime_lb = num ? 1 : 0;
          }
        else if (!strcmp(config_name, PARSE_HELP_STARVING_JOBS))
          {
          if (prime == PRIME || prime == ALL)
            conf.prime_hsv = num ? 1 : 0;

          if (prime == NON_PRIME || prime == ALL)
            conf.non_prime_hsv = num ? 1 : 0;
          }
        else if (!strcmp(config_name, PARSE_SORT_QUEUES))
          {
          if (prime == PRIME || prime == ALL)
            conf.prime_sq = num ? 1 : 0;

          if (prime == NON_PRIME || prime == ALL)
            conf.non_prime_sq = num ? 1 : 0;
          }
        else if (!strcmp(config_name, PARSE_LOAD_BALENCING_RR))
          {
          if (prime == PRIME || prime == ALL)
            conf.prime_lbrr = num ? 1 : 0;

          if (prime == NON_PRIME || prime == ALL)
            conf.non_prime_lbrr = num ? 1 : 0;
          }
        else if (!strcmp(config_name, PARSE_MAX_STARVE))
          conf.max_starve = res_to_num(config_value);
        else if (!strcmp(config_name, PARSE_HALF_LIFE))
          conf.half_life = res_to_num(config_value);
        else if (!strcmp(config_name, PARSE_SYNC_TIME))
          conf.sync_time = res_to_num(config_value);
        else if (!strcmp(config_name, PARSE_UNKNOWN_SHARES))
          conf.unknown_shares = num;
        else if (!strcmp(config_name, PARSE_LOG_FILTER))
          conf.log_filter = num;
        else if (!strcmp(config_name, PARSE_DEDICATED_PREFIX))
          {
          if (strlen(config_value) > PBS_MAXQUEUENAME)
            error = 1;
          else
            strcpy(conf.ded_prefix, config_value);
          }
        else if (!strcmp(config_name, PARSE_SORT_BY))
          {
          error = 1;

          for (i = 0; i < num_sorts; i++)
            {
            if (!strcmp(config_value, sorting_info[i].config_name))
              {
              error = 0;

              if (prime == PRIME || prime == ALL)
                conf.prime_sort[0] = sorting_info[i];

              if (prime == NON_PRIME || prime == ALL)
                conf.non_prime_sort[0] = sorting_info[i];
              }
            }
          }
        else if (!strcmp(config_name, PARSE_KEY))
          {
          error = 1;

          for (i = 0; i < num_sorts; i++)
            {
            if (!strcmp(config_value, sorting_info[i].config_name))
              {
              error = 0;

              if (((prime == PRIME || prime == ALL) && pkey_num <= num_sorts) ||
                  ((prime == NON_PRIME || prime == ALL) && npkey_num <= num_sorts)
                 )
                {
                if (prime == PRIME || prime == ALL)
                  {
                  conf.prime_sort[pkey_num] = sorting_info[i];
                  pkey_num++;
                  }

                if (prime == PRIME || prime == ALL)
                  {
                  conf.non_prime_sort[npkey_num] = sorting_info[i];
                  npkey_num++;
                  }
                }
              else
                fprintf(stderr, "Too many keys for %s , ignoring %s.\n",
                        prime == PRIME ? "prime" : "non-prime", config_value);
              }
            }
          }
        else if (!strcmp(config_name,PARSE_IGNORE_QUEUE))
          {
          if (strlen(config_value) > PBS_MAXQUEUENAME)
            error = 1;
          else
            {
            for (i = 0; i < MAX_IGNORED_QUEUES; i++)
              {
              if (conf.ignored_queues[i][0] == '\0')
                {
                strcpy(conf.ignored_queues[i],config_value);
                break;
                }
              }
            }
          }
        }
      else
        error = 1;
      }

    if (error)
      fprintf(stderr, "Error reading line %d of file %s\n%s\n", linenum, fname, buf);
    }

  fclose(fp);

  return 1;
  }

/*
 *
 *      init_config - initalize the config struture
 *
 *      returns success / failure
 *
 */
int
init_config(void)
  {
  int i;
  memset(&conf, 0, sizeof(struct config));
  memset(&cstat, 0, sizeof(struct status));

  if ((conf.prime_sort = malloc((num_sorts + 1) * sizeof(struct sort_info)))
      == NULL)
    {
    perror("Error Allocating Memory");
    return 0;
    }

  memset(conf.prime_sort, 0, (num_sorts + 1) * sizeof(struct sort_info));

  if ((conf.non_prime_sort = malloc((num_sorts + 1) * sizeof(struct sort_info)
                                   )) == NULL)
    {
    perror("Error Allocating Memory");
    return 0;
    }

  memset(conf.non_prime_sort, 0, (num_sorts + 1) *

         sizeof(struct sort_info));

  if (is_prime_time() == PRIME)
    init_prime_time();
  else
    init_non_prime_time();
  
  for (i = 0; i < MAX_IGNORED_QUEUES; i++)
    {
    if ((conf.ignored_queues[i] = (char*)malloc(PBS_MAXQUEUENAME+1)) == NULL)
      {
      perror("Error Allocating Memory");
      return 0;
      }
    memset(conf.ignored_queues[i],0,PBS_MAXQUEUENAME+1);
    }

  return 1;
  }

/*
 *
 *      reinit_config - reinitialize the conf structure so the config file can
 *                    reparsed.
 *
 *      returns success / failure
 *
 */
int
reinit_config(void)
  {
  int i;
  free(conf.prime_sort);
  free(conf.non_prime_sort);
  free_group_tree(conf.group_root);
  for (i = 0; i < MAX_IGNORED_QUEUES; i++)
    {
    free(conf.ignored_queues[i]);
    }
  return init_config();
  }
