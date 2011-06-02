noinst_SCRIPTS = checkpoint_script restart_script

CLEANFILES = checkpoint_script restart_script
EXTRA_DIST = checkpoint_script.in restart_script.in

edit_blcr_scripts = sed \
    -e "s|@BLCR_BINDIR[@]|$(BLCR_BINDIR)|g" \
    -e "s|@bindir[@]|$(bindir)|g" \
    -e "s|@PBS_SERVER_HOME[@]|$(PBS_SERVER_HOME)|g"

checkpoint_script restart_script: Makefile
	rm -f $@ $@.tmp
	my_srcdir=''; \
	  test -f ./$@.in || my_srcdir=$(srcdir)/; \
	  $(edit_blcr_scripts) $${my_srcdir}$@.in >$@.tmp
	chmod +x $@.tmp
	chmod a-w $@.tmp
	mv $@.tmp $@

checkpoint_script: $(srcdir)/checkpoint_script.in
restart_script: $(srcdir)/restart_script.in
