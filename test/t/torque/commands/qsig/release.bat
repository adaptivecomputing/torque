#!/usr/bin/perl

use CRI::Test;
plan('no_plan');

use strict;
use warnings;

use FindBin;
use lib "$FindBin::Bin/../../../../lib/";


my $testbase = $FindBin::Bin;

setDesc("RELEASE qsig Compatibility Tests");

execute_tests("$testbase/setup.t")
 or die("Could not set up qsig tests");

execute_tests(
              "$testbase/qsig.t",
#              "$testbase/qsig_s.t",
);

execute_tests("$testbase/cleanup.t");
