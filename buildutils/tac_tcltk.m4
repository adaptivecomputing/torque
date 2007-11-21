dnl
dnl  ######################################################################
dnl  Tcl/Tk settings
dnl
AC_DEFUN([TAC_TCLTK],[
AC_MSG_NOTICE([Starting Tcl/Tk configuration])

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
	# find and load tclxConfig.sh which only exists for older versions
	TEA_PATH_CONFIG([tclx])
        if test "x$tclx_BIN_DIR" != "xnone" ; then
	  TEA_LOAD_CONFIG([tclx])

          orig_CFLAGS="$CFLAGS"
          CFLAGS="$TCL_INCLUDES"
          AC_CHECK_HEADER([tclExtend.h], [TCLX=1], [TCLX=0])
          CFLAGS="$orig_CFLAGS"
        else
          TCLX=0
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



dnl  Now assemble appropriate compiler flags for includes and linking.
MY_TCL_LIBS=""
MY_TCL_INCS=""

if test "${TCL}" = "1"; then
	MY_TCL_LIBS="${MY_TCL_LIBS} ${TCL_LIB_SPEC} ${TCL_LIBS}"
	MY_TCL_INCS="${MY_TCL_INCS} ${TCL_INCLUDES}"
else
	AC_MSG_NOTICE([Tcl support disabled])
fi

if test "${TCLX}" = "1"; then
	MY_TCL_LIBS="${TCLX_LIB_SPEC} ${TCLX_LIBS} ${MY_TCL_LIBS}"
fi

if test "${TK}" = "1"; then
	MY_TCLTK_LIBS="${TK_LIB_SPEC} ${TK_XLIBSW} ${MY_TCL_LIBS}"
	MY_TCLTK_INCS="${MY_TCL_INCS} ${TK_INCLUDES} ${TK_XINCLUDES}"
else
	AC_MSG_NOTICE([Tk support disabled])
fi

if test "${TKX}" = "1"; then
	MY_TCLTK_LIBS="${TKX_LIB_SPEC} ${TKX_LIBS} ${MY_TCLTK_LIBS}"
fi

if test "${TCL}" = "1"; then
  # try to hack in a run-time search path if needed
  T_ADD_RPATH([MY_TCL_LIBS])
  
  orig_LIBS="$LIBS"
  orig_CFLAGS="$CFLAGS"
  LIBS="$LIBS $MY_TCL_LIBS"
  CFLAGS="$CFLAGS $MY_TCL_INCS"
  AC_CHECK_FUNC(Tcl_Init, ,[
	if test "x$user_requested_tcl" = "xyes" ;then
          AC_MSG_ERROR([Your Tcl install is broken.  Rerun configure with --without-tcl])
        else
          AC_MSG_NOTICE([Your Tcl install is broken.  Disabling Tcl support.])
          TCL=0; TCLX=0; TK=0; TKX=0;
	fi])
  LIBS="$orig_LIBS"
  CFLAGS="$orig_CFLAGS"
fi

if test "${TK}" = "1"; then
  # try to hack in a run-time search path if needed
  T_ADD_RPATH([MY_TCLTK_LIBS])

  orig_LIBS="$LIBS"
  orig_CFLAGS="$CFLAGS"
  LIBS="$LIBS $MY_TCLTK_LIBS"
  CFLAGS="$CFLAGS $MY_TCLTK_INCS"
  AC_CHECK_FUNC(Tk_Init, ,[
	if test "x$user_requested_tk" = "xyes" ;then
          AC_MSG_ERROR([Your Tk install is broken.  Rerun configure with --without-tk])
        else
          AC_MSG_NOTICE([Your Tk install is broken.  Disabling Tk support.])
           TK=0; TKX=0;
	fi])
  LIBS="$orig_LIBS"
  CFLAGS="$orig_CFLAGS"
fi

dnl  It is possible that the user has asked for a gui but no Tk
dnl  libraries were found. In this case we must disable the gui.
AC_MSG_CHECKING([whether to include the GUI-clients])
AC_ARG_ENABLE(gui,
    [  --disable-gui           do not include the GUI-clients],
    build_gui=$enableval)
if test "x${build_clients}" != "xyes" -a "x${build_gui}" = "xyes"; then
	AC_MSG_ERROR([the GUI can not be build independent of the clients])
fi
if test "x${build_clients}" != "xyes" -a "x${build_gui}" = "x"; then
	build_gui=no 
fi
if test "x${build_gui}" = "xyes" -a "${TK}" != "1"; then
	AC_MSG_ERROR([cannot build GUI without Tk library])
fi
if test "x${build_gui}" = "x" -a "${TK}" = "1"; then
	build_gui=yes
fi
if test "x${build_gui}" = "x"; then
	build_gui=no
fi
AC_MSG_RESULT($build_gui)
AC_SUBST(build_gui)
AM_CONDITIONAL(INCLUDE_GUI, [test "x$build_gui" = "xyes"])


AC_DEFINE_UNQUOTED(TCL, ${TCL}, [Define if PBS should use Tcl in its tools])
AC_DEFINE_UNQUOTED(TCLX, ${TCLX}, [Define if PBS should use TclX in its tools])
AC_DEFINE_UNQUOTED(TK, ${TK}, [Define if PBS should use Tk in its tools])
AC_DEFINE_UNQUOTED(TKX, ${TKX}, [Define if PBS should use Tkx in its tools])

AC_SUBST(MY_TCL_LIBS)
AC_SUBST(MY_TCL_INCS)
AC_SUBST(MY_TCLTK_LIBS)
AC_SUBST(MY_TCLTK_INCS)
AM_CONDITIONAL(USING_TCL, [test "$TCL" = "1"])
AM_CONDITIONAL(USING_TK, [test "$TK" = "1"])
use_tcl=$TCL
use_tk=$TK
AC_SUBST(use_tcl)
AC_SUBST(use_tk)

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
    AC_HELP_STRING([--with-tclatrsep=CHAR],[set the Tcl attribute separator character
                        this will default to "." if unspecified]),
    [tcl_atrsep="${withval}"], [tcl_atrsep="."])
if test "$TCL" = "1" ; then
AC_MSG_CHECKING([checking for Tcl attribute seperator])
AC_MSG_RESULT([$tcl_atrsep])
fi
AC_DEFINE_UNQUOTED(TCL_ATRSEP, "${tcl_atrsep}",
	[the seperator for Tcl attribute parsing])


AC_ARG_ENABLE(tcl-qstat,
   [  --enable-tcl-qstat      setting this builds qstat with Tcl interpreter
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

if test "$SCHD_TYPE" = "tcl" -a "$TCL" = "0" ;then
  AC_MSG_ERROR([Tcl scheduler cannot be built without Tcl support])
fi

AC_MSG_NOTICE([Finished Tcl/Tk configuration])

])
