#! /usr/bin/perl 
use strict;
use warnings;

use FindBin;
use lib "$FindBin::Bin../../../../lib/";

use CRI::Test;
plan('no_plan');
setDesc('Uninstall Torque from Primary and Fallback Servers for Moab HA');

my $testbase = $props->get_property('test.base') . "torque/uninstall";
my $fallback = $props->get_property('fallback.hostname');

my @testList = (
    "$testbase/shutdown.t",
    "$testbase/remove.t",
);

foreach my $test (@testList)
{
    execute_tests($test)
	or die "Torque uninstall failed on Primary server due to $test";
}

foreach my $test (@testList)
{
    runCommandSsh($fallback, $test)
	and die "Torque uninstall failed on Fallback server due to $test";
}
