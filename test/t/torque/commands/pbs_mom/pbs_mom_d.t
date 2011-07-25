#!/usr/bin/perl
use strict;
use warnings;

use FindBin;
use TestLibFinder;
use lib test_lib_loc();

use CRI::Test;
use Torque::Ctrl;
plan('no_plan');
setDesc('pbs_mom -d');

my $directory   = $props->get_property("Torque.Home.Dir");

stopPbsmom();
startPbsmom({ 'args' => "-d $directory" });

#diag("Restarting pbs_mom");
#startPbsmom();
