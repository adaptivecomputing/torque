include $(top_srcdir)/buildutils/config.mk

PBS_LIBS = ../lib/Libpbs/libtorque.la


lib_LTLIBRARIES = libdrmaa.la
libdrmaa_la_SOURCES = drmaa.c
libdrmaa_la_LIBADD = $(PBS_LIBS)
include_HEADERS = drmaa.h
