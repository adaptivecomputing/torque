include $(top_srcdir)/buildutils/config.mk

DIST_SUBDIRS =

CLEANFILES = *.gcda *.gcno *.gcov

noinst_HEADERS = attr_atomic.h attr_fn_b.h attr_fn_hold.h attr_fn_ll.h attr_fn_size.h attr_fn_tokens.h attr_func.h \
                 attr_fn_acl.h attr_fn_c.h attr_fn_intr.h attr_fn_nppcu.h attr_fn_str.h attr_fn_tv.h attr_node_func.h \
                 attr_fn_arst.h attr_fn_freq.h attr_fn_l.h attr_fn_resc.h attr_fn_time.h attr_fn_unkn.h

noinst_LIBRARIES = libattr.a

libattr_a_SOURCES = attr_atomic.c attr_fn_acl.c attr_fn_arst.c attr_fn_b.c \
		     attr_fn_c.c attr_fn_hold.c attr_fn_intr.c attr_fn_l.c \
		     attr_fn_ll.c attr_fn_nppcu.c attr_fn_resc.c attr_fn_size.c \
		     attr_fn_str.c attr_fn_time.c attr_fn_unkn.c \
		     attr_func.c attr_node_func.c attr_fn_tokens.c attr_fn_tv.c \
	       attr_str_conversion.c attr_fn_freq.c req.cpp complete_req.cpp \
				 attr_fn_complete_req.c attr_fn_attr_req_info.c attr_req_info.cpp


