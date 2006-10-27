
AC_DEFUN([AX_PROG_GPERF], [
	AC_MSG_CHECKING([for gperf])
	if { echo a; echo b; echo c; } | gperf >/dev/null 2>&1; then
		HAVE_GPERF=yes
		GPERF=gperf
	else
		if echo $srcdir | grep -q "^/"; then
			abs_srcdir="$srcdir"
		else
			abs_srcdir="`pwd`/$srcdir"
		fi
		GPERF="${abs_srcdir}/m4/gperf-fallback.sh"
		HAVE_GPERF=no
	fi
	AC_SUBST(GPERF)
	AC_SUBST(HAVE_GPERF)
	AC_MSG_RESULT([$HAVE_GPERF])
])

