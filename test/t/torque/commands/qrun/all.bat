#!/usr/bin/perl

use strict;
use warnings;

use FindBin;
use lib "$FindBin::Bin/../../../../lib/";

use CRI::Test;

plan('no_plan');
setDesc("ALL qrun tests");

my $testbase = $FindBin::Bin;

execute_tests("$testbase/setup.t")
  or die("Could not set up for qrun tests");

execute_tests(
              "$testbase/qrun.t",
              "$testbase/qrun_cH.t",
);

execute_tests("$testbase/cleanup.t");
