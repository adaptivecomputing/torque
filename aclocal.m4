dnl
dnl This is a redefinition of the AC_OUTPUT_FILES macros from the
dnl stock autoconf-2.12 acgeneral.m4 file. This has had a very small
dnl modification done to it to support a standard substitution variable
dnl called "top_builddir" which is simply the path to the top of the
dnl build tree. This is useful if some Makefiles reference files in 
dnl the build tree and not the src tree as is the case here where we
dnl do inclusions from the build tree. Unfortunately makes
dnl cannot always do variable expansion on VPATH and "include" lines
dnl because of the unspecified order of evaluation of Makefile lines.
dnl For these two reasons it seems impossible to construct a path to
dnl the root of the build tree.
dnl
dnl If you want to modify a newer acgeneral.m4 file from a newer autoconf
dnl sometime in the future (in case there are significant structural
dnl changes to the configure files produced by newer autoconfs) take a
dnl look at the simple changes marked with "NAS PBS MODIFICATION"
dnl comments towards the bottom of the AC_OUTPUT_FILES macro.
dnl
dnl lonhyn@nas.nasa.gov
dnl


dnl
dnl First undefine the original AC_OUTPUT_FILES macro
dnl
undefine([AC_OUTPUT_FILES])

dnl Do the variable substitutions to create the Makefiles or whatever.
dnl This is a subroutine of AC_OUTPUT.  It is called inside an unquoted
dnl here document whose contents are going into config.status, but
dnl upon returning, the here document is being quoted.
dnl AC_OUTPUT_FILES(FILE...)
define(AC_OUTPUT_FILES,
[# Protect against being on the right side of a sed subst in config.status.
changequote(, )dnl
sed 's/%@/@@/; s/@%/@@/; s/%g\$/@g/; /@g\$/s/[\\\\&%]/\\\\&/g;
 s/@@/%@/; s/@@/@%/; s/@g\$/%g/' > conftest.subs <<\\CEOF
changequote([, ])dnl
dnl These here document variables are unquoted when configure runs
dnl but quoted when config.status runs, so variables are expanded once.
$ac_vpsub
dnl Shell code in configure.in might set extrasub.
$extrasub
dnl Insert the sed substitutions of variables.
dnl undivert(AC_DIVERSION_SED)
undivert(AC_DIVERSION_SED)
CEOF
EOF

cat >> $CONFIG_STATUS <<\EOF

# Split the substitutions into bite-sized pieces for seds with
# small command number limits, like on Digital OSF/1 and HP-UX.
ac_max_sed_cmds=90 # Maximum number of lines to put in a sed script.
ac_file=1 # Number of current file.
ac_beg=1 # First line for current file.
ac_end=$ac_max_sed_cmds # Line after last line for current file.
ac_more_lines=:
ac_sed_cmds=""
while $ac_more_lines; do
  if test $ac_beg -gt 1; then
    sed "1,${ac_beg}d; ${ac_end}q" conftest.subs > conftest.s$ac_file
  else
    sed "${ac_end}q" conftest.subs > conftest.s$ac_file
  fi
  if test ! -s conftest.s$ac_file; then
    ac_more_lines=false
    rm -f conftest.s$ac_file
  else
    if test -z "$ac_sed_cmds"; then
      ac_sed_cmds="sed -f conftest.s$ac_file"
    else
      ac_sed_cmds="$ac_sed_cmds | sed -f conftest.s$ac_file"
    fi
    ac_file=`expr $ac_file + 1`
    ac_beg=$ac_end
    ac_end=`expr $ac_end + $ac_max_sed_cmds`
  fi
done
if test -z "$ac_sed_cmds"; then
  ac_sed_cmds=cat
fi
EOF

cat >> $CONFIG_STATUS <<EOF

CONFIG_FILES=\${CONFIG_FILES-"$1"}
EOF
cat >> $CONFIG_STATUS <<\EOF
for ac_file in .. $CONFIG_FILES; do if test "x$ac_file" != x..; then
changequote(, )dnl
  # Support "outfile[:infile[:infile...]]", defaulting infile="outfile.in".
  case "$ac_file" in
  *:*) ac_file_in=`echo "$ac_file"|sed 's%[^:]*:%%'`
       ac_file=`echo "$ac_file"|sed 's%:.*%%'` ;;
  *) ac_file_in="${ac_file}.in" ;;
  esac

  # Adjust a relative srcdir, top_srcdir, and INSTALL for subdirectories.

  # Remove last slash and all that follows it.  Not all systems have dirname.
  ac_dir=`echo $ac_file|sed 's%/[^/][^/]*$%%'`
changequote([, ])dnl
  if test "$ac_dir" != "$ac_file" && test "$ac_dir" != .; then
    # The file is in a subdirectory.
    test ! -d "$ac_dir" && mkdir "$ac_dir"
    ac_dir_suffix="/`echo $ac_dir|sed 's%^\./%%'`"
    # A "../" for each directory in $ac_dir_suffix.
changequote(, )dnl
    ac_dots=`echo $ac_dir_suffix|sed 's%/[^/]*%../%g'`
changequote([, ])dnl
  else
    ac_dir_suffix= ac_dots=
  fi

  case "$ac_given_srcdir" in
  .)  srcdir=.
      if test -z "$ac_dots"; then top_srcdir=.
      else top_srcdir=`echo $ac_dots|sed 's%/$%%'`; fi ;;
  /*) srcdir="$ac_given_srcdir$ac_dir_suffix"; top_srcdir="$ac_given_srcdir" ;;
  *) # Relative path.
    srcdir="$ac_dots$ac_given_srcdir$ac_dir_suffix"
    top_srcdir="$ac_dots$ac_given_srcdir" ;;
  esac

ifdef([AC_PROVIDE_AC_PROG_INSTALL],
[  case "$ac_given_INSTALL" in
changequote(, )dnl
  [/$]*) INSTALL="$ac_given_INSTALL" ;;
changequote([, ])dnl
  *) INSTALL="$ac_dots$ac_given_INSTALL" ;;
  esac
])dnl

  echo creating "$ac_file"
  rm -f "$ac_file"
  configure_input="Generated automatically from `echo $ac_file_in|sed 's%.*/%%'` by configure."
  case "$ac_file" in
  *Makefile*) ac_comsub="1i\\
# $configure_input" ;;
  *) ac_comsub= ;;
  esac

  # start NAS PBS MODIFICATION
  # if ac_dots is nothing make the builddir "." to prevent accidental 
  # creation of an absolute path if someone puts a "/..." after it
  # otherwise strip the trailing slashes for aesthetic reasons
  if test -z "${ac_dots}"; then
    top_builddir=.
  else
    top_builddir=`echo "${ac_dots}" | sed 's%/*$%%'`
  fi
  #  end  NAS PBS MODIFICATION

  ac_file_inputs=`echo $ac_file_in|sed -e "s%^%$ac_given_srcdir/%" -e "s%:% $ac_given_srcdir/%g"`
# NAS PBS MODIFICATION (5 lines down, the line with the $ac_dots stuff)
  sed -e "$ac_comsub
s%@configure_input@%$configure_input%g
s%@srcdir@%$srcdir%g
s%@top_srcdir@%$top_srcdir%g
s%@top_builddir@%$top_builddir%g
ifdef([AC_PROVIDE_AC_PROG_INSTALL], [s%@INSTALL@%$INSTALL%g
])dnl
dnl The parens around the eval prevent an "illegal io" in Ultrix sh.
" $ac_file_inputs | (eval "$ac_sed_cmds") > $ac_file
dnl This would break Makefile dependencies.
dnl  if cmp -s $ac_file conftest.out 2>/dev/null; then
dnl    echo "$ac_file is unchanged"
dnl    rm -f conftest.out
dnl   else
dnl     rm -f $ac_file
dnl    mv conftest.out $ac_file
dnl  fi
fi; done
rm -f conftest.s*
])


dnl
dnl The AC_ARG_SET macro behaves somewhat like AC_ARG_WITH except it
dnl allows you to set a flag without the "with" or "without" prefixes.
dnl Oftentimes one wants to specify an arbitrary option that does
dnl not fit the "with" or "enable" paradigm. This allows you to set an
dnl option "foo" with a possible value. It can also default to a value.
dnl If no value is specified then OPTION_NAME is set to "yes".
dnl One uses a "--set-" prefix followed by the option name and a possible
dnl value.
dnl
dnl    --set-OPTION_NAME[=VALUE]
dnl
dnl Unfortunately the AC_INIT_PARSE_ARGS needs to be redefined to support
dnl the parsing of this type of flag.
dnl

dnl AC_ARG_SET(OPTION, HELP-STRING, ACTION-IF-TRUE [, ACTION-IF-FALSE])
AC_DEFUN(AC_ARG_SET,
[AC_DIVERT_PUSH(AC_DIVERSION_NOTICE)dnl
ac_help="$ac_help
[$2]"
AC_DIVERT_POP()dnl
[#] Check whether --set-[$1] was given.
if test "[${set_]patsubst([$1], -, _)+zzq}" = zzq; then
  setval="[$set_]patsubst([$1], -, _)"
  ifelse([$3], , :, [$3])
ifelse([$4], , , [else
  $4
])dnl
fi
])


dnl
dnl redefined version of the AC_INIT_PARSE_ARGS() macro. This handles the
dnl parsing of "--set-" style arguments. This is accomplished by adding
dnl a hook to the normal macro. First undefine, then redefine it.
dnl
dnl new AC_INIT_PARSE_ARGS()
dnl

undefine([AC_INIT_PARSE_ARGS])

AC_DEFUN(AC_INIT_PARSE_ARGS,
[
# Initialize some variables set by options.
# The variables have the same names as the options, with
# dashes changed to underlines.
build=NONE
cache_file=./config.cache
exec_prefix=NONE
host=NONE
no_create=
nonopt=NONE
no_recursion=
prefix=NONE
program_prefix=NONE
program_suffix=NONE
program_transform_name=s,x,x,
silent=
site=
srcdir=
target=NONE
verbose=
x_includes=NONE
x_libraries=NONE
dnl Installation directory options.
dnl These are left unexpanded so users can "make install exec_prefix=/foo"
dnl and all the variables that are supposed to be based on exec_prefix
dnl by default will actually change.
dnl Use braces instead of parens because sh, perl, etc. also accept them.
bindir='${exec_prefix}/bin'
sbindir='${exec_prefix}/sbin'
libexecdir='${exec_prefix}/libexec'
datadir='${prefix}/share'
sysconfdir='${prefix}/etc'
sharedstatedir='${prefix}/com'
localstatedir='${prefix}/var'
libdir='${exec_prefix}/lib'
includedir='${prefix}/include'
oldincludedir='/usr/include'
infodir='${prefix}/info'
mandir='${prefix}/man'

# Initialize some other variables.
subdirs=
MFLAGS= MAKEFLAGS=
# Maximum number of lines to put in a shell here document.
ac_max_here_lines=12

ac_prev=
for ac_option
do

  # If the previous option needs an argument, assign it.
  if test -n "$ac_prev"; then
    eval "$ac_prev=\$ac_option"
    ac_prev=
    continue
  fi

  case "$ac_option" in
changequote(, )dnl
  -*=*) ac_optarg=`echo "$ac_option" | sed 's/[-_a-zA-Z0-9]*=//'` ;;
changequote([, ])dnl
  *) ac_optarg= ;;
  esac

  # Accept the important Cygnus configure options, so we can diagnose typos.

  case "$ac_option" in

  -bindir | --bindir | --bindi | --bind | --bin | --bi)
    ac_prev=bindir ;;
  -bindir=* | --bindir=* | --bindi=* | --bind=* | --bin=* | --bi=*)
    bindir="$ac_optarg" ;;

  -cache-file | --cache-file | --cache-fil | --cache-fi \
  | --cache-f | --cache- | --cache | --cach | --cac | --ca | --c)
    ac_prev=cache_file ;;
  -cache-file=* | --cache-file=* | --cache-fil=* | --cache-fi=* \
  | --cache-f=* | --cache-=* | --cache=* | --cach=* | --cac=* | --ca=* | --c=*)
    cache_file="$ac_optarg" ;;

  -datadir | --datadir | --datadi | --datad | --data | --dat | --da)
    ac_prev=datadir ;;
  -datadir=* | --datadir=* | --datadi=* | --datad=* | --data=* | --dat=* \
  | --da=*)
    datadir="$ac_optarg" ;;

  -disable-* | --disable-*)
    ac_feature=`echo $ac_option|sed -e 's/-*disable-//'`
    # Reject names that are not valid shell variable names.
changequote(, )dnl
    if test -n "`echo $ac_feature| sed 's/[-a-zA-Z0-9_]//g'`"; then
changequote([, ])dnl
      AC_MSG_ERROR($ac_feature: invalid feature name)
    fi
    ac_feature=`echo $ac_feature| sed 's/-/_/g'`
    eval "enable_${ac_feature}=no" ;;

  -enable-* | --enable-*)
    ac_feature=`echo $ac_option|sed -e 's/-*enable-//' -e 's/=.*//'`
    # Reject names that are not valid shell variable names.
changequote(, )dnl
    if test -n "`echo $ac_feature| sed 's/[-_a-zA-Z0-9]//g'`"; then
changequote([, ])dnl
      AC_MSG_ERROR($ac_feature: invalid feature name)
    fi
    ac_feature=`echo $ac_feature| sed 's/-/_/g'`
    case "$ac_option" in
      *=*) ;;
      *) ac_optarg=yes ;;
    esac
    eval "enable_${ac_feature}='$ac_optarg'" ;;

  -exec-prefix | --exec_prefix | --exec-prefix | --exec-prefi \
  | --exec-pref | --exec-pre | --exec-pr | --exec-p | --exec- \
  | --exec | --exe | --ex)
    ac_prev=exec_prefix ;;
  -exec-prefix=* | --exec_prefix=* | --exec-prefix=* | --exec-prefi=* \
  | --exec-pref=* | --exec-pre=* | --exec-pr=* | --exec-p=* | --exec-=* \
  | --exec=* | --exe=* | --ex=*)
    exec_prefix="$ac_optarg" ;;

  -help | --help | --hel | --he)
    # Omit some internal or obsolete options to make the list less imposing.
    # This message is too long to be a string in the A/UX 3.1 sh.
    cat << EOF
changequote(, )dnl
Usage: configure [options] [host]
Options: [defaults in brackets after descriptions]
Configuration:
  --cache-file=FILE       cache test results in FILE
  --help                  print this message
  --no-create             do not create output files
  --quiet, --silent       do not print \`checking...' messages
  --version               print the version of autoconf that created configure
Directory and file names:
  --prefix=PREFIX         install architecture-independent files in PREFIX
                          [$ac_default_prefix]
  --exec-prefix=EPREFIX   install architecture-dependent files in EPREFIX
                          [same as prefix]
  --bindir=DIR            user executables in DIR [EPREFIX/bin]
  --sbindir=DIR           system admin executables in DIR [EPREFIX/sbin]
  --datadir=DIR           read-only architecture-independent data in DIR
                          [PREFIX/share]
  --libdir=DIR            object code libraries in DIR [EPREFIX/lib]
  --includedir=DIR        C header files in DIR [PREFIX/include]
  --mandir=DIR            man documentation in DIR [PREFIX/man]
  --srcdir=DIR            find the sources in DIR [configure dir or ..]
EOF
    cat << EOF
Features and packages:
  --disable-FEATURE       do not include FEATURE (same as --enable-FEATURE=no)
  --enable-FEATURE[=ARG]  include FEATURE [ARG=yes]
  --with-PACKAGE[=ARG]    use PACKAGE [ARG=yes]
  --without-PACKAGE       do not use PACKAGE (same as --with-PACKAGE=no)
  --x-includes=DIR        X include files are in DIR
  --x-libraries=DIR       X library files are in DIR
changequote([, ])dnl
EOF
    if test -n "$ac_help"; then
      echo "--enable and --with options recognized:$ac_help"
    fi
    exit 0 ;;

  -includedir | --includedir | --includedi | --included | --include \
  | --includ | --inclu | --incl | --inc)
    ac_prev=includedir ;;
  -includedir=* | --includedir=* | --includedi=* | --included=* | --include=* \
  | --includ=* | --inclu=* | --incl=* | --inc=*)
    includedir="$ac_optarg" ;;

  -libdir | --libdir | --libdi | --libd)
    ac_prev=libdir ;;
  -libdir=* | --libdir=* | --libdi=* | --libd=*)
    libdir="$ac_optarg" ;;

  -mandir | --mandir | --mandi | --mand | --man | --ma | --m)
    ac_prev=mandir ;;
  -mandir=* | --mandir=* | --mandi=* | --mand=* | --man=* | --ma=* | --m=*)
    mandir="$ac_optarg" ;;

  -nfp | --nfp | --nf)
    # Obsolete; use --without-fp.
    with_fp=no ;;

  -no-create | --no-create | --no-creat | --no-crea | --no-cre \
  | --no-cr | --no-c)
    no_create=yes ;;

  -no-recursion | --no-recursion | --no-recursio | --no-recursi \
  | --no-recurs | --no-recur | --no-recu | --no-rec | --no-re | --no-r)
    no_recursion=yes ;;

  -prefix | --prefix | --prefi | --pref | --pre | --pr | --p)
    ac_prev=prefix ;;
  -prefix=* | --prefix=* | --prefi=* | --pref=* | --pre=* | --pr=* | --p=*)
    prefix="$ac_optarg" ;;

  -q | -quiet | --quiet | --quie | --qui | --qu | --q \
  | -silent | --silent | --silen | --sile | --sil)
    silent=yes ;;

  -sbindir | --sbindir | --sbindi | --sbind | --sbin | --sbi | --sb)
    ac_prev=sbindir ;;
  -sbindir=* | --sbindir=* | --sbindi=* | --sbind=* | --sbin=* \
  | --sbi=* | --sb=*)
    sbindir="$ac_optarg" ;;

  -srcdir | --srcdir | --srcdi | --srcd | --src | --sr)
    ac_prev=srcdir ;;
  -srcdir=* | --srcdir=* | --srcdi=* | --srcd=* | --src=* | --sr=*)
    srcdir="$ac_optarg" ;;

  -v | -verbose | --verbose | --verbos | --verbo | --verb)
    verbose=yes ;;

  -version | --version | --versio | --versi | --vers)
    echo "configure generated by autoconf version AC_ACVERSION"
    exit 0 ;;

  -with-* | --with-*)
    ac_package=`echo $ac_option|sed -e 's/-*with-//' -e 's/=.*//'`
    # Reject names that are not valid shell variable names.
changequote(, )dnl
    if test -n "`echo $ac_package| sed 's/[-_a-zA-Z0-9]//g'`"; then
changequote([, ])dnl
      AC_MSG_ERROR($ac_package: invalid package name)
    fi
    ac_package=`echo $ac_package| sed 's/-/_/g'`
    case "$ac_option" in
      *=*) ;;
      *) ac_optarg=yes ;;
    esac
    eval "with_${ac_package}='$ac_optarg'" ;;

  -without-* | --without-*)
    ac_package=`echo $ac_option|sed -e 's/-*without-//'`
    # Reject names that are not valid shell variable names.
changequote(, )dnl
    if test -n "`echo $ac_package| sed 's/[-a-zA-Z0-9_]//g'`"; then
changequote([, ])dnl
      AC_MSG_ERROR($ac_package: invalid package name)
    fi
    ac_package=`echo $ac_package| sed 's/-/_/g'`
    eval "with_${ac_package}=no" ;;

  -x-includes | --x-includes | --x-include | --x-includ | --x-inclu \
  | --x-incl | --x-inc | --x-in | --x-i)
    ac_prev=x_includes ;;
  -x-includes=* | --x-includes=* | --x-include=* | --x-includ=* | --x-inclu=* \
  | --x-incl=* | --x-inc=* | --x-in=* | --x-i=*)
    x_includes="$ac_optarg" ;;

  -x-libraries | --x-libraries | --x-librarie | --x-librari \
  | --x-librar | --x-libra | --x-libr | --x-lib | --x-li | --x-l)
    ac_prev=x_libraries ;;
  -x-libraries=* | --x-libraries=* | --x-librarie=* | --x-librari=* \
  | --x-librar=* | --x-libra=* | --x-libr=* | --x-lib=* | --x-li=* | --x-l=*)
    x_libraries="$ac_optarg" ;;

  AC_INIT_PARSE_ARGS_OPT_HOOK()

  -*) AC_MSG_ERROR([$ac_option: invalid option; use --help to show usage])
    ;;

  *)
changequote(, )dnl
    if test -n "`echo $ac_option| sed 's/[-a-z0-9.]//g'`"; then
changequote([, ])dnl
      AC_MSG_WARN($ac_option: invalid host type)
    fi
    if test "x$nonopt" != xNONE; then
      AC_MSG_ERROR(can only configure for one host and one target at a time)
    fi
    nonopt="$ac_option"
    ;;

  esac
done

if test -n "$ac_prev"; then
  AC_MSG_ERROR(missing argument to --`echo $ac_prev | sed 's/_/-/g'`)
fi
])



dnl
dnl The actual defintions of the hook functions for AC_INIT_PARSE_ARGS
dnl
AC_DEFUN(AC_INIT_PARSE_ARGS_OPT_HOOK, [
  -set-* | --set-*)
    ac_package=`echo $ac_option|sed -e 's/-*set-//' -e 's/=.*//'`
    # Reject names that are not valid shell variable names.
changequote(, )dnl
    if test -n "`echo $ac_package| sed 's/[-_a-zA-Z0-9]//g'`"; then
changequote([, ])dnl
      AC_MSG_ERROR($ac_package: invalid package name)
    fi
    ac_package=`echo $ac_package| sed 's/-/_/g'`
    case "$ac_option" in
      *=*) ;;
      *) ac_optarg=yes ;;
    esac
    eval "set_${ac_package}='$ac_optarg'" ;;
])


dnl
dnl  Try to run something through make and see if it succeeds
dnl
dnl AC_TRY_MAKE(MAKEFILE, [ACTION-IF-SUCCESS [, ACTION-IF-NO-SUCCESS]])
AC_DEFUN(AC_TRY_MAKE,
[cat > conftest.$ac_ext <<EOF
[$1]
EOF
if make -f conftest.$ac_ext > /dev/null 2>&1 ; then
  ifelse([$2], , :, [rm -rf conftest*
  $2])
else
  echo "configure: failed program was:" >&AC_FD_CC
  cat conftest.$ac_ext >&AC_FD_CC
ifelse([$3], , , [  rm -rf conftest*
  $3
])dnl
fi
rm -f conftest*])



dnl
dnl AC_LOCATE_LIB(VAR, PATTERN [, VAR-VAL-IF-NOT-FOUND [, PATH])
dnl
dnl  Try to find a library using PATH. Go through and for each
dnl  path in PATH remove any trailing "bin" and replace it with lib.
dnl  Thus if you did a:
dnl
dnl     AC_LOCATE_LIB(libmpath, m, no, "/bin:/usr/bin:/usr/local/bin:/some/dir")
dnl
dnl  Then it would check for the existence of:
dnl
dnl     /lib/libm.*
dnl     /usr/lib/libm.*
dnl     /usr/local/lib/libm.*
dnl     /some/dir/lib/libm.*
dnl
dnl  And stop as soon as anything was encountered. libmpath would then
dnl  be set to "/usr/lib" if the first place something was found was
dnl  in /usr/lib/libm.*
dnl
dnl  If nothing matched then the variable would be explicitly set to 'no'
dnl  (if no had not been given then '' would be used)
dnl
dnl
AC_DEFUN(AC_LOCATE_LIB,
[

[$1]=dnl
ifelse([$3], , '', [$3])
ac_LL_PATH=`echo "dnl
ifelse([$4], , $PATH, [$4])" | sed -e 's/:/ /g'`
dnl
dnl  there is a danger that ac_LL_PATH is empty causing the
dnl  for loop to fail on syntax. Thus we add a dummy path
dnl  that should never exist. Quoting ac_LL_PATH would defeat
dnl  the purpose.
dnl
for ac_libpath in $ac_LL_PATH /__XqqFrobozz ; do
    ac_libpath=`echo $ac_libpath | sed -e 's;/bin$;;'`
    if test `/bin/ls ${ac_libpath}/lib/lib[$2].* 2> /dev/null | wc -l` -gt 0; then
        [$1]="$ac_libpath"
	break;
    fi
done
])



dnl
dnl  Test to see whether h_errno is visible when netdb.h is included.
dnl  At least under HP-UX 10.x this is not the case unless 
dnl  XOPEN_SOURCE_EXTENDED is declared but then other nasty stuff happens.
dnl  The appropriate thing to do is to call this macro and then
dnl  if it is not available do a "extern int h_errno;" in the code.
dnl
AC_DEFUN(AC_DECL_H_ERRNO,
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
  AC_DEFINE(H_ERRNO_DECLARED)
fi
])


dnl
dnl  On some systems one needs to include sys/select.h to get the
dnl  definition of FD_* macros for select bitmask handling.
dnl
AC_DEFUN(AC_DECL_FD_SET_SYS_SELECT_H,
[AC_CACHE_CHECK([for FD_SET eclaration in sys/select.h],
  ac_cv_decl_fdset_sys_select_h,
[AC_EGREP_CPP(oh_yeah, [#include <sys/select.h>
#ifdef FD_SETSIZE
oh_yeah
#endif
], ac_cv_decl_fdset_sys_select_h=yes, ac_cv_decl_fdset_sys_select_h=no)])
if test $ac_cv_decl_fdset_sys_select_h = yes; then
  AC_DEFINE(FD_SET_IN_SYS_SELECT_H)
fi
])


dnl
dnl  A test to determine whether make understands the ".POSIX:" phony
dnl  target that POSIX makes are supposed to understand.
dnl
dnl  This macro will generate code to test for the feature. It will
dnl  then call AC_SUBST() for MAKE_POSIX_TARGET, replacing all
dnl  occurences of @MAKE_POSIX_TARGET@ with either ".POSIX:" or "" depending
dnl  on whether '+' is supported. This macro takes no arguments.
dnl
AC_DEFUN(AC_PROG_MAKE_POSIX_TARGET,
[AC_MSG_CHECKING(whether ${MAKE-make} understands ['.POSIX:'])
set dummy ${MAKE-make}; ac_make=`echo "[$]2" | sed 'y%./+-%__p_%'`
AC_CACHE_VAL(ac_cv_prog_${ac_make}_posix_target,
[ac_maketemp=""
cat > conftestmake <<\EOF
.POSIX:

all:
	@echo "ac_maketemp=yes"
EOF
eval `${MAKE-make} -f conftestmake 2>/dev/null | grep "^ac_"`
if test -n "$ac_maketemp"; then
  eval ac_cv_prog_${ac_make}_posix_target=yes
else
  eval ac_cv_prog_${ac_make}_posix_target=no
fi
rm -f conftestmake])dnl
if eval "test \"`echo '$ac_cv_prog_'${ac_make}_posix_target`\" = yes"; then
  AC_MSG_RESULT(yes)
  MAKE_POSIX_TARGET=".POSIX:"
else
  AC_MSG_RESULT(no)
  MAKE_POSIX_TARGET=""
fi
AC_SUBST([MAKE_POSIX_TARGET])dnl
])



dnl
dnl  A test to determine whether make understands a leading "+" in a
dnl  make command to mean "execute even if -n has been given" or whether
dnl  it produces an error.
dnl
dnl  This macro will generate code to test for the feature. It will
dnl  then call AC_SUBST() for MAKE_POSIX_PLUS, replacing all
dnl  occurences of @MAKE_POSIX_PLUS@ with either "+" or "" depending
dnl  on whether "+" is supported. This macro takes no arguments.
dnl
AC_DEFUN(AC_PROG_MAKE_POSIX_PLUS,
[AC_MSG_CHECKING(whether ${MAKE-make} understands an initial ['+'])
set dummy ${MAKE-make}; ac_make=`echo "[$]2" | sed 'y%./+-%__p_%'`
AC_CACHE_VAL(ac_cv_prog_${ac_make}_posix_plus,
[ac_maketemp=""
cat > conftestmake <<\EOF
all:
	+@echo ac_maketemp=yes
EOF
eval `${MAKE-make} -n -f conftestmake 2>/dev/null | grep "^ac_"`
if test -n "$ac_maketemp"; then
  eval ac_cv_prog_${ac_make}_posix_plus=yes
else
  eval ac_cv_prog_${ac_make}_posix_plus=no
fi
rm -f conftestmake])dnl
if eval "test \"`echo '$ac_cv_prog_'${ac_make}_posix_plus`\" = yes"; then
  AC_MSG_RESULT(yes)
  MAKE_POSIX_PLUS='+'
else
  AC_MSG_RESULT(no)
  MAKE_POSIX_PLUS=""
fi
AC_SUBST([MAKE_POSIX_PLUS])dnl
])



AC_DEFUN(AC_USE_INSTALL_SH,
[AC_REQUIRE([AC_CONFIG_AUX_DIR_DEFAULT])dnl

# Fool autoconf into thinking that you have called AC_PROG_INSTALL
# since this is tested for in other places
AC_PROVIDE([AC_PROG_INSTALL])

# Always use the supplied install-sh script instead of trying to divine
# which OS has a good install program.
AC_MSG_CHECKING(for the install-sh script)
if test -z "$INSTALL"; then
  INSTALL="$ac_install_sh"
fi
dnl We do special magic for INSTALL instead of AC_SUBST, to get
dnl relative paths right.
AC_MSG_RESULT($INSTALL)

# Use test -z because SunOS4 sh mishandles braces in ${var-val}.
# It thinks the first close brace ends the variable substitution.
test -z "$INSTALL_PROGRAM" && INSTALL_PROGRAM='${INSTALL}'
AC_SUBST(INSTALL_PROGRAM)dnl

test -z "$INSTALL_DATA" && INSTALL_DATA='${INSTALL} -m 644'
AC_SUBST(INSTALL_DATA)dnl
])
