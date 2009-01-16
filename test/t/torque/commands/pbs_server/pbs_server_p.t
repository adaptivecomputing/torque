#!/usr/bin/perl

use strict;
use warnings;

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
setDesc('pbs_server -p');

# Variables
my $port = '3334';

# Commands
my $pgrep_cmd   = 'pgrep -x pbs_server';
my $pbs_server_cmd = "pbs_server -p $port";
my $lsof_cmd    = "lsof -i | grep $port | grep 'pbs_serve'"; # NOTE: we look for pbs_serve instead of pbs_server 
                                                             #       because lsof limite the command name length
                                                             #       to 9.

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

# Verify that pbs_server is listening on the given port
%lsof = runCommand($lsof_cmd);
ok($lsof{ 'EXIT_CODE' } == 0, "Checking the exit code of '$lsof_cmd'")
  or diag("EXIT_CODE: '$lsof{ 'EXIT_CODE' }'");
ok($lsof{ 'STDOUT' } ne '', "Checking if pbs_server is listening on port '$port'");

# Restart pbs_server to it's default state
diag("Restarting pbs_server");
startPbsserver();
