#! /usr/bin/perl 
#? apitest

use strict;
use warnings;

use FindBin;
use lib "$FindBin::Bin/../../../lib/";

use CRI::Test;

plan('no_plan'); 
setDesc('make install Torque');

my $build_dir = "$FindBin::Bin/../../../../";
my $cmd       = "make install";

# Extract build directory from test properties
ok(-d $build_dir, "Checking if the torque build directory '$build_dir' exists") 
  or die("Torque build dir '$build_dir' doesn't exist");

# Change directory to build dir
ok(chdir $build_dir, "Changing directory to $build_dir") 
  or die("Can't change to torque build dir to $build_dir");

# Run make 
my %cmd_result = runCommand($cmd);
cmp_ok($cmd_result{ 'EXIT_CODE' }, '==', 0, "Checking exit code of '$cmd'")
  or die("'$cmd' failed: $cmd_result{ 'STDERR' }");
