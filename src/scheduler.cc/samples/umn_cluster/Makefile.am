include $(top_srcdir)/buildutils/config.mk

# Support files used by the scheduler
schedprivdir = $(PBS_SERVER_HOME)/sched_priv
schedpriv_DATA = sched_config

EXTRA_DIST = $(schedpriv_DATA) admin_guide.txt

# File which contains the scheduler's configuration information
CONFIGFILE = $(schedprivdir)/sched_config

AM_CXXFLAGS = -DCONFIGFILE=\"$(CONFIGFILE)\"

noinst_LTLIBRARIES = libfoo.la

libfoo_la_SOURCES = acl_support.cpp cleanup.cpp comment.cpp evaluate_system.cpp \
		    fair_access.cpp getconfig.cpp getjobs.cpp getqueues.cpp \
		    getrsrcs.cpp jobinfo.cpp msgs.cpp pack_queues.cpp queue_limits.cpp \
		    resource_limits.cpp runjob.cpp schedinit.cpp schedule.cpp \
		    time_limits.cpp toolkit.cpp user_limits.cpp usersort.cpp \
		    gblxvars.h  msgs.h  toolkit.h

PBS_LIBS = ../lib/Libpbs/libtorque.la

bin_PROGRAMS = qschedule
qschedule_CPPFLAGS = $(AM_CXXFLAGS) \
		     -DPREDICTED_SCHEDULE_FILE=\"$(schedprivdir)/job_schedule\"
qschedule_LDADD = $(PBS_LIBS)
qschedule_SOURCES = qschedule.cpp
