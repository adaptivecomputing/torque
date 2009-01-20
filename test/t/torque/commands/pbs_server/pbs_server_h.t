#!/usr/bin/perl

use strict;
use warnings;

use FindBin;
use lib "$FindBin::Bin../../../../../lib/";


use CRI::Test;

use Torque::Test::Utils qw(
                            run_and_check_cmd
                          );
use Torque::Ctrl        qw(
                            stopPbsserver
                            startPbsserver
                          );


plan('no_plan');
setDesc('pbs_server -h');

# Variables
my $host = $props->get_property('MoabHost');

# StopPbsserver
stopPbsserver();

TODO:
  {

  local $TODO = 'pbs_server -h not implemented yet';

  my $cmd = "pbs_server -h $host";
  my %pbs_server = runCommand($cmd);

  ok($pbs_server{'STDERR'} !~ /Usage:/, 'Checking for the usage message'); 

  }; # END TODO:
