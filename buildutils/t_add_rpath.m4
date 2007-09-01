AC_DEFUN([T_ADD_RPATH], [

  orig_libdir="$libdir"
  t_add_rpath_libs=""
  for lib_part in $$1 ;do
    case $lib_part in
      -L/*)
        t_libdir=`echo $lib_part | sed s/-L//`
        t_dir_in_syslib_path=no
        for sysdir in $sys_lib_dlsearch_path_spec ;do
           if test "X$t_libdir" = "X$sysdir" ;then
               t_dir_in_syslib_path=yes
               break
           fi
        done
        t_add_rpath_libs="${t_add_rpath_libs} ${lib_part}"
        if test "X$t_dir_in_syslib_path" = "Xno" ;then
           libdir="$t_libdir"
           eval "new_lib_spec=\"$hardcode_libdir_flag_spec\""
           t_add_rpath_libs="${t_add_rpath_libs} ${new_lib_spec}"
        fi
        ;;
      -L)
        ;;
      *) t_add_rpath_libs="${t_add_rpath_libs} ${lib_part}"
        ;;
    esac
  done
  libdir="$orig_libdir"
  $1="$t_add_rpath_libs"
])

