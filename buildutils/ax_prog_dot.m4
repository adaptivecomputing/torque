
AC_DEFUN([AX_PROG_DOT], [
	AC_MSG_CHECKING([for dot])
	if dot -V >/dev/null 2>&1; then
		HAVE_DOT=yes
	else
		HAVE_DOT=no
	fi
	AC_SUBST(HAVE_DOT)
	AC_MSG_RESULT([$HAVE_DOT])
])
