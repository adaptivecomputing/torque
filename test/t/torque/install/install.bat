#!/usr/bin/perl
use strict;
use warnings;

use FindBin;
use TestLibFinder;
use lib test_lib_loc();

use CRI::Test;
plan('no_plan');
setDesc('Install Torque from Source');

my $testbase = "$FindBin::Bin";

my @testList = (
    "$testbase/configure.t",
    "$testbase/make_clean.t",
    "$testbase/make.t",
    "$testbase/make_install.t",
    "$testbase/config_mom.t",
    "$testbase/remote_install_torque.t",
    "$testbase/startup.t",
);

foreach (@testList)
{
    execute_tests($_)
	or die "Torque Install failed on $_";
}
