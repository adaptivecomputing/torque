include $(top_srcdir)/buildutils/config.mk

CHECK_DIR = test

DIST_SUBDIRS =
CLEANFILES = *.gcda *.gcno *.gcov

include_HEADERS = trq_auth_daemon.h

AM_CXXFLAGS = -DPBS_SERVER_HOME=\"$(PBS_SERVER_HOME)\" -Wall -pthread -ldl -lrt -lssl -lcrypto

sbin_PROGRAMS = trqauthd

trqauthd_SOURCES = trq_auth_daemon.cpp trq_main.cpp
trqauthd_LDFLAGS = -L../lib/Libpbs/.libs -ltorque

if HAVE_CHECK
check:
	$(MAKE) -C $(CHECK_DIR) $(MAKECMDGOALS)

.PHONY: cleancheck
cleancheck:
	cd test && $(MAKE) cleancheck
endif
