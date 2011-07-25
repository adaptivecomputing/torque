#!/usr/bin/perl

use strict;
use warnings;

use FindBin;
use TestLibFinder;
use lib test_lib_loc();


use CRI::Test;

plan('no_plan');
setDesc("RELEASE Torque Regression Tests");

my $testbase = $FindBin::Bin;

execute_tests(
    "$testbase/torque/release.bat",
);
