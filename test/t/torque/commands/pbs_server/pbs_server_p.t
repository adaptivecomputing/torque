#!/usr/bin/perl
use strict;
use warnings;

use FindBin;
use lib "$FindBin::Bin/../../../../lib/";

use CRI::Test;
use Torque::Ctrl;
plan('no_plan');
setDesc('pbs_server -p');

my $port = '3334';

my $lsof_cmd = "lsof -i | grep $port | grep 'pbs_serve'"; # NOTE: we look for pbs_serve instead of pbs_server 
                                                             #       because lsof limite the command name length
                                                             #       to 9.

stopPbsserver();
startPbsserver({ 'args' => "-p $port" });

# Verify that pbs_server is listening on the given port
my %lsof = runCommand($lsof_cmd, 'test_success' => 1);

cmp_ok($lsof{STDOUT},'ne','', "PBS_Server is Listening on Port $port");
