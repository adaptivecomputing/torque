#ifndef _PBS_CONFIG_H_
#define _PBS_CONFIG_H_

@TOP@

/* acconfig.h
   These are additional symbols used and defined in configure.in
   for PBS that are not known to autoconf.

   Descriptive text for the C preprocessor macros that
   the distributed Autoconf macros can define.

   The entries are in sort -df order: alphabetical, case insensitive,
   ignoring punctuation (such as underscores).  Although this order
   can split up related entries, it makes it easier to check whether
   a given entry is in the file.

   Leave the following blank line there!!  Autoheader needs it.  */


/* PBS specific: turns on the compilation of DEBUG code */
#undef DEBUG

/* are FD_SET and friends defined in sys/select.h */
#undef FD_SET_IN_SYS_SELECT_H

/* should we use setresuid instead os seteuid */
#undef HAVE_SETRESUID

/* does the header file sys/ioctl.h exist */
#undef HAVE_SYS_IOCTL_H

/* is the global int h_errno declared in netdb.h */
#undef H_ERRNO_DECLARED

/* PBS specific: defined if this is an SP2 */
#undef IBM_SP2

/* PBS specific: the PBS style machine type name */
#undef PBS_MACH

/* PBS specific: the machine specific resc_def_* file name */
#undef MACH_RESC_DEFS

/* PBS specific: whether checkpointing is supported */
#undef MOM_CHECKPOINT

/* PBS specific: Define if you will be using a nodefile */
#undef NEEDNODES

/* PBS specific: Define to the path of the default file */
#undef PBS_DEFAULT_FILE

/* PBS specific: Define to the path of the environment file */
#undef PBS_ENVIRON

/* PBS specific: Define if you are on a solaris box */
#undef PBS_PUSH_STREAM

/* PBS specific: Define to the path of the server home dir */
#undef PBS_SERVER_HOME

/* PBS specific: Select which daemons plock themselves, a bit pattern */
#undef PLOCK_DAEMONS

/* PBS specific: Define to the path of the qstat init file */
#undef QSTATRC_PATH

/* PBS specific: Define of the options to pass to the rcp program */
#undef RCP_ARGS

/* PBS specific: set the type of scheduler to use */
#undef SCHD_TYPE

/* PBS specific: Define to the path of scp if to be used for file delivery */
#undef SCP_PATH

/* PBS specific: full pathname of sendmail */
#undef SENDMAIL_CMD

/* PBS specific: set whether job scripts use a pipe */
#undef SHELL_INVOKE

/* PBS specific: Define if one should use SRFS on Crays */
#undef SRFS

/* PBS specific: Define if PBS should use syslog for error reporting */
#undef SYSLOG

/* PBS specific: Define if PBS should use Tcl in its tools */
#undef TCL

/* PBS specific: the seperator for Tcl attribute parsing */
#undef TCL_ATRSEP

/* PBS specific: Define if PBS should build qstat with Tcl or Tclx */
#undef TCL_QSTAT

/* PBS specific: Define if PBS should use Tclx in its tools */
#undef TCLX

/* PBS specific: The pathname of the temporary directory for mom */
#undef TMP_DIR


/* Leave that blank line there!!  Autoheader needs it.
   If you're adding to this file, keep in mind:
   The entries are in sort -df order: alphabetical, case insensitive,
   ignoring punctuation (such as underscores).  */

@BOTTOM@

#endif /* _PBS_CONFIG_H_ */
