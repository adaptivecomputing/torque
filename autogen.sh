#!//usr/bin/env bash
# autogen.sh
set -e

mkdir -p m4
libtoolize -c -f
aclocal
autoconf -f
autoheader -f
automake -a -c -f
