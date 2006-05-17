include $(top_srcdir)/buildutils/config.mk

SUBDIRS = Ccode

xpbsdir = $(XPBS_DIR)
xpbsbitmapdir = $(xpbsdir)/bitmaps
xpbshelpdir = $(xpbsdir)/help

SUFFIXES = .src

bin_SCRIPTS = xpbs
nodist_xpbs_DATA = xpbsrc tclIndex

CLEANFILES = xpbs xpbsrc tclIndex buildindex
EXTRA_DIST = xpbs.src xpbsrc.src buildindex.src

dist_xpbs_DATA = pbs.tcl preferences.tcl acctname.tk after_depend.tk \
		 auto_upd.tk before_depend.tk bindings.tk box.tk button.tk \
		 checkpoint.tk common.tk concur.tk datetime.tk email_list.tk \
		 entry.tk fileselect.tk hold.tk jobname.tk listbox.tk main.tk \
		 misc.tk owners.tk prefsave.tk pref.tk priority.tk qalter.tk \
		 qdel.tk qhold.tk qmove.tk qmsg.tk qrls.tk qsig.tk qsub.tk \
		 qterm.tk qtime.tk rerun.tk res.tk spinbox.tk staging.tk \
		 state.tk text.tk trackjob.tk wmgr.tk

dist_xpbsbitmap_DATA = bitmaps/curve_down_arrow.bmp \
		       bitmaps/curve_up_arrow.bmp bitmaps/cyclist-only.xbm \
		       bitmaps/Downarrow.bmp bitmaps/hourglass.bmp \
		       bitmaps/iconize.bmp bitmaps/logo.bmp \
		       bitmaps/maximize.bmp bitmaps/sm_down_arrow.bmp \
		       bitmaps/sm_up_arrow.bmp bitmaps/Uparrow.bmp

dist_xpbshelp_DATA = help/after_depend.hlp help/auto_update.hlp \
		     help/before_depend.hlp help/concur.hlp help/datetime.hlp \
		     help/delete.hlp help/email.hlp help/fileselect.hlp \
		     help/hold.hlp help/main.hlp help/message.hlp \
		     help/misc.hlp help/modify.hlp help/move.hlp \
		     help/notes.hlp help/preferences.hlp help/release.hlp \
		     help/select.acctname.hlp help/select.checkpoint.hlp \
		     help/select.hold.hlp help/select.jobname.hlp \
		     help/select.owners.hlp help/select.priority.hlp \
		     help/select.qtime.hlp help/select.rerun.hlp \
		     help/select.resources.hlp help/select.states.hlp \
		     help/signal.hlp help/staging.hlp help/submit.hlp \
		     help/terminate.hlp help/trackjob.hlp

install_gui: install

xpbs: $(srcdir)/xpbs.src
	sed -e 's,#\!.*wish.*$$,#\!@WISH_PROG@,;' \
	    -e 's,set  *libdir.*$$,set libdir $(xpbsdir),' \
	    -e 's,set  *appdefdir.*$$,set appdefdir $(xpbsdir),' \
	    -e 's,set  *xpbs_datadump.*$$,set xpbs_datadump $(program_prefix)xpbs_datadump$(program_suffix),' \
	    -e 's,set  *xpbs_scriptload.*$$,set xpbs_scriptload $(program_prefix)xpbs_scriptload$(program_suffix),' \
	    -e 's,RSH_PATH,$(RSH_PATH),' \
	    $? > $@

xpbsrc: $(srcdir)/xpbsrc.src
	sed -e 's,\*serverHosts:.*$$,\*serverHosts: $(PBS_DEFAULT_SERVER),;' \
	    -e ' s,\*selectHosts:.*$$,\*selectHosts: $(PBS_DEFAULT_SERVER),' \
	    $? > $@

buildindex: $(srcdir)/buildindex.src
	@echo Processing buildindex
	@sed -e 's,#\!.*tclsh.*$$,#\!@TCLSH_PROG@,' $? > $@
	@chmod +x $@

tclIndex: $(dist_xpbs_DATA) buildindex
	@echo Running buildindex \(locally\)
	@./buildindex $(srcdir) .

install-data-hook:
	$(INSTALL_SCRIPT) buildindex $(DESTDIR)$(XPBS_DIR)/buildindex
	rm -f $(DESTDIR)$(XPBS_DIR)/tclIndex
	cd $(DESTDIR)$(XPBS_DIR) && ./buildindex $(DESTDIR)$(XPBS_DIR) . $(DESTDIR)
	chmod 644 $(DESTDIR)$(XPBS_DIR)/tclIndex


uninstall-hook:
	rm -f $(DESTDIR)$(XPBS_DIR)/buildindex
