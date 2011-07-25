#!/usr/bin/perl

use strict;
use warnings;

use FindBin;
use TestLibFinder;
use lib test_lib_loc();


use CRI::Test;

plan('no_plan');
setDesc("FULL Torque Regression Tests");

my $testbase = $FindBin::Bin;

execute_tests(
    "$testbase/reinstall.bat",
) or die("Torque reinstall test failed!");

execute_tests(
    "$testbase/commands/full.bat",
    "$testbase/ha/full.bat",
);
