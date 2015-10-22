#!/bin/bash
# Simple script to impersonate sendmail but
# write to a file in /tmp instead of actually
# mailing the message

MAILFILE=./mail.out
MAILFROM=$2
shift 2
MAILTO="$@"

echo "MAILFROM=$MAILFROM" > $MAILFILE
echo "MAILTO=$MAILTO" >> $MAILFILE
echo >> $MAILFILE
cat >> $MAILFILE
