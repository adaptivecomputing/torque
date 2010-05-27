m4_include([buildutils/tcl.m4])
m4_include([buildutils/ac_create_generic_config.m4])
m4_include([buildutils/acx_pthread.m4])
m4_include([buildutils/ax_prog_dot.m4])
m4_include([buildutils/ac_c_bigendian_cross.m4])
m4_include([buildutils/t_add_rpath.m4])
m4_include([buildutils/tac_tcltk.m4])
m4_include([buildutils/ax_cflags_gcc_option.m4])


dnl
dnl  Test to see whether h_errno is visible when netdb.h is included.
dnl  At least under HP-UX 10.x this is not the case unless 
dnl  XOPEN_SOURCE_EXTENDED is declared but then other nasty stuff happens.
dnl  The appropriate thing to do is to call this macro and then
dnl  if it is not available do a "extern int h_errno;" in the code.
dnl
dnl  This test was taken from the original OpenPBS buildsystem
dnl
AC_DEFUN([AC_DECL_H_ERRNO],
[AC_CACHE_CHECK([for h_errno declaration in netdb.h],
  ac_cv_decl_h_errno,
[AC_TRY_COMPILE([#include <sys/types.h>
#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif
#include <netdb.h>
], [int _ZzQ = (int)(h_errno + 1);],
  ac_cv_decl_h_errno=yes, ac_cv_decl_h_errno=no)])
if test $ac_cv_decl_h_errno = yes; then
  AC_DEFINE(H_ERRNO_DECLARED, 1,
            [is the global int h_errno declared in netdb.h])
fi
])dnl


dnl
dnl  On some systems one needs to include sys/select.h to get the
dnl  definition of FD_* macros for select bitmask handling.
dnl 
dnl  This test was taken from the original OpenPBS buildsystem
dnl
AC_DEFUN([AC_DECL_FD_SET_SYS_SELECT_H],
[AC_CACHE_CHECK([for FD_SET declaration in sys/select.h],
  ac_cv_decl_fdset_sys_select_h,
[AC_EGREP_CPP(oh_yeah, [#include <sys/select.h> 
#ifdef FD_SETSIZE
oh_yeah
#endif
], ac_cv_decl_fdset_sys_select_h=yes, ac_cv_decl_fdset_sys_select_h=no)])
if test $ac_cv_decl_fdset_sys_select_h = yes; then
  AC_DEFINE(FD_SET_IN_SYS_SELECT_H, 1,
            [are FD_SET and friends defined in sys/select.h])
fi
])dnl


dnl
dnl  Old compilers do not support C9X style __func__ identifiers so
dnl  check if this one is able to.
dnl
dnl  This test was first written by Christopher Currie.
dnl
AC_DEFUN([AC_C_VAR_FUNC],
[AC_REQUIRE([AC_PROG_CC])
AC_CACHE_CHECK(whether $CC recognizes __func__, ac_cv_c_var_func,
AC_TRY_COMPILE(,[const char *s = __func__;],
AC_DEFINE(HAVE_FUNC, 1,
[Define if the C complier supports __func__]) ac_cv_c_var_func=yes,
ac_cv_c_var_func=no) )
])dnl


dnl
dnl  Some compilers do not support GNU style __FUNCTION__ identifiers so
dnl  check if this one is able to.
dnl
dnl  This test is just copy'n'paste of the above one.
dnl
AC_DEFUN([AC_C_VAR_FUNCTION],
[AC_REQUIRE([AC_PROG_CC])
AC_CACHE_CHECK(whether $CC recognizes __FUNCTION__, ac_cv_c_var_function,
AC_TRY_COMPILE(,[const char *s = __FUNCTION__;],
AC_DEFINE(HAVE_FUNCTION, 1,
[Define if the C complier supports __FUNCTION__]) ac_cv_c_var_function=yes,
ac_cv_c_var_function=no) )
])dnl



dnl Check for socklen_t: historically on BSD it is an int, and in
dnl POSIX 1g it is a type of its own, but some platforms use different
dnl types for the argument to getsockopt, getpeername, etc.  So we
dnl have to test to find something that will work.

dnl Remove the AC_CHECK_TYPE - on HP-UX it would find a socklen_t, but the 
dnl function prototypes for getsockopt et al will not actually use 
dnl socklen_t args unless _XOPEN_SOURCE_EXTENDED is defined. so, the
dnl AC_CHECK_TYPE will find a socklen_t and think all is happiness and
dnl joy when you will really get warnings about mismatch types - type
dnl mismatches that would be possibly Bad (tm) in a 64-bit compile.
dnl raj 2005-05-11 this change may be redistributed at will 

dnl also, added "extern" to the "int getpeername" in an attempt to resolve                         
dnl an issue with this code under Solaris 2.9.  this too may be 
dnl redistributed at will

AC_DEFUN([TAC_SOCKLEN_EQUIV],
[AC_REQUIRE([AC_PROG_CC])
      AC_MSG_CHECKING([for socklen_t equivalent])
      AC_CACHE_VAL([curl_cv_socklen_t_equiv],
      [
         # Systems have either "struct sockaddr *" or
         # "void *" as the second argument to getpeername
         curl_cv_socklen_t_equiv=
         for arg2 in "struct sockaddr" void; do
            for t in int size_t unsigned long "unsigned long" socklen_t; do
               AC_TRY_COMPILE([
                  #ifdef HAVE_SYS_TYPES_H
                  #include <sys/types.h>
                  #endif
                  #ifdef HAVE_SYS_SOCKET_H
                  #include <sys/socket.h>
                  #endif

                  extern int getpeername (int, $arg2 *, $t *);
               ],[
                  $t len;
                  getpeername(0,0,&len);
               ],[
                  curl_cv_socklen_t_equiv="$t"
                  break
               ])
            done
         done

         if test "x$curl_cv_socklen_t_equiv" = x; then
         # take a wild guess
            curl_cv_socklen_t_equiv="socklen_t"
            AC_MSG_WARN([Cannot find a type to use in place of socklen_t, guessing socklen_t])
         fi
      ])
      AC_MSG_RESULT($curl_cv_socklen_t_equiv)
      AC_DEFINE_UNQUOTED($1, $curl_cv_socklen_t_equiv,
                        [type to use in place of socklen_t if not defined])
])




dnl
dnl largefile support
dnl
dnl We can't just use AC_SYS_LARGEFILE because that breaks kernel ABIs on Solaris.
dnl Instead, we just figure out if we have stat64() and stat64.st_mode.
AC_DEFUN([TAC_SYS_LARGEFILE],[
orig_CFLAGS="$CFLAGS"
AC_CHECK_FUNC(stat64,
  AC_DEFINE(HAVE_STAT64,1,[Define if stat64() is available]),
  [CFLAGS="$CFLAGS -D_LARGEFILE64_SOURCE"
   unset ac_cv_func_stat64
   AC_CHECK_FUNC(stat64,
    AC_DEFINE(HAVE_STAT64,1,[Define if stat64() is available]),
     [CFLAGS="$orig_CFLAGS"])])

AC_CHECK_FUNC(open64,
  AC_DEFINE(HAVE_OPEN64,1,[Define if open64() is available]))

AC_CHECK_FUNC(lseek64,
  AC_DEFINE(HAVE_LSEEK64,1,[Define if lseek64() is available]))

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

AC_MSG_CHECKING([if largefile compiles (looking at you, OSX)])
AC_COMPILE_IFELSE([ 
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

struct stat64 file_stat;
int main(void) {
  stat64("config.log", &file_stat);
  return(0);
  }
               ],[
                  AC_MSG_RESULT([yes])
                  AC_DEFINE([LARGEFILE_WORKS],1,[Define if open64, stat64, and friends work])
               ],[AC_MSG_RESULT([no])])
])


dnl
dnl remove annoying "lib not installed" warning
dnl
AC_DEFUN([TAC_PROG_LIBTOOL_PATCH],[
  # patch libtool to remove that annoying lib install warning
  test -f libtool.old || (mv libtool libtool.old && cp libtool.old libtool)
  sed -e '/.*has not been installed in.*/d' <libtool >libtool.new
  (test -s libtool.new || rm libtool.new) 2>/dev/null
  test -f libtool.new && mv libtool.new libtool # not 2>/dev/null !!
  test -f libtool     || mv libtool.old libtool
  test -f libtool.old && rm -f libtool.old
])


dnl
dnl Test the 2nd arg to pam_get_user to be char or const char
dnl
AC_DEFUN([TAC_PAM_GET_USER_2ND_ARG],
[AC_MSG_CHECKING([for pam_get_user() calling type])
tac_pam_get_user_2nd_arg=none
 for t in "char" "const char" ; do
    AC_TRY_COMPILE([ 
#ifdef HAVE_SECURITY_PAM_APPL_H 
#include <security/pam_appl.h>
#endif
  
#ifdef HAVE_SECURITY_PAM_MODULES_H
#include <security/pam_modules.h>
#else
#ifdef HAVE_PAM_PAM_MODULES_H
#include <pam/pam_modules.h>
#endif
#endif
  
extern int
pam_get_user(
        pam_handle_t *pamh,             /* PAM handle */
        $t **user,                    /* User Name */
        const char *prompt              /* Prompt */
);

               ],[
                  $t* username;
                  pam_get_user((pam_handle_t*) 1,&username,0);
               ],[
                  tac_pam_get_user_2nd_arg="$t"
                  break
               ])
            done
AC_MSG_RESULT([$tac_pam_get_user_2nd_arg])
AC_DEFINE_UNQUOTED(pam_get_user_2nd_arg_t, $tac_pam_get_user_2nd_arg,
                        [type to use for 2nd arg of pam_get_user])
])

