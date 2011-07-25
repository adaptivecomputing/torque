#!/usr/bin/perl

use strict;
use warnings;

use FindBin;
use TestLibFinder;
use lib test_lib_loc();


use CRI::Test;

use Torque::Ctrl        qw(
                            stopPbsserver
                          );


plan('no_plan');
setDesc('pbs_server');

# Commands
my $pgrep_cmd   = 'pgrep -x pbs_server';
my $pbs_server_cmd = "pbs_server";

# Hashes
my %pgrep;
my %pbs_server;

# Make sure pbs_server is stopped
stopPbsserver();

# Start pbs_server
%pbs_server = runCommand($pbs_server_cmd);
ok($pbs_server{ 'EXIT_CODE' } == 0, "Checking exit code of '$pbs_server_cmd'")
  or diag("EXIT_CODE: $pbs_server{ 'EXIT_CODE' }\nSTDERR: $pbs_server{ 'STDERR' }");

# Make sure that pbs_server has started
%pgrep = runCommand($pgrep_cmd);
ok($pgrep{ 'EXIT_CODE' } == 0, "Verifying that pbs_server is running");
