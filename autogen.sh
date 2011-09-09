#!/bin/bash
# autogen.sh
# 

libtoolize -c -f && aclocal && autoconf -f && autoheader -f && automake -a -c -f

