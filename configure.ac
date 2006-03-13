dnl
dnl This is an autoconf script.
dnl To rebuild the `configure' script from this, use the bootstrap
dnl script in the directory containing this script.
dnl You must have autoconf version 2.53 or later.
dnl

dnl
dnl Configuration and build system for PBS written
dnl  by Lonhyn T. Jasinskyj (lonhyn@nas.nasa.gov)
dnl 
dnl (credit for the inspiration for some of this is due to the work of
dnl  Martin Buchholz who autoconf'ed XEmacs)
dnl
dnl Partly rewritten and ported to Autoconf-2.53 and Automake
dnl  by Karsten Petersen (kapet@hrz.tu-chemnitz.de)
dnl
dnl Expanded to support 'make distcheck', shared libs,
dnl  bi-arches, pbs-config script, and updated to TORQUE 2.x
dnl  by Garrick Staples (garrick@usc.edu)

AC_PREREQ(2.53)
AC_INIT([torque], [2.1.0p0], [torqueusers@supercluster.org])
AC_REVISION($Revision$)
AC_CONFIG_SRCDIR([src/cmds/qrun.c])
AC_CONFIG_AUX_DIR([buildutils])
AC_CANONICAL_HOST


# Change some defaults.
# I'm not proud of just shoving this here in configure.ac, but I couldn't find
# a better way of doing it. -garrick
test "$libdir" = '${exec_prefix}/lib' && libdir='${exec_prefix}/lib/torque/lib'
test "$includedir" = '${prefix}/include' && includedir='${prefix}/lib/torque/include'
test "$localstatedir" = '${prefix}/var' && localstatedir='/var'


AM_INIT_AUTOMAKE
AM_MAINTAINER_MODE

dnl  Instead of putting a long list of defines on the command line
dnl  for each compile, use a file called "pbs_config.h" that will
dnl  be created in the include directory.
AM_CONFIG_HEADER([src/include/pbs_config.h])


dnl
dnl  ######################################################################
dnl  figure out what PBS "machine type" we are on
dnl
AC_CACHE_CHECK([PBS machine type], pbs_cv_type_mach,
	       [pbs_cv_type_mach=`$ac_aux_dir/pbs_mach_type`])
PBS_MACH=$pbs_cv_type_mach
if test "x$PBS_MACH" = "xunknown"; then
	AC_MSG_ERROR([Machine type "$PBS_MACH" is not supported.])
fi
AC_SUBST(PBS_MACH)
AC_DEFINE_UNQUOTED(PBS_MACH, "${PBS_MACH}",
		   [the PBS style machine type name])

#  this defines the name of a .c file which is then included in
#  another resc_def_all.c
AC_DEFINE_UNQUOTED(MACH_RESC_DEFS, "resc_def_${PBS_MACH}.c",
		   [the machine specific resc_def_* file name])


# All of these need to go away!  The code needs to be fixed to use
# feature-based defines with proper tests here
case $PBS_MACH in
  darwin*) AC_DEFINE(__TDARWIN,1,[Define if on Darwin])
    case `uname -r` in
      8*) AC_DEFINE(__TDARWIN_8,1,[Define if on Darwin 8.x]) ;;
    esac
    ;;
esac


dnl
dnl  ######################################################################
dnl  Debug mode?
dnl
AC_MSG_CHECKING([whether to turn on the DEBUG flag])
AC_ARG_ENABLE(debug,
    [  --enable-debug          turn on the DEBUG flag],
    enable_debug=$enableval, enable_debug=no)
AC_MSG_RESULT($enable_debug)
if test "x$enable_debug" = "xyes" ; then
	AC_DEFINE(DEBUG, 1, [turns on the compilation of DEBUG code])
fi


dnl
dnl  ######################################################################
dnl  Let the user choose, what parts of PBS he wants to compile and install
dnl
AC_MSG_CHECKING([whether to include server and scheduler])
AC_ARG_ENABLE(server,
    [  --disable-server        do not include server and scheduler],
    include_server=$enableval, include_server=yes)
AC_MSG_RESULT($include_server)
AM_CONDITIONAL(INCLUDE_SERVER, [test "x$include_server" = "xyes"])


AC_MSG_CHECKING([whether to include the mom daemon])
AC_ARG_ENABLE(mom,
    [  --disable-mom           do not include the mom daemon],
    include_mom=$enableval, include_mom=yes)
AC_MSG_RESULT($include_mom)
AM_CONDITIONAL(INCLUDE_MOM, [test "x$include_mom" = "xyes"])


AC_MSG_CHECKING([whether to include the clients])
AC_ARG_ENABLE(clients,
    [  --disable-clients       do not include the clients],
    include_clients=$enableval, include_clients=yes)
AC_MSG_RESULT($include_clients)
AM_CONDITIONAL(INCLUDE_CLIENTS, [test "x$include_clients" = "xyes"])


dnl
dnl  ######################################################################
dnl  Check for the C compiler (some tests rely on it)
dnl

dnl  did the user provide CC and CFLAGS variables?  store for later
user_choosed_cc_set=${CC+set}
user_choosed_cflags_set=${CFLAGS+set}


dnl  look for the C-Compiler
AC_PROG_CC

dnl
dnl  ######################################################################
dnl  Check for programs
dnl

AC_PROG_GCC_TRADITIONAL

dnl  Libtool
dnl AC_DISABLE_SHARED
AC_PROG_LIBTOOL

# my libtool seems to be missing the libsuff check for ppc64.  This is pretty
# ugly, but hopefully temporary.
case $host_os in
  linux-gnu*)
  case "$host_cpu" in
  powerpc*|ppc*)
    echo '[#]line __oline__ "configure"' > conftest.$ac_ext
    if AC_TRY_EVAL(ac_compile); then
      case `/usr/bin/file conftest.$ac_objext` in
      *64-bit*)
        libsuff=64
        ;;
      *) libsuff= ;;
      esac
    fi
    rm -rf conftest*
    sys_lib_dlsearch_path_spec="/lib${libsuff} /usr/lib${libsuff}"
    sys_lib_search_path_spec="/lib${libsuff} /usr/lib${libsuff} /usr/local/lib${libsuff}"
    ;;
  esac
  ;;
esac


dnl this is causing problems with solaris procfs.h
dnl AC_SYS_LARGEFILE

dnl largefile support: try 2!
dnl this seems to work well, but did I over-design this?
orig_CFLAGS="$CFLAGS"
AC_CHECK_FUNC(stat64,
  AC_DEFINE(HAVE_STAT64,1,[Define if stat64() is available]),
  [CFLAGS="$CFLAGS -D_LARGEFILE64_SOURCE"
   unset ac_cv_func_stat64
   AC_CHECK_FUNC(stat64,
    AC_DEFINE(HAVE_STAT64,1,[Define if stat64() is available]),
     [CFLAGS="$orig_CFLAGS"])])

AC_CHECK_MEMBER(struct stat64.st_mode,
  AC_DEFINE(HAVE_STRUCT_STAT64,1,[Define if struct stat64 is available]),
  CFLAGS="$CFLAGS -D_LARGEFILE64_SOURCE"
  unset ac_cv_member_struct_stat64_st_mode
  AC_CHECK_MEMBER(struct stat64.st_mode,
    AC_DEFINE(HAVE_STRUCT_STAT64,1,[Define if struct stat64 is available]),
                          [CFLAGS="$orig_CFLAGS"],
[#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>]),
[#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>])



AC_PROG_LN_S


dnl  needed for scheduler.basl
AM_PROG_LEX
AC_PROG_YACC


dnl  Check for troff and its family. Prefer groff because we know
dnl  its fonts whereas various troffs name their fonts differently.
dnl  (e.g. CO vs C)
AC_CHECK_PROGS(TBL, gtbl tbl, exit)
AC_CHECK_PROGS(PIC, gpic pic, exit)
AC_CHECK_PROGS(ROFF, groff troff, exit)


if test foo = bar ;then
dnl  Now for some gore...
dnl  Massage the CFLAGS on a per-machine basis
dnl  (But only, if the user did not manually choose some.)
if test -z "$user_choosed_cflags_set"; then
  dnl
  dnl  Following values of CFLAGS are known to work well.
  dnl
  if   test "$GCC" = "yes"; then
#    CFLAGS="-g -O2"
#    do not optimize during SPBS-1.0.1 beta (11/22/03)
    CFLAGS="-g"
  elif test "$CC" = "xlc"; then
    CFLAGS="-O3 -qstrict -qnoansialias -qlibansi -qinfo -qro -qmaxmem=20000"
  else
    case "$build" in
      *-*-irix*) CFLAGS='-O -Wl,-woff,84' ;;
      *-*-hpux*) CFLAGS='-Ae' ;;
      *-*-osf*)  CFLAGS='-std1' ;;
      *-*-solaris2.7) if test -n "`isainfo -kv | grep 64-bit`" ; then
                        CFLAGS=-xarch=v9
                      else
                        CFLAGS=-O
                      fi ;;
      *)         CFLAGS="-O" ;; dnl The only POSIX-approved flag
    esac
  fi
fi
fi


dnl
dnl  ######################################################################
dnl  System specific settings
dnl
AC_ARG_ENABLE(nodemask,
    [  --enable-nodemask       enable nodemask-based scheduling on the Origin 2000])
if test "${enable_nodemask}" = "yes" ; then
  case "${PBS_MACH}" in
    irix6*) AC_DEFINE(NODEMASK, 1, [use nodemask-based scheduling on O2k]) ;;
    *) AC_MSG_ERROR([Cannot --enable-nodemask on a machine not running IRIX6]);;
  esac
fi


AC_ARG_ENABLE(pemask,
    [  --enable-pemask         enable pemask-based scheduling on the Cray T3e])
if test "${enable_pemask}" = "yes" ; then
  case "${PBS_MACH}" in
    unicosmk*) AC_DEFINE(PE_MASK, 1, [use pemask-based scheduling on Cray T3e]) ;;
    *) AC_MSG_ERROR([Cannot --enable-pemask on a machine not running UNICOS/mk]);;
  esac
fi


SRFS=0
AC_ARG_ENABLE(srfs,
    [  --enable-srfs           enable support for SRFS on Cray])
if test "${enable_srfs}" = "yes" ; then
  case "${PBS_MACH}" in
    unicos*) SRFS=1 ;;
    *) AC_MSG_ERROR([Cannot --enable-srfs on a machine not running UNICOS]);;
  esac
fi
AC_DEFINE_UNQUOTED(SRFS, ${SRFS}, [include support for SRFS on Cray])


MOMLIBS=""

PBSPOE=""
PBSPOEO=""
AC_ARG_ENABLE(sp2,
    [  --enable-sp2            build PBS for an IBM SP2])
if test "x$enable_sp2" = "xyes" ; then
    MOMLIBS="-ljm_client -lSDR"
    case "$PBS_MACH" in
      aix*) AC_CHECK_LIB(switchtbl, swtbl_load_table,
                         [ IBM_SP2=2
                           LIBS="-lswitchtbl $LIBS"
                           PBSPOE="pbspd pbspoe"
                           PBSPOEO="pbspoe.o pbspd.o" ],
                         [ IBM_SP2=1 ])
            ;;
      *) AC_MSG_ERROR([Cannot --enable-sp2 on a machine not running AIX]) ;;
    esac
    AC_DEFINE_UNQUOTED(IBM_SP2, ${IBM_SP2}, [defined if this is an SP2])
fi
AC_SUBST(PBSPOE)    
AC_SUBST(PBSPOEO)


dnl  irix6 can be changed to irix6array explicitly with an
dnl  --enable-array and irix6array can be made into irix6 with a
dnl  --disable-array.
AC_ARG_ENABLE(array,
    [  --enable-array          setting this under IRIX enables the SGI Origin 2000
                          parallel support. Normally autodetected from
                          the /etc/config/array file.],
[
    if test "$PBS_MACH" = irix6; then
        if test "$enableval" = yes; then
            AC_MSG_WARN([No array daemon detected but enable-array overrides.])
            PBS_MACH=irix6array
        fi
    elif test "$PBS_MACH" = irix6array; then
        if test "$enableval" = no; then
            AC_MSG_WARN([Array daemon detected but disable-array overrides.])
            PBS_MACH=irix6
        fi
    else
        AC_MSG_ERROR([Cannot use --en/disable-array unless configuring for IRIX 6.x or above])
    fi
])

dnl
dnl arch-specific libs
dnl
MOM_ADDTL_OBJS=""
case $PBS_MACH in
  aix4|aix5) MOMLIBS="$MOMLIBS -lodm -lcfg"
    ;;

  irix6array) MOMLIBS="-larray"
    ;;

  linux)
    AC_CHECK_FUNC(openpty, ,
      AC_CHECK_LIB(util, openpty, MOMLIBS="$MOMLIBS -lutil"),
        AC_MSG_WARN([This is an ancient distro... will open ptys directly])
        AC_DEFINE([USEOLDTTY], 1, [Define on ancient linux distros]))
    ;;
  darwin*) AC_DEFINE(SETCONTROLLINGTTY, 1, [Define to set the controlling tty])
    ;;
  digitalunix*) AC_DEFINE(SETCONTROLLINGTTY, 1, [Define to set the controlling tty])
    ;;
  unicosmk*) MOM_ADDTL_OBJS="wait_job.o"
    AC_DEFINE(SETCONTROLLINGTTY, 1, [Define to set the controlling tty])
    ;;
esac

case $PBS_MACH in
aix4|aix5|sunos4|solaris5|freebsd|darwin|fujitsu)
  AC_CHECK_FUNC(nlist, , AC_CHECK_LIB(elf, nlist)) ;;
esac

case $PBS_MACH in
sunos4|solaris5|netbsd|freebsd|darwin)
  AC_CHECK_LIB(kvm, kvm_open) ;;
esac

AC_SUBST(MOM_ADDTL_OBJS)



dnl  Support for checkpointing, note that under IRIX at this time
dnl  it seems that you must be compiling with the -64 flag to
dnl  get the cpr libraries on O2000s
MOM_CHECKPOINT=0
case "$PBS_MACH" in
    unicos*) MOM_CHECKPOINT=1 ;;
    irix6*)  AC_CHECK_LIB(cpr, atcheckpoint, [MOM_CHECKPOINT=1])
             if test "$MOM_CHECKPOINT" = 1 ; then
                MOMLIBS="$MOMLIBS -lcpr"
                #
                # if we are using plain cc then under irix6array
                # make it cc -64 unless the user has explicitly asked for cc
                #
		if test "$CC" = cc -a "x$user_choosed_cc_set" != xset ; then
                  AC_MSG_WARN([Automatically setting CC='cc -64' under irix6.])
                  CC='cc -64'
		fi
              fi ;;
esac
AC_DEFINE_UNQUOTED(MOM_CHECKPOINT, ${MOM_CHECKPOINT},
	[whether checkpointing is supported])

AC_SUBST(MOMLIBS)



dnl  Streams on Solaris
case "$build" in
  *-*-solaris*) AC_DEFINE(PUSH_STREAM, 1,
			[Define if you are on a solaris box]) ;;
  *) ;;
esac


dnl  Readline
AC_ARG_WITH(readline,
    [  --without-readline      do not include readline support
                          (default: included if found)],
    [HAVE_READLINE="${withval}"], [HAVE_READLINE="default"])
case "$HAVE_READLINE" in
	yes|no|default) ;;
	*) AC_MSG_ERROR([--with-readline only takes "yes" or "no" as arguments]) ;;
esac


dnl
dnl  ######################################################################
dnl  Scheduler settings
dnl
AC_ARG_WITH(sched,
    [  --with-sched=TYPE       sets the scheduler type. If TYPE is
                          "c" the scheduler will be written in C
                          "tcl" the server will use a Tcl based scheduler
                          "basl" will use the rule based scheduler
                          "no" then their will be no scheduling done
                          (the "c" scheduler is the default)],
[case "${withval}" in
  c*|C*)        SCHD_TYPE=cc ;;
  tcl|Tcl)      SCHD_TYPE=tcl ;;
  basl|BASL)    SCHD_TYPE=basl ;;
  *)            SCHD_TYPE=none ;;
esac],[SCHD_TYPE=cc])
AC_DEFINE_UNQUOTED(SCHD_TYPE, "${SCHD_TYPE}",
	[set the type of scheduler to use])

SCHD_TYPE_SUBDIR=""
if test "$SCHD_TYPE" != none; then
  SCHD_TYPE_SUBDIR=scheduler.$SCHD_TYPE
fi
AC_SUBST(SCHD_TYPE_SUBDIR)
AC_SUBST(SCHD_TYPE)
AM_CONDITIONAL(SCHD_BASL, test "x$SCHD_TYPE" = "xbasl")

if test "$SCHD_TYPE" = basl ;then
    AC_CHECK_FUNC(sqrt, ,
      AC_CHECK_LIB(m, sqrt, SCHD_LIBS="$SCHD_LIBS -lm"))
fi
AC_SUBST(SCHD_LIBS)

AC_ARG_WITH(sched_code,
    [  --with-sched-code=PATH  sets the name of the scheduler to use.
                          This only applies to BASL schedulers and those
                          written in the C language. For C schedulers
                          this should be a directory name and for BASL
                          schedulers a filename ending in ".basl".
			  It will be interpreted relative to
			  srctree/src/schedulers.SCHD_TYPE/samples.
                          As an example, an appropriate BASL scheduler
                          realtive path would be "nas.basl". The default
                          scheduler code for "C" schedulers is "fifo".],
[if test "${SCHD_TYPE}" = "cc" -o "${SCHD_TYPE}" = "basl"; then
  SCHD_CODE="${withval}"
else
  AC_MSG_ERROR([--with-sched-code was used but sched-type was not "c" or "basl".])
fi],[SCHD_CODE=none])


dnl  if the scheduler is the C scheduler then the default code is samples/fifo
if test "$SCHD_CODE" = none -a "$SCHD_TYPE" = cc; then
    SCHD_CODE=fifo
fi

if test "$SCHD_TYPE" = cc -o "$SCHD_TYPE" = basl; then
  if test "$SCHD_CODE" = none; then
    AC_MSG_ERROR([Must specify --with-sched-code for C and BASL schedulers.])
  fi
fi  

AC_SUBST(SCHD_CODE)


dnl
dnl  ######################################################################
dnl  Global settings
dnl
AC_ARG_ENABLE(rpp,
[
  --disable-rpp           use RPP/UDP for resource queries from PBS server
                          to mom.  This is enabled by default.  If disabled,
                          TCP is used.  This does not effect general node/job
                          status messages, job launch/exit messages, inter-mom
                          messages, etc.],
[case "${enableval}" in
  yes) RPP=1 ;;
  no)  RPP=0 ;;
  *)   AC_MSG_ERROR(--enable-rpp cannot take a value) ;;
esac],[RPP=1])dnl
AC_DEFINE_UNQUOTED(RPP, ${RPP}, [Define to enable RPP resource queries])
AM_CONDITIONAL(USING_RPP, [test "$RPP" = "1"])



AC_ARG_ENABLE(filesync,
[
  --enable-filesync       open files with sync on each write operation.  Don't
                          bother enabling this, all it does is slow down TORQUE.
                          This is disabled by default.],
[case "${enableval}" in
  yes) TDISABLEFILESYNC=0; AC_MSG_WARN([--enable-filesync does not actually make for safer writes.]) ;;
  no)  TDISABLEFILESYNC=1 ;;
  *)   AC_MSG_ERROR(--enable-filesync cannot take a value) ;;
esac],[TDISABLEFILESYNC=1])dnl
AC_DEFINE_UNQUOTED(TDISABLEFILESYNC, ${TDISABLEFILESYNC}, [Define to prevent sync writes])



AC_ARG_ENABLE(plock_daemons,
    [  --enable-plock-daemons[[=ARG]]
                          enable daemons to lock themselves into memory:
                          logical-or of 1 for pbs_server, 2 for pbs_scheduler,
                          4 for pbs_mom (no argument means 7 for all three)],
[case "${enableval}" in
  yes) PLOCK_DAEMONS=7 ;;
  no)  PLOCK_DAEMONS=0 ;;
  *) PLOCK_DAEMONS="${enableval}" ;;
esac],[PLOCK_DAEMONS=0])
AC_DEFINE_UNQUOTED(PLOCK_DAEMONS, ${PLOCK_DAEMONS},
	[Select which daemons plock themselves, a bit pattern])


AC_ARG_ENABLE(syslog,
[
  --enable-syslog         enable (default) the use of syslog for error reporting],
[case "${enableval}" in
  yes) SYSLOG=1 ;;
  no)  SYSLOG=0 ;;
  *)   AC_MSG_ERROR(--enable-syslog cannot take a value.) ;;
esac],[SYSLOG=1])dnl
AC_DEFINE_UNQUOTED(SYSLOG, ${SYSLOG}, [Define to enable syslog])
AM_CONDITIONAL(USING_SYSLOG, [test "$SYSLOG" = "1"])



AC_ARG_ENABLE(shell_pipe,
    [  --disable-shell-pipe    give the job script file as standard input to
                          the shell instead of passing its name via a pipe],
[case "${enableval}" in
  yes) SHELL_INVOKE=1 ;;
  no)  SHELL_INVOKE=0 ;;
  *)   SHELL_INVOKE=0 ;;
esac],[SHELL_INVOKE=1])
AC_DEFINE_UNQUOTED(SHELL_INVOKE, ${SHELL_INVOKE},
	[set whether job scripts use a pipe])

AC_ARG_WITH(maildomain,
[
  --with-maildomain=MAILDOMAIN
                          override the default domain for outgoing mail messages,
                          i.e. "user@maildomain".  The default maildomain is the
                          hostname where the job was submitted from.],
[MAILDOMAIN="${withval}"
AC_MSG_WARN([--with-maildomain is depricated.  Set mail_domain in qmgr.])
AC_DEFINE_UNQUOTED(TMAILDOMAIN, "${MAILDOMAIN}", [override the default domain for outgoing mail messages])])

AC_ARG_ENABLE(spool,
[
  --disable-spool         if disabled, TORQUE will create output and error files
                          directly in $HOME/.pbs_spool if it exists or in $HOME
                          otherwise.  By default, TORQUE will spool files in
                          $TORQUEHOME/spool and copy them to the users home
                          directory when the job completes.],
[case "${enableval}" in
  yes) NO_SPOOL_OUTPUT=0 ;;
  no)  NO_SPOOL_OUTPUT=1 ;;
  *)   AC_MSG_ERROR(--enable-spool cannot take a value) ;;
esac],[NO_SPOOL_OUTPUT=0])dnl
AC_DEFINE_UNQUOTED(NO_SPOOL_OUTPUT, ${NO_SPOOL_OUTPUT}, [directly use homedirs instead of $TORQUEHOME/spool])

AC_ARG_ENABLE(shell-use-argv,
[
  --enable-shell-use-argv enable this to put the job script name on the
                          command line that invokes the shell.  Not on by
                          default.  Ignores --enable-shell-pipe setting.],
[case "${enableval}" in
  yes) SHELL_USE_ARGV=1 ;;
  no)  SHELL_USE_ARGV=0 ;;
  *)   AC_MSG_ERROR(--enable-shell-use-argv cannot take a value) ;;
esac],[SHELL_USE_ARGV=0])dnl
AC_DEFINE_UNQUOTED(SHELL_USE_ARGV, ${SHELL_USE_ARGV}, [job script name passed as the shell's arg])


AC_ARG_ENABLE(privports,
[
  --disable-privports     disable the use of priviledged ports for authentication.  Some versions of OSX have
                          a buggy bind() and cannot bind to priviledged ports.],
[case "${enableval}" in
  yes)  ;;
  no)  NOPRIVPORTS=1; AC_DEFINE(NOPRIVPORTS, 1, [Define to disable priviledged ports]) ;;
  *)   AC_MSG_ERROR(--enable-privports cannot take a value) ;;
esac])dnl


AC_ARG_ENABLE(mom-checkspool,
[
  --disable-mom-checkspool
                          Don't check free space on spool directory and set an error],
[case "${enableval}" in
  yes) MOMCHECKLOCALSPOOL=1; AC_DEFINE(NOPRIVPORTS, 1, [Define to error if spool is full on MOM]) ;;
  no)  ;;
  *)   AC_MSG_ERROR(--enable-privports cannot take a value) ;;
esac])dnl


AC_ARG_WITH(tmpdir,
    [  --with-tmpdir=DIR       set the tmp directory that pbs_mom will use
                          defaults to "/tmp". This is a Cray-specific feature.],
    [TMP_DIR="${withval}"], [TMP_DIR="/tmp"])
test "$TMP_DIR" = yes && TMP_DIR="/tmp"
AC_DEFINE_UNQUOTED(TMP_DIR, "${TMP_DIR}",
		   [The pathname of the Cray temporary directory for mom])


AC_ARG_WITH(server_home,
    [  --with-server-home=DIR  set the server home/spool directory for PBS use
                          defaults to /var/spool/torque],
    [PBS_SERVER_HOME="${withval}"], [PBS_SERVER_HOME="$localstatedir/spool/torque"])
AC_SUBST(PBS_SERVER_HOME)


AC_ARG_WITH(server_name_file, 
    [  --with-server-name-file=FILE
                          set the file that will contain the name of
                          the default server for clients to use. If this
                          is not an absolute pathname, it will be evaluated
                          relative to the server home directory that either
                          defaults to /usr/spool/PBS or is set using
                          the --with-server-home option to configure. If this
                          option is not specified, the default name for
                          this file will be set to "server_name".],
    [pbs_default_file="${withval}"], [pbs_default_file="server_name"])
case "${pbs_default_file}" in
  /*) PBS_DEFAULT_FILE="${pbs_default_file}" ;;
  *)  PBS_DEFAULT_FILE="\${PBS_SERVER_HOME}/${pbs_default_file}" ;;
esac
AC_SUBST(PBS_DEFAULT_FILE)


AC_ARG_WITH(default_server,
    [  --with-default-server=HOSTNAME
                          set the name of the computer that clients will
                          access when no machine name is specified as part
                          of the queue name. It defaults to the hostname
                          of the machine on which PBS is being compiled.],
[PBS_DEFAULT_SERVER="${withval}"], [PBS_DEFAULT_SERVER="`uname -n`"])
AC_DEFINE_UNQUOTED(PBS_DEFAULT_SERVER, "${PBS_DEFAULT_SERVER}",
	[Define to the name of the default PBS server])
AC_SUBST(PBS_DEFAULT_SERVER)


AC_ARG_WITH(environ,
    [  --with-environ=PATH     set the path containing the environment variables
                          for the daemons.  For SP2 and AIX systems,
                          suggested setting is to /etc/environment. Defaults
                          to the file "pbs_environment" in the server-home.
                          Relative paths are interpreted within the context
                          of the server-home.],
[pbs_environ="${withval}"], [pbs_environ="no"])
case "${pbs_environ}" in
  yes) AC_MSG_ERROR([--with-environ must be given a path]) ;;
  no)  PBS_ENVIRON="\${PBS_SERVER_HOME}/pbs_environment" ;;
  /*)  PBS_ENVIRON="${pbs_environ}" ;;
  *)   PBS_ENVIRON="\${PBS_SERVER_HOME}/${pbs_environ}" ;;
esac
AC_SUBST(PBS_ENVIRON)


AC_ARG_WITH(qstatrc-file,
    [  --with-qstatrc-file=FILE
                          set the name of the file that qstat will use
                          if there is no ".qstatrc" file in the directory
                          where it is being invoked.
                          Relative path names will be evaluated relative
                          to the server home directory (see above).
                          If this option is not specified, the default name
                          for this file will be set to "qstatrc" (no dot)
                          in the server home directory.],
[case "${withval}" in
  /*)  QSTATRC_PATH="${withval}" ;;
  *)   QSTATRC_PATH="${PBS_SERVER_HOME}/${withval}" ;;
esac],[QSTATRC_PATH="${PBS_SERVER_HOME}/qstatrc"])
AC_DEFINE_UNQUOTED(QSTATRC_PATH, "${QSTATRC_PATH}",
	[Define to the path of the qstat init file])


dnl Declare if scp is to be used for file delivery
dnl The default is pbs_rcp (in mom_rcp) which is Berkley's rcp
dnl We know it always returns a non-zero exit status for any error.
dnl If scp is to be used and it fails, PBS falls back to rcp.
SCP_PATH=""
AC_ARG_WITH(scp,
    [  --with-scp              use scp instead of mom_rcp.],
  [ AC_PATH_PROG(SCP_PATH, "scp", "error")
    case "${SCP_PATH}" in
      error) AC_MSG_ERROR([--with-scp scp not found in path]) ;;
      *)     AC_DEFINE_UNQUOTED(SCP_PATH, "${SCP_PATH}",
		[Define to the path of scp if to be used for file delivery]) ;;
    esac])
AM_CONDITIONAL(INCLUDE_MOM_RCP, [test "x$SCP_PATH" = "x"])

dnl find sendmail and add it to defs
AC_ARG_WITH(sendmail,
    [  --with-sendmail[[=FILE]]  sendmail executable to use],
    [SENDMAIL_CMD=$withval],
    [sendmail_maybe_in="/usr/lib:/usr/sbin:/usr/bin:/etc:/usr/etc:$PATH"
     AC_PATH_PROGS(SENDMAIL_CMD, sendmail, sendmail,
		   ${sendmail_maybe_in})
    ])
case "${SENDMAIL_CMD}" in
  /*) ;;
  *) AC_MSG_WARN([emails might not be sent unless ${SENDMAIL_CMD} is found at run-time]) ;;
esac
AC_DEFINE_UNQUOTED(SENDMAIL_CMD, "${SENDMAIL_CMD}",
	[full pathname of sendmail])


dnl
dnl  ######################################################################
dnl  Tcl/Tk settings
dnl

TCL=0
TCLX=0
TK=0
TKX=0

if test "x${with_tcl}" != "xno" ; then
	if test "x${with_tcl}" = "xyes" ; then
		with_tcl=""
		user_requested_tcl=yes
	fi
	# find and load tclConfig.sh
	TEA_PATH_TCLCONFIG
        if test "x$TCL_BIN_DIR" = "xnone" ; then
          TCL=0
        else
	  TEA_LOAD_TCLCONFIG
	  TEA_PUBLIC_TCL_HEADERS
          if test "x$TCL_INCLUDES" = "xnone" ;then
            TCL=0
          else
	    TCL=1
          fi
        fi
	if test "x$user_requested_tcl" = "xyes" -a $TCL = 0 ;then
          AC_MSG_ERROR([Tcl was requested but not found])
	fi
fi

if test "${TCL}" = "1" -a "x${with_tclx}" != "xno" ; then
	if test "x${with_tclx}" = "xyes" ; then
		with_tclx=""
		user_requested_tclx=yes
	fi
	# find and load tclxConfig.sh
	TEA_PATH_CONFIG([tclx])
        if test "x$tclx_BIN_DIR" = "xnone" ; then
          TCLX=0
        else
	  TEA_LOAD_CONFIG([tclx])
	  TCLX=1
        fi
	if test "x$user_requested_tclx" = "xyes" -a $TCLX = 0 ;then
          AC_MSG_ERROR([TclX was requested but not found])
	fi
fi

if test "${TCL}" = "1" -a "x${with_tk}" != "xno" ; then
	if test "x${with_tk}" = "xyes" ; then
		with_tk=""
		user_requested_tk=yes
	fi
	# find and load tkConfig.sh
	TEA_PATH_TKCONFIG
        if test "x$TK_BIN_DIR" = "xnone" ; then
          TK=0
        else
	  TEA_LOAD_TKCONFIG
	  TEA_PUBLIC_TK_HEADERS
          if test "x$TK_INCLUDES" = "xnone" ; then
            TK=0
          else
	    TK=1
          fi
        fi
	if test "x$user_requested_tk" = "xyes" -a $TK = 0 ;then
          AC_MSG_ERROR([Tk was requested but not found])
	fi
fi

if test "${TK}" = "1" -a "${TCLX}" = "1" -a "x${with_tkx}" != "xno" ; then
	if test "x${with_tkx}" = "xyes" ; then
		with_tkx=""
		user_requested_tkx=yes
	fi
	# find and load tkxConfig.sh
	TEA_PATH_CONFIG([tkx])
        if test "x$tkx_BIN_DIR" = "xnone" ; then
          TKX=0
        else
	  TEA_LOAD_CONFIG([tkx])
	  TKX=1
        fi
	if test "x$user_requested_tkx" = "xyes" -a $TK = 0 ;then
          AC_MSG_ERROR([TkX was requested but not found])
	fi
fi

AC_DEFINE_UNQUOTED(TCL, ${TCL}, [Define if PBS should use Tcl in its tools])
AC_DEFINE_UNQUOTED(TCLX, ${TCLX}, [Define if PBS should use TclX in its tools])
AC_DEFINE_UNQUOTED(TK, ${TK}, [Define if PBS should use Tk in its tools])
AC_DEFINE_UNQUOTED(TKX, ${TKX}, [Define if PBS should use Tkx in its tools])


dnl  It is possible that the user has asked for a gui but no Tk
dnl  libraries were found. In this case we must disable the gui.
AC_MSG_CHECKING([whether to include the GUI-clients])
AC_ARG_ENABLE(gui,
    [  --disable-gui           do not include the GUI-clients],
    include_gui=$enableval)
if test "x${include_clients}" != "xyes" -a "x${include_gui}" = "xyes"; then
	AC_MSG_ERROR([the GUI can not be build independent of the clients])
fi
if test "x${include_clients}" != "xyes" -a "x${include_gui}" = "x"; then
	include_gui=no 
fi
if test "x${include_gui}" = "xyes" -a "${TK}" != "1"; then
	AC_MSG_ERROR([cannot build GUI without Tk library])
fi
if test "x${include_gui}" = "x" -a "${TK}" = "1"; then
	include_gui=yes
fi
if test "x${include_gui}" = "x"; then
	include_gui=no
fi
AC_MSG_RESULT($include_gui)
AM_CONDITIONAL(INCLUDE_GUI, [test "x$include_gui" = "xyes"])




dnl  Now assemble appropriate compiler flags for includes and linking.
MY_TCL_LIBS=""
MY_TCL_INCS=""

if test "${TCL}" = "1"; then
	MY_TCL_LIBS="${MY_TCL_LIBS} ${TCL_LIB_SPEC} ${TCL_LIBS}"
	MY_TCL_INCS="${MY_TCL_INCS} ${TCL_INCLUDES}"
fi

if test "${TCLX}" = "1"; then
	MY_TCL_LIBS="${TCLX_LIB_SPEC} ${TCLX_LIBS} ${MY_TCL_LIBS}"
fi

if test "${TK}" = "1"; then
	MY_TCL_LIBS="${TK_LIB_SPEC} ${TK_XLIBSW} ${MY_TCL_LIBS}"
	MY_TCL_INCS="${MY_TCL_INCS} ${TK_INCLUDES} ${TK_XINCLUDES}"
fi

if test "${TKX}" = "1"; then
	MY_TCL_LIBS="${TKX_LIB_SPEC} ${TKX_LIBS} ${MY_TCL_LIBS}"
fi

if test "${TCL}" = "1"; then
  # try to hack in a run-time search path if needed
  orig_MY_TCL_LIBS="$MY_TCL_LIBS"
  orig_libdir="$libdir"
  MY_TCL_LIBS=""
  for tcllib_part in $orig_MY_TCL_LIBS ;do
    case $tcllib_part in
      -L/*)
        tcllibdir=`echo $tcllib_part | sed s/-L//`
        tcldir_in_syslib_path=no
        for sysdir in $sys_lib_dlsearch_path_spec ;do
           if test "X$tcllibdir" = "X$sysdir" ;then
               tcldir_in_syslib_path=yes
               break
           fi
        done
        MY_TCL_LIBS="${MY_TCL_LIBS} ${tcllib_part}"
        if test "X$tcldir_in_syslib_path" = "Xno" ;then
           libdir="$tcllibdir"
           eval "new_lib_spec=\"$hardcode_libdir_flag_spec\""
           MY_TCL_LIBS="${MY_TCL_LIBS} ${new_lib_spec}"
        fi
        ;;
      -L)
        ;;
      *) MY_TCL_LIBS="${MY_TCL_LIBS} ${tcllib_part}"
        ;;
    esac
  done
  libdir="$orig_libdir"
  
  AC_MSG_NOTICE([TCL_LIBS: $MY_TCL_LIBS])
  AC_MSG_NOTICE([TCL_INCS: $MY_TCL_INCS])
fi

AC_SUBST(MY_TCL_LIBS)
AC_SUBST(MY_TCL_INCS)
AM_CONDITIONAL(USING_TCL, [test "$TCL" = "1"])
AM_CONDITIONAL(USING_TK, [test "$TK" = "1"])

dnl  Now look around for tclsh and wish
if test "${TCL}" = "1"; then
	TEA_PROG_TCLSH
	dnl automatically subst TCLSH_PROG
fi

if test "${TK}" = "1"; then
	TEA_PROG_WISH
	dnl automatically subst WISH_PROG
fi


AC_ARG_WITH(tclatrsep,
    [  --with-tclatrsep=CHAR   set the Tcl attribute separator character
                        this will default to "." if unspecified],
    [tcl_atrsep="${withval}"], [tcl_atrsep="."])
if test "$TCL" = "1" ; then
AC_MSG_CHECKING([checking for Tcl attribute seperator])
AC_MSG_RESULT([$tcl_atrsep])
fi
AC_DEFINE_UNQUOTED(TCL_ATRSEP, "${tcl_atrsep}",
	[the seperator for Tcl attribute parsing])


AC_ARG_ENABLE(tcl-qstat,
   [  --enable-tcl-qstat       setting this builds qstat with Tcl interpreter
                          features. This is enabled if Tcl is enabled.])
AC_MSG_CHECKING([whether to enable tcl-qstat])
EXTRA_QSTAT_LIBS=""
if test "x${enable_tcl_qstat}" = "x" -o "x${enable_tcl_qstat}" != "xno"; then
    if test "$TCL" = "1" ; then
	EXTRA_QSTAT_LIBS='$(EXTRA_QSTAT_LIBS)'
        AC_DEFINE(TCL_QSTAT, 1,
		[Define if PBS should build qstat with Tcl])
        enable_tcl_qstat=yes
    else
        if test "${enable_tcl_qstat}" = "yes" ;then
           AC_MSG_ERROR([Cannot enable tcl-qstat without also using Tcl])
        else
           enable_tcl_qstat=no
        fi
    fi
fi
AC_MSG_RESULT($enable_tcl_qstat)
AC_SUBST(EXTRA_QSTAT_LIBS)
AM_CONDITIONAL(USE_TCLQSTAT, [test "x$EXTRA_QSTAT_LIBS" != "x"])



dnl
dnl  ######################################################################
dnl  Check for libraries
dnl

dnl  nsl is needed on some systems but duplicates libc.a on others (O2000s)
AC_CHECK_FUNC(xdr_int, , AC_CHECK_LIB(nsl, xdr_int))
AC_CHECK_FUNC(ruserok, , AC_CHECK_LIB(socket, ruserok))


dnl  Readline
if test "$HAVE_READLINE" = "default" ; then
  case $PBS_MACH in
    aix5|darwin) HAVE_READLINE="no";;
    *) HAVE_READLINE="yes";;
  esac
fi
if test "$HAVE_READLINE" = "yes" ; then
AC_CHECK_LIB(ncurses, initscr, READLINE_LIBS="-lncurses")
AC_CHECK_LIB(readline, rl_callback_handler_install,
	     [READLINE_LIBS="$READLINE_LIBS -lreadline"],
	     [HAVE_READLINE="no"], [$READLINE_LIBS])
fi
AC_SUBST(READLINE_LIBS)

	     
dnl
dnl  ######################################################################
dnl  Check for headers
dnl

AC_HEADER_DIRENT
AC_HEADER_STDC
AC_HEADER_SYS_WAIT


AC_CHECK_HEADERS([arpa/inet.h fcntl.h float.h limits.h malloc.h netdb.h \
		  netinet/in.h stddef.h sys/file.h sys/param.h \
		  sys/socket.h sys/time.h sys/ioctl.h sys/mount.h sys/vfs.h sys/statfs.h syslog.h termios.h err.h])

dnl  Readline
if test "$HAVE_READLINE" = "yes" ; then
AC_CHECK_HEADER(readline/readline.h, :, [HAVE_READLINE="no"])
fi
if test "$HAVE_READLINE" = "yes" ; then
	AC_DEFINE(HAVE_READLINE, 1, [include readline support])
fi


dnl
dnl  ######################################################################
dnl  Check for typedefs, structures and such
dnl


dnl  under some OSs h_errno is not visible
AC_DECL_H_ERRNO

dnl  see if we need to include sys/select.h to get FD_SET and friends
AC_DECL_FD_SET_SYS_SELECT_H


dnl  Check the sizes of various types for DIS
dnl  The cross-compiling sizes are there to shut autoconf up
dnl  and are chosen to all be different so that dis.h uses
dnl  specific routines for each type instead of substituting in the
dnl  unlikely (and unsupported) event of cross-compilation.
AC_CHECK_SIZEOF(int, 20)
AC_CHECK_SIZEOF(long, 21)
AC_CHECK_SIZEOF(float, 22)
AC_CHECK_SIZEOF(double, 23)
AC_CHECK_SIZEOF(long double, 24)
AC_CHECK_SIZEOF(short, 25)
AC_CHECK_SIZEOF(unsigned, 26)
AC_CHECK_SIZEOF(unsigned int, 27)
AC_CHECK_SIZEOF(unsigned short, 28)
AC_CHECK_SIZEOF(unsigned char, 29)
AC_CHECK_SIZEOF(unsigned long, 30)
AC_CHECK_SIZEOF(signed char, 31)
AC_C_CHAR_UNSIGNED


dnl
dnl  ######################################################################
dnl  Check for functions
dnl

dnl  under HP-UX seteuid does not exist, look for setresuid instead
AC_CHECK_FUNCS(seteuid setresuid,break)
AC_CHECK_FUNCS(setegid setresgid,break)


AC_CHECK_FUNCS([gettimeofday bindresvport wordexp])

dnl see if atexit() exists, if not check for on_exit(), if that fails, panic
AC_CHECK_FUNCS(atexit on_exit, [ { found_an_exit=yes; break; } ])
if test "$found_an_exit" != yes; then
    AC_MSG_ERROR([Must have atexit() or on_exit()])
fi

dnl detect if __func__ is usable
AC_C_VAR_FUNC
if test "${ac_cv_c_var_func}" != "yes" ; then
	dnl __func__ is not usable so check for __FUNCTION__
	AC_C_VAR_FUNCTION
	if test "${ac_cv_c_var_function}" != "yes" ; then
		AC_MSG_ERROR([$CC must support __func__ or __FUNCTION__])
	fi
fi

dnl possibly define __func__ as __FUNCTION__
AH_VERBATIM([__func__],
[#ifndef HAVE_FUNC
#define __func__ __FUNCTION__
#endif])


dnl
dnl  ######################################################################
dnl  Output
dnl

AC_CREATE_GENERIC_CONFIG([pbs],[-llog -lcmds -lnet -lpbs])

dnl these are quoted, and this removes them
spec_name=$PACKAGE_TARNAME
spec_version=`echo $PACKAGE_VERSION | sed 's/-snap.*//'`
spec_snap=""
if test "$spec_version" != "$PACKAGE_VERSION" ;then
  spec_snap="%define snap `echo $PACKAGE_VERSION | sed 's/.*-snap.//'`"
fi
AC_SUBST(spec_name)
AC_SUBST(spec_version)
AC_SUBST(spec_snap)

dnl  Head and bottom for the config-file
AH_TOP([#ifndef _PBS_CONFIG_H_
#define _PBS_CONFIG_H_])

AH_BOTTOM([#endif /* _PBS_CONFIG_H_ */])


dnl  Include the Cplant Fault Recovery Patch jugglery
AH_VERBATIM([NEED_BLOCKING_CONNECTIONS],
[#ifndef NEED_BLOCKING_CONNECTIONS
#define write(a,b,c) write_nonblocking_socket(a,b,c)
#define read(a,b,c) read_nonblocking_socket(a,b,c)
#endif])


AC_OUTPUT(buildutils/pbs_mkdirs
	  buildutils/self-extract-head-sh
	  Makefile
	  torque.spec
	  doc/Makefile
	  doc/man1/Makefile
	  doc/man3/Makefile
	  doc/man8/Makefile
	  src/Makefile
	  src/cmds/Makefile
	  src/iff/Makefile
	  src/gui/Makefile
	  src/gui/Ccode/Makefile
	  src/include/Makefile
	  src/lib/Makefile
	  src/lib/Libattr/Makefile
	  src/lib/Libcmds/Makefile
	  src/lib/Libdis/Makefile
	  src/lib/Libifl/Makefile
	  src/lib/Liblog/Makefile
	  src/lib/Libnet/Makefile
	  src/lib/Libpbs/Makefile
	  src/lib/Libsite/Makefile
	  src/mom_rcp/Makefile
	  src/resmom/Makefile
	  src/resmom/aix4/Makefile
	  src/resmom/aix5/Makefile
	  src/resmom/darwin/Makefile
	  src/resmom/digitalunix/Makefile
	  src/resmom/freebsd/Makefile
	  src/resmom/fujitsu/Makefile
	  src/resmom/hpux10/Makefile
	  src/resmom/hpux11/Makefile
	  src/resmom/irix5/Makefile
	  src/resmom/irix6/Makefile
	  src/resmom/irix6array/Makefile
	  src/resmom/linux/Makefile
	  src/resmom/netbsd/Makefile
	  src/resmom/solaris5/Makefile
	  src/resmom/solaris7/Makefile
	  src/resmom/sunos4/Makefile
	  src/resmom/unicos8/Makefile
	  src/resmom/unicosmk2/Makefile
	  src/scheduler.basl/Makefile
	  src/scheduler.cc/Makefile
	  src/scheduler.cc/samples/Makefile
	  src/scheduler.cc/samples/cray_t3e/Makefile
	  src/scheduler.cc/samples/dec_cluster/Makefile
	  src/scheduler.cc/samples/fifo/Makefile
	  src/scheduler.cc/samples/msic_cluster/Makefile
	  src/scheduler.cc/samples/sgi_origin/Makefile
	  src/scheduler.cc/samples/umn_cluster/Makefile
	  src/scheduler.tcl/Makefile
	  src/server/Makefile
	  src/tools/Makefile
	  src/tools/xpbsmon/Makefile)

