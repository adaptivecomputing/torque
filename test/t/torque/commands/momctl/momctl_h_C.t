#!/usr/bin/perl

use strict;
use warnings;

use FindBin;
use lib "$FindBin::Bin/../../../../lib/";


# Test Modules
use CRI::Test;

# Test Description
plan('no_plan');
setDesc('Momctl -h <HOST> -C');

# Variables
my %momctl;
my $host = $props->get_property('Test.Host');

# Create some stale jobs
my $job_id = 'all'; # stubed out

%momctl = runCommand("momctl -h $host -C");
ok($momctl{ 'EXIT_CODE' } == 0,
   "Checking that 'momctl -h $host -C' ran")
  or die "Couldn't run momctl -h $host -C";

my $stdout = $momctl{ 'STDOUT' };

ok($stdout =~ /mom ${host} successfully cycled cycle forced/i, "Checking output of momctl -h <HOST> -C");

