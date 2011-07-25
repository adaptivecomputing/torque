#!/usr/bin/perl 
use strict;
use warnings;

use FindBin;
use TestLibFinder;
use lib test_lib_loc();

use CRI::Test;
plan('no_plan');
setDesc("Reinstall Torque with BLCR checkpointing");

my $testbase = "$FindBin::Bin/..";

my @testList = (
    "$testbase/blcr/reinstall.bat",
    "$testbase/torque/uninstall/uninstall.bat",
    "$testbase/torque/install/install_blcr.bat",
);

foreach( @testList )
{
    execute_tests($_)
	or die "Installation failed due to $_";
}
