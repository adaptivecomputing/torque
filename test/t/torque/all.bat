#!/usr/bin/perl

use strict;
use warnings;

use FindBin;
use lib "$FindBin::RealBin/../../lib/";


use CRI::Test;

plan('no_plan');
setDesc("ALL Torque Regression Tests");

my $testbase = $FindBin::RealBin;

execute_tests(
    "$testbase/reinstall.bat",
) or die("Torque reinstall test failed!");

execute_tests(
    "$testbase/commands/all.bat",
    "$testbase/ha/all.bat",
);
