/* C++ code produced by gperf version 3.0.3 */
/* Command-line: /usr/bin/gperf -L C++ --struct-type --readonly-tables --includes --hash-function-name=pbs_attrib_hash --lookup-function-name=pbs_attrib_lookup pbs_attrib.gperf  */
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
#	include <pbs_config.h>
#endif

#include <attrib.h>

#ifndef lint
static char rcsid[]
#	ifdef __GNUC__
		__attribute__ ((unused))
#	endif
	= "$Id: pbs_attrib.gperf,v 1.4 2006/06/05 23:27:18 ciesnik Exp $";
#endif
#line 36 "pbs_attrib.gperf"
struct pbs_attrib{ const char *name; int code; };
#include <string.h>

#define TOTAL_KEYWORDS 47
#define MIN_WORD_LENGTH 5
#define MAX_WORD_LENGTH 15
#define MIN_HASH_VALUE 6
#define MAX_HASH_VALUE 75
/* maximum key range = 70, duplicates = 0 */

class Perfect_Hash
{
private:
  static inline unsigned int pbs_attrib_hash (const char *str, unsigned int len);
public:
  static const struct pbs_attrib *pbs_attrib_lookup (const char *str, unsigned int len);
};

inline unsigned int
Perfect_Hash::pbs_attrib_hash (register const char *str, register unsigned int len)
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

const struct pbs_attrib *
Perfect_Hash::pbs_attrib_lookup (register const char *str, register unsigned int len)
{
  static const struct pbs_attrib wordlist[] =
    {
      {""}, {""}, {""}, {""}, {""}, {""},
#line 72 "pbs_attrib.gperf"
      {"egroup",              ATTR_EGROUP},
      {""}, {""},
#line 79 "pbs_attrib.gperf"
      {"exec_host",           ATTR_EXECUTION_HOST},
#line 71 "pbs_attrib.gperf"
      {"euser",               ATTR_EUSER},
#line 76 "pbs_attrib.gperf"
      {"server",              ATTR_SERVER},
      {""},
#line 69 "pbs_attrib.gperf"
      {"substate",            ATTR_JOB_SUBMIT_STATE},
#line 63 "pbs_attrib.gperf"
      {"job_state",           ATTR_JOB_STATE},
#line 65 "pbs_attrib.gperf"
      {"etime",               ATTR_ETIME},
#line 81 "pbs_attrib.gperf"
      {"max_running",         ATTR_MAX_RUNNING},
#line 46 "pbs_attrib.gperf"
      {"stagein",             ATTR_STAGEIN},
#line 47 "pbs_attrib.gperf"
      {"stageout",            ATTR_STAGEOUT},
#line 67 "pbs_attrib.gperf"
      {"resources_used",      ATTR_RESOURCES_USED},
#line 85 "pbs_attrib.gperf"
      {"session_id",          ATTR_SESSION_ID},
#line 68 "pbs_attrib.gperf"
      {"exit_status",         ATTR_EXIT_STATUS},
      {""},
#line 39 "pbs_attrib.gperf"
      {"Job_Name",            ATTR_JOB_NAME},
#line 70 "pbs_attrib.gperf"
      {"Job_Owner",           ATTR_JOB_OWNER},
#line 77 "pbs_attrib.gperf"
      {"queue",               ATTR_QUEUE},
#line 86 "pbs_attrib.gperf"
      {"alt_id",              ATTR_ALT_ID},
      {""},
#line 40 "pbs_attrib.gperf"
      {"Variable_List",       ATTR_ENV},
#line 56 "pbs_attrib.gperf"
      {"Rerunable",           ATTR_RERUNABLE},
#line 78 "pbs_attrib.gperf"
      {"queue_rank",          ATTR_QUEUE_RANK},
#line 60 "pbs_attrib.gperf"
      {"depend",              ATTR_DEPENDENCIES},
      {""},
#line 54 "pbs_attrib.gperf"
      {"Priority",            ATTR_PRIORITY},
#line 43 "pbs_attrib.gperf"
      {"Join_Path",           ATTR_JOIN_FILES},
#line 66 "pbs_attrib.gperf"
      {"qtime",               ATTR_QTIME},
#line 61 "pbs_attrib.gperf"
      {"interactive",         ATTR_INTERACTIVE},
      {""},
#line 53 "pbs_attrib.gperf"
      {"Resource_List",       ATTR_RESOURCES},
#line 73 "pbs_attrib.gperf"
      {"hop_count",           ATTR_HOP_COUNT},
#line 80 "pbs_attrib.gperf"
      {"total_jobs",          ATTR_TOTAL_JOBS},
#line 55 "pbs_attrib.gperf"
      {"destination",         ATTR_DESTINATION_QUEUE},
#line 82 "pbs_attrib.gperf"
      {"comment",             ATTR_COMMENT},
#line 84 "pbs_attrib.gperf"
      {"hashname",            ATTR_HASHNAME},
#line 44 "pbs_attrib.gperf"
      {"Mail_User",           ATTR_EMAIL},
#line 75 "pbs_attrib.gperf"
      {"sched_hint",          ATTR_SCHEDULER_HINT},
#line 45 "pbs_attrib.gperf"
      {"Mail_Points",         ATTR_MAIL_POINTS},
#line 58 "pbs_attrib.gperf"
      {"Account_Name",        ATTR_REMOTE_USER},
#line 74 "pbs_attrib.gperf"
      {"security",            ATTR_SECURITY},
#line 57 "pbs_attrib.gperf"
      {"User_List",           ATTR_USER_LIST},
#line 64 "pbs_attrib.gperf"
      {"ctime",               ATTR_CTIME},
#line 83 "pbs_attrib.gperf"
      {"cookie",              ATTR_JOB_COOKIE},
      {""}, {""}, {""},
#line 42 "pbs_attrib.gperf"
      {"Error_Path",          ATTR_ERROR_PATH},
#line 41 "pbs_attrib.gperf"
      {"Output_Path",         ATTR_OUTPUT_PATH},
      {""}, {""},
#line 49 "pbs_attrib.gperf"
      {"Execution_Time",      ATTR_SOFT_CPU_TIME_LIMIT},
#line 51 "pbs_attrib.gperf"
      {"group_list",          ATTR_GROUP_LIST},
#line 50 "pbs_attrib.gperf"
      {"Checkpoint",          ATTR_CHECKPOINT},
      {""}, {""}, {""},
#line 59 "pbs_attrib.gperf"
      {"Shell_Path_List",     ATTR_SHELL},
      {""}, {""}, {""}, {""},
#line 48 "pbs_attrib.gperf"
      {"Keep_Files",          ATTR_KEEP_FILES},
      {""}, {""}, {""}, {""},
#line 52 "pbs_attrib.gperf"
      {"Hold_Types",          ATTR_HOLD_TYPES}
    };

  if (len <= MAX_WORD_LENGTH && len >= MIN_WORD_LENGTH)
    {
      register int key = pbs_attrib_hash (str, len);

      if (key <= MAX_HASH_VALUE && key >= 0)
        {
          register const char *s = wordlist[key].name;

          if (*str == *s && !strcmp (str + 1, s + 1))
            return &wordlist[key];
        }
    }
  return 0;
}
