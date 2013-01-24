include $(top_srcdir)/buildutils/config.mk

CHECK_DIR = test

DIST_SUBDIRS =

# all compilation happens in lib/Libpbs
if HAVE_CHECK
check:
	$(MAKE) -C $(CHECK_DIR) $(MAKECMDGOALS)

PHONY: cleancheck
cleancheck:
	cd test && $(MAKE) cleancheck
endif
