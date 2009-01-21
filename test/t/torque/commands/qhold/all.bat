#!/usr/bin/perl

use strict;
use warnings;

use FindBin;
use lib "$FindBin::Bin/../../../../lib/";

use CRI::Test;
plan('no_plan');
setDesc("ALL qhold Tests");

my $testbase = $FindBin::Bin;

execute_tests("$testbase/setup.t") 
  or die("Couldn't setup for qhold tests!");

execute_tests(
              "$testbase/checkpoint/all.bat",
              "$testbase/non-checkpoint/all.bat",
);

execute_tests("$testbase/cleanup.t"); 
