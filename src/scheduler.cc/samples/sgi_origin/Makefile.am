include $(top_srcdir)/buildutils/config.mk

# Support files used by the scheduler
schedprivdir = $(PBS_SERVER_HOME)/sched_priv
schedpriv_DATA = config

EXTRA_DIST = $(schedpriv_DATA)

# File which contains the scheduler's configuration information
CONFIGFILE = $(schedprivdir)/config

# File in which scheduler maintains decayed recent usage.
DECAY_INFO_FILE = $(schedprivdir)/decay_usage

AM_CPPFLAGS += -fullwarn -D_POSIX_C_SOURCE -DDEBUG \
	       -DCONFIGFILE=\"$(CONFIGFILE)\" \
	       -DDECAY_INFO_FILE=\"$(DECAY_INFO_FILE)\"

noinst_LTLIBRARIES = libfoo.la

libfoo_la_SOURCES = acl_support.c allocations.c byte2val.c cleanup.c \
		    comment.c dedicated.c dedqueue.c dyn_nodemask.c \
		    evaluate_system.c file_chgs.c fragments.c getat.c \
		    getconfig.c getjobs.c getqueues.c getrsrcs.c how_many.c \
		    hpm.c jobinfo.c misc.c movejob.c msgs.c overlaps.c \
		    pack_queues.c pnp.c queue_limits.c rejectjob.c \
		    resource_limits.c runjob.c schedinit.c schedule.c \
		    sec2val.c time_limits.c user_limits.c usersort.c \
		    val2bool.c val2byte.c val2sec.c \
		    bitfield.h  gblxvars.h  hpm.h  msgs.h  toolkit.h
