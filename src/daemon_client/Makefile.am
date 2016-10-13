include $(top_srcdir)/buildutils/config.mk

noinst_HEADERS = trq_auth_daemon.h 
DIST_SUBDIRS =
CLEANFILES = *.gcda *.gcno *.gcov

AM_CFLAGS = -DPBS_SERVER_HOME=\"$(PBS_SERVER_HOME)\" -Wall -pthread -ldl -lrt -lssl -lcrypto

sbin_PROGRAMS = trqauthd

trqauthd_SOURCES = trq_auth_daemon.c trq_main.c
trqauthd_LDFLAGS = -L../lib/Libpbs/.libs -ltorque

install-exec-hook:
	systemctl 2>/dev/null | grep -- -\.mount >/dev/null 2>&1 && \
	  echo "$(install_sh) -m 644 $(top_srcdir)/contrib/systemd/trqauthd.service $(DESTDIR)/usr/lib/systemd/system/trqauthd.service" && \
	  $(install_sh) -m 644 $(top_srcdir)/contrib/systemd/trqauthd.service $(DESTDIR)/usr/lib/systemd/system/trqauthd.service || \
	  (echo "$(install_sh) -m 755 $(top_srcdir)/contrib/init.d/trqauthd $(DESTDIR)/etc/init.d/trqauthd" && \
	  $(install_sh) -m 755 $(top_srcdir)/contrib/init.d/trqauthd $(DESTDIR)/etc/init.d/trqauthd)
	-$(install_sh) -d -m 755 $(DESTDIR)/etc/ld.so.conf.d
	test -f $(DESTDIR)/etc/ld.so.conf.d/torque.conf || \
	  echo $(libdir) > $(DESTDIR)/etc/ld.so.conf.d/torque.conf && chmod 644 $(DESTDIR)/etc/ld.so.conf.d/torque.conf && \
	  echo $(DESTDIR) | egrep -q tpackages\|rpmbuild || /sbin/ldconfig

uninstall-hook:
	-rm -f $(DESTDIR)/usr/lib/systemd/system/trqauthd.service || \
	  rm -f $(DESTDIR)/etc/init.d/trqauthd
	-rm -f $(DESTDIR)/etc/ld.so.conf.d/torque.conf
	echo $(DESTDIR) | egrep -q tpackages\|rpmbuild || /sbin/ldconfig

