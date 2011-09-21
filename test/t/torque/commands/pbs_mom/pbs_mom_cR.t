#!/usr/bin/perl

use strict;
use warnings;

use FindBin;
use TestLibFinder;
use lib test_lib_loc();


use CRI::Test;

use Torque::Util qw(
                            query_mom_cfg
                          );
use Torque::Ctrl        qw(
                            stopPbsmom
                            startPbsmom
                          );


plan('no_plan');
setDesc('pbs_mom -R');

# Variables
my $port = '3334';

# Commands
my $pgrep_cmd   = 'pgrep -x pbs_mom';
my $pbs_mom_cmd = "pbs_mom -R $port";
my $lsof_cmd    = "lsof -i :$port | grep pbs_mom";

# Hashes
my %pgrep;
my %pbs_mom;
my %lsof;

# Make sure pbs_mom is stopped
stopPbsmom();

# Start pbs_mom
%pbs_mom = runCommand($pbs_mom_cmd, test_success => 1);

diag("Verify that pbs_mom correctly started");

# Make sure that pbs_mom has started
%pgrep = runCommand($pgrep_cmd, test_success => 1, msg => 'Verifying that pbs_mom is running');

# Check for any error messages
ok($pbs_mom{ 'STDERR' } eq '', "Checking for an error message")
  or diag("STDERR: '$pbs_mom{ 'STDERR' }'");

# Verify that pbs_mom is listening on the given port
%lsof = runCommand($lsof_cmd, test_success => 1);
ok($lsof{ 'STDOUT' } ne '', "Checking if pbs_mom is listening on port '$port'");

# Restart pbs_mom to it's default state
diag("Restarting pbs_mom");
stopPbsmom({flags => "-p $port"});
startPbsmom();
