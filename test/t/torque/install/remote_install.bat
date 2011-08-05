#!/usr/bin/perl 

use strict;
use warnings;

use FindBin;
use TestLibFinder;
use lib test_lib_loc();

use CRI::Test;
plan('no_plan');
setDesc('Remote Install Torque');

my $testbase = $FindBin::Bin;

my @testList = (
"$testbase/autogen.t",
"$testbase/make_clean.t",
"$testbase/configure.t",
"$testbase/make.t",
"$testbase/make_install.t",
"$testbase/config_mom.t",
#"$testbase/create_torque_conf.t",
);

foreach(@testList)
{
  execute_tests($_)
    or die "Remote Installation failed at $_";
}
