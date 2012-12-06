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
/* $Id$ */

/* Print the published start time of all queued jobs */

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>

#include  "toolkit.h"

#include        "pbs_error.h"
#include        "pbs_ifl.h"

int connector;

void extract_times(Job *joblist);
void list_start_times();
void list_gaps(int all, int q_cpu, size_t q_mem, time_t q_time);
void create_job_schedule(Job *joblist);
static int compare_qtime(const void *e1, const void *e2);
time_t convert_time(char *time_string);
char * invert_time(time_t time_p);

/* Anticipated schedule of jobs */

struct Sinfo *JobSchedule = NULL;
int nJobSchedule;

char arch_list[40][40];
int  arch_count;
time_t schd_TimeNow;

int main(int argc, char **argv, char **envp)
  {
  char   *id = "qschedule";
  char   *pc;
  int     c, errflg = 0;
  int     ret, show_gaps = 0, query_gaps = 0, q_cpu = 0;
  time_t  q_time = 0;
  size_t  q_mem = 0;

  while ((c = getopt(argc, argv, "gc:m:w:")) != EOF)
    {
    switch (c)
      {

      case 'g':
        show_gaps = 1;
        break;

      case 'c':
        query_gaps = 1;

        while (isspace((int)*optarg)) optarg++;

        if (strlen(optarg) == 0)
          {
          fprintf(stderr, "qschedule: illegal -c value\n");
          errflg++;
          break;
          }

        q_cpu = atoi(optarg);

        break;

      case 'm':
        query_gaps = 1;

        while (isspace((int)*optarg)) optarg++;

        if (strlen(optarg) == 0)
          {
          fprintf(stderr, "qschedule: illegal -m value\n");
          errflg++;
          break;
          }

        q_mem = schd_val2byte(optarg);

        break;

      case 'w':
        query_gaps = 1;

        while (isspace((int)*optarg)) optarg++;

        if (strlen(optarg) == 0)
          {
          fprintf(stderr, "qschedule: illegal -w value\n");
          errflg++;
          break;
          }

        q_time = schd_val2sec(optarg);

        break;

      default :
        errflg++;
      }
    }

  if (errflg || (show_gaps && query_gaps))
    {
    static char usage[] =
      "usage: qschedule [-g] [-c cpus -m memory -w walltime] \n";
    fprintf(stderr, usage);
    exit(2);
    }

  time(&schd_TimeNow);

  if (show_gaps)
    list_gaps(0, 0, 0, 0);

  else if (query_gaps)
    list_gaps(query_gaps, q_cpu, q_mem, q_time);

#if 0
  else
    list_start_times();

#endif

  exit(0);
  }

void list_gaps(int all, int q_cpu, size_t q_mem, time_t q_time)
  {
  int i, j, ok, now_cpu, last_cpu, printed_cpu, time_passed;
  char queue[50], last_queue[50], printed_queue[50];
  char arch[40], last_arch[40];
  time_t timestamp, last_time, printed_time;
  size_t now_mem, last_mem, printed_mem;
  FILE *fptr;

  fptr = fopen(PREDICTED_SCHEDULE_FILE, "r");

  if (fptr == NULL)
    {
    fprintf(stderr, "ERROR: unable to open predicted schedule ");
    fprintf(stderr, "file. Notify your\n PBS Administrator.\n");
    exit(-3);
    }

  strcpy(last_arch, "EMPTY");

  strcpy(last_queue, "EMPTY");
  strcpy(printed_queue, "EMPTY");
  printed_cpu = 0;
  printed_mem = 0;
  printed_time = 0;
  time_passed = 0;
  last_time = 0;
  last_cpu  = 0;
  last_mem  = 0;

  printf("Available space or \"gaps\" between scheduled jobs:\n");

  while (fscanf(fptr, "%s %s %ld %d cpus %ld mem\n", arch, queue,
                &timestamp, &now_cpu, &now_mem) != EOF)
    {

    if (now_cpu < 0)
      now_cpu = 0;

    if (now_mem < 0)
      now_mem = 0;

    if (last_time == 0)
      {
      printf("On architecture %s:\n", arch);
      strcpy(last_arch, arch);

      last_time = timestamp;
      last_cpu  = now_cpu;
      last_mem  = now_mem;
      strcpy(last_queue, queue);
      continue;
      }

    if (last_time == timestamp && !(strcmp(last_queue, queue)))
      {
      last_cpu  = now_cpu;
      last_mem  = now_mem;
      continue;
      }

    if (strcmp(last_queue, queue) || last_time != timestamp)
      {
      if (last_cpu > 0 && last_mem > 0)
        {

        if (!(strcmp(printed_queue, last_queue)) &&
            printed_cpu == last_cpu &&
            printed_mem == last_mem)
          ;
        else
          {
          ok = 1;

          if (all)
            {
            if (q_cpu && last_cpu < q_cpu)
              ok = 0;

            if (q_mem && last_mem < q_mem)
              ok = 0;
            }

          if ((all && ok) || (!all))
            {
            /*
            time_passed = last_time - printed_time;
            if (time_passed > 0)
            printf(" for %d min.\n", time_passed/60);
            */

            printf("   At %s on queue %s: %d cpus %s memory available\n",
                   invert_time(last_time), last_queue, last_cpu,
                   schd_byte2val(last_mem));
            printed_cpu = last_cpu;
            printed_mem = last_mem;
            printed_time = last_time;
            strcpy(printed_queue, last_queue);
            }
          }
        }
      else
        {
        printed_cpu = 0;
        printed_mem = 0;
        }

      last_time = timestamp;

      last_cpu  = now_cpu;
      last_mem  = now_mem;
      strcpy(last_queue, queue);

      if (strcmp(last_arch, arch))
        {
        printf("On architecture %s:\n", arch);
        strcpy(last_arch, arch);
        time_passed = 0;
        }
      }
    }

  /* don't forget about the last line */
  if (last_cpu > 0 && last_mem > 0)
    ok = 1;

  if (all)
    {
    if (q_cpu && last_cpu < q_cpu)
      ok = 0;

    if (q_mem && last_mem < q_mem)
      ok = 0;
    }

  if ((all && ok) || (!all))
    {
    /*
    if (time_passed > 0)
        printf(" for %d min.\n", time_passed/60);
    */

    printf("   At %s on queue %s: %d cpus %s memory available\n",
           invert_time(last_time), last_queue, last_cpu,
           schd_byte2val(last_mem));
    }

  fclose(fptr);
  }

void extract_times(Job *joblist)
  {
  Job *this;
  char scratch[100] = "", scratch2[100] = "";
  char *p;

  for (this = joblist; this != NULL; this = this->next)
    {
    if (this->state == 'Q')
      {
      if (this->comment)
        {
        strncpy(scratch, this->comment, 15);
        free(this->comment);
        p = &scratch[4];
        sprintf(scratch2, "Scheduled to start at %s", p);
        this->mtime = convert_time(p);
        this->time_left = this->mtime + this->walltime;
        }
      else
        {
        strcpy(scratch2, "Start estimate not available");
        }

      this->comment = schd_strdup(scratch2);
      }
    }
  }

time_t convert_time(char *time_string)
  {
  time_t ret_time;

  struct tm *tmptime;
  char  *p;

  tmptime = localtime(&schd_TimeNow);

  p = strtok(time_string, "/");
  tmptime->tm_mon = atoi(p);
  p = strtok(NULL, " ");
  tmptime->tm_mday = atoi(p);
  p = strtok(NULL, ":");
  tmptime->tm_hour = atoi(p);
  p = strtok(NULL, "/n");
  tmptime->tm_min = atoi(p);
  tmptime->tm_sec = 0;

  ret_time = mktime(tmptime);

  return(ret_time);
  }

char * invert_time(time_t time_p)
  {

  struct tm *tmptime;
  static char  time_str[50];

  tmptime = localtime(&time_p);

  sprintf(time_str, "%0.2d/%0.2d %0.2d:%0.2d",
          tmptime->tm_mon,
          tmptime->tm_mday,
          tmptime->tm_hour,
          tmptime->tm_min);

  return(time_str);
  }

void
list_start_times(void)
  {
  int ret;
  int local_errno = 0;
  Job    *joblist = NULL, *jobtail = NULL, *new_joblist, *this;
  Batch_Status *pbs_head, *pbs_ptr;

  /* Connect to the default PBS server */
  connector = pbs_connect(NULL);

  /* Ask PBS for the list of jobs requested */
  pbs_head = pbs_selstat_err(connector, NULL, NULL, &local_errno);

  if ((pbs_head == NULL) && (local_errno))
    {
    fprintf(stderr, "ERROR: pbs_selstat failed, %d\n", local_errno);
    exit(-1);
    }

  for (pbs_ptr = pbs_head; pbs_ptr != NULL; pbs_ptr = pbs_ptr->next)
    {
    /*
     * If there is no list yet, create one. If there is already a list,
     * create a new element and place it after the current tail.  The new
     * element then becomes the tail.
     */
    new_joblist = (Job *)malloc(sizeof(Job));

    if (new_joblist == NULL)
      {
      fprintf(stderr, "ERROR: malloc of new job struct failed\n");
      joblist = NULL;
      break;
      }

    new_joblist->next = NULL;

    if (!joblist)
      {
      joblist = new_joblist;
      jobtail = joblist;
      }
    else
      {
      jobtail->next = new_joblist;
      jobtail = jobtail->next;
      }

    /*
     * 'jobtail' now points to a newly-created Job at the end of the
     * list of jobs.  Call get_jobinfo() to fill it in with the contents
     * of this PBS batch_struct description.
     */

    ret = schd_get_short_jobinfo(pbs_ptr, jobtail);

    if (ret < 0)
      {
      ;
      }
    }

  pbs_statfree(pbs_head);

  pbs_disconnect(connector);

  extract_times(joblist);
  create_job_schedule(joblist);

  if (joblist)
    {
    printf("Scheduled start time of all jobs:\n\n");

    for (this = joblist; this != NULL; this = this->next)
      {
      if (this->state == 'R')
        printf("Job %s:  %s\n", this->jobid, this->comment);
      }

    for (this = joblist; this != NULL; this = this->next)
      {
      if (this->state != 'R')
        printf("Job %s:  %s\n", this->jobid, this->comment);
      }
    }
  }


/*
 * This function takes a pointer to a struct batch_status for a job, and
 * fills in the appropriate fields of the supplied job struct.  It returns
 * the number of items that were found.
 */
int
schd_get_short_jobinfo(Batch_Status *bs, Job *job)
  {
  int       changed = 0;
  int       istrue;
  char      tmp_str[120];
  char     *id = "schd_get_short_jobinfo";
  char     *host;
  char     *p, *tmp_p, *var_p;
  AttrList *attr;

  memset((void *)job, 0, sizeof(Job));

  job->jobid = schd_strdup(bs->name);

  if (job->jobid == NULL)
    {
    return (-1);
    }

  changed ++;

  for (attr = bs->attribs; attr != NULL; attr = attr->next)
    {

    /* The state of this job. */
    if (!strcmp(attr->name, ATTR_state))
      {
      job->state = attr->value[0];
      changed ++;
      continue;
      }

    /* The comment currently assigned to this job. */
    if (!strcmp(attr->name, ATTR_comment))
      {
      job->comment = schd_strdup(attr->value);

      if (job->comment == NULL)
        {
        return (-1);
        }

      changed ++;

      continue;
      }

    /* Modified time attribute. */
    if (!strcmp(attr->name, ATTR_mtime))
      {
      /* When was the job last modified? */
      job->mtime = atoi(attr->value);
      continue;
      }

    if (!strcmp(attr->name, ATTR_l))
      {
      if (!strcmp(attr->resource, "arch"))
        {
        job->arch = schd_strdup(attr->value);
        changed ++;

        }
      else if (!strcmp(attr->resource, "mem"))
        {
        job->memory = schd_val2byte(attr->value);
        changed ++;

        }
      else if (!strcmp(attr->resource, "ncpus"))
        {
        job->ncpus = atoi(attr->value);
        changed ++;

        }
      else if (!strcmp(attr->resource, "walltime"))
        {
        job->walltime = schd_val2sec(attr->value);
        changed ++;

        }

      /* That's all for requested resources. */
      continue;
      }

    if (!strcmp(attr->name, ATTR_used))
      {
      if (!strcmp(attr->resource, "walltime"))
        {
        job->walltime_used = schd_val2sec(attr->value);
        changed ++;
        }

      /* No other interesting cases. */
      continue;
      }
    }

  /*
   * If this job is "Running" compute how
   * many seconds remain until it is completed.
   */
  if (job->state == 'R' || job->state == 'S')
    job->time_left = job->walltime - job->walltime_used;

  return (changed);
  }

void create_job_schedule(Job *joblist)
  {
  char   *id = "create_job_schedule";
  Job    *job_ptr, *this;
  int     i;

  /*
   * Destroy any previously created list.
   */

  if (JobSchedule)
    free(JobSchedule);

  JobSchedule  = NULL;

  nJobSchedule = 0;

  /*
   * Walk the list of jobs, adding Start and End entries
   * to the global JobSchedule table along the way.
   */
  for (this = joblist; this != NULL; this = this->next)
    {

    /* Start Entry */
    ++nJobSchedule;
    JobSchedule = realloc(JobSchedule, nJobSchedule * sizeof * JobSchedule);

    if (!JobSchedule)
      {
      return;
      }

    JobSchedule[nJobSchedule -1].event    = 'S';

    JobSchedule[nJobSchedule -1].time     = this->mtime;
    JobSchedule[nJobSchedule -1].cpu      = this->ncpus;
    JobSchedule[nJobSchedule -1].mem      = this->memory;
    JobSchedule[nJobSchedule -1].walltime = this->walltime + 2 * 60;

    if (this->arch)
      strcpy(JobSchedule[nJobSchedule -1].qname, this->arch);
    else
      strcpy(JobSchedule[nJobSchedule -1].qname, "");


    /* End Entry */
    ++nJobSchedule;

    JobSchedule = realloc(JobSchedule, nJobSchedule * sizeof * JobSchedule);

    if (!JobSchedule)
      {
      return;
      }

    JobSchedule[nJobSchedule -1].event  = 'E';

    JobSchedule[nJobSchedule -1].time  =
      this->mtime + this->walltime;
    JobSchedule[nJobSchedule -1].cpu  = this->ncpus;
    JobSchedule[nJobSchedule -1].mem  = this->memory;
    JobSchedule[nJobSchedule -1].walltime  = 0;

    if (this->arch)
      strcpy(JobSchedule[nJobSchedule -1].qname, this->arch);
    else
      strcpy(JobSchedule[nJobSchedule -1].qname, "");
    }

  /* now sort the table into chronological order */
  qsort(JobSchedule, nJobSchedule, sizeof *JobSchedule, compare_qtime);

  return;
  }

void
schd_print_schedule(void)
  {
  int i;

  /* print Schedule for sanity check */

  for (i = 0; i < nJobSchedule; i++)
    {
    printf("%c %ld %d cpus %ld(b)memory %ld arch=%s\n",
           JobSchedule[i].event, JobSchedule[i].time,
           JobSchedule[i].cpu, schd_byte2val(JobSchedule[i].mem),
           JobSchedule[i].walltime, JobSchedule[i].qname);
    }
  }

/*
 * qsort() function to order Job Schedule
 *
 * Order Job Schedule by date/time stamp of events,
 * descending by number of seconds queued
 */
static int
compare_qtime(const void *e1, const void *e2)
  {

  struct Sinfo    *ent1 = (struct Sinfo*)e1;

  struct Sinfo    *ent2 = (struct Sinfo*)e2;

  if (ent1->time > ent2->time)
    return 1;

  if (ent1->time < ent2->time)
    return -1;

  return 0;
  }


/* Power-of-two unit multipliers. */
#define KILO ((size_t)1024)
#define MEGA (KILO*1024)
#define GIGA (MEGA*1024)
#define TERA (GIGA*1024)

/*
 * byte2val(bytes)
 *
 * Return a pointer to a static string that is the shortest string by which
 * the number of bytes can be accurately represented. i.e.:
 *
 * 1023   -> 1023b
 * 16384   -> 16kb
 * 1048576   -> 1mb
 * 16777216  -> 16mb
 * 16777217  -> 16777217b
 * 34359738368  -> 32gb
 */

char *
schd_byte2val(size_t bytes)
  {
  int     log_1024  = 0; /* logarithm base 1024 of multiplier */
  size_t  mult      = 1; /* Initial multiplier */
  size_t  next_mult = 1024; /* multiplier of next-highest unit */
  static char string[32];
  char   *units[] =
    {
    "b",   /* bytes     */
    "kb",   /* kilobytes */
    "mb",   /* megabytes */
    "gb",   /* gigabytes */
    "tb",   /* terabytes */
    "pb",   /* petabytes */
    "eb"  /* exabytes  */
    };

  /*
   * Find the first multiplier by which the given byte count is not
   * evenly divisible.  If we overflow the next multiplier, we have
   * gone far enough.
   */

  while (bytes && (bytes % next_mult) == 0)
    {
    mult = next_mult;
    next_mult <<= 10;
    log_1024 ++;

    if (next_mult == 0)
      break;
    }

  /*
   * Make 'bytes' be the number of units being represented.
   */
  bytes /= mult;

  /*
   * Create a string from number of units, and the symbol for that unit.
   */
  sprintf(string, "%lu%s", bytes, units[log_1024]);

  /*
   * Remember: this is a static string!
   */
  return (string);
  }

/*
 * Convert seconds into "HH:MM:SS" format.  Note that the returned string
 * is a pointer to a static buffer.  The caller must copy the time string
 * into a holding buffer before the next call to sec2val() occurs.
 */

char   *
schd_sec2val(int seconds)
  {
  /* char   *id = "sec2val"; */
  static char tval[16];

  int     hours   = 0;
  int     minutes = 0;

  /* Hours */

  if (seconds >= (60 * 60))
    hours = seconds / (60 * 60);

  seconds -= (hours * (60 * 60));

  /* Minutes */
  if (seconds >= 60)
    minutes = seconds / 60;

  /* Seconds */
  seconds -= (minutes * 60);

  (void)sprintf(tval, "%2.2d:%2.2d:%2.2d", hours, minutes, seconds);

  return (&tval[0]);
  }

/* Convert an allocation value string to its equivalent value in bytes. */

size_t
schd_val2byte(char *val)
  {
  char   *id = "schd_val2byte";
  int     b = 0;
  size_t num = 0;
  char   *p;

  if (val == NULL)
    return (0);

  if (val[0] == '?')
    {
    fprintf(stderr, "error from getreq(physmem): %s: [%d]",
            val, errno);
    return (0);
    }

  b = 0;

  num = 0;
  num = strtoul(val, &p, 0);


  /* Now 'p' should point to first non-number character. */

  /* If no units given, return the number. */

  if ((p == val) || (*p == '\0'))
    return (num); /* XXX return num if p == val? */

  /* Parse the given order of magnitude. */
  switch (*p)
    {

    case 'k':

    case 'K':
      num *= KILO;
      break;

    case 'm':

    case 'M':
      num *= MEGA;
      break;

    case 'g':

    case 'G':
      num *= GIGA;
      break;

    case 't':

    case 'T':
      num *= TERA;
      break;

    case 'b':

    case 'B':
      b++;
      break;

    default:
      return (-1);
    }

  /* XXX if (b && *p != '\0') return error. */

  /* Parse the word-size unit. */
  p++;

  if ((*p != '\0') && !b)
    {
    switch (*p)
      {

      case 'b':

      case 'B':
        break;

      case 'w':

      case 'W':
        num *= NBPW; /* Multiply by size of word. */
        break;

      default:
        return (-1);
      }
    }

  /* Return the total number of bytes represented by the value. */
  return (num);
  }

/*
 *  Convert a value string from time to its equivalent value in seconds.
 */

time_t
schd_val2sec(char *val)
  {
  char   *id = "val2sec";
  char   *p1, *p2, *p3, *end, *zero = "0";
  time_t  v1, v2, v3;
  time_t  sec;
  char   *valcopy;

  p3 = NULL;

  valcopy = schd_strdup(val);

  if (valcopy == NULL)
    {
    fprintf(stderr, "ERROR: schd_strdup(val) failed.");
    return ((time_t) - 1);
    }

  /* Split string into at most 3 tokens. */
  p1 = strtok(valcopy, ":");

  p2 = strtok(NULL, ":");

  if (p2 != NULL)
    {
    p3 = strtok(NULL, ":");
    }

  /* Only seconds specified.  Shift right 2 places into p3, and zero fill */
  if (p2 == NULL)
    {
    p3 = p1;
    p2 = zero;
    p1 = zero;
    }

  /* Minutes and seconds specified.  Shift right into p3, and zero fill */
  if (p3 == NULL)
    {
    p3 = p2;
    p2 = p1;
    p1 = zero;
    }

  v1 = (time_t)strtol(p1, &end, 10);

  if (*end != '\0')
    goto error;

  if (v1 < 0)
    goto error;

  v2 = (time_t)strtol(p2, &end, 10);

  if (*end != '\0')
    goto error;

  if (v2 < 0 || v2 > 59)
    goto error;

  v3 = (time_t)strtol(p3, &end, 10);

  if (*end != '\0')
    goto error;

  if (v3 < 0 || v3 > 59)
    goto error;

  free(valcopy);

  sec = (v1 * 3600) + (v2 * 60) + v3;

  return (sec);

error:

  fprintf(stderr, "Can't parse time '%s' into seconds.\n", val);

  free(valcopy);

  return ((time_t) - 1);
  }

#define DATEFMT "%m/%d/%Y@%H:%M:%S" /* MM/DD/YYYY@HH:MM:SS */

/*
 * Convert a string of the form MM/DD/YYYY@HH:MM:SS to a time_t.  Uses
 * strptime(3) function for parsing.
 */
int
schd_val2datetime(char *string, time_t *when)
  {
  /* char   *id = "get_datetime"; */
  char   *remain;

  struct tm tm;
  time_t  then;

  /* Call the "inverse" of strftime(3) to parse the string. */
  remain = strptime(string, DATEFMT, &tm);

  /* Check for successful parsing of the date string in DATEFMT. */

  if (remain == NULL)
    return -1;

  /* Make sure the date@time was the only thing in the string. */
  if (*remain != '\0' && !isspace(*remain))
    return -1;

  /*
  * The struct tm now contains the right information.  Turn it into a
  * valid time_t.
  */
  then = mktime(&tm);

  if (then == (time_t) - 1)
    return -1;

  /* Copy the time value and return success. */
  *when = then;

  return 0;
  }

/*
 * strdup(3) is not required for POSIX compliance, so we must provide a
 * "lookalike".
 */
char *
schd_strdup(char *string)
  {
  size_t  length;
  char   *copy;

  /*
   * Allocate new space for a copy of the string contents, and a
   * trailing '\0'.
   */
  length = strlen(string) + 1;

  copy = malloc(length);

  if (copy == NULL)
    return (NULL);

  memcpy(copy, string, length);

  return(copy);
  }
