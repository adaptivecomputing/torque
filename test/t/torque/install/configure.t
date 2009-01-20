#! /usr/bin/perl
#? apitest

use CRI::Test;
plan('no_plan'); 
setDesc('Configure Torque');
use strict;
use warnings;

use FindBin;
use lib "$FindBin::Bin../../../../lib/";


ok(-d ($props->get_property('torque.build.dir')),"Checking if torque build dir exists") or die("Torque build dir doesn't exist");

# Change directory to build dir
ok(chdir $props->get_property('torque.build.dir'),"Changing directory to " . $props->get_property('torque.build.dir')) or die("Couldn't change to torque build directory");

# Run configure
runCommand("./configure --prefix=" . $props->get_property('torque.home.dir') . " --with-server-home=" . $props->get_property('torque.home.dir') . " " . $props->get_property('torque.config.args') . " --with-debug --enable-syslog") && die("Torque configure failed!");
