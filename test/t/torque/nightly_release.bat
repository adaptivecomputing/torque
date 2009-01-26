#!/usr/bin/perl

use strict;
use warnings;

use FindBin;
use lib "$FindBin::Bin/../../lib/";

use CRI::Test;

plan('no_plan');
setDesc("RELEASE Torque Regression Tests (NIGHTLY)");

my $testbase = $FindBin::Bin;


execute_tests(
    "$testbase/nightly_reinstall.bat",
) or die("Torque reinstall test failed!");

execute_tests(
    "$testbase/commands/release.bat",
#    "$testbase/ha/release.bat",
);
