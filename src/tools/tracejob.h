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


#ifndef TRACEJOB_H
#define TRACEJOB_H

/* Symbolic constants */

/* default number of columns on a terminal */
#ifndef DEFAULT_WRAP
#define DEFAULT_WRAP 80
#endif

/*
 * filter excessive log entires
 */
#ifndef FILTER_EXCESSIVE
#define FILTER_EXCESSIVE
#endif

/* if filter excessive if turned on and there are at least this many
 * log entires, its considered is considered excessive
 */
#ifndef EXCESSIVE_COUNT
#define EXCESSIVE_COUNT 15
#endif

/* number of entries to start */
#ifndef DEFAULT_LOG_LINES
#define DEFAULT_LOG_LINES 1024
#endif

/* Maximum number of log files to check per day */
#ifndef MAX_LOG_FILES_PER_DAY
#define MAX_LOG_FILES_PER_DAY 1024
#endif

#define SECONDS_IN_DAY 86400

/* indicies into the mid_path array */
enum index
  {
  IND_ACCT = 0,
  IND_SERVER = 1,
  IND_MOM = 2,
  IND_SCHED = 3
  };

/* fields of a log entry */
enum field
  {
  FLD_DATE = 0,
  FLD_EVENT = 1,
  FLD_OBJ = 2,
  FLD_TYPE = 3,
  FLD_NAME = 4,
  FLD_MSG = 5
  };

/* A PBS log entry */

struct log_entry
  {
  char *date;           /* date of log entry */
  time_t date_time;     /* number of seconds from the epoch to date */
  char *event;          /* event type */
  char *obj;            /* what entity is writing the log */
  char *type;           /* type of object Job/Svr/etc */
  char *name;           /* name of object */
  char *msg;            /* log message */
  char log_file;        /* What log file */
  int lineno;  /* what line in the file.  used to stabilize the sort */

unsigned no_print:
  1; /* whether or not to print the message */
  /* A=accounting S=server M=Mom L=Scheduler */
  };

/* prototypes */
int sort_by_date(const void *v1, const void *v2);
int parse_log(FILE *, char *, int);
char *strip_path(char *path);
void free_log_entry(struct log_entry *lg);
void line_wrap(char *line, int start, int end);
int  log_path(char *path, int index, struct tm *tm_ptr, char *filenames[]);
void alloc_more_space();
void filter_excess(int threshold);
int sort_by_message(const void *v1, const void *v2);
int get_cols(void);


/* used by getopt(3) */
extern char *optarg;
extern int optind;

#endif
