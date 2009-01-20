#!/usr/bin/perl 
use strict;
use warnings;

use FindBin;
use lib "$FindBin::Bin../../../../lib/";

use CRI::Test;
plan('no_plan');
setDesc('Install Torque on Primary and Fallback Servers for Moab HA');

my $testbase 		= $props->get_property('test.base') . "torque/install"; 
my $server_name_file 	= $props->get_property('torque.home.dir')."/server_name";
my $hostname 		= $props->get_property('MoabHost');
my $fallback 		= $props->get_property('fallback.hostname');

my @testList = (
    "$testbase/get_latest_rcs.t",
    "$testbase/configure.t",
    "$testbase/make_clean.t",
    "$testbase/make.t",
    "$testbase/make_install.t",
);

foreach my $test (@testList)
{
    execute_tests($test)
	or die "Torque Setup Failed on Primary Server!";
}

foreach my $test (@testList)
{
    runCommandSsh($fallback, $test)
	and die "Torque Setup Failed on Fallback Server!";
}

runCommandSsh($fallback, "echo $hostname > $server_name_file")
    and die "Unable to set server_name file on Fallback Server: $!";

my @testList = (
    "$testbase/setup.t",
    "$testbase/config_mom.t",
    "$testbase/config_server.t",
    "$testbase/startup_ha.t",
    "$testbase/qmgr_access_moabha.t",
);

foreach my $test (@testList)
{
    execute_tests($test)
	or die "Torque Setup Failed on Primary Server!";
}
