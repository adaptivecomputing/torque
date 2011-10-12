#!/usr/bin/perl
use strict;
use warnings;

use FindBin;
use TestLibFinder;
use lib test_lib_loc();

use CRI::Test;
use Torque::Ctrl;
plan('no_plan');
setDesc('pbs_mom -M');

my $port      = '3334';
my @exp_ports = ( $port, 15003 );
my $cmd = "netstat -nap | grep pbs_mom | grep LISTEN";

stopPbsmom();
startPbsmom({ 'args' => "-M $port" });

# Verify that pbs_mom is listening on the given port
my %cmd = runCommand($cmd, 'test_success' => 1);
my @ports = ($cmd{STDOUT} =~ /^tcp\s+\d+\s+\d+\s+\S+:(\d+)/gm);

is_deeply(\@ports, \@exp_ports, 'Torque MOM is listening on expected ports');
