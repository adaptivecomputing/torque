noinst_SCRIPTS = pbs_server pbs_mom pbs_sched trqauthd \
  debian.pbs_server debian.pbs_mom debian.pbs_sched debian.trqauthd \
  suse.pbs_server suse.pbs_mom suse.pbs_sched suse.trqauthd

CLEANFILES = pbs_server pbs_mom pbs_sched trqauthd \
  debian.pbs_server debian.pbs_mom debian.pbs_sched debian.trqauthd \
  suse.pbs_server suse.pbs_mom suse.pbs_sched suse.trqauthd
EXTRA_DIST = pbs_server.in pbs_mom.in pbs_sched.in trqauthd.in \
  debian.pbs_server.in debian.pbs_mom.in debian.pbs_sched.in debian.trqauthd.in \
  suse.pbs_server.in suse.pbs_mom.in suse.pbs_sched.in suse.trqauthd.in

edit_init_scripts = sed \
    -e "s|@sbindir[@]|$(sbindir)|g" \
    -e "s|@bindir[@]|$(bindir)|g" \
    -e "s|@PBS_HOME[@]|$(PBS_SERVER_HOME)|g"

pbs_server pbs_mom pbs_sched trqauthd debian.pbs_server debian.pbs_mom debian.pbs_sched  debian.trqauthd suse.pbs_server suse.pbs_mom suse.pbs_sched suse.trqauthd: Makefile
	rm -f $@ $@.tmp
	my_srcdir=''; \
	  test -f ./$@.in || my_srcdir=$(srcdir)/; \
	  $(edit_init_scripts) $${my_srcdir}$@.in >$@.tmp
	chmod +x $@.tmp
	chmod a-w $@.tmp
	mv $@.tmp $@

pbs_server: $(srcdir)/pbs_server.in
pbs_mom: $(srcdir)/pbs_mom.in
pbs_server: $(srcdir)/pbs_server.in
trqauthd: $(srcdir)/trqauthd.in
debian.pbs_sched: $(srcdir)/debian.pbs_sched.in
debian.pbs_mom: $(srcdir)/debian.pbs_mom.in
debian.pbs_sched: $(srcdir)/debian.pbs_sched.in
debian.trqauthd: $(srcdir)/debian.trqauthd.in
suse.pbs_sched: $(srcdir)/suse.pbs_sched.in
suse.pbs_mom: $(srcdir)/suse.pbs_mom.in
suse.pbs_sched: $(srcdir)/suse.pbs_sched.in
suse.trqauthd: $(srcdir)/suse.trqauthd.in
