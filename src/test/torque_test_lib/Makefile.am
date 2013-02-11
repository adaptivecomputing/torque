include $(top_srcdir)/buildutils/config.mk

noinst_LTLIBRARIES = libtorque_test.la

libtorque_test_la_SOURCES = ../../lib/Libifl/list_link.c \
                            ../../lib/Libutils/u_mutex_mgr.cpp \
                            ../../lib/Libutils/u_resizable_array.c \
                            ../../lib/Libutils/u_hash_table.c
libtorque_test_la_LDFLAGS = @CHECK_LIBS@ -shared -lgcov

