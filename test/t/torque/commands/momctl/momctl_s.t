#!/usr/bin/perl

use strict;
use warnings;

use FindBin;
use TestLibFinder;
use lib test_lib_loc();


# Test Modules
use CRI::Test;
use Torque::Ctrl;

# Test Description
plan('no_plan');
setDesc('Momctl -s');

# Variables
my %momctl;
my $host          = 'localhost';
my $reconfig_file = $props->get_property( 'mom.reconfig.file' );
my $cmd           = "momctl -s";

%momctl = runCommand($cmd, test_success => 1);

# Perform the tests
my $stdout = $momctl{ 'STDOUT' };
like($stdout, /shutdown request successful on ${host}/i, "Checking output of '$cmd'");

sleep_diag 10;
my $pgrep = `pgrep -x pbs_mom`;
ok($pgrep !~ /\d+/, "Checking that the pbs_mom pid does not exists");

# Restart pbs
startPbsmom();
