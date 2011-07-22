#!/usr/bin/perl
use strict;
use warnings;

use TestLibFinder;
use lib test_lib_loc();

use CRI::Test;
plan('no_plan');
setDesc('Remove Torque');

my $build_dir  = $props->get_property('torque.build.dir');
$build_dir =~ s%/$%%;    # Remove trailing /

# Remove Torque install directory
runCommand("rm -rf " . $props->get_property('Torque.Home.Dir'));
ok(!-d $props->get_property('Torque.Home.Dir'),"Removing " . $props->get_property('Torque.Home.Dir'))
    or die("Torque install directory still exists but shouldn't");

# Remove Torque build directory
runCommand("rm -rf $build_dir");
ok(!-d $build_dir, "Removing  $build_dir")
    or die("Torque build directory still exists but shouldn't");
