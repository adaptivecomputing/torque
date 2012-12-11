/* src/include/pbs_config.h.  Generated from pbs_config.h.in by configure.  */
/* src/include/pbs_config.h.in.  Generated from configure.ac by autoheader.  */

#ifndef _PBS_CONFIG_H_
#define _PBS_CONFIG_H_

/* Define to add x attributes to accounting log */
/* #undef ATTR_X_ACCT */

/* turns on the compilation of AUTORUN_JOBS code */
/* #undef AUTORUN_JOBS */

/* 1234 = LIL_ENDIAN, 4321 = BIGENDIAN */
/* #undef BYTEORDER */

/* turns on the compilation of DEBUG code */
/* #undef DEBUG */

/* Alternate directory for MOM logs */
/* #undef DEFAULT_MOMLOGDIR */

/* Alternate suffix for MOM logs */
/* #undef DEFAULT_MOMLOGSUFFIX */

/* Define to disable daemons */
/* #undef DISABLE_DAEMONS */

/* Define to enable BLCR support */
/* #undef ENABLE_BLCR */

/* Define to enable Cray's CPA support */
/* #undef ENABLE_CPA */

/* Define to enable Cray's CSA support */
/* #undef ENABLE_CSA */

/* Define to enable unix domain sockets */
/* #undef ENABLE_UNIX_SOCKETS */

/* are FD_SET and friends defined in sys/select.h */
#define FD_SET_IN_SYS_SELECT_H 1

/* Define to the type of elements in the array set by `getgroups'. Usually
   this is either `int' or `gid_t'. */
#define GETGROUPS_T gid_t

/* Define to 1 if you have the <arpa/inet.h> header file. */
#define HAVE_ARPA_INET_H 1

/* Define to 1 if you have the `asprintf' function. */
/* #undef HAVE_ASPRINTF */

/* Define to 1 if you have the `atexit' function. */
#define HAVE_ATEXIT 1

/* Define to 1 if you have the `bindresvport' function. */
#define HAVE_BINDRESVPORT 1

/* Define to 1 if you have the <bitmask.h> header file. */
/* #undef HAVE_BITMASK_H */

/* Define to 1 if you have the <cpalib.h> header file. */
/* #undef HAVE_CPALIB_H */

/* Define to 1 if you have the <cpuset.h> header file. */
/* #undef HAVE_CPUSET_H */

/* Define to 1 if you have the declaration of `strerror_r', and to 0 if you
   don't. */
/* #undef HAVE_DECL_STRERROR_R */

/* Define to 1 if you have the <dirent.h> header file, and it defines `DIR'.
   */
#define HAVE_DIRENT_H 1

/* Define to 1 if you have the <dlfcn.h> header file. */
#define HAVE_DLFCN_H 1

/* Define to 1 if you have the <err.h> header file. */
#define HAVE_ERR_H 1

/* Define to 1 if you have the <fcntl.h> header file. */
#define HAVE_FCNTL_H 1

/* Define to 1 if you have the <float.h> header file. */
#define HAVE_FLOAT_H 1

/* Define to 1 if you have the `fstat' function. */
/* #undef HAVE_FSTAT */

/* Define to 1 if you have the `getaddrinfo' function. */
#define HAVE_GETADDRINFO 1

/* Define to 1 if you have the `getcwd' function. */
/* #undef HAVE_GETCWD */

/* Define to 1 if your system has a working `getgroups' function. */
#define HAVE_GETGROUPS 1

/* Define to 1 if you have the `gettimeofday' function. */
#define HAVE_GETTIMEOFDAY 1

/* Define to 1 if you have the <inttypes.h> header file. */
#define HAVE_INTTYPES_H 1

/* Define to 1 if you have the `cpuset' library (-lcpuset). */
/* #undef HAVE_LIBCPUSET */

/* Define to 1 if you have the `crypto' library (-lcrypto). */
#define HAVE_LIBCRYPTO 1

/* Define to 1 if you have the `elf' library (-lelf). */
/* #undef HAVE_LIBELF */

/* Define to 1 if you have the `hwloc' library (-lhwloc). */
/* #undef HAVE_LIBHWLOC */

/* Define to 1 if you have the `kvm' library (-lkvm). */
/* #undef HAVE_LIBKVM */

/* Define to 1 if you have the `memacct' library (-lmemacct). */
/* #undef HAVE_LIBMEMACCT */

/* Define to 1 if you have the `nsl' library (-lnsl). */
/* #undef HAVE_LIBNSL */

/* Define to 1 if you have the `resolv' library (-lresolv). */
/* #undef HAVE_LIBRESOLV */

/* Define to 1 if you have the `socket' library (-lsocket). */
/* #undef HAVE_LIBSOCKET */

/* Define to 1 if you have the `ssl' library (-lssl). */
#define HAVE_LIBSSL 1

/* Define to 1 if you have the <limits.h> header file. */
#define HAVE_LIMITS_H 1

/* Define if lseek64() is available */
#define HAVE_LSEEK64 1

/* Define to 1 if you have the <mach/shared_region.h> header file. */
/* #undef HAVE_MACH_SHARED_REGION_H */

/* Define to 1 if you have the <malloc.h> header file. */
#define HAVE_MALLOC_H 1

/* Define to 1 if you have the <memory.h> header file. */
#define HAVE_MEMORY_H 1

/* Define to 1 if you have the `mkstemp' function. */
/* #undef HAVE_MKSTEMP */

/* Define to 1 if you have the <ndir.h> header file, and it defines `DIR'. */
/* #undef HAVE_NDIR_H */

/* Define to 1 if you have the <netdb.h> header file. */
#define HAVE_NETDB_H 1

/* Define to 1 if you have the <netinet/in.h> header file. */
#define HAVE_NETINET_IN_H 1

/* Define to 1 if you have the `on_exit' function. */
/* #undef HAVE_ON_EXIT */

/* Define if open64() is available */
#define HAVE_OPEN64 1

/* Define to 1 if you have the <pam/pam_modules.h> header file. */
/* #undef HAVE_PAM_PAM_MODULES_H */

/* Define to 1 if you have the `poll' function. */
#define HAVE_POLL 1

/* Define if you have POSIX threads libraries and header files. */
/* #undef HAVE_PTHREAD */

/* include readline support */
/* #undef HAVE_READLINE */

/* Define to 1 if you have the <readline/readline.h> header file. */
/* #undef HAVE_READLINE_READLINE_H */

/* Define to 1 if you have the `rresvport' function. */
#define HAVE_RRESVPORT 1

/* Define to 1 if you have the <security/pam_appl.h> header file. */
#define HAVE_SECURITY_PAM_APPL_H 1

/* Define to 1 if you have the <security/pam_modules.h> header file. */
#define HAVE_SECURITY_PAM_MODULES_H 1

/* Define to 1 if you have the `setegid' function. */
#define HAVE_SETEGID 1

/* Define to 1 if you have the `seteuid' function. */
#define HAVE_SETEUID 1

/* Define to 1 if you have the `setresgid' function. */
/* #undef HAVE_SETRESGID */

/* Define to 1 if you have the `setresuid' function. */
/* #undef HAVE_SETRESUID */

/* Define to 1 if you have the <source/COIPipeline_source.h> header file. */
/* #undef HAVE_SOURCE_COIPIPELINE_SOURCE_H */

/* Define if stat64() is available */
#define HAVE_STAT64 1

/* Define to 1 if stdbool.h conforms to C99. */
/* #undef HAVE_STDBOOL_H */

/* Define to 1 if you have the <stddef.h> header file. */
#define HAVE_STDDEF_H 1

/* Define to 1 if you have the <stdint.h> header file. */
#define HAVE_STDINT_H 1

/* Define to 1 if you have the <stdlib.h> header file. */
#define HAVE_STDLIB_H 1

/* Define to 1 if you have the `strchr' function. */
/* #undef HAVE_STRCHR */

/* Define to 1 if you have the `strdup' function. */
/* #undef HAVE_STRDUP */

/* Define to 1 if you have the `strerror' function. */
/* #undef HAVE_STRERROR */

/* Define to 1 if you have the `strerror_r' function. */
/* #undef HAVE_STRERROR_R */

/* Define to 1 if you have the `strftime' function. */
/* #undef HAVE_STRFTIME */

/* Define to 1 if you have the <strings.h> header file. */
#define HAVE_STRINGS_H 1

/* Define to 1 if you have the <string.h> header file. */
#define HAVE_STRING_H 1

/* Define to 1 if you have the `strlcpy' function. */
/* #undef HAVE_STRLCPY */

/* Define if struct stat64 is available */
#define HAVE_STRUCT_STAT64 1

/* Define to 1 if you have the <syslog.h> header file. */
#define HAVE_SYSLOG_H 1

/* Define to 1 if you have the <sys/dir.h> header file, and it defines `DIR'.
   */
/* #undef HAVE_SYS_DIR_H */

/* Define to 1 if you have the <sys/file.h> header file. */
#define HAVE_SYS_FILE_H 1

/* Define to 1 if you have the <sys/ioctl.h> header file. */
#define HAVE_SYS_IOCTL_H 1

/* Define to 1 if you have the <sys/mount.h> header file. */
#define HAVE_SYS_MOUNT_H 1

/* Define to 1 if you have the <sys/ndir.h> header file, and it defines `DIR'.
   */
/* #undef HAVE_SYS_NDIR_H */

/* Define to 1 if you have the <sys/param.h> header file. */
#define HAVE_SYS_PARAM_H 1

/* Define to 1 if you have the <sys/poll.h> header file. */
#define HAVE_SYS_POLL_H 1

/* Define to 1 if you have the <sys/socket.h> header file. */
#define HAVE_SYS_SOCKET_H 1

/* Define to 1 if you have the <sys/statfs.h> header file. */
#define HAVE_SYS_STATFS_H 1

/* Define to 1 if you have the <sys/statvfs.h> header file. */
#define HAVE_SYS_STATVFS_H 1

/* Define to 1 if you have the <sys/stat.h> header file. */
#define HAVE_SYS_STAT_H 1

/* Define to 1 if you have the <sys/time.h> header file. */
#define HAVE_SYS_TIME_H 1

/* Define to 1 if you have the <sys/tty.h> header file. */
/* #undef HAVE_SYS_TTY_H */

/* Define to 1 if you have the <sys/types.h> header file. */
#define HAVE_SYS_TYPES_H 1

/* Define to 1 if you have the <sys/ucred.h> header file. */
/* #undef HAVE_SYS_UCRED_H */

/* Define to 1 if you have the <sys/uio.h> header file. */
#define HAVE_SYS_UIO_H 1

/* Define to 1 if you have the <sys/un.h> header file. */
#define HAVE_SYS_UN_H 1

/* Define to 1 if you have the <sys/vfs.h> header file. */
#define HAVE_SYS_VFS_H 1

/* Define to 1 if you have <sys/wait.h> that is POSIX.1 compatible. */
#define HAVE_SYS_WAIT_H 1

/* Define to 1 if you have the <termios.h> header file. */
#define HAVE_TERMIOS_H 1

/* Define to 1 if you have the <unistd.h> header file. */
#define HAVE_UNISTD_H 1

/* Define to 1 if you have the `vasprintf' function. */
/* #undef HAVE_VASPRINTF */

/* Define to 1 if you have the va_copy function. */
/* #undef HAVE_VA_COPY */

/* Define to 1 if you have the `wordexp' function. */
#define HAVE_WORDEXP 1

/* Define to 1 if the system has the type `_Bool'. */
/* #undef HAVE__BOOL */

/* Define to 1 if you have the __va_copy function. */
/* #undef HAVE___VA_COPY */

/* is the global int h_errno declared in netdb.h */
/* #undef H_ERRNO_DECLARED */

/* defined if this is an SP2 */
#define IBM_SP2 0

/* Define if open64, stat64, and friends work */
#define LARGEFILE_WORKS 1

/* Define to the sub-directory in which libtool stores uninstalled libraries.
   */
#define LT_OBJDIR ".libs/"

/* the machine specific resc_def_* file name */
#define MACH_RESC_DEFS "resc_def_linux.c"

/* Define to enable MIC support */
/* #undef MIC */

/* Define to error if spool is full on MOM */
#define MOMCHECKLOCALSPOOL 1

/* whether checkpointing is supported */
#define MOM_CHECKPOINT 0

/* force creation of nodefile */
#define MOM_FORCENODEFILE 0

#ifndef NEED_BLOCKING_CONNECTIONS
#define write(a,b,c) write_nonblocking_socket(a,b,c)
#define read(a,b,c) read_nonblocking_socket(a,b,c)
#endif

/* use nodemask-based scheduling on O2k */
/* #undef NODEMASK */

/* Define to disable mlockall */
/* #undef NOPOSIXMEMLOCK */

/* Define to disable privileged ports */
/* #undef NOPRIVPORTS */

/* directly use homedirs instead of $TORQUEHOME/spool */
#define NO_SPOOL_OUTPUT 0

/* Define if ntohl() is declared in arpa/inet.h */
/* #undef NTOHL_NEEDS_ARPA_INET_H */

/* Define to enable Nvidia gpu support */
/* #undef NVIDIA_GPUS */

/* Define to enable Nvidia NVML api support */
/* #undef NVML_API */

/* Name of package */
#define PACKAGE "torque"

/* Define to the address where bug reports for this package should be sent. */
#define PACKAGE_BUGREPORT "torqueusers@supercluster.org"

/* Define to the full name of this package. */
#define PACKAGE_NAME "torque"

/* Define to the full name and version of this package. */
#define PACKAGE_STRING "torque 4.2.0"

/* Define to the one symbol short name of this package. */
#define PACKAGE_TARNAME "torque"

/* Define to the home page for this package. */
#define PACKAGE_URL ""

/* Define to the version of this package. */
#define PACKAGE_VERSION "4.2.0"

/* Define to the build date */
#define PBS_BUILD_DATE "Mon Dec 10 10:58:12 MST 2012"

/* Define to the build directory */
#define PBS_BUILD_DIR "/sdb/dev/torque/actorque/torque"

/* Define to the build hostname */
#define PBS_BUILD_HOST "kmn"

/* Define to the building username */
#define PBS_BUILD_USER "knielson"

/* Define to the install directory */
#define PBS_CFLAGS "-g -W -Wall -Wno-unused-parameter -Wno-long-long -pedantic -Werror -D_LARGEFILE64_SOURCE -I/opt/intel/mic/coi/include"

/* Define to the install directory */
#define PBS_CONFIG_ARGS " '--with-debug' '--enable-gcc-warnings' '--with-server-home=/sdb/torque'"

/* Define to the name of the default PBS server */
#define PBS_DEFAULT_SERVER "kmn"

/* Define to the install directory */
#define PBS_INSTALL_DIR "/usr/local"

/* the PBS style machine type name */
#define PBS_MACH "linux"

/* Define to the source directory */
#define PBS_SOURCE_DIR "/sdb/dev/torque/actorque/torque"

/* Define to enable Linux 2.6 cpusets */
/* #undef PENABLE_LINUX26_CPUSETS */

/* use pemask-based scheduling on Cray T3e */
/* #undef PE_MASK */

/* Select which daemons plock themselves, a bit pattern */
#define PLOCK_DAEMONS 0

/* Define to necessary symbol if this constant uses a non-standard name on
   your system. */
/* #undef PTHREAD_CREATE_JOINABLE */

/* Define if you are on a solaris box */
/* #undef PUSH_STREAM */

/* Define to the path of the qstat init file */
#define QSTATRC_PATH "/sdb/torque/qstatrc"

/* turns on the QSUB_KEEP_NO_OVERRIDE flag */
/* #undef QSUB_KEEP_NO_OVERRIDE */

/* turns on the compilation of QUICKCOMMIT code */
/* #undef QUICKCOMMIT */

/* turns on the compilation of RESOURCEMAXDEFAULT code */
/* #undef RESOURCEMAXDEFAULT */

/* set the type of scheduler to use */
#define SCHD_TYPE "cc"

/* full pathname of sendmail */
#define SENDMAIL_CMD "sendmail"

/* Directory for Server checkpoint files */
/* #undef SERVER_CHKPTDIR */

/* Define to set the controlling tty */
/* #undef SETCONTROLLINGTTY */

/* set whether job scripts use a pipe */
#define SHELL_INVOKE 1

/* job script name passed as the shell's arg */
#define SHELL_USE_ARGV 0

/* The size of `double', as computed by sizeof. */
#define SIZEOF_DOUBLE 8

/* The size of `float', as computed by sizeof. */
#define SIZEOF_FLOAT 4

/* The size of `int', as computed by sizeof. */
#define SIZEOF_INT 4

/* The size of `long', as computed by sizeof. */
#define SIZEOF_LONG 8

/* The size of `long double', as computed by sizeof. */
#define SIZEOF_LONG_DOUBLE 16

/* The size of `short', as computed by sizeof. */
#define SIZEOF_SHORT 2

/* The size of `signed char', as computed by sizeof. */
#define SIZEOF_SIGNED_CHAR 1

/* The size of `unsigned', as computed by sizeof. */
#define SIZEOF_UNSIGNED 4

/* The size of `unsigned char', as computed by sizeof. */
#define SIZEOF_UNSIGNED_CHAR 1

/* The size of `unsigned int', as computed by sizeof. */
#define SIZEOF_UNSIGNED_INT 4

/* The size of `unsigned long', as computed by sizeof. */
#define SIZEOF_UNSIGNED_LONG 8

/* The size of `unsigned short', as computed by sizeof. */
#define SIZEOF_UNSIGNED_SHORT 2

/* include support for SRFS on Cray */
#define SRFS 0

/* Define to 1 if you have the ANSI C header files. */
#define STDC_HEADERS 1

/* Define to 1 if strerror_r returns char *. */
/* #undef STRERROR_R_CHAR_P */

/* repository svn version */
#define SVN_VERSION "unknown"

/* Define to enable syslog */
#define SYSLOG 1

/* Define if PBS should use Tcl in its tools */
#define TCL 0

/* Define if PBS should use TclX in its tools */
#define TCLX 0

/* the seperator for Tcl attribute parsing */
#define TCL_ATRSEP "."

/* Define if PBS should build qstat with Tcl */
/* #undef TCL_QSTAT */

/* Define to set a max retry limit */
#define TCP_RETRY_LIMIT 0

/* Define to prevent sync writes */
#define TDISABLEFILESYNC 1

/* Define if PBS should use Tk in its tools */
#define TK 0

/* Define if PBS should use Tkx in its tools */
#define TKX 0

/* override the default domain for outgoing mail messages */
/* #undef TMAILDOMAIN */

/* The pathname of the Cray temporary directory for mom */
#define TMP_DIR "/tmp"

/* Define if building on unsupported OS */
/* #undef UNSUPPORTED_MACH */

/* Define to enable libjob job_create support */
/* #undef USEJOBCREATE */

/* Define to enable libcpuset support */
/* #undef USELIBCPUSET */

/* Define to enable libmemacct support */
/* #undef USELIBMEMACCT */

/* Define on ancient linux distros */
/* #undef USEOLDTTY */

/* Version number of package */
#define VERSION "4.2.0"

/* whether byteorder is bigendian */
/* #undef WORDS_BIGENDIAN */

/* Define to the path of xauth */
#define XAUTH_PATH "/usr/bin/xauth"

/* Define to 1 if `lex' declares `yytext' as a `char *' by default, not a
   `char[]'. */
/* #undef YYTEXT_POINTER */

/* Define _GNU_SOURCE for portability */
#define _GNU_SOURCE 1

/* Define to 1 if type `char' is unsigned and you are not using gcc.  */
#ifndef __CHAR_UNSIGNED__
/* # undef __CHAR_UNSIGNED__ */
#endif

/* Define to `int' if <sys/types.h> doesn't define. */
/* #undef gid_t */

/* type to use for 2nd arg of pam_get_user */
#define pam_get_user_2nd_arg_t char

/* Define to `unsigned int' if <sys/types.h> does not define. */
/* #undef size_t */

/* type to use in place of socklen_t if not defined */
#define torque_socklen_t socklen_t

/* Define to `int' if <sys/types.h> doesn't define. */
/* #undef uid_t */


#ifndef HAVE_VA_COPY
#       define va_copy(a,b) __va_copy(a,b)
#endif



#ifndef __GNUC__
#       define __attribute__ /* nothing */
#endif



#endif /* _PBS_CONFIG_H_ */

