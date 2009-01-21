#!/usr/bin/perl

use strict;
use warnings;

use FindBin;
use lib "$FindBin::Bin/../../../../lib/";

use CRI::Test;

plan('no_plan');
setDesc("RELEASE qrerun Compatibility Tests");

my $testbase = $FindBin::Bin;

execute_tests("$testbase/setup.t") 
  or die("Couldn't setup for qrerun tests!");

execute_tests(
              "$testbase/qrerun.t",
              "$testbase/qrerun_f.t",
);

execute_tests("$testbase/cleanup.t"); 
