#!/usr/bin/perl

use strict;
use warnings;

use FindBin;
use lib "$FindBin::Bin/../../../../lib/";

use CRI::Test;

plan('no_plan');
setDesc("RELEASE qdel Compatibility Tests");

my $testbase = $FindBin::Bin;

execute_tests("$testbase/setup.t") 
  or die("Couldn't setup for qdel tests!");

execute_tests(
              "$testbase/qdel.t",
              "$testbase/qdel_p.t",
              "$testbase/qdel_m.t",
              "$testbase/qdel_cW.t",
);

execute_tests("$testbase/cleanup.t"); 
