#! /usr/bin/perl 

use strict;
use warnings;

use FindBin;
use lib "$FindBin::Bin/../../../lib/";

use CRI::Test;

plan('no_plan'); 
setDesc('Make Packages');

my $torque_build_dir = $props->get_property('torque.build.dir');

# Extract build directory from test properties
ok(-d $torque_build_dir,       "Checking if torque build directory '$torque_build_dir' exists") 
  or die("Torque build directory doesn't exist");

# Run make packages
my $cmd  = "cd $torque_build_dir; make packages";
my %make = runCommand($cmd);
ok($make{ 'EXIT_CODE' } == 0,  "Checking exit code of '$cmd'")
  or die "'make packages' unsuccessful: $make{ 'STDERR' }";

