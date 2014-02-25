include $(top_srcdir)/buildutils/config.mk

CHECK_DIR = test

DIST_SUBDIRS =
CLEANFILES = *.gcda *.gcno *.gcov

noinst_LIBRARIES = libutils.a

include_HEADERS = u_lock_ctl.h lib_utils.h

libutils_a_SOURCES = u_groups.c u_tree.c u_mu.c u_MXML.c u_xml.c \
										 u_threadpool.c u_resizable_array.c u_hash_table.c \
                     u_lock_ctl.c u_mom_hierarchy.c u_dynamic_string.c \
                     u_hash_map_structs.c u_memmgr.c u_users.c \
										 u_constants.c u_hash_map.c u_mutex_mgr.cpp \
										 u_misc.c u_putenv.c u_wrapper.c u_timer.cpp

if HAVE_CHECK
check:
	$(MAKE) -C $(CHECK_DIR) $(MAKECMDGOALS)

.PHONY: cleancheck
cleancheck:
	cd test && $(MAKE) cleancheck
endif
