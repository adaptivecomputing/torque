include $(top_srcdir)/buildutils/config.mk

CHECK_DIR = test

DIST_SUBDIRS =
CLEANFILES = *.gcda *.gcno *.gcov

noinst_LIBRARIES = libutils.a

include_HEADERS = u_lock_ctl.h lib_utils.h

libutils_a_SOURCES = u_groups.cpp u_tree.cpp u_mu.cpp u_MXML.cpp u_xml.cpp \
                     u_threadpool.cpp \
                     u_lock_ctl.cpp u_mom_hierarchy.cpp \
                     u_hash_map_structs.cpp u_users.cpp \
										 u_constants.cpp u_mutex_mgr.cpp \
										 u_misc.cpp u_putenv.cpp u_wrapper.cpp u_timer.cpp

if HAVE_CHECK
check:
	$(MAKE) -C $(CHECK_DIR) $(MAKECMDGOALS)

.PHONY: cleancheck
cleancheck:
	cd test && $(MAKE) cleancheck
endif
