include $(top_srcdir)/buildutils/config.mk

CHECK_DIR = test

DIST_SUBDIRS =

include_HEADERS = lib_mom.h

AM_CPPFLAGS += -DPBS_MOM -DDEMUX=\"$(DEMUX_PATH)\" 

noinst_LIBRARIES = libmommach.a

libmommach_a_SOURCES = mom_mach.c mom_mach.h mom_start.c pe_input.c node_internals.cpp numa_node.cpp
if BUILD_L26_CPUSETS
libmommach_a_SOURCES += cpuset.c
endif

if HAVE_CHECK
check:
	$(MAKE) -C $(CHECK_DIR) $(MAKECMDGOALS)

.PHONY: cleancheck
cleancheck:
	cd $(CHECK_DIR) && $(MAKE) cleancheck
endif
