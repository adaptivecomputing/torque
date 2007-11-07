# $Id: Makefile.am,v 1.13 2006/08/28 23:03:26 ciesnik Exp $
# 
#   DRMAA library for Torque/PBS
#   Copyright (C) 2006  Poznan Supercomputing and Networking Center
#                       DSP team <dsp-devel@hedera.man.poznan.pl>
#
#   This library is free software; you can redistribute it and/or
#   modify it under the terms of the GNU Lesser General Public
#   License as published by the Free Software Foundation; either
#   version 2.1 of the License, or (at your option) any later version.
#
#   This library is distributed in the hope that it will be useful,
#   but WITHOUT ANY WARRANTY; without even the implied warranty of
#   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
#   Lesser General Public License for more details.
#
#   You should have received a copy of the GNU Lesser General Public
#   License along with this library; if not, write to the Free Software
#   Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
#

SUBDIRS = src test

pkgdocdir=@drmaadocdir@

if DRMAA_DOCS
pdf_doc = drmaa.pdf
install-data-local: install-doc
endif
pkgdoc_DATA = $(pdf_doc)

uninstall-hook:
	rm -rf $(DESTDIR)$(pkgdocdir)


#install-data-hook:
#	for i in $(plaintext_doc); do gzip --force --best $(pkgdocdir)/$$i; done

install-doc: doc
	( cd doc; find -type f -exec $(INSTALL_DATA) -D '{}' '$(DESTDIR)$(pkgdocdir)/{}' \; )

doc: doc-timestamp
drmaa.pdf: doc-timestamp
doc-timestamp:
	@if [ ! -f $@ \
		-o `find $(srcdir)/src \( -name '*.c' -o -name '*.h' \) -a -newer $@ \
			| wc -l` -gt 0 \
		-o Doxyfile -nt $@ \
		]; then \
			rm -rf doc; \
			echo "$(DOXYGEN)"; \
			$(DOXYGEN) && touch $@ || exit 1; \
			if [ -d doc/latex ]; then \
				(cd doc/latex && $(MAKE) $(AM_MAKEFLAGS) ) || exit 1; \
				mv doc/latex/refman.pdf drmaa.pdf; \
				(cd doc/latex && $(MAKE) $(AM_MAKEFLAGS) clean ); \
			fi \
	fi

DISTCLEANFILES = doc-timestamp doc/html/* doc/latex/* doc/man/man*/* drmaa.pdf

maintainer-clean-local:
	-rm -rf doc

doc doc-timestamp: FORCE
FORCE:


