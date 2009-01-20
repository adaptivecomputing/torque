#!/usr/bin/perl

use strict;
use warnings;

use FindBin;
use lib "$FindBin::Bin/../../../../../lib/";


# Test Modules
use CRI::Test;
use Torque::Test::Utils qw( run_and_check_cmd );

# Test Description
plan('no_plan');
setDesc('pbsnodes -p');

# Variables
my $cmd      = "pbsnodes -p";
my %pbsnodes;

TODO:
  {
  
  local $TODO =<<TODO;
  pbsnodes - not yet implemented.  Will need to be implemented for
    pbsnodes -p
    pbsnodes -s -p
    pbsnodes -p -q
    pbsnodes -s -p -q
TODO

  %pbsnodes   = run_and_check_cmd($cmd);

  ok($pbsnodes{ 'STDOUT' } =~ /.+/, "Checking the stdout of '$cmd' for anything");

  };

