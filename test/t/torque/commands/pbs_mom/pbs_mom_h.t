#!/usr/bin/perl

use strict;
use warnings;

use FindBin;
use lib "$FindBin::Bin/../../../../lib/";


use CRI::Test;

use Torque::Test::Utils qw(
                            run_and_check_cmd
                          );
use Torque::Ctrl        qw(
                            stopPbsmom
                            startPbsmom
                          );


plan('no_plan');
setDesc('pbs_mom -h');

# Variables
my $host = $props->get_property('Test.Host');

# StopPbsmom
stopPbsmom();

TODO:
  {

  local $TODO = 'pbs_mom -h not implemented yet';

  my $cmd = "pbs_mom -h $host";
  my %pbs_mom = runCommand($cmd);

  ok($pbs_mom{'STDERR'} !~ /Usage:/, 'Checking for the usage message'); 

  }; # END TODO:
