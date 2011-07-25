#!/usr/bin/perl

use strict;
use warnings;

use FindBin;
use TestLibFinder;
use lib test_lib_loc();


# Test Modules
use CRI::Test;
use Torque::Util qw( run_and_check_cmd );

# Test Description
plan('no_plan');
setDesc('pbsnodes -d');

# Variables
my $cmd      = "pbsnodes -d";
my %pbsnodes;

TODO:
  {
  
  local $TODO =<<TODO;
  pbsnodes -d not yet implemented.  Will need to be implementd for
  pbsnodes -d
  pbsnodes -s -d
  pbsnodes -d -q
  pbsnodes -s -d -q
TODO

  %pbsnodes   = run_and_check_cmd($cmd);

  ok($pbsnodes{ 'STDOUT' } =~ /.+/, "Checking the stdout of '$cmd' for anything");

  };

