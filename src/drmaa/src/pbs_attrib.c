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


#define TOTAL_KEYWORDS 47
#define MIN_WORD_LENGTH 5
#define MAX_WORD_LENGTH 15
#define MIN_HASH_VALUE 6
#define MAX_HASH_VALUE 75
/* maximum key range = 70, duplicates = 0 */


struct pbs_attrib{ const char *name; int code; };

static unsigned int pbs_attrib_hash (
  
  const char *str, 
  unsigned int len)

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

const struct pbs_attrib * pbs_attrib_lookup (const char *str, unsigned int len)
  {  
  
  static const struct pbs_attrib wordlist[] =
    {
      {""}, {""}, {""}, {""}, {""}, {""},
      {"egroup",              ATTR_EGROUP},
      {""}, {""},
      {"exec_host",           ATTR_EXECUTION_HOST},
      {"euser",               ATTR_EUSER},
      {"server",              ATTR_SERVER},
      {""},
      {"substate",            ATTR_JOB_SUBMIT_STATE},
      {"job_state",           ATTR_JOB_STATE},
      {"etime",               ATTR_ETIME},
      {"max_running",         ATTR_MAX_RUNNING},
      {"stagein",             ATTR_STAGEIN},
      {"stageout",            ATTR_STAGEOUT},
      {"resources_used",      ATTR_RESOURCES_USED},
      {"session_id",          ATTR_SESSION_ID},
      {"exit_status",         ATTR_EXIT_STATUS},
      {""},
      {"Job_Name",            ATTR_JOB_NAME},
      {"Job_Owner",           ATTR_JOB_OWNER},
      {"queue",               ATTR_QUEUE},
      {"alt_id",              ATTR_ALT_ID},
      {""},
      {"Variable_List",       ATTR_ENV},
      {"Rerunable",           ATTR_RERUNABLE},
      {"queue_rank",          ATTR_QUEUE_RANK},
      {"depend",              ATTR_DEPENDENCIES},
      {""},
      {"Priority",            ATTR_PRIORITY},
      {"Join_Path",           ATTR_JOIN_FILES},
      {"qtime",               ATTR_QTIME},
      {"interactive",         ATTR_INTERACTIVE},
      {""},
      {"Resource_List",       ATTR_RESOURCES},
      {"hop_count",           ATTR_HOP_COUNT},
      {"total_jobs",          ATTR_TOTAL_JOBS},
      {"destination",         ATTR_DESTINATION_QUEUE},
      {"comment",             ATTR_COMMENT},
      {"hashname",            ATTR_HASHNAME},
      {"Mail_User",           ATTR_EMAIL},
      {"sched_hint",          ATTR_SCHEDULER_HINT},
      {"Mail_Points",         ATTR_MAIL_POINTS},
      {"Account_Name",        ATTR_REMOTE_USER},
      {"security",            ATTR_SECURITY},
      {"User_List",           ATTR_USER_LIST},
      {"ctime",               ATTR_CTIME},
      {"cookie",              ATTR_JOB_COOKIE},
      {""}, {""}, {""},
      {"Error_Path",          ATTR_ERROR_PATH},
      {"Output_Path",         ATTR_OUTPUT_PATH},
      {""}, {""},
      {"Execution_Time",      ATTR_SOFT_CPU_TIME_LIMIT},
      {"group_list",          ATTR_GROUP_LIST},
      {"Checkpoint",          ATTR_CHECKPOINT},
      {""}, {""}, {""},
      {"Shell_Path_List",     ATTR_SHELL},
      {""}, {""}, {""}, {""},
      {"Keep_Files",          ATTR_KEEP_FILES},
      {""}, {""}, {""}, {""},
      {"Hold_Types",          ATTR_HOLD_TYPES}
    };

  if (len <= MAX_WORD_LENGTH && len >= MIN_WORD_LENGTH)
    {
      int key = pbs_attrib_hash (str, len);

      if (key <= MAX_HASH_VALUE && key >= 0)
        {
          const char *s = wordlist[key].name;

          if (*str == *s && !strcmp (str + 1, s + 1))
            return &wordlist[key];
        }
    }
 

  return(NULL);
  }

#ifdef __cplusplus
}
#endif
