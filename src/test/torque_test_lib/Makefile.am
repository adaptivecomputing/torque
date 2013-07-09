include $(top_srcdir)/buildutils/config.mk

noinst_LTLIBRARIES = libtorque_test.la

libtorque_test_la_SOURCES = ../../lib/Libifl/list_link.c \
                            ../../lib/Libutils/u_mutex_mgr.cpp \
                            ../../lib/Libutils/u_resizable_array.c \
                            ../../lib/Libutils/u_hash_table.c \
                            ../../lib/Libattr/attr_func.c \
                            ../../lib/Libattr/attr_fn_arst.c \
                            ../../lib/Libattr/attr_fn_tv.c \
                            ../../lib/Libattr/attr_fn_intr.c \
                            ../../lib/Libattr/attr_fn_b.c \
                            ../../lib/Libattr/attr_fn_c.c \
                            ../../lib/Libattr/attr_fn_l.c \
                            ../../lib/Libattr/attr_fn_hold.c \
                            ../../lib/Libattr/attr_fn_str.c \
                            ../../lib/Libattr/attr_fn_unkn.c \
														../../server/execution_slot_tracker.cpp \
														../../server/job_usage_info.cpp
libtorque_test_la_LDFLAGS = @CHECK_LIBS@ -shared -lgcov

