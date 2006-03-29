include $(top_srcdir)/buildutils/config.mk

# Support files used by the scheduler
schedprivdir = $(PBS_SERVER_HOME)/sched_priv
schedpriv_DATA = sched_config

EXTRA_DIST = $(schedpriv_DATA) admin_guide.txt

# File which contains the scheduler's configuration information
CONFIGFILE = $(schedprivdir)/sched_config

AM_CPPFLAGS += -DCONFIGFILE=\"$(CONFIGFILE)\"

noinst_LTLIBRARIES = libfoo.la

libfoo_la_SOURCES = acl_support.c cleanup.c comment.c evaluate_system.c \
		    fair_access.c getconfig.c getjobs.c getqueues.c \
		    getrsrcs.c jobinfo.c msgs.c pack_queues.c queue_limits.c \
		    resource_limits.c runjob.c schedinit.c schedule.c \
		    time_limits.c toolkit.c user_limits.c usersort.c \
		    gblxvars.h  msgs.h  toolkit.h

PBS_LIBS = ../lib/Libpbs/libtorque.la

bin_PROGRAMS = qschedule
qschedule_CPPFLAGS = $(AM_CPPFLAGS) \
		     -DPREDICTED_SCHEDULE_FILE=\"$(schedprivdir)/job_schedule\"
qschedule_LDADD = $(PBS_LIBS)
qschedule_SOURCES = qschedule.c
