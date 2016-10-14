include $(top_srcdir)/buildutils/config.mk

noinst_HEADERS = trq_auth_daemon.h 
DIST_SUBDIRS =
CLEANFILES = *.gcda *.gcno *.gcov

AM_CFLAGS = -DPBS_SERVER_HOME=\"$(PBS_SERVER_HOME)\" -Wall -pthread -ldl -lrt -lssl -lcrypto

sbin_PROGRAMS = trqauthd

trqauthd_SOURCES = trq_auth_daemon.c trq_main.c
trqauthd_LDFLAGS = -L../lib/Libpbs/.libs -ltorque

install-exec-hook:
	if [ $(FROM_TORQUE_BUILD)0 -eq 0 ]; then \
	  if systemctl 2>/dev/null | grep -- -\.mount >/dev/null 2>&1; then \
	    if [ -f $(top_srcdir)/contrib/systemd/trqauthd.service ]; then \
	      echo "$(install_sh) -m 644 $(top_srcdir)/contrib/systemd/trqauthd.service $(DESTDIR)/usr/lib/systemd/system/trqauthd.service"; \
	      $(install_sh) -m 644 $(top_srcdir)/contrib/systemd/trqauthd.service $(DESTDIR)/usr/lib/systemd/system/trqauthd.service; \
	    fi \
	  elif [ -f $(top_srcdir)/contrib/init.d/trqauthd ]; then \
	    echo "$(install_sh) -m 755 $(top_srcdir)/contrib/init.d/trqauthd $(DESTDIR)/etc/init.d/trqauthd"; \
	    $(install_sh) -m 755 $(top_srcdir)/contrib/init.d/trqauthd $(DESTDIR)/etc/init.d/trqauthd; \
	  fi; \
	  $(install_sh) -d -m 755 $(DESTDIR)/etc/ld.so.conf.d || :; \
	  if [ ! -f $(DESTDIR)/etc/ld.so.conf.d/torque.conf ]; then \
	    echo $(libdir) > $(DESTDIR)/etc/ld.so.conf.d/torque.conf; \
	    chmod 644 $(DESTDIR)/etc/ld.so.conf.d/torque.conf; \
	    if [ $(FROM_MAKE_PACKAGES)0 -eq 0 ]; then \
	      /sbin/ldconfig; \
	    fi; \
	  fi; \
	fi

uninstall-hook:
	if [ $(FROM_TORQUE_BUILD)0 -eq 0 ]; then \
	  rm -f $(DESTDIR)/usr/lib/systemd/system/trqauthd.service || :; \
	  rm -f $(DESTDIR)/etc/init.d/trqauthd || :; \
	  rm -f $(DESTDIR)/etc/ld.so.conf.d/torque.conf || :; \
	  if $(FROM_MAKE_PACKAGES)0 -eq 0; then \
	    /sbin/ldconfig; \
	  fi; \
	fi

