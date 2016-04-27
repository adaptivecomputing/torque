/* C++ code produced by gperf version 3.0.4 */
#ifdef HAVE_CONFIG_H
#include <pbs_config.h>
#endif

#include <string.h>
#include <attrib.h>

#ifdef __cplusplus
extern "C"
  {
#endif

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

struct drmaa_attrib { const char *name; int code; };

#define TOTAL_KEYWORDS 21
#define MIN_WORD_LENGTH 8
#define MAX_WORD_LENGTH 26
#define MIN_HASH_VALUE 8
#define MAX_HASH_VALUE 46
/* maximum key range = 39, duplicates = 0 */
static unsigned int drmaa_attrib_hash (
    
  const char *str, 
  unsigned int len)
  
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
    int hval = len;

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

const struct drmaa_attrib * drmaa_attrib_lookup (
    
  const char *str, 
  unsigned int len)

  {
  static const struct drmaa_attrib wordlist[] =
    {
      {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""},
      {"drmaa_wd",                    ATTR_JOB_WORKING_DIR},
      {""}, {""},
      {"drmaa_v_env",                 ATTR_ENV},
      {"drmaa_v_argv",                ATTR_ARGV},
      {"drmaa_v_email",               ATTR_EMAIL},
      {"drmaa_js_state",              ATTR_JOB_SUBMIT_STATE},
      {""},
      {"drmaa_wct_slimit",            ATTR_SOFT_WCT_LIMIT},
      {"drmaa_output_path",           ATTR_OUTPUT_PATH},
      {"drmaa_job_category",          ATTR_JOB_CATEGORY},
      {"drmaa_job_name",              ATTR_JOB_NAME},
      {"drmaa_transfer_files",        ATTR_TRANSFER_FILES},
      {"drmaa_duration_slimit",       ATTR_SOFT_CPU_TIME_LIMIT},
      {"drmaa_block_email",           ATTR_BLOCK_EMAIL},
      {""},
      {"drmaa_deadline_time",         ATTR_DEADLINE_TIME},
      {"drmaa_remote_command",        ATTR_JOB_PATH},
      {"drmaa_input_path",            ATTR_INPUT_PATH},
      {""}, {""},
      {"drmaa_wct_hlimit",            ATTR_HARD_WCT_LIMIT},
      {""},
      {"drmaa_duration_hlimit",       ATTR_HARD_CPU_TIME_LIMIT},
      {""}, {""},
      {"drmaa_join_files",            ATTR_JOIN_FILES},
      {""},
      {"drmaa_start_time",            ATTR_START_TIME},
      {""}, {""}, {""}, {""},
      {"drmaa_error_path",            ATTR_ERROR_PATH},
      {""}, {""}, {""}, {""},
      {"drmaa_native_specification",  ATTR_NATIVE}
    };

  if (len <= MAX_WORD_LENGTH && len >= MIN_WORD_LENGTH)
    {
      int key = drmaa_attrib_hash (str, len);

      if (key <= MAX_HASH_VALUE && key >= 0)
        {
          const char *s = wordlist[key].name;

          if (*str == *s && !strcmp (str + 1, s + 1))
            return &wordlist[key];
        }
    }
  return 0;
}

#ifdef __cplusplus
}
#endif
