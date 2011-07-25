#!/usr/bin/perl

use strict;
use warnings;

use FindBin;
use TestLibFinder;
use lib test_lib_loc();


use CRI::Test;

plan('no_plan');
setDesc("SNAP Torque HA Regression Tests");

my $testbase = $FindBin::Bin;


execute_tests(
#    "$testbase/fail_over.bat",
);
