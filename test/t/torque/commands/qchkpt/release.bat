#!/usr/bin/perl

use CRI::Test;

plan('no_plan');
use strict;
use warnings;

use FindBin;
use lib "$FindBin::Bin../../../../../lib/";


my $testbase=$props->get_property('test.base') . "torque/commands/qchkpt";
setDesc("RELEASE qchckpt Compatibility Tests");

execute_tests("$testbase/setup.t") 
  or die("Couldn't setup for qchkpt tests!");

execute_tests(
              "$testbase/qchkpt.t",
#              "$testbase/mult_node_qchkpt.t",
);

execute_tests("$testbase/cleanup.t"); 
