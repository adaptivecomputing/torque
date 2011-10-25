#!/usr/bin/perl
use strict;
use warnings;

use FindBin;
use TestLibFinder;
use lib test_lib_loc();

use CRI::Test;
plan('no_plan');
setDesc("ALL Job Array Tests");

my $testbase = $FindBin::RealBin;

execute_tests(
    "$testbase/lifecycle.t",
);
