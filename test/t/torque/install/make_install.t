#! /usr/bin/perl 

use CRI::Test;
plan('no_plan'); 
setDesc('Make Install');
use strict;
use warnings;

# Extract build directory from test properties
ok(-d $props->get_property('torque.build.dir'),"Checking if torque build directory exists") or die("Torque build directory doesn't exist");

# Change directory to build dir
ok(chdir $props->get_property('torque.build.dir'),"Changing directory to " . $props->get_property('torque.build.dir')) or die("Couldn't change directory to torque build directory");

# Run configure
runCommand('make install') &&  die("Couldn't run make install");

