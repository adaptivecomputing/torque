#!/usr/bin/perl -w

use strict;
use warnings;
use Test::More tests => 1;
sub warning ($) { diag "WARNING: $_[0]" };

##
#
# This set of tests is for providing warnings to the users. It should not be
# used for checking for actual errors. Error checking should be done in an
# actual test file. This is the final set of checks that occurs when testing
# the TORQUE installation.
#
##

# Default Queue & Queue Defaults
my $qmgr = `qmgr -c 'p s'`;
if (defined($qmgr) and $qmgr)
  {

  # - Default Queue
  warning('No default_queue set - see Section 4.1.3')
    unless ($qmgr =~ /^set server default_queue = \S+/mi);

  # - Queue Defaults
  my $queue = undef;
  my %queues;
  # - - Parse Queue Information
  foreach my $line (split /[\r\n]+/, $qmgr)
    {
    $queue = $1 if $line =~ /^create queue (\S+)/;
    next unless $line =~ /^set queue batch (\S+) = (\S+)/;
    $queues{$queue}{$1} = $2 if defined $queue;
    }
  # - - Check Queue Information
  foreach $queue (sort keys %queues)
    {
    warning("No default nodes for queue '$queue' set - see Section 4.1.2")
      unless exists $queues{$queue}{'resources_default.nodes'};
    warning("No default walltime for queue '$queue' set - see Section 4.1.2.")
      unless exists $queues{$queue}{'resources_default.walltime'};
    }

  }

ok($qmgr,'Warnings Check') or
  BAIL_OUT('Cannot communicate with TORQUE through qmgr');

