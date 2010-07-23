#!/bin/bash
# autogen.sh
# 

libtoolize -c -f && aclocal && autoconf && autoheader && automake -a -c

