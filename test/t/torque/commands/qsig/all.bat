#!/usr/bin/perl

use strict;
use warnings;

use FindBin;
use lib "$FindBin::Bin/../../../../lib/";

use CRI::Test;
plan('no_plan');
setDesc("ALL qsig tests");

my $testbase = $FindBin::Bin;

execute_tests("$testbase/setup.t")
 or die("Could not set up for qsig tests");

execute_tests(
              "$testbase/qsig.t",
              "$testbase/qsig_s.t",
);

execute_tests("$testbase/cleanup.t");
