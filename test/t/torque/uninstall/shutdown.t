#!/usr/bin/perl
use strict;
use warnings;

use TestLibFinder;
use lib test_lib_loc();
 
use CRI::Test;
use Torque::Ctrl;
plan('no_plan');
setDesc('Shutdown Torque');

stopTorque();
stopTorque({host => $_}) foreach split ',', $props->get_property('Torque.Remote.Nodes');
