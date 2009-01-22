#!/usr/bin/perl

use strict;
use warnings;

use FindBin;
use lib "$FindBin::Bin/../../../../lib/";


# Test Module
use CRI::Test;

# Test Case Description
plan('no_plan');
setDesc('Momctl -p <PORT> -h <HOST> -c <JOBID>');

# Variables
my %momctl;
my $host = $props->get_property('Test.Host');
my $port = $props->get_property('mom.host.port');

# Create some stale jobs
my $job_id = 'all'; # stubed out

# Run the test
%momctl = runCommand("momctl -p $port -h $host -c $job_id");
ok($momctl{ 'EXIT_CODE' } == 0,
   "Checking that 'momctl -p $port -h $host -c $job_id' ran")
  or die "Couldn't run momctl -p $port -h $host -c $job_id";

my $stdout = $momctl{ 'STDOUT' };

ok($stdout =~ /job clear request successful on ${host}/i, "Checking output of momctl -p $port -h $host -c $job_id");

