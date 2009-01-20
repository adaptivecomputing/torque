#!/usr/bin/perl

use CRI::Test;

plan('no_plan');
use strict;
use warnings;

use FindBin;
use lib "$FindBin::Bin/../../../../lib/";


my $testbase=$props->get_property('test.base') . "torque/commands/qhold";
setDesc("RELEASE qhold Compatibility Tests");

execute_tests("$testbase/setup.t") 
  or die("Couldn't setup for qhold tests!");

execute_tests(
              "$testbase/checkpoint/release.bat",
              "$testbase/non-checkpoint/release.bat",
);

execute_tests("$testbase/cleanup.t"); 
