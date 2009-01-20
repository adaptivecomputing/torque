#!/usr/bin/perl

use strict;
use warnings;

use FindBin;
use lib "$FindBin::Bin/../../../lib/";


use CRI::Test;

plan('no_plan');
setDesc("SNAP Torque Regression Tests");

my $testbase = $props->get_property('test.base') . "/torque";

execute_tests(
    "$testbase/torque/reinstall.bat",
) or die("Torque reinstall test failed!");

execute_tests(
    "$testbase/commands/snap.bat",
    "$testbase/ha/snap.bat",
);
