include $(top_srcdir)/buildutils/config.mk

SUBDIRS = Libattr Libcmds Libcsv Libdis Libifl Liblog Libnet Libpbs Libsite Libutils

CHECK_DIR = test

DIST_SUBDIRS = Libattr Libcmds Libcsv Libdis Libifl Liblog Libnet Libpbs Libsite Libutils

cleancheck:
	@for dir in $(SUBDIRS) $(CHECK_DIR); do (cd $$dir && $(MAKE) cleancheck); done

if HAVE_CHECK
check:
	@for dir in $(SUBDIRS) $(CHECK_DIR); do (cd $$dir && $(MAKE) $(MAKECMDGOALS)); done
endif
