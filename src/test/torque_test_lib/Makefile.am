include $(top_srcdir)/buildutils/config.mk

noinst_LTLIBRARIES = libtorque_test.la

libtorque_test_la_SOURCES = ../../lib/Libifl/list_link.c \
                            ../../lib/Libutils/u_mutex_mgr.cpp \
                            ../../lib/Libattr/attr_func.c \
                            ../../lib/Libattr/attr_fn_arst.c \
                            ../../lib/Libattr/attr_fn_tv.c \
                            ../../lib/Libattr/attr_fn_intr.c \
                            ../../lib/Libattr/attr_fn_b.c \
                            ../../lib/Libattr/attr_fn_c.c \
                            ../../lib/Libattr/attr_fn_l.c \
                            ../../lib/Libattr/attr_fn_ll.c \
                            ../../lib/Libattr/attr_fn_hold.c \
                            ../../lib/Libattr/attr_fn_str.c \
                            ../../lib/Libattr/attr_fn_unkn.c \
                            ../../lib/Libattr/attr_fn_nppcu.c \
                            ../../lib/Libattr/attr_fn_freq.c \
			    ../../lib/Libattr/attr_fn_resc.c \
			    ../../resmom/node_frequency.cpp \
			    ../../resmom/linux/cpu_frequency.cpp \
			    ../../resmom/linux/sys_file.cpp \
						    ../../server/execution_slot_tracker.cpp \
							../../server/job_usage_info.cpp \
							../../server/execution_slot_tracker.cpp \
							../../server/job_usage_info.cpp \
							test_tcp_dis.cpp \
							../../lib/Libdis/dis.c \
							../../lib/Libdis/disi10l_.c \
							../../lib/Libdis/disrcs.c \
							../../lib/Libdis/disrfst.c \
							../../lib/Libdis/disrsi_.c \
							../../lib/Libdis/disrss.c \
							../../lib/Libdis/disrul.c \
							../../lib/Libdis/diswl_.c \
							../../lib/Libdis/diswui.c \
							../../lib/Libdis/discui_.c \
							../../lib/Libdis/disiui_.c \
							../../lib/Libdis/disrd.c \
							../../lib/Libdis/disrl_.c \
							../../lib/Libdis/disrsi.c \
							../../lib/Libdis/disrst.c \
							../../lib/Libdis/disrus.c \
							../../lib/Libdis/diswsi.c \
							../../lib/Libdis/diswul.c \
							../../lib/Libdis/discul_.c \
							../../lib/Libdis/disp10d_.c \
							../../lib/Libdis/disrf.c \
							../../lib/Libdis/disrl.c \
							../../lib/Libdis/disrsl_.c \
							../../lib/Libdis/disruc.c \
							../../lib/Libdis/diswcs.c \
							../../lib/Libdis/diswsl.c \
							../../lib/Libdis/disi10d_.c \
							../../lib/Libdis/disp10l_.c \
							../../lib/Libdis/disrfcs.c \
							../../lib/Libdis/disrsc.c \
							../../lib/Libdis/disrsl.c \
							../../lib/Libdis/disrui.c \
							../../lib/Libdis/diswf.c \
							../../lib/Libdis/diswui_.c
							
							
libtorque_test_la_LDFLAGS = @CHECK_LIBS@ -shared -lgcov

