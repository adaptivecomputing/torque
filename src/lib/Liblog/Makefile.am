include $(top_srcdir)/buildutils/config.mk

CHECK_DIR = test

DIST_SUBDIRS =

noinst_HEADERS = chk_file_sec.h log_event.h pbs_log.h pbs_messages.h setup_env.h

# all compilation happens in lib/Libpbs

if HAVE_CHECK
check:
	$(MAKE) -C $(CHECK_DIR) $(MAKECMDGOALS)

.PHONY: cleancheck
cleancheck:
	cd test && $(MAKE) cleancheck
endif
