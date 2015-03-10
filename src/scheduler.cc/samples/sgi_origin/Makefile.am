include $(top_srcdir)/buildutils/config.mk

# Support files used by the scheduler
schedprivdir = $(PBS_SERVER_HOME)/sched_priv
schedpriv_DATA = config

EXTRA_DIST = $(schedpriv_DATA)

# File which contains the scheduler's configuration information
CONFIGFILE = $(schedprivdir)/config

# File in which scheduler maintains decayed recent usage.
DECAY_INFO_FILE = $(schedprivdir)/decay_usage

AM_CXXFLAGS = -fullwarn -D_POSIX_C_SOURCE -DDEBUG \
	       -DCONFIGFILE=\"$(CONFIGFILE)\" \
	       -DDECAY_INFO_FILE=\"$(DECAY_INFO_FILE)\"

noinst_LTLIBRARIES = libfoo.la

libfoo_la_SOURCES = acl_support.cpp allocations.cpp byte2val.cpp cleanup.cpp \
		    comment.cpp dedicated.cpp dedqueue.cpp dyn_nodemask.cpp \
		    evaluate_system.cpp file_chgs.cpp fragments.cpp getat.cpp \
		    getconfig.cpp getjobs.cpp getqueues.cpp getrsrcs.cpp how_many.cpp \
		    hpm.cpp jobinfo.cpp misc.cpp movejob.cpp msgs.cpp overlaps.cpp \
		    pack_queues.cpp pnp.cpp queue_limits.cpp rejectjob.cpp \
		    resource_limits.cpp runjob.cpp schedinit.cpp schedule.cpp \
		    sec2val.cpp time_limits.cpp user_limits.cpp usersort.cpp \
		    val2bool.cpp val2byte.cpp val2sec.cpp \
		    bitfield.h  gblxvars.h  hpm.h  msgs.h  toolkit.h
