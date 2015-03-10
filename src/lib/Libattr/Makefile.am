include $(top_srcdir)/buildutils/config.mk

DIST_SUBDIRS =

CHECK_DIR = test

CLEANFILES = *.gcda *.gcno *.gcov

include_HEADERS = attr_node_func.h

noinst_LIBRARIES = libattr.a

libattr_a_SOURCES = attr_atomic.cpp attr_fn_acl.cpp attr_fn_arst.cpp attr_fn_b.cpp \
		     attr_fn_c.cpp attr_fn_hold.cpp attr_fn_intr.cpp attr_fn_l.cpp \
		     attr_fn_ll.cpp attr_fn_nppcu.cpp attr_fn_resc.cpp attr_fn_size.cpp \
		     attr_fn_str.cpp attr_fn_time.cpp attr_fn_unkn.cpp \
		     attr_func.cpp attr_node_func.cpp attr_fn_tokens.cpp attr_fn_tv.cpp \
                     attr_str_conversion.cpp attr_fn_freq.cpp 

if HAVE_CHECK
check:
	$(MAKE) -C $(CHECK_DIR) $(MAKECMDGOALS)

.PHONY: cleancheck
cleancheck:
	cd test && $(MAKE) cleancheck
endif
