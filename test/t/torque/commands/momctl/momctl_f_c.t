#!/usr/bin/perl

use strict;
use warnings;

use FindBin;
use lib "$FindBin::Bin../../../../../lib/";


# Test Modules
use CRI::Test;

# Test Description
plan('no_plan');
setDesc('Momctl -f <HOSTLIST_FILE> -c <JOBID>');

# Variables
my %momctl;
my $hostlistfile = $props->get_property('mom.host.list.file');
my @hosts        = split(/,|\s/, `cat $hostlistfile`);

# Create some stale jobs
my $job_id = 'all'; # stubed out

%momctl = runCommand("momctl -f $hostlistfile -c $job_id");
ok($momctl{ 'EXIT_CODE' } == 0,
   "Checking that 'momctl -f $hostlistfile -c $job_id' ran")
  or die "Couldn't run momctl -f $hostlistfile -c $job_id";

# Check the stdout
my $stdout = $momctl{ 'STDOUT' };
foreach my $host (@hosts)
  {

  ok($stdout =~ /job clear request successful on ${host}/i, "Checking output of 'momctl -f $hostlistfile -c $job_id' for $host");

  }

