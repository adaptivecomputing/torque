#!/usr/bin/perl

use strict;
use warnings;

use CRI::Test;

plan('no_plan');
setDesc("RELEASE Torque Regression Tests");

my $testbase = $props->get_property('test.base') . "";

execute_tests(
    "$testbase/torque/reinstall.bat",
) or die("Torque reinstall test failed!");

execute_tests(
    "$testbase/torque/release.bat",
);
