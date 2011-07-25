#!/usr/bin/perl

use strict;
use warnings;

use FindBin;
use TestLibFinder;
use lib test_lib_loc();


use CRI::Test;

use Torque::Ctrl        qw(
                            stopPbsmom
                          );


plan('no_plan');
setDesc('pbs_mom -a');

# Commands
my $pgrep_cmd   = 'pgrep -x pbs_mom';
my $pbs_mom_cmd = "pbs_mom -a 6";

# Hashes
my %pgrep;
my %pbs_mom;

# Make sure pbs_mom is stopped
stopPbsmom();

# Start pbs_mom
%pbs_mom = runCommand($pbs_mom_cmd);
ok($pbs_mom{ 'EXIT_CODE' } == 0, "Checking exit code of '$pbs_mom_cmd'")
  or diag("EXIT_CODE: $pbs_mom{ 'EXIT_CODE' }\nSTDERR: $pbs_mom{ 'STDERR' }");

# Make sure that pbs_mom has started
%pgrep = runCommand($pgrep_cmd);
ok($pgrep{ 'EXIT_CODE' } == 0, "Verifying that pbs_mom is running");
