include $(top_srcdir)/buildutils/config.mk

SUBDIRS = src doc

EXTRA_DIST = CHANGELOG INSTALL.GNU README.configure README.mpiexec \
             README.torque Release_Notes torque.setup torque.spec.in \
             DEVELOPMENT PBS_License.txt torque.spec \
	     buildutils/tcl.m4 buildutils/self-extract-head-sh.in \
	     buildutils/pbs_mkdirs.in buildutils/pbs_mach_type \
	     buildutils/config.mk buildutils/ac_create_generic_config.m4 \
	     contrib/README.pestat contrib/README.pbstop \
	     contrib/README.qpeek contrib/pestat-1.0/pestat.c \
	     contrib/pestat-1.0/README contrib/pestat-1.0/Makefile \
	     contrib/README.pbs_ganglia contrib/pam_authuser.tar.gz \
	     contrib/README.pam_authuser contrib/pbstop \
	     contrib/README.pbs_python contrib/README.pbs_weblite \
	     contrib/qpeek contrib/init.d/pbs_mom \
	     contrib/init.d/pbs_server contrib/init.d/pbs_sched

bin_SCRIPTS = pbs-config

DISTCLEANFILES = pbs-config

DISTCHECK_CONFIGURE_FLAGS=--with-server-home=$$dc_install_base/spool

snap:
	$(MAKE) VERSION=$(VERSION)-snap.$${snapstamp:-`date +%Y%m%d%H%M`} dist

dist-hook:
	chmod u+w $(distdir)/README.torque $(distdir)/configure $(distdir)/torque.spec
	sed -e 's/TORQUE .* README .*/TORQUE $(VERSION) README (released '"`date '+%b, %d %Y'`"')/' < $(srcdir)/README.torque > $(distdir)/README.torque
	sed -e 's/VERSION=.*/VERSION=$(VERSION)/' < $(srcdir)/configure > $(distdir)/configure
	spec_name=torque; \
	spec_version=`echo $(VERSION) | sed 's/-snap.*//'`; \
	spec_snap=""; \
	if test "$$spec_version" != "$(VERSION)" ;then \
  	  spec_snap="%define snap `echo $(VERSION) | sed 's/.*-snap.//'`"; \
	fi; \
	sed -e "s/@spec_""name@/$$spec_name/" -e "s/@spec_""version@/$$spec_version/" \
	    -e "s/@spec_""snap@/$$spec_snap/" < $(srcdir)/torque.spec.in > $(distdir)/torque.spec

srpm: dist
	rpmbuild -ts $(distdir).tar.gz

if USING_TCL
tclrpmflag=--with tcl
else
tclrpmflag=--without tcl
endif
if INCLUDE_GUI
guirpmflag=--with gui
else
guirpmflag=--without gui
endif
if USING_FILESYNC
fsyncrpmflag=--with filesync
else
fsyncrpmflag=--without filesync
endif
if USING_SYSLOG
syslogrpmflag=--with syslog
else
syslogrpmflag=--without syslog
endif
if USING_RPP
rpprpmflag=--with rpp
else
rpprpmflag=--without rpp
endif
if INCLUDE_MOM_RCP
scprpmflag=
else
scprpmflag=--with scp
endif

RPMCFLAGS=@CFLAGS@
RPMLDFLAGS=@LDFLAGS@

rpm: dist
	RPMCFLAGS="$(RPMCFLAGS)" RPMLDFLAGS="$(RPMLDFLAGS)" rpmbuild -tb $(distdir).tar.gz \
	  $(tclrpmflag) $(guirpmflag) $(fsyncrpmflag) $(syslogrpmflag) $(rpprpmflag) $(scprpmflag) \
	  --with server_name=$(PBS_DEFAULT_SERVER) \
	  --with homedir=$(PBS_SERVER_HOME) \
	  --with libdir=$(libdir) \
	  --with includedir=$(includedir) \
	  --with prefix=$(prefix)

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

ALL_PKGS = $(SERVER_PKG) $(MOM_PKG) $(CLIENTS_PKG) $(GUI) devel doc

install_server install_mom install_clients install_gui install_devel:
	(cd src && $(MAKE) $@)
install_doc:
	(cd doc && $(MAKE) $@)

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
           tarball=$(PKGROOT)/torque-$$pname-$$d-$$phost.tar.gz; \
           pkgfile=./torque-$$pname-$$d-$$phost.sh; \
           echo "Building $$pkgfile ..."; \
           $(MAKE) DESTDIR=$$proot install_$$d >/dev/null || exit 1; \
           if test -d $(PKGOVERRIDE)/$$d ;then \
             echo "Using override dir $(PKGOVERRIDE)/$$d"; \
             ( cd $(PKGOVERRIDE)/$$d && tar cf - . | (cd $$proot; tar xvf - ) ); \
           fi; \
           ( cd $$proot; tar cf - * ) | gzip > $$tarball && \
           ( cd $$proot; tar cf - \
                           `test -d .$(sbindir) && echo .$(sbindir)/*` \
                           `test -d .$(bindir) && echo .$(bindir)/*` \
                           `test -d .$(libdir) && echo .$(libdir)/*` \
                           `test -d .$(mandir) && echo .$(mandir)/*/*` \
                           `test -d .$(includedir) && echo .$(includedir)/*` \
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

