include $(top_srcdir)/buildutils/config.mk

CHECK_DIR = test

DIST_SUBDIRS =
CLEANFILES = *.gcda *.gcno *.gcov

AM_CXXFLAGS = -I$(top_srcdir)/src/resmom/@PBS_MACH@

noinst_LIBRARIES = libsite.a

libsite_a_SOURCES = site_allow_u.cpp site_alt_rte.cpp site_check_u.cpp \
		     site_map_usr.cpp site_mom_chu.cpp site_mom_ckp.cpp \
		     site_mom_jst.cpp

if HAVE_CHECK
check:
	$(MAKE) -C $(CHECK_DIR) $(MAKECMDGOALS)

.PHONY: cleancheck
cleancheck:
	cd test && $(MAKE) cleancheck
endif
