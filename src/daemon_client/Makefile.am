include $(top_srcdir)/buildutils/config.mk

noinst_HEADERS = trq_auth_daemon.h 
DIST_SUBDIRS =
CLEANFILES = *.gcda *.gcno *.gcov

AM_CFLAGS = -DPBS_SERVER_HOME=\"$(PBS_SERVER_HOME)\" -Wall -pthread -ldl -lrt -lssl -lcrypto

sbin_PROGRAMS = trqauthd

trqauthd_SOURCES = trq_auth_daemon.c trq_main.c
trqauthd_LDFLAGS = -L../lib/Libpbs/.libs -ltorque

