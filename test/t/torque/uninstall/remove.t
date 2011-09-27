#!/usr/bin/perl
use strict;
use warnings;

use TestLibFinder;
use lib test_lib_loc();

use CRI::Test;
plan('no_plan');
setDesc('Remove Torque Directories');

my $torque_home = $props->get_property('Torque.Home.Dir');

my $ec;
my $endtime = time + 10;

do
{
  $ec = runCommand("rm -rf $torque_home");

}until( $ec == 0) || time > $endtime );

ok(!-d $torque_home, "Removing $torque_home")
  or die("Torque install directory still exists but shouldn't");
