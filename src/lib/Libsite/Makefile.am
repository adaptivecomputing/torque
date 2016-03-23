include $(top_srcdir)/buildutils/config.mk

CHECK_DIR = test

noinst_HEADERS = lib_site.h

DIST_SUBDIRS =
CLEANFILES = *.gcda *.gcno *.gcov

AM_CPPFLAGS += -I$(top_srcdir)/src/resmom/@PBS_MACH@

noinst_LIBRARIES = libsite.a

libsite_a_SOURCES = site_allow_u.c site_alt_rte.c site_check_u.c \
		     site_map_usr.c site_mom_chu.c site_mom_ckp.c \
		     site_mom_jst.c

if HAVE_CHECK
check:
	$(MAKE) -C $(CHECK_DIR) $(MAKECMDGOALS)

.PHONY: cleancheck
cleancheck:
	cd test && $(MAKE) cleancheck
endif
