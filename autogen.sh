#!/bin/bash
# autogen.sh
set -e

mkdir -p m4
if [ -x "$(which glibtoolize)" ]; then
  glibtoolize -c -f
else
  libtoolize -c -f
fi
aclocal
autoconf -f
autoheader -f
automake -a -c -f
