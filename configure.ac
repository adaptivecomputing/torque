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
dnl
dnl Expanded to allow --enable-high-availability in order to have improved
dnl high availability features
dnl by David Beer (dbeer@adaptivecomputing.com)
dnl

AC_PREREQ(2.53)
AC_INIT([torque], [2.5.13], [torqueusers@supercluster.org])
AC_REVISION($Revision$)
AC_CONFIG_SRCDIR([src/cmds/qrun.c])
AC_CONFIG_AUX_DIR([buildutils])
AC_CANONICAL_HOST


AC_CHECK_PROGS(MAKE,$MAKE make gmake,error)
if test "x$MAKE" = "xerror" ;then
  AC_MSG_ERROR([cannot find a make command])
fi

AM_INIT_AUTOMAKE
AM_MAINTAINER_MODE

if test "$program_prefix" = "NONE";then
  program_prefix=""
fi
if test "$program_suffix" = "NONE";then
  program_suffix=""
fi
AC_SUBST(program_prefix)
AC_SUBST(program_suffix)

m4_pattern_allow([RPM_AC_OPTS])

RPM_AC_OPTS=""

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
	AC_MSG_WARN([MOM Machine type "$host_os" is not supported.])
        AC_DEFINE(UNSUPPORTED_MACH,1,[Define if building on unsupported OS])
fi
AC_SUBST(PBS_MACH)
AC_DEFINE_UNQUOTED(PBS_MACH, "${PBS_MACH}",
		   [the PBS style machine type name])

#  this defines the name of a .c file which is then included in
#  another resc_def_all.c
AC_DEFINE_UNQUOTED(MACH_RESC_DEFS, "resc_def_${PBS_MACH}.c",
		   [the machine specific resc_def_* file name])



dnl
dnl  ######################################################################
dnl  Autorun mode?
dnl
AC_MSG_CHECKING([whether to turn on the AUTORUN_JOBS flag])
AC_ARG_ENABLE(autorun,
    [  --enable-autorun        turn on the AUTORUN_JOBS flag],
    enable_autorun=$enableval, enable_autorun=no)
AC_MSG_RESULT($enable_autorun)
if test "x$enable_autorun" = "xyes" ; then
	AC_DEFINE(AUTORUN_JOBS, 1, [turns on the compilation of AUTORUN_JOBS code])
fi



dnl
dnl  ######################################################################
dnl  Resource max not default mode?
dnl
dnl  NOTE: due to confusion we are changing MAX to not be default by default
dnl
AC_MSG_CHECKING([whether to turn on the RESOURCEMAXDEFAULT flag])
AC_ARG_ENABLE(maxdefault,
    [  --enable-maxdefault  turn on the RESOURCEMAXDEFAULT flag],
    enable_maxdefault=$enableval, enable_maxdefault=no)
AC_MSG_RESULT($enable_maxdefault)
if test "x$enable_maxdefault" = "xyes" ; then
	AC_DEFINE(RESOURCEMAXDEFAULT, 1, [turns on the compilation of RESOURCEMAXDEFAULT code])
fi


dnl
dnl  ######################################################################
dnl  Quick commit mode?
dnl
AC_MSG_CHECKING([whether to turn on the QUICKCOMMIT flag])
AC_ARG_ENABLE(quickcommit,
    [  --enable-quickcommit    turn on the QUICKCOMMIT flag],
    enable_quickcommit=$enableval, enable_quickcommit=no)
AC_MSG_RESULT($enable_quickcommit)
if test "x$enable_quickcommit" = "xyes" ; then
	AC_DEFINE(QUICKCOMMIT, 1, [turns on the compilation of QUICKCOMMIT code])
fi

dnl
dnl  ######################################################################
dnl   Disable qsub keep(-k) override?
dnl
AC_ARG_ENABLE(qsub-keep-override, [
  --disable-qsub-keep-override 
                          do not allow the qsub -k flag to override -o -e.],
[case "${enableval}" in
  yes)  ;;
  no)  QSUB_KEEP_NO_OVERRIDE=1;
       AC_DEFINE(QSUB_KEEP_NO_OVERRIDE, 1, [turns on the QSUB_KEEP_NO_OVERRIDE flag]) ;;
  *)   AC_MSG_ERROR(--enable-qsub-keep-override cannot take a value) ;;
esac])dnl

dnl
dnl  ######################################################################
dnl  No SIG_CHLD mode?
dnl
AC_MSG_CHECKING([whether to turn on the NO_SIGCHLD flag])
AC_ARG_ENABLE(nochildsignal,
    [  --enable-nochildsignal  turn on the NO_SIGCHLD flag],
    enable_nochildsignal=$enableval, enable_nochildsignal=no)
AC_MSG_RESULT($enable_nochildsignal)
if test "x$enable_nochildsignal" = "xyes" ; then
	AC_DEFINE(NO_SIGCHLD, 1, [turns on the compilation of NO_SIGCHLD code])
fi


dnl
dnl  ######################################################################
dnl  Let the user choose, what parts of PBS he wants to compile and install
dnl
partdefault=yes
if test "x$PBS_MACH" = "xunknown"; then
    partdefault=no
fi
AC_MSG_CHECKING([whether to include server and scheduler])
AC_ARG_ENABLE(server,
    [  --disable-server        do not include server and scheduler],
    build_server=$enableval, build_server=$partdefault)
AC_MSG_RESULT($build_server)
AM_CONDITIONAL(INCLUDE_SERVER, [test "x$build_server" = "xyes"])
AC_SUBST(build_server)


AC_MSG_CHECKING([whether to include the mom daemon])
AC_ARG_ENABLE(mom,
    [  --disable-mom           do not include the mom daemon],
    build_mom=$enableval, build_mom=$partdefault)
AC_MSG_RESULT($build_mom)
AM_CONDITIONAL(INCLUDE_MOM, [test "x$build_mom" = "xyes"])
AC_SUBST(build_mom)


AC_MSG_CHECKING([whether to include the clients])
AC_ARG_ENABLE(clients,
    [  --disable-clients       do not include the clients],
    build_clients=$enableval, build_clients=yes)
AC_MSG_RESULT($build_clients)
AM_CONDITIONAL(INCLUDE_CLIENTS, [test "x$build_clients" = "xyes"])
AC_SUBST(build_clients)

AC_MSG_CHECKING([whether to build DRMAA lib])
AC_ARG_ENABLE(drmaa,
    [  --enable-drmaa          build the DRMAA 1.0 library (default is off)],
    build_drmaa=$enableval, build_drmaa=no)
AC_MSG_RESULT($build_drmaa)
AM_CONDITIONAL(INCLUDE_DRMAA, [test "x$build_drmaa" = "xyes"])
AC_SUBST(build_drmaa)


dnl
dnl  ######################################################################
dnl  Check for the C compiler (some tests rely on it)
dnl  this needs to be before AC_ARG_ENABLE(debug)
dnl

dnl  did the user provide CC and CFLAGS variables?  store for later
dnl  user_choosed_cc_set=${CC+set}
dnl  user_choosed_cflags_set=${CFLAGS+set}
dnl  SPECIFIED_CFLAGS=$CFLAGS
dnl  look for the C-Compiler
AC_PROG_CC
AC_PROG_GCC_TRADITIONAL

dnl
dnl  ######################################################################
dnl  Check for programs
dnl

AC_CHECK_PROGS(AR,ar,exit)

dnl wrapped libtool macro to remove annoying warning message
AC_PROG_LIBTOOL
TAC_PROG_LIBTOOL_PATCH

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


dnl
dnl  ######################################################################
dnl  Check how to compile
dnl


dnl
dnl  ######################################################################
dnl  compile in debug code?
dnl
AC_MSG_CHECKING([whether to turn on the compilation of DEBUG code])
AC_ARG_ENABLE(debug,
    [  --enable-debug          turn on the compilation of DEBUG code],
    enable_debug=$enableval, enable_debug=no)
AC_MSG_RESULT($enable_debug)
if test "x$enable_debug" = "xyes" ; then
	AC_DEFINE(DEBUG, 1, [turns on the compilation of DEBUG code])
fi


dnl
dnl  ######################################################################
dnl  compile with debugging symbols
dnl  this needs to be after AC_PROG_CC and before AX_CFLAGS_GCC_OPTION
dnl  and TAC_SYS_LARGEFILE
dnl
AC_MSG_CHECKING([whether to compile with debugging symbols])
AC_ARG_WITH([debug],
  AC_HELP_STRING([--with-debug], [compile with debugging symbols]),
  DEBUG_SYMBOLS=$withval, DEBUG_SYMBOLS="no")
AC_MSG_RESULT([DEBUG_SYMBOLS=$DEBUG_SYMBOLS])
dnl  remove -O* and add -g
if test "$DEBUG_SYMBOLS" = 'yes'; then
  AC_MSG_RESULT([before tweak CFLAGS=$CFLAGS])
  CFLAGS=`echo $CFLAGS | sed 's/ \?-O[[^ ]]*//g'`
  AC_MSG_RESULT([mid tweak CFLAGS=$CFLAGS])
  case $CFLAGS in
    *-g*)
      ;;
    *)
      if test "$CFLAGS" = ''; then
        CFLAGS="-g"
      else
        CFLAGS="-g $CFLAGS"
      fi
      ;;
  esac
  AC_MSG_RESULT([after tweak CFLAGS=$CFLAGS])
fi

dnl if using gcc, we can be very strict
AC_ARG_ENABLE(gcc_warnings, [
  --enable-gcc-warnings  Enable gcc strictness and warnings.  If using
                          gcc, default is to not error on any warning])
if test "x$GCC" = "xyes" ;then
  AC_MSG_CHECKING([whether to use strict gcc warnings])
  if test "${enable_gcc_warnings}" == "yes" ; then
    if test "x$build_drmaa" = "xyes" ;then
      AC_MSG_RESULT([no... because DRMAA is being built])
    else
      AC_MSG_RESULT([yes])
      AX_CFLAGS_GCC_OPTION([-W -Wall -Wno-unused-parameter -Wno-long-long -pedantic -Werror])
      gccwarnings=yes
    fi
  else
    AC_MSG_RESULT([no])
  fi
fi

dnl Torque's own largefile support
TAC_SYS_LARGEFILE

dnl check if we should use high availability mode
AC_ARG_ENABLE(high_availability, [
  --enable-high-availability  Enables advanced high availability mode (multi-threaded)])
if test "x$GCC" = "xyes" ;then
  AC_MSG_CHECKING([whether to use threaded high availability])
  if test "${enable_high_availability}" == "yes" ; then
    AC_MSG_RESULT([yes])
    PTHREAD_LIBS=-lpthread
    LIBS="$LIBS $PTHREAD_LIBS"
    CFLAGS="$CFLAGS -DUSE_HA_THREADS"
  else
    AC_MSG_RESULT([no])
    high_availability=yes
  fi
fi

dnl Configure saving the server as xml instead of bytes
dnl

AC_ARG_ENABLE(server_xml, [
  --enable-server-xml  Tells the server to save the serverdb in xml format])
if test "x$GCC" = "xyes" ; then
  AC_MSG_CHECKING([whether to save the serverdb in legacy format or xml format])
  if test "${enable_server_xml}" == "yes" ; then
    AC_MSG_RESULT([yes])
    CFLAGS="$CFLAGS -DSERVER_XML"
  else
    AC_MSG_RESULT([no])
  fi
fi

dnl

dnl If hpux and gcc, force the XOPEN interface.
AC_MSG_CHECKING([whether to force XOPEN networking stack])
AC_ARG_ENABLE(xopen_networking, [
  --disable-xopen-networking      
                          With HPUX and GCC, don't force usage of XOPEN and libxnet])
if test "${enable_xopen_networking}" != "no" ; then
case $GCC,$PBS_MACH in
  yes,hpux*) AC_MSG_RESULT([yes])
             AC_MSG_WARN([On HPUX and gcc, forcing XOPEN network stack.])
             AC_MSG_WARN([Use --disable-xopen-networking to prevent it])
             AC_CHECK_LIB([xnet], [getpeername], 
              [LIBS="-lxnet $LIBS"
               CFLAGS="$CFLAGS -D_XOPEN_SOURCE -D_XOPEN_SOURCE_EXTENDED=1"],
              [AC_MSG_WARN([This build will likely fail in mysterious ways at run-time])
               sleep 10])
  ;;
  *) AC_MSG_RESULT([no... phew!])
esac
else
  AC_MSG_RESULT([disabled])
fi


dnl
dnl  ######################################################################
dnl  Check for headers
dnl

AC_HEADER_DIRENT
AC_HEADER_STDC
AC_HEADER_SYS_WAIT

AC_CHECK_HEADERS([arpa/inet.h fcntl.h float.h limits.h malloc.h netdb.h \
		  netinet/in.h stddef.h sys/file.h sys/param.h sys/tty.h \
		  sys/socket.h sys/time.h sys/ioctl.h sys/mount.h \
                  sys/vfs.h sys/statfs.h sys/statvfs.h sys/ucred.h sys/un.h sys/uio.h \
                  syslog.h readline/readline.h \
                  termios.h err.h sys/poll.h pam/pam_modules.h security/pam_appl.h \
                  mach/shared_region.h])

# On Solaris, pam_modules.h requires pam_appl.h
AC_CHECK_HEADERS([security/pam_modules.h], [], [],
[#if HAVE_SECURITY_PAM_APPL_H
# include <security/pam_appl.h>
# endif
])







dnl
dnl  ######################################################################
dnl  System specific settings
dnl


AC_ARG_ENABLE(nodemask, [  --enable-nodemask       enable nodemask-based scheduling on the Origin 2000])
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
IBM_SP2=0
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
fi
AC_DEFINE_UNQUOTED(IBM_SP2, ${IBM_SP2}, [defined if this is an SP2])
AC_SUBST(PBSPOE)    
AC_SUBST(PBSPOEO)

AC_ARG_ENABLE(cpuset,[  --enable-cpuset         enable Linux 2.6 kernel cpusets (in development)])
build_l26_cpuset=no
if test "x$enable_cpuset" = "xyes" ;then
  build_l26_cpuset=yes
  AC_DEFINE(PENABLE_LINUX26_CPUSETS, 1, [Define to enable Linux 2.6 cpusets])
  cpuset=1
  dnl define geometry requests to allow these to be requested here
dnl  CFLAGS="$CFLAGS -DGEOMETRY_REQUESTS"  
else
  cpuset=0
fi
AM_CONDITIONAL([BUILD_L26_CPUSETS], test "$build_l26_cpuset" = yes)


dnl turn on the new feature to request a specific geometry for a process
dnl this uses cpusets to bind the job to that geometry
dnl --enable-geometry-requests
AC_ARG_ENABLE(geometry_requests, [
  --enable-geometry-requests  Enables the user to request a specific geometry using a cpuset implementation])
if test "x$GCC" = "xyes" ;then
  AC_MSG_CHECKING([whether to allow geometry requests])
  if test "${enable_geometry_requests}" = "yes" ; then
    AC_MSG_RESULT([yes])
    build_l26_cpuset=yes
    AC_DEFINE(PENABLE_LINUX26_CPUSETS, 1, [Define to enable Linux 2.6 cpusets])
dnl    AC_DEFINE(GEOMETRY_REQUESTS, 1, [Define to allow job specific geometry requests])
    if test "${cpuset}" = "1" ; then
      CFLAGS="$CFLAGS -DGEOMETRY_REQUESTS -DALWAYS_USE_CPUSETS"
    else
      CFLAGS="$CFLAGS -DGEOMETRY_REQUESTS"
    fi
  else
    AC_MSG_RESULT([no])
  fi
fi
AM_CONDITIONAL([BUILD_L26_CPUSETS], test "$build_l26_cpuset" = yes)

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
dnl enable BLCR support
dnl
AC_ARG_ENABLE(blcr,
    [  --enable-blcr           enable BLCR support],
    BLCR=$enableval,BLCR=no)

AC_ARG_WITH(blcr,
    [  --with-blcr=DIR          BLCR installation prefix],
    [
        blcrprefix=$withval
        BLCR=yes
        blcrlib="${blcrprefix}/lib"
        blcrbin="${blcrprefix}/bin"
        blcrinclude="${blcrprefix}/include"
    ],[
        blcrprefix=none
        blcrlib=none
        blcrbin=none
        blcrinclude=none
    ])
AC_ARG_WITH(blcr_include,
    [  --with-blcr-include=DIR  include path for libcr.h],
    blcrinclude=$withval)
AC_ARG_WITH(blcr_lib,
    [  --with-blcr-lib=DIR      lib path for libcr],
    blcrlib=$withval)
AC_ARG_WITH(blcr_bin,
    [  --with-blcr-bin=DIR      bin path for BLCR utilities],
    blcrbin=$withval)

AC_MSG_CHECKING([whether to build BLCR support])
AC_MSG_RESULT($BLCR)

case "$BLCR" in
  yes) AC_DEFINE([ENABLE_BLCR], 1, [Define to enable BLCR support])
    AS_IF([test "x$blcrlib" != xnone], [
      BLCR_LDFLAGS="-L$blcrlib -lcr"
    ],[
      BLCR_LDFLAGS="-lcr"
    ])
    SAVE_LDFLAGS="$LDFLAGS"
    LDFLAGS="$BLCR_LDFLAGS $LDFLAGS"
    AC_CHECK_LIB([cr], [cr_init], [ :], 
      AC_MSG_ERROR([pass the path to libcr.so to --with-blcr-lib]))
    LDFLAGS="$SAVE_LDFLAGS"
    AC_SUBST([BLCR_LDFLAGS])

    AS_IF([test "x$blcrinclude" != xnone], [
      BLCR_CPPFLAGS="-I$blcrinclude -std=gnu99"
    ],[
      BLCR_CPPFLAGS=""
    ])
    SAVE_CPPFLAGS="$CPPFLAGS"
    CPPFLAGS="$BLCR_CPPFLAGS $CPPFLAGS"
    AC_CHECK_HEADER([libcr.h], [ :], 
      AC_MSG_ERROR([pass the path to libcr.so to --with-blcr-include]))
    CPPFLAGS="$SAVE_CPPFLAGS"
    AC_SUBST([BLCR_CPPFLAGS])

    AS_IF([test "x$blcrbin" != xnone], [
        BLCR_BINDIR="$blcrbin"
        BLCR_PATH="$PATH_SEPARATOR$BLCR_BINDIR"
    ],[
        BLCR_BINDIR=""
        BLCR_PATH=""
    ])
    AC_CHECK_PROGS([HAVE_CR_RESTART], [cr_restart], [no],
      [PATH=$PATH$BLCR_PATH])
    AC_SUBST([BLCR_BINDIR])
    ;;
  no) : ;;
  *) AC_MSG_ERROR([--enable-blcr should be yes or no]) ;;
esac


dnl
dnl enable Nvidia gpu support
dnl
AC_MSG_CHECKING([whether to build Nvidia gpu support])
AC_ARG_ENABLE(nvidia-gpus,
    [  --enable-nvidia-gpus    enable Nvidia gpu support],
    NVIDIA=$enableval,NVIDIA=no)
AC_MSG_RESULT($NVIDIA)
case "$NVIDIA" in
  yes) AC_DEFINE([NVIDIA_GPUS], 1, [Define to enable Nvidia gpu support]) ;;
  no) : ;;
  *) AC_MSG_ERROR([--enable-nvidia-gpus should be yes or no]) ;;
esac

AC_ARG_WITH(nvml_include,
    [  --with-nvml-include=DIR include path for nvml.h],
    nvmlinclude=$withval,nvmlinclude=none)   
AC_ARG_WITH(nvml_lib,
    [  --with-nvml-lib=DIR     lib path for libnvidia-ml],
    nvmllib=$withval,nvmllib=none)   

if test "x$nvmlinclude" != "xnone" && test "x$nvmllib" != "xnone" ;then
  CPPFLAGS="$CPPFLAGS -I$nvmlinclude"
  MOMLIBS="$MOMLIBS -L$nvmllib -lnvidia-ml"
  AC_DEFINE([NVML_API], 1, [Define to enable Nvidia NVML api support])
fi

if test "x$nvmlinclude" != "xnone" && test "x$nvmllib" == "xnone" ;then
  AC_MSG_ERROR([--with-nvml-lib needs to be specified])
fi

if test "x$nvmllib" != "xnone" && test "x$nvmlinclude" == "xnone" ;then
  AC_MSG_ERROR([--with-nvml-include needs to be specified])
fi


dnl
dnl enable logging of x attributes for accounting
dnl
AC_MSG_CHECKING([whether to add x attributes to accounting])
AC_ARG_ENABLE(acct-x,
    [  --enable-acct-x         enable adding x attributes to accounting log],
    ACCTX=$enableval,ACCTX=no)
AC_MSG_RESULT($ACCTX)
case "$ACCTX" in
  yes) AC_DEFINE([ATTR_X_ACCT], 1, [Define to add x attributes to accounting log]) ;;
  no) : ;;
  *) AC_MSG_ERROR([--enable-acct-x should be yes or no]) ;;
esac

dnl
dnl enable Cray's CPA support
dnl
AC_MSG_CHECKING([whether to build Cray's CPA support])
AC_ARG_ENABLE(cpa,
    [  --enable-cpa            enable Cray's CPA support],
    CPA=$enableval,CPA=no)   
AC_MSG_RESULT($CPA)

AC_ARG_WITH(cpa_include,
    [  --with-cpa-include=DIR  include path for cpalib.h],
    cpainclude=$withval,cpainclude=none)   
AC_ARG_WITH(cpa_lib,
    [  --with-cpa-lib=DIR      lib path for libcpalib],
    cpalib=$withval,cpalib=none)   

if test "x$cpainclude" != "xnone" ;then
  CPPFLAGS="$CPPFLAGS -I$cpainclude"
fi
LDFLAGS_cpasave="$LDFLAGS"
if test "x$cpalib" != "xnone" ;then
  LDFLAGS="$LDFLAGS -L$cpalib"
fi
buildcpa="no"
case "$CPA" in
  yes) AC_DEFINE([ENABLE_CPA], 1, [Define to enable Cray's CPA support])
    AC_CHECK_HEADERS([cpalib.h], [ :],
      AC_MSG_ERROR([pass the path to cpalib.h to --with-cpa-include]))
    AC_CHECK_LIB(cpalib, cpa_new_node_req, [ :],
      AC_MSG_ERROR([pass the path to libcpalib to --with-cpa-lib]))
      if test "x$cpalib" != "xnone" ;then
        MOMLIBS="$MOMLIBS -L$cpalib -lcpalib"
      else
        MOMLIBS="$MOMLIBS -lcpalib"
      fi
      buildcpa="yes"
      ;;
  no) : ;;
  *) AC_MSG_ERROR([--enable-cpa should be yes or no]) ;;
esac
AM_CONDITIONAL([BUILDCPA], test "$buildcpa" = yes)
LDFLAGS="$LDFLAGS_cpasave"


dnl
dnl enable Cray's CSA support
dnl
AC_MSG_CHECKING([whether to build Cray's CSA support])
AC_ARG_ENABLE(csa,
    [  --enable-csa            enable Cray's CSA support],
    CSA=$enableval,CSA=no)   
AC_MSG_RESULT($CSA)

case "$CSA" in
  yes) AC_DEFINE([ENABLE_CSA], 1, [Define to enable Cray's CSA support])
      LDFLAGS_csasave="$LDFLAGS"
      LDFLAGS="$LDFLAGS -L/opt/cray/job/default/lib64"
      AC_CHECK_LIB([csa], [csa_check],
        [MOMLIBS="$MOMLIBS -L/opt/cray/job/default/lib64 -lcsa"],
        [AC_MSG_ERROR([--enable-csa requires libcsa])])
      LDFLAGS=$LDFLAGS_csasave
      ;;
  no) : ;;
  *) AC_MSG_ERROR([--enable-csa should be yes or no]) ;;
esac

dnl We need pthreads by default in the MOM
  AC_CHECK_LIB(pthread, pthread_create, MOMLIBS="$MOMLIBS -lpthread -lrt")


dnl
dnl enable libjob job_create support
dnl attempt to find a location to install job_create support for CRAY
dnl
AC_MSG_CHECKING([whether to enable libjob job_create support])
jobcreatedir=disabled
AC_ARG_WITH(job_create,
    [  --with-job-create=DIR   Directory that holds the job create modules.
                          On Linux, 'yes' uses the default /opt/cray.],
    [jobcreatedir=$withval])
case "$jobcreatedir" in
  disabled) ;;
  no) jobcreatedir=disabled ;;
  yes)
    case "${PBS_MACH}" in
      linux) AC_DEFINE([USEJOBCREATE], 1, [Define to enable libjob job_create support])
        AC_CHECK_LIB([job],[job_create],
          [MOMLIBS="-L/opt/cray/job/default/lib64 -ljob $MOMLIBS"
          CFLAGS="$CFLAGS -I/opt/cray/job/default/include"],
          [AC_MSG_ERROR([--enable-job-create requires libjob support]) ]);;
      *) AC_MSG_ERROR([--with-job-create takes a full path to a directory]);;
    esac ;;
  *)
    case "${PBS_MACH}" in
      linux) AC_DEFINE([USEJOBCREATE], 1, [Define to enable libjob job_create support])
        MOMLIBS="$jobcreatedir $MOMLIBS"
        CFLAGS="$CFLAGS $jobcreatedir"
        ;;
      *) AC_MSG_ERROR([--with-job-create takes a full path to a directory]);;
    esac ;;
esac
AC_MSG_RESULT([$jobcreatedir])

AUTH_TYPE="classic (pbs_iff)"

dnl compile for munge authorization - allows sites to use munge
dnl as a means to validate user identity from remote hosts. (executable based implementation)
dnl 
AC_ARG_ENABLE(munge_auth_exec, [
  --enable-munge-exec  (LEGACY) Allows users to be authorized using munge instead of ruserok ])
if test "x$GCC" = "xyes" ;then
  AC_MSG_CHECKING([whether to support exec based munge authorization])
  if test "${enable_munge_exec}" = "yes" ; then
    AC_MSG_RESULT([yes])
    CFLAGS="$CFLAGS -DMUNGE_AUTH -DMUNGE_AUTH_EXEC"
    AUTH_TYPE="munge exec (LEGACY)"
  else
    AC_MSG_RESULT([no])
  fi
fi

dnl compile for munge library based authorization - link with munge library
dnl instead of using expensive popen() calls. This option requires munge 
dnl libraries to be installed during compilation.

AC_ARG_ENABLE(munge_auth, [ 
  --enable-munge-auth Use MUNGE authentication instead ruserok method. Munge library is required. 
             This method is much faster than former exec-based implementation ] )

AC_MSG_CHECKING([whether to support MUNGE library based authentication])
if test  "x$GCC" = "xyes" ; then
  
  if test "${enable_munge_auth}" = "yes"; then
    if test x"${enable_munge_exec}" = x"yes"; then
        echo 
        AC_MSG_ERROR([ --enable-munge-auth and --enable-munge-exec options are mutually exclusive])
    fi


    AC_MSG_RESULT([yes])
    AC_CHECK_LIB(munge,munge_ctx_create,LDFLAGS="$LDFLAGS -lmunge")
    AC_CHECK_HEADERS([munge.h])

    if test "$ac_cv_header_munge_h" = "yes" && test "$ac_cv_lib_munge_munge_ctx_create" = "yes"; then 
       CFLAGS="$CFLAGS -DMUNGE_AUTH -DMUNGE_AUTH_LIB"
       AUTH_TYPE="munge library (NEW)"
    else
       echo
       AC_MSG_ERROR([requested munge library API support is disabled - cannot find library or headers])
    fi
  else
    AC_MSG_RESULT([no])
  fi

  AM_CONDITIONAL([WITH_MUNGE_LIBRARY], [ test "${enable_munge_library}" = "yes" ])

fi

dnl
dnl arch-specific libs
dnl
case $PBS_MACH in
  aix4|aix5) MOMLIBS="$MOMLIBS -lodm -lcfg"
    ;;

  irix6array) MOMLIBS="$MOMLIBS -larray"
    ;;

  cygwin) MOMLIBS="$MOMLIBS -liphlpapi"
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
  unicosmk*) AC_DEFINE(SETCONTROLLINGTTY, 1, [Define to set the controlling tty])
    ;;
esac

case $PBS_MACH in
aix4|aix5|sunos4|solaris5|freebsd|freebsd5|darwin|fujitsu)
  AC_CHECK_FUNC(nlist, , AC_CHECK_LIB(elf, nlist)) ;;
esac

case $PBS_MACH in
sunos4|solaris5|netbsd|freebsd|freebsd5)
  AC_CHECK_LIB(kvm, kvm_open) ;;
esac



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
    linux)  if test "$BLCR" = "yes" ;then MOM_CHECKPOINT=1;fi ;;
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



dnl
dnl  ######################################################################
dnl  Scheduler settings
dnl
AC_ARG_WITH(sched, [
  --with-sched=TYPE       sets the scheduler type. If TYPE is
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
if test "x$build_server" != "xyes" ;then
   SCHD_TYPE=none
fi
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

AC_ARG_WITH(sched_code, [
  --with-sched-code=PATH  sets the name of the scheduler to use.
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
AC_MSG_NOTICE([Checking global settings...])

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



AC_ARG_ENABLE(filesync, [
  --enable-filesync       open files with sync on each write operation.  Don't
                          bother enabling this, all it does is slow down TORQUE.
                          This is disabled by default.],
[case "${enableval}" in
  yes) TDISABLEFILESYNC=0; AC_MSG_WARN([--enable-filesync does not actually make for safer writes.]) ;;
  no)  TDISABLEFILESYNC=1 ;;
  *)   AC_MSG_ERROR(--enable-filesync cannot take a value) ;;
esac],[TDISABLEFILESYNC=1])dnl
AC_DEFINE_UNQUOTED(TDISABLEFILESYNC, ${TDISABLEFILESYNC}, [Define to prevent sync writes])



AC_ARG_ENABLE(plock_daemons, [
  --enable-plock-daemons[[=ARG]]
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


AC_ARG_ENABLE(syslog, [
  --enable-syslog         enable (default) the use of syslog for error reporting],
[case "${enableval}" in
  yes) SYSLOG=1 ; RPM_AC_OPTS="$RPM_AC_OPTS --with syslog" ;;
  no)  SYSLOG=0 ; RPM_AC_OPTS="$RPM_AC_OPTS --without syslog" ;;
  *)   AC_MSG_ERROR(--enable-syslog cannot take a value.) ;;
esac],[SYSLOG=1 ; RPM_AC_OPTS="$RPM_AC_OPTS --with syslog"])dnl
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

AC_ARG_WITH(maildomain, [
  --with-maildomain=MAILDOMAIN
                          override the default domain for outgoing mail messages,
                          i.e. "user@maildomain".  The default maildomain is the
                          hostname where the job was submitted from.],
[MAILDOMAIN="${withval}"
AC_MSG_WARN([--with-maildomain is depricated.  Set mail_domain in qmgr.])
AC_DEFINE_UNQUOTED(TMAILDOMAIN, "${MAILDOMAIN}", [override the default domain for outgoing mail messages])])

AC_ARG_ENABLE(spool, [
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

AC_ARG_ENABLE(shell-use-argv, [
  --enable-shell-use-argv enable this to put the job script name on the
                          command line that invokes the shell.  Not on by
                          default.  Ignores --enable-shell-pipe setting.],
[case "${enableval}" in
  yes) SHELL_USE_ARGV=1 ;;
  no)  SHELL_USE_ARGV=0 ;;
  *)   AC_MSG_ERROR(--enable-shell-use-argv cannot take a value) ;;
esac],[SHELL_USE_ARGV=0])dnl
AC_DEFINE_UNQUOTED(SHELL_USE_ARGV, ${SHELL_USE_ARGV}, [job script name passed as the shell's arg])


AC_ARG_ENABLE(posixmemlock, [
  --disable-posixmemlock  disable the moms use of mlockall.  
                          Some versions of OSs seem to have buggy POSIX MEMLOCK.],
[case "${enableval}" in
  yes)  ;;
  no)  NOPOSIXMEMLOCK=1; AC_DEFINE(NOPOSIXMEMLOCK, 1, [Define to disable mlockall]) ;;
  *)   AC_MSG_ERROR(--enable-posixmemlock cannot take a value) ;;
esac])dnl



AC_ARG_ENABLE(privports, [
  --disable-privports     disable the use of privileged ports for authentication.  
                          Some versions of OSX have a buggy bind() and cannot
                          bind to privileged ports.],
[case "${enableval}" in
  yes)  ;;
  no)  NOPRIVPORTS=1; AC_DEFINE(NOPRIVPORTS, 1, [Define to disable privileged ports]) ;;
  *)   AC_MSG_ERROR(--enable-privports cannot take a value) ;;
esac])dnl

AC_ARG_ENABLE(daemons, [
  --disable-daemons       disable the use of background daemons.
                          For Cygwin, disable daemons to run as Windows services],
[case "${enableval}" in
  yes)  ;;
  no)  DISABLE_DAEMONS=1; AC_DEFINE(DISABLE_DAEMONS, 1, [Define to disable daemons]) ;;
  *)   AC_MSG_ERROR(--enable-daemons cannot take a value) ;;
esac])dnl


AC_ARG_ENABLE(mom-checkspool, [
  --disable-mom-checkspool
                          Don't check free space on spool directory and set an error],
[case "${enableval}" in
  yes) MOMCHECKLOCALSPOOL=1 ;;
  no)  MOMCHECKLOCALSPOOL=0;;
  *)   AC_MSG_ERROR(--enable-mom-checkspool cannot take a value) ;;
esac],[MOMCHECKLOCALSPOOL=1])dnl
AC_DEFINE_UNQUOTED(MOMCHECKLOCALSPOOL, ${MOMCHECKLOCALSPOOL}, [Define to error if spool is full on MOM])


AC_ARG_WITH(tmpdir, [
  --with-tmpdir=DIR       set the tmp directory that pbs_mom will use
                          defaults to "/tmp". This is a Cray-specific feature.],
    [TMP_DIR="${withval}"], [TMP_DIR="/tmp"])
test "$TMP_DIR" = yes && TMP_DIR="/tmp"
AC_DEFINE_UNQUOTED(TMP_DIR, "${TMP_DIR}",
		   [The pathname of the Cray temporary directory for mom])

AC_ARG_ENABLE(force-nodefile, [
  --enable-force-nodefile forces creation of nodefile regardless of job
                          submission parameters. Not on by default.],
[case "${enableval}" in
  yes) MOM_FORCENODEFILE=1 ;;
  no)  MOM_FORCENODEFILE=0 ;;
  *)   AC_MSG_ERROR(--enable-force-nodefile cannot take a value) ;;
esac],[MOM_FORCENODEFILE=0])dnl
AC_DEFINE_UNQUOTED(MOM_FORCENODEFILE, ${MOM_FORCENODEFILE}, [force creation of nodefile])



AC_MSG_CHECKING([for CMSG_SPACE])
torque_cv_have_cmsg="no"
  AC_TRY_COMPILE([
  #ifdef HAVE_SYS_UCRED_H
  #include <sys/ucred.h>
  #endif
  #ifdef HAVE_SYS_SOCKET_H
  #define _GNU_SOURCE 1
  #include <sys/socket.h>
  #endif
  #ifdef HAVE_SYS_UN_H
  #include <sys/un.h>
  #endif
  #include <stdlib.h>
  ],[exit(CMSG_SPACE(sizeof(struct ucred)));],
    [torque_cv_have_cmsg="yes"])
AC_MSG_RESULT([$torque_cv_have_cmsg])

AC_MSG_CHECKING([if enabling Unix Domain socket supprt])
ENABLE_UNIX_SOCKETS=$torque_cv_have_cmsg
AC_ARG_ENABLE(unixsockets, [
  --enable-unixsockets    enable the use of Unix Domain sockets for authentication.],
[case "${enableval}" in
  yes) ENABLE_UNIX_SOCKETS=yes ;;
  no) ENABLE_UNIX_SOCKETS=no;;
  *)   AC_MSG_ERROR(--enable-unixsockets cannot take a value) ;;
esac])dnl
if test "x$ENABLE_UNIX_SOCKETS" = "xyes" ;then
   AC_DEFINE(ENABLE_UNIX_SOCKETS, 1, [Define to enable unix domain sockets])
dnl
dnl add _GNU_SOURCE 
dnl
AC_DEFINE([_GNU_SOURCE], 1, [Define _GNU_SOURCE for unix socket messages]) 
fi
AC_MSG_RESULT([$ENABLE_UNIX_SOCKETS])


AC_ARG_WITH(server_home, [
  --with-server-home=DIR  set the server home/spool directory for PBS use
                          defaults to /var/spool/torque],
    [PBS_SERVER_HOME="${withval}" ; RPM_AC_OPTS="$RPM_AC_OPTS --define \"torque_home ${withval}\"" ],
    [PBS_SERVER_HOME="/var/spool/torque"])
AC_SUBST(PBS_SERVER_HOME)

    dnl [PBS_SERVER_HOME="${withval}"], [PBS_SERVER_HOME="$localstatedir/spool/torque"])

AC_ARG_WITH(server_name_file, [
  --with-server-name-file=FILE
                          set the file that will contain the name of
                          the default server for clients to use. If this
                          is not an absolute pathname, it will be evaluated
                          relative to the server home directory that either
                          defaults to /usr/spool/torque or is set using
                          the --with-server-home option to configure. If this
                          option is not specified, the default name for
                          this file will be set to "server_name".],
    [pbs_default_file="${withval}"], [pbs_default_file="server_name"])
case "${pbs_default_file}" in
  /*) PBS_DEFAULT_FILE="${pbs_default_file}" ;;
  *)  PBS_DEFAULT_FILE="\${PBS_SERVER_HOME}/${pbs_default_file}" ;;
esac
AC_SUBST(PBS_DEFAULT_FILE)




AC_ARG_WITH(tcp_retry_limit, [
  --with-tcp-retry-limit=NUMBER
                         set the number of tcp retries. This avoids a rare
                         but potential server hang. When a tcp connection
                         between a mom and the server dies unexpectedly,
                         such as the machine crashing, pbs_server can hang
                         for long periods of time while retrying all possible
                         sockets. This limits those retries.],
    [TCP_RETRY_LIMIT=${withval}],[TCP_RETRY_LIMIT=0])
AC_DEFINE_UNQUOTED(TCP_RETRY_LIMIT,${TCP_RETRY_LIMIT},[Define to set a max retry limit])



AC_ARG_WITH(default_server, [
  --with-default-server=HOSTNAME
                          set the name of the computer that clients will
                          access when no machine name is specified as part
                          of the queue name. It defaults to the hostname
                          of the machine on which PBS is being compiled.],
[PBS_DEFAULT_SERVER="${withval}" ; RPM_AC_OPTS="$RPM_AC_OPTS --define \"torque_server ${withval}\"" ],
[PBS_DEFAULT_SERVER="`uname -n`"])
AC_DEFINE_UNQUOTED(PBS_DEFAULT_SERVER, "${PBS_DEFAULT_SERVER}",
	[Define to the name of the default PBS server])
AC_SUBST(PBS_DEFAULT_SERVER)


AC_ARG_WITH(environ, [
  --with-environ=PATH     set the path containing the environment variables
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


AC_ARG_WITH(qstatrc-file, [
  --with-qstatrc-file=FILE
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


dnl Possibly define default MOM logdir and file suffix
AC_ARG_WITH(momlogdir,
    [  --with-momlogdir        use this directory for MOM logs.],
    AC_DEFINE_UNQUOTED(DEFAULT_MOMLOGDIR,"$with_momlogdir",[Alternate directory for MOM logs])
)
AC_ARG_WITH(momlogsuffix,
    [  --with-momlogsuffix     use this suffix for MOM logs.],
    AC_DEFINE_UNQUOTED(DEFAULT_MOMLOGSUFFIX,"$with_momlogsuffix",[Alternate suffix for MOM logs])
)

dnl Possibly define server checkpoint directory
AC_ARG_WITH(servchkptdir,
    [  --with-servchkptdir     use this directory for Server checkpoint files.],
    AC_DEFINE_UNQUOTED(SERVER_CHKPTDIR,"$with_servchkptdir",[Directory for Server checkpoint files])
)

dnl Declare which of scp, rcp, or mom_rcp is to be used for file delivery
dnl As of TORQUE 2.1.0, default to scp if found, otherwise use the internal mom_rcp
AC_ARG_WITH(scp,
    [  --with-scp              use scp instead of mom_rcp (deprecated, use 
                          --with-rcp=scp).],
  [ AC_MSG_WARN([--with-scp is deprecated, use --with-rcp=scp])
    AC_PATH_PROG(RCP_PATH, "scp", "error")
    case "${RCP_PATH}" in
      error) AC_MSG_ERROR([--with-scp scp not found in path]) ;;
      *)     RCP_PATH="${RCP_PATH}" ;;
    esac])

AC_ARG_WITH(rcp,
    [  --with-rcp              one of "scp", "rcp", "mom_rcp", or the fullpath of
                          a remote file copy program.  scp is the default if
                          found, otherwise mom_rcp is used.  Some rcp programs
                          don't always exit with valid error codes in case of
                          failure.  mom_rcp is a copy of BSD rcp included with
                          this source that has correct error codes, but it is
                          also old, unmaintained, and doesn't have largefile
                          support. ],
    [case "$with_rcp" in
      yes|no) AC_MSG_ERROR([--with-rcp takes an argument]) ;;
      *) RCP_PATH="$with_rcp" ;;
    esac])

# if noone requested an rcp prog, use scp if found, or use mom_rcp
if test "x$RCP_PATH" = "x" ;then
  AC_PATH_PROG(RCP_PATH, scp, error)
  if test "x$RCP_PATH" = "xerror" ;then
    RCP_PATH=mom_rcp
    RPM_AC_OPTS="$RPM_AC_OPTS --without scp"
  else
    RPM_AC_OPTS="$RPM_AC_OPTS --with scp"
  fi
fi

# figure out what was requested and turn that into a full path if necessary
build_pbs_rcp=no
case "$RCP_PATH" in
  mom_rcp|pbs_rcp) RCP_PATH="${sbindir}/pbs_rcp"; build_pbs_rcp=yes ;;
  /*) ;;
  *) AC_PATH_PROG(RCP_PATH, "$RCP_PATH", error)
    if test "x$RCP_PATH" = "xerror" ;then
      AC_MSG_ERROR([requested copy program not found in path])
    fi ;;
esac

# figure out the required args
if test "x$RCP_ARGS" = "x" ;then
  case "$RCP_PATH" in
    *rcp) RCP_ARGS="-rp"; RSH_PATH=rsh ;;
    *scp) RCP_ARGS="-rpB"; RSH_PATH=ssh ;;
    *) RCP_ARGS="-rp"; RSH_PATH=rsh; AC_MSG_WARN([not using rcp or scp, assuming -r]) ;;
  esac
fi
AC_MSG_NOTICE([remote file copy program... $RCP_PATH $RCP_ARGS])

AC_SUBST(RCP_PATH)
AC_SUBST(RCP_ARGS)
AC_SUBST(RSH_PATH)
AC_SUBST(build_pbs_rcp)
AM_CONDITIONAL(INCLUDE_MOM_RCP, [test "x$build_pbs_rcp" = "xyes"])



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


dnl attempt to find a location to install PAM modules
AC_MSG_CHECKING([for PAM install directory])
pammoddir=disabled
AC_ARG_WITH(pam,
    [  --with-pam=DIR          Directory that holds the system PAM modules.
                          On Linux, 'yes' uses the default /lib(64)/security.],
    [pammoddir=$withval])
case "$pammoddir" in
  disabled) ;;
  no) pammoddir=disabled ;;
  yes)
    case "${PBS_MACH}" in
      linux) pammoddir=/lib$libsuff/security;;
      *) AC_MSG_ERROR([--with-pam takes a full path to a directory]);;
    esac ;;
  /*) ;;
  *) AC_MSG_ERROR([--with-pam takes a full path to a directory $pammoddir]);;
esac
if test "x$pammoddir" = "xdisabled" ; then
  RPM_AC_OPTS="$RPM_AC_OPTS --without pam"
else
  RPM_AC_OPTS="$RPM_AC_OPTS --with pam"
fi
AC_MSG_RESULT([$pammoddir])
AC_SUBST(pammoddir)
AM_CONDITIONAL(INCLUDE_PAM, [test "x$pammoddir" != "xdisabled"])
      




dnl find default xauth run-time path
AC_ARG_WITH(xauth,
        [  --with-xauth=PATH       Specify path to xauth program ],
        [
                if test "x$withval" != "xno" ; then
                        xauth_path=$withval
                fi
        ],
        [       
                TestPath="$PATH"
                TestPath="${TestPath}${PATH_SEPARATOR}/usr/X/bin"
                TestPath="${TestPath}${PATH_SEPARATOR}/usr/bin/X11"
                TestPath="${TestPath}${PATH_SEPARATOR}/usr/X11R6/bin"
                TestPath="${TestPath}${PATH_SEPARATOR}/usr/openwin/bin"
                AC_PATH_PROG(xauth_path, xauth, , $TestPath)
                if (test ! -z "$xauth_path" && test -x "/usr/openwin/bin/xauth") ; then
                        xauth_path="/usr/openwin/bin/xauth"
                fi
        ]
)

if test -z "$xauth_path" ; then
        xauth_path="/usr/X11R6/bin/xauth"
fi
AC_DEFINE_UNQUOTED(XAUTH_PATH, "${xauth_path}", [Define to the path of xauth])


dnl  Readline
AC_ARG_WITH(readline,
    [  --without-readline      do not include readline support
                          (default: included if found)],
    [HAVE_READLINE="${withval}"], [HAVE_READLINE="default"])
case "$HAVE_READLINE" in
	yes|no) ;;
        default)
          case $PBS_MACH in
            aix5|darwin) HAVE_READLINE="no";;
            *) HAVE_READLINE="yes";;
          esac
           ;;
	*) AC_MSG_ERROR([--with-readline only takes "yes" or "no" as arguments]) ;;
esac



dnl modulefiles
AC_MSG_CHECKING([whether to install modulefiles])

AC_ARG_WITH(modulefiles,
    AC_HELP_STRING([--with-modulefiles@<:@=DIR@:>@], [use modulefiles in specified directory [[/etc/modulefiles]]]),
[], [with_modulefiles="no"])
if test "$with_modulefiles" != 'no'; then
  if test "$with_modulefiles" = 'yes'; then
    MODULEFILES_DIR="/etc/modulefiles"
  else
    MODULEFILES_DIR="$with_modulefiles"
  fi
else
  MODULEFILES_DIR=no
fi
AM_CONDITIONAL(INSTALL_MODULEFILES, [test "x$MODULEFILES_DIR" != "xno"])
AC_SUBST(MODULEFILES_DIR)
AC_MSG_RESULT($MODULEFILES_DIR)


dnl
dnl  ######################################################################
dnl  Check for libraries
dnl

dnl  nsl is needed on some systems but duplicates libc.a on others (O2000s)
AC_CHECK_FUNC(xdr_int, , AC_CHECK_LIB(nsl, xdr_int))
AC_CHECK_FUNC(ruserok, , AC_CHECK_LIB(socket, ruserok))
AC_CHECK_FUNC(hstrerror, , AC_CHECK_LIB(resolv, hstrerror))

dnl  Readline
if test "$HAVE_READLINE" = "yes" ; then
AC_CHECK_LIB(ncurses, initscr, READLINE_LIBS="-lncurses")
AC_CHECK_LIB(readline, rl_callback_handler_install,
	     [READLINE_LIBS="$READLINE_LIBS -lreadline"],
	     [HAVE_READLINE="no"], [$READLINE_LIBS])
fi
AC_SUBST(READLINE_LIBS)

if test "$ac_cv_header_readline_readline_h" = "no" ; then
	HAVE_READLINE="no"
fi
if test "$HAVE_READLINE" = "yes" ; then
	AC_DEFINE(HAVE_READLINE, 1, [include readline support])
else
	AC_MSG_WARN([readline support is disabled])
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

AC_CHECK_TYPE([socklen_t],
  [AC_DEFINE_UNQUOTED([torque_socklen_t],[socklen_t],[type to use in place of socklen_t])],
  [TAC_SOCKLEN_EQUIV([torque_socklen_t])])

TAC_PAM_GET_USER_2ND_ARG
if test "$tac_pam_get_user_2nd_arg" = "none" && test "$pammoddir" != "disabled" ;then
  AC_MSG_ERROR([Unable to compile PAM bits, missing pam-devel?])
fi

dnl
dnl  ######################################################################
dnl  Check for functions
dnl

dnl  under HP-UX seteuid does not exist, look for setresuid instead
AC_CHECK_FUNCS(seteuid setresuid,break)
AC_CHECK_FUNCS(setegid setresgid,break)


AC_CHECK_FUNCS([gettimeofday bindresvport wordexp poll getaddrinfo])

AC_FUNC_GETGROUPS

dnl see if atexit() exists, if not check for on_exit(), if that fails, panic
AC_CHECK_FUNCS(atexit on_exit, [ { found_an_exit=yes; break; } ])
if test "$found_an_exit" != yes; then
    AC_MSG_ERROR([Must have atexit() or on_exit()])
fi

AC_CHECK_FUNC(ntohl, [ :],
  AC_MSG_CHECKING([for ntohl in arpa/inet.h])
  torque_cv_ntohl_needs_arpa_inet_h="no"
  AC_TRY_COMPILE([
  #ifdef HAVE_SYS_TYPES_H
  #include <sys/types.h>
  #endif
  #ifdef HAVE_NETINET_IN_H
  #include <netinet/in.h>
  #endif
  #ifdef HAVE_ARPA_INET_H
  #include <arpa/inet.h>
  #endif
  ],[exit(ntohl(0));],
    [torque_cv_ntohl_needs_arpa_inet_h="yes"
     AC_DEFINE_UNQUOTED([NTOHL_NEEDS_ARPA_INET_H],1,[Define if ntohl() is declared in arpa/inet.h])])
  AC_MSG_RESULT($torque_cv_ntohl_needs_arpa_inet_h)
)

TAC_TCLTK

dnl
dnl  ######################################################################
dnl Checks needed to build DRMAA
dnl
if test "$build_drmaa" = "yes" ;then
AC_MSG_NOTICE([Configuring DRMAA ...])
AC_PATH_PROG(GPERF, "gperf")
AX_PROG_DOT
AC_PATH_PROG(DOXYGEN, "doxygen", "none")
AC_C_BIGENDIAN_CROSS
ACX_PTHREAD(,[AC_MSG_ERROR([POSIX threads library is required by DRMAA.])])

AC_CHECK_HEADERS([stddef.h limits.h stdint.h inttypes.h])
AC_HEADER_STDBOOL

#AC_FUNC_MALLOC
#AC_FUNC_REALLOC
AC_FUNC_STRERROR_R
AC_FUNC_STRFTIME
AC_CHECK_FUNCS([getcwd strchr strdup strerror mkstemp fstat strlcpy asprintf vasprintf])

AH_TEMPLATE([HAVE_VA_COPY],
                [Define to 1 if you have the va_copy function.])
AC_MSG_CHECKING([for va_copy])
AC_LINK_IFELSE([AC_LANG_PROGRAM([[#include <stdarg.h>]],
                        [[va_list a, b; va_copy(a, b);]])],[
  AC_MSG_RESULT([yes])
  AC_DEFINE([HAVE_VA_COPY])
  ],[
      AC_MSG_RESULT([no])
  ],[
      AC_MSG_RESULT([no])
])

AH_TEMPLATE([HAVE___VA_COPY],
                [Define to 1 if you have the __va_copy function.])
AC_MSG_CHECKING([for __va_copy])
AC_LINK_IFELSE([AC_LANG_PROGRAM([[#include <stdarg.h>]],
                        [[va_list a, b; __va_copy(a, b);]])],[
  AC_MSG_RESULT([yes])
  AC_DEFINE([HAVE___VA_COPY])
        AH_BOTTOM([
#ifndef HAVE_VA_COPY
#       define va_copy(a,b) __va_copy(a,b)
#endif
])
  ],[
  AC_MSG_RESULT([no])
  ], [
  AC_MSG_RESULT([no])
])

  build_drmaa_docs=yes
  RPM_AC_OPTS="$RPM_AC_OPTS --with drmaa"
else  
  DOXYGEN=none
  build_drmaa_docs=no
  RPM_AC_OPTS="$RPM_AC_OPTS --without drmaa"
fi
AM_CONDITIONAL(DRMAA_DOCS, [test "$DOXYGEN" != "none"])
AC_SUBST(build_drmaa_docs)
drmaadocdir=$datadir/doc/$PACKAGE-drmaa
AC_SUBST(drmaadocdir)


dnl
dnl  ######################################################################
dnl  Output
dnl

AC_CREATE_GENERIC_CONFIG([pbs],[-ltorque])

dnl these are quoted, and this removes them
SPEC_NAME=$PACKAGE_TARNAME
SPEC_VERSION=$PACKAGE_VERSION
AC_SUBST(SPEC_NAME)
AC_SUBST(SPEC_VERSION)
AC_SUBST(RPM_AC_OPTS)

dnl  Head and bottom for the config-file
AH_TOP([#ifndef _PBS_CONFIG_H_
#define _PBS_CONFIG_H_])

AH_BOTTOM([
#ifndef __GNUC__
#       define __attribute__ /* nothing */
#endif
])

AH_BOTTOM([
#endif /* _PBS_CONFIG_H_ */
])


dnl  Include the Cplant Fault Recovery Patch jugglery
AH_VERBATIM([NEED_BLOCKING_CONNECTIONS],
[#ifndef NEED_BLOCKING_CONNECTIONS
#define write(a,b,c) write_nonblocking_socket(a,b,c)
#define read(a,b,c) read_nonblocking_socket(a,b,c)
#endif])

pbs_build_host=`uname -n`
pbs_build_date=`date`
pbs_build_dir=`pwd`
pbs_source_dir=`cd $srcdir && pwd`
pbs_config_args=$@
AC_DEFINE_UNQUOTED([PBS_BUILD_HOST],"${pbs_build_host}",[Define to the build hostname])
AC_DEFINE_UNQUOTED([PBS_BUILD_DATE],"${pbs_build_date}",[Define to the build date])
AC_DEFINE_UNQUOTED([PBS_BUILD_DIR],"${pbs_build_dir}",[Define to the build directory])
AC_DEFINE_UNQUOTED([PBS_SOURCE_DIR],"${pbs_source_dir}",[Define to the source directory])
AC_DEFINE_UNQUOTED([PBS_BUILD_USER],"${USER}",[Define to the building username])
AC_DEFINE_UNQUOTED([PBS_INSTALL_DIR],"${prefix}",[Define to the install directory])
AC_DEFINE_UNQUOTED([PBS_CONFIG_ARGS],"${ac_configure_args}",[Define to the install directory])
AC_DEFINE_UNQUOTED([PBS_CFLAGS],"${CFLAGS}",[Define to the install directory])

AC_CONFIG_FILES([torque.spec:buildutils/torque.spec.in])
AC_OUTPUT(buildutils/pbs_mkdirs
	  buildutils/self-extract-head-sh
	  buildutils/modulefiles
	  buildutils/modulefiles.vers
	  Makefile
	  contrib/blcr/Makefile
	  doc/Makefile
	  doc/man1/Makefile
	  doc/man3/Makefile
	  doc/man7/Makefile
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
	  src/lib/Libcsv/Makefile
	  src/lib/Libdis/Makefile
	  src/lib/Libifl/Makefile
	  src/lib/Liblog/Makefile
	  src/lib/Libnet/Makefile
	  src/lib/Libpbs/Makefile
	  src/lib/Libsite/Makefile
	  src/lib/Libutils/Makefile
	  src/mom_rcp/Makefile
	  src/momctl/Makefile
	  src/resmom/Makefile
	  src/resmom/aix4/Makefile
	  src/resmom/aix5/Makefile
	  src/resmom/cygwin/Makefile
	  src/resmom/darwin/Makefile
	  src/resmom/digitalunix/Makefile
	  src/resmom/freebsd/Makefile
	  src/resmom/freebsd5/Makefile
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
	  src/tools/xpbsmon/Makefile
	  src/drmaa/Makefile
	  src/drmaa/src/Makefile
	  src/drmaa/test/Makefile
	  src/drmaa/Doxyfile
	  src/pam/Makefile)

echo
echo "Building components: server=$build_server mom=$build_mom clients=$build_clients 
                     gui=$build_gui drmaa=$build_drmaa pam=`test "x$pammoddir" = "xdisabled" && echo no || echo yes`"
echo "PBS Machine type: $PBS_MACH"
echo "Remote copy: $RCP_PATH $RCP_ARGS"
echo "PBS home: $PBS_SERVER_HOME"
echo "Default server: $PBS_DEFAULT_SERVER"
echo "Unix Domain sockets: $ENABLE_UNIX_SOCKETS"
echo "Tcl: `test "$TCL" = "1" && echo $MY_TCL_INCS $MY_TCL_LIBS || echo disabled`"
echo "Tk: `test "$TK" = "1" && echo $MY_TCLTK_INCS $MY_TCLTK_LIBS || echo disabled`"
echo "Authentication: $AUTH_TYPE"
echo

if test "x$gccwarnings" = "xyes" ;then
   AC_MSG_WARN([This compilation has strict compiler options enabled that cause
the build to fail if any compiler warnings are emitted.  If this build fails
because of a harmless warning, please report the problem to $PACKAGE_BUGREPORT
and run configure again with --disable-gcc-warnings.])
   echo
fi
echo "Ready for '${MAKE-make}'."
