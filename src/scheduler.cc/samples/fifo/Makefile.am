include $(top_srcdir)/buildutils/config.mk

# Support files used by the scheduler
schedprivdir = $(PBS_SERVER_HOME)/sched_priv
schedpriv_DATA = sched_config resource_group holidays dedicated_time

EXTRA_DIST = $(schedpriv_DATA)

noinst_LTLIBRARIES = libfoo.la

libfoo_la_SOURCES = check.cpp dedtime.cpp fairshare.cpp fifo.cpp globals.cpp \
		    job_info.cpp misc.cpp node_info.cpp parse.cpp prev_job_info.cpp \
		    prime.cpp queue_info.cpp server_info.cpp sort.cpp state_count.cpp \
		    check.h config.h constant.h data_types.h dedtime.h \
		    fairshare.h fifo.h globals.h job_info.h misc.h node_info.h \
		    parse.h prev_job_info.h prime.h queue_info.h server_info.h \
		    sort.h state_count.h token_acct.h token_accounting.cpp
