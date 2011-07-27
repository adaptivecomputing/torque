#!/usr/bin/perl
use strict;
use warnings;

use TestLibFinder;
use lib test_lib_loc();
 
use CRI::Test;
use FindBin;
plan('no_plan');
setDesc('Shutdown and Remove Torque');

my $testbase = "$FindBin::Bin";

my @testlist = (
  "$testbase/shutdown.t",
  "$testbase/remove.t"
);

foreach(@testlist)
{
  execute_tests($_)
    or die 'Uninstall failed!';
}
