#!/usr/bin/perl

use strict;
use warnings;

use FindBin;
use lib "$FindBin::Bin../../../../../lib/";


use CRI::Test;

use Torque::Test::Utils qw(
                            run_and_check_cmd
                            query_mom_cfg
                          );
use Torque::Ctrl        qw(
                            stopPbsserver
                            startPbsserver
                          );


plan('no_plan');
setDesc('pbs_server -S');

# Variables
my $port = '3334';

# Commands
my $pgrep_cmd   = 'pgrep -x pbs_server';
my $pbs_server_cmd = "pbs_server -S $port";
my $lsof_cmd    = "lsof -i | grep $port | grep pbs_server";

# Hashes
my %pgrep;
my %pbs_server;
my %lsof;

# Make sure pbs_server is stopped
stopPbsserver();

# Start pbs_server
%pbs_server = runCommand($pbs_server_cmd);
ok($pbs_server{ 'EXIT_CODE' } == 0, "Checking exit code of '$pbs_server_cmd'")
  or diag("EXIT_CODE: $pbs_server{ 'EXIT_CODE' }\nSTDERR: $pbs_server{ 'STDERR' }");

diag("Verify that pbs_server correctly started");

# Make sure that pbs_server has started
%pgrep = runCommand($pgrep_cmd);
ok($pgrep{ 'EXIT_CODE' } == 0, "Verifying that pbs_server is running");

# Check for any error messages
ok($pbs_server{ 'STDERR' } eq '', "Checking for an error message")
  or diag("STDERR: '$pbs_server{ 'STDERR' }'");

# Verify that pbs_server can communicate with the schedular

# TODO:  In these tests suites we are not doing anything with moab or any other
#        schedular.  We may need to do a test in a moab-torque compatibility tests

# Restart pbs_server to it's default state
diag("Restarting pbs_server");
startPbsserver();
