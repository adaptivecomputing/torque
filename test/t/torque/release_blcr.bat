#!/usr/bin/perl

use strict;
use warnings;

use FindBin;
use TestLibFinder;
use lib test_lib_loc();


use CRI::Test;

plan('no_plan');
setDesc("Release Torque Regression Tests");

my $testbase = $FindBin::Bin;

execute_tests(
    "$testbase/reinstall_blcr.bat",
) or die("Torque reinstall test failed!");

execute_tests(
    "$testbase/commands/release.bat",
#    "$testbase/ha/release.bat",
);
