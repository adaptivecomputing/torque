include $(top_srcdir)/buildutils/config.mk

xpbsmondir = $(XPBSMON_DIR)
xpbsmonbitmapdir = $(xpbsmondir)/bitmaps
xpbsmonhelpdir = $(xpbsmondir)/help

SUFFIXES = .src

bin_SCRIPTS = xpbsmon
nodist_xpbsmon_DATA = xpbsmonrc tclIndex

CLEANFILES = xpbsmon xpbsmonrc tclIndex buildindex
EXTRA_DIST = xpbsmon.src xpbsmonrc.src patch_scripts buildindex.src

dist_xpbsmon_DATA = preferences.tcl auto_upd.tk bindings.tk box.tk button.tk \
		    cluster.tk color.tk common.tk dialog.tk entry.tk expr.tk \
		    listbox.tk main.tk node.tk pbs.tk prefsave.tk pref.tk \
		    spinbox.tk system.tk wmgr.tk

dist_xpbsmonbitmap_DATA = bitmaps/cyclist-only.xbm bitmaps/hourglass.bmp \
			  bitmaps/iconize.bmp bitmaps/logo.bmp \
			  bitmaps/maximize.bmp

dist_xpbsmonhelp_DATA = help/auto_update.hlp help/main.hlp help/notes.hlp \
			help/pref.hlp help/prefQuery.hlp help/prefServer.hlp

xpbsmon: $(srcdir)/xpbsmon.src
	sed -e 's,#\!.*pbs_wish.*$$,#\!$(bindir)/$(program_prefix)pbs_wish$(program_suffix) -f,' \
	    -e 's,set  *libdir.*$$,set libdir $(xpbsmondir),' \
	    -e 's,set  *appdefdir.*$$,set appdefdir $(xpbsmondir),' \
	    $? > $@

xpbsmonrc: $(srcdir)/xpbsmonrc.src
	$(SHELL) $(srcdir)/patch_scripts "$(PBS_MACH)" "$(PBS_DEFAULT_SERVER)" "$?" "$@"

buildindex: $(srcdir)/buildindex.src
	@echo Processing buildindex
	@sed -e 's,#\!.*tclsh.*$$,#\!@TCLSH_PROG@,' $? > $@
	@chmod +x $@

tclIndex: $(dist_xpbs_DATA) buildindex
	@echo Running buildindex \(locally\)
	@./buildindex $(srcdir) .

install-data-hook:
	$(INSTALL_SCRIPT) buildindex $(DESTDIR)$(XPBSMON_DIR)/buildindex
	rm -f $(DESTDIR)$(XPBSMON_DIR)/tclIndex
	cd $(DESTDIR)$(XPBSMON_DIR) && ./buildindex $(DESTDIR)$(XPBSMON_DIR) . $(DESTDIR)
	chmod 644 tclIndex

uninstall-hook:
	rm -f $(DESTDIR)$(XPBS_DIR)/buildindex

