#!/usr/bin/perl 

use strict;
use warnings;

use FindBin;
use lib "$FindBin::Bin/../../../../lib/";


use CRI::Test;

plan('no_plan');
setDesc('Uninstall Torque');

my $testbase = $props->get_property('test.base') . "/torque/uninstall";

execute_tests(
              "$testbase/shutdown.t",
              "$testbase/remove.t"
             );
