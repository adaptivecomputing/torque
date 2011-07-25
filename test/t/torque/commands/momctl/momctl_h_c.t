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
setDesc('Momctl -h <HOST> -c <JOBID>');

# Variables
my %momctl;
my $host = $props->get_property('Test.Host');

# Create some stale jobs
my $job_id = 'all'; # stubed out

%momctl = runCommand("momctl -h $host -c $job_id");
ok($momctl{ 'EXIT_CODE' } == 0,
   "Checking that 'momctl -c $job_id' ran")
  or die "Couldn't run momctl -c $job_id";

my $stdout = $momctl{ 'STDOUT' };

ok($stdout =~ /job clear request successful on ${host}/i, "Checking output of momctl -c $job_id");

