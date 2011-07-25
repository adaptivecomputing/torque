#!/usr/bin/perl

use strict;
use warnings;

use FindBin;
use TestLibFinder;
use lib test_lib_loc();


use CRI::Test;

use Torque::Util qw(
                            run_and_check_cmd
                          );
use Torque::Ctrl        qw(
                            stopPbsserver
                            startPbsserver
                          );


plan('no_plan');
setDesc('pbs_server -h');

# Variables
my $host = $props->get_property('Test.Host');

# StopPbsserver
stopPbsserver();

TODO:
  {

  local $TODO = 'pbs_server -h not implemented yet';

  my $cmd = "pbs_server -h $host";
  my %pbs_server = runCommand($cmd);

  ok($pbs_server{'STDERR'} !~ /Usage:/, 'Checking for the usage message'); 

  }; # END TODO:
