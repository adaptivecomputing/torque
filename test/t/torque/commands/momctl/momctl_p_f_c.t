#!/usr/bin/perl

use strict;
use warnings;

use FindBin;
use TestLibFinder;
use lib test_lib_loc();


# Test Modules
use CRI::Test;

# Test Description
plan('no_plan');
setDesc('Momctl -p <PORT> -f <HOSTLIST_FILE> -c <JOBID>');

# Variables
my %momctl;
my $hostlistfile = $props->get_property('mom.host.list.file');
my $port         = $props->get_property('mom.host.port');
my @hosts        = split(/,|\s/, `cat $hostlistfile`);

# Create some stale jobs
my $job_id = 'all'; # stubed out

%momctl = runCommand("momctl -p $port -f $hostlistfile -c $job_id", test_success => 1);

# Check the stdout
my $stdout = $momctl{ 'STDOUT' };
foreach my $host (@hosts)
  {

  like($stdout, qr/job clear request successful on $host/i, "Checking output of 'momctl -p $port -f $hostlistfile -c $job_id' for $host");

  }

