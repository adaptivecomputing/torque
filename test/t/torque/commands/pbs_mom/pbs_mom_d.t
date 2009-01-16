#!/usr/bin/perl

use strict;
use warnings;

use CRI::Test;

use Torque::Test::Utils qw(
                            run_and_check_cmd
                            query_mom_cfg
                          );
use Torque::Ctrl        qw(
                            stopPbsmom
                            startPbsmom
                          );


plan('no_plan');
setDesc('pbs_mom -d');

# Variables
my $directory   = $props->get_property("torque.home.dir");

# Commands
my $pgrep_cmd   = 'pgrep -x pbs_mom';
my $pbs_mom_cmd = "pbs_mom -d $directory";

# Hashes
my %pgrep;
my %pbs_mom;

# Make sure pbs_mom is stopped
stopPbsmom();

# Start pbs_mom
%pbs_mom = runCommand($pbs_mom_cmd);
ok($pbs_mom{ 'EXIT_CODE' } == 0, "Checking exit code of '$pbs_mom_cmd'")
  or diag("EXIT_CODE: $pbs_mom{ 'EXIT_CODE' }\nSTDERR: $pbs_mom{ 'STDERR' }");

diag("Verify that pbs_mom correctly started");

# Make sure that pbs_mom has started
%pgrep = runCommand($pgrep_cmd);
ok($pgrep{ 'EXIT_CODE' } == 0, "Verifying that pbs_mom is running");

# Check for any error messages
ok($pbs_mom{ 'STDERR' } eq '', "Checking for an error message")
  or diag("STDERR: '$pbs_mom{ 'STDERR' }'");

# Restart pbs_mom to it's default state
diag("Restarting pbs_mom");
startPbsmom();
