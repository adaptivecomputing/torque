#!/usr/bin/perl 
use strict;
use warnings;

use FindBin;
use lib "$FindBin::Bin/../../lib/";

use CRI::Test;
plan('no_plan');
setDesc("Reinstall Latest SVN Torque to use BLCR checkpointing");

my $testbase = "$FindBin::Bin/..";

my @testList = (
    "$testbase/blcr/reinstall.bat",
    "$testbase/torque/uninstall/uninstall.bat",
    "$testbase/torque/install/install_latest_rcs.bat",
);

foreach( @testList )
{
    execute_tests($_)
	or die "Installation failed due to $_";
}
