include $(top_srcdir)/buildutils/config.mk

if BUILD_L26_CPUSETS
csetsubdir=cpuset
csetlib=cpuset/libmomcpuset.a
endif

DIST_SUBDIRS = cpuset

SUBDIRS = $(cpuset)
LDADD = $(csetlib)

AM_CPPFLAGS += -DPBS_MOM -DDEMUX=\"$(DEMUX_PATH)\" 

noinst_LIBRARIES = libmommach.a

libmommach_a_SOURCES = mom_mach.c mom_mach.h mom_start.c pe_input.c
