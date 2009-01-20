#!/usr/bin/perl

use strict;
use warnings;

use FindBin;
use lib "$FindBin::Bin../../../../../lib/";


# Test Modules
use CRI::Test;
use Torque::Ctrl;

# Test Description
plan('no_plan');
setDesc('Momctl -p <PORT> -h <HOST> -s');

# Variables
my %momctl;

my $host = $props->get_property('MoabHost');
my $port = $props->get_property('mom.host.port');
my $cmd  = "momctl -p $port -h $host -s";

%momctl = runCommand($cmd);
ok($momctl{ 'EXIT_CODE' } == 0,
   "Checking that '$cmd' ran")
  or die "Couldn't run '$cmd'";

# Perform the tests
my $stdout = $momctl{ 'STDOUT' };
ok($stdout =~ /shutdown request successful on ${host}/i, "Checking output of '$cmd'"                    );

sleep 15;
my $pgrep = `pgrep -x pbs_mom`;
ok($pgrep !~ /\d+/,                                      "Checking that the pbs_mom pid does not exists");

# Restart pbs
startPbsmom();
