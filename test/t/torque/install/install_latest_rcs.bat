#!/usr/bin/perl 

use CRI::Test;
plan('no_plan');
setDesc('Install Latest Torque from Source');
use strict;
use warnings;

use FindBin;
use lib "$FindBin::Bin../../../../lib/";


my $testbase = $props->get_property('test.base') . "torque/install"; 

my @testList = (
    "$testbase/get_latest_rcs.t",
    "$testbase/configure.t",
    "$testbase/make_clean.t",
    "$testbase/make.t",
    "$testbase/make_install.t",
    "$testbase/setup.t",
    "$testbase/config_mom.t",
    "$testbase/config_server.t",
    "$testbase/startup.t",
);

foreach (@testList)
{
    execute_tests($_)
	or die "Torque Install failed on $_";
}
