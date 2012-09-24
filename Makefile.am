include $(top_srcdir)/buildutils/config.mk

ACLOCAL_AMFLAGS = -I buildutils
SUBDIRS = src doc contrib/blcr contrib/init.d

EXTRA_DIST = acinclude.m4 \
  autogen.sh \
  CHANGELOG \
  configure.ac \
  cov_file_results.pl \
  Doxyfile \
  INSTALL \
	INSTALL.GNU \
  Makefile.am \
  parse_cov_results.pl \
  PBS_License.txt \
	README.array_changes \
  README.coding_notes \
	README.configure \
	README.cygwin \
  README.NUMA \
	README.torque \
  README.trqauthd \
	README.building_40 \
	Release_Notes \
  run_report.pl \
	torque.setup \
	torque.spec \
	buildutils/config.mk \
	buildutils/modulefiles.in \
	buildutils/modulefiles.vers.in \
	buildutils/pbs_mach_type \
	buildutils/pbs_mkdirs.in \
	buildutils/self-extract-head-sh.in \
	buildutils/torque.spec.in \
	contrib/AddPrivileges \
  contrib/hwloc_install.sh \
  contrib/mom_gencfg \
	contrib/pam_authuser.tar.gz \
	contrib/pbstop \
	contrib/pbsweb-lite-0.95.tar.gz \
	contrib/pestat-1.0/Makefile \
	contrib/pestat-1.0/pestat.c \
	contrib/pestat-1.0/README \
	contrib/qpeek \
	contrib/qpool.gz \
	contrib/PBS_License_2.3.txt \
	contrib/README.pam_authuser \
	contrib/README.pbs_ganglia_jobmonarch \
	contrib/README.pbs_python \
	contrib/README.pbstools \
	contrib/README.pbstop \
	contrib/README.pbs_weblite \
	contrib/README.pestat \
	contrib/README.qpeek \
	contrib/showjobs \
	contrib/diag/tdiag.sh

bin_SCRIPTS = pbs-config

DISTCLEANFILES = pbs-config
MOSTLYCLEANFILES = *.gcda *.gcno *.gcov

DISTCHECK_CONFIGURE_FLAGS=--with-server-home=$$dc_install_base/spool

snap:
	$(MAKE) VERSION=$(VERSION)-snap.$${snapstamp:-`date +%Y%m%d%H%M`} dist

dist-hook:
	case $(VERSION) in *-snap.*) \
	    for SPEC in torque.spec buildutils/torque.spec.in ; do \
	        chmod u+w $(distdir)/$$SPEC ; \
	        sed -e 's/^.define snap .*/%define snap 1/;' \
	            -e 's/^.define tarversion .*/%define tarversion $(VERSION)/' \
	            $(srcdir)/$$SPEC > $(distdir)/$$SPEC ; \
	    done ; \
	    chmod u+wx $(distdir)/configure ; \
	    sed -e 's/VERSION=.*/VERSION=$(VERSION)/' $(srcdir)/configure > $(distdir)/configure ;; \
	esac
	chmod u+w $(distdir)/README.torque
	sed -e 's/TORQUE .* README .*/TORQUE $(VERSION) README (released '"`date '+%b, %d %Y'`"')/' $(srcdir)/README.torque > $(distdir)/README.torque

srpm: dist
	rpmbuild -ts $(distdir).tar.gz

rpm: dist
	rpmbuild $(RPM_AC_OPTS) $(RPMOPTS) -tb $(distdir).tar.gz

#
# Used by the 'packages' target below
#
# Additional package name, should be redefined to something meaningful to
# the user.
PKGNAME     = package

# Directory with files that override the packaged files.
# Can be redefined on the make cmdline to a permanent location
PKGOVERRIDE = $(PWD)/pkgoverride

# temp dir for the packaged files and tarballs
PKGROOT     = $(PWD)/tpackages

if INCLUDE_SERVER
SERVER_PKG = server
endif

if INCLUDE_MOM
MOM_PKG = mom
endif

if INCLUDE_CLIENTS
CLIENTS_PKG = clients
endif

if INCLUDE_GUI
GUI_PKG = gui
endif

if INCLUDE_PAM
PAM_PKG = pam
endif

if INCLUDE_DRMAA
DRMAA_PKG = drmaa
endif

ALL_PKGS = $(SERVER_PKG) $(MOM_PKG) $(CLIENTS_PKG) $(GUI_PKG) $(PAM_PKG) $(DRMAA_PKG) devel doc

install_server install_mom install_clients install_gui install_lib install_pam install_drmaa:
	(cd src && $(MAKE) $@)
install_doc:
	(cd doc && $(MAKE) $@)
install_devel: install-binSCRIPTS
	(cd src && $(MAKE) $@)

packages:
	@if `echo $(PKGROOT) | grep ^/ >/dev/null`;then \
          echo "Building packages from $(PKGROOT)"; \
        else \
          echo "Error: PKGROOT must be an absolute path"; \
          exit 1; \
        fi
	rm -rf $(PKGROOT)
	mkdir $(PKGROOT)
	@for d in $(ALL_PKGS);do \
           proot=$(PKGROOT)/$$d; \
           pname=$(PKGNAME); \
           phost=$(PBS_MACH)-@host_cpu@; \
           tarball=$(PKGROOT)/$(PACKAGE_NAME)-$$pname-$$d-$$phost.tar.gz; \
           pkgfile=./$(PACKAGE_NAME)-$$pname-$$d-$$phost.sh; \
           echo "Building $$pkgfile ..."; \
           $(MAKE) DESTDIR=$$proot install_$$d >/dev/null || exit 1; \
	   case $$d in \
	      devel) :;; \
	      *) rm -f $$proot/$(libdir)/*a $$proot/$(libdir)/*.so;; \
	   esac; \
           if test -d $(PKGOVERRIDE)/$$d ;then \
             echo "Using override dir $(PKGOVERRIDE)/$$d"; \
             ( cd $(PKGOVERRIDE)/$$d && tar cf - . | (cd $$proot; tar xvf - ) ); \
           fi; \
           test $$d = gui && echo dummy > $$proot/regen-tclIndex; \
           ( cd $$proot; tar cf - * ) | gzip > $$tarball && \
           ( cd $$proot; tar cf - \
                           `test -f ./post-install && echo ./post-install` \
                           `test -f ./regen-tclIndex && echo ./regen-tclIndex` \
                           `test -d .$(sbindir) && echo .$(sbindir)/*` \
                           `test -d .$(bindir) && echo .$(bindir)/*` \
                           `test -d .$(libdir) && echo .$(libdir)/*` \
                           `test -d .$(mandir) && echo .$(mandir)/*/*` \
                           `test -d .$(includedir) && echo .$(includedir)/*` \
                           `test -d .$(pammoddir) && echo .$(pammoddir)/*` \
                           `test -d .$(PBS_SERVER_HOME) && echo .$(PBS_SERVER_HOME)/*` ) \
                           | gzip > $$tarball && \
           cat ./buildutils/self-extract-head-sh > $$pkgfile && \
           cat $$tarball >> $$pkgfile && \
           chmod 755 $$pkgfile; \
        done
	@echo "Done."
	@echo ""
	@echo "The package files are self-extracting packages that can be copied"; \
        echo "and executed on your production machines.  Use --help for options.";


if INSTALL_MODULEFILES
install-data-hook:
	$(INSTALL_DATA) -D ./buildutils/modulefiles $(DESTDIR)$(MODULEFILES_DIR)/$(PACKAGE_NAME)/$(PACKAGE_VERSION)
	$(INSTALL_DATA) ./buildutils/modulefiles.vers $(DESTDIR)$(MODULEFILES_DIR)/$(PACKAGE_NAME)/.version


uninstall-hook:
	rm -f $(DESTDIR)$(MODULEFILES_DIR)/$(PACKAGE_NAME)/$(PACKAGE_VERSION)
	rm -f $(DESTDIR)$(MODULEFILES_DIR)/$(PACKAGE_NAME)/.version
	rmdir $(DESTDIR)$(MODULEFILES_DIR)/$(PACKAGE_NAME)
endif

