#!/usr/bin/perl
use strict;
use warnings;

use FindBin;
use TestLibFinder;
use lib test_lib_loc();

use CRI::Test;
use Torque::Ctrl;
plan('no_plan');
setDesc('pbs_server -p');

my $port = '3334';
my $lsof_cmd = "lsof -i :$port";

stopTorque();
startPbsserver({ 'args' => "-p $port" });

# Verify that pbs_server is listening on the given port
my %lsof = runCommand($lsof_cmd, 'test_success' => 1);

like($lsof{STDOUT}, qr/^pbs_serve /m, "PBS_Server is Listening on Port $port");
