#include "license_pbs.h" /* See here for the software license */
#ifndef _PBS_CONSTANTS_H
#define _PBS_CONSTANTS_H

#define SUBMIT_FILTER_ADMIN_REJECT_CODE -1

/* allow only 9999 for a maximum number on the radix */
#define MAX_RADIX_NUM_LEN 4

#define MAX_QSUB_PREFIX_LEN 32

/* A 15 digit number is a lot of processors. 100 trillion 
 *  * will this be enough for the future? */
#define MAX_PROCS_DIGITS  15

/* hash_map_structs */
#define ENV_DATA      1
#define CMDLINE_DATA  2
#define LOGIC_DATA    3
#define STATIC_DATA   4
#define CONFIG_DATA   5
#define FILTER_DATA   6
#define SCRIPT_DATA   7

/* Actually 32 (inc -) but this allows for the NULL at the end */
#define MAXLONGLEN  33

/* qsub */
#define X11_CHAR_SIZE 512
#define MMAX_VERIFY_BYTES 50
#define MAX_ARGV_LEN 128
#define TMPLINE_LEN 4096
#define TCONST_CFGFILE "torque.cfg"

/* pbs_auth new version */
#define AUTH_IP "127.0.0.1"
#define AUTH_PORT 15005

/* long long is not defined on some systems. */
#if defined(__hpux)         /* HP-UX */
typedef long long  Long;
typedef unsigned long long u_Long;
#define atoL(nptr)  atol((nptr))
#define LLD "%lld"

#elif defined(__GNUC__) ||      /* SunOS, FreeBSD, NetBSD, BSDI */\
          defined(_AIX) && defined(__EXTENDED__)        /* AIX */
typedef long long  Long;
typedef unsigned long long u_Long;
#define atoL(nptr)  atol((nptr))
#define LLD "%lld"

#elif defined(__sgi) && defined(_LONGLONG) && _MIPS_SZLONG == 32    /* Irix */
typedef long long  Long;
typedef unsigned long long u_Long;
#define atoL(nptr)  atoll((nptr))
#define LLD "%lld"

#elif defined(sun) && defined(sparc) && defined(LLONG_MAX)  /* Solaris */
typedef long long  Long;
typedef unsigned long long u_Long;
#define atoL(nptr)  atoll((nptr))
#define LLD "%lld"

#else
typedef long   Long;
typedef unsigned long  u_Long;
#define atoL(nptr)  atol((nptr))
#define LLD "%ld"
#endif

/* These should only be used where the only options are TRUE/FALSE
 * for all other cases use PBSE_NONE and the associated error codes */
/* 
#ifdef FALSE
#  undef FALSE
#endif

#ifdef TRUE
#  undef TRUE
#endif 

enum { FALSE, TRUE };
*/

#endif /* _PBS_CONSTANTS_H */
