
include $(top_srcdir)/buildutils/config.mk

pammoddir=@pammoddir@

pammod_LTLIBRARIES = pam_pbssimpleauth.la

AM_CFLAGS = -DPBS_SERVER_HOME=\"$(PBS_SERVER_HOME)\"

EXTRA_DIST = README.pam

pam_pbssimpleauth_la_SOURCES = pam_pbssimpleauth.c
pam_pbssimpleauth_la_LDFLAGS = -no-undefined -module -avoid-version

# ugly, ugly, gross, yuck, ugly... but this is the cleanest way to
# avoid libtool droppings in the PAM module directory.
install-data-hook:
	@$(NORMAL_INSTALL)
	$(mkinstalldirs) $(DESTDIR)$(pammoddir)
	@list='$(pammod_LTLIBRARIES)'; for p in $$list; do \
          if test -f $$p; then \
            f="`echo $$p | sed -e 's|^.*/||'`"; \
            b="`echo $$f | sed -e 's|\.la$$||'`"; \
          echo " rm -f $(DESTDIR)$(pammoddir)/$$b.a $(DESTDIR)$(pammoddir)/$$b.la"; \
          rm -f $(DESTDIR)$(pammoddir)/$$b.a $(DESTDIR)$(pammoddir)/$$b.la; \
          else :; fi; \
        done

