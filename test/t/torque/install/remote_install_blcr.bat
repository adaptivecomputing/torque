#!/usr/bin/perl 

use strict;
use warnings;

use FindBin;
use TestLibFinder;
use lib test_lib_loc();

use CRI::Test;
plan('no_plan');
setDesc('Remote Install Torque with BLCR');

my $testbase = $FindBin::Bin;

execute_tests(
"$testbase/autogen.t",
"$testbase/configure.t",
"$testbase/make_clean.t",
"$testbase/make.t",
"$testbase/make_install.t",
"$testbase/setup.t",
"$testbase/config_mom.t",
"$testbase/cp_chkpt_scripts.t",
"$testbase/create_torque_conf.t",
"$testbase/check_blcr.t",
);
