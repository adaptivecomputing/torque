noinst_SCRIPTS = pbs_server.service pbs_mom.service pbs_sched.service trqauthd.service

CLEANFILES = pbs_server.service pbs_mom.service pbs_sched.service trqauthd.service
EXTRA_DIST = pbs_server.service.in pbs_mom.service.in pbs_sched.service.in trqauthd.service.in

edit_systemd_scripts = sed \
    -e "s|@sbindir[@]|$(sbindir)|g" \
    -e "s|@bindir[@]|$(bindir)|g" \
    -e "s|@PBS_HOME[@]|$(PBS_SERVER_HOME)|g"

pbs_server.service pbs_mom.service pbs_sched.service trqauthd.service: Makefile
	rm -f $@ $@.tmp
	my_srcdir=''; \
	  test -f ./$@.in || my_srcdir=$(srcdir)/; \
	  $(edit_systemd_scripts) $${my_srcdir}$@.in >$@.tmp
	chmod a-w $@.tmp
	mv $@.tmp $@

pbs_server.service: $(srcdir)/pbs_server.service.in
pbs_mom.service: $(srcdir)/pbs_mom.service.in
trqauthd.service: $(srcdir)/trqauthd.service.in
pbs_sched.service: $(srcdir)/pbs_sched.service.in
