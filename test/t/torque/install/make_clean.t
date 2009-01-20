#! /usr/bin/perl 
use strict;
use warnings;

use FindBin;
use lib "$FindBin::Bin/../../../lib/";

use CRI::Test;
use CRI::Utils qw(
                   run_and_check_cmd
                 );

plan('no_plan');
setDesc('Make Clean on Torque');

my $build_dir = "$FindBin::Bin/../../../../";
my $cmd       = "make clean";

ok(-d $build_dir, "Checking if the torque build dir '$build_dir' exists")
    or die "The Torque build dir '$build_dir' doesn't exist";
ok(chdir $build_dir, "Changing to the Torque build dir '$build_dir'")
    or die("Couldn't change to Torque build dir '$build_dir'"); 

# Run make clean
run_and_check_cmd($cmd);
