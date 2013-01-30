/* C++ code produced by gperf version 3.0.3 */
/* Command-line: /usr/bin/gperf -L C++ --struct-type --readonly-tables --includes --hash-function-name=drmaa_attrib_hash --lookup-function-name=drmaa_attrib_lookup drmaa_attrib.gperf  */
/* Computed positions: -k'11,16' */

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

#line 1 "drmaa_attrib.gperf"

/* $Id: drmaa_attrib.gperf,v 1.4 2006/06/05 23:27:17 ciesnik Exp $ */
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
	= "$Id: drmaa_attrib.gperf,v 1.4 2006/06/05 23:27:17 ciesnik Exp $";
#endif
#line 36 "drmaa_attrib.gperf"
struct drmaa_attrib { const char *name; int code; };
#include <string.h>

#define TOTAL_KEYWORDS 21
#define MIN_WORD_LENGTH 8
#define MAX_WORD_LENGTH 26
#define MIN_HASH_VALUE 8
#define MAX_HASH_VALUE 46
/* maximum key range = 39, duplicates = 0 */

class Perfect_Hash
{
private:
  static inline unsigned int drmaa_attrib_hash (const char *str, unsigned int len);
public:
  static const struct drmaa_attrib *drmaa_attrib_lookup (const char *str, unsigned int len);
};

inline unsigned int
Perfect_Hash::drmaa_attrib_hash (register const char *str, register unsigned int len)
{
  static const unsigned char asso_values[] =
    {
      47, 47, 47, 47, 47, 47, 47, 47, 47, 47,
      47, 47, 47, 47, 47, 47, 47, 47, 47, 47,
      47, 47, 47, 47, 47, 47, 47, 47, 47, 47,
      47, 47, 47, 47, 47, 47, 47, 47, 47, 47,
      47, 47, 47, 47, 47, 47, 47, 47, 47, 47,
      47, 47, 47, 47, 47, 47, 47, 47, 47, 47,
      47, 47, 47, 47, 47, 47, 47, 47, 47, 47,
      47, 47, 47, 47, 47, 47, 47, 47, 47, 47,
      47, 47, 47, 47, 47, 47, 47, 47, 47, 47,
      47, 47, 47, 47, 47, 18, 47,  0, 47,  0,
      47, 47, 20,  0, 13, 10,  5,  0,  5, 47,
       5, 47,  0, 47, 15,  0,  0,  0,  0, 47,
      47, 47, 47, 47, 47, 47, 47, 47, 47, 47,
      47, 47, 47, 47, 47, 47, 47, 47, 47, 47,
      47, 47, 47, 47, 47, 47, 47, 47, 47, 47,
      47, 47, 47, 47, 47, 47, 47, 47, 47, 47,
      47, 47, 47, 47, 47, 47, 47, 47, 47, 47,
      47, 47, 47, 47, 47, 47, 47, 47, 47, 47,
      47, 47, 47, 47, 47, 47, 47, 47, 47, 47,
      47, 47, 47, 47, 47, 47, 47, 47, 47, 47,
      47, 47, 47, 47, 47, 47, 47, 47, 47, 47,
      47, 47, 47, 47, 47, 47, 47, 47, 47, 47,
      47, 47, 47, 47, 47, 47, 47, 47, 47, 47,
      47, 47, 47, 47, 47, 47, 47, 47, 47, 47,
      47, 47, 47, 47, 47, 47, 47, 47, 47, 47,
      47, 47, 47, 47, 47, 47, 47
    };
  register int hval = len;

  switch (hval)
    {
      default:
        hval += asso_values[(unsigned char)str[15]+1];
      /*FALLTHROUGH*/
      case 15:
      case 14:
      case 13:
      case 12:
      case 11:
        hval += asso_values[(unsigned char)str[10]];
      /*FALLTHROUGH*/
      case 10:
      case 9:
      case 8:
        break;
    }
  return hval;
}

const struct drmaa_attrib *
Perfect_Hash::drmaa_attrib_lookup (register const char *str, register unsigned int len)
{
  static const struct drmaa_attrib wordlist[] =
    {
      {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""},
#line 47 "drmaa_attrib.gperf"
      {"drmaa_wd",                    ATTR_JOB_WORKING_DIR},
      {""}, {""},
#line 41 "drmaa_attrib.gperf"
      {"drmaa_v_env",                 ATTR_ENV},
#line 40 "drmaa_attrib.gperf"
      {"drmaa_v_argv",                ATTR_ARGV},
#line 48 "drmaa_attrib.gperf"
      {"drmaa_v_email",               ATTR_EMAIL},
#line 51 "drmaa_attrib.gperf"
      {"drmaa_js_state",              ATTR_JOB_SUBMIT_STATE},
      {""},
#line 55 "drmaa_attrib.gperf"
      {"drmaa_wct_slimit",            ATTR_SOFT_WCT_LIMIT},
#line 43 "drmaa_attrib.gperf"
      {"drmaa_output_path",           ATTR_OUTPUT_PATH},
#line 57 "drmaa_attrib.gperf"
      {"drmaa_job_category",          ATTR_JOB_CATEGORY},
#line 38 "drmaa_attrib.gperf"
      {"drmaa_job_name",              ATTR_JOB_NAME},
#line 46 "drmaa_attrib.gperf"
      {"drmaa_transfer_files",        ATTR_TRANSFER_FILES},
#line 53 "drmaa_attrib.gperf"
      {"drmaa_duration_slimit",       ATTR_SOFT_CPU_TIME_LIMIT},
#line 49 "drmaa_attrib.gperf"
      {"drmaa_block_email",           ATTR_BLOCK_EMAIL},
      {""},
#line 56 "drmaa_attrib.gperf"
      {"drmaa_deadline_time",         ATTR_DEADLINE_TIME},
#line 39 "drmaa_attrib.gperf"
      {"drmaa_remote_command",        ATTR_JOB_PATH},
#line 42 "drmaa_attrib.gperf"
      {"drmaa_input_path",            ATTR_INPUT_PATH},
      {""}, {""},
#line 54 "drmaa_attrib.gperf"
      {"drmaa_wct_hlimit",            ATTR_HARD_WCT_LIMIT},
      {""},
#line 52 "drmaa_attrib.gperf"
      {"drmaa_duration_hlimit",       ATTR_HARD_CPU_TIME_LIMIT},
      {""}, {""},
#line 45 "drmaa_attrib.gperf"
      {"drmaa_join_files",            ATTR_JOIN_FILES},
      {""},
#line 50 "drmaa_attrib.gperf"
      {"drmaa_start_time",            ATTR_START_TIME},
      {""}, {""}, {""}, {""},
#line 44 "drmaa_attrib.gperf"
      {"drmaa_error_path",            ATTR_ERROR_PATH},
      {""}, {""}, {""}, {""},
#line 58 "drmaa_attrib.gperf"
      {"drmaa_native_specification",  ATTR_NATIVE}
    };

  if (len <= MAX_WORD_LENGTH && len >= MIN_WORD_LENGTH)
    {
      register int key = drmaa_attrib_hash (str, len);

      if (key <= MAX_HASH_VALUE && key >= 0)
        {
          register const char *s = wordlist[key].name;

          if (*str == *s && !strcmp (str + 1, s + 1))
            return &wordlist[key];
        }
    }
  return 0;
}
