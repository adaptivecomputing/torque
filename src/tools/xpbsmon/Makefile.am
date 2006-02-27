include $(top_srcdir)/buildutils/config.mk

xpbsmondir = $(XPBSMON_DIR)
xpbsmonbitmapdir = $(xpbsmondir)/bitmaps
xpbsmonhelpdir = $(xpbsmondir)/help

SUFFIXES = .src

bin_SCRIPTS = xpbsmon
nodist_xpbsmon_DATA = xpbsmonrc tclIndex

CLEANFILES = xpbsmon xpbsmonrc tclIndex
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
	sed -e 's,#\!.*pbs_wish.*$$,#\!$(bindir)/pbs_wish -f,' \
	    -e 's,set  *libdir.*$$,set libdir $(xpbsmondir),' \
	    -e 's,set  *appdefdir.*$$,set appdefdir $(xpbsmondir),' \
	    $? > $@

xpbsmonrc: $(srcdir)/xpbsmonrc.src
	$(SHELL) $(srcdir)/patch_scripts "$(PBS_MACH)" "$(PBS_DEFAULT_SERVER)" "$?" "$@"

tclIndex: $(dist_xpbsmon_DATA)
	echo "auto_mkindex . $(srcdir)/*.{tcl,tk};exit" | @TCLSH_PROG@

