#! /usr/bin/perl 
#? apitest

use strict;
use warnings;

use FindBin;
use lib "$FindBin::Bin/../../../lib/";

use CRI::Test;

plan('no_plan'); 
setDesc('Make torque');

# Extract build directory from test properties
ok(-d $props->get_property('torque.build.dir'),"Checking if torque build directory exists") or die("Torque build dir doesn't exist");

# Change directory to build dir
ok(chdir $props->get_property('torque.build.dir'),"Changing directory to " . $props->get_property('torque.build.dir')) or die("Can't change to torque build dir");

# Run make 
runCommand('make') && die("Making torque failed");
