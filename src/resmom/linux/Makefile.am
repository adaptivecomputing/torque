include $(top_srcdir)/buildutils/config.mk

CHECK_DIR = test

DIST_SUBDIRS =

include_HEADERS = lib_mom.h

AM_CPPFLAGS += -DPBS_MOM -DDEMUX=\"$(DEMUX_PATH)\" 

noinst_LIBRARIES = libmommach.a

libmommach_a_SOURCES = mom_mach.cpp mom_mach.h mom_start.cpp pe_input.cpp node_internals.cpp numa_node.cpp cpu_frequency.cpp sys_file.cpp power_state.cpp
if BUILD_L26_CPUSETS
libmommach_a_SOURCES += cpuset.cpp
endif

if HAVE_CHECK
check:
	$(MAKE) -C $(CHECK_DIR) $(MAKECMDGOALS)

.PHONY: cleancheck
cleancheck:
	cd $(CHECK_DIR) && $(MAKE) cleancheck
endif

CLEANFILES = *.gcda *.gcno *.gcov
