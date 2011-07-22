#!/usr/bin/perl
use strict;
use warnings;

use FindBin;
use TestLibFinder;
use lib test_lib_loc();

use CRI::Test;
plan('no_plan');
setDesc('Install Latest Torque from Source');

my $testbase = "$FindBin::Bin";

my @testList = (
    "$testbase/get_latest_rcs.t",
    "$testbase/configure.t",
    "$testbase/make_clean.t",
    "$testbase/make.t",
    "$testbase/make_install.t",
    "$testbase/config_mom.t",
    "$testbase/startup.t",
);

foreach (@testList)
{
    execute_tests($_)
	or die "Torque Install failed on $_";
}
