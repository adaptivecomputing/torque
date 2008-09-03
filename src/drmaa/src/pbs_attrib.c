/* ANSI-C code produced by gperf version 3.0.1 */
/* Command-line: gperf --language=ANSI-C --struct-type --readonly-tables --includes --pic --hash-function-name=pbs_attrib_hash --lookup-function-name=pbs_attrib_lookup pbs_attrib.gperf  */
/* Computed positions: -k'1,3' */

#if !((' ' == 32) && ('!' == 33) && ('"' == 34) && ('#' == 35) \
      && ('%' == 37) && ('&' == 38) && ('\'' == 39) && ('(' == 40) \
      && (')' == 41) && ('*' == 42) && ('+' == 43) && (',' == 44) \
      && ('-' == 45) && ('.' == 46) && ('/' == 47) && ('0' == 48) \
      && ('1' == 49) && ('2' == 50) && ('3' == 51) && ('4' == 52) \
      && ('5' == 53) && ('6' == 54) && ('7' == 55) && ('8' == 56) \
      && ('9' == 57) && (':' == 58) && (';' == 59) && ('<' == 60) \
      && ('=' == 61) && ('>' == 62) && ('?' == 63) && ('A' == 65) \
      && ('B' == 66) && ('C' == 67) && ('D' == 68) && ('E' == 69) \
      && ('F' == 70) && ('G' == 71) && ('H' == 72) && ('I' == 73) \
      && ('J' == 74) && ('K' == 75) && ('L' == 76) && ('M' == 77) \
      && ('N' == 78) && ('O' == 79) && ('P' == 80) && ('Q' == 81) \
      && ('R' == 82) && ('S' == 83) && ('T' == 84) && ('U' == 85) \
      && ('V' == 86) && ('W' == 87) && ('X' == 88) && ('Y' == 89) \
      && ('Z' == 90) && ('[' == 91) && ('\\' == 92) && (']' == 93) \
      && ('^' == 94) && ('_' == 95) && ('a' == 97) && ('b' == 98) \
      && ('c' == 99) && ('d' == 100) && ('e' == 101) && ('f' == 102) \
      && ('g' == 103) && ('h' == 104) && ('i' == 105) && ('j' == 106) \
      && ('k' == 107) && ('l' == 108) && ('m' == 109) && ('n' == 110) \
      && ('o' == 111) && ('p' == 112) && ('q' == 113) && ('r' == 114) \
      && ('s' == 115) && ('t' == 116) && ('u' == 117) && ('v' == 118) \
      && ('w' == 119) && ('x' == 120) && ('y' == 121) && ('z' == 122) \
      && ('{' == 123) && ('|' == 124) && ('}' == 125) && ('~' == 126))
/* The character set is not based on ISO-646.  */
#error "gperf generated tables don't work with this execution character set. Please report a bug to <bug-gnu-gperf@gnu.org>."
#endif

#line 1 "pbs_attrib.gperf"

/* $Id: pbs_attrib.gperf,v 1.4 2006/06/05 23:27:18 ciesnik Exp $ */
/*
 *  DRMAA library for Torque/PBS
 *  Copyright (C) 2006  Poznan Supercomputing and Networking Center
 *                      DSP team <dsp-devel@hedera.man.poznan.pl>
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License as published by the Free Software Foundation; either
 *  version 2.1 of the License, or (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this library; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */
#ifdef HAVE_CONFIG_H
# include <pbs_config.h>
#endif

#include <attrib.h>

#ifndef lint
static char rcsid[]
# ifdef __GNUC__
__attribute__((unused))
# endif
= "$Id: pbs_attrib.gperf,v 1.4 2006/06/05 23:27:18 ciesnik Exp $";
#endif
#line 36 "pbs_attrib.gperf"

struct pbs_attrib
  {
  const char *name;
  int code;
  };

#include <string.h>

#define TOTAL_KEYWORDS 47
#define MIN_WORD_LENGTH 5
#define MAX_WORD_LENGTH 15
#define MIN_HASH_VALUE 6
#define MAX_HASH_VALUE 75
/* maximum key range = 70, duplicates = 0 */

#ifdef __GNUC__
__inline
#else
#ifdef __cplusplus
inline
#endif
#endif
static unsigned int
pbs_attrib_hash(register const char *str, register unsigned int len)
  {
  static const unsigned char asso_values[] =
    {
    76, 76, 76, 76, 76, 76, 76, 76, 76, 76,
    76, 76, 76, 76, 76, 76, 76, 76, 76, 76,
    76, 76, 76, 76, 76, 76, 76, 76, 76, 76,
    76, 76, 76, 76, 76, 76, 76, 76, 76, 76,
    76, 76, 76, 76, 76, 76, 76, 76, 76, 76,
    76, 76, 76, 76, 76, 76, 76, 76, 76, 76,
    76, 76, 76, 76, 76,  0, 76, 51, 76, 45,
    76, 76, 35, 76, 15, 60, 76, 25, 76, 30,
    15, 76, 20, 50, 76, 40, 15, 76, 76, 76,
    76, 76, 76, 76, 76, 76, 76,  5,  0, 35,
    25,  0, 76, 40, 30, 10,  5, 76, 30,  0,
    76, 10,  0, 20,  0,  5, 15, 76, 76, 76,
    5, 76, 76, 76, 76, 76, 76, 76, 76, 76,
    76, 76, 76, 76, 76, 76, 76, 76, 76, 76,
    76, 76, 76, 76, 76, 76, 76, 76, 76, 76,
    76, 76, 76, 76, 76, 76, 76, 76, 76, 76,
    76, 76, 76, 76, 76, 76, 76, 76, 76, 76,
    76, 76, 76, 76, 76, 76, 76, 76, 76, 76,
    76, 76, 76, 76, 76, 76, 76, 76, 76, 76,
    76, 76, 76, 76, 76, 76, 76, 76, 76, 76,
    76, 76, 76, 76, 76, 76, 76, 76, 76, 76,
    76, 76, 76, 76, 76, 76, 76, 76, 76, 76,
    76, 76, 76, 76, 76, 76, 76, 76, 76, 76,
    76, 76, 76, 76, 76, 76, 76, 76, 76, 76,
    76, 76, 76, 76, 76, 76, 76, 76, 76, 76,
    76, 76, 76, 76, 76, 76
    };
  return len + asso_values[(unsigned char)str[2]] + asso_values[(unsigned char)str[0]];
  }

struct stringpool_t
  {
  char stringpool_str6[sizeof("egroup")];
  char stringpool_str9[sizeof("exec_host")];
  char stringpool_str10[sizeof("euser")];
  char stringpool_str11[sizeof("server")];
  char stringpool_str13[sizeof("substate")];
  char stringpool_str14[sizeof("job_state")];
  char stringpool_str15[sizeof("etime")];
  char stringpool_str16[sizeof("max_running")];
  char stringpool_str17[sizeof("stagein")];
  char stringpool_str18[sizeof("stageout")];
  char stringpool_str19[sizeof("resources_used")];
  char stringpool_str20[sizeof("session_id")];
  char stringpool_str21[sizeof("exit_status")];
  char stringpool_str23[sizeof("Job_Name")];
  char stringpool_str24[sizeof("Job_Owner")];
  char stringpool_str25[sizeof("queue")];
  char stringpool_str26[sizeof("alt_id")];
  char stringpool_str28[sizeof("Variable_List")];
  char stringpool_str29[sizeof("Rerunable")];
  char stringpool_str30[sizeof("queue_rank")];
  char stringpool_str31[sizeof("depend")];
  char stringpool_str33[sizeof("Priority")];
  char stringpool_str34[sizeof("Join_Path")];
  char stringpool_str35[sizeof("qtime")];
  char stringpool_str36[sizeof("interactive")];
  char stringpool_str38[sizeof("Resource_List")];
  char stringpool_str39[sizeof("hop_count")];
  char stringpool_str40[sizeof("total_jobs")];
  char stringpool_str41[sizeof("destination")];
  char stringpool_str42[sizeof("comment")];
  char stringpool_str43[sizeof("hashname")];
  char stringpool_str44[sizeof("Mail_User")];
  char stringpool_str45[sizeof("sched_hint")];
  char stringpool_str46[sizeof("Mail_Points")];
  char stringpool_str47[sizeof("Account_Name")];
  char stringpool_str48[sizeof("security")];
  char stringpool_str49[sizeof("User_List")];
  char stringpool_str50[sizeof("ctime")];
  char stringpool_str51[sizeof("cookie")];
  char stringpool_str55[sizeof("Error_Path")];
  char stringpool_str56[sizeof("Output_Path")];
  char stringpool_str59[sizeof("Execution_Time")];
  char stringpool_str60[sizeof("group_list")];
  char stringpool_str61[sizeof("Checkpoint")];
  char stringpool_str65[sizeof("Shell_Path_List")];
  char stringpool_str70[sizeof("Keep_Files")];
  char stringpool_str75[sizeof("Hold_Types")];
  };

static const struct stringpool_t stringpool_contents =
  {
  "egroup",
  "exec_host",
  "euser",
  "server",
  "substate",
  "job_state",
  "etime",
  "max_running",
  "stagein",
  "stageout",
  "resources_used",
  "session_id",
  "exit_status",
  "Job_Name",
  "Job_Owner",
  "queue",
  "alt_id",
  "Variable_List",
  "Rerunable",
  "queue_rank",
  "depend",
  "Priority",
  "Join_Path",
  "qtime",
  "interactive",
  "Resource_List",
  "hop_count",
  "total_jobs",
  "destination",
  "comment",
  "hashname",
  "Mail_User",
  "sched_hint",
  "Mail_Points",
  "Account_Name",
  "security",
  "User_List",
  "ctime",
  "cookie",
  "Error_Path",
  "Output_Path",
  "Execution_Time",
  "group_list",
  "Checkpoint",
  "Shell_Path_List",
  "Keep_Files",
  "Hold_Types"
  };

#define stringpool ((const char *) &stringpool_contents)
#ifdef __GNUC__
__inline
#endif

const struct pbs_attrib *
      pbs_attrib_lookup(register const char *str, register unsigned int len)
  {

  static const struct pbs_attrib wordlist[] =
    {
      {
      -1
      }, { -1}, { -1}, { -1}, { -1}, { -1},
#line 72 "pbs_attrib.gperf"
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str6,              ATTR_EGROUP},
    { -1}, { -1},
#line 79 "pbs_attrib.gperf"
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str9,           ATTR_EXECUTION_HOST},
#line 71 "pbs_attrib.gperf"
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str10,               ATTR_EUSER},
#line 76 "pbs_attrib.gperf"
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str11,              ATTR_SERVER},
    { -1},
#line 69 "pbs_attrib.gperf"
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str13,            ATTR_JOB_SUBMIT_STATE},
#line 63 "pbs_attrib.gperf"
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str14,           ATTR_JOB_STATE},
#line 65 "pbs_attrib.gperf"
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str15,               ATTR_ETIME},
#line 81 "pbs_attrib.gperf"
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str16,         ATTR_MAX_RUNNING},
#line 46 "pbs_attrib.gperf"
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str17,             ATTR_STAGEIN},
#line 47 "pbs_attrib.gperf"
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str18,            ATTR_STAGEOUT},
#line 67 "pbs_attrib.gperf"
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str19,      ATTR_RESOURCES_USED},
#line 85 "pbs_attrib.gperf"
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str20,          ATTR_SESSION_ID},
#line 68 "pbs_attrib.gperf"
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str21,         ATTR_EXIT_STATUS},
    { -1},
#line 39 "pbs_attrib.gperf"
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str23,            ATTR_JOB_NAME},
#line 70 "pbs_attrib.gperf"
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str24,           ATTR_JOB_OWNER},
#line 77 "pbs_attrib.gperf"
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str25,               ATTR_QUEUE},
#line 86 "pbs_attrib.gperf"
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str26,              ATTR_ALT_ID},
    { -1},
#line 40 "pbs_attrib.gperf"
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str28,       ATTR_ENV},
#line 56 "pbs_attrib.gperf"
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str29,           ATTR_RERUNABLE},
#line 78 "pbs_attrib.gperf"
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str30,          ATTR_QUEUE_RANK},
#line 60 "pbs_attrib.gperf"
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str31,              ATTR_DEPENDENCIES},
    { -1},
#line 54 "pbs_attrib.gperf"
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str33,            ATTR_PRIORITY},
#line 43 "pbs_attrib.gperf"
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str34,           ATTR_JOIN_FILES},
#line 66 "pbs_attrib.gperf"
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str35,               ATTR_QTIME},
#line 61 "pbs_attrib.gperf"
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str36,         ATTR_INTERACTIVE},
    { -1},
#line 53 "pbs_attrib.gperf"
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str38,       ATTR_RESOURCES},
#line 73 "pbs_attrib.gperf"
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str39,           ATTR_HOP_COUNT},
#line 80 "pbs_attrib.gperf"
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str40,          ATTR_TOTAL_JOBS},
#line 55 "pbs_attrib.gperf"
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str41,         ATTR_DESTINATION_QUEUE},
#line 82 "pbs_attrib.gperf"
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str42,             ATTR_COMMENT},
#line 84 "pbs_attrib.gperf"
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str43,            ATTR_HASHNAME},
#line 44 "pbs_attrib.gperf"
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str44,           ATTR_EMAIL},
#line 75 "pbs_attrib.gperf"
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str45,          ATTR_SCHEDULER_HINT},
#line 45 "pbs_attrib.gperf"
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str46,         ATTR_MAIL_POINTS},
#line 58 "pbs_attrib.gperf"
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str47,        ATTR_REMOTE_USER},
#line 74 "pbs_attrib.gperf"
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str48,            ATTR_SECURITY},
#line 57 "pbs_attrib.gperf"
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str49,           ATTR_USER_LIST},
#line 64 "pbs_attrib.gperf"
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str50,               ATTR_CTIME},
#line 83 "pbs_attrib.gperf"
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str51,              ATTR_JOB_COOKIE},
    { -1}, { -1}, { -1},
#line 42 "pbs_attrib.gperf"
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str55,          ATTR_ERROR_PATH},
#line 41 "pbs_attrib.gperf"
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str56,         ATTR_OUTPUT_PATH},
    { -1}, { -1},
#line 49 "pbs_attrib.gperf"
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str59,      ATTR_SOFT_CPU_TIME_LIMIT},
#line 51 "pbs_attrib.gperf"
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str60,          ATTR_GROUP_LIST},
#line 50 "pbs_attrib.gperf"
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str61,          ATTR_CHECKPOINT},
    { -1}, { -1}, { -1},
#line 59 "pbs_attrib.gperf"
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str65,     ATTR_SHELL},
    { -1}, { -1}, { -1}, { -1},
#line 48 "pbs_attrib.gperf"
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str70,          ATTR_KEEP_FILES},
    { -1}, { -1}, { -1}, { -1},
#line 52 "pbs_attrib.gperf"
    {(int)(long)&((struct stringpool_t *)0)->stringpool_str75,          ATTR_HOLD_TYPES}
    };

  if (len <= MAX_WORD_LENGTH && len >= MIN_WORD_LENGTH)
    {
    register int key = pbs_attrib_hash(str, len);

    if (key <= MAX_HASH_VALUE && key >= 0)
      {
      register int o = wordlist[key].name;

      if (o >= 0)
        {
        register const char *s = o + stringpool;

        if (*str == *s && !strcmp(str + 1, s + 1))
          return &wordlist[key];
        }
      }
    }

  return 0;
  }
