#!/usr/bin/perl

use strict;
use warnings;

use FindBin;
use lib "$FindBin::Bin/../../../../../lib/";


# Test Modules
use CRI::Test;

# Test Description
plan('no_plan');
setDesc('Momctl -C');

# Variables
my %momctl;
my $host = 'localhost';

# Create some stale jobs
my $job_id = 'all'; # stubed out

%momctl = runCommand("momctl -C");
ok($momctl{ 'EXIT_CODE' } == 0,
   "Checking that 'momctl -C' ran")
  or die "Couldn't run momctl -C";

my $stdout = $momctl{ 'STDOUT' };

ok($stdout =~ /mom ${host} successfully cycled cycle forced/i, "Checking output of momctl -C");

