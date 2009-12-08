#!/usr/bin/perl
use strict;
use warnings;

use FindBin;
use lib "$FindBin::Bin/../../../../lib/";

use CRI::Test;
use Torque::Ctrl;
plan('no_plan');
setDesc('pbs_mom -M');

my $port     = '3334';
my $lsof_cmd = "lsof -i | grep $port | grep pbs_mom";

stopPbsmom();
startPbsmom({ 'args' => "-M $port" });

# Verify that pbs_mom is listening on the given port
my %lsof = runCommand($lsof_cmd, 'test_success' => 1);
ok($lsof{ 'STDOUT' } ne '', "PBS_Mom is Listening on Port '$port'");
