#!/usr/bin/perl 
use strict;
use warnings;

use FindBin;
use TestLibFinder;
use lib test_lib_loc();

use CRI::Test;
plan('no_plan');
setDesc('Install TORQUE with BLCR checkpointing');

my $testbase = $FindBin::Bin;

my @testList = (
"$testbase/kill_moab.t",
"$testbase/autogen.t",
"$testbase/configure.t",
"$testbase/make_clean.t",
"$testbase/make.t",
"$testbase/make_install.t",
"$testbase/setup.t",
"$testbase/config_mom.t",
"$testbase/cp_chkpt_scripts.t",
"$testbase/remote_install_torque.t",
"$testbase/config_server.t",
"$testbase/create_torque_conf.t",
"$testbase/check_blcr.t",
"$testbase/startup.t",
);

foreach( @testList )
{
    execute_tests($_)
	or die "Torque installation Failed on $_";
}
