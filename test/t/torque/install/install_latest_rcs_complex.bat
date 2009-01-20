#!/usr/bin/perl 
use strict;
use warnings;

use FindBin;
use lib "$FindBin::Bin../../../../lib/";

use CRI::Test;
plan('no_plan');
setDesc('Install Latest Torque from subversion for Moab Complex Test Case');

my $testbase = $props->get_property('test.base') . "torque/install"; 

execute_tests(
"$testbase/get_latest_rcs.t",
"$testbase/configure.t",
"$testbase/make_clean.t",
"$testbase/make.t",
"$testbase/make_install.t",
"$testbase/setup_complex.t",
"$testbase/config_mom.t",
"$testbase/config_server.t",
"$testbase/startup.t",
);
